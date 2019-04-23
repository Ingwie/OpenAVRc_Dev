/*
**************************************************************************
*                                                                        *
*                 ____                ___ _   _____                      *
*                / __ \___  ___ ___  / _ | | / / _ \____                 *
*               / /_/ / _ \/ -_) _ \/ __ | |/ / , _/ __/                 *
*               \____/ .__/\__/_//_/_/ |_|___/_/|_|\__/                  *
*                   /_/                                                  *
*                                                                        *
*              This file is part of the OpenAVRc project.                *
*                                                                        *
*                         Based on code(s) named :                       *
*             OpenTx - https://github.com/opentx/opentx                  *
*             Deviation - https://www.deviationtx.com/                   *
*                                                                        *
*                Only AVR code here for visibility ;-)                   *
*                                                                        *
*   OpenAVRc is free software: you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by *
*   the Free Software Foundation, either version 2 of the License, or    *
*   (at your option) any later version.                                  *
*                                                                        *
*   OpenAVRc is distributed in the hope that it will be useful,          *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
*   GNU General Public License for more details.                         *
*                                                                        *
*       License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html          *
*                                                                        *
**************************************************************************
*/


#include "OpenAVRc_SimulatorMain.h"
#include "MixerFrame.h"
#include "OutBarsFrame.h"
#include "GvarsFrame.h"
#include "RadioDataFrame.h"
#include "ModelNameDialog.h"
#include "TelemetryFrame.h"
#include "ModelNameDialog.h"

#include <wx/msgdlg.h>
#include <wx/dcclient.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>
#include <wx/log.h>
#include <wx/file.h>
#include <wx/colordlg.h>
#include <wx/chartype.h>
#include <wx/aboutdlg.h>
#include <wx/filefn.h>
#include <wx/busyinfo.h>




//(*InternalHeaders(OpenAVRc_SimulatorFrame)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)


#include "woodmain.xpm"
#include "woodH.xpm"
#include "PanelB.xpm"


//helper functions
enum wxbuildinfoformat {
  short_f, long_f
};

wxString wxbuildinfo(wxbuildinfoformat format)
{
  wxString wxbuild(wxVERSION_STRING);

  if (format == long_f ) {
#if defined(__WXMSW__)
    wxbuild << _T("-Windows");
#elif defined(__UNIX__)
    wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
    wxbuild << _T("-Unicode build");
#else
    wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
  }

  return wxbuild;
}

bool Ini_Changed = false;
wxString AppPath;

MixerFrame *MixFr;
OutBarsFrame *BarFr;
GvarsFrame *GvFr;
RadioDataFrame *RaFr;
TelemetryFrame *TeleFr;


int comwaitcounter;
// Voice
bool Mp3RepExist = false;
extern volatile uint8_t JQ6500_InputIndex;
extern uint8_t JQ6500_PlayIndex;
extern uint8_t JQ6500_playlist[];

//Audio
uint32_t BeepFreq;
uint32_t BeepTime;

// Telemetry datas
int Tele_Protocol;

float Tele_GPSLat;
float Tele_GPSLong;
float Tele_GPSAlt;
float Tele_GPSSpeed;
float Tele_GPSCourse;
int Tele_GPSYear;
int Tele_GPSMonth;
int Tele_GPSDay;
int Tele_GPSHour;
int Tele_GPSMinute;
int Tele_GPSSecond;

float Tele_AccelX;
float Tele_AccelY;
float Tele_AccelZ;

float Tele_VarioAlt;
float Tele_VarioAccelH;

int Tele_RSSITx;
int Tele_RSSIRx;

int Tele_Analog1;
int Tele_Analog2;
int Tele_Analog3;
int Tele_Analog4;

float Tele_BattVolt;
float Tele_BattCurrent;

float Tele_RPM;
float Tele_Fuel;
float Tele_Temp1;
float Tele_Temp2;
float Tele_Airspeed;

float Tele_Cell1;
float Tele_Cell2;
float Tele_Cell3;
float Tele_Cell4;
float Tele_Cell5;
float Tele_Cell6;
float Tele_Cell7;
float Tele_Cell8;
float Tele_Cell9;
float Tele_Cell10;
float Tele_Cell11;
float Tele_Cell12;

bool SimuComIsValid;
bool showeditmodeldialog = 0;

int R_mid;
int L_mid;
bool Lspringactive = false;
bool Rspringactive = false;
//(*IdInit(OpenAVRc_SimulatorFrame)
const long OpenAVRc_SimulatorFrame::ID_PANELH = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_POT1 = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_POT2 = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_POT3 = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BPMENU = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BPH = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BPEXIT = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_LLTRIM = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_LUTRIM = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_LDTRIM = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_RDTRIM = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_RUTRIM = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_RLTRIM = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_RRTRIM = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_LRTRIM = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BPG = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BPB = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BPD = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_RSTICK = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_SIMULCD = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BPTHR = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BPRUD = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BPELE = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BPTRN = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BPAIL = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BPGEA = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_PBID1 = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BOID2 = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_LSTICK = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_SPINREA = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_SPINREB = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_TEXTCTRLDUMMY = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BPREA = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BPREB = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_PANELMAIN = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_ONTGLBUTTON = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BUTTONSTARTDESKTOP = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_CHECKBOXA7105 = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_CHECKBOXNRF24L01 = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_CHECKBOXCC2500 = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_CHECKBOXCYRF6936 = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_CHECKBOXMULTIMOD = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_CHECKBOXDSMSER = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_STATICTEXT1 = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_STATICTEXTEEPATH = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_PANELL = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_PANELPRINCIPAL = wxNewId();
const long OpenAVRc_SimulatorFrame::IdMenuOpenEE = wxNewId();
const long OpenAVRc_SimulatorFrame::IdMenuSaveEE = wxNewId();
const long OpenAVRc_SimulatorFrame::IDMENUEXPORTEEPROM = wxNewId();
const long OpenAVRc_SimulatorFrame::IDMENUIMPORTEEPROM = wxNewId();
const long OpenAVRc_SimulatorFrame::idMenuQuit = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_LCDB = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_LCDF = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BUTOFF = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BUTON = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_STICKB = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_STICKF = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_COLOURS = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_MENUITEMOUTPUTMIXER = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_MENUITEMOUTPUTOUTPUT = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_MENUITEMOUTPUTGVARS = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_MENUITEMRADIODATA = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_MENUITEMTELEMETRY = wxNewId();
const long OpenAVRc_SimulatorFrame::idMenuAbout = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_STATUSBAR = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_TIMER10MS = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_TIMERMAIN = wxNewId();
//*)

BEGIN_EVENT_TABLE(OpenAVRc_SimulatorFrame,wxFrame)
  //(*EventTable(OpenAVRc_SimulatorFrame)
  //*)
END_EVENT_TABLE()

bool wxBackgroundBitmap::ProcessEvent(wxEvent &Event)
{
  if (Event.GetEventType() == wxEVT_ERASE_BACKGROUND) {
    wxEraseEvent &EraseEvent = dynamic_cast<wxEraseEvent &>(Event);
    wxDC *DC = EraseEvent.GetDC();
    DC->DrawBitmap(Bitmap, 0, 0, false);
    return true;
  } else return Inherited::ProcessEvent(Event);
}

