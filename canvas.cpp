//
// Created by Patricia M Marukot on 2020-04-30.
//
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "canvas.h"
#include "interpolation.h"

#include <stdio.h>
#include <iostream>

#define LOC(x,y,w) (3*((y)*(w)+(x)))

BEGIN_EVENT_TABLE( Canvas, wxPanel )
  EVT_PAINT(Canvas::paintEvent)
  EVT_MOTION(Canvas::mouseMoved)
  EVT_LEFT_DOWN(Canvas::mouseDown)
  EVT_LEFT_UP(Canvas::mouseReleased)
END_EVENT_TABLE()

/* CONSTRUCTORS */
Canvas::Canvas(wxFrame *parent) :
wxPanel(parent) {
  color.set(0, 0, 0);
}

Canvas::Canvas(wxFrame *parent, unsigned int width, unsigned int height) :
wxPanel(parent) {
  this->width = width;
  this->height = height;

  color.set(0, 0, 0);

  /* Initialize the buffer */
  size_t sz = 3*width*height*sizeof(char);
  Buffer = (char *)malloc(sz);

  /* White-out buffer */
  memset(Buffer, 255, sz);
}

void Canvas::addTransaction(Transaction &t) {
  transactions.push_back(&t);
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
    case DrawRect:
    case DrawCircle:
      updateBuffer(
        drawCircle(currPos, txn),
        color);
      currentTxn = txn;
      break;
    case Eraser:
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
}

void Canvas::mouseReleased(wxMouseEvent &evt)
{
}

std::vector<wxPoint> Canvas::drawCircle(const wxPoint &currPos, Transaction &txn) {
  std::vector<wxPoint> points;
  
  return points; 
}
