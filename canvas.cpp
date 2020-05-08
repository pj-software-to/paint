//
// Created by Patricia M Marukot on 2020-04-30.
//
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <stdio.h>
#include <iostream>
#include <math.h>

#include "helper.h"
#include "canvas.h"
#include "interpolation.h"

#define LOC(x,y,w) (3*((y)*(w)+(x)))

BEGIN_EVENT_TABLE( Canvas, wxPanel )
  EVT_PAINT(Canvas::paintEvent)
  EVT_MOTION(Canvas::mouseMoved)
  EVT_LEFT_DOWN(Canvas::mouseDown)
  EVT_LEFT_UP(Canvas::mouseReleased)
END_EVENT_TABLE()

Color WHITE = Color((char) 255, (char) 255, (char) 255);

/* CONSTRUCTORS */
Canvas::Canvas(wxFrame *parent) :
wxPanel(parent) {
  color = Color(0, 0, 0);
}

Canvas::Canvas(wxFrame *parent, unsigned int width, unsigned int height) :
wxPanel(parent) {
  this->width = width;
  this->height = height;

  color = Color(0, 0, 0);

  /* Initialize the buffer */
  size_t sz = 3*width*height*sizeof(char);
  Buffer = (char *)malloc(sz);

  /* White-out buffer */
  memset(Buffer, 255, sz);
}

void Canvas::addTransaction(Transaction &t) {
  transactions.push_back(&t);
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

Color Canvas::getPixelColor(wxPoint &p) {
  int i = LOC(p.x, p.y, width);
  if (i >= 3 * width * height)
    return Color(255, 255, 255);

  return Color(
    Buffer[i],
    Buffer[i+1],
    Buffer[i+2]
  );
}

void Canvas::updateBuffer(const Pixel &p) {
  /* Update buffer with new colors */
  int i = LOC(p.x, p.y, width);
  if (i >= 3 * width * height)
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

/* Event handlers to handle CANVAS mouse events */
/*
 * Handle CLICK event
 * Drawing Tools: Pencil, Line, DrawRect, DrawCircle
 *    - Fill in the clicked pixel - all have the same functionality
 */
void Canvas::mouseDown(wxMouseEvent &evt)
{
  /* Always should be left is down */
  assert(evt.LeftIsDown());

  isNewTxn = true;
  int x = evt.GetX();
  int y = evt.GetY();
  prevPos = wxPoint(x, y);
  startPos = wxPoint(x, y);

  Pixel p(color.r, color.g, color.b, x, y);
  switch(toolType) {
    case Pencil:
    case Line:
    case DrawRect:
    case DrawCircle:
      updateBuffer(p);
      break;
    case Eraser:
      updateBuffer(Pixel((char) 255, (char)255, (char)255, x, y));
      break;
    case Fill:
      break;
    case SlctRect:
      break;
    case SlctCircle:
      break;
    case Lasso:
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
      updateBuffer(
        linearInterpolation(prevPos, currPos),
        color);
      break;
    case Line:
      break;
    case DrawRect:
      break;
    case DrawCircle:
      updateBuffer(
        drawCircle(currPos, txn),
        color);
      currentTxn = txn;
      break;
    case Eraser:
      updateBuffer(
          linearInterpolation(prevPos, currPos),
          WHITE);
      break;
    case Fill:
      break;
    case SlctRect:
      break;
    case SlctCircle:
      break;
    case Lasso:
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
}

std::vector<wxPoint>
Canvas::drawCircle(const wxPoint &currPos, Transaction &txn) {
  std::vector<wxPoint> points;
  /*
   * Steps:
   * (1) If not first transaction, delete previous transaction
   * (2) Interpolate some points such that the interpolated points
   *     form a circle by using distance b/w currPos and startPos
   * (3) Further interpolate by calling either linear or polynomial
   *     interpolation on the sparse points to generate the pixels.
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
   * of the circuit
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

      pts = linearInterpolation(p0, p1);
      points.insert(points.end(), pts.begin(), pts.end());
    } 
  }

  // (4)
  {
    Pixel pixel; 
    wxPoint pt;
    int i;
    for (i=0; i<points.size(); i++) {
      pt = points[i];
      pixel = Pixel(getPixelColor(pt), pt);
      txn.update(pixel);
    }
  }

  return points;
}



