OpenAVRc_SimulatorFrame::OpenAVRc_SimulatorFrame(wxWindow* parent,wxWindowID id)
{
  //(*Initialize(OpenAVRc_SimulatorFrame)
  wxMenu* MenuHelp;
  wxMenuBar* MenuBar1;
  wxMenuItem* MenuAbout;
  wxMenuItem* MenuItem1;

  Create(parent, wxID_ANY, _("Simulateur V3"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxSUNKEN_BORDER|wxRAISED_BORDER, _T("wxID_ANY"));
  SetClientSize(wxSize(787,415));
  Move(wxPoint(25,25));
  SetMaxSize(wxSize(-1,-1));
  PanelPrincipal = new wxPanel(this, ID_PANELPRINCIPAL, wxPoint(424,216), wxSize(777,400), wxRAISED_BORDER, _T("ID_PANELPRINCIPAL"));
  PanelH = new wxPanel(PanelPrincipal, ID_PANELH, wxPoint(0,0), wxSize(784,64), wxDOUBLE_BORDER, _T("ID_PANELH"));
  PanelMain = new wxPanel(PanelPrincipal, ID_PANELMAIN, wxPoint(0,64), wxSize(784,248), wxDOUBLE_BORDER, _T("ID_PANELMAIN"));
  Pot1 = new wxSlider(PanelMain, ID_POT1, 1024, 0, 2048, wxPoint(352,160), wxSize(14,78), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_POT1"));
  Pot1->SetBackgroundColour(wxColour(128,64,0));
  Pot2 = new wxSlider(PanelMain, ID_POT2, 1024, 0, 2048, wxPoint(380,160), wxSize(14,78), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_POT2"));
  Pot2->SetBackgroundColour(wxColour(128,64,0));
  Pot3 = new wxSlider(PanelMain, ID_POT3, 1024, 0, 2048, wxPoint(408,160), wxSize(14,78), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_POT3"));
  Pot3->SetBackgroundColour(wxColour(128,64,0));
  BPmenu = new wxPanel(PanelMain, ID_BPMENU, wxPoint(440,170), wxSize(50,20), wxDOUBLE_BORDER, _T("ID_BPMENU"));
  BPmenu->SetBackgroundColour(wxColour(0,0,0));
  BPmenu->SetToolTip(_("MENU"));
  BPh = new wxPanel(PanelMain, ID_BPH, wxPoint(288,160), wxSize(25,25), wxDOUBLE_BORDER, _T("ID_BPH"));
  BPh->SetBackgroundColour(wxColour(0,0,0));
  BPexit = new wxPanel(PanelMain, ID_BPEXIT, wxPoint(440,200), wxSize(50,20), wxDOUBLE_BORDER, _T("ID_BPEXIT"));
  BPexit->SetBackgroundColour(wxColour(0,0,0));
  BPexit->SetToolTip(_("EXIT"));
  LlTrim = new wxPanel(PanelMain, ID_LLTRIM, wxPoint(102,200), wxSize(25,12), wxDOUBLE_BORDER, _T("ID_LLTRIM"));
  LlTrim->SetBackgroundColour(wxColour(0,0,0));
  LuTrim = new wxPanel(PanelMain, ID_LUTRIM, wxPoint(224,72), wxSize(12,25), wxDOUBLE_BORDER, _T("ID_LUTRIM"));
  LuTrim->SetBackgroundColour(wxColour(0,0,0));
  LdTrim = new wxPanel(PanelMain, ID_LDTRIM, wxPoint(224,128), wxSize(12,25), wxDOUBLE_BORDER, _T("ID_LDTRIM"));
  LdTrim->SetBackgroundColour(wxColour(0,0,0));
  RdTrim = new wxPanel(PanelMain, ID_RDTRIM, wxPoint(536,128), wxSize(12,25), wxDOUBLE_BORDER, _T("ID_RDTRIM"));
  RdTrim->SetBackgroundColour(wxColour(0,0,0));
  RuTrim = new wxPanel(PanelMain, ID_RUTRIM, wxPoint(536,72), wxSize(12,25), wxDOUBLE_BORDER, _T("ID_RUTRIM"));
  RuTrim->SetBackgroundColour(wxColour(0,0,0));
  RlTrim = new wxPanel(PanelMain, ID_RLTRIM, wxPoint(592,200), wxSize(25,12), wxDOUBLE_BORDER, _T("ID_RLTRIM"));
  RlTrim->SetBackgroundColour(wxColour(0,0,0));
  RrTrim = new wxPanel(PanelMain, ID_RRTRIM, wxPoint(644,200), wxSize(25,12), wxDOUBLE_BORDER, _T("ID_RRTRIM"));
  RrTrim->SetBackgroundColour(wxColour(0,0,0));
  LrTrim = new wxPanel(PanelMain, ID_LRTRIM, wxPoint(152,200), wxSize(25,12), wxDOUBLE_BORDER, _T("ID_LRTRIM"));
  LrTrim->SetBackgroundColour(wxColour(0,0,0));
  BPg = new wxPanel(PanelMain, ID_BPG, wxPoint(264,184), wxSize(25,25), wxDOUBLE_BORDER, _T("ID_BPG"));
  BPg->SetBackgroundColour(wxColour(0,0,0));
  BPb = new wxPanel(PanelMain, ID_BPB, wxPoint(288,208), wxSize(25,25), wxDOUBLE_BORDER, _T("ID_BPB"));
  BPb->SetBackgroundColour(wxColour(0,0,0));
  BPd = new wxPanel(PanelMain, ID_BPD, wxPoint(312,184), wxSize(25,25), wxDOUBLE_BORDER, _T("ID_BPD"));
  BPd->SetBackgroundColour(wxColour(0,0,0));
  Rstick = new wxPanel(PanelMain, ID_RSTICK, wxPoint(552,32), wxSize(158,158), wxDOUBLE_BORDER, _T("ID_RSTICK"));
  Rstick->SetForegroundColour(wxColour(255,0,0));
  Rstick->SetBackgroundColour(wxColour(0,0,0));
  Simulcd = new wxPanel(PanelMain, ID_SIMULCD, wxPoint(256,20), wxSize(260,132), wxNO_BORDER, _T("ID_SIMULCD"));
  Simulcd->SetBackgroundColour(wxColour(120,210,30));
  BpThr = new wxPanel(PanelMain, ID_BPTHR, wxPoint(25,48), wxSize(25,25), wxDOUBLE_BORDER, _T("ID_BPTHR"));
  BpThr->SetBackgroundColour(wxColour(0,0,0));
  BpThr->SetToolTip(_("THR"));
  BpRud = new wxPanel(PanelMain, ID_BPRUD, wxPoint(25,88), wxSize(25,25), wxDOUBLE_BORDER, _T("ID_BPRUD"));
  BpRud->SetBackgroundColour(wxColour(0,0,0));
  BpRud->SetToolTip(_("RUD"));
  BpEle = new wxPanel(PanelMain, ID_BPELE, wxPoint(25,128), wxSize(25,25), wxDOUBLE_BORDER, _T("ID_BPELE"));
  BpEle->SetBackgroundColour(wxColour(0,0,0));
  BpEle->SetToolTip(_("ELE"));
  BpTrn = new wxPanel(PanelMain, ID_BPTRN, wxPoint(16,208), wxSize(25,25), wxDOUBLE_BORDER, _T("ID_BPTRN"));
  BpTrn->SetBackgroundColour(wxColour(0,0,0));
  BpTrn->SetToolTip(_("TRN"));
  BpAil = new wxPanel(PanelMain, ID_BPAIL, wxPoint(720,48), wxSize(25,25), wxDOUBLE_BORDER, _T("ID_BPAIL"));
  BpAil->SetBackgroundColour(wxColour(0,0,0));
  BpAil->SetToolTip(_("AIL"));
  BpGea = new wxPanel(PanelMain, ID_BPGEA, wxPoint(720,88), wxSize(25,25), wxDOUBLE_BORDER, _T("ID_BPGEA"));
  BpGea->SetBackgroundColour(wxColour(0,0,0));
  BpGea->SetToolTip(_("GEA"));
  BpId1 = new wxPanel(PanelMain, ID_PBID1, wxPoint(720,128), wxSize(25,20), wxDOUBLE_BORDER, _T("ID_PBID1"));
  BpId1->SetBackgroundColour(wxColour(0,0,0));
  BpId1->SetToolTip(_("ID1"));
  BpId2 = new wxPanel(PanelMain, ID_BOID2, wxPoint(720,148), wxSize(25,20), wxDOUBLE_BORDER, _T("ID_BOID2"));
  BpId2->SetBackgroundColour(wxColour(0,0,0));
  BpId2->SetToolTip(_("ID2"));
  Lstick = new wxPanel(PanelMain, ID_LSTICK, wxPoint(60,32), wxSize(158,158), wxDOUBLE_BORDER, _T("ID_LSTICK"));
  Lstick->SetForegroundColour(wxColour(255,0,0));
  Lstick->SetBackgroundColour(wxColour(0,0,0));
  SpinRea = new wxSpinButton(PanelMain, ID_SPINREA, wxPoint(14,14), wxDefaultSize, wxSP_HORIZONTAL|wxSP_WRAP|wxSTATIC_BORDER, _T("ID_SPINREA"));
  SpinRea->SetRange(-100, 100);
  SpinRea->SetToolTip(_("Rea"));
  SpinReb = new wxSpinButton(PanelMain, ID_SPINREB, wxPoint(732,14), wxDefaultSize, wxSP_HORIZONTAL|wxSP_WRAP|wxSTATIC_BORDER, _T("ID_SPINREB"));
  SpinReb->SetRange(-100, 100);
  SpinReb->SetToolTip(_("Reb"));
  TextCtrlgetkbinput = new wxTextCtrl(PanelMain, ID_TEXTCTRLDUMMY, wxEmptyString, wxPoint(64,216), wxSize(0,0), 0, wxDefaultValidator, _T("ID_TEXTCTRLDUMMY"));
  BpRea = new wxPanel(PanelMain, ID_BPREA, wxPoint(14,2), wxSize(35,12), wxDOUBLE_BORDER, _T("ID_BPREA"));
  BpRea->SetBackgroundColour(wxColour(0,0,0));
  BpRea->SetToolTip(_("BP REA"));
  BpReb = new wxPanel(PanelMain, ID_BPREB, wxPoint(732,2), wxSize(35,12), wxDOUBLE_BORDER, _T("ID_BPREB"));
  BpReb->SetBackgroundColour(wxColour(0,0,0));
  BpReb->SetToolTip(_("BP REB"));
  PanelL = new wxPanel(PanelPrincipal, ID_PANELL, wxPoint(0,305), wxSize(784,64), wxDOUBLE_BORDER, _T("ID_PANELL"));
  OnTglButton = new wxToggleButton(PanelL, ID_ONTGLBUTTON, _("ON"), wxPoint(8,8), wxSize(62,22), wxDOUBLE_BORDER, wxDefaultValidator, _T("ID_ONTGLBUTTON"));
  ButtonStartDesktop = new wxButton(PanelL, ID_BUTTONSTARTDESKTOP, _("Desktop"), wxPoint(8,32), wxSize(62,22), wxSIMPLE_BORDER, wxDefaultValidator, _T("ID_BUTTONSTARTDESKTOP"));
  CheckBoxA7105 = new wxCheckBox(PanelL, ID_CHECKBOXA7105, _("A7105"), wxPoint(704,24), wxSize(70,13), 0, wxDefaultValidator, _T("ID_CHECKBOXA7105"));
  CheckBoxA7105->SetValue(true);
  CheckBoxA7105->Disable();
  CheckBoxA7105->SetBackgroundColour(wxColour(192,192,192));
  CheckBoxNRF24L01 = new wxCheckBox(PanelL, ID_CHECKBOXNRF24L01, _("NRF24L01"), wxPoint(704,40), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOXNRF24L01"));
  CheckBoxNRF24L01->SetValue(true);
  CheckBoxNRF24L01->Disable();
  CheckBoxNRF24L01->SetBackgroundColour(wxColour(192,192,192));
  CheckBoxCC2500 = new wxCheckBox(PanelL, ID_CHECKBOXCC2500, _("CC2500"), wxPoint(632,24), wxSize(70,13), 0, wxDefaultValidator, _T("ID_CHECKBOXCC2500"));
  CheckBoxCC2500->SetValue(true);
  CheckBoxCC2500->Disable();
  CheckBoxCC2500->SetBackgroundColour(wxColour(192,192,192));
  CheckBoxCYRF6936 = new wxCheckBox(PanelL, ID_CHECKBOXCYRF6936, _("CYRF6936"), wxPoint(632,40), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOXCYRF6936"));
  CheckBoxCYRF6936->SetValue(true);
  CheckBoxCYRF6936->Disable();
  CheckBoxCYRF6936->SetBackgroundColour(wxColour(192,192,192));
  CheckBoxMULTIMOD = new wxCheckBox(PanelL, ID_CHECKBOXMULTIMOD, _("MULTIMOD"), wxPoint(552,40), wxSize(78,13), 0, wxDefaultValidator, _T("ID_CHECKBOXMULTIMOD"));
  CheckBoxMULTIMOD->SetValue(true);
  CheckBoxMULTIMOD->Disable();
  CheckBoxMULTIMOD->SetBackgroundColour(wxColour(192,192,192));
  CheckBoxDSMSER = new wxCheckBox(PanelL, ID_CHECKBOXDSMSER, _("DSM SER."), wxPoint(552,24), wxSize(78,13), 0, wxDefaultValidator, _T("ID_CHECKBOXDSMSER"));
  CheckBoxDSMSER->SetValue(true);
  CheckBoxDSMSER->Disable();
  CheckBoxDSMSER->SetBackgroundColour(wxColour(192,192,192));
  StaticTextProtocols = new wxStaticText(PanelL, ID_STATICTEXT1, _("Protocoles Activés"), wxPoint(624,8), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
  StaticTextProtocols->SetBackgroundColour(wxColour(192,192,192));
  StaticTextEepromPath = new wxStaticText(PanelL, ID_STATICTEXTEEPATH, wxEmptyString, wxPoint(184,10), wxSize(2,13), 0, _T("ID_STATICTEXTEEPATH"));
  StaticTextEepromPath->SetBackgroundColour(wxColour(192,192,192));
  MenuBar1 = new wxMenuBar();
  MenuFile = new wxMenu();
  MenuLoadee = new wxMenuItem(MenuFile, IdMenuOpenEE, _("Charger Eeprom"), _("Charger fichier BIN"), wxITEM_NORMAL);
  MenuLoadee->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_FORWARD")),wxART_MENU));
  MenuFile->Append(MenuLoadee);
  MenuSaveee = new wxMenuItem(MenuFile, IdMenuSaveEE, _("Sauver Eeprom"), _("Sauvegarde du fichier BIN"), wxITEM_NORMAL);
  MenuSaveee->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_BACK")),wxART_MENU));
  MenuFile->Append(MenuSaveee);
  MenuExportEeprom = new wxMenuItem(MenuFile, IDMENUEXPORTEEPROM, _("Exporter Eeprom"), _("Exporte l\'eeprom en format texte"), wxITEM_NORMAL);
  MenuExportEeprom->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_DOWN")),wxART_MENU));
  MenuFile->Append(MenuExportEeprom);
  MenuExportEeprom->Enable(false);
  MenuImportEeprom = new wxMenuItem(MenuFile, IDMENUIMPORTEEPROM, _("Importer Eeprom"), _("Importe une eeprom en format texte"), wxITEM_NORMAL);
  MenuImportEeprom->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_UP")),wxART_MENU));
  MenuFile->Append(MenuImportEeprom);
  MenuImportEeprom->Enable(false);
  MenuItem1 = new wxMenuItem(MenuFile, idMenuQuit, _("Quitter\tAlt-F4"), _("Quitter le simulateur"), wxITEM_NORMAL);
  MenuItem1->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_QUIT")),wxART_MENU));
  MenuFile->Append(MenuItem1);
  MenuBar1->Append(MenuFile, _("&Fichier"));
  MenuOption = new wxMenu();
  MenuColours = new wxMenu();
  MenuLcdBack = new wxMenuItem(MenuColours, ID_LCDB, _("Lcd (fond)"), wxEmptyString, wxITEM_NORMAL);
  MenuLcdBack->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_TICK_MARK")),wxART_MENU));
  MenuColours->Append(MenuLcdBack);
  MenuLcdPixel = new wxMenuItem(MenuColours, ID_LCDF, _("Lcd (pixel)"), wxEmptyString, wxITEM_NORMAL);
  MenuLcdPixel->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_TICK_MARK")),wxART_MENU));
  MenuColours->Append(MenuLcdPixel);
  MenuButOff = new wxMenuItem(MenuColours, ID_BUTOFF, _("Boutons (off)"), wxEmptyString, wxITEM_NORMAL);
  MenuButOff->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_TICK_MARK")),wxART_MENU));
  MenuColours->Append(MenuButOff);
  MenuButOn = new wxMenuItem(MenuColours, ID_BUTON, _("Boutons (on)"), wxEmptyString, wxITEM_NORMAL);
  MenuButOn->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_TICK_MARK")),wxART_MENU));
  MenuColours->Append(MenuButOn);
  MenuStickBack = new wxMenuItem(MenuColours, ID_STICKB, _("Manches (fond)"), wxEmptyString, wxITEM_NORMAL);
  MenuStickBack->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_TICK_MARK")),wxART_MENU));
  MenuColours->Append(MenuStickBack);
  MenuStickStick = new wxMenuItem(MenuColours, ID_STICKF, _("Manches (stick)"), wxEmptyString, wxITEM_NORMAL);
  MenuStickStick->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_TICK_MARK")),wxART_MENU));
  MenuColours->Append(MenuStickStick);
  MenuOption->Append(ID_COLOURS, _("Couleurs"), MenuColours, wxEmptyString);
  MenuBar1->Append(MenuOption, _("&Option"));
  MenuFrame = new wxMenu();
  OutputMixeur = new wxMenuItem(MenuFrame, ID_MENUITEMOUTPUTMIXER, _("Mixeur"), wxEmptyString, wxITEM_NORMAL);
  MenuFrame->Append(OutputMixeur);
  OutputBars = new wxMenuItem(MenuFrame, ID_MENUITEMOUTPUTOUTPUT, _("Graphique Sorties"), wxEmptyString, wxITEM_NORMAL);
  MenuFrame->Append(OutputBars);
  OutputGvars = new wxMenuItem(MenuFrame, ID_MENUITEMOUTPUTGVARS, _("Variables globales"), wxEmptyString, wxITEM_NORMAL);
  MenuFrame->Append(OutputGvars);
  RadioData = new wxMenuItem(MenuFrame, ID_MENUITEMRADIODATA, _("Paramètres Radio"), wxEmptyString, wxITEM_NORMAL);
  MenuFrame->Append(RadioData);
  telemetry = new wxMenuItem(MenuFrame, ID_MENUITEMTELEMETRY, _("Télémétrie"), wxEmptyString, wxITEM_NORMAL);
  MenuFrame->Append(telemetry);
  MenuBar1->Append(MenuFrame, _("&Fenêtres"));
  MenuHelp = new wxMenu();
  MenuAbout = new wxMenuItem(MenuHelp, idMenuAbout, _("A propos ...\tF1"), _("C\'est quoi donc \?"), wxITEM_NORMAL);
  MenuAbout->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_INFORMATION")),wxART_MENU));
  MenuHelp->Append(MenuAbout);
  MenuBar1->Append(MenuHelp, _("&Aide"));
  SetMenuBar(MenuBar1);
  StatusBar = new wxStatusBar(this, ID_STATUSBAR, 0, _T("ID_STATUSBAR"));
  int __wxStatusBarWidths_1[3] = { -30, -10, -10 };
  int __wxStatusBarStyles_1[3] = { wxSB_NORMAL, wxSB_RAISED, wxSB_RAISED };
  StatusBar->SetFieldsCount(3,__wxStatusBarWidths_1);
  StatusBar->SetStatusStyles(3,__wxStatusBarStyles_1);
  SetStatusBar(StatusBar);
  Timer10ms.SetOwner(this, ID_TIMER10MS);
  TimerMain.SetOwner(this, ID_TIMERMAIN);
  Center();

  BPmenu->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBPmenuLeftDown,0,this);
  BPmenu->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBPmenuLeftUp,0,this);
  BPh->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBPhLeftDown,0,this);
  BPh->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBPhLeftUp,0,this);
  BPexit->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBPexitLeftDown,0,this);
  BPexit->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBPexitLeftUp,0,this);
  LlTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnLlTrimLeftDown,0,this);
  LlTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnLlTrimLeftUp,0,this);
  LuTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnLuTrimLeftDown,0,this);
  LuTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnLuTrimLeftUp,0,this);
  LdTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnLdTrimLeftDown,0,this);
  LdTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnLdTrimLeftUp,0,this);
  RdTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRdTrimLeftDown,0,this);
  RdTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRdTrimLeftUp,0,this);
  RuTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRuTrimLeftDown,0,this);
  RuTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRuTrimLeftUp,0,this);
  RlTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRlTrimLeftDown,0,this);
  RlTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRlTrimLeftUp,0,this);
  RrTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRrTrimLeftDown,0,this);
  RrTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRrTrimLeftUp,0,this);
  LrTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnLrTrimLeftDown,0,this);
  LrTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnLrTrimLeftUp,0,this);
  BPg->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBPgLeftDown,0,this);
  BPg->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBPgLeftUp,0,this);
  BPb->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBPbLeftDown,0,this);
  BPb->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBPbLeftUp,0,this);
  BPd->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBPdLeftDown,0,this);
  BPd->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBPdLeftUp,0,this);
  Rstick->Connect(wxEVT_PAINT,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRstickPaint,0,this);
  Rstick->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnPanelMainKeyDown,0,this);
  Rstick->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRstickMouseMove,0,this);
  Rstick->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRstickLeftUp,0,this);
  Rstick->Connect(wxEVT_MOTION,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRstickMouseMove,0,this);
  Rstick->Connect(wxEVT_LEAVE_WINDOW,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRstickLeftUp,0,this);
  Simulcd->Connect(wxEVT_PAINT,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnwxsimulcdPaint,0,this);
  Simulcd->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnPanelMainKeyDown,0,this);
  Simulcd->Connect(wxEVT_LEFT_DCLICK,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnSimulcdLeftDClick,0,this);
  BpThr->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBpThrLeftDown,0,this);
  BpRud->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBpRudLeftDown,0,this);
  BpEle->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBpEleLeftDown,0,this);
  BpTrn->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBpTrnLeftDown,0,this);
  BpTrn->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBpTrnLeftUp,0,this);
  BpTrn->Connect(wxEVT_RIGHT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBpTrnRightDown,0,this);
  BpAil->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBpAilLeftDown,0,this);
  BpGea->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBpGeaLeftDown,0,this);
  BpId1->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBpId1LeftDown,0,this);
  BpId2->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBpId2LeftDown,0,this);
  Lstick->Connect(wxEVT_PAINT,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnLstickPaint,0,this);
  Lstick->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnPanelMainKeyDown,0,this);
  Lstick->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnLstickMouseMove,0,this);
  Lstick->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnLstickLeftUp,0,this);
  Lstick->Connect(wxEVT_MOTION,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnLstickMouseMove,0,this);
  Lstick->Connect(wxEVT_LEAVE_WINDOW,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnLstickLeftUp,0,this);
  BpRea->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBpReaLeftDown,0,this);
  BpRea->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBpReaLeftUp,0,this);
  BpReb->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBpRebLeftDown,0,this);
  BpReb->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnBpRebLeftUp,0,this);
  PanelMain->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnPanelMainKeyDown,0,this);
  Connect(ID_ONTGLBUTTON,wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnOnTglButtonToggle);
  Connect(ID_BUTTONSTARTDESKTOP,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnButtonStartDesktopClick);
  Connect(ID_CHECKBOXA7105,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnCheckBoxProtocolsClick);
  Connect(ID_CHECKBOXNRF24L01,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnCheckBoxProtocolsClick);
  Connect(ID_CHECKBOXCC2500,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnCheckBoxProtocolsClick);
  Connect(ID_CHECKBOXCYRF6936,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnCheckBoxProtocolsClick);
  Connect(ID_CHECKBOXMULTIMOD,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnCheckBoxProtocolsClick);
  Connect(ID_CHECKBOXDSMSER,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnCheckBoxProtocolsClick);
  PanelL->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnPanelMainKeyDown,0,this);
  PanelPrincipal->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnPanelMainKeyDown,0,this);
  Connect(IdMenuOpenEE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuLoadEeprom);
  Connect(IdMenuSaveEE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuSaveeeSelected);
  Connect(IDMENUEXPORTEEPROM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuExportEepromSelected);
  Connect(IDMENUIMPORTEEPROM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuImportEepromSelected);
  Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnQuit);
  Connect(ID_LCDB,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuLcdBackSelected);
  Connect(ID_LCDF,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuLcdPixelSelected);
  Connect(ID_BUTOFF,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuButOffSelected);
  Connect(ID_BUTON,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuButOnSelected);
  Connect(ID_STICKB,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuStickBackSelected);
  Connect(ID_STICKF,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuStickStickSelected);
  Connect(ID_MENUITEMOUTPUTMIXER,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMixeurSelected);
  Connect(ID_MENUITEMOUTPUTOUTPUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnOutputBarsSelected);
  Connect(ID_MENUITEMOUTPUTGVARS,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnOutputGvarsSelected);
  Connect(ID_MENUITEMRADIODATA,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRadioDataSelected);
  Connect(ID_MENUITEMTELEMETRY,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OntelemetrySelected);
  Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnAbout);
  Connect(ID_TIMER10MS,wxEVT_TIMER,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnTimer10msTrigger);
  Connect(ID_TIMERMAIN,wxEVT_TIMER,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnTimerMainTrigger);
  Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnClose);
  //*)
  {
    wxIcon FrameIcon;
    SetIcon(wxICON(oavrc_icon));
  }

  //App Path
  wxFileName appPathWithExeName = wxStandardPaths::Get().GetExecutablePath();
  AppPath = (appPathWithExeName.GetPath());
  //Ini File
  Ini_Filename = wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator() + "OpenAVRcSimulatorV3.ini";
  configFile = new wxFileConfig( "", "", Ini_Filename);
  Col_Lcd_Back = *wxGREEN;
  Col_Lcd_Front = *wxBLACK;
  Col_Button_Off = *wxBLACK;
  Col_Button_On = *wxWHITE;
  Col_Stick_Back = *wxBLACK;
  Col_Stick_Circle = *wxWHITE;

  R_mid = (Rstick->GetSize().GetWidth())/2;
  L_mid = (Lstick->GetSize().GetWidth())/2;

  CurrentEEPath = "";

  LoadConfig();
  Lstick->SetForegroundColour(Col_Stick_Circle);
  Rstick->SetForegroundColour(Col_Stick_Circle);
  Lstick->SetBackgroundColour(Col_Stick_Back);
  Rstick->SetBackgroundColour(Col_Stick_Back);

  //LCD var
  SimuLcdScale = 2;

  // The Lcd bitmap
  SimuLcd_ClientDC = new wxClientDC(Simulcd);
  SimuLcd_Bitmap = wxBitmap(Simulcd->GetClientSize().GetWidth(), Simulcd->GetClientSize().GetHeight());
  SimuLcd_MemoryDC = new wxMemoryDC(SimuLcd_Bitmap);

  // Task mesurement
  ChronoMain = new wxStopWatch;
  Chrono10ms = new wxStopWatch;


  //Virtual pin
  SpinA = new Spin("A", &simu_pina, &simu_ddra, &simu_porta);
  SpinB = new Spin("B", &simu_pinb, &simu_ddrb, &simu_portb);
  SpinC = new Spin("C", &simu_pinc, &simu_ddrc, &simu_portc);
  SpinD = new Spin("D", &simu_pind, &simu_ddrd, &simu_portd);
  SpinE = new Spin("E", &simu_pine, &simu_ddre, &simu_porte);
  SpinF = new Spin("F", &simu_pinf, &simu_ddrf, &simu_portf);
  SpinG = new Spin("G", &simu_ping, &simu_ddrg, &simu_portg);
  SpinH = new Spin("H", &simu_pinh, &simu_ddrh, &simu_porth);
  SpinJ = new Spin("J", &simu_pinj, &simu_ddrj, &simu_portj);
  SpinK = new Spin("K", &simu_pink, &simu_ddrk, &simu_portk);
  SpinL = new Spin("L", &simu_pinl, &simu_ddrl, &simu_portl);

  //wood & inox panel paint
  PanelMain->PushEventHandler(new wxBackgroundBitmap(woodmain));
  PanelH->PushEventHandler(new wxBackgroundBitmap(woodH));
  PanelL->PushEventHandler(new wxBackgroundBitmap(PanelB));

  //Test if MP3 exist
  wxString Filename = AppPath + "\\VOICEMP3\\0000.mp3";
  if(wxFileExists(Filename)) Mp3RepExist = true;

  //Telemetry
  comPort = new Tserial();
  SimuComIsValid = false;
  frskySportSimuSetup();

}

//// FW Functions ///////////////////////////////////////////////////

void OpenAVRc_SimulatorFrame::OnOnTglButtonToggle(wxCommandEvent& event)
{
  event.Skip();
  int answer;
  if (OnTglButton->GetValue()) {
    if (simu_eeprom[1] == 0) {
      if (CurrentEEPath != "") {
        answer = wxMessageBox((_("Recharger le fichier eeprom ") +CurrentEEPath+" ?"), _("EEPROM VIDE"), wxYES_NO, this);
        if (answer == wxYES) {
          LoadEepromFile(CurrentEEPath);
        }
      }
      if ((CurrentEEPath == "") || (answer == wxNO)) {
        answer = wxMessageBox(_("Charger un fichier eeprom ?"), _("EEPROM VIDE"), wxYES_NO, this);
        if (answer == wxYES) LoadEeprom();
      }
    }
    StartFirmwareCode();
  }
  else
  {
    pwrCheck = false; // Shut down firmware command (Pin simulation)
  }
}

void OpenAVRc_SimulatorFrame::StartFirmwareCode()
{
  simu_off = false;
  simu_mainloop_is_runing = false;
  ISR10msLoop_is_runing = false;
  simu_shutDownSimu_is_runing = false;
  boardInit(); // Is called by simumain but needed here to Spin init
  //Init virtual PORTS and PINS
  SpinA->init();
  SpinB->init();
  SpinC->init();
  SpinD->init();
  SpinE->init();
  SpinF->init();
  SpinG->init();
  SpinH->init();
  SpinJ->init();
  SpinK->init();
  SpinL->init();
#if !defined(PCBMEGAMINI)
  SpinH->SetPin(6); //Rf activated pin
  SpinH->ResetPin(5); //Dsc inactivated
#endif
  SpinC->ResetPin(0); // Set 3pos at ID0
  s_anaFilt[0] = 1024;
  s_anaFilt[1] = 1024;
  s_anaFilt[2] = 1024;
  s_anaFilt[3] = 1024;

  s_anaFilt[7] = 1024; // 7.62 V Battery (7.22V adc + 0.4 V Schottky Diode)

  Timer10ms.StartOnce(10); //Simulate 10mS Interrupt vector
  simumain();
  CheckActiveProtocols();
  TimerMain.StartOnce(18); // Simulate ?mS cycle for mainloop function
  MenuFile->Enable(IDMENUEXPORTEEPROM, true);
  MenuFile->Enable(IDMENUIMPORTEEPROM, true);
}

void OpenAVRc_SimulatorFrame::ResetSimuLcd()
{
  for (uint32_t i=0; i<(sizeof(displayBuf)); ++i) displayBuf[i] = 0xFF;
  DrawWxSimuLcd();
}

void OpenAVRc_SimulatorFrame::OnTimerMainTrigger(wxTimerEvent& event) //1mS
{
  event.Skip();

  if (simu_mainloop_is_runing) // Avoid re-entrance
    {
      TimerMain.StartOnce(1); //whait 1 mS
      return;
    }
  else
    {
      if (showeditmodeldialog) // Edit model name in main thread
        {
          ModelNameDialog *MoDi = new  ModelNameDialog(NULL); // In ModelNameDialog.cpp
          MoDi->ShowModal();
          MoDi->Destroy();
          showeditmodeldialog = 0;
        }
      MainFirmwareTask();
    }
}

void OpenAVRc_SimulatorFrame::MainFirmwareTask()
{
  if ((simu_off) && (!simu_mainloop_is_runing))
  {
    TimerMain.Stop();
    ChronoMain->Pause();
    Timer10ms.Stop();
    Chrono10ms->Pause();
    ResetSimuLcd();
    CloseApp();
  }
  else
  {
    if ((!simu_mainloop_is_runing) && (!simu_shutDownSimu_is_runing))
    {
      if (Tele_Protocol == Tele_Proto_Frsky_Sport) // Telemetry simulation
        frskySportSimuloop();
      if ((Tele_Protocol == Tele_Proto_Frsky_D) & !IS_USR_PROTO_SMART_PORT()) // Telemetry simulation
        frskyDSimuloop();
      StatusBar->SetStatusText(_T("MAIN ")+MaintTaskChronoval.ToString()+_T(" uS"),1);
      TimerMain.StartOnce(18);
      MainFWThread = new MainFirmwareThread;
    }
  }
}

void OpenAVRc_SimulatorFrame::OnTimer10msTrigger(wxTimerEvent& event)
{
  event.Skip();

  for(int i=0 ; i<4; ++i) // spring simulate
    {
      if (Lspringactive) PaintSticks(true,L_mid,L_mid,Lstick);
      if (Rspringactive) PaintSticks(true,R_mid,R_mid,Rstick);
    }

  for (int i=0; i < 2; i++)
    {
      if (eeprom_buffer_size)
        {
          EE_READY_vect(); // simulate eeprom write ISR
        }
    }


  if (ISR10msLoop_is_runing) // Avoid re-entrance
  {
    Timer10ms.StartOnce(1); //whait 1 mS
    return;
  }
  else Isr10msTaskFirmware();
}

void OpenAVRc_SimulatorFrame::Isr10msTaskFirmware()
{
  if (Mp3RepExist) PlayTts(); // Check and play voice if needed

  if ((BeepFreq != 0) || (BeepTime != 0)) // A beep to play ?
    {
      BeepFWThread = new BeepThread(BeepFreq, BeepTime);
      BeepFreq = 0;
      BeepTime = 0;
    }

  CheckInputs();

  if (!simu_off)
    {
      StatusBar->SetStatusText(_T("10 mS IRQ ")+Isr10msTaskChronoval.ToString()+_T(" uS"),2);
      Isr10msFWThread = new Isr10msFirmwareThread;
    }
  else
    {
    }

  Timer10ms.StartOnce(10); //Simulate 10mS Interrupt vector
}

void OpenAVRc_SimulatorFrame::EditModelName()
{
  showeditmodeldialog = 1;
}

void OpenAVRc_SimulatorFrame::DrawWxSimuLcd()
{
  uint8_t *p;
  p = displayBuf;
  wxColor Col_Lcd_Back_dyn = Col_Lcd_Back;
  if (!(isBacklightEnable())) {
    Col_Lcd_Back_dyn = Col_Lcd_Back_dyn.ChangeLightness(80);
  }
  wxBrush brush_back(Col_Lcd_Back_dyn, wxBRUSHSTYLE_SOLID );
  SimuLcd_MemoryDC->SetBrush(brush_back);
  SimuLcd_MemoryDC->DrawRectangle(0,0,4+LCD_W*SimuLcdScale,4+LCD_H*SimuLcdScale);
  wxBrush brush_top(Col_Lcd_Front, wxBRUSHSTYLE_SOLID );
  wxPen pen_top(Col_Lcd_Front,1,wxPENSTYLE_SOLID);
  SimuLcd_MemoryDC->SetPen(pen_top);
  SimuLcd_MemoryDC->SetBrush(brush_top);

  for (uint8_t y=0; y < (LCD_H / 8); y++) {
    for (uint8_t x=0; x < LCD_W; x++) {
      uint8_t bit = *p;
      p++;
      for (uint8_t i=0; i < 8; i++) {
        if (bit & 0x01) SimuLcd_MemoryDC->DrawRectangle(2+ x*SimuLcdScale,2+ (y*8*SimuLcdScale) +(i*SimuLcdScale),SimuLcdScale,SimuLcdScale);
        bit >>= 1;
      }
    }
  }

  SimuLcd_ClientDC->Blit(0,0,SimuLcd_Bitmap.GetWidth(),SimuLcd_Bitmap.GetHeight(),SimuLcd_MemoryDC,0,0);
}


