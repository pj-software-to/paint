//
// Created by Patricia M Marukot on 2020-04-30.
//

#ifndef PAINT_CANVAS_H
#define PAINT_CANVAS_H

enum ToolType
{
  Pencil,
  Line,
  DrawRect,
  DrawCircle,
  Eraser,
  Fill,
  SlctRect,
  SlctCircle,
  Lasso
};

class Canvas : public wxPanel {
  public:
    Canvas(wxFrame *parent);
    ToolType toolType;

    void paintEvent(wxPaintEvent & evt);
    void paintNow();

    void SetPencil(wxCommandEvent& WXUNUSED(event));
    void SetDrawLine(wxCommandEvent& WXUNUSED(event));
    void SetDrawRect(wxCommandEvent& WXUNUSED(event));
    void SetDrawCircle(wxCommandEvent& WXUNUSED(event));
    void SetEraser(wxCommandEvent& WXUNUSED(event));
    void SetFill(wxCommandEvent& WXUNUSED(event));
    void SetSlctRect(wxCommandEvent& WXUNUSED(event));
    void SetSlctCircle(wxCommandEvent& WXUNUSED(event));
    void SetLasso(wxCommandEvent& WXUNUSED(event));

    void render(wxDC& dc);
    DECLARE_EVENT_TABLE()
};

#endif //PAINT_CANVAS_H
