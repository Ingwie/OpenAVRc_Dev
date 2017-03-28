#include "GvarsFrame.h"
#include "OpenAVRc_SimulatorMain.h"


//(*InternalHeaders(GvarsFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(GvarsFrame)
const long GvarsFrame::ID_GRID1 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoPhase0 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoPhase1 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoPhase2 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoPhase3 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoPhase4 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoPhase5 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar1 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar2 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar3 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar4 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar5 = wxNewId();
const long GvarsFrame::ID_PANEL1 = wxNewId();
const long GvarsFrame::ID_TIMERGVARS = wxNewId();
//*)

BEGIN_EVENT_TABLE(GvarsFrame,wxFrame)
  //(*EventTable(GvarsFrame)
  //*)
END_EVENT_TABLE()

GvarsFrame::GvarsFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
  //(*Initialize(GvarsFrame)
  Create(parent, wxID_ANY, _("Variables globales"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxSUNKEN_BORDER, _T("wxID_ANY"));
  SetClientSize(wxSize(384,155));
  Move(wxPoint(60,60));
  Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(0,48), wxSize(392,176), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
  GVARSGrid = new wxGrid(Panel1, ID_GRID1, wxPoint(0,0), wxSize(376,168), wxSUNKEN_BORDER, _T("ID_GRID1"));
  GVARSGrid->CreateGrid(5,6);
  GVARSGrid->Disable();
  GVARSGrid->EnableEditing(false);
  GVARSGrid->EnableGridLines(true);
  GVARSGrid->SetColLabelSize(22);
  GVARSGrid->SetRowLabelSize(52);
  GVARSGrid->SetDefaultRowSize(22, true);
  GVARSGrid->SetDefaultColSize(52, true);
  GVARSGrid->SetDefaultCellFont( GVARSGrid->GetFont() );
  GVARSGrid->SetDefaultCellTextColour( GVARSGrid->GetForegroundColour() );
  TextCtrlpersoPhase0 = new wxTextCtrl(Panel1, ID_TextCtrlpersoPhase0, _("npha0"), wxPoint(53,2), wxSize(53,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoPhase0"));
  TextCtrlpersoPhase0->SetMaxLength(6);
  TextCtrlpersoPhase1 = new wxTextCtrl(Panel1, ID_TextCtrlpersoPhase1, _("npha1"), wxPoint(105,2), wxSize(53,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoPhase1"));
  TextCtrlpersoPhase1->SetMaxLength(6);
  TextCtrlpersoPhase2 = new wxTextCtrl(Panel1, ID_TextCtrlpersoPhase2, _("npha2"), wxPoint(158,2), wxSize(52,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoPhase2"));
  TextCtrlpersoPhase2->SetMaxLength(6);
  TextCtrlpersoPhase3 = new wxTextCtrl(Panel1, ID_TextCtrlpersoPhase3, _("npha3"), wxPoint(210,2), wxSize(52,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoPhase3"));
  TextCtrlpersoPhase3->SetMaxLength(6);
  TextCtrlpersoPhase4 = new wxTextCtrl(Panel1, ID_TextCtrlpersoPhase4, _("npha4"), wxPoint(262,2), wxSize(52,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoPhase4"));
  TextCtrlpersoPhase4->SetMaxLength(6);
  TextCtrlpersoPhase5 = new wxTextCtrl(Panel1, ID_TextCtrlpersoPhase5, _("npha5"), wxPoint(314,2), wxSize(52,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoPhase5"));
  TextCtrlpersoPhase5->SetMaxLength(6);
  TextCtrlpersoGvar1 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar1, _("valvg1"), wxPoint(2,22), wxSize(52,21), wxTE_CENTRE|wxWANTS_CHARS|wxFULL_REPAINT_ON_RESIZE, wxDefaultValidator, _T("ID_TextCtrlpersoGvar1"));
  TextCtrlpersoGvar1->SetMaxLength(6);
  TextCtrlpersoGvar2 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar2, _("valvg2"), wxPoint(2,44), wxSize(52,22), wxTE_CENTRE|wxWANTS_CHARS|wxFULL_REPAINT_ON_RESIZE, wxDefaultValidator, _T("ID_TextCtrlpersoGvar2"));
  TextCtrlpersoGvar2->SetMaxLength(6);
  TextCtrlpersoGvar3 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar3, _("valvg3"), wxPoint(2,66), wxSize(52,22), wxTE_CENTRE|wxWANTS_CHARS|wxFULL_REPAINT_ON_RESIZE, wxDefaultValidator, _T("ID_TextCtrlpersoGvar3"));
  TextCtrlpersoGvar3->SetMaxLength(6);
  TextCtrlpersoGvar4 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar4, _("valvg4"), wxPoint(2,88), wxSize(52,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoGvar4"));
  TextCtrlpersoGvar4->SetMaxLength(6);
  TextCtrlpersoGvar5 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar5, _("valvg5"), wxPoint(2,110), wxSize(52,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoGvar5"));
  TextCtrlpersoGvar5->SetMaxLength(6);
  gvarsTimer.SetOwner(this, ID_TIMERGVARS);
  gvarsTimer.Start(2000, false);

  Connect(ID_GRID1,wxEVT_GRID_CELL_LEFT_CLICK,(wxObjectEventFunction)&GvarsFrame::OnGVARSGridCellLeftClick);
  Connect(ID_TextCtrlpersoPhase0,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoPhase0,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoPhase0TextEnter);
  Connect(ID_TextCtrlpersoPhase1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoPhase1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoPhase1TextEnter);
  Connect(ID_TextCtrlpersoPhase2,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoPhase2,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoPhase2TextEnter);
  Connect(ID_TextCtrlpersoPhase3,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoPhase3,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoPhase3TextEnter);
  Connect(ID_TextCtrlpersoPhase4,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoPhase4,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoPhase4TextEnter);
  Connect(ID_TextCtrlpersoPhase5,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoPhase5,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoPhase5TextEnter);
  Connect(ID_TextCtrlpersoGvar1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar1TextEnter);
  Connect(ID_TextCtrlpersoGvar2,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar2,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar2TextEnter);
  Connect(ID_TextCtrlpersoGvar3,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar3,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar3TextEnter);
  Connect(ID_TextCtrlpersoGvar4,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar4,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar4TextEnter);
  Connect(ID_TextCtrlpersoGvar5,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar5,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar5TextEnter);
  Connect(ID_TIMERGVARS,wxEVT_TIMER,(wxObjectEventFunction)&GvarsFrame::OngvarsTimerTrigger);
  Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&GvarsFrame::OnClose);
  //*)

  {
    wxIcon FrameIcon;
    SetIcon(wxICON(nsrcs_icon));
  }

  PopulateGvarsFrame();

}

GvarsFrame::~GvarsFrame()
{
  //(*Destroy(GvarsFrame)
  //*)
}

void GvarsFrame::PopulateGvarsFrame()
{
  //#define CFN_GVAR_CST_MAX    125 (from myeeprom.h)
  //#define MAX_GVARS            5
  //#define MAX_FLIGHT_MODES     6

  for (uint8_t p = 0; p < MAX_FLIGHT_MODES; p++) {
    for (int8_t x = 0; x < MAX_GVARS; x++) {
      int16_t gvarVal = (g_model.flightModeData[p].gvars[x]);
      wxString gvarStr;
      if (gvarVal == 0) gvarStr = "---";
      else if (gvarVal >= (CFN_GVAR_CST_MAX + MAX_GVARS - 0)) { // was -1
        gvarStr = "*PV";
        if (p <= (gvarVal - (CFN_GVAR_CST_MAX + MAX_GVARS - 0))) { // was -1
          ++gvarVal;                    // Flight phase can't refer to himself. Skip one phase.
        }
        gvarStr.Append (wxString::Format(wxT("%i"),(gvarVal - (CFN_GVAR_CST_MAX + MAX_GVARS - 1))));
      } else gvarStr.Append (wxString::Format(wxT("%i"),(gvarVal)));

      GVARSGrid->SetCellValue(x,p,gvarStr);//m:was x+1 p+1
    }
  }
  // Noms des VG
  wxString myString = ConvCharFwToWxstr(g_model.gvars[0].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR1";
  TextCtrlpersoGvar1->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[1].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR2";
  TextCtrlpersoGvar2->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[2].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR3";
  TextCtrlpersoGvar3->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[3].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR4";
  TextCtrlpersoGvar4->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[4].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR5";
  TextCtrlpersoGvar5->SetValue(myString);

  //TextCtrlpersoGvar1->SetValue(ConvCharFwToWxstr(g_model.gvars[0].name,LEN_GVAR_NAME));
  //TextCtrlpersoGvar2->SetValue(ConvCharFwToWxstr(g_model.gvars[1].name,LEN_GVAR_NAME));
  //TextCtrlpersoGvar3->SetValue(ConvCharFwToWxstr(g_model.gvars[2].name,LEN_GVAR_NAME));
  //TextCtrlpersoGvar4->SetValue(ConvCharFwToWxstr(g_model.gvars[3].name,LEN_GVAR_NAME));
  //TextCtrlpersoGvar5->SetValue(ConvCharFwToWxstr(g_model.gvars[4].name,LEN_GVAR_NAME));

  TextCtrlpersoGvar1->Refresh();
  TextCtrlpersoGvar2->Refresh();
  TextCtrlpersoGvar3->Refresh();
  TextCtrlpersoGvar4->Refresh();
  TextCtrlpersoGvar5->Refresh(); // Refresh all vertical textctrl because wxgrid mask them

  //Noms des phases de vol

  myString = ConvCharFwToWxstr(g_model.flightModeData[0].name,LEN_FLIGHT_MODE_NAME);
  if (myString == "") myString = "FM1";
  TextCtrlpersoPhase0->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.flightModeData[1].name,LEN_FLIGHT_MODE_NAME);
  if (myString == "") myString = "FM2";
  TextCtrlpersoPhase1->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.flightModeData[2].name,LEN_FLIGHT_MODE_NAME);
  if (myString == "") myString = "FM3";
  TextCtrlpersoPhase2->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.flightModeData[3].name,LEN_FLIGHT_MODE_NAME);
  if (myString == "") myString = "FM4";
  TextCtrlpersoPhase3->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.flightModeData[4].name,LEN_FLIGHT_MODE_NAME);
  if (myString == "") myString = "FM5";
  TextCtrlpersoPhase4->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.flightModeData[5].name,LEN_FLIGHT_MODE_NAME);
  if (myString == "") myString = "FM6";
  TextCtrlpersoPhase5->SetValue(myString);

  //TextCtrlpersoPhase0->SetValue(ConvCharFwToWxstr(g_model.flightModeData[0].name, LEN_FLIGHT_MODE_NAME));
  //TextCtrlpersoPhase1->SetValue(ConvCharFwToWxstr(g_model.flightModeData[1].name, LEN_FLIGHT_MODE_NAME));
  //TextCtrlpersoPhase2->SetValue(ConvCharFwToWxstr(g_model.flightModeData[2].name, LEN_FLIGHT_MODE_NAME));
  //TextCtrlpersoPhase3->SetValue(ConvCharFwToWxstr(g_model.flightModeData[3].name, LEN_FLIGHT_MODE_NAME));
  //TextCtrlpersoPhase4->SetValue(ConvCharFwToWxstr(g_model.flightModeData[4].name, LEN_FLIGHT_MODE_NAME));
  //TextCtrlpersoPhase5->SetValue(ConvCharFwToWxstr(g_model.flightModeData[5].name, LEN_FLIGHT_MODE_NAME));

  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}

void GvarsFrame::OnClose(wxCloseEvent& event)
{
  OpenAVRc_SimulatorFrame *parent = wxDynamicCast(this->GetParent(), OpenAVRc_SimulatorFrame);
  if(parent)
    parent->EnableOutputGvarsMenu();
  Destroy();
}

void GvarsFrame::OngvarsTimerTrigger(wxTimerEvent& event)
{
  PopulateGvarsFrame();
}

// Gestion affichage nom des Variables Globales
// VG1 *************************************
void GvarsFrame::OnTextCtrlpersoGvar1TextEnter(wxCommandEvent& event)
{
  wxString persoGvar1 = TextCtrlpersoGvar1->GetValue();
  ConvWxstrToCharFw(persoGvar1,g_model.gvars[0].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}
// VG2 *************************************
void GvarsFrame::OnTextCtrlpersoGvar2TextEnter(wxCommandEvent& event)
{
  wxString persoGvar2 = TextCtrlpersoGvar2->GetValue();
  ConvWxstrToCharFw(persoGvar2,g_model.gvars[1].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}
// VG3 *************************************
void GvarsFrame::OnTextCtrlpersoGvar3TextEnter(wxCommandEvent& event)
{
  wxString persoGvar3 = TextCtrlpersoGvar3->GetValue();
  ConvWxstrToCharFw(persoGvar3,g_model.gvars[2].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}
// VG4 *************************************
void GvarsFrame::OnTextCtrlpersoGvar4TextEnter(wxCommandEvent& event)
{
  wxString persoGvar4 = TextCtrlpersoGvar4->GetValue();
  ConvWxstrToCharFw(persoGvar4,g_model.gvars[3].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}
// VG5 *************************************
void GvarsFrame::OnTextCtrlpersoGvar5TextEnter(wxCommandEvent& event)
{
  wxString persoGvar5 = TextCtrlpersoGvar5->GetValue();
  ConvWxstrToCharFw(persoGvar5,g_model.gvars[4].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}

// Ajouts Variables au dela de VG5

// Gestion affichage du nom des phases de vol
// Phase de vol PV0 **********************
void GvarsFrame::OnTextCtrlpersoPhase0TextEnter(wxCommandEvent& event)
{
  wxString persoPhase0 = TextCtrlpersoPhase0->GetValue();
  ConvWxstrToCharFw(persoPhase0,g_model.flightModeData[0].name, LEN_FLIGHT_MODE_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}

// Phase de vol PV1 **********************
void GvarsFrame::OnTextCtrlpersoPhase1TextEnter(wxCommandEvent& event)
{
  wxString persoPhase1 = TextCtrlpersoPhase1->GetValue();
  ConvWxstrToCharFw(persoPhase1,g_model.flightModeData[1].name, LEN_FLIGHT_MODE_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}

// Phase de vol PV2 **********************
void GvarsFrame::OnTextCtrlpersoPhase2TextEnter(wxCommandEvent& event)
{
  wxString persoPhase2 = TextCtrlpersoPhase2->GetValue();
  ConvWxstrToCharFw(persoPhase2,g_model.flightModeData[2].name, LEN_FLIGHT_MODE_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}

// Phase de vol PV3 **********************
void GvarsFrame::OnTextCtrlpersoPhase3TextEnter(wxCommandEvent& event)
{
  wxString persoPhase3 = TextCtrlpersoPhase3->GetValue();
  ConvWxstrToCharFw(persoPhase3,g_model.flightModeData[3].name, LEN_FLIGHT_MODE_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}
// Phase de vol PV4 **********************
void GvarsFrame::OnTextCtrlpersoPhase4TextEnter(wxCommandEvent& event)
{
  wxString persoPhase4 = TextCtrlpersoPhase4->GetValue();
  ConvWxstrToCharFw(persoPhase4,g_model.flightModeData[4].name, LEN_FLIGHT_MODE_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}

// Phase de vol PV5 **********************
void GvarsFrame::OnTextCtrlpersoPhase5TextEnter(wxCommandEvent& event)
{
  wxString persoPhase5 = TextCtrlpersoPhase5->GetValue();
  ConvWxstrToCharFw(persoPhase5,g_model.flightModeData[5].name, LEN_FLIGHT_MODE_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}
// Fin

void GvarsFrame::OnTextCtrlpersoAllText(wxCommandEvent& event)
{
  gvarsTimer.Stop();  // Stop timer in "edit" mode
}

void GvarsFrame::OnGVARSGridCellLeftClick(wxGridEvent& event)
{
}