//// GUI Functions ///////////////////////////////////////////////////

OpenAVRc_SimulatorFrame::~OpenAVRc_SimulatorFrame()
{
  //(*Destroy(OpenAVRc_SimulatorFrame)
  //*)
}

void OpenAVRc_SimulatorFrame::OnAbout(wxCommandEvent& event)
{
  event.Skip();
  wxAboutDialogInfo Aboutbox;
  Aboutbox.SetName(_("OpenAVRc Simulateur"));
  Aboutbox.SetVersion(_("V 3.0 Beta"));
  Aboutbox.SetLicence(_(" GPLv2 . Firmware basé sur NextStepRc 2.18 "));
  Aboutbox.SetDescription(_("Simulateur du code OpenAVRc 'toutes options' sur carte Méga 2560     "));
  Aboutbox.SetCopyright(wxT("(C) 2016-2019 OpenAVRc Team"));
  Aboutbox.SetWebSite(wxT("https://github.com/Ingwie/OpenAVRc_Dev"));
  Aboutbox.AddDeveloper(wxT("OpenAVRc Team :\n\n"));
  Aboutbox.AddDeveloper(wxT("Firmware : Bracame, RC-Navy, Sloped Soarer, Supiiik. \n"));
  Aboutbox.AddDeveloper(wxT("Applications : Bracame, Mentero. \n"));
  Aboutbox.AddDeveloper(wxT("PCB Shield : Anthobreizh, Pierrotm777 , Pyrall. \n"));
  Aboutbox.AddDeveloper(wxT("Documentation : JPZ(Testeur68), Pierrotm777 , Pyrall. \n"));
  Aboutbox.AddDeveloper(wxT("Translations : Mentero. \n"));
  Aboutbox.AddDeveloper(wxT("Beta tests : JPZ(Testeur68), Pyrall. \n\n"));
  Aboutbox.AddDeveloper(wxT("Forum contributors :\n\n"));
  Aboutbox.AddDeveloper(wxT("ElectoPete (English Voice file), Ievgen (Ukrainian translation). \n"));
  Aboutbox.AddDeveloper(wxT("EyesOnly (English documentation translation). \n"));

  wxAboutBox(Aboutbox);
}

void OpenAVRc_SimulatorFrame::OnwxsimulcdPaint(wxPaintEvent& event)
{
  event.Skip();
  DrawWxSimuLcd();
}

void OpenAVRc_SimulatorFrame::PlayTts()
{
  static long pid = 0;

  if (Mp3process->Exists(pid)) return;

  if (JQ6500_InputIndex != JQ6500_PlayIndex) {
    uint16_t prompt;
    prompt = JQ6500_playlist[JQ6500_PlayIndex];
    ++JQ6500_PlayIndex;
    prompt <<= 8;
    prompt |= JQ6500_playlist[JQ6500_PlayIndex];
    ++JQ6500_PlayIndex;
    if (JQ6500_PlayIndex == (24*2)) JQ6500_PlayIndex = 0; //QUEUE_LENGTH = 24*2 in JQ6500 driver
    wxString Mp3file;
    --prompt;
    Mp3file.Printf("%04d.mp3",prompt);
    Mp3process = wxProcess::Open(AppPath + "\\tools\\cmdmp3win.exe " + AppPath + "\\VOICEMP3\\" + Mp3file, wxEXEC_HIDE_CONSOLE  | wxEXEC_ASYNC);
    pid = Mp3process->GetPid();
  }
}

void OpenAVRc_SimulatorFrame::LoadEeprom()
{
  if (Timer10ms.IsRunning()) {
    wxLogError(_("Impossible : Simulateur en fonctionnement"));
    return;
  }
  wxFileDialog openFileDialog(this, _("Ouvrir Fichier BIN"), AppPath+ "\\eeprom\\", "","Fichiers BIN (*.bin)|*.bin", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() == wxID_CANCEL) return;
  wxFileInputStream input_stream(openFileDialog.GetPath());
  if (!input_stream.IsOk()) {
    wxLogError(_("Impossible d'ouvrir le fichier '%s'."), openFileDialog.GetPath());
    return;
  }
  LoadEepromFile(openFileDialog.GetPath());
  CurrentEEPath = openFileDialog.GetPath();
  Ini_Changed = true;
}

void OpenAVRc_SimulatorFrame::LoadEepromFile(wxString path)
{
  wxFile bin_file(path);
  StaticTextEepromPath->SetLabel(path);
  StaticTextEepromPath->Update();

  if(bin_file.IsOpened()) {
    for (int i=0; i<EESIZE; ++i) simu_eeprom[i] = 0x00;
    bin_file.Read(&simu_eeprom[0], EESIZE);
    bin_file.Close();
  }
}

void OpenAVRc_SimulatorFrame::OnMenuLoadEeprom(wxCommandEvent& event)
{
  event.Skip();
  LoadEeprom();
}

void OpenAVRc_SimulatorFrame::PaintSticks(bool spring, int x, int y, wxPanel* stick)
{
  static int Lx_mem = L_mid;
  static int Ly_mem = L_mid;
  static int Rx_mem = R_mid;
  static int Ry_mem = R_mid;

  if (spring)
    {
      if (stick == Lstick)
        {
          if (Lx_mem > L_mid)
            x = Lx_mem-1;
          if (Lx_mem < L_mid)
            x = Lx_mem+1;
          if (!(g_eeGeneral.stickMode & 0x01))
            {
              if (Ly_mem > L_mid)
                y = Ly_mem-1;
              if (Ly_mem < L_mid)
                y = Ly_mem+1;
            } else y = Ly_mem;
        }
      if (stick == Rstick )
        {
          if (Rx_mem > R_mid)
            x = Rx_mem-1;
          if (Rx_mem < R_mid)
            x = Rx_mem+1;
          if (g_eeGeneral.stickMode & 0x01)
            {
              if (Ry_mem > R_mid)
                y = Ry_mem-1;
              if (Ry_mem < R_mid)
                y = Ry_mem+1;
            } else y = Ry_mem;
        }
    }

  wxBrush brush_Stick(Col_Stick_Circle, wxBRUSHSTYLE_SOLID );
  wxBrush brush_Back(Col_Stick_Back, wxBRUSHSTYLE_SOLID );
  wxPen pen_Back(Col_Stick_Back,1,wxPENSTYLE_SOLID);
  wxPen pen_Stick(Col_Stick_Circle,1,wxPENSTYLE_SOLID);
  wxClientDC dc(stick);
  dc.SetBrush(brush_Back);
  dc.SetPen(pen_Back);
  dc.DrawCircle((stick == Lstick)? Lx_mem:Rx_mem,(stick == Lstick)? Ly_mem:Ry_mem,8);
  dc.SetPen(pen_Stick);
  dc.SetBrush(brush_Stick);
  dc.DrawCircle(x,y,8);

  int xmul = 2048000 / (stick->GetSize().GetWidth() - 5);
  int ymul = 2048000 / (stick->GetSize().GetHeight() -5);
  int xx = (x * xmul)/1000;
  int yy = 2048 - (y * ymul)/1000;

  if (stick == Lstick)
    {
      Lx_mem = x;
      Ly_mem = y;
      s_anaFilt[3] = (uint16_t)xx; //LH
      s_anaFilt[1] = (uint16_t)yy; //LV
    }
  if (stick == Rstick)
    {
      Rx_mem = x;
      Ry_mem = y;
      s_anaFilt[0] = (uint16_t)xx; //RH
      s_anaFilt[2] = (uint16_t)yy; //RV
    }
}

void OpenAVRc_SimulatorFrame::OnLstickMouseMove(wxMouseEvent& event)
{
  event.Skip();
  Lspringactive = false;
  wxPoint pt(event.GetPosition());

  if (event.LeftIsDown() || event.RightIsDown()) {
    PaintSticks(false, pt.x, pt.y, Lstick);
  };
}

void OpenAVRc_SimulatorFrame::OnLstickLeftUp(wxMouseEvent& event)
{
  Lspringactive = true;
}

void OpenAVRc_SimulatorFrame::OnRstickMouseMove(wxMouseEvent& event)
{
  event.Skip();
  Rspringactive = false;
  wxPoint pt(event.GetPosition());

  if (event.LeftIsDown() || event.RightIsDown()) {
    PaintSticks(false, pt.x, pt.y, Rstick);
  };
}

void OpenAVRc_SimulatorFrame::OnRstickLeftUp(wxMouseEvent& event)
{
  Rspringactive = true;
}

