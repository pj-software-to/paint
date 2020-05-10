#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>
#include <iostream>
#include "canvas.h"
#include "base.h"

#define DATA_PATH "data"
#define DEFAULT_WIDTH 1000
#define DEFAULT_HEIGHT 500
#define MAIN_FRAME_WIDTH 1100
#define MAIN_FRAME_HEIGHT 600

BEGIN_EVENT_TABLE( MainFrame, wxFrame )
END_EVENT_TABLE()

IMPLEMENT_APP(MainApp)

bool MainApp::OnInit()
{
  wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
  frame = new MainFrame(wxT("Hello wxDC"), wxPoint(50,50),
    wxSize(MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT));

  unsigned int width, height;
  
  canvas = new Canvas( (wxFrame*) frame,
    DEFAULT_WIDTH, DEFAULT_HEIGHT);
  sizer->Add(canvas, 1, wxEXPAND);

  canvas->toolType = Pencil;

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
  /* Create sidebar with tools */
  std::string DPATH(DATA_PATH);
  std::string exitP = DPATH + "/exit.png";
  std::string pencilP = DPATH + "/pencil1.png";
  std::string circleP = DPATH + "/circle.png";
  std::string rectP = DPATH + "/rectangle.png";
  std::string lineP = DPATH + "/line.png";
  std::string eraserP = DPATH + "/eraser.png";
  std::string slctRectP = DPATH + "/slct_rect.png";
  std::string slctCircleP = DPATH + "/slct_circle.png";
  std::string lassoP = DPATH + "/lasso.png";

  wxImage::AddHandler(new wxPNGHandler);
  wxBitmap exit(wxString(exitP), wxBITMAP_TYPE_PNG);
  wxBitmap pencil(wxString(pencilP), wxBITMAP_TYPE_PNG);
  wxBitmap circle(wxString(circleP), wxBITMAP_TYPE_PNG);
  wxBitmap rectangle(wxString(rectP), wxBITMAP_TYPE_PNG);
  wxBitmap line(wxString(lineP), wxBITMAP_TYPE_PNG);
  wxBitmap eraser(wxString(eraserP), wxBITMAP_TYPE_PNG);
  wxBitmap slctRect(wxString(slctRectP), wxBITMAP_TYPE_PNG);
  wxBitmap slctCircle(wxString(slctCircleP), wxBITMAP_TYPE_PNG);
  wxBitmap lasso(wxString(lassoP), wxBITMAP_TYPE_PNG);

  toolBar = CreateToolBar(wxTB_VERTICAL);

  /* Freehand Drawing Tools */
  toolBar->AddTool(BTN_Pencil, wxT("Pencil"), pencil);
  toolBar->AddTool(BTN_Eraser, wxT("Eraser"), eraser);
  toolBar->AddTool(BTN_Fill, wxT("Fill"), exit);

  /* Predefined Shapes */
  toolBar->AddTool(BTN_Draw_line, wxT("Line"), line);
  toolBar->AddTool(BTN_Draw_rect, wxT("Rectangle"), rectangle);
  toolBar->AddTool(BTN_Draw_circ, wxT("Circle"), circle);

  /* Selection Tools */
  toolBar->AddTool(BTN_Slct_rect, wxT("Select Rectangle"), slctRect);
  toolBar->AddTool(BTN_Slct_circ, wxT("Select Circle"), slctCircle);
  toolBar->AddTool(BTN_Slct_lasso, wxT("Lasso"), lasso);

  /* Render */
  toolBar->Realize();

  /* Set event handlers for each button */
  Connect(BTN_Pencil, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(MainApp::SetCanvasPencil));
  Connect(BTN_Draw_line, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(MainApp::SetCanvasDrawLine));
  Connect(BTN_Draw_rect, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(MainApp::SetCanvasDrawRect));
  Connect(BTN_Draw_circ, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(MainApp::SetCanvasDrawCircle));
  Connect(BTN_Eraser, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(MainApp::SetCanvasEraser));
  Connect(BTN_Fill, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(MainApp::SetCanvasFill));
  Connect(BTN_Slct_rect, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(MainApp::SetCanvasSlctRect));
  Connect(BTN_Slct_circ, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(MainApp::SetCanvasSlctCircle));
  Connect(BTN_Slct_lasso, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(MainApp::SetCanvasLasso));
}

/*********** Event handlers to handle ONCLICK events for toolbar ************/
void MainApp::SetCanvasPencil(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = Pencil;
}

void MainApp::SetCanvasDrawLine(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = Line;
}

void MainApp::SetCanvasDrawRect(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = DrawRect;
}

void MainApp::SetCanvasDrawCircle(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = DrawCircle;
}

void MainApp::SetCanvasEraser(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = Eraser;
}

void MainApp::SetCanvasFill(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = Fill;
}

void MainApp::SetCanvasSlctRect(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = SlctRect;
}

void MainApp::SetCanvasSlctCircle(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = SlctCircle;
}

void MainApp::SetCanvasLasso(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = Lasso;
}