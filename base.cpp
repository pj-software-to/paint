#include <wx/wxprec.h>
#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include "base.h"

IMPLEMENT_APP(MainApp)

bool MainApp::OnInit() {
  MainFrame *MainWin = new MainFrame(_("Hello World"), wxDefaultPosition, wxSize(300, 200));
  MainWin->Show(true);
  SetTopWindow(MainWin);
  return true;
}

MainFrame::MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size) : wxFrame((wxFrame *)NULL, -1, title, pos, size) {
  // initialize buttons, textboxes, etc.
}