void OpenAVRc_SimulatorFrame::OnSimulcdLeftDClick(wxMouseEvent& event)
{
  event.Skip();
  wxFileDialog saveFileDialog(this, _("Sauver Capture écran"), "", "", "Fichier BMP (*.bmp)|*.bmp", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
  if (saveFileDialog.ShowModal() == wxID_CANCEL)
    return;     // the user changed idea...
  wxFileOutputStream output_stream(saveFileDialog.GetPath());
  if (!output_stream.IsOk()) {
    wxLogError(_("Ne peut écrire le fichier '%s'."), saveFileDialog.GetPath());
    return;
  }
  SimuLcd_Bitmap.SaveFile(saveFileDialog.GetPath(), wxBITMAP_TYPE_BMP, NULL);

}


void OpenAVRc_SimulatorFrame::OnMenuSaveeeSelected(wxCommandEvent& event)
{
  event.Skip();
  wxFileDialog saveFileDialog(this, _("Sauver Eeprom"), "", "", "Fichier BIN (*.bin)|*.bin", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
  if (saveFileDialog.ShowModal() == wxID_CANCEL)
    return;     // the user changed idea...
  wxFile bin_file;
  bin_file.Create(saveFileDialog.GetPath(), true);
  CurrentEEPath = saveFileDialog.GetPath();
  StaticTextEepromPath->SetLabel(CurrentEEPath);
  StaticTextEepromPath->Update();
  eeDirty(EE_GENERAL); //Save Radio eeprom immediatly
  eeCheck(true);
  eeDirty(EE_MODEL);
  eeCheck(true);

  if(bin_file.IsOpened()) {
    bin_file.Seek(0);
    bin_file.Write(&simu_eeprom[0], EESIZE);
    bin_file.Close();
  }
}

void OpenAVRc_SimulatorFrame::CloseApp()
{
  if (Ini_Changed) SaveConfig();

  while (TimerMain.IsRunning() || Timer10ms.IsRunning()) {SimuSleepMs(1);}

  TimerMain.DeletePendingEvents();
  Timer10ms.DeletePendingEvents();
  if (ChronoMain != NULL) {delete ChronoMain;}
  if (Chrono10ms != NULL) {delete Chrono10ms;}

  PanelMain->PopEventHandler(NULL);
  PanelH->PopEventHandler(NULL);
  PanelL->PopEventHandler(NULL);
  //delete PanelMainBackground;
  //delete PanelHBackckground;
  //delete PanelBBackckground;
  //delete PanelMain;
  //delete PanelH;
  //delete PanelB;

  delete SpinA;
  delete SpinB;
  delete SpinC;
  delete SpinD;
  delete SpinE;
  delete SpinF;
  delete SpinG;
  delete SpinH;
  delete SpinJ;
  delete SpinK;
  delete SpinL;

  if (comPort != NULL) delete comPort;

  if (SimuLcd_MemoryDC != NULL) delete SimuLcd_MemoryDC;
  if (SimuLcd_ClientDC != NULL)	delete SimuLcd_ClientDC;

  Destroy();
}

void OpenAVRc_SimulatorFrame::OnQuit(wxCommandEvent& event)
{
  event.Skip();
  if (OnTglButton->GetValue() && (!simu_off)) {
    pwrCheck = false; // Shut down firmware
  } else CloseApp();
}

void OpenAVRc_SimulatorFrame::OnClose(wxCloseEvent& event)
{
  event.Veto();
  if (OnTglButton->GetValue() && (!simu_off)) {
    pwrCheck = false; // Shut down firmware
  } else CloseApp();
}

void OpenAVRc_SimulatorFrame::LoadConfig()
{
  configFile->Read(wxT("Col_Lcd_Back"),&Col_Lcd_Back);
  configFile->Read(wxT("Col_Lcd_Front"),&Col_Lcd_Front);
  configFile->Read(wxT("Col_Button_Off"),&Col_Button_Off);
  configFile->Read(wxT("Col_Button_On"),&Col_Button_On);
  configFile->Read(wxT("Col_Stick_Back"),&Col_Stick_Back);
  configFile->Read(wxT("Col_Stick_Circle"),&Col_Stick_Circle);
  // Telemetry datas
  configFile->Read(wxT("Tele_Protocol"),&Tele_Protocol);
  configFile->Read(wxT("Tele_GPSLat"),&Tele_GPSLat);
  configFile->Read(wxT("Tele_GPSLong"),&Tele_GPSLong);
  configFile->Read(wxT("Tele_GPSAlt"),&Tele_GPSAlt);
  configFile->Read(wxT("Tele_GPSSpeed"),&Tele_GPSSpeed);
  configFile->Read(wxT("Tele_GPSCourse"),&Tele_GPSCourse);
  /*configFile->Read(wxT("Tele_GPSYear"),&Tele_GPSYear); Use real Date Time as default
  configFile->Read(wxT("Tele_GPSMonth"),&Tele_GPSMonth);
  configFile->Read(wxT("Tele_GPSDay"),&Tele_GPSDay);
  configFile->Read(wxT("Tele_GPSHour"),&Tele_GPSHour);
  configFile->Read(wxT("Tele_GPSMinute"),&Tele_GPSMinute);
  configFile->Read(wxT("Tele_GPSSecond"),&Tele_GPSSecond);*/

  configFile->Read(wxT("Tele_AccelX"),&Tele_AccelX);
  configFile->Read(wxT("Tele_AccelY"),&Tele_AccelY);
  configFile->Read(wxT("Tele_AccelZ"),&Tele_AccelZ);

  configFile->Read(wxT("Tele_VarioAlt"),&Tele_VarioAlt);
  configFile->Read(wxT("Tele_VarioAccelH"),&Tele_VarioAccelH);

  configFile->Read(wxT("Tele_RSSITx"),&Tele_RSSITx);
  configFile->Read(wxT("Tele_RSSIRx"),&Tele_RSSIRx);

  configFile->Read(wxT("Tele_Analog1"),&Tele_Analog1);
  configFile->Read(wxT("Tele_Analog2"),&Tele_Analog2);
  configFile->Read(wxT("Tele_Analog3"),&Tele_Analog3);
  configFile->Read(wxT("Tele_Analog4"),&Tele_Analog4);

  configFile->Read(wxT("Tele_BattVolt"),&Tele_BattVolt);
  configFile->Read(wxT("Tele_BattCurrent"),&Tele_BattCurrent);

  configFile->Read(wxT("Tele_RPM"),&Tele_RPM);
  configFile->Read(wxT("Tele_Fuel"),&Tele_Fuel);
  configFile->Read(wxT("Tele_Temp1"),&Tele_Temp1);
  configFile->Read(wxT("Tele_Temp2"),&Tele_Temp2);
  configFile->Read(wxT("Tele_Airspeed"),&Tele_Airspeed);

  configFile->Read(wxT("Tele_Cell1"),&Tele_Cell1);
  configFile->Read(wxT("Tele_Cell2"),&Tele_Cell2);
  configFile->Read(wxT("Tele_Cell3"),&Tele_Cell3);
  configFile->Read(wxT("Tele_Cell4"),&Tele_Cell4);
  configFile->Read(wxT("Tele_Cell5"),&Tele_Cell5);
  configFile->Read(wxT("Tele_Cell6"),&Tele_Cell6);
  configFile->Read(wxT("Tele_Cell7"),&Tele_Cell7);
  configFile->Read(wxT("Tele_Cell8"),&Tele_Cell8);
  configFile->Read(wxT("Tele_Cell9"),&Tele_Cell9);
  configFile->Read(wxT("Tele_Cell10"),&Tele_Cell10);
  configFile->Read(wxT("Tele_Cell11"),&Tele_Cell11);
  configFile->Read(wxT("Tele_Cell12"),&Tele_Cell12);

  configFile->Read(wxT("EEfile"),&CurrentEEPath);
}

void OpenAVRc_SimulatorFrame::SaveConfig()
{
  //wxMessageBox( Ini_Filename, _("Les paramètres sont sauvé dans :"));

  configFile->Write(wxT("Col_Lcd_Back"),Col_Lcd_Back);
  configFile->Write(wxT("Col_Lcd_Front"),Col_Lcd_Front);
  configFile->Write(wxT("Col_Button_Off"),Col_Button_Off);
  configFile->Write(wxT("Col_Button_On"),Col_Button_On);
  configFile->Write(wxT("Col_Stick_Back"),Col_Stick_Back);
  configFile->Write(wxT("Col_Stick_Circle"),Col_Stick_Circle);
  // Telemetry datas
  configFile->Write(wxT("Tele_Protocol"),Tele_Protocol);
  configFile->Write(wxT("Tele_GPSLat"),Tele_GPSLat);
  configFile->Write(wxT("Tele_GPSLong"),Tele_GPSLong);
  configFile->Write(wxT("Tele_GPSAlt"),Tele_GPSAlt);
  configFile->Write(wxT("Tele_GPSSpeed"),Tele_GPSSpeed);
  configFile->Write(wxT("Tele_GPSCourse"),Tele_GPSCourse);
  /*configFile->Write(wxT("Tele_GPSYear"),Tele_GPSYear);
  configFile->Write(wxT("Tele_GPSMonth"),Tele_GPSMonth);
  configFile->Write(wxT("Tele_GPSDay"),Tele_GPSDay);
  configFile->Write(wxT("Tele_GPSHour"),Tele_GPSHour);
  configFile->Write(wxT("Tele_GPSMinute"),Tele_GPSMinute);
  configFile->Write(wxT("Tele_GPSSecond"),Tele_GPSSecond);*/

  configFile->Write(wxT("Tele_AccelX"),Tele_AccelX);
  configFile->Write(wxT("Tele_AccelY"),Tele_AccelY);
  configFile->Write(wxT("Tele_AccelZ"),Tele_AccelZ);

  configFile->Write(wxT("Tele_VarioAlt"),Tele_VarioAlt);
  configFile->Write(wxT("Tele_VarioAccelH"),Tele_VarioAccelH);

  configFile->Write(wxT("Tele_RSSITx"),Tele_RSSITx);
  configFile->Write(wxT("Tele_RSSIRx"),Tele_RSSIRx);

  configFile->Write(wxT("Tele_Analog1"),Tele_Analog1);
  configFile->Write(wxT("Tele_Analog2"),Tele_Analog2);
  configFile->Write(wxT("Tele_Analog3"),Tele_Analog3);
  configFile->Write(wxT("Tele_Analog4"),Tele_Analog4);

  configFile->Write(wxT("Tele_BattVolt"),Tele_BattVolt);
  configFile->Write(wxT("Tele_BattCurrent"),Tele_BattCurrent);

  configFile->Write(wxT("Tele_RPM"),Tele_RPM);
  configFile->Write(wxT("Tele_Fuel"),Tele_Fuel);
  configFile->Write(wxT("Tele_Temp1"),Tele_Temp1);
  configFile->Write(wxT("Tele_Temp2"),Tele_Temp2);
  configFile->Write(wxT("Tele_Airspeed"),Tele_Airspeed);

  configFile->Write(wxT("Tele_Cell1"),Tele_Cell1);
  configFile->Write(wxT("Tele_Cell2"),Tele_Cell2);
  configFile->Write(wxT("Tele_Cell3"),Tele_Cell3);
  configFile->Write(wxT("Tele_Cell4"),Tele_Cell4);
  configFile->Write(wxT("Tele_Cell5"),Tele_Cell5);
  configFile->Write(wxT("Tele_Cell6"),Tele_Cell6);
  configFile->Write(wxT("Tele_Cell7"),Tele_Cell7);
  configFile->Write(wxT("Tele_Cell8"),Tele_Cell8);
  configFile->Write(wxT("Tele_Cell9"),Tele_Cell9);
  configFile->Write(wxT("Tele_Cell10"),Tele_Cell10);
  configFile->Write(wxT("Tele_Cell11"),Tele_Cell11);
  configFile->Write(wxT("Tele_Cell12"),Tele_Cell12);

  configFile->Write(wxT("EEfile"),CurrentEEPath);


  configFile->Flush();
}

////////// IMPORT EXPORT TO TEXT ////////////////

void OpenAVRc_SimulatorFrame::OnMenuExportEepromSelected(wxCommandEvent& event)
{
  event.Skip();
  ExportEeprom();
}

void OpenAVRc_SimulatorFrame::OnMenuImportEepromSelected(wxCommandEvent& event)
{
  event.Skip();
  int answer = wxMessageBox( _("L'eeprom va être formatée, étes vous sur ?"), _("    OpenAVRc Simulateur"), wxYES_NO, this);
  if (answer == wxNO) {
    return;
  }
  ImportEeprom();
}

void OpenAVRc_SimulatorFrame::ImportEeprom()
{
  wxFileDialog loaFileDialog(this, _("Importer Eeprom"), "", "", "Fichier TXT (*.txt)|*.txt", wxFD_OPEN |wxFD_FILE_MUST_EXIST);
  if (loaFileDialog.ShowModal() == wxID_CANCEL)
    return;     // the user changed idea...

  eepromfile = new wxFileConfig( "", "", loaFileDialog.GetPath());

  int version = 0;
  eepromfile->SetPath("/EEGENERAL/");
  eepromfile->Read(wxT("version"),&version);

  OpenAVRcClose();
  ChronoMain->Pause();
  Chrono10ms->Pause();
  TimerMain.Stop();
  Timer10ms.Stop();
  ResetSimuLcd();

  if (version == 217) {
    wxBusyInfo wait(_("Importation en cours, attendez SVP......"));
    eepromFormat();
    load_EEGeneral_217();
    load_ModelData_217();
   } else if (version == EEPROM_VER || version == (EEPROM_VER-1)) {
    wxBusyInfo wait(_("Importation en cours, attendez SVP......"));
    eepromFormat();
    load_EEGeneral_EEPROM_VER();
    load_ModelData_EEPROM_VER();
  } else {
    wxMessageBox( _("Version d'eeprom non prise en charge"), _("    OpenAVRc Simulateur"));
    Timer10ms.StartOnce(10); //Simulate 10mS Interrupt vector
    TimerMain.StartOnce(18); // Simulate ?mS cycle for mainloop function
    ChronoMain->Resume();
    Chrono10ms->Resume();
    return;
  }

  Timer10ms.StartOnce(10); //Simulate 10mS Interrupt vector
  TimerMain.StartOnce(18); // Simulate ?mS cycle for mainloop function

  delete eepromfile;

}

void OpenAVRc_SimulatorFrame::ExportEeprom()
{

  EEGeneral General = g_eeGeneral;
  if (General.version == 0 ) {
    wxMessageBox( _("Aucune eeprom detectée en mémoire"), _("    OpenAVRc Simulateur"));
    return;
  }

  OpenAVRcClose();
  ChronoMain->Pause();
  Chrono10ms->Pause();
  TimerMain.Stop();
  Timer10ms.Stop();
  ResetSimuLcd();

  wxFileDialog saveFileDialog(this, _("Exporter Eeprom"), "", "", "Fichier TXT (*.txt)|*.txt", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
  if (saveFileDialog.ShowModal() == wxID_CANCEL)
    return;     // the user changed idea...

  eepromfile = new wxFileConfig( "", "", saveFileDialog.GetPath());

  wxBusyInfo wait(_("Exportation en cours, attendez SVP......"));

  if (General.version == EEPROM_VER) {
    eepromfile->DeleteAll();
    save_EEGeneral_EEPROM_VER(General);
    save_ModelData_EEPROM_VER();
  }

  eepromfile->Flush();
  delete eepromfile;

  Timer10ms.StartOnce(10); //Simulate 10mS Interrupt vector
  TimerMain.StartOnce(18); // Simulate ?mS cycle for mainloop function
  ChronoMain->Resume();
  Chrono10ms->Resume();
}

int calculeSwitchOffset(int switchnum) // From 217 to 31
{
  int add = 0;
  int sign = switchnum<0 ? -1 : 1;
  switchnum = abs(switchnum);
  if (switchnum > SWSRC_REB) add = 4;
    if (sign == -1)
     sign = -1;

  if (switchnum > (SWSRC_REB+12)) add = 4 + 8;

  return ((switchnum + add) * sign);
}

int calculeAndSwitchOffset(int switchnum) // From 217 to 31
{
  int sign = (switchnum<0) ? -1 : 1;
  switchnum = abs(switchnum);
  if (switchnum > SWSRC_TRN) switchnum += 14;
  return (switchnum) * sign;
}

int calculeMixSwitchOffset(int mixsrcnum)
{
  int add = 0;
  if (mixsrcnum > (MIXSRC_FIRST_LOGICAL_SWITCH +10)) add += 8;
  if (mixsrcnum > (MIXSRC_FIRST_GVAR +3)) add += 7;
  return (mixsrcnum + add);
}

void OpenAVRc_SimulatorFrame::load_ModelData_EEPROM_VER()
{
  ModelData temp_model;
  int tmp = 0;
  wxString strtmp;

  for (uint8_t m=0; m<MAX_MODELS; ++m) {
    wxString num = wxString::Format(wxT("%i"),m+1);


    if (eepromfile->HasGroup("/MODEL"+num+"/")) {

      theFile.openRlc(FILE_MODEL(m));

      eepromfile->SetPath("/MODEL"+num+"/");
      eepromfile->Read(wxT("name"),&strtmp,"          ");
      ConvWxstrToCharFw(strtmp,temp_model.name, LEN_MODEL_NAME);
      eepromfile->Read(wxT("modelId"),&tmp,0);
      temp_model.modelId = tmp;

      for (int i=0; i<MAX_TIMERS; ++i) { //TimerData timers[MAX_TIMERS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("timers"+num+".mode"),&tmp,0);
        temp_model.timers[i].mode = tmp;
        eepromfile->Read(wxT("timers"+num+".start"),&tmp,0);
        temp_model.timers[i].start = tmp;
        eepromfile->Read(wxT("timers"+num+".countdownBeep"),&tmp,0);
        temp_model.timers[i].countdownBeep = tmp;
        eepromfile->Read(wxT("timers"+num+".minuteBeep"),&tmp,0);
        temp_model.timers[i].minuteBeep = tmp;
        eepromfile->Read(wxT("timers"+num+".persistent"),&tmp,0);
        temp_model.timers[i].persistent = tmp;
        eepromfile->Read(wxT("timers"+num+".spare"),&tmp,0);
        temp_model.timers[i].spare = tmp;
        eepromfile->Read(wxT("timers"+num+".value"),&tmp,0);
        temp_model.timers[i].value = tmp;
      }

      eepromfile->Read(wxT("rfProtocol"),&tmp,0);
      temp_model.rfProtocol = tmp;
      eepromfile->Read(wxT("thrTrim"),&tmp,0);
      temp_model.thrTrim = tmp;
      eepromfile->Read(wxT("rfSubType"),&tmp,0);
      temp_model.rfSubType = tmp;
      eepromfile->Read(wxT("trimInc"),&tmp,0);
      temp_model.trimInc = tmp;
      eepromfile->Read(wxT("disableThrottleWarning"),&tmp,0);
      temp_model.disableThrottleWarning = tmp;
      eepromfile->Read(wxT("rfOptionBool1"),&tmp,0);
      temp_model.rfOptionBool1 = tmp;
      eepromfile->Read(wxT("rfOptionBool2"),&tmp,0);
      temp_model.rfOptionBool2 = tmp;
      eepromfile->Read(wxT("rfOptionBool3"),&tmp,0);
      temp_model.rfOptionBool3 = tmp;
      eepromfile->Read(wxT("extendedLimits"),&tmp,0);
      temp_model.extendedLimits = tmp;
      eepromfile->Read(wxT("extendedTrims"),&tmp,0);
      temp_model.extendedTrims = tmp;
      eepromfile->Read(wxT("throttleReversed"),&tmp,0);
      if (tmp) wxMessageBox(_("La fonction ") + TR_THROTTLEREVERSE + _(" n'hexiste plus\n Modifiez votre modèle"), strtmp, wxICON_WARNING | wxOK, this);
      eepromfile->Read(wxT("rfOptionValue1"),&tmp,0);
      temp_model.rfOptionValue1 = tmp;
      eepromfile->Read(wxT("rfOptionValue2"),&tmp,0);
      temp_model.rfOptionValue2 = tmp;
      eepromfile->Read(wxT("rfOptionValue3"),&tmp,0);
      temp_model.rfOptionValue3 = tmp;
      eepromfile->Read(wxT("beepANACenter"),&tmp,0);
      temp_model.beepANACenter = tmp;

      for (int i=0; i<MAX_MIXERS; ++i) { //MixData   mixData[MAX_MIXERS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("mixData"+num+".destCh"),&tmp,0);
        temp_model.mixData[i].destCh = tmp;
        eepromfile->Read(wxT("mixData"+num+".curveMode"),&tmp,0);
        temp_model.mixData[i].curveMode = tmp;
        eepromfile->Read(wxT("mixData"+num+".noExpo"),&tmp,0);
        temp_model.mixData[i].noExpo = tmp;
        eepromfile->Read(wxT("mixData"+num+".weightMode"),&tmp,0);
        temp_model.mixData[i].weightMode = tmp;
        eepromfile->Read(wxT("mixData"+num+".offsetMode"),&tmp,0);
        temp_model.mixData[i].offsetMode = tmp;
        eepromfile->Read(wxT("mixData"+num+".srcRaw"),&tmp,0);
        temp_model.mixData[i].srcRaw = tmp;
        eepromfile->Read(wxT("mixData"+num+".weight"),&tmp,0);
        temp_model.mixData[i].weight = tmp;
        eepromfile->Read(wxT("mixData"+num+".swtch"),&tmp,0);
        temp_model.mixData[i].swtch = tmp;
        eepromfile->Read(wxT("mixData"+num+".flightModes"),&tmp,0);
        temp_model.mixData[i].flightModes = tmp;
        eepromfile->Read(wxT("mixData"+num+".mltpx"),&tmp,0);
        temp_model.mixData[i].mltpx = tmp;
        eepromfile->Read(wxT("mixData"+num+".carryTrim"),&tmp,0);
        temp_model.mixData[i].carryTrim = tmp;
        eepromfile->Read(wxT("mixData"+num+".mixWarn"),&tmp,0);
        temp_model.mixData[i].mixWarn = tmp;
        eepromfile->Read(wxT("mixData"+num+".spare"),&tmp,0);
        temp_model.mixData[i].spare = tmp;
        eepromfile->Read(wxT("mixData"+num+".delayUp"),&tmp,0);
        temp_model.mixData[i].delayUp = tmp;
        eepromfile->Read(wxT("mixData"+num+".delayDown"),&tmp,0);
        temp_model.mixData[i].delayDown = tmp;
        eepromfile->Read(wxT("mixData"+num+".speedUp"),&tmp,0);
        temp_model.mixData[i].speedUp = tmp;
        eepromfile->Read(wxT("mixData"+num+".speedDown"),&tmp,0);
        temp_model.mixData[i].speedDown = tmp;
        eepromfile->Read(wxT("mixData"+num+".curveParam"),&tmp,0);
        temp_model.mixData[i].curveParam = tmp;
        eepromfile->Read(wxT("mixData"+num+".offset"),&tmp,0);
        temp_model.mixData[i].offset = tmp;
      }

      for (int i=0; i<NUM_CHNOUT; ++i) { //LimitData limitData[NUM_CHNOUT];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("limitData"+num+".min"),&tmp,0);
        temp_model.limitData[i].min = tmp;
        eepromfile->Read(wxT("limitData"+num+".max"),&tmp,0);
        temp_model.limitData[i].max = tmp;
        eepromfile->Read(wxT("limitData"+num+".ppmCenter"),&tmp,0);
        temp_model.limitData[i].ppmCenter = tmp;
        eepromfile->Read(wxT("limitData"+num+".offset"),&tmp,0);
        temp_model.limitData[i].offset = tmp;
        eepromfile->Read(wxT("limitData"+num+".symetrical"),&tmp,0);
        temp_model.limitData[i].symetrical = tmp;
        eepromfile->Read(wxT("limitData"+num+".revert"),&tmp,0);
        temp_model.limitData[i].revert = tmp;
      }

      for (int i=0; i<MAX_EXPOS; ++i) { //ExpoData  expoData[MAX_EXPOS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("expoData"+num+".mode"),&tmp,0);
        temp_model.expoData[i].mode = tmp;
        eepromfile->Read(wxT("expoData"+num+".chn"),&tmp,0);
        temp_model.expoData[i].chn = tmp;
        eepromfile->Read(wxT("expoData"+num+".curveMode"),&tmp,0);
        temp_model.expoData[i].curveMode = tmp;
        eepromfile->Read(wxT("expoData"+num+".spare"),&tmp,0);
        temp_model.expoData[i].spare = tmp;
        eepromfile->Read(wxT("expoData"+num+".flightModes"),&tmp,0);
        temp_model.expoData[i].flightModes = tmp;
        eepromfile->Read(wxT("expoData"+num+".swtch"),&tmp,0);
        temp_model.expoData[i].swtch = tmp;
        eepromfile->Read(wxT("expoData"+num+".weight"),&tmp,0);
        temp_model.expoData[i].weight = tmp;
        eepromfile->Read(wxT("expoData"+num+".curveParam"),&tmp,0);
        temp_model.expoData[i].curveParam = tmp;
      }

      for (int i=0; i<MAX_CURVES; ++i) { //CURVDATA(=int8)  curves[MAX_CURVES];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("curves"+num),&tmp,0);
        temp_model.curves[i] = tmp;
      }

      for (int i=0; i<NUM_POINTS; ++i) { //int8_t    points[NUM_POINTS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("points"+num),&tmp,0);
        temp_model.points[i] = tmp;
      }

      for (int i=0; i<NUM_LOGICAL_SWITCH; ++i) { //LogicalSwitchData logicalSw[NUM_LOGICAL_SWITCH];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("logicalSw"+num+".v1"),&tmp,0);
        temp_model.logicalSw[i].v1 = tmp;
        eepromfile->Read(wxT("logicalSw"+num+".v2"),&tmp,0);
        temp_model.logicalSw[i].v2 = tmp;
        eepromfile->Read(wxT("logicalSw"+num+".func"),&tmp,0);
        temp_model.logicalSw[i].func = tmp;
        eepromfile->Read(wxT("logicalSw"+num+".andsw"),&tmp,0);
        temp_model.logicalSw[i].andsw = tmp;
      }

      for (int i=0; i<NUM_CFN; ++i) { //CustomFunctionData customFn[NUM_CFN];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("customFn"+num+".swtch"),&tmp,0);
        temp_model.customFn[i].swtch = tmp;
        eepromfile->Read(wxT("customFn"+num+".func"),&tmp,0);
        temp_model.customFn[i].func = tmp;
        eepromfile->Read(wxT("customFn"+num+".mode"),&tmp,0);
        temp_model.customFn[i].mode = tmp;
        eepromfile->Read(wxT("customFn"+num+".param"),&tmp,0);
        temp_model.customFn[i].param = tmp;
        eepromfile->Read(wxT("customFn"+num+".active"),&tmp,0);
        temp_model.customFn[i].active = tmp;
        eepromfile->Read(wxT("customFn"+num+".spare"),&tmp,0);
        temp_model.customFn[i].spare = tmp;
        eepromfile->Read(wxT("customFn"+num+".value"),&tmp,0);
        temp_model.customFn[i].value = tmp;
      }

      eepromfile->Read(wxT("swashR.invertELE"),&tmp,0);
      temp_model.swashR.invertELE = tmp; //SwashRingData swashR;
      eepromfile->Read(wxT("swashR.invertAIL"),&tmp,0);
      temp_model.swashR.invertAIL = tmp;
      eepromfile->Read(wxT("swashR.invertCOL"),&tmp,0);
      temp_model.swashR.invertCOL = tmp;
      eepromfile->Read(wxT("swashR.type"),&tmp,0);
      temp_model.swashR.type = tmp;
      eepromfile->Read(wxT("swashR.collectiveSource"),&tmp,0);
      temp_model.swashR.collectiveSource = tmp;
      eepromfile->Read(wxT("swashR.value"),&tmp,0);
      temp_model.swashR.value = tmp;

      for (int i=0; i<MAX_FLIGHT_MODES; ++i) { //FlightModeData flightModeData[MAX_FLIGHT_MODES];
        num = wxString::Format(wxT("%i"),i);

        for (int j=0; j<4; ++j) { //int16 trim[4] TRIMS_ARRAY;
          wxString numtrim = wxString::Format(wxT("%i"),j);
          eepromfile->Read(wxT("flightModeData"+num+".trim"+numtrim),&tmp,0);
          temp_model.flightModeData[i].trim[j] = tmp;
        }

        eepromfile->Read(wxT("flightModeData"+num+".swtch"),&tmp,0);
        temp_model.flightModeData[i].swtch = tmp;
        eepromfile->Read(wxT("flightModeData"+num+".name"),&strtmp,"");
        ConvWxstrToCharFw(strtmp,temp_model.flightModeData[i].name, LEN_FLIGHT_MODE_NAME);
        eepromfile->Read(wxT("flightModeData"+num+".fadeIn"),&tmp,0);
        temp_model.flightModeData[i].fadeIn = tmp;
        eepromfile->Read(wxT("flightModeData"+num+".fadeOut"),&tmp,0);
        temp_model.flightModeData[i].fadeOut = tmp;

        for (int j=0; j<NUM_ROTARY_ENCODERS; ++j) { //ROTARY_ENCODER_ARRAY int16_t rotaryEncoders[2];
          wxString numtrim = wxString::Format(wxT("%i"),j);
          eepromfile->Read(wxT("flightModeData"+num+".rotaryEncoders"+numtrim),&tmp,0);
          temp_model.flightModeData[i].rotaryEncoders[j] = tmp;
        }

        for (int k=0; k<MAX_GVARS; ++k) { //PHASE_GVARS_DATA gvar_t gvars[MAX_GVARS]
          wxString numgvar = wxString::Format(wxT("%i"),k);
          eepromfile->Read(wxT("flightModeData"+num+".gvars"+numgvar),&tmp,0);
          temp_model.flightModeData[i].gvars[k] = tmp;
        }
      }

      eepromfile->Read(wxT("rfOptionValue1"),&tmp,0);
      temp_model.rfOptionValue1 = tmp;
      eepromfile->Read(wxT("rfOptionValue2"),&tmp,0);
      temp_model.rfOptionValue2 = tmp;
      eepromfile->Read(wxT("rfOptionValue3"),&tmp,0);
      temp_model.rfOptionValue3 = tmp;
      eepromfile->Read(wxT("thrTraceSrc"),&tmp,0);
      temp_model.thrTraceSrc = tmp;
      eepromfile->Read(wxT("thrSwitch"),&tmp,0);
      temp_model.thrSwitch = tmp;
      eepromfile->Read(wxT("switchWarningState"),&tmp,0);
      temp_model.switchWarningState = tmp;
      eepromfile->Read(wxT("switchWarningEnable"),&tmp,0);
      temp_model.switchWarningEnable = tmp;

      for (int i=0; i<MAX_GVARS; ++i) { //global_gvar_t gvars[MAX_GVARS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("gvars"+num+".name"),&strtmp,"");
        ConvWxstrToCharFw(strtmp,temp_model.gvars[i].name, LEN_GVAR_NAME);
      }

      eepromfile->Read(wxT("UnusedModel"),&tmp,0);
      temp_model.UnusedModel = tmp;

      for (int i=0; i<MAX_FRSKY_A_CHANNELS; ++i) { //FrSkyChannelData channels[MAX_FRSKY_A_CHANNELS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("telemetry.channels"+num+".ratio"),&tmp,0);
        temp_model.telemetry.channels[i].ratio = tmp;
        eepromfile->Read(wxT("telemetry.channels"+num+".offset"),&tmp,0);
        temp_model.telemetry.channels[i].offset = tmp;
        eepromfile->Read(wxT("telemetry.channels"+num+".type"),&tmp,0);
        temp_model.telemetry.channels[i].type = tmp;
        /*for (int j=0; j<2; ++j) { //alarms_value[2];
          wxString numvalue = wxString::Format(wxT("%i"),j);
          eepromfile->Read(wxT("telemetry.channels"+num+".alarms_value"+numvalue),&tmp,0);
          temp_model.telemetry.channels[i].alarms_value[j] = tmp;
        }
        eepromfile->Read(wxT("telemetry.channels"+num+".alarms_level"),&tmp,0);
        temp_model.telemetry.channels[i].alarms_level = tmp;
        eepromfile->Read(wxT("telemetry.channels"+num+".alarms_greater"),&tmp,0);
        temp_model.telemetry.channels[i].alarms_greater = tmp;*/
        eepromfile->Read(wxT("telemetry.channels"+num+".multiplier"),&tmp,0);
        temp_model.telemetry.channels[i].multiplier = tmp;
      }
      eepromfile->Read(wxT("telemetry.usrProto"),&tmp,0);
      temp_model.telemetry.usrProto = tmp;
      eepromfile->Read(wxT("telemetry.blades"),&tmp,0);
      temp_model.telemetry.blades = tmp;
      eepromfile->Read(wxT("telemetry.screensType"),&tmp,0);
      temp_model.telemetry.screensType = tmp;
      eepromfile->Read(wxT("telemetry.voltsSource"),&tmp,0);
      temp_model.telemetry.voltsSource = tmp;

      eepromfile->Read(wxT("telemetry.rssiAlarm"),&tmp,0);//tdtele added
      temp_model.telemetry.rssiAlarm = tmp;

      eepromfile->Read(wxT("telemetry.varioMin"),&tmp,0);
      temp_model.telemetry.varioMin = tmp;
      eepromfile->Read(wxT("telemetry.varioMax"),&tmp,0);
      temp_model.telemetry.varioMax = tmp;
      /*for (int i=0; i<2; ++i) { //FrSkyRSSIAlarm rssiAlarms[2];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("telemetry.rssiAlarms"+num+".level"),&tmp,0);
        temp_model.telemetry.rssiAlarms[i].level = tmp;
        eepromfile->Read(wxT("telemetry.rssiAlarms"+num+".value"),&tmp,0);
        temp_model.telemetry.rssiAlarms[i].value = tmp;
      }*/
      for (int i=0; i<MAX_TELEMETRY_SCREENS; ++i) { //FrSkyScreenData screens[MAX_TELEMETRY_SCREENS];
        num = wxString::Format(wxT("%i"),i);
        for (int j=0; j<4; ++j) { //FrSkyBarData bars[4]; or FrSkyLineData
          wxString numbl = wxString::Format(wxT("%i"),j);
          eepromfile->Read(wxT("telemetry.screens"+num+".bars"+numbl+"source"),&tmp,0);
          temp_model.telemetry.screens[i].bars[j].source = tmp;
          eepromfile->Read(wxT("telemetry.screens"+num+".bars"+numbl+"barMin"),&tmp,0);
          temp_model.telemetry.screens[i].bars[j].barMin = tmp;
          eepromfile->Read(wxT("telemetry.screens"+num+".bars"+numbl+"barMax"),&tmp,0);
          temp_model.telemetry.screens[i].bars[j].barMax = tmp;
        }
      }
      eepromfile->Read(wxT("telemetry.varioSource"),&tmp,0);
      temp_model.telemetry.varioSource = tmp;
      eepromfile->Read(wxT("telemetry.varioCenterMin"),&tmp,0);
      temp_model.telemetry.varioCenterMin = tmp;
      eepromfile->Read(wxT("telemetry.currentSource"),&tmp,0);
      temp_model.telemetry.currentSource = tmp;
      eepromfile->Read(wxT("telemetry.varioCenterMax"),&tmp,0);
      temp_model.telemetry.varioCenterMax = tmp;
      eepromfile->Read(wxT("telemetry.fasOffset"),&tmp,0);
      temp_model.telemetry.fasOffset = tmp;

      for (int i=0; i<NUM_MAX_X_ANY; ++i)
        {
          num = wxString::Format(wxT("%i"),i);
          eepromfile->Read(wxT("Xany"+num+"Active"),&tmp,0);
          temp_model.Xany[i].Active = tmp;
          eepromfile->Read(wxT("Xany"+num+"ChId"),&tmp,0);
          temp_model.Xany[i].ChId = tmp;
          eepromfile->Read(wxT("Xany"+num+"RepeatNb"),&tmp,0);
          temp_model.Xany[i].RepeatNb = tmp;
          eepromfile->Read(wxT("Xany"+num+"Reserved1"),&tmp,0);
          temp_model.Xany[i].Reserved1 = tmp;
          eepromfile->Read(wxT("Xany"+num+"PayloadCfg.AngleSrcIdx"),&tmp,0);
          temp_model.Xany[i].PayloadCfg.AngleSrcIdx = tmp;
          eepromfile->Read(wxT("Xany"+num+"PayloadCfg.PropSrcIdx"),&tmp,0);
          temp_model.Xany[i].PayloadCfg.PropSrcIdx = tmp;
          eepromfile->Read(wxT("Xany"+num+"PayloadCfg.SwitchSrcIdx"),&tmp,0);
          temp_model.Xany[i].PayloadCfg.SwitchSrcIdx = tmp;
          eepromfile->Read(wxT("Xany"+num+"PayloadCfg.FreeCfgBits"),&tmp,0);
          temp_model.Xany[i].PayloadCfg.FreeCfgBits = tmp;
          eepromfile->Read(wxT("Xany"+num+"Param.Raw"),&tmp,0);
          temp_model.Xany[i].Param.Raw = tmp;
          eepromfile->Read(wxT("Xany"+num+"Rfu"),&tmp,0);
          temp_model.Xany[i].Rfu = tmp;
        }

      theFile.writeRlc(FILE_MODEL(m), FILE_TYP_MODEL, (uint8_t*)&temp_model, sizeof(temp_model), 1);

    }
  }
  theFile.openRlc(FILE_MODEL(g_eeGeneral.currModel));
  theFile.readRlc((uint8_t*)&g_model, sizeof(g_model)); // Reload current
}

void OpenAVRc_SimulatorFrame::load_ModelData_217()
{
  ModelData temp_model;
  int tmp = 0;
  wxString strtmp;
  uint8_t cstate;

  for (uint8_t m=0; m<MAX_MODELS; ++m) {
    wxString num = wxString::Format(wxT("%i"),m+1);


    if (eepromfile->HasGroup("/MODEL"+num+"/")) {

      theFile.openRlc(FILE_MODEL(m));

      eepromfile->SetPath("/MODEL"+num+"/");
      eepromfile->Read(wxT("header.name"),&strtmp,"          ");
      ConvWxstrToCharFw(strtmp,temp_model.name, LEN_MODEL_NAME);
      eepromfile->Read(wxT("header.modelId"),&tmp,0);
      temp_model.modelId = tmp;

      for (int i=0; i<MAX_TIMERS; ++i) { //TimerData timers[MAX_TIMERS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("timers"+num+".mode"),&tmp,0);
        temp_model.timers[i].mode = tmp;
        eepromfile->Read(wxT("timers"+num+".start"),&tmp,0);
        temp_model.timers[i].start = tmp;
        eepromfile->Read(wxT("timers"+num+".countdownBeep"),&tmp,0);
        temp_model.timers[i].countdownBeep = tmp;
        eepromfile->Read(wxT("timers"+num+".minuteBeep"),&tmp,0);
        temp_model.timers[i].minuteBeep = tmp;
        eepromfile->Read(wxT("timers"+num+".persistent"),&tmp,0);
        temp_model.timers[i].persistent = tmp;
        eepromfile->Read(wxT("timers"+num+".spare"),&tmp,0);
        temp_model.timers[i].spare = tmp;
        eepromfile->Read(wxT("timers"+num+".value"),&tmp,0);
        temp_model.timers[i].value = tmp;
      }

      eepromfile->Read(wxT("protocol"),&tmp,0);
      temp_model.rfProtocol = tmp;
      eepromfile->Read(wxT("thrTrim"),&tmp,0);
      temp_model.thrTrim = tmp;
      eepromfile->Read(wxT("ppmNCH"),&tmp,0);
      temp_model.rfSubType = tmp+2;
      eepromfile->Read(wxT("trimInc"),&tmp,0);
      temp_model.trimInc = tmp;
      eepromfile->Read(wxT("disableThrottleWarning"),&tmp,0);
      temp_model.disableThrottleWarning = tmp;
      eepromfile->Read(wxT("pulsePol"),&tmp,0);
      temp_model.rfOptionBool1 = tmp;
      eepromfile->Read(wxT("extendedLimits"),&tmp,0);
      temp_model.extendedLimits = tmp;
      eepromfile->Read(wxT("extendedTrims"),&tmp,0);
      temp_model.extendedTrims = tmp;
      eepromfile->Read(wxT("throttleReversed"),&tmp,0);
      if (tmp) wxMessageBox(_("La fonction ") + TR_THROTTLEREVERSE + _(" n'hexiste plus\n Modifiez votre modèle"), strtmp, wxICON_WARNING | wxOK, this);
      eepromfile->Read(wxT("ppmDelay"),&tmp,0);
      temp_model.rfOptionValue2 = tmp;
      eepromfile->Read(wxT("beepANACenter"),&tmp,0);
      temp_model.beepANACenter = tmp;

      for (int i=0; i<MAX_MIXERS; ++i) { //MixData   mixData[MAX_MIXERS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("mixData"+num+".destCh"),&tmp,0);
        temp_model.mixData[i].destCh = tmp;
        eepromfile->Read(wxT("mixData"+num+".curveMode"),&tmp,0);
        temp_model.mixData[i].curveMode = tmp;
        eepromfile->Read(wxT("mixData"+num+".noExpo"),&tmp,0);
        temp_model.mixData[i].noExpo = tmp;
        eepromfile->Read(wxT("mixData"+num+".weightMode"),&tmp,0);
        temp_model.mixData[i].weightMode = tmp;
        eepromfile->Read(wxT("mixData"+num+".offsetMode"),&tmp,0);
        temp_model.mixData[i].offsetMode = tmp;
        eepromfile->Read(wxT("mixData"+num+".srcRaw"),&tmp,0);
        temp_model.mixData[i].srcRaw = calculeMixSwitchOffset(tmp);
        eepromfile->Read(wxT("mixData"+num+".weight"),&tmp,0);
        temp_model.mixData[i].weight = tmp;
        eepromfile->Read(wxT("mixData"+num+".swtch"),&tmp,0);
        temp_model.mixData[i].swtch = calculeSwitchOffset(tmp);
        eepromfile->Read(wxT("mixData"+num+".flightModes"),&tmp,0);
        temp_model.mixData[i].flightModes = tmp;
        eepromfile->Read(wxT("mixData"+num+".mltpx"),&tmp,0);
        temp_model.mixData[i].mltpx = tmp;
        eepromfile->Read(wxT("mixData"+num+".carryTrim"),&tmp,0);
        temp_model.mixData[i].carryTrim = tmp;
        eepromfile->Read(wxT("mixData"+num+".mixWarn"),&tmp,0);
        temp_model.mixData[i].mixWarn = tmp;
        eepromfile->Read(wxT("mixData"+num+".spare"),&tmp,0);
        temp_model.mixData[i].spare = tmp;
        eepromfile->Read(wxT("mixData"+num+".delayUp"),&tmp,0);
        temp_model.mixData[i].delayUp = tmp;
        eepromfile->Read(wxT("mixData"+num+".delayDown"),&tmp,0);
        temp_model.mixData[i].delayDown = tmp;
        eepromfile->Read(wxT("mixData"+num+".speedUp"),&tmp,0);
        temp_model.mixData[i].speedUp = tmp;
        eepromfile->Read(wxT("mixData"+num+".speedDown"),&tmp,0);
        temp_model.mixData[i].speedDown = tmp;
        eepromfile->Read(wxT("mixData"+num+".curveParam"),&tmp,0);
        temp_model.mixData[i].curveParam = tmp;
        eepromfile->Read(wxT("mixData"+num+".offset"),&tmp,0);
        temp_model.mixData[i].offset = tmp;
      }

      for (int i=0; i<NUM_CHNOUT; ++i) { //LimitData limitData[NUM_CHNOUT];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("limitData"+num+".min"),&tmp,0);
        temp_model.limitData[i].min = tmp;
        eepromfile->Read(wxT("limitData"+num+".max"),&tmp,0);
        temp_model.limitData[i].max = tmp;
        eepromfile->Read(wxT("limitData"+num+".ppmCenter"),&tmp,0);
        temp_model.limitData[i].ppmCenter = tmp;
        eepromfile->Read(wxT("limitData"+num+".offset"),&tmp,0);
        temp_model.limitData[i].offset = tmp;
        eepromfile->Read(wxT("limitData"+num+".symetrical"),&tmp,0);
        temp_model.limitData[i].symetrical = tmp;
        eepromfile->Read(wxT("limitData"+num+".revert"),&tmp,0);
        temp_model.limitData[i].revert = tmp;
      }

      for (int i=0; i<MAX_EXPOS; ++i) { //ExpoData  expoData[MAX_EXPOS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("expoData"+num+".mode"),&tmp,0);
        temp_model.expoData[i].mode = tmp;
        eepromfile->Read(wxT("expoData"+num+".chn"),&tmp,0);
        temp_model.expoData[i].chn = tmp;
        eepromfile->Read(wxT("expoData"+num+".curveMode"),&tmp,0);
        temp_model.expoData[i].curveMode = tmp;
        eepromfile->Read(wxT("expoData"+num+".spare"),&tmp,0);
        temp_model.expoData[i].spare = tmp;
        eepromfile->Read(wxT("expoData"+num+".flightModes"),&tmp,0);
        temp_model.expoData[i].flightModes = tmp;
        eepromfile->Read(wxT("expoData"+num+".swtch"),&tmp,0);
        temp_model.expoData[i].swtch = calculeSwitchOffset(tmp);
        eepromfile->Read(wxT("expoData"+num+".weight"),&tmp,0);
        temp_model.expoData[i].weight = tmp;
        eepromfile->Read(wxT("expoData"+num+".curveParam"),&tmp,0);
        temp_model.expoData[i].curveParam = tmp;
      }

      for (int i=0; i<MAX_CURVES; ++i) { //CURVDATA(=int8)  curves[MAX_CURVES];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("curves"+num),&tmp,0);
        temp_model.curves[i] = tmp;
      }

      for (int i=0; i<NUM_POINTS; ++i) { //int8_t    points[NUM_POINTS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("points"+num),&tmp,0);
        temp_model.points[i] = tmp;
      }

      for (int i=0; i<NUM_LOGICAL_SWITCH; ++i)
      {
        //LogicalSwitchData logicalSw[NUM_LOGICAL_SWITCH];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("logicalSw"+num+".func"),&tmp,0);
        temp_model.logicalSw[i].func = (tmp == 0 ? tmp : tmp+1); // Function v==offset added
        eepromfile->Read(wxT("logicalSw"+num+".andsw"),&tmp,0);
        temp_model.logicalSw[i].andsw = calculeAndSwitchOffset(tmp);
        cstate = lswFamily(temp_model.logicalSw[i].func);
        if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY)
        {
          eepromfile->Read(wxT("logicalSw"+num+".v1"),&tmp,0);
          temp_model.logicalSw[i].v1 = calculeSwitchOffset(tmp);
          eepromfile->Read(wxT("logicalSw"+num+".v2"),&tmp,0);
          temp_model.logicalSw[i].v2 = calculeSwitchOffset(tmp);
        }
        else
        {
          eepromfile->Read(wxT("logicalSw"+num+".v1"),&tmp,0);
          temp_model.logicalSw[i].v1 = (tmp);
          eepromfile->Read(wxT("logicalSw"+num+".v2"),&tmp,0);
          temp_model.logicalSw[i].v2 = (tmp);
        }
      }

      for (int i=0; i<NUM_CFN; ++i) {  //CustomFunctionData customFn[NUM_CFN];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("customFn"+num+".swtch"),&tmp,0);
        temp_model.customFn[i].swtch = calculeSwitchOffset(tmp);
        eepromfile->Read(wxT("customFn"+num+".func"),&tmp,0);
        temp_model.customFn[i].func = tmp;
        eepromfile->Read(wxT("customFn"+num+".mode"),&tmp,0);
        temp_model.customFn[i].mode = tmp;
        eepromfile->Read(wxT("customFn"+num+".param"),&tmp,0);
        temp_model.customFn[i].param = tmp;
        eepromfile->Read(wxT("customFn"+num+".active"),&tmp,0);
        temp_model.customFn[i].active = tmp;
        eepromfile->Read(wxT("customFn"+num+".spare"),&tmp,0);
        temp_model.customFn[i].spare = tmp;
        eepromfile->Read(wxT("customFn"+num+".value"),&tmp,0);
        if (temp_model.customFn[i].func == FUNC_PLAY_VALUE ) {
        temp_model.customFn[i].value = calculeMixSwitchOffset(tmp);
        } else {
        temp_model.customFn[i].value = tmp;
        }
      }

      eepromfile->Read(wxT("swashR.invertELE"),&tmp,0);
      temp_model.swashR.invertELE = tmp; //SwashRingData swashR;
      eepromfile->Read(wxT("swashR.invertAIL"),&tmp,0);
      temp_model.swashR.invertAIL = tmp;
      eepromfile->Read(wxT("swashR.invertCOL"),&tmp,0);
      temp_model.swashR.invertCOL = tmp;
      eepromfile->Read(wxT("swashR.type"),&tmp,0);
      temp_model.swashR.type = tmp;
      eepromfile->Read(wxT("swashR.collectiveSource"),&tmp,0);
      temp_model.swashR.collectiveSource = tmp;
      eepromfile->Read(wxT("swashR.value"),&tmp,0);
      temp_model.swashR.value = tmp;

      for (int i=0; i<MAX_FLIGHT_MODES; ++i) { //FlightModeData flightModeData[MAX_FLIGHT_MODES];
        num = wxString::Format(wxT("%i"),i);

        for (int j=0; j<4; ++j) { //int16 trim[4] TRIMS_ARRAY;
          wxString numtrim = wxString::Format(wxT("%i"),j);
          eepromfile->Read(wxT("flightModeData"+num+".trim"+numtrim),&tmp,0);
          temp_model.flightModeData[i].trim[j] = tmp;
        }

        eepromfile->Read(wxT("flightModeData"+num+".swtch"),&tmp,0);
        temp_model.flightModeData[i].swtch = calculeSwitchOffset(tmp);
        eepromfile->Read(wxT("flightModeData"+num+".name"),&strtmp,"");
        ConvWxstrToCharFw(strtmp,temp_model.flightModeData[i].name, LEN_FLIGHT_MODE_NAME);
        eepromfile->Read(wxT("flightModeData"+num+".fadeIn"),&tmp,0);
        temp_model.flightModeData[i].fadeIn = tmp;
        eepromfile->Read(wxT("flightModeData"+num+".fadeOut"),&tmp,0);
        temp_model.flightModeData[i].fadeOut = tmp;

        for (int j=0; j<NUM_ROTARY_ENCODERS; ++j) { //ROTARY_ENCODER_ARRAY int16_t rotaryEncoders[2];
          wxString numtrim = wxString::Format(wxT("%i"),j);
          eepromfile->Read(wxT("flightModeData"+num+".rotaryEncoders"+numtrim),&tmp,0);
          temp_model.flightModeData[i].rotaryEncoders[j] = tmp;
        }

        for (int k=0; k<MAX_GVARS; ++k) { //PHASE_GVARS_DATA gvar_t gvars[MAX_GVARS]
          wxString numgvar = wxString::Format(wxT("%i"),k);
          eepromfile->Read(wxT("flightModeData"+num+".gvars"+numgvar),&tmp,0);
          if (tmp >128) {tmp = tmp-128 /*GVAR_MAX V217*/+121/*GVAR_MAX V31*/;}
          temp_model.flightModeData[i].gvars[k] = tmp;
        }
      }

      eepromfile->Read(wxT("ppmFrameLength"),&tmp,0);
      temp_model.rfOptionValue1 = tmp;
      eepromfile->Read(wxT("thrTraceSrc"),&tmp,0);
      temp_model.thrTraceSrc = tmp;
      eepromfile->Read(wxT("switchWarningState"),&tmp,0);
      temp_model.switchWarningState = tmp;
      eepromfile->Read(wxT("switchWarningEnable"),&tmp,0);
      temp_model.switchWarningEnable = tmp;

      for (int i=0; i<MAX_GVARS; ++i) { //global_gvar_t gvars[MAX_GVARS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("gvars"+num+".name"),&strtmp,"");
        ConvWxstrToCharFw(strtmp,temp_model.gvars[i].name, LEN_GVAR_NAME);
      }

      temp_model.UnusedModel = 0;

      for (int i=0; i<MAX_FRSKY_A_CHANNELS; ++i) { //FrSkyChannelData channels[MAX_FRSKY_A_CHANNELS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("frsky.channels"+num+".ratio"),&tmp,0);
        temp_model.telemetry.channels[i].ratio = tmp;
        eepromfile->Read(wxT("frsky.channels"+num+".offset"),&tmp,0);
        temp_model.telemetry.channels[i].offset = tmp;
        eepromfile->Read(wxT("frsky.channels"+num+".type"),&tmp,0);
        temp_model.telemetry.channels[i].type = tmp;
        /*for (int j=0; j<2; ++j) { //alarms_value[2];
          wxString numvalue = wxString::Format(wxT("%i"),j);
          eepromfile->Read(wxT("frsky.channels"+num+".alarms_value"+numvalue),&tmp,0);
          temp_model.telemetry.channels[i].alarms_value[j] = tmp;
        }
        eepromfile->Read(wxT("frsky.channels"+num+".alarms_level"),&tmp,0);
        temp_model.telemetry.channels[i].alarms_level = tmp;
        eepromfile->Read(wxT("frsky.channels"+num+".alarms_greater"),&tmp,0);
        temp_model.telemetry.channels[i].alarms_greater = tmp;*/
        eepromfile->Read(wxT("frsky.channels"+num+".multiplier"),&tmp,0);
        temp_model.telemetry.channels[i].multiplier = tmp;
      }
      eepromfile->Read(wxT("frsky.usrProto"),&tmp,0);
      temp_model.telemetry.usrProto = tmp;
      eepromfile->Read(wxT("frsky.blades"),&tmp,0);
      temp_model.telemetry.blades = tmp;
      eepromfile->Read(wxT("frsky.screensType"),&tmp,0);
      temp_model.telemetry.screensType = tmp;
      eepromfile->Read(wxT("frsky.voltsSource"),&tmp,0);
      temp_model.telemetry.voltsSource = tmp;

      eepromfile->Read(wxT("frsky.rssiAlarm"),&tmp,0);
      temp_model.telemetry.rssiAlarm = tmp;


      eepromfile->Read(wxT("frsky.varioMin"),&tmp,0);
      temp_model.telemetry.varioMin = tmp;
      eepromfile->Read(wxT("frsky.varioMax"),&tmp,0);
      temp_model.telemetry.varioMax = tmp;
      /*for (int i=0; i<2; ++i) { //FrSkyRSSIAlarm rssiAlarms[2];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Read(wxT("frsky.rssiAlarms"+num+".level"),&tmp,0);
        temp_model.telemetry.rssiAlarms[i].level = tmp;
        eepromfile->Read(wxT("frsky.rssiAlarms"+num+".value"),&tmp,0);
        temp_model.telemetry.rssiAlarms[i].value = tmp;
      }*/
      for (int i=0; i<MAX_TELEMETRY_SCREENS; ++i) { //FrSkyScreenData screens[MAX_TELEMETRY_SCREENS];
        num = wxString::Format(wxT("%i"),i);
        for (int j=0; j<4; ++j) { //FrSkyBarData bars[4]; or FrSkyLineData
          wxString numbl = wxString::Format(wxT("%i"),j);
          eepromfile->Read(wxT("frsky.screens"+num+".bars"+numbl+"source"),&tmp,0);
          temp_model.telemetry.screens[i].bars[j].source = tmp;
          eepromfile->Read(wxT("frsky.screens"+num+".bars"+numbl+"barMin"),&tmp,0);
          temp_model.telemetry.screens[i].bars[j].barMin = tmp;
          eepromfile->Read(wxT("frsky.screens"+num+".bars"+numbl+"barMax"),&tmp,0);
          temp_model.telemetry.screens[i].bars[j].barMax = tmp;
        }
      }
      eepromfile->Read(wxT("frsky.varioSource"),&tmp,0);
      temp_model.telemetry.varioSource = tmp;
      eepromfile->Read(wxT("frsky.varioCenterMin"),&tmp,0);
      temp_model.telemetry.varioCenterMin = tmp;
      eepromfile->Read(wxT("frsky.currentSource"),&tmp,0);
      temp_model.telemetry.currentSource = tmp;
      eepromfile->Read(wxT("frsky.varioCenterMax"),&tmp,0);
      temp_model.telemetry.varioCenterMax = tmp;
      eepromfile->Read(wxT("frsky.fasOffset"),&tmp,0);
      temp_model.telemetry.fasOffset = tmp;

      for (int i=0; i<NUM_MAX_X_ANY; ++i)
        {
          temp_model.Xany[i].Active = 0;
          temp_model.Xany[i].ChId = 0;
          temp_model.Xany[i].RepeatNb = 0;
          temp_model.Xany[i].Reserved1 = 0;
          temp_model.Xany[i].PayloadCfg.AngleSrcIdx = 0;
          temp_model.Xany[i].PayloadCfg.PropSrcIdx = 0;
          temp_model.Xany[i].PayloadCfg.SwitchSrcIdx = 0;
          temp_model.Xany[i].PayloadCfg.FreeCfgBits = 0;
          temp_model.Xany[i].Param.Raw = 0;
          temp_model.Xany[i].Rfu = 0;
        }

      theFile.writeRlc(FILE_MODEL(m), FILE_TYP_MODEL, (uint8_t*)&temp_model, sizeof(temp_model), 1);

    }
  }
  theFile.openRlc(FILE_MODEL(g_eeGeneral.currModel));
  theFile.readRlc((uint8_t*)&g_model, sizeof(g_model)); // Reload current
}

void OpenAVRc_SimulatorFrame::load_EEGeneral_EEPROM_VER()
{
  int tmp =0;
  eepromfile->SetPath("/EEGENERAL/");
  eepromfile->Read(wxT("version"),&tmp,0);
  g_eeGeneral.version = tmp;
  wxString strtmp;

  for (int i=0; i<(NUM_STICKS+NUM_POTS); ++i) { //CalibData calib[NUM_STICKS+NUM_POTS]
    wxString num = wxString::Format(wxT("%i"),i);
    eepromfile->Read(wxT("calib"+num+".mid"),&tmp,0);
    g_eeGeneral.calib[i].mid = tmp;
    eepromfile->Read(wxT("calib"+num+".spanNeg"),&tmp,0);
    g_eeGeneral.calib[i].spanNeg = tmp;
    eepromfile->Read(wxT("calib"+num+".spanPos"),&tmp,0);
    g_eeGeneral.calib[i].spanPos = tmp;
  }

  eepromfile->Read(wxT("chkSum"),g_eeGeneral.chkSum);
  eepromfile->Read(wxT("currModel"),&tmp,0);
  g_eeGeneral.currModel = tmp;
  eepromfile->Read(wxT("contrast"),&tmp,0);
  g_eeGeneral.contrast = tmp;
  eepromfile->Read(wxT("vBatWarn"),&tmp,0);
  g_eeGeneral.vBatWarn = tmp;
  eepromfile->Read(wxT("txVoltageCalibration"),&tmp,0);
  g_eeGeneral.txVoltageCalibration = tmp;
  eepromfile->Read(wxT("backlightMode"),&tmp,0);
  g_eeGeneral.backlightMode = tmp;

  for (int i=0; i<4; ++i) { //TrainerData trainer;
    wxString num = wxString::Format(wxT("%i"),i);
    eepromfile->Read(wxT("trainer.calib"+num),&tmp,0);
    g_eeGeneral.trainer.calib[i] = tmp;
    eepromfile->Read(wxT("trainer.mix"+num+".srcChn"),&tmp,0);
    g_eeGeneral.trainer.mix[i].srcChn = tmp;
    eepromfile->Read(wxT("trainer.mix"+num+".mode"),&tmp,0);
    g_eeGeneral.trainer.mix[i].mode = tmp;
    eepromfile->Read(wxT("trainer.mix"+num+".studWeight"),&tmp,0);
    g_eeGeneral.trainer.mix[i].studWeight = tmp;
  }

  eepromfile->Read(wxT("view"),&tmp,0);
  g_eeGeneral.view = tmp;
  eepromfile->Read(wxT("buzzerMode"),&tmp,0);
  g_eeGeneral.buzzerMode = tmp;
  eepromfile->Read(wxT("fai"),&tmp,0);
  g_eeGeneral.fai = tmp;
  eepromfile->Read(wxT("beepMode"),&tmp,0);
  g_eeGeneral.beepMode = tmp;
  eepromfile->Read(wxT("alarmsFlash"),&tmp,0);
  g_eeGeneral.alarmsFlash = tmp;
  eepromfile->Read(wxT("disableMemoryWarning"),&tmp,0);
  g_eeGeneral.disableMemoryWarning = tmp;
  eepromfile->Read(wxT("disableAlarmWarning"),&tmp,0);
  g_eeGeneral.disableAlarmWarning = tmp;
  eepromfile->Read(wxT("stickMode"),&tmp,0);
  g_eeGeneral.stickMode = tmp;
  eepromfile->Read(wxT("timezone"),&tmp,0);
  g_eeGeneral.timezone = tmp;
  eepromfile->Read(wxT("adjustRTC"),&tmp,0);
  g_eeGeneral.adjustRTC = tmp;
  eepromfile->Read(wxT("inactivityTimer"),&tmp,0);
  g_eeGeneral.inactivityTimer = tmp;
  eepromfile->Read(wxT("mavbaud"),&tmp,0);
  g_eeGeneral.mavbaud = tmp;
  eepromfile->Read(wxT("splashMode"),&tmp,0);
  g_eeGeneral.splashMode = tmp;
  eepromfile->Read(wxT("hapticMode"),&tmp,0);
  g_eeGeneral.hapticMode = tmp;
  eepromfile->Read(wxT("blOffBright"),&tmp,0);
  g_eeGeneral.blOffBright = tmp;
  eepromfile->Read(wxT("blOnBright"),&tmp,0);
  g_eeGeneral.blOnBright = tmp;
  eepromfile->Read(wxT("lightAutoOff"),&tmp,0);
  g_eeGeneral.lightAutoOff = tmp;
  eepromfile->Read(wxT("templateSetup"),&tmp,0);
  g_eeGeneral.templateSetup = tmp;
  eepromfile->Read(wxT("PPM_Multiplier"),&tmp,0);
  g_eeGeneral.PPM_Multiplier = tmp;
  eepromfile->Read(wxT("hapticLength"),&tmp,0);
  g_eeGeneral.hapticLength = tmp;
  eepromfile->Read(wxT("reNavigation"),&tmp,0);
  g_eeGeneral.reNavigation = tmp;
  eepromfile->Read(wxT("protocol_mask"),&tmp,0);
  g_eeGeneral.protocol_mask = tmp;
  eepromfile->Read(wxT("stickReverse"),&tmp,0);
  g_eeGeneral.stickReverse = tmp;
  eepromfile->Read(wxT("beepLength"),&tmp,0);
  g_eeGeneral.beepLength = tmp;
  eepromfile->Read(wxT("hapticStrength"),&tmp,0);
  g_eeGeneral.hapticStrength = tmp;
  eepromfile->Read(wxT("gpsFormat"),&tmp,0);
  g_eeGeneral.gpsFormat = tmp;
  eepromfile->Read(wxT("unexpectedShutdown"),&tmp,0);
  g_eeGeneral.unexpectedShutdown = tmp;
  eepromfile->Read(wxT("speakerPitch"),&tmp,0);
  g_eeGeneral.speakerPitch = tmp;
  eepromfile->Read(wxT("speakerVolume"),&tmp,0);
  g_eeGeneral.speakerVolume = tmp;
  eepromfile->Read(wxT("vBatMin"),&tmp,0);
  g_eeGeneral.vBatMin = tmp;
  eepromfile->Read(wxT("vBatMax"),&tmp,0);
  g_eeGeneral.vBatMax = tmp;

  eepromfile->Read(wxT("BTParams"),&tmp,0);
  g_eeGeneral.BTParams = tmp;

  eepromfile->Read(wxT("BTName"),&strtmp,"");
  ConvWxstrToCharFw(strtmp,g_eeGeneral.BTName, 6);
  eepromfile->Read(wxT("BTSlaveName"),&strtmp,"");
  ConvWxstrToCharFw(strtmp,g_eeGeneral.BTSlaveName, 6);
  for (int i=0; i<6; ++i) {
    wxString num = wxString::Format(wxT("%i"),i);
    eepromfile->Read(wxT("BTSlaveMac"+num),&tmp,0);
    g_eeGeneral.BTSlaveMac[i] = tmp;
  }

  for (int i=0; i<4; ++i) { //fixed_ID[4]
    wxString num = wxString::Format(wxT("%i"),i);
    eepromfile->Read(wxT("fixed_ID"+num),&tmp,0);
    g_eeGeneral.fixed_ID.ID_8[i] = tmp;
  }

  theFile.writeRlc(FILE_GENERAL, FILE_TYP_GENERAL, (uint8_t*)&g_eeGeneral, sizeof(EEGeneral), 1);
}

void OpenAVRc_SimulatorFrame::load_EEGeneral_217()
{
  int tmp =0;
  eepromfile->SetPath("/EEGENERAL/");
  eepromfile->Read(wxT("version"),&tmp,0);
  g_eeGeneral.version = EEPROM_VER; //todo convertion dialog

  for (int i=0; i<(NUM_STICKS+NUM_POTS); ++i) { //CalibData calib[NUM_STICKS+NUM_POTS]
    wxString num = wxString::Format(wxT("%i"),i);
    eepromfile->Read(wxT("calib"+num+".mid"),&tmp,0);
    g_eeGeneral.calib[i].mid = tmp;
    eepromfile->Read(wxT("calib"+num+".spanNeg"),&tmp,0);
    g_eeGeneral.calib[i].spanNeg = tmp;
    eepromfile->Read(wxT("calib"+num+".spanPos"),&tmp,0);
    g_eeGeneral.calib[i].spanPos = tmp;
  }

  eepromfile->Read(wxT("chkSum"),g_eeGeneral.chkSum);
  eepromfile->Read(wxT("currModel"),&tmp,0);
  g_eeGeneral.currModel = tmp;
  eepromfile->Read(wxT("contrast"),&tmp,0);
  g_eeGeneral.contrast = tmp;
  eepromfile->Read(wxT("vBatWarn"),&tmp,0);
  g_eeGeneral.vBatWarn = tmp;
  eepromfile->Read(wxT("txVoltageCalibration"),&tmp,0);
  g_eeGeneral.txVoltageCalibration = tmp;
  eepromfile->Read(wxT("backlightMode"),&tmp,0);
  g_eeGeneral.backlightMode = tmp;

  for (int i=0; i<4; ++i) { //TrainerData trainer;
    wxString num = wxString::Format(wxT("%i"),i);
    eepromfile->Read(wxT("trainer.calib"+num),&tmp,0);
    g_eeGeneral.trainer.calib[i] = tmp;
    eepromfile->Read(wxT("trainer.mix"+num+".srcChn"),&tmp,0);
    g_eeGeneral.trainer.mix[i].srcChn = tmp;
    eepromfile->Read(wxT("trainer.mix"+num+".mode"),&tmp,0);
    g_eeGeneral.trainer.mix[i].mode = tmp;
    eepromfile->Read(wxT("trainer.mix"+num+".studWeight"),&tmp,0);
    g_eeGeneral.trainer.mix[i].studWeight = tmp;
  }

  eepromfile->Read(wxT("view"),&tmp,0);
  g_eeGeneral.view = tmp;
  eepromfile->Read(wxT("buzzerMode"),&tmp,0);
  g_eeGeneral.buzzerMode = tmp;
  eepromfile->Read(wxT("fai"),&tmp,0);
  g_eeGeneral.fai = tmp;
  eepromfile->Read(wxT("beepMode"),&tmp,0);
  g_eeGeneral.beepMode = tmp;
  eepromfile->Read(wxT("alarmsFlash"),&tmp,0);
  g_eeGeneral.alarmsFlash = tmp;
  eepromfile->Read(wxT("disableMemoryWarning"),&tmp,0);
  g_eeGeneral.disableMemoryWarning = tmp;
  eepromfile->Read(wxT("disableAlarmWarning"),&tmp,0);
  g_eeGeneral.disableAlarmWarning = tmp;
  eepromfile->Read(wxT("stickMode"),&tmp,0);
  g_eeGeneral.stickMode = tmp;
  eepromfile->Read(wxT("timezone"),&tmp,0);
  g_eeGeneral.timezone = tmp;
  eepromfile->Read(wxT("adjustRTC"),&tmp,0);
  g_eeGeneral.adjustRTC = tmp;
  eepromfile->Read(wxT("inactivityTimer"),&tmp,0);
  g_eeGeneral.inactivityTimer = tmp;
  eepromfile->Read(wxT("mavbaud"),&tmp,0);
  g_eeGeneral.mavbaud = tmp;
  eepromfile->Read(wxT("splashMode"),&tmp,0);
  g_eeGeneral.splashMode = tmp;
  eepromfile->Read(wxT("hapticMode"),&tmp,0);
  g_eeGeneral.hapticMode = tmp;
  eepromfile->Read(wxT("blOffBright"),&tmp,0);
  g_eeGeneral.blOffBright = tmp;
  eepromfile->Read(wxT("blOnBright"),&tmp,0);
  g_eeGeneral.blOnBright = tmp;
  eepromfile->Read(wxT("lightAutoOff"),&tmp,0);
  g_eeGeneral.lightAutoOff = tmp;
  eepromfile->Read(wxT("templateSetup"),&tmp,0);
  g_eeGeneral.templateSetup = tmp;
  eepromfile->Read(wxT("PPM_Multiplier"),&tmp,0);
  g_eeGeneral.PPM_Multiplier = tmp;
  eepromfile->Read(wxT("hapticLength"),&tmp,0);
  g_eeGeneral.hapticLength = tmp;
  eepromfile->Read(wxT("reNavigation"),&tmp,0);
  g_eeGeneral.reNavigation = tmp;
  eepromfile->Read(wxT("stickReverse"),&tmp,0);
  g_eeGeneral.stickReverse = tmp;
  eepromfile->Read(wxT("beepLength"),&tmp,0);
  g_eeGeneral.beepLength = tmp;
  eepromfile->Read(wxT("hapticStrength"),&tmp,0);
  g_eeGeneral.hapticStrength = tmp;
  eepromfile->Read(wxT("gpsFormat"),&tmp,0);
  g_eeGeneral.gpsFormat = tmp;
  eepromfile->Read(wxT("unexpectedShutdown"),&tmp,0);
  g_eeGeneral.unexpectedShutdown = tmp;
  eepromfile->Read(wxT("speakerPitch"),&tmp,0);
  g_eeGeneral.speakerPitch = tmp;
  eepromfile->Read(wxT("speakerVolume"),&tmp,0);
  g_eeGeneral.speakerVolume = tmp;
  eepromfile->Read(wxT("vBatMin"),&tmp,0);
  g_eeGeneral.vBatMin = tmp+90;
  eepromfile->Read(wxT("vBatMax"),&tmp,0);
  g_eeGeneral.vBatMax = tmp+120;

  theFile.writeRlc(FILE_GENERAL, FILE_TYP_GENERAL, (uint8_t*)&g_eeGeneral, sizeof(EEGeneral), 1);
}

void OpenAVRc_SimulatorFrame::save_ModelData_EEPROM_VER()
{
  ModelData temp_model;

  for (uint8_t m=0; m<MAX_MODELS; ++m) {
    wxString num = wxString::Format(wxT("%i"),m+1);
    if (eeModelExists(m)) {
      theFile.openRlc(FILE_MODEL(m));
      theFile.readRlc((uint8_t*)&temp_model, sizeof(temp_model));
      eepromfile->SetPath("/MODEL"+num+"/");
      eepromfile->Write(wxT("name"),ConvCharFwToWxstr(temp_model.name, LEN_MODEL_NAME));
      eepromfile->Write(wxT("modelId"),(int)temp_model.modelId);

      for (int i=0; i<MAX_TIMERS; ++i) { //TimerData timers[MAX_TIMERS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Write(wxT("timers"+num+".mode"),(int)temp_model.timers[i].mode);
        eepromfile->Write(wxT("timers"+num+".start"),(int)temp_model.timers[i].start);
        eepromfile->Write(wxT("timers"+num+".countdownBeep"),(int)temp_model.timers[i].countdownBeep);
        eepromfile->Write(wxT("timers"+num+".minuteBeep"),(int)temp_model.timers[i].minuteBeep);
        eepromfile->Write(wxT("timers"+num+".persistent"),(int)temp_model.timers[i].persistent);
        eepromfile->Write(wxT("timers"+num+".spare"),(int)temp_model.timers[i].spare);
        eepromfile->Write(wxT("timers"+num+".value"),(int)temp_model.timers[i].value);
      }

      eepromfile->Write(wxT("rfProtocol"),(int)temp_model.rfProtocol);
      eepromfile->Write(wxT("thrTrim"),(int)temp_model.thrTrim);
      eepromfile->Write(wxT("rfSubType"),(int)temp_model.rfSubType);
      eepromfile->Write(wxT("trimInc"),(int)temp_model.trimInc);
      eepromfile->Write(wxT("disableThrottleWarning"),(int)temp_model.disableThrottleWarning);
      eepromfile->Write(wxT("rfOptionBool1"),(int)temp_model.rfOptionBool1);
      eepromfile->Write(wxT("rfOptionBool2"),(int)temp_model.rfOptionBool2);
      eepromfile->Write(wxT("rfOptionBool3"),(int)temp_model.rfOptionBool3);
      eepromfile->Write(wxT("extendedLimits"),(int)temp_model.extendedLimits);
      eepromfile->Write(wxT("extendedTrims"),(int)temp_model.extendedTrims);
      eepromfile->Write(wxT("rfOptionValue1"),(int)temp_model.rfOptionValue1);
      eepromfile->Write(wxT("rfOptionValue2"),(int)temp_model.rfOptionValue2);
      eepromfile->Write(wxT("rfOptionValue3"),(int)temp_model.rfOptionValue3);
      eepromfile->Write(wxT("beepANACenter"),(int)temp_model.beepANACenter);

      for (int i=0; i<MAX_MIXERS; ++i) { //MixData   mixData[MAX_MIXERS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Write(wxT("mixData"+num+".destCh"),(int)temp_model.mixData[i].destCh);
        eepromfile->Write(wxT("mixData"+num+".curveMode"),(int)temp_model.mixData[i].curveMode);
        eepromfile->Write(wxT("mixData"+num+".noExpo"),(int)temp_model.mixData[i].noExpo);
        eepromfile->Write(wxT("mixData"+num+".weightMode"),(int)temp_model.mixData[i].weightMode);
        eepromfile->Write(wxT("mixData"+num+".offsetMode"),(int)temp_model.mixData[i].offsetMode);
        eepromfile->Write(wxT("mixData"+num+".srcRaw"),(int)temp_model.mixData[i].srcRaw);
        eepromfile->Write(wxT("mixData"+num+".weight"),(int)temp_model.mixData[i].weight);
        eepromfile->Write(wxT("mixData"+num+".swtch"),(int)temp_model.mixData[i].swtch);
        eepromfile->Write(wxT("mixData"+num+".flightModes"),(int)temp_model.mixData[i].flightModes);
        eepromfile->Write(wxT("mixData"+num+".mltpx"),(int)temp_model.mixData[i].mltpx);
        eepromfile->Write(wxT("mixData"+num+".carryTrim"),(int)temp_model.mixData[i].carryTrim);
        eepromfile->Write(wxT("mixData"+num+".mixWarn"),(int)temp_model.mixData[i].mixWarn);
        eepromfile->Write(wxT("mixData"+num+".spare"),(int)temp_model.mixData[i].spare);
        eepromfile->Write(wxT("mixData"+num+".delayUp"),(int)temp_model.mixData[i].delayUp);
        eepromfile->Write(wxT("mixData"+num+".delayDown"),(int)temp_model.mixData[i].delayDown);
        eepromfile->Write(wxT("mixData"+num+".speedUp"),(int)temp_model.mixData[i].speedUp);
        eepromfile->Write(wxT("mixData"+num+".speedDown"),(int)temp_model.mixData[i].speedDown);
        eepromfile->Write(wxT("mixData"+num+".curveParam"),(int)temp_model.mixData[i].curveParam);
        eepromfile->Write(wxT("mixData"+num+".offset"),(int)temp_model.mixData[i].offset);
      }

      for (int i=0; i<NUM_CHNOUT; ++i) { //LimitData limitData[NUM_CHNOUT];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Write(wxT("limitData"+num+".min"),(int)temp_model.limitData[i].min);
        eepromfile->Write(wxT("limitData"+num+".max"),(int)temp_model.limitData[i].max);
        eepromfile->Write(wxT("limitData"+num+".ppmCenter"),(int)temp_model.limitData[i].ppmCenter);
        eepromfile->Write(wxT("limitData"+num+".offset"),(int)temp_model.limitData[i].offset);
        eepromfile->Write(wxT("limitData"+num+".symetrical"),(int)temp_model.limitData[i].symetrical);
        eepromfile->Write(wxT("limitData"+num+".revert"),(int)temp_model.limitData[i].revert);
      }

      for (int i=0; i<MAX_EXPOS; ++i) { //ExpoData  expoData[MAX_EXPOS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Write(wxT("expoData"+num+".mode"),(int)temp_model.expoData[i].mode);
        eepromfile->Write(wxT("expoData"+num+".chn"),(int)temp_model.expoData[i].chn);
        eepromfile->Write(wxT("expoData"+num+".curveMode"),(int)temp_model.expoData[i].curveMode);
        eepromfile->Write(wxT("expoData"+num+".spare"),(int)temp_model.expoData[i].spare);
        eepromfile->Write(wxT("expoData"+num+".flightModes"),(int)temp_model.expoData[i].flightModes);
        eepromfile->Write(wxT("expoData"+num+".swtch"),(int)temp_model.expoData[i].swtch);
        eepromfile->Write(wxT("expoData"+num+".weight"),(int)temp_model.expoData[i].weight);
        eepromfile->Write(wxT("expoData"+num+".curveParam"),(int)temp_model.expoData[i].curveParam);
      }

      for (int i=0; i<MAX_CURVES; ++i) { //CURVDATA(=int8)  curves[MAX_CURVES];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Write(wxT("curves"+num),(int)temp_model.curves[i]);
      }

      for (int i=0; i<NUM_POINTS; ++i) { //int8_t    points[NUM_POINTS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Write(wxT("points"+num),(int)temp_model.points[i]);
      }

      for (int i=0; i<NUM_LOGICAL_SWITCH; ++i) { //LogicalSwitchData logicalSw[NUM_LOGICAL_SWITCH];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Write(wxT("logicalSw"+num+".v1"),(int)temp_model.logicalSw[i].v1);
        eepromfile->Write(wxT("logicalSw"+num+".v2"),(int)temp_model.logicalSw[i].v2);
        eepromfile->Write(wxT("logicalSw"+num+".func"),(int)temp_model.logicalSw[i].func);
        eepromfile->Write(wxT("logicalSw"+num+".andsw"),(int)temp_model.logicalSw[i].andsw);
      }

      for (int i=0; i<NUM_CFN; ++i) { //CustomFunctionData customFn[NUM_CFN];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Write(wxT("customFn"+num+".swtch"),(int)temp_model.customFn[i].swtch);
        eepromfile->Write(wxT("customFn"+num+".func"),(int)temp_model.customFn[i].func);
        eepromfile->Write(wxT("customFn"+num+".mode"),(int)temp_model.customFn[i].mode);
        eepromfile->Write(wxT("customFn"+num+".param"),(int)temp_model.customFn[i].param);
        eepromfile->Write(wxT("customFn"+num+".active"),(int)temp_model.customFn[i].active);
        eepromfile->Write(wxT("customFn"+num+".spare"),(int)temp_model.customFn[i].spare);
        eepromfile->Write(wxT("customFn"+num+".value"),(int)temp_model.customFn[i].value);
      }

      eepromfile->Write(wxT("swashR.invertELE"),(int)temp_model.swashR.invertELE); //SwashRingData swashR;
      eepromfile->Write(wxT("swashR.invertAIL"),(int)temp_model.swashR.invertAIL);
      eepromfile->Write(wxT("swashR.invertCOL"),(int)temp_model.swashR.invertCOL);
      eepromfile->Write(wxT("swashR.type"),(int)temp_model.swashR.type);
      eepromfile->Write(wxT("swashR.collectiveSource"),(int)temp_model.swashR.collectiveSource);
      eepromfile->Write(wxT("swashR.value"),(int)temp_model.swashR.value);

      for (int i=0; i<MAX_FLIGHT_MODES; ++i) { //FlightModeData flightModeData[MAX_FLIGHT_MODES];
        num = wxString::Format(wxT("%i"),i);

        for (int j=0; j<4; ++j) { //int16 trim[4] TRIMS_ARRAY;
          wxString numtrim = wxString::Format(wxT("%i"),j);
          eepromfile->Write(wxT("flightModeData"+num+".trim"+numtrim),(int)temp_model.flightModeData[i].trim[j]);
        }

        eepromfile->Write(wxT("flightModeData"+num+".swtch"),(int)temp_model.flightModeData[i].swtch);
        eepromfile->Write(wxT("flightModeData"+num+".name"),ConvCharFwToWxstr(temp_model.flightModeData[i].name, LEN_FLIGHT_MODE_NAME));
        eepromfile->Write(wxT("flightModeData"+num+".fadeIn"),(int)temp_model.flightModeData[i].fadeIn);
        eepromfile->Write(wxT("flightModeData"+num+".fadeOut"),(int)temp_model.flightModeData[i].fadeOut);

        for (int j=0; j<NUM_ROTARY_ENCODERS; ++j) { //ROTARY_ENCODER_ARRAY int16_t rotaryEncoders[2];
          wxString numtrim = wxString::Format(wxT("%i"),j);
          eepromfile->Write(wxT("flightModeData"+num+".rotaryEncoders"+numtrim),(int)temp_model.flightModeData[i].rotaryEncoders[j]);
        }

        for (int k=0; k<MAX_GVARS; ++k) { //PHASE_GVARS_DATA gvar_t gvars[MAX_GVARS]
          wxString numgvar = wxString::Format(wxT("%i"),k);
          eepromfile->Write(wxT("flightModeData"+num+".gvars"+numgvar),(int)temp_model.flightModeData[i].gvars[k]);
        }
      }

      eepromfile->Write(wxT("rfOptionValue1"),(int)temp_model.rfOptionValue1);
      eepromfile->Write(wxT("rfOptionValue2"),(int)temp_model.rfOptionValue2);
      eepromfile->Write(wxT("rfOptionValue3"),(int)temp_model.rfOptionValue3);
      eepromfile->Write(wxT("thrTraceSrc"),(int)temp_model.thrTraceSrc);
      eepromfile->Write(wxT("thrSwitch"),(int)temp_model.thrSwitch);
      eepromfile->Write(wxT("switchWarningState"),(int)temp_model.switchWarningState);
      eepromfile->Write(wxT("switchWarningEnable"),(int)temp_model.switchWarningEnable);

      for (int i=0; i<MAX_GVARS; ++i) { //global_gvar_t gvars[MAX_GVARS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Write(wxT("gvars"+num+".name"),ConvCharFwToWxstr(temp_model.gvars[i].name, LEN_GVAR_NAME));
      }

      eepromfile->Write(wxT("UnusedModel"),(int)temp_model.UnusedModel);

      for (int i=0; i<MAX_FRSKY_A_CHANNELS; ++i) { //FrSkyChannelData channels[MAX_FRSKY_A_CHANNELS];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Write(wxT("telemetry.channels"+num+".ratio"),(int)temp_model.telemetry.channels[i].ratio);
        eepromfile->Write(wxT("telemetry.channels"+num+".offset"),(int)temp_model.telemetry.channels[i].offset);
        eepromfile->Write(wxT("telemetry.channels"+num+".type"),(int)temp_model.telemetry.channels[i].type);
        /*for (int j=0; j<2; ++j) { //alarms_value[2];
          wxString numvalue = wxString::Format(wxT("%i"),j);
          eepromfile->Write(wxT("telemetry.channels"+num+".alarms_value"+numvalue),(int)temp_model.telemetry.channels[i].alarms_value[j]);
        }
        eepromfile->Write(wxT("telemetry.channels"+num+".alarms_level"),(int)temp_model.telemetry.channels[i].alarms_level);
        eepromfile->Write(wxT("telemetry.channels"+num+".alarms_greater"),(int)temp_model.telemetry.channels[i].alarms_greater);*/
        eepromfile->Write(wxT("telemetry.channels"+num+".multiplier"),(int)temp_model.telemetry.channels[i].multiplier);
      }
      eepromfile->Write(wxT("telemetry.usrProto"),(int)temp_model.telemetry.usrProto);
      eepromfile->Write(wxT("telemetry.blades"),(int)temp_model.telemetry.blades);
      eepromfile->Write(wxT("telemetry.screensType"),(int)temp_model.telemetry.screensType);
      eepromfile->Write(wxT("telemetry.voltsSource"),(int)temp_model.telemetry.voltsSource);

      eepromfile->Write(wxT("telemetry.rssiAlarm"),(int)temp_model.telemetry.rssiAlarm);

      eepromfile->Write(wxT("telemetry.varioMin"),(int)temp_model.telemetry.varioMin);
      eepromfile->Write(wxT("telemetry.varioMax"),(int)temp_model.telemetry.varioMax);
      /*for (int i=0; i<2; ++i) { //FrSkyRSSIAlarm rssiAlarms[2];
        num = wxString::Format(wxT("%i"),i);
        eepromfile->Write(wxT("telemetry.rssiAlarms"+num+".level"),(int)temp_model.telemetry.rssiAlarms[i].level);
        eepromfile->Write(wxT("telemetry.rssiAlarms"+num+".value"),(int)temp_model.telemetry.rssiAlarms[i].value);
      }*/
      for (int i=0; i<MAX_TELEMETRY_SCREENS; ++i) { //FrSkyScreenData screens[MAX_TELEMETRY_SCREENS];
        num = wxString::Format(wxT("%i"),i);
        for (int j=0; j<4; ++j) { //FrSkyBarData bars[4]; or FrSkyLineData
          wxString numbl = wxString::Format(wxT("%i"),j);
          eepromfile->Write(wxT("telemetry.screens"+num+".bars"+numbl+"source"),(int)temp_model.telemetry.screens[i].bars[j].source);
          eepromfile->Write(wxT("telemetry.screens"+num+".bars"+numbl+"barMin"),(int)temp_model.telemetry.screens[i].bars[j].barMin);
          eepromfile->Write(wxT("telemetry.screens"+num+".bars"+numbl+"barMax"),(int)temp_model.telemetry.screens[i].bars[j].barMax);
        }
      }
      eepromfile->Write(wxT("telemetry.varioSource"),(int)temp_model.telemetry.varioSource);
      eepromfile->Write(wxT("telemetry.varioCenterMin"),(int)temp_model.telemetry.varioCenterMin);
      eepromfile->Write(wxT("telemetry.currentSource"),(int)temp_model.telemetry.currentSource);
      eepromfile->Write(wxT("telemetry.varioCenterMax"),(int)temp_model.telemetry.varioCenterMax);
      eepromfile->Write(wxT("telemetry.fasOffset"),(int)temp_model.telemetry.fasOffset);

      for (int i=0; i<NUM_MAX_X_ANY; ++i)
        {
          num = wxString::Format(wxT("%i"),i);
          eepromfile->Write(wxT("Xany"+num+"Active"),(int)temp_model.Xany[i].Active);
          eepromfile->Write(wxT("Xany"+num+"ChId"),(int)temp_model.Xany[i].ChId);
          eepromfile->Write(wxT("Xany"+num+"RepeatNb"),(int)temp_model.Xany[i].RepeatNb);
          eepromfile->Write(wxT("Xany"+num+"Reserved1"),(int)temp_model.Xany[i].Reserved1);
          eepromfile->Write(wxT("Xany"+num+"PayloadCfg.AngleSrcIdx"),(int)temp_model.Xany[i].PayloadCfg.AngleSrcIdx);
          eepromfile->Write(wxT("Xany"+num+"PayloadCfg.PropSrcIdx"),(int)temp_model.Xany[i].PayloadCfg.PropSrcIdx);
          eepromfile->Write(wxT("Xany"+num+"PayloadCfg.SwitchSrcIdx"),(int)temp_model.Xany[i].PayloadCfg.SwitchSrcIdx);
          eepromfile->Write(wxT("Xany"+num+"PayloadCfg.FreeCfgBits"),(int)temp_model.Xany[i].PayloadCfg.FreeCfgBits);
          eepromfile->Write(wxT("Xany"+num+"Param.Raw"),(int)temp_model.Xany[i].Param.Raw);
          eepromfile->Write(wxT("Xany"+num+"Rfu"),(int)temp_model.Xany[i].Rfu);
        }
    }
  }
}

void OpenAVRc_SimulatorFrame::save_EEGeneral_EEPROM_VER(EEGeneral General)
{
  eepromfile->SetPath("/EEGENERAL/");
  eepromfile->Write(wxT("version"),General.version);

  for (int i=0; i<(NUM_STICKS+NUM_POTS); ++i) { //CalibData calib[NUM_STICKS+NUM_POTS]
    wxString num = wxString::Format(wxT("%i"),i);
    eepromfile->Write(wxT("calib"+num+".mid"),(int)General.calib[i].mid);
    eepromfile->Write(wxT("calib"+num+".spanNeg"),(int)General.calib[i].spanNeg);
    eepromfile->Write(wxT("calib"+num+".spanPos"),(int)General.calib[i].spanPos);
  }

  eepromfile->Write(wxT("chkSum"),General.chkSum);
  eepromfile->Write(wxT("currModel"),(int)General.currModel);
  eepromfile->Write(wxT("contrast"),General.contrast);
  eepromfile->Write(wxT("vBatWarn"),General.vBatWarn);
  eepromfile->Write(wxT("txVoltageCalibration"),(int)General.txVoltageCalibration);
  eepromfile->Write(wxT("backlightMode"),(int)General.backlightMode);

  for (int i=0; i<4; ++i) { //TrainerData trainer;
    wxString num = wxString::Format(wxT("%i"),i);
    eepromfile->Write(wxT("trainer.calib"+num),(int)General.trainer.calib[i]);
    eepromfile->Write(wxT("trainer.mix"+num+".srcChn"),General.trainer.mix[i].srcChn);
    eepromfile->Write(wxT("trainer.mix"+num+".mode"),General.trainer.mix[i].mode);
    eepromfile->Write(wxT("trainer.mix"+num+".studWeight"),(int)General.trainer.mix[i].studWeight);
  }

  eepromfile->Write(wxT("view"),General.view);
  eepromfile->Write(wxT("buzzerMode"),(int)General.buzzerMode);
  eepromfile->Write(wxT("fai"),General.fai);
  eepromfile->Write(wxT("beepMode"),(int)General.beepMode);
  eepromfile->Write(wxT("alarmsFlash"),General.alarmsFlash);
  eepromfile->Write(wxT("disableMemoryWarning"),General.disableMemoryWarning);
  eepromfile->Write(wxT("disableAlarmWarning"),General.disableAlarmWarning);
  eepromfile->Write(wxT("stickMode"),General.stickMode);
  eepromfile->Write(wxT("timezone"),(int)General.timezone);
  eepromfile->Write(wxT("adjustRTC"),General.adjustRTC);
  eepromfile->Write(wxT("inactivityTimer"),General.inactivityTimer);
  eepromfile->Write(wxT("mavbaud"),General.mavbaud);
  eepromfile->Write(wxT("splashMode"),General.splashMode);
  eepromfile->Write(wxT("hapticMode"),(int)General.hapticMode);
  eepromfile->Write(wxT("blOffBright"),General.blOffBright);
  eepromfile->Write(wxT("blOnBright"),General.blOnBright);
  eepromfile->Write(wxT("lightAutoOff"),General.lightAutoOff);
  eepromfile->Write(wxT("templateSetup"),General.templateSetup);
  eepromfile->Write(wxT("PPM_Multiplier"),(int)General.PPM_Multiplier);
  eepromfile->Write(wxT("hapticLength"),(int)General.hapticLength);
  eepromfile->Write(wxT("reNavigation"),General.reNavigation);
  eepromfile->Write(wxT("protocol_mask"),General.protocol_mask);
  eepromfile->Write(wxT("stickReverse"),General.stickReverse);
  eepromfile->Write(wxT("beepLength"),(int)General.beepLength);
  eepromfile->Write(wxT("hapticStrength"),(int)General.hapticStrength);
  eepromfile->Write(wxT("gpsFormat"),General.gpsFormat);
  eepromfile->Write(wxT("unexpectedShutdown"),General.unexpectedShutdown);
  eepromfile->Write(wxT("speakerPitch"),General.speakerPitch);
  eepromfile->Write(wxT("speakerVolume"),(int)General.speakerVolume);
  eepromfile->Write(wxT("vBatMin"),(int)General.vBatMin);
  eepromfile->Write(wxT("vBatMax"),(int)General.vBatMax);

  eepromfile->Write(wxT("BTParams"),(int)General.BTParams);
  eepromfile->Write(wxT("BTName"),ConvCharFwToWxstr(General.BTName, 6));
  eepromfile->Write(wxT("BTSlaveName"),ConvCharFwToWxstr(General.BTSlaveName, 6));
  for (int i=0; i<6; ++i) {
    wxString num = wxString::Format(wxT("%i"),i);
    eepromfile->Write(wxT("BTSlaveMac"+num),(int)General.BTSlaveMac[i]);
  }

  for (int i=0; i<4; ++i) { // fixed_ID[4]
    wxString num = wxString::Format(wxT("%i"),i);
    eepromfile->Write(wxT("fixed_ID"+num),(int)General.fixed_ID.ID_8[i]);
  }

}

//////////////////////////////VIRTUAL PIN WORD !! JOB /////////////////////////

void OpenAVRc_SimulatorFrame::OnBPmenuLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinL->ResetPin(4);
}

void OpenAVRc_SimulatorFrame::OnBPmenuLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinL->SetPin(4);
}

