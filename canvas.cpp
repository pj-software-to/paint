//
// Created by Patricia M Marukot on 2020-04-30.
//
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/clipbrd.h>
#include <wx/bitmap.h>
#include <wx/rawbmp.h>

#endif

#include <unordered_set>
#include <queue>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <math.h>

#include "helper.h"
#include "canvas.h"
#include "interpolation.h"
#include "selection.h"

#define LOC(x,y,w) (3*((y)*(w)+(x)))
#define ALPHA_LOC(x,y,w) ((y)*(w)+(x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

BEGIN_EVENT_TABLE( Canvas, wxPanel )
  EVT_KEY_DOWN(Canvas::keyDownEvent)
  EVT_KEY_UP(Canvas::keyUpEvent)
  EVT_PAINT(Canvas::paintEvent)
  EVT_MOTION(Canvas::mouseMoved)
  EVT_LEFT_DOWN(Canvas::mouseDown)
  EVT_LEFT_UP(Canvas::mouseReleased)
END_EVENT_TABLE()

Color WHITE = Color((char) 255, (char) 255, (char) 255);
Color SELECT = Color((char) 66, (char) 135, (char) 245);

/* CONSTRUCTORS */
Canvas::Canvas(wxFrame *parent) :
wxPanel(parent) {
  color = Color(0, 0, 0);
}

Canvas::Canvas(wxFrame *parent, unsigned int width, unsigned int height) :
wxPanel(parent) {
  this->width = width;
  this->height = height;
  this->SetFocus();

  color = Color(0, 0, 0);

  /* Initialize the buffer */
  size_t sz = 3*width*height*sizeof(char);
  Buffer = (char *)malloc(sz);

  /* White-out buffer */
  memset(Buffer, 255, sz);
}

void Canvas::addTransaction(Transaction &t) {
  transactions.push_back(t);
}

void Canvas::revertTransaction(Transaction &txn) {
  std::vector<Pixel> *pixels;
  Pixel p;
  pixels = &(txn.pixels);
  int i;
  for (i=0; i<pixels->size(); i++) {
    p = (*pixels)[i];
    updateBuffer(p); 
  }
}

void
Canvas::updateTransaction(Transaction &txn, const std::vector<wxPoint> &points)
{
  int i=0;
  wxPoint pt;
  Pixel p;
  for (i=0; i < points.size(); i++) {
    pt = points[i];
    p = Pixel(getPixelColor(pt), pt);
    txn.update(p);
  }
}

Color
Canvas::getPixelColor(const wxPoint &p) {
  /* Update buffer with new colors */
  if (p.x >= width || p.y >= height)
    return WHITE;

  int i = LOC(p.x, p.y, width);
  if (i >= 3 * width * height)
    return WHITE;

  return Color(
    Buffer[i],
    Buffer[i+1],
    Buffer[i+2]
  );
}

Pixel
Canvas::getPixel(const wxPoint &p) {
  wxPoint _p(p.x, p.y);
  return Pixel(getPixelColor(p), _p);
}

void
Canvas::getNeighbors(const wxPoint &p, wxPoint *neighbors, int &ncount) {
  int x, y;
  ncount = 0;

  // Up
  x = p.x;
  y = p.y - 1;
  if (x < width && y < height) {
    neighbors[ncount].x = x;
    neighbors[ncount].y = y;
    ncount++; 
  }

  // Down
  x = p.x;
  y = p.y + 1;
  if (x < width && y < height) {
    neighbors[ncount].x = x;
    neighbors[ncount].y = y;
    ncount++; 
  }

  // Left
  x = p.x - 1;
  y = p.y;
  if (x < width && y < height) {
    neighbors[ncount].x = x;
    neighbors[ncount].y = y;
    ncount++; 
  }

  // Right
  x = p.x + 1;
  y = p.y;
  if (x < width && y < height) {
    neighbors[ncount].x = x;
    neighbors[ncount].y = y;
    ncount++; 
  }
}

