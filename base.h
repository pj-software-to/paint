#ifndef __MAIN_APP__
#define __MAIN_APP__

#include "canvas.h"

class MainFrame: public wxFrame {
public:
  MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
  wxToolBar *toolBar;

  DECLARE_EVENT_TABLE()
};

class MainApp: public wxApp {
  public:
    virtual bool OnInit();
    MainFrame *frame;
    Canvas *canvas;
};

DECLARE_APP(MainApp)

enum
{
  BTN_Pencil = wxID_HIGHEST + 1,
  BTN_Draw_line = wxID_HIGHEST + 2,
  BTN_Draw_rect = wxID_HIGHEST + 3,
  BTN_Draw_circ = wxID_HIGHEST + 4,
  BTN_Eraser = wxID_HIGHEST + 5,
  BTN_Slct_rect = wxID_HIGHEST + 6,
  BTN_Slct_circ = wxID_HIGHEST + 7,
  BTN_Slct_lasso = wxID_HIGHEST + 8,
  BTN_Fill = wxID_HIGHEST + 9
};

#endif