void OpenAVRc_SimulatorFrame::OnBPexitLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinL->ResetPin(5);
}

void OpenAVRc_SimulatorFrame::OnBPexitLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinL->SetPin(5);
}

void OpenAVRc_SimulatorFrame::OnBPhLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinL->ResetPin(1);
}

void OpenAVRc_SimulatorFrame::OnBPhLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinL->SetPin(1);
}

void OpenAVRc_SimulatorFrame::OnBPbLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinL->ResetPin(0);
}

void OpenAVRc_SimulatorFrame::OnBPbLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinL->SetPin(0);
}

void OpenAVRc_SimulatorFrame::OnBPgLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinL->ResetPin(3);
}

void OpenAVRc_SimulatorFrame::OnBPgLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinL->SetPin(3);
}

void OpenAVRc_SimulatorFrame::OnBPdLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinL->ResetPin(2);
}

void OpenAVRc_SimulatorFrame::OnBPdLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinL->SetPin(2);
}

void OpenAVRc_SimulatorFrame::OnLlTrimLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinF->ResetPin(7);
}

void OpenAVRc_SimulatorFrame::OnLlTrimLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinF->SetPin(7);
}

void OpenAVRc_SimulatorFrame::OnLrTrimLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinF->ResetPin(6);
}

void OpenAVRc_SimulatorFrame::OnLrTrimLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinF->SetPin(6);
}