void Canvas::updateBuffer(const Pixel &p) {
  /* Update buffer with new colors */
  if (p.x >= width || p.y >= height)
    return;

  int i = LOC(p.x, p.y, width);
  if (i >= 3 * width * height || i < 0)
    return;

  Buffer[i] = p.color.r;
  Buffer[i+1] = p.color.g;
  Buffer[i+2] = p.color.b;
}

void Canvas::updateBuffer(const std::vector<wxPoint> &points,
                          const Color &color) {
  Pixel p;
  wxPoint point;
  int i;
  for (i=0; i<points.size(); i++) {
    point = points[i];
    p = Pixel(color.r, color.g, color.b, point.x, point.y);
    updateBuffer(p);
  }
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
void Canvas::paintEvent(wxPaintEvent & evt)
{
  wxPaintDC dc(this);
  render(dc);
}

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void Canvas::render(wxDC&  dc)
{
  /*
   * Draw out the bitmap
   */
  
  ////////////////////////////////////
  wxImage img(width, height, (unsigned char *)Buffer, true);
  wxBitmap bmp(img);  
  dc.DrawBitmap(bmp, 0, 0, false);
  ///////////////////////////////////
}

bool Canvas::pasteFromClip(Transaction &txn) {
  if (wxTheClipboard->Open()) {
    if (wxTheClipboard->IsSupported(wxDF_BITMAP)) {
      /*
       * Copy bitmap / image:
       * (1) Read and cast clipboard bitmap as wxImage
       * (2) Get wxImage's internal data buffer which contains
       *     RGBRGBRGB.. data format of pixels, row major. 
       * (3) Check corresponding alpha values for each pixel.
       *     If alpha value of pixel is NOT 0 (i.e. not completely
       *     transparent), then copy that pixel's RGB.
       *     If alpha value of pixel is 0, then ignore pixel, as
       *     is transparent.
       * (4) Iterate through, add previous color to transactions,
       *     then update display buffer.
       * (5) Initialize selectionArea and selectionBorder:
       *   - If previous selection exists, free it
       *   - Initialize selectionArea to be the non-alpha pixels
       *     pasted from clipboard.
       *   - For simplicity, set border to the bounding box
       *     (i.e. for Lasso, border would not be tightly 
       *     bounded like it is during the actual selection).
       *     This should not affect the actual pixels being
       *     moved if the user performs click-drag since
       *     selectionArea is still strictly based on the area
       *     selected by the user (and not, say, the bounding
       *     box)
       *  
       * Note: Could try and optimize using memset, this
       * requires changes to the transaction system.
       */
      clearSelection();

      wxImage bmpImage;
      wxBitmapDataObject data;

      wxTheClipboard->GetData(data);
      bmpImage = data.GetBitmap().ConvertToImage();

      unsigned char *buffer, *alpha;
      unsigned int N, M;
      bool hasAlpha;
      N = bmpImage.GetHeight();
      M = bmpImage.GetWidth();
      buffer = bmpImage.GetData();
      alpha = bmpImage.GetAlpha();
      hasAlpha = bmpImage.HasAlpha(); 

      printf("Paste bitmap: %dx%d. Alpha: 0x%p\n", N, M, 
        (void *)alpha);
      int x, y;
      for (y=0; y<std::min(height, N); y++) {
        for (x=0; x<std::min(width, M); x++) {
          wxPoint p(x,y);
          Pixel pixel;
          Color prev_c = getPixelColor(p); /* prev color */
          Color c;
          int ind;

          ind = ALPHA_LOC(x, y, M);
          if (!hasAlpha || alpha[ind] != wxIMAGE_ALPHA_TRANSPARENT) {
            ind = LOC(x, y, M);
            c = Color(
              buffer[ind],
              buffer[ind+1],
              buffer[ind+2]
            );

            pixel = Pixel(c, p);
            txn.update(Pixel(prev_c, p));

            updateBuffer(pixel);
            selectionArea.push_back(pixel);
          }
        }
      }

      wxPoint tl, br;
      tl = wxPoint(0, 0);
      br = wxPoint(std::min(width, M), std::min(height, N)); 
      selectionBorder = drawRectangle(tl, br, 1);
      selected = true;
    } 
    /* 
     * Add more options here:
     * e.g. wxDF_TEXT to process clipboard text input
     */

    // Always close the clipboard
    wxTheClipboard->Close();
  }
}
 void Canvas::cpySelectToClip() {
  /* 
   * Alpha channel issue:
   * https://forums.wxwidgets.org/viewtopic.php?t=46865&p=197052
   * http://trac.wxwidgets.org/ticket/16198
   * 
   * Note - Non-rectangular selection:
   * Use alpha channel to accept non-rectangular
   * selection areas.
   * e.g. Set alpha to 1 for transparent, so when
   * pasting, we can check alpha channel of 
   * the corresponding pixel to decide whether to
   * update it or not.
   */
  if (selected) {
    /*
     * Steps:
     * (1) Do one pass on data to set every pixel's
     *     alpha to 0 (i.e. transparent)
     * (2) Do one pass on selectionArea. Since all pixels
     *     contained in selectionArea have been selected,
     *     set color of the respective pixel in 'data' 
     *     and set the alpha of that pixel to 1.
     * (3) Copy data onto clipboard as bitmap data
     */
    assert(selection != NULL);

    int N, M;
    int minX, minY;
    unsigned char *data, *alpha;

    N = selection->getHeight();
    M = selection->getWidth();
    minX = (int)selection->minX;
    minY = (int)selection->minY;

    data = (unsigned char *)malloc(3*N*M);
    alpha = (unsigned char *)malloc(N*M);

    // (1)
    memset(data, 255, 3*N*M);
    memset(alpha, wxIMAGE_ALPHA_TRANSPARENT, N*M);

    // (2)
    {
      wxPoint p;
      Pixel pixel;
      int i;
      for (i=0; i<selectionArea.size(); i++) {
        pixel = selectionArea[i];
        p = wxPoint(pixel.x, pixel.y);
        
        int x, y;
        x = p.x - minX;
        y = p.y - minY;

        int ind;
        ind = LOC(x, y, M);
        data[ind] = pixel.color.r;
        data[ind+1] = pixel.color.g;
        data[ind+2] = pixel.color.b;         

        ind = ALPHA_LOC(x, y, M);
        alpha[ind] = wxIMAGE_ALPHA_OPAQUE;
      }
    }

    // (3)
    wxImage img(M, N, data, alpha, false);
    wxBitmap bmp(img, 4*8*sizeof(unsigned char));
    bmp.SetDepth(4*8*sizeof(unsigned char));
    if (wxTheClipboard->Open()) {
      wxTheClipboard->SetData(new wxBitmapDataObject(bmp));
      wxTheClipboard->Close();
      printf("Copied bitmap: %dx%d, hasAlpha=%d, depth=%d\n", N, M,
        img.HasAlpha(), bmp.GetDepth());;
    } else {
      free(alpha);
    }    
  }
}

void Canvas::keyDownEvent(wxKeyEvent &evt) {
  char uc = evt.GetUnicodeKey();
  if (evt.ControlDown()) {
    switch (uc) {
      case (KEY_Z):
        if (!isUndo && transactions.size() > 0) {
          isUndo = true;

          Transaction *latest;
          latest = &transactions.back(); 
          revertTransaction(*latest);
          transactions.pop_back();
        }
        break;
      case (KEY_C):
        if (!isCopy) {
          isCopy = true;
          cpySelectToClip();           
        }
        break;
      case (KEY_V):
        if (!isPaste) {
          isPaste = true;
          Transaction txn;
          if (pasteFromClip(txn)) {
            currentTxn = txn;
            addTransaction(currentTxn);
          }
        }
        break;
      default: 
        break;
    }
  }
  wxWindow::Refresh();
}

void Canvas::keyUpEvent(wxKeyEvent & evt) {
  char uc = evt.GetUnicodeKey();
  // Z is released - not checking for Ctrl here on purpose
  switch(uc) {
    case (KEY_Z):
      isUndo = false;
      break;
    case (KEY_C):
      isCopy = false;
      break;
    case (KEY_V):
      isPaste = false;
      break;
    default:
      break; 
  }
}

/* Event handlers to handle CANVAS mouse events */
/*
 * Handle CLICK event
 * Drawing Tools: Pencil, Line, DrawRect, DrawCircle
 *    - Fill in the clicked pixel - all have the same functionality
 */
void Canvas::mouseDown(wxMouseEvent &evt)
{
  this->SetFocus();
  
  /* Always should be left is down */
  assert(evt.LeftIsDown());

  isNewTxn = true;
  int x = evt.GetX();
  int y = evt.GetY();
  prevPos = wxPoint(x, y);
  startPos = wxPoint(x, y);

  Transaction txn;
  Pixel p(color.r, color.g, color.b, x, y);
  switch(toolType) {
    case Eraser:
    case Pencil:
      freehand.clear();
      freehand.push_back(wxPoint(x, y));
    case Line:
    case DrawRect:
    case DrawCircle:
      break;
    case Fill:
      fill(startPos, color, txn);
      currentTxn = txn;
      break;
    case SlctRect:
      handleSelectionClick(startPos);
      break;
    case SlctCircle:
      handleSelectionClick(startPos);
      break;
    case Lasso:
      freehand.clear();
      handleSelectionClick(startPos);
      break;
    default:
      break;
  }

  wxWindow::Refresh();
}

void Canvas::mouseMoved(wxMouseEvent &evt)
{
  if (!evt.LeftIsDown())
    return;

  wxPoint currPos = wxPoint(evt.GetX(), evt.GetY());
  Transaction txn;
  switch(toolType) {
    case Pencil:
      freehand.push_back(currPos);
      updateBuffer(
        drawFreeHand(currPos, txn, 5),
        color);
      currentTxn = txn;
      break;
    case Line:
      updateBuffer(
        drawLine(currPos, txn),
        color);
      currentTxn = txn;
      break;
    case DrawRect:
      updateBuffer(
          drawRectangle(currPos, txn, 3),
          color);
      currentTxn = txn;
      break;
    case DrawCircle:
      updateBuffer(
        drawCircle(currPos, txn, 5),
        color);
      currentTxn = txn;
      break;
    case Eraser:
      freehand.push_back(currPos);
      updateBuffer(
          drawFreeHand(currPos, txn, 5),
          WHITE);
      currentTxn = txn;
      break;
    case SlctRect:
      handleSelectionMove(currPos, &Canvas::drawRectangle);
      break;
    case SlctCircle:
      handleSelectionMove(currPos, &Canvas::drawCircle);
      break;
    case Lasso:
      freehand.push_back(currPos);
      handleSelectionMove(currPos, &Canvas::drawFreeHand);
      break;
    default:
      break;
  }

  wxWindow::Refresh();
  prevPos = wxPoint(currPos.x, currPos.y);
  isNewTxn = false;
}

void Canvas::mouseReleased(wxMouseEvent &evt)
{
  wxPoint pt = wxPoint(evt.GetX(), evt.GetY());
  switch (toolType) {
    case SlctRect:
    case SlctCircle:
    case Lasso:
      handleSelectionRelease(startPos, pt);
      wxWindow::Refresh();
      break;
    default:
      break;
  }

  // commit transaction
  addTransaction(currentTxn);
}

std::vector<wxPoint>
Canvas::drawFreeHand(const wxPoint &currPos, Transaction &txn, const int &_width)
{
  if (!isNewTxn) {
    revertTransaction(currentTxn);
  }

  std::vector<wxPoint> points, _pts;
  wxPoint p0, p1;
  int i;
  for (i=0; i<freehand.size()-1; i++) {
    p0 = freehand[i];
    p1 = freehand[i+1];

    _pts = lerp(p0, p1, _width);
    points.insert(points.end(), _pts.begin(), _pts.end());
  }

  updateTransaction(txn, points);
  return points;
}

std::vector<wxPoint>
Canvas::drawCircle(const wxPoint &currPos, Transaction &txn, const int &_width) {
  std::vector<wxPoint> points;
  /*
   * Steps:
   * (1) If not first transaction, delete previous transaction.
   *     We must do this since we're constantly redrawing
   *     the circle's trace as the user is "dragging" across
   *     the screen 
   * (2) Interpolate some points such that the interpolated
   *     points form a circle by using distance b/w currPos
   *     and startPos
   * (3) Further interpolate by calling either linear or
   *     polynomial interpolation on the sparse points to
   *     generate the pixels
   * (4) Write all previous buffer values to the txn
   */  

  // (1)
  if (!isNewTxn) {
    revertTransaction(currentTxn);
  }

  /*
   * Steps for (2):
   * (2a) Find diameter/radius
   * (2b) Find center point
   * (2c) Traverse around the circle by using center point
   */
  double radius = length(currPos, startPos)/2;
  wxPoint c;
  {
    wxRealVec v; 
    wxVec _v = currPos - startPos;
    v = wxRealVec((double)_v.x, (double)_v.y);
    v = normalize(v);

    wxRealPoint _curr(startPos);
    c = wxPoint(_curr + radius*v);
  }

  /*
   * Number of samples depends on the circumference
   * of the circle
   */ 
  std::vector<wxPoint> draft;
  {
    wxRealPoint p;
    wxRealVec _u; 
    double _x, _y;
    double _P = (2*M_PI) / (2*M_PI*radius);
    double theta;
    for (theta=0.0; theta<2*M_PI; theta+=_P) {
      _x = cos(theta);
      _y = sin(theta);
      _u = normalize(wxRealVec(_x,_y)); // should be normal, but..

      p = wxRealPoint(c) + radius*_u;
      draft.push_back(wxPoint(p));
    }
  }

  // (3)
  {
    std::vector<wxPoint> pts;
    wxPoint p0, p1; 
    int i;
    for (i=0; i<draft.size()-1; i++) {
      p0 = draft[i];
      p1 = draft[i+1];

      pts = lerp(p0, p1, _width);
      points.insert(points.end(), pts.begin(), pts.end());
    } 
  }

  // (4)
  updateTransaction(txn, points);
  return points;
}

std::vector<wxPoint> 
Canvas::drawLine(const wxPoint &currPos, Transaction &txn) {
  std::vector<wxPoint> points; 
  /*
   * Steps:
   * (1) If isNewTxn, revert
   * (2) Interpolate line from startPos to currPos
   */
  if (!isNewTxn) {
    revertTransaction(currentTxn);
  } 

  points = lerp(startPos, currPos, 5);
  updateTransaction(txn, points);

  return points;
}

void
Canvas::fill(const wxPoint &p, const Color &color, Transaction &txn) {
  /*
   * Steps:
   * (1) From given point 'p', perform breadth-first
   *     algorithm, fill in every pixel whose color
   *     is the same as 'p'.
   * (2) Update given Transaction 'txn' with all filled
   *     pixels.
   * 
   * Note: we are not 'filling' in the pixel here, that is
   * done in calling event handler function. This function
   * simply passes back the points to be filled in. 
   */
  Color c = getPixelColor(p);
  if (c == color) {
    return;
  }

  int loc = LOC(p.x, p.y, width);
  txn.update(Pixel(c,p));
  Buffer[loc] = color.r;
  Buffer[loc+1] = color.g;
  Buffer[loc+2] = color.b;

  std::queue<wxPoint> Q;
  Q.push(p);

  wxPoint neighbors[4];
  int ncount;
  wxPoint _p;
  while (!Q.empty()) {
    _p = Q.front();
    Q.pop();

    getNeighbors(_p, neighbors, ncount);

    std::string _pstr;
    int _x, _y;
    int i;
    for (i=0; i<ncount; i++) {
      _x = neighbors[i].x;
      _y = neighbors[i].y;
      char r,g,b;
      loc = LOC(_x, _y, width);
      r = Buffer[loc];
      g = Buffer[loc+1];
      b = Buffer[loc+2];
      if (r == c.r && g == c.g && b == c.b) {
        txn.update(Pixel(r,g,b,_x,_y));
        Buffer[loc] = color.r;
        Buffer[loc+1] = color.g;
        Buffer[loc+2] = color.b;
        Q.push(wxPoint(_x,_y));
      }
    }
  }
}

std::vector<wxPoint>
Canvas::drawRectangle(const wxPoint &tl, const wxPoint &br, const int &w)
{
  /* 
   * tl
   * p3                              p2
   * |-------------------------------|
   * |                               |
   * |                               | 
   * |-------------------------------|
   * p0                              p1
   *                                 br
   */
  std::vector<wxPoint> points;
  wxPoint p0, p1, p2, p3;

  auto insert = [&points, &w](wxPoint &from, wxPoint &to)
  {
    std::vector<wxPoint> line;
    line = lerp(from, to, w);
    points.insert(points.end(), line.begin(), line.end());
  };

  p0 = wxPoint(tl.x, br.y);
  p1 = wxPoint(br.x, br.y);
  p2 = wxPoint(br.x, tl.y);
  p3 = wxPoint(tl.x, tl.y);

  insert(p0, p1);
  insert(p1, p2);
  insert(p2, p3);
  insert(p3, p0);

  return points; 
}

std::vector<wxPoint>
Canvas::drawRectangle(const wxPoint &p1, Transaction &txn, const int &_width)
{
  /*
   * Approach:
   * 1. If isNewTxn is FALSE (i.e. user previously moved the mouse) then
   *    revert the current Transaction
   * 2. Get the pixels to fill in by performing linear interpolation on each
   *    pair of coordinates on the rectangle
   *      <x0, y0> -> <x1, y1>, <x0, y0> -> <x0, y1>
   *      <x1, y0> -> <x1, y1>, <x1, y1> -> <x1, y0>
   * 3. Update currentTxn to include all points from 2.
   * 4. Return the points
   */
  // (1)
  if (!isNewTxn) {
    revertTransaction(currentTxn);
  }

  // (2)
  std::vector<wxPoint> points;

  {
    wxPoint p2 = wxPoint(startPos.x, p1.y);
    wxPoint p3 = wxPoint(p1.x, startPos.y);
    std::vector<wxPoint> lines[4];

    lines[0] = lerp(startPos, p3, _width);
    lines[1] = lerp(p1, p3, _width);
    lines[2] = lerp(p1, p2, _width);
    lines[3] = lerp(startPos, p2, _width);

    int i;
    for (i=0; i < 4; i++) {
      points.insert(points.end(), lines[i].begin(), lines[i].end());
    }
  }

  updateTransaction(txn, points);
  return points;
}

/*
 * Used for generating Selection Border
 * - Given a border, remove points on the border
 */
std::vector<wxPoint>
Canvas::makeDashed(const std::vector<wxPoint> &border)
{
  std::vector<wxPoint> dashed;
  int i;
  for (i=0; i < border.size()-5; i++) {
    if (i % 8 == 0) {
      if (i+5 >= border.size()) {
        break;
      }
      dashed.insert(
          dashed.end(),
          border.begin() + i,
          border.begin() + i + 5);
    }
  }
  return dashed;
}

void printPixels(std::vector<Pixel> pixels) {
  int i;
  Pixel p;
  for (i=0; i < pixels.size(); i++) {
    p = pixels[i];
    printf("pixel: (%d, %d) (%d %d %d)\n",
        p.x,
        p.y,
        p.color.r,
        p.color.g,
        p.color.b);
  }
}

/*
 * Reset all selection related fields
 */
void Canvas::clearSelection() {
  selected = false;
  selectionArea.clear();
  selectionBorder.clear();
  revertTransaction(selectTxn);
  selectTxn.pixels.clear();
  if (selection != NULL) {
    delete selection;
    selection = NULL;
  }
}

void
Canvas::handleSelectionClick(wxPoint &pt)
{
  if (!selected || selection == NULL) {
    return;
  }
  if (!selection->isWithinBounds(pt)) {
    clearSelection();
  }
}

/*
 * Functions to set the selection area
 * based on the Selection object.
 * One function for each type of selection.
 */
void
Canvas::getSelectionArea(
    std::vector<Pixel> &area,
    RectangleSelection *selection)
{
  /*
   * The width and height of the selection
   * should include the pixels occupied
   * by the selection border
   */
  int _width = (selection->maxX - selection->minX);
  int _height = (selection->maxY - selection->minY);
  area.resize(_width * _height);

  int startX = selection->minX + 1;
  int startY = selection->minY + 1;
  int endX = startX + _width;
  int endY = startY + _height;

  /* Set the selectionArea pixels */
  int i, j, k=0;
  Pixel p;
  wxPoint pt;
  for (i=startX; i < endX; i++) {
    for (j=startY; j < endY; j++) {
      pt = wxPoint(i, j);
      p = getPixel(pt);
      area[k] = p;
      k++;
    }
  }
}

void
Canvas::getSelectionArea(
    std::vector<Pixel> &selectionArea,
    CircleSelection *selection)
{
  wxPoint c = selection->_c;
  double r = selection->_r;

  int minX, maxX, minY, maxY;
  minX = selection->minX;
  maxX = selection->maxX;
  minY = selection->minY;
  maxY = selection->maxY;

  int x, y;
  wxPoint pt;
  for (x=minX; x < maxX; x++) {
    for (y=minY; y < maxY; y++) {
      pt = wxPoint(x, y);
      if (squaredLength(pt, c) < r*r) {
        selectionArea.push_back(getPixel(pt));
      }
    }
  }

}

void
Canvas::getSelectionArea(
    std::vector<Pixel> &selectionArea,
    LassoSelection *selection)
{
  int minX, maxX, minY, maxY;
  minX = selection->minX;
  maxX = selection->maxX;
  minY = selection->minY;
  maxY = selection->maxY;

  int x, y;
  wxPoint pt;
  for (x=minX+1; x < maxX; x++) {
    for (y=minY+1; y < maxY; y++) {
      pt = wxPoint(x, y);
      if (selection->isWithinBounds(pt)) {
        selectionArea.push_back(getPixel(pt));
      }
    }
  }
}

/*
 * Handles mouseMove event for selection tools
 * Takes in a "drawBorder" parameter which points to one
 * of: drawRectangle(), drawCircle(), or drawFreehand()
 * depending on the tool type.
 */
void
Canvas::handleSelectionMove(const wxPoint &currPos,
  std::vector<wxPoint> (Canvas::*drawBorder)(const wxPoint&, Transaction &, const int&))
{
  /*
   * Two cases to consider:
   * 1. User hasn't made a selection
   *    - Have to draw the selection border
   * 2. User has a selection
   *    - Have to move the selected pixels to the
   *      new position
   */
  if (!selected) {
    Transaction txn, sTxn;
    if (!isNewTxn)
      revertTransaction(currentTxn);

    selectionBorder = (this->*drawBorder)(currPos, txn, 1);
    updateBuffer(
        makeDashed(selectionBorder),
        SELECT);

    selectTxn = txn;
    currentTxn = txn;
  }
  else {
    int xOffset = currPos.x - startPos.x;
    int yOffset = currPos.y - startPos.y;
    Transaction txn;
    move(selectionArea, xOffset, yOffset, txn);
    currentTxn = txn;
  }
}

void
Canvas::handleSelectionRelease(const wxPoint &p0, const wxPoint &p1)
{
  /*
   * Two cases to consider:
   * 1. User has not made a selection yet
   *      The release action will complete the selection
   *      process and define the selectionArea.
   * 2. User has a selection already
   *      Clear the selection.
   */
  if (selected) {
    clearSelection();
  }
  else {
    /*
     * Selection area provided by the getSelectionArea()
     * functions don't include the border pixels because
     * when we reach this function, the bordr pixels
     * will no longer have the original colours but the
     * SELECT colour as the border is already rendered.
     * We get the border pixels from the selectTxn
     * because selectTxn has the original colours.
     */
    std::vector<wxPoint> interp = lerp(p0, p1, 1);
    switch (toolType) {
      case SlctRect:
        selection = new RectangleSelection(p0, p1);
        getSelectionArea(selectionArea,
            dynamic_cast<RectangleSelection *>(selection));
        break;
      case SlctCircle:
        selection = new CircleSelection(p0, p1);
        getSelectionArea(selectionArea,
            dynamic_cast<CircleSelection *>(selection));
        break;
      case Lasso:
        /*
         * Have to connect the start and end mouse
         * positions to create a closed polygon shape.
         * Since the border is changed, we also
         * have to update selectTxn which is in charge
         * of reverting the border once the user
         * completes translation (on mouse release).
         */
        selectionBorder.insert(selectionBorder.end(),
            interp.begin(), interp.end());
        updateTransaction(selectTxn, interp);
        selection = new LassoSelection(p0, p1, selectionBorder);
        getSelectionArea(selectionArea,
            dynamic_cast<LassoSelection *>(selection));
        break;
      default:
        break;
    }
    selectionArea.insert(selectionArea.end(),
        selectTxn.pixels.begin(),
        selectTxn.pixels.end());
    selected = true;
  }
}

void
Canvas::move(
    const std::vector<Pixel> &pixels,
    const int &xOffset, const int &yOffset,
    Transaction &txn)
{
  /*
   * Step:
   * 1. Save all pixels (the original selection, and the new
   *    translaged pixels). This has to be done first in case
   *    there is overlap between the original selection and
   *    the new position.
   * 2. White out selection (should include border pixels)
   * 3. Translate the selection area
   * 4. Save border pixels to selectTxn (required
   *      for mouseRelease event - have to revert
   *      the border)
   * 5. Redraw border
   */
  if (!isNewTxn) {
    revertTransaction(currentTxn);
  }

  // (1) Save all pixels (original and translated)
  {
    int i;
    Pixel pixel, translatedPix;
    wxPoint oldPt, newPt;
    for (i=0; i < selectionArea.size(); i++) {
      pixel = selectionArea[i];

      newPt = wxPoint(pixel.x + xOffset, pixel.y + yOffset);
      oldPt = wxPoint(pixel.x, pixel.y);
      translatedPix = Pixel(getPixelColor(newPt), newPt);
      txn.update(pixel);
      txn.update(translatedPix);
    }
  }

  // (2) white out pixels
  {
    int i;
    wxPoint oldPt, newPt;
    Pixel pixel, whitePixel, oldPixel;
    for (i=0; i < selectionArea.size(); i++) {
      pixel = selectionArea[i];
      oldPt = wxPoint(pixel.x, pixel.y);
      whitePixel = Pixel(Color(255, 255, 255), oldPt);
      updateBuffer(whitePixel);
    }
  }

  // (3) translate the selection
  {
    int i;
    wxPoint newPt;
    Color clr;
    Pixel pixel, newPixel, p;
    for (i=0; i < selectionArea.size(); i++) {
      pixel = selectionArea[i];
      clr = pixel.color;

      newPt = wxPoint(
          pixel.x + xOffset,
          pixel.y + yOffset);
      newPixel = Pixel(clr, newPt);

      updateBuffer(newPixel);
    }
  }

  // (4-5) Save border pixels to select txn, draw border
  {
    int i;
    wxPoint pt, newPt;
    std::vector<wxPoint> border;
    border.resize(selectionBorder.size());

    for (i=0; i < selectionBorder.size(); i++) {
      pt = selectionBorder[i];
      newPt = wxPoint(pt.x + xOffset, pt.y + yOffset);
      selectTxn.pixels[i] = Pixel(getPixelColor(newPt), newPt);
      border[i] = newPt;
    }

    /* draw border */
    updateBuffer(makeDashed(border), SELECT);
  }
}