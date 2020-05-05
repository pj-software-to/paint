//
// Created by Patricia M Marukot on 2020-04-30.
//
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "canvas.h"

#include <stdio.h>
#include <iostream>

#define LOC(x,y,w) (3*((y)*(w)+(x)))

BEGIN_EVENT_TABLE( Canvas, wxPanel )
  EVT_PAINT(Canvas::paintEvent)
END_EVENT_TABLE()

/* Event handlers to handle toolbar ONCLICK events */
void Canvas::SetPencil(wxCommandEvent& WXUNUSED(event)) {
  toolType = Pencil;
  std::cout << "Pencil" << std::endl;
}
void Canvas::SetDrawLine(wxCommandEvent& WXUNUSED(event)) {
  toolType = Line;
  std::cout << "Line" << std::endl;
}
void Canvas::SetDrawRect(wxCommandEvent& WXUNUSED(event)) {
  toolType = DrawRect;
  std::cout << "Draw Rectangle" << std::endl;
}
void Canvas::SetDrawCircle(wxCommandEvent& WXUNUSED(event)) {
  toolType = DrawCircle;
  std::cout << "Draw Circle" << std::endl;
}
void Canvas::SetEraser(wxCommandEvent& WXUNUSED(event)) {
  toolType = Eraser;
  std::cout << "Eraser" << std::endl;
}
void Canvas::SetFill(wxCommandEvent& WXUNUSED(event)) {
  toolType = Fill;
  std::cout << "Fill" << std::endl;
}
void Canvas::SetSlctRect(wxCommandEvent& WXUNUSED(event)) {
  toolType = SlctRect;
  std::cout << "Select Rectangle" << std::endl;
}
void Canvas::SetSlctCircle(wxCommandEvent& WXUNUSED(event)) {
  toolType = SlctCircle;
  std::cout << "Select Circle" << std::endl;
}
void Canvas::SetLasso(wxCommandEvent& WXUNUSED(event)) {
  toolType = Lasso;
  std::cout << "Lasso" << std::endl;
}

Canvas::Canvas(wxFrame *parent) :
wxPanel(parent) {}

Canvas::Canvas(wxFrame *parent, unsigned int width, unsigned int height) :
wxPanel(parent) {
  this->width = width;
  this->height = height;

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
  Buffer[i] = p.color.r;
  Buffer[i+1] = p.color.g;
  Buffer[i+2] = p.color.b;
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
  int i,j;
  i=height/2;
  for (j=0; j<width; j++) {
    Pixel p(0, 0, 0, j, i);
    updateBuffer(p);
  }

  wxImage img(width, height, (unsigned char *)Buffer, true);
  wxBitmap bmp(img);  
  dc.DrawBitmap(bmp, 0, 0, false);
  ///////////////////////////////////
}