void OpenAVRc_SimulatorFrame::OnLdTrimLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinF->ResetPin(5);
}

void OpenAVRc_SimulatorFrame::OnLdTrimLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinF->SetPin(5);
}

void OpenAVRc_SimulatorFrame::OnLuTrimLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinF->ResetPin(4);
}

void OpenAVRc_SimulatorFrame::OnLuTrimLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinF->SetPin(4);
}

void OpenAVRc_SimulatorFrame::OnRdTrimLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinF->ResetPin(3);
}

void OpenAVRc_SimulatorFrame::OnRdTrimLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinF->SetPin(3);
}

void OpenAVRc_SimulatorFrame::OnRuTrimLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinF->ResetPin(2);
}

void OpenAVRc_SimulatorFrame::OnRuTrimLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinF->SetPin(2);
}

void OpenAVRc_SimulatorFrame::OnRlTrimLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinF->ResetPin(1);
}

void OpenAVRc_SimulatorFrame::OnRlTrimLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinF->SetPin(1);
}

void OpenAVRc_SimulatorFrame::OnRrTrimLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinF->ResetPin(0);
}

void OpenAVRc_SimulatorFrame::OnRrTrimLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinF->SetPin(0);
}

void OpenAVRc_SimulatorFrame::OnBpTrnLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinL->ResetPin(7);
}

