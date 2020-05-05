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
  wxImage::AddHandler(new wxPNGHandler);
  wxBitmap exit(wxT("exit.png"), wxBITMAP_TYPE_PNG);
  wxBitmap pencil(wxT("pencil.png"), wxBITMAP_TYPE_PNG);
  wxBitmap circle(wxT("circle.png"), wxBITMAP_TYPE_PNG);
  wxBitmap rectangle(wxT("rectangle.png"), wxBITMAP_TYPE_PNG);
  wxBitmap line(wxT("line.png"), wxBITMAP_TYPE_PNG);
  wxBitmap eraser(wxT("eraser.png"), wxBITMAP_TYPE_PNG);
  wxBitmap slctRect(wxT("slct_rect.png"), wxBITMAP_TYPE_PNG);

  toolBar = CreateToolBar(wxTB_VERTICAL);

  /* Drawing Tools */
  toolBar->AddTool(BTN_Pencil, wxT("Pencil"), pencil);
  toolBar->AddTool(BTN_Draw_line, wxT("Line"), line);
  toolBar->AddTool(BTN_Draw_rect, wxT("Rectangle"), rectangle);
  toolBar->AddTool(BTN_Draw_circ, wxT("Circle"), circle);

  /* Misc */
  toolBar->AddTool(BTN_Eraser, wxT("Eraser"), eraser);
  toolBar->AddTool(BTN_Fill, wxT("Fill"), exit);

  /* Selection Tools */
  toolBar->AddTool(BTN_Slct_rect, wxT("Select Rectangle"), slctRect);
  toolBar->AddTool(BTN_Slct_circ, wxT("Select Circle"), exit);
  toolBar->AddTool(BTN_Slct_lasso, wxT("Lasso"), exit);

  /* Render */
  toolBar->Realize();

  Connect(BTN_Pencil, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(Canvas::SetPencil));
  Connect(BTN_Draw_line, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(Canvas::SetDrawLine));
  Connect(BTN_Draw_rect, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(Canvas::SetDrawRect));
  Connect(BTN_Draw_circ, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(Canvas::SetDrawCircle));
  Connect(BTN_Eraser, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(Canvas::SetEraser));
  Connect(BTN_Fill, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(Canvas::SetFill));
  Connect(BTN_Slct_rect, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(Canvas::SetSlctRect));
  Connect(BTN_Slct_circ, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(Canvas::SetSlctCircle));
  Connect(BTN_Slct_lasso, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(Canvas::SetLasso));
}
