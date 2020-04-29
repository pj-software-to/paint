#ifndef __MAIN_APP__
#define __MAIN_APP__

class MainApp: public wxApp {
  public:
    virtual bool OnInit();
};

class MainFrame: public wxFrame {
  public:
    MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
};

DECLARE_APP(MainApp)

#endif