void OpenAVRc_SimulatorFrame::OnBpTrnLeftUp(wxMouseEvent& event)
{
  event.Skip();
  SpinL->SetPin(7);
}

void OpenAVRc_SimulatorFrame::OnBpTrnRightDown(wxMouseEvent& event)
{
  event.Skip();
  SpinL->ResetPin(7);
  CheckInputs();
}

void OpenAVRc_SimulatorFrame::OnBpThrLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinG->TogglePin(2);
}

void OpenAVRc_SimulatorFrame::OnBpRudLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinG->TogglePin(0);
}

void OpenAVRc_SimulatorFrame::OnBpEleLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinL->TogglePin(6);
}

void OpenAVRc_SimulatorFrame::OnBpAilLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinD->TogglePin(7);
}

void OpenAVRc_SimulatorFrame::OnBpGeaLeftDown(wxMouseEvent& event)
{
  event.Skip();
  SpinG->TogglePin(1);
}

void OpenAVRc_SimulatorFrame::OnBpId1LeftDown(wxMouseEvent& event)
{
  event.Skip();
  if (!SpinC->GetPin(1)) SpinC->TogglePin(1);
  else SpinC->TogglePin(0);
}

void OpenAVRc_SimulatorFrame::OnBpId2LeftDown(wxMouseEvent& event)
{
  event.Skip();
  if (!SpinC->GetPin(0)) SpinC->TogglePin(0);
  else SpinC->TogglePin(1);
}

void OpenAVRc_SimulatorFrame::OnBpReaLeftDown(wxMouseEvent& event)
{
  event.Skip();
#if defined(PCBMEGAMINI)
  SpinG->ResetPin(5);
#else
  SpinJ->ResetPin(0);
#endif
}

