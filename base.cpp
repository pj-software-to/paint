#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "canvas.h"
#include "base.h"

BEGIN_EVENT_TABLE( MainFrame, wxFrame )
END_EVENT_TABLE()

IMPLEMENT_APP(MainApp)

bool MainApp::OnInit()
{
  wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
  frame = new MainFrame(wxT("Hello wxDC"), wxPoint(50,50), wxSize(800,600));

  canvas = new Canvas( (wxFrame*) frame );
  sizer->Add(canvas, 1, wxEXPAND);

  frame->SetSizer(sizer);
  frame->SetAutoLayout(true);

  frame->Show();
  return true;
}

MainFrame::MainFrame(const wxString &title,
    const wxPoint &pos,
    const wxSize &size)
    : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  // initialize buttons, textboxes, etc.
}