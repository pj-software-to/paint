#ifndef __MAIN_APP__
#define __MAIN_APP__

#include "canvas.h"

class MainFrame: public wxFrame {
public:
  MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

  DECLARE_EVENT_TABLE()
};

class MainApp: public wxApp {
  public:
    virtual bool OnInit();
    MainFrame *frame;
    Canvas *canvas;
};

DECLARE_APP(MainApp)

#endif
