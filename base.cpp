#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>
#include "canvas.h"
#include "base.h"

#define DATA_PATH "data"
#define DEFAULT_WIDTH 1000 
#define DEFAULT_HEIGHT 500 

BEGIN_EVENT_TABLE( MainFrame, wxFrame )
END_EVENT_TABLE()

IMPLEMENT_APP(MainApp)

bool MainApp::OnInit()
{
  wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
  frame = new MainFrame(wxT("Hello wxDC"), wxPoint(50,50), wxSize(800,600));

  unsigned int width, height;
  
  canvas = new Canvas( (wxFrame*) frame,
    DEFAULT_WIDTH, DEFAULT_HEIGHT);
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
  std::string DPATH(DATA_PATH);
  std::string exitP = DPATH + "/exit.png";
  std::string pencilP = DPATH + "/pencil.png";
  std::string circleP = DPATH + "/circle.png";
  std::string rectP = DPATH + "/rectangle.png";
  std::string lineP = DPATH + "/line.png";
  std::string eraserP = DPATH + "/eraser.png";
  std::string slctP = DPATH + "/slct_rect.png";

  wxImage::AddHandler(new wxPNGHandler);
  wxBitmap exit(wxString(exitP), wxBITMAP_TYPE_PNG);
  wxBitmap pencil(wxString(pencilP), wxBITMAP_TYPE_PNG);
  wxBitmap circle(wxString(circleP), wxBITMAP_TYPE_PNG);
  wxBitmap rectangle(wxString(rectP), wxBITMAP_TYPE_PNG);
  wxBitmap line(wxString(lineP), wxBITMAP_TYPE_PNG);
  wxBitmap eraser(wxString(eraserP), wxBITMAP_TYPE_PNG);
  wxBitmap slctRect(wxString(slctP), wxBITMAP_TYPE_PNG);

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
