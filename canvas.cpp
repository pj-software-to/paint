//
// Created by Patricia M Marukot on 2020-04-30.
//
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "canvas.h"

BEGIN_EVENT_TABLE( Canvas, wxPanel )
  EVT_PAINT(Canvas::paintEvent)
END_EVENT_TABLE()

Canvas::Canvas(wxFrame *parent) :
wxPanel(parent) {}

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
  // draw some text
  dc.DrawText(wxT("Testing"), 40, 60);

  // draw a circle
  dc.SetBrush(*wxGREEN_BRUSH); // green filling
  dc.SetPen( wxPen( wxColor(255,0,0), 5 ) ); // 5-pixels-thick red outline
  dc.DrawCircle( wxPoint(200,100), 25 /* radius */ );

  // draw a rectangle
  dc.SetBrush(*wxBLUE_BRUSH); // blue filling
  dc.SetPen( wxPen( wxColor(255,175,175), 10 ) ); // 10-pixels-thick pink outline
  dc.DrawRectangle( 300, 100, 400, 200 );

  // draw a line
  dc.SetPen( wxPen( wxColor(0,0,0), 3 ) ); // black line, 3 pixels thick
  dc.DrawLine( 300, 100, 700, 300 ); // draw line across the rectangle

  // Look at the wxDC docs to learn how to draw other stuff
}