#ifndef __MAIN_APP__
#define __MAIN_APP__

#include "canvas.h"

class MainFrame: public wxFrame {
public:
  MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
  wxToolBar *toolBar;
  wxColourPickerCtrl *colorPicker;

  void OnColourChanged(wxColourPickerEvent &evt);
  void setThiccnessTool(bool enabled);

  DECLARE_EVENT_TABLE()
};

class MainApp: public wxApp {
  public:
    virtual bool OnInit();
    MainFrame *frame;
    Canvas *canvas;

    void OnColourChanged(wxColourPickerEvent &evt);

    void SetCanvasPencil(wxCommandEvent& WXUNUSED(event));
    void SetCanvasDrawLine(wxCommandEvent& WXUNUSED(event));
    void SetCanvasDrawRect(wxCommandEvent& WXUNUSED(event));
    void SetCanvasDrawCircle(wxCommandEvent& WXUNUSED(event));
    void SetCanvasEraser(wxCommandEvent& WXUNUSED(event));
    void SetCanvasFill(wxCommandEvent& WXUNUSED(event));
    void SetCanvasSlctRect(wxCommandEvent& WXUNUSED(event));
    void SetCanvasSlctCircle(wxCommandEvent& WXUNUSED(event));
    void SetCanvasLasso(wxCommandEvent& WXUNUSED(event));
    void SetThiccness1(wxCommandEvent& WXUNUSED(event));
    void SetThiccness2(wxCommandEvent& WXUNUSED(event));
    void SetThiccness3(wxCommandEvent& WXUNUSED(event));

    void disableThiccness();
    void enableThiccness();
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
  BTN_Fill = wxID_HIGHEST + 9,
  CLR_PICKER = wxID_HIGHEST + 10,
  THICC_1 = wxID_HIGHEST + 11,
  THICC_2 = wxID_HIGHEST + 12,
  THICC_3 = wxID_HIGHEST + 13
};

#endif