void OpenAVRc_SimulatorFrame::OnBpReaLeftUp(wxMouseEvent& event)
{
  event.Skip();
#if defined(PCBMEGAMINI)
  SpinG->SetPin(5);
#else
  SpinJ->SetPin(0);
#endif
}

void OpenAVRc_SimulatorFrame::OnBpRebLeftDown(wxMouseEvent& event)
{
  event.Skip();
#if defined(PCBMEGAMINI)
  SpinE->ResetPin(3);
#else
  SpinJ->ResetPin(1);
#endif
}

void OpenAVRc_SimulatorFrame::OnBpRebLeftUp(wxMouseEvent& event)
{
  event.Skip();
#if defined(PCBMEGAMINI)
  SpinE->SetPin(3);
#else
  SpinJ->SetPin(1);
#endif
}

void OpenAVRc_SimulatorFrame::CheckInputs()
{
  //Rea & Reb
  if (SpinRea->GetValue() > 0) {
    SpinRea->SetValue(0);
    SpinE->ResetPin(4);
    SpinE->ResetPin(5);
    INT5_vect();
    SpinE->SetPin(4);
    SpinE->SetPin(5);
  }

  if (SpinRea->GetValue() < 0) {
    SpinRea->SetValue(0);
    SpinE->ResetPin(4);
    SpinE->ResetPin(5);
    INT4_vect();
    SpinE->SetPin(4);
    SpinE->SetPin(5);
  }

  if (SpinReb->GetValue() > 0) {
    SpinReb->SetValue(0);
#if defined(PCBMEGAMINI)
    SpinE->ResetPin(7);
    SpinE->ResetPin(6);
    INT6_vect();
    SpinE->SetPin(7);
    SpinE->SetPin(6);
#else
    SpinD->ResetPin(2);
    SpinD->ResetPin(3);
    INT3_vect();
    SpinD->SetPin(2);
    SpinD->SetPin(3);
#endif
  }

  if (SpinReb->GetValue() < 0) {
    SpinReb->SetValue(0);
#if defined(PCBMEGAMINI)
    SpinE->ResetPin(7);
    SpinE->ResetPin(6);
    INT7_vect();
    SpinE->SetPin(7);
    SpinE->SetPin(6);
#else
    SpinD->ResetPin(2);
    SpinD->ResetPin(3);
    INT2_vect();
    SpinD->SetPin(2);
    SpinD->SetPin(3);
#endif
  }

  //Pots1,2&3
  s_anaFilt[4] = (uint16_t)Pot1->GetValue();
  s_anaFilt[5] = (uint16_t)Pot2->GetValue();
  s_anaFilt[6] = (uint16_t)Pot3->GetValue();

  if (!SpinL->GetPin(4)) BPmenu->SetBackgroundColour(Col_Button_On);
  else BPmenu->SetBackgroundColour(Col_Button_Off);
  BPmenu->Refresh();

  if (!SpinL->GetPin(5)) BPexit->SetBackgroundColour(Col_Button_On);
  else BPexit->SetBackgroundColour(Col_Button_Off);
  BPexit->Refresh();

  if (!SpinL->GetPin(1)) BPh->SetBackgroundColour(Col_Button_On);
  else BPh->SetBackgroundColour(Col_Button_Off);
  BPh->Refresh();

  if (!SpinL->GetPin(0)) BPb->SetBackgroundColour(Col_Button_On);
  else BPb->SetBackgroundColour(Col_Button_Off);
  BPb->Refresh();

  if (!SpinL->GetPin(3)) BPg->SetBackgroundColour(Col_Button_On);
  else BPg->SetBackgroundColour(Col_Button_Off);
  BPg->Refresh();

  if (!SpinL->GetPin(2)) BPd->SetBackgroundColour(Col_Button_On);
  else BPd->SetBackgroundColour(Col_Button_Off);
  BPd->Refresh();

  if (!SpinF->GetPin(7)) LlTrim->SetBackgroundColour(Col_Button_On);
  else LlTrim->SetBackgroundColour(Col_Button_Off);
  LlTrim->Refresh();

  if (!SpinF->GetPin(6)) LrTrim->SetBackgroundColour(Col_Button_On);
  else LrTrim->SetBackgroundColour(Col_Button_Off);
  LrTrim->Refresh();

  if (!SpinF->GetPin(5)) LdTrim->SetBackgroundColour(Col_Button_On);
  else LdTrim->SetBackgroundColour(Col_Button_Off);
  LdTrim->Refresh();

  if (!SpinF->GetPin(4)) LuTrim->SetBackgroundColour(Col_Button_On);
  else LuTrim->SetBackgroundColour(Col_Button_Off);
  LuTrim->Refresh();

  if (!SpinF->GetPin(3)) RdTrim->SetBackgroundColour(Col_Button_On);
  else RdTrim->SetBackgroundColour(Col_Button_Off);
  RdTrim->Refresh();

  if (!SpinF->GetPin(2)) RuTrim->SetBackgroundColour(Col_Button_On);
  else RuTrim->SetBackgroundColour(Col_Button_Off);
  RuTrim->Refresh();

  if (!SpinF->GetPin(1)) RlTrim->SetBackgroundColour(Col_Button_On);
  else RlTrim->SetBackgroundColour(Col_Button_Off);
  RlTrim->Refresh();

  if (!SpinF->GetPin(0)) RrTrim->SetBackgroundColour(Col_Button_On);
  else RrTrim->SetBackgroundColour(Col_Button_Off);
  RrTrim->Refresh();

  if (!SpinG->GetPin(2)) BpThr->SetBackgroundColour(Col_Button_On);
  else BpThr->SetBackgroundColour(Col_Button_Off);
  BpThr->Refresh();

  if (!SpinG->GetPin(0)) BpRud->SetBackgroundColour(Col_Button_On);
  else BpRud->SetBackgroundColour(Col_Button_Off);
  BpRud->Refresh();

  if (!SpinL->GetPin(6)) BpEle->SetBackgroundColour(Col_Button_On);
  else BpEle->SetBackgroundColour(Col_Button_Off);
  BpEle->Refresh();

  if (!SpinD->GetPin(7)) BpAil->SetBackgroundColour(Col_Button_On);
  else BpAil->SetBackgroundColour(Col_Button_Off);
  BpAil->Refresh();

  if (!SpinG->GetPin(1)) BpGea->SetBackgroundColour(Col_Button_On);
  else BpGea->SetBackgroundColour(Col_Button_Off);
  BpGea->Refresh();

  if (!SpinL->GetPin(7)) BpTrn->SetBackgroundColour(Col_Button_On);
  else BpTrn->SetBackgroundColour(Col_Button_Off);
  BpTrn->Refresh();

  if (!SpinC->GetPin(0)) BpId1->SetBackgroundColour(Col_Button_On);
  else BpId1->SetBackgroundColour(Col_Button_Off);
  BpId1->Refresh();

  if (!SpinC->GetPin(1)) BpId2->SetBackgroundColour(Col_Button_On);
  else BpId2->SetBackgroundColour(Col_Button_Off);
  BpId2->Refresh();

#if defined(PCBMEGAMINI)
  if (!SpinG->GetPin(5)) BpRea->SetBackgroundColour(Col_Button_On);
#else
  if (!SpinJ->GetPin(0)) BpRea->SetBackgroundColour(Col_Button_On);
#endif
  else BpRea->SetBackgroundColour(Col_Button_Off);
  BpRea->Refresh();

 #if defined(PCBMEGAMINI)
 if (!SpinE->GetPin(3)) BpReb->SetBackgroundColour(Col_Button_On);
#else
  if (!SpinJ->GetPin(1)) BpReb->SetBackgroundColour(Col_Button_On);
#endif
  else BpReb->SetBackgroundColour(Col_Button_Off);
  BpReb->Refresh();

}

///// UTILS ///////////

wxColour OpenAVRc_SimulatorFrame::SetColour(wxColour mem)
{
  wxColourDialog ColourDlg(this);
  ColourDlg.GetColourData().SetChooseFull(true);
  if (ColourDlg.ShowModal() == wxID_OK) {
    Ini_Changed = true;
    return ColourDlg.GetColourData().GetColour().GetAsString();
  }
  return mem;
}

void OpenAVRc_SimulatorFrame::OnMenuLcdBackSelected(wxCommandEvent& event)
{
  event.Skip();
  Col_Lcd_Back = SetColour(Col_Lcd_Back);
  DrawWxSimuLcd();
}

void OpenAVRc_SimulatorFrame::OnMenuLcdPixelSelected(wxCommandEvent& event)
{
  event.Skip();
  Col_Lcd_Front = SetColour(Col_Lcd_Front);
  DrawWxSimuLcd();
}

void OpenAVRc_SimulatorFrame::OnMenuButOffSelected(wxCommandEvent& event)
{
  event.Skip();
  Col_Button_Off = SetColour(Col_Button_Off);
}

void OpenAVRc_SimulatorFrame::OnMenuButOnSelected(wxCommandEvent& event)
{
  event.Skip();
  Col_Button_On = SetColour(Col_Button_On);
}

void OpenAVRc_SimulatorFrame::OnMenuStickBackSelected(wxCommandEvent& event)
{
  event.Skip();
  Col_Stick_Back = SetColour(Col_Stick_Back);
  Lstick->SetBackgroundColour(Col_Stick_Back);
  Rstick->SetBackgroundColour(Col_Stick_Back);
  Lstick->Refresh();
  Rstick->Refresh();
}

void OpenAVRc_SimulatorFrame::OnMenuStickStickSelected(wxCommandEvent& event)
{
  Col_Stick_Circle = SetColour(Col_Stick_Circle);
  Lstick->SetForegroundColour(Col_Stick_Circle);
  Rstick->SetForegroundColour(Col_Stick_Circle);
}

void OpenAVRc_SimulatorFrame::OnLstickPaint(wxPaintEvent& event)
{
  event.Skip();
  PaintSticks(false,L_mid,L_mid,Lstick);
}

void OpenAVRc_SimulatorFrame::OnRstickPaint(wxPaintEvent& event)
{
  event.Skip();
  PaintSticks(false,R_mid,R_mid,Rstick);
}

void OpenAVRc_SimulatorFrame::OnButtonStartDesktopClick(wxCommandEvent& event)
{
  event.Skip();
  wxString desktop("OpenAVRc_Desktop.exe");
  wxExecute(desktop);
}

void OpenAVRc_SimulatorFrame::OnKey(wxKeyEvent& event)
{
  if (this->IsActive()) {

    wxMouseEvent fakevt;

    int result = event.GetKeyCode();

    if (event.GetKeyCode() == 315) {
      if (event.GetEventType() == wxEVT_KEY_DOWN) OnBPhLeftDown(fakevt);
      else OnBPhLeftUp(fakevt);
    }
    if (event.GetKeyCode() == 317) {
      if (event.GetEventType() == wxEVT_KEY_DOWN) OnBPbLeftDown(fakevt);
      else OnBPbLeftUp(fakevt);
    }
    if (event.GetKeyCode() == 314) {
      if (event.GetEventType() == wxEVT_KEY_DOWN) OnBPgLeftDown(fakevt);
      else OnBPgLeftUp(fakevt);
    }
    if (event.GetKeyCode() == 316) {
      if (event.GetEventType() == wxEVT_KEY_DOWN) OnBPdLeftDown(fakevt);
      else OnBPdLeftUp(fakevt);
    }
    if (event.GetKeyCode() == 366) {
      if (event.GetEventType() == wxEVT_KEY_DOWN) OnBPmenuLeftDown(fakevt);
      else OnBPmenuLeftUp(fakevt);
    }
    if (event.GetKeyCode() == 367) {
      if (event.GetEventType() == wxEVT_KEY_DOWN) OnBPexitLeftDown(fakevt);
      else OnBPexitLeftUp(fakevt);
    }
    if (event.GetKeyCode() == 65) {
      if (event.GetEventType() == wxEVT_KEY_DOWN) OnBpThrLeftDown(fakevt);
    }
    if (event.GetKeyCode() == 90) {
      if (event.GetEventType() == wxEVT_KEY_DOWN) OnBpRudLeftDown(fakevt);
    }
    if (event.GetKeyCode() == 69) {
      if (event.GetEventType() == wxEVT_KEY_DOWN) OnBpEleLeftDown(fakevt);
    }
    if (event.GetKeyCode() == 82) {
      if (event.GetEventType() == wxEVT_KEY_DOWN) OnBpTrnLeftDown(fakevt);
      else OnBpTrnLeftUp(fakevt);
    }
    if (event.GetKeyCode() == 84) {
      if (event.GetEventType() == wxEVT_KEY_DOWN) OnBpAilLeftDown(fakevt);
    }
    if (event.GetKeyCode() == 89) {
      if (event.GetEventType() == wxEVT_KEY_DOWN) OnBpGeaLeftDown(fakevt);
    }
    if (event.GetKeyCode() == 85) {
      if (event.GetEventType() == wxEVT_KEY_DOWN) OnBpId1LeftDown(fakevt);
    }
    if (event.GetKeyCode() == 73) {
      if (event.GetEventType() == wxEVT_KEY_DOWN) OnBpId2LeftDown(fakevt);
    }
  }
  event.Skip(true);
}

void OpenAVRc_SimulatorFrame::OnMixeurSelected(wxCommandEvent& event)//Draws the mixer display screen
{
  event.Skip();
  MixFr = new  MixerFrame(this);
  MixFr->Show(TRUE);
  MenuFrame->Enable(ID_MENUITEMOUTPUTMIXER, false);
}

void OpenAVRc_SimulatorFrame::EnableMixerFrameMenu()
{
  MenuFrame->Enable(ID_MENUITEMOUTPUTMIXER, true);
}

void OpenAVRc_SimulatorFrame::OnOutputBarsSelected(wxCommandEvent& event)
{
  event.Skip();
  BarFr = new  OutBarsFrame(this);
  BarFr->Show(TRUE);
  MenuFrame->Enable(ID_MENUITEMOUTPUTOUTPUT, false);
}

void OpenAVRc_SimulatorFrame::EnableOutputBarsMenu()
{
  MenuFrame->Enable(ID_MENUITEMOUTPUTOUTPUT, true);
}

void OpenAVRc_SimulatorFrame::OnOutputGvarsSelected(wxCommandEvent& event)
{
  event.Skip();
  GvFr = new  GvarsFrame(this);
  GvFr->Show(TRUE);
  MenuFrame->Enable(ID_MENUITEMOUTPUTGVARS, false);
}

void OpenAVRc_SimulatorFrame::EnableOutputGvarsMenu()
{
  MenuFrame->Enable(ID_MENUITEMOUTPUTGVARS, true);
}

void OpenAVRc_SimulatorFrame::OnRadioDataSelected(wxCommandEvent& event)
{
  event.Skip();
  RaFr = new  RadioDataFrame(this);
  RaFr->Show(TRUE);
  MenuFrame->Enable(ID_MENUITEMRADIODATA, false);
}

void OpenAVRc_SimulatorFrame::EnableRadioDataMenu()
{
  MenuFrame->Enable(ID_MENUITEMRADIODATA, true);
}

void OpenAVRc_SimulatorFrame::OntelemetrySelected(wxCommandEvent& event)
{
  event.Skip();
  TeleFr = new  TelemetryFrame(this);
  TeleFr->Show(TRUE);
  MenuFrame->Enable(ID_MENUITEMTELEMETRY, false);
}

void OpenAVRc_SimulatorFrame::EnableTelemetryMenu()
{
  MenuFrame->Enable(ID_MENUITEMTELEMETRY, true);
}

/////////////////////////////////////////////////////
//               UTILS
/////////////////////////////////////////////////////

void ConvWxstrToCharFw(wxString str,char *fwchar, uint8_t length) //Convert wxString to Firmware chars[]
{
  char buff[length];

  str2zchar(buff, str.mb_str(wxConvUTF8),length);
  memcpy(fwchar, buff, length);
}

wxString ConvCharFwToWxstr(char *cstr, uint8_t length) //Convert Firmware chars[] to wxString
{
  char buff[length];

  zchar2str(buff, cstr, length);
  return wxString(buff,wxConvUTF8);
}

wxString int2wxString(int integer)
{
  wxString intString = wxString::Format(wxT("%i"), integer);
  //wxMessageBox(intString);
  return intString;
}
/////////////////////////////////////////////////////////

void ConnectCom(wxString name)
{
  int error;
  char comMame[20];
  strncpy(comMame, (const char*)name.mb_str(wxConvUTF8), 20);
  assert(comPort);
  //comPort->disconnect();
  switch (Tele_Protocol)
  {
  case Tele_Proto_Frsky_D :
    error = comPort->connect(comMame, 9600, spNONE);
    break;
  case Tele_Proto_Frsky_Sport :
    error = comPort->connect(comMame, 57600, spNONE);
    break;
  default :
    SimuComIsValid = false;
    break;
  }
  if (error == 0) {
      SimuComIsValid = true;
      comwaitcounter = GetTickCount();
  }
  else {
    wxString intString = wxString::Format(wxT("%i"), error);
    wxMessageBox("Erreur N°"+ intString + " port COM");
    }
}

void SendByteCom(uint8_t data)
{
  if ((SimuComIsValid) && ((comwaitcounter + 1000) < GetTickCount()))
  {
    comPort->sendChar(data);
  }
}

void OpenAVRc_SimulatorFrame::OnPanelMainKeyDown(wxKeyEvent& event)
{
  OnKey(event);
}

void OpenAVRc_SimulatorFrame::CheckActiveProtocols()
{
  if (!(g_eeGeneral.protocol_mask & PROTOMASK0)) // DSM2_SERIAL Not present
    {
      CheckBoxDSMSER->SetValue(false);
    }
    else
    {
      CheckBoxDSMSER->SetValue(true);
    }

  if (!(g_eeGeneral.protocol_mask & PROTOMASK1)) // MULTI Not present
    {
      CheckBoxMULTIMOD->SetValue(false);
    }
    else
    {
      CheckBoxMULTIMOD->SetValue(true);
    }

  if (!(g_eeGeneral.protocol_mask & PROTOMASK2)) // CC2500 Not present
    {
      CheckBoxCC2500->SetValue(false);
    }
    else
    {
      CheckBoxCC2500->SetValue(true);
    }

  if (!(g_eeGeneral.protocol_mask & PROTOMASK3)) // CYRF6936 Not present
    {
      CheckBoxCYRF6936->SetValue(false);
    }
    else
    {
      CheckBoxCYRF6936->SetValue(true);
    }

  if (!(g_eeGeneral.protocol_mask & PROTOMASK4)) // A7105 Not present
    {
      CheckBoxA7105->SetValue(false);
    }
    else
    {
      CheckBoxA7105->SetValue(true);
    }

  if (!(g_eeGeneral.protocol_mask & PROTOMASK5)) // NRF24L01 Not present
    {
      CheckBoxNRF24L01->SetValue(false);
    }
    else
    {
      CheckBoxNRF24L01->SetValue(true);
    }

  CheckBoxDSMSER->Enable();
  CheckBoxMULTIMOD->Enable();
  CheckBoxCC2500->Enable();
  CheckBoxCYRF6936->Enable();
  CheckBoxA7105->Enable();
  CheckBoxNRF24L01->Enable();
}

void OpenAVRc_SimulatorFrame::OnCheckBoxProtocolsClick(wxCommandEvent& event)
{
  int answer = wxMessageBox((_("Appliquer à tous les modèles de ") +CurrentEEPath+" ?"), _("Convertir eeprom ?"), wxYES_NO, this);
        if (answer == wxNO) {
          CheckActiveProtocols();
          return; // No modifications
        }
  // Start converting ...

  OpenAVRcClose();
  ChronoMain->Pause();
  Chrono10ms->Pause();
  TimerMain.Stop();
  Timer10ms.Stop();
  ResetSimuLcd();
  wxBusyInfo wait(_("Conversion en cours, attendez SVP......"));

  uint8_t currmodel = g_eeGeneral.currModel;
  uint8_t oldprotomask = g_eeGeneral.protocol_mask;
  uint8_t newprotomask = 0;
  newprotomask += (CheckBoxDSMSER->IsChecked())? PROTOMASK0:0;
  newprotomask += (CheckBoxMULTIMOD->IsChecked())? PROTOMASK1:0;
  newprotomask += (CheckBoxCC2500->IsChecked())? PROTOMASK2:0;
  newprotomask += (CheckBoxCYRF6936->IsChecked())? PROTOMASK3:0;
  newprotomask += (CheckBoxA7105->IsChecked())? PROTOMASK4:0;
  newprotomask += (CheckBoxNRF24L01->IsChecked())? PROTOMASK5:0;

  for (uint8_t m=0; m<MAX_MODELS; ++m)
    {

    if (eeModelExists(m))
      {
        theFile.openRlc(FILE_MODEL(m));
        theFile.readRlc((uint8_t*)&g_model, sizeof(g_model));
        g_eeGeneral.protocol_mask = oldprotomask;
        ADAPT_PROTOCOL_TO_SIMU(); // Convert with old mask
        g_eeGeneral.protocol_mask = newprotomask;
        ADAPT_PROTOCOL_TO_TX(); // Convert with new mask
        theFile.writeRlc(FILE_MODEL(m), FILE_TYP_MODEL, (uint8_t*)&g_model, sizeof(g_model), 1);
      }
    }

  theFile.openRlc(FILE_MODEL(g_eeGeneral.currModel));
  theFile.readRlc((uint8_t*)&g_model, sizeof(g_model)); // Reload current
  ADAPT_PROTOCOL_TO_SIMU();
  Timer10ms.StartOnce(10); //Simulate 10mS Interrupt vector
  TimerMain.StartOnce(18); // Simulate ?mS cycle for mainloop function
  ChronoMain->Resume();
  Chrono10ms->Resume();
}
