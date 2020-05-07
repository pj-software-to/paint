//
// Created by Patricia M Marukot on 2020-04-30.
//

#ifndef PAINT_CANVAS_H
#define PAINT_CANVAS_H

#include "transaction.h"
#include "pixel.h"

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

    /* The colour selected by the user */
    Color color;

    /* The previous mouse position
     * before mouse move event */
    wxPoint prevPos;

    /* The mouse position where the user first
     * clicked the left mouse button*/
    wxPoint startPos;

    /* Holds the transaction currently
     * taking place.
     * There should only be ONE txn occuring
     * at a time */
    Transaction currentTxn;

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

    /* Screen refresh event handlers */
    void paintEvent(wxPaintEvent & evt);
    void paintNow();

    /* Mouse event handlers */
    void mouseDown(wxMouseEvent & evt);
    void mouseMoved(wxMouseEvent & evt);
    void mouseReleased(wxMouseEvent & evt);

    void render(wxDC& dc);
    DECLARE_EVENT_TABLE()
};

#endif //PAINT_CANVAS_H
