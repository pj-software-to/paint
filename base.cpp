#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/clrpicker.h>
#include <wx/sysopt.h>

#include <string>
#include <iostream>
#include "canvas.h"
#include "base.h"

#define DATA_PATH "data"
#define DEFAULT_WIDTH 1000
#define DEFAULT_HEIGHT 500
#define MAIN_FRAME_WIDTH 1400
#define MAIN_FRAME_HEIGHT 900

#define wxMAC_USE_NATIVE_TOOLBAR 1

BEGIN_EVENT_TABLE( MainFrame, wxFrame )
  EVT_COLOURPICKER_CHANGED(CLR_PICKER, MainApp::OnColourChanged)
END_EVENT_TABLE()

IMPLEMENT_APP(MainApp)

bool MainApp::OnInit()
{
#ifdef __APPLE__
  wxSystemOptions::SetOption("mac.toolbar.no-native", 1);
#endif

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
  std::string bucketP = DPATH + "/bucket.png";
  std::string pencilP = DPATH + "/pencil1.png";
  std::string circleP = DPATH + "/circle.png";
  std::string rectP = DPATH + "/rectangle.png";
  std::string lineP = DPATH + "/line.png";
  std::string eraserP = DPATH + "/eraser.png";
  std::string slctRectP = DPATH + "/slct_rect.png";
  std::string slctCircleP = DPATH + "/slct_circle.png";
  std::string lassoP = DPATH + "/lasso.png";
  std::string thicc1P = DPATH + "/thicc1.png";
  std::string thicc2P = DPATH + "/thicc2.png";
  std::string thicc3P = DPATH + "/thicc3.png";

  wxImage::AddHandler(new wxPNGHandler);
  wxBitmap bucket(wxString(bucketP), wxBITMAP_TYPE_PNG);
  wxBitmap pencil(wxString(pencilP), wxBITMAP_TYPE_PNG);
  wxBitmap circle(wxString(circleP), wxBITMAP_TYPE_PNG);
  wxBitmap rectangle(wxString(rectP), wxBITMAP_TYPE_PNG);
  wxBitmap line(wxString(lineP), wxBITMAP_TYPE_PNG);
  wxBitmap eraser(wxString(eraserP), wxBITMAP_TYPE_PNG);
  wxBitmap slctRect(wxString(slctRectP), wxBITMAP_TYPE_PNG);
  wxBitmap slctCircle(wxString(slctCircleP), wxBITMAP_TYPE_PNG);
  wxBitmap lasso(wxString(lassoP), wxBITMAP_TYPE_PNG);
  wxBitmap thicc1(wxString(thicc1P), wxBITMAP_TYPE_PNG);
  wxBitmap thicc2(wxString(thicc2P), wxBITMAP_TYPE_PNG);
  wxBitmap thicc3(wxString(thicc3P), wxBITMAP_TYPE_PNG);

  toolBar = CreateToolBar(wxTB_VERTICAL);

  /* Freehand Drawing Tools */
  toolBar->AddTool(BTN_Pencil, wxT("Pencil"), pencil);
  toolBar->AddTool(BTN_Eraser, wxT("Eraser"), eraser);
  toolBar->AddTool(BTN_Fill, wxT("Fill"), bucket);

  /* Predefined Shapes */
  toolBar->AddTool(BTN_Draw_line, wxT("Line"), line);
  toolBar->AddTool(BTN_Draw_rect, wxT("Rectangle"), rectangle);
  toolBar->AddTool(BTN_Draw_circ, wxT("Circle"), circle);

  /* Selection Tools */
  toolBar->AddTool(BTN_Slct_rect, wxT("Select Rectangle"), slctRect);
  toolBar->AddTool(BTN_Slct_circ, wxT("Select Circle"), slctCircle);
  toolBar->AddTool(BTN_Slct_lasso, wxT("Lasso"), lasso);

  /* Line thiccness */
  toolBar->AddTool(THICC_1, wxT("1 pixel"), thicc1);
  toolBar->AddTool(THICC_2, wxT("3 pixels"), thicc2);
  toolBar->AddTool(THICC_3, wxT("5 pixels"), thicc3);

  /* Colour picker */
  wxColourPickerCtrl* colourPickerCtrl = new wxColourPickerCtrl(
      toolBar, CLR_PICKER, *wxBLACK, wxDefaultPosition, wxDefaultSize,
      wxCLRP_DEFAULT_STYLE, wxDefaultValidator, wxEmptyString);
  toolBar->AddControl(colourPickerCtrl);

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
  Connect(THICC_1, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(MainApp::SetThiccness1));
  Connect(THICC_2, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(MainApp::SetThiccness2));
  Connect(THICC_3, wxEVT_COMMAND_TOOL_CLICKED,
      wxCommandEventHandler(MainApp::SetThiccness3));
}

/*********** Event handlers to handle ONCLICK events for toolbar ************/
void MainApp::SetCanvasPencil(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = Pencil;
  enableThiccness();
}

void MainApp::SetCanvasDrawLine(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = Line;
  enableThiccness();
}

void MainApp::SetCanvasDrawRect(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = DrawRect;
  enableThiccness();
}

void MainApp::SetCanvasDrawCircle(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = DrawCircle;
  enableThiccness();
}

void MainApp::SetCanvasEraser(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = Eraser;
  enableThiccness();
}

void MainApp::SetCanvasFill(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = Fill;
  disableThiccness();
}

void MainApp::SetCanvasSlctRect(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = SlctRect;
  disableThiccness();
}

void MainApp::SetCanvasSlctCircle(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = SlctCircle;
  disableThiccness();
}

void MainApp::SetCanvasLasso(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->toolType = Lasso;
  disableThiccness();
}

void MainApp::OnColourChanged(wxColourPickerEvent &evt) {
  wxColour clr = evt.GetColour();
  wxGetApp().canvas->color = Color(clr.Red(),
      clr.Green(),
      clr.Blue());
}

void MainApp::SetThiccness1(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->thiccness = 1;
}

void MainApp::SetThiccness2(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->thiccness = 3;
}

void MainApp::SetThiccness3(wxCommandEvent& WXUNUSED(event)) {
  wxGetApp().canvas->thiccness = 5;
}

void MainApp::enableThiccness() {
  wxGetApp().frame->setThiccnessTool(true);
}

void MainApp::disableThiccness() {
  wxGetApp().frame->setThiccnessTool(false);
}

void MainFrame::setThiccnessTool(bool enabled) {
  toolBar->EnableTool(THICC_1, enabled);
  toolBar->EnableTool(THICC_2, enabled);
  toolBar->EnableTool(THICC_3, enabled);
  toolBar->Realize();
}