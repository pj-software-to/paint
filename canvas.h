//
// Created by Patricia M Marukot on 2020-04-30.
//

#ifndef PAINT_CANVAS_H
#define PAINT_CANVAS_H

#include "transaction.h"

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
  private:
    unsigned int width;
    unsigned int height;

    /* This is the main buffer that is drawn to the screen */
    char *Buffer;
    std::vector<Transaction *> transactions;
    
    /*
     * Private functions
     */
    void updateBuffer(const Pixel &p);
    void addTransaction(Transaction &t);

  public:
    Canvas(wxFrame *parent);
    Canvas(wxFrame *parent, unsigned int width, unsigned int height);
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
