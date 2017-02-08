/*
**************************************************************************
*                                                                        *
*              This file is part of the OpenAVRc project.                *
*                                                                        *
*                         Based on code named                            *
*             OpenTx - https://github.com/opentx/opentx                  *
*                                                                        *
*                Only AVR code here for lisibility ;-)                   *
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


//(*InternalHeaders(OpenAVRc_SimulatorFrame)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/intl.h>
#include <wx/image.h>
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

bool Mp3RepExist = false;
extern volatile uint8_t JQ6500_InputIndex;
extern uint8_t JQ6500_PlayIndex;
extern uint8_t JQ6500_playlist[];



//(*IdInit(OpenAVRc_SimulatorFrame)
const long OpenAVRc_SimulatorFrame::ID_PANELH = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_ONTGLBUTTON = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BUTTONSTARTDESKTOP = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_PANELL = wxNewId();
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
const long OpenAVRc_SimulatorFrame::ID_PANELMAIN = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_PANELPRINCIPAL = wxNewId();
const long OpenAVRc_SimulatorFrame::IdMenuOpenEE = wxNewId();
const long OpenAVRc_SimulatorFrame::IdMenuSaveEE = wxNewId();
const long OpenAVRc_SimulatorFrame::idMenuQuit = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_LCDB = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_LCDF = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BUTOFF = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_BUTON = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_STICKB = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_STICKF = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_COLOURS = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_MENUITEMOUTPUTOUTPUT = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_MENUITEMOUTPUTMIXER = wxNewId();
const long OpenAVRc_SimulatorFrame::ID_MENUITEMOUTPUTGVARS = wxNewId();
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
  wxMenuItem* MenuAbout;
  wxMenuItem* MenuItem1;
  wxMenu* Menu1;
  wxMenuBar* MenuBar1;

  Create(parent, wxID_ANY, _("Simulateur"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxSUNKEN_BORDER|wxRAISED_BORDER, _T("wxID_ANY"));
  SetClientSize(wxSize(787,415));
  Move(wxPoint(25,25));
  SetMaxSize(wxSize(-1,-1));
  PanelPrincipal = new wxPanel(this, ID_PANELPRINCIPAL, wxPoint(424,216), wxSize(777,400), wxRAISED_BORDER, _T("ID_PANELPRINCIPAL"));
  PanelH = new wxPanel(PanelPrincipal, ID_PANELH, wxPoint(0,0), wxSize(784,64), wxDOUBLE_BORDER, _T("ID_PANELH"));
  PanelL = new wxPanel(PanelPrincipal, ID_PANELL, wxPoint(0,305), wxSize(784,64), wxDOUBLE_BORDER, _T("ID_PANELL"));
  OnTglButton = new wxToggleButton(PanelL, ID_ONTGLBUTTON, _("ON"), wxPoint(8,8), wxSize(62,22), wxDOUBLE_BORDER, wxDefaultValidator, _T("ID_ONTGLBUTTON"));
  ButtonStartDesktop = new wxButton(PanelL, ID_BUTTONSTARTDESKTOP, _("Desktop"), wxPoint(8,32), wxSize(62,22), wxSIMPLE_BORDER, wxDefaultValidator, _T("ID_BUTTONSTARTDESKTOP"));
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
  SpinRea = new wxSpinButton(PanelMain, ID_SPINREA, wxPoint(14,12), wxDefaultSize, wxSP_HORIZONTAL|wxSP_WRAP|wxSTATIC_BORDER, _T("ID_SPINREA"));
  SpinRea->SetRange(-100, 100);
  SpinRea->SetToolTip(_("Rea"));
  SpinReb = new wxSpinButton(PanelMain, ID_SPINREB, wxPoint(732,12), wxDefaultSize, wxSP_HORIZONTAL|wxSP_WRAP|wxSTATIC_BORDER, _T("ID_SPINREB"));
  SpinReb->SetRange(-100, 100);
  SpinReb->SetToolTip(_("Reb"));
  TextCtrlgetkbinput = new wxTextCtrl(PanelMain, ID_TEXTCTRLDUMMY, wxEmptyString, wxPoint(64,216), wxSize(0,0), 0, wxDefaultValidator, _T("ID_TEXTCTRLDUMMY"));
  MenuBar1 = new wxMenuBar();
  Menu1 = new wxMenu();
  MenuLoadee = new wxMenuItem(Menu1, IdMenuOpenEE, _("Charger Eeprom"), _("Charger fichier BIN"), wxITEM_NORMAL);
  MenuLoadee->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_FORWARD")),wxART_MENU));
  Menu1->Append(MenuLoadee);
  MenuSaveee = new wxMenuItem(Menu1, IdMenuSaveEE, _("Sauver Eeprom"), _("Sauvegarde du fichier BIN"), wxITEM_NORMAL);
  MenuSaveee->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_BACK")),wxART_MENU));
  Menu1->Append(MenuSaveee);
  MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quitter\tAlt-F4"), _("Quitter le simulateur"), wxITEM_NORMAL);
  MenuItem1->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_QUIT")),wxART_MENU));
  Menu1->Append(MenuItem1);
  MenuBar1->Append(Menu1, _("&Fichier"));
  Menu3 = new wxMenu();
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
  Menu3->Append(ID_COLOURS, _("Couleurs"), MenuColours, wxEmptyString);
  MenuBar1->Append(Menu3, _("&Option"));
  Menu2 = new wxMenu();
  OutputBars = new wxMenuItem(Menu2, ID_MENUITEMOUTPUTOUTPUT, _("Graphique Sorties"), wxEmptyString, wxITEM_NORMAL);
  Menu2->Append(OutputBars);
  OutputMixeur = new wxMenuItem(Menu2, ID_MENUITEMOUTPUTMIXER, _("Mixeur"), wxEmptyString, wxITEM_NORMAL);
  Menu2->Append(OutputMixeur);
  OutputGvars = new wxMenuItem(Menu2, ID_MENUITEMOUTPUTGVARS, _("Variables globales"), wxEmptyString, wxITEM_NORMAL);
  Menu2->Append(OutputGvars);
  MenuBar1->Append(Menu2, _("Fenêtres"));
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

  Connect(ID_ONTGLBUTTON,wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnOnTglButtonToggle);
  Connect(ID_BUTTONSTARTDESKTOP,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnButtonStartDesktopClick);
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
  Rstick->Connect(wxEVT_MOTION,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnRstickMouseMove,0,this);
  Simulcd->Connect(wxEVT_PAINT,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnwxsimulcdPaint,0,this);
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
  Lstick->Connect(wxEVT_MOTION,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnLstickMouseMove,0,this);
  Connect(IdMenuOpenEE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuLoadEeprom);
  Connect(IdMenuSaveEE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuSaveeeSelected);
  Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnQuit);
  Connect(ID_LCDB,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuLcdBackSelected);
  Connect(ID_LCDF,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuLcdPixelSelected);
  Connect(ID_BUTOFF,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuButOffSelected);
  Connect(ID_BUTON,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuButOnSelected);
  Connect(ID_STICKB,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuStickBackSelected);
  Connect(ID_STICKF,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMenuStickStickSelected);
  Connect(ID_MENUITEMOUTPUTOUTPUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnOutputBarsSelected);
  Connect(ID_MENUITEMOUTPUTMIXER,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnMixeurSelected);
  Connect(ID_MENUITEMOUTPUTGVARS,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnOutputGvarsSelected);
  Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnAbout);
  Connect(ID_TIMER10MS,wxEVT_TIMER,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnTimer10msTrigger);
  Connect(ID_TIMERMAIN,wxEVT_TIMER,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnTimerMainTrigger);
  Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&OpenAVRc_SimulatorFrame::OnClose);
  //*)
  {
    wxIcon FrameIcon;
    SetIcon(wxICON(nsrcs_icon));
  }

  //App Path
  wxFileName appPathWithExeName = wxStandardPaths::Get().GetExecutablePath();
  AppPath = (appPathWithExeName.GetPath());
  //Ini File
  Ini_Filename = wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator() + "OpenAVRcSimulator.ini";
  configFile = new wxFileConfig( "", "", Ini_Filename);
  Col_Lcd_Back = *wxGREEN;
  Col_Lcd_Front = *wxBLACK;
  Col_Button_Off = *wxBLACK;
  Col_Button_On = *wxWHITE;
  Col_Stick_Back = *wxBLACK;
  Col_Stick_Circle = *wxWHITE;
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
}

//// FW Functions ///////////////////////////////////////////////////

void OpenAVRc_SimulatorFrame::OnOnTglButtonToggle(wxCommandEvent& event)
{
  int answer;
  if (OnTglButton->GetValue()) {
    if (simu_eeprom[1] == 0) {
      if (CurrentEEPath != "") {
        answer = wxMessageBox(_("Recharger le fichier eeprom "+CurrentEEPath+" ?"), _("EEPROM VIDE"), wxYES_NO, this);
        if (answer == wxYES) {
          LoadEepromFile(CurrentEEPath);
        }
      }
      if ((CurrentEEPath == "") || (answer == wxNO)) {
        int answer = wxMessageBox(_("Charger un fichier eeprom ?"), _("EEPROM VIDE"), wxYES_NO, this);
        if (answer == wxYES) LoadEeprom();
      }
    }
    StartFirmwareCode();
//tests
    //wxMessageBox(ConvCharFwToWxstr(g_model.flightModeData[mixerCurrentFlightMode].name, LEN_FLIGHT_MODE_NAME),_("Phase To Miguel   "));
    //wxString toto = "PHtest";
    //ConvWxstrToCharFw(toto,g_model.flightModeData[mixerCurrentFlightMode].name, LEN_FLIGHT_MODE_NAME);
    //wxMessageBox(ConvCharFwToWxstr(g_model.header.name, LEN_MODEL_NAME),_("Model To Miguel   "));
    //toto = "MENTERO";
    //ConvWxstrToCharFw(toto,g_model.header.name, LEN_MODEL_NAME);

  }

}

void OpenAVRc_SimulatorFrame::StartFirmwareCode()
{
  simu_off = false;
  simu_mainloop_is_runing = false;
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
  SpinH->SetPin(6); //Rf activated pin
  SpinH->ResetPin(5); //Dsc inactivated
  SpinC->ResetPin(0); // Set 3pos at ID0
  s_anaFilt[0] = 1024;
  s_anaFilt[1] = 1024;
  s_anaFilt[2] = 1024;
  s_anaFilt[3] = 1024;

  Timer10ms.Start(10, false); //Simulate 10mS Interrupt vector
  simumain();
  TimerMain.Start(1, false); // Simulate ?mS cycle for mainloop function
}

void OpenAVRc_SimulatorFrame::ResetSimu()
{
  for (int i=0; i<(sizeof(displayBuf)); ++i) displayBuf[i] = 0xFF;
  DrawWxSimuLcd();
}



void OpenAVRc_SimulatorFrame::OnTimerMainTrigger(wxTimerEvent& event) //1mS
{
  if (!OnTglButton->GetValue()) {
    SpinH->ResetPin(6);
  }
  if ((!simu_mainloop_is_runing) && (!simu_shutDownSimu_is_runing)) {
    s_anaFilt[7] = 1500;

    ChronoMain->Start(0);
    SimuMainLoop();
    Chronoval = ChronoMain->TimeInMicro();
    ChronoMain->Pause();
    StatusBar->SetStatusText(_T("MAIN ")+Chronoval.ToString()+_T(" uS"),1);
  }
  if ((simu_off) && (!simu_mainloop_is_runing)) {
    TimerMain.Stop();
    Timer10ms.Stop();
    ResetSimu();
    Close();
  }
}

void OpenAVRc_SimulatorFrame::OnTimer10msTrigger(wxTimerEvent& event)
{

  if (Mp3RepExist) PlayTts(); // Check and play voice if needed
  CheckInputs();
  Chrono10ms->Start(0);
  if (!simu_off) {
    TIMER_10MS_VECT();
  } else {
  }
  Chronoval = Chrono10ms->TimeInMicro();
  Chrono10ms->Pause();
  StatusBar->SetStatusText(_T("10 mS IRQ ")+Chronoval.ToString()+_T(" uS"),2);
}

const void OpenAVRc_SimulatorFrame::DrawWxSimuLcd()
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

void OpenAVRc_SimulatorFrame::OnQuit(wxCommandEvent& event)
{
  Close();
}

void OpenAVRc_SimulatorFrame::OnAbout(wxCommandEvent& event)
{
  wxAboutDialogInfo Aboutbox;
  Aboutbox.SetName(_("OpenAVRc Simulateur"));
  Aboutbox.SetVersion(_("0.68 Beta"));
  Aboutbox.SetLicence(" GPLv2 . Firmware basé sur NextStepRc 2.18 ");
  Aboutbox.SetDescription(_("Simulateur du code OpenAVRc 'toutes options' sur carte Méga 2560     "));
  Aboutbox.SetCopyright(wxT("(C) 2016 OpenAVRc Team"));
  Aboutbox.SetWebSite(wxT("https://github.com/Ingwie/OpenAVRc_Dev"));

  wxAboutBox(Aboutbox);
}

void OpenAVRc_SimulatorFrame::OnwxsimulcdPaint(wxPaintEvent& event)
{
  DrawWxSimuLcd();
}

void OpenAVRc_SimulatorFrame::PlayTts()
{
  static long pid = NULL;

  if (Mp3process->Exists(pid)) return;

  if (JQ6500_InputIndex != JQ6500_PlayIndex) {
    uint16_t prompt;
    prompt = JQ6500_playlist[JQ6500_PlayIndex];
    ++JQ6500_PlayIndex;
    prompt <<= 8;
    prompt |= JQ6500_playlist[JQ6500_PlayIndex];
    ++JQ6500_PlayIndex;
    if (JQ6500_PlayIndex == (16*2)) JQ6500_PlayIndex = 0; //QUEUE_LENGTH = 16*2 in JQ6500 driver
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
    wxLogError(_T("Impossible : Simulateur en fonctionnement"));
    return;
  }
  wxFileDialog openFileDialog(this, _("Ouvrir Fichier BIN"), AppPath+ "\\eeprom\\", "","Fichiers BIN (*.bin)|*.bin", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() == wxID_CANCEL) return;
  wxFileInputStream input_stream(openFileDialog.GetPath());
  if (!input_stream.IsOk()) {
    wxLogError(_T("Impossible d'ouvrir le fichier '%s'."), openFileDialog.GetPath());
    return;
  }
  LoadEepromFile(openFileDialog.GetPath());
  CurrentEEPath = openFileDialog.GetPath();
  Ini_Changed = true;
}

void OpenAVRc_SimulatorFrame::LoadEepromFile(wxString path)
{
  wxFile bin_file(path);
  if(bin_file.IsOpened()) {
    for (int i=0; i<(sizeof(simu_eeprom)); ++i) simu_eeprom[i] = 0;
    bin_file.Read(&simu_eeprom[0], EESIZE);
    bin_file.Close();
  }
}

void OpenAVRc_SimulatorFrame::OnMenuLoadEeprom(wxCommandEvent& event)
{
  LoadEeprom();
}

void OpenAVRc_SimulatorFrame::PaintSticks(int x, int y, int x_mem, int y_mem, wxPanel* stick)
{
  wxBrush brush_Stick(Col_Stick_Circle, wxBRUSHSTYLE_SOLID );
  wxBrush brush_Back(Col_Stick_Back, wxBRUSHSTYLE_SOLID );
  wxPen pen_Back(Col_Stick_Back,1,wxPENSTYLE_SOLID);
  wxPen pen_Stick(Col_Stick_Circle,1,wxPENSTYLE_SOLID);
  wxClientDC dc(stick);
  dc.SetBrush(brush_Back);
  dc.SetPen(pen_Back);
  dc.DrawCircle(x_mem,y_mem,8);
  dc.SetPen(pen_Stick);
  dc.SetBrush(brush_Stick);
  dc.DrawCircle(x,y,8);
}

void OpenAVRc_SimulatorFrame::OnLstickMouseMove(wxMouseEvent& event)
{
  static int x_mem = (Lstick->GetSize().GetWidth())/2;
  static int y_mem = (Lstick->GetSize().GetWidth())/2;

  int xmul = 2048000 / (Lstick->GetSize().GetWidth() - 5);
  int ymul = 2048000 / (Lstick->GetSize().GetHeight() -5);
  wxPoint pt(event.GetPosition());
  int x = (pt.x * xmul)/1000;
  int y = 2048 - (pt.y * ymul)/1000;

  if (event.LeftUp()) ; //TODO
  if (event.LeftIsDown()) {
    s_anaFilt[3] = (uint16_t)x;
    s_anaFilt[1] = (uint16_t)y;
    PaintSticks( pt.x, pt.y, x_mem, y_mem, Lstick);
    x_mem = pt.x;
    y_mem = pt.y;
  };
}

void OpenAVRc_SimulatorFrame::OnRstickMouseMove(wxMouseEvent& event)
{
  static int x_mem = (Rstick->GetSize().GetWidth())/2;
  static int y_mem = (Rstick->GetSize().GetWidth())/2;

  int xmul = 2048000 / (Rstick->GetSize().GetWidth() - 5);
  int ymul = 2048000 / (Rstick->GetSize().GetHeight() -5);
  wxPoint pt(event.GetPosition());
  int x = (pt.x * xmul)/1000;
  int y = 2048 - (pt.y * ymul)/1000;

  if (event.LeftUp()) ; //TODO
  if (event.LeftIsDown()) {
    s_anaFilt[0] = (uint16_t)x;
    s_anaFilt[2] = (uint16_t)y;

    PaintSticks( pt.x, pt.y, x_mem, y_mem, Rstick);
    x_mem = pt.x;
    y_mem = pt.y;
  };
}

void OpenAVRc_SimulatorFrame::OnSimulcdLeftDClick(wxMouseEvent& event)
{
  wxFileDialog saveFileDialog(this, _("Sauver Capture écran"), "", "", "Fichier BMP (*.bmp)|*.bmp", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
  if (saveFileDialog.ShowModal() == wxID_CANCEL)
    return;     // the user changed idea...
  wxFileOutputStream output_stream(saveFileDialog.GetPath());
  if (!output_stream.IsOk()) {
    wxLogError("Ne peut écrire le fichier '%s'.", saveFileDialog.GetPath());
    return;
  }
  SimuLcd_Bitmap.SaveFile(saveFileDialog.GetPath(), wxBITMAP_TYPE_BMP, NULL);

}


void OpenAVRc_SimulatorFrame::OnMenuSaveeeSelected(wxCommandEvent& event)
{
  wxFileDialog saveFileDialog(this, _("Sauver Eeprom"), "", "", "Fichier BIN (*.bin)|*.bin", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
  if (saveFileDialog.ShowModal() == wxID_CANCEL)
    return;     // the user changed idea...
  wxFile bin_file;
  bin_file.Create(saveFileDialog.GetPath(), true);
  eeFlush(); //Save Radio eeprom immediatly
  if(bin_file.IsOpened()) {
    bin_file.Seek(0);
    bin_file.Write(&simu_eeprom[0], EESIZE);
    bin_file.Close();
  }
}

void OpenAVRc_SimulatorFrame::OnClose(wxCloseEvent& event)
{
  if (OnTglButton->GetValue() && (!simu_off)) {
    wxMessageBox( _("Merci d'éteindre le simulateur pour quitter"), _("      OpenAVRc Simulateur"));
    event.Veto();
    return;
  }
  if (Ini_Changed) SaveConfig();
  if (ChronoMain != NULL) {
    ChronoMain->Pause();
    Sleep(1);
    delete ChronoMain;
  }
  if (Chrono10ms != NULL) {
    Chrono10ms->Pause();
    Sleep(1);
    delete Chrono10ms;
  }
  Sleep(100);

  PanelMain->PopEventHandler(NULL);
  PanelH->PopEventHandler(NULL);
  PanelL->PopEventHandler(NULL);
  //delete PanelMainBackground;
  //delete PanelHBackckground;
  //delete PanelBBackckground;
  //delete PanelMain;
  //delete PanelH;
  //delete PanelB;

  /*delete SpinA;
  delete SpinB;
  delete SpinC;
  delete SpinD;
  delete SpinE;
  delete SpinF;
  delete SpinG;
  delete SpinH;
  delete SpinJ;
  delete SpinK;
  delete SpinL;*/

  if (SimuLcd_MemoryDC != NULL) delete SimuLcd_MemoryDC;
  if (SimuLcd_ClientDC != NULL)	delete SimuLcd_ClientDC;

  Destroy();
}

void OpenAVRc_SimulatorFrame::LoadConfig()
{
  configFile->Read(wxT("Col_Lcd_Back"),&Col_Lcd_Back);
  configFile->Read(wxT("Col_Lcd_Front"),&Col_Lcd_Front);
  configFile->Read(wxT("Col_Button_Off"),&Col_Button_Off);
  configFile->Read(wxT("Col_Button_On"),&Col_Button_On);
  configFile->Read(wxT("Col_Stick_Back"),&Col_Stick_Back);
  configFile->Read(wxT("Col_Stick_Circle"),&Col_Stick_Circle);

  configFile->Read(wxT("EEfile"),&CurrentEEPath);
}

void OpenAVRc_SimulatorFrame::SaveConfig()
{
  wxMessageBox( Ini_Filename, _("Les paramètres sont sauvé dans :"));

  configFile->Write(wxT("Col_Lcd_Back"),Col_Lcd_Back);
  configFile->Write(wxT("Col_Lcd_Front"),Col_Lcd_Front);
  configFile->Write(wxT("Col_Button_Off"),Col_Button_Off);
  configFile->Write(wxT("Col_Button_On"),Col_Button_On);
  configFile->Write(wxT("Col_Stick_Back"),Col_Stick_Back);
  configFile->Write(wxT("Col_Stick_Circle"),Col_Stick_Circle);

  configFile->Write(wxT("EEfile"),CurrentEEPath);


  configFile->Flush();
}

//////////////////////////////VIRTUAL PIN WORD !! JOB /////////////////////////

void OpenAVRc_SimulatorFrame::OnBPmenuLeftDown(wxMouseEvent& event)
{
  SpinL->ResetPin(4);
}

void OpenAVRc_SimulatorFrame::OnBPmenuLeftUp(wxMouseEvent& event)
{
  SpinL->SetPin(4);
}

void OpenAVRc_SimulatorFrame::OnBPexitLeftDown(wxMouseEvent& event)
{
  SpinL->ResetPin(5);
}

void OpenAVRc_SimulatorFrame::OnBPexitLeftUp(wxMouseEvent& event)
{
  SpinL->SetPin(5);
}

void OpenAVRc_SimulatorFrame::OnBPhLeftDown(wxMouseEvent& event)
{
  SpinL->ResetPin(1);
}

void OpenAVRc_SimulatorFrame::OnBPhLeftUp(wxMouseEvent& event)
{
  SpinL->SetPin(1);
}

void OpenAVRc_SimulatorFrame::OnBPbLeftDown(wxMouseEvent& event)
{
  SpinL->ResetPin(0);
}

void OpenAVRc_SimulatorFrame::OnBPbLeftUp(wxMouseEvent& event)
{
  SpinL->SetPin(0);
}

void OpenAVRc_SimulatorFrame::OnBPgLeftDown(wxMouseEvent& event)
{
  SpinL->ResetPin(3);
}

void OpenAVRc_SimulatorFrame::OnBPgLeftUp(wxMouseEvent& event)
{
  SpinL->SetPin(3);
}

void OpenAVRc_SimulatorFrame::OnBPdLeftDown(wxMouseEvent& event)
{
  SpinL->ResetPin(2);
}

void OpenAVRc_SimulatorFrame::OnBPdLeftUp(wxMouseEvent& event)
{
  SpinL->SetPin(2);
}

void OpenAVRc_SimulatorFrame::OnLlTrimLeftDown(wxMouseEvent& event)
{
  SpinF->ResetPin(7);
}

void OpenAVRc_SimulatorFrame::OnLlTrimLeftUp(wxMouseEvent& event)
{
  SpinF->SetPin(7);
}

void OpenAVRc_SimulatorFrame::OnLrTrimLeftDown(wxMouseEvent& event)
{
  SpinF->ResetPin(6);
}

void OpenAVRc_SimulatorFrame::OnLrTrimLeftUp(wxMouseEvent& event)
{
  SpinF->SetPin(6);
}

void OpenAVRc_SimulatorFrame::OnLdTrimLeftDown(wxMouseEvent& event)
{
  SpinF->ResetPin(5);
}

void OpenAVRc_SimulatorFrame::OnLdTrimLeftUp(wxMouseEvent& event)
{
  SpinF->SetPin(5);
}

void OpenAVRc_SimulatorFrame::OnLuTrimLeftDown(wxMouseEvent& event)
{
  SpinF->ResetPin(4);
}

void OpenAVRc_SimulatorFrame::OnLuTrimLeftUp(wxMouseEvent& event)
{
  SpinF->SetPin(4);
}

void OpenAVRc_SimulatorFrame::OnRdTrimLeftDown(wxMouseEvent& event)
{
  SpinF->ResetPin(3);
}

void OpenAVRc_SimulatorFrame::OnRdTrimLeftUp(wxMouseEvent& event)
{
  SpinF->SetPin(3);
}

void OpenAVRc_SimulatorFrame::OnRuTrimLeftDown(wxMouseEvent& event)
{
  SpinF->ResetPin(2);
}

void OpenAVRc_SimulatorFrame::OnRuTrimLeftUp(wxMouseEvent& event)
{
  SpinF->SetPin(2);
}

void OpenAVRc_SimulatorFrame::OnRlTrimLeftDown(wxMouseEvent& event)
{
  SpinF->ResetPin(1);
}

void OpenAVRc_SimulatorFrame::OnRlTrimLeftUp(wxMouseEvent& event)
{
  SpinF->SetPin(1);
}

void OpenAVRc_SimulatorFrame::OnRrTrimLeftDown(wxMouseEvent& event)
{
  SpinF->ResetPin(0);
}

void OpenAVRc_SimulatorFrame::OnRrTrimLeftUp(wxMouseEvent& event)
{
  SpinF->SetPin(0);
}

void OpenAVRc_SimulatorFrame::OnBpTrnLeftDown(wxMouseEvent& event)
{
  SpinL->ResetPin(7);
}

void OpenAVRc_SimulatorFrame::OnBpTrnLeftUp(wxMouseEvent& event)
{
  SpinL->SetPin(7);
}

void OpenAVRc_SimulatorFrame::OnBpTrnRightDown(wxMouseEvent& event)
{
  SpinL->ResetPin(7);
  CheckInputs();
}

void OpenAVRc_SimulatorFrame::OnBpThrLeftDown(wxMouseEvent& event)
{
  SpinG->TogglePin(2);
}

void OpenAVRc_SimulatorFrame::OnBpRudLeftDown(wxMouseEvent& event)
{
  SpinG->TogglePin(0);
}

void OpenAVRc_SimulatorFrame::OnBpEleLeftDown(wxMouseEvent& event)
{
  SpinL->TogglePin(6);
}

void OpenAVRc_SimulatorFrame::OnBpAilLeftDown(wxMouseEvent& event)
{
  SpinD->TogglePin(7);
}

void OpenAVRc_SimulatorFrame::OnBpGeaLeftDown(wxMouseEvent& event)
{
  SpinG->TogglePin(1);
}

void OpenAVRc_SimulatorFrame::OnBpId1LeftDown(wxMouseEvent& event)
{
  if (!SpinC->GetPin(1)) SpinC->TogglePin(1);
  else SpinC->TogglePin(0);
}

void OpenAVRc_SimulatorFrame::OnBpId2LeftDown(wxMouseEvent& event)
{
  if (!SpinC->GetPin(0)) SpinC->TogglePin(0);
  else SpinC->TogglePin(1);
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
    SpinD->ResetPin(2);
    SpinD->ResetPin(3);
    INT3_vect();
    SpinD->SetPin(2);
    SpinD->SetPin(3);
  }

  if (SpinReb->GetValue() < 0) {
    SpinReb->SetValue(0);
    SpinD->ResetPin(2);
    SpinD->ResetPin(3);
    INT2_vect();
    SpinD->SetPin(2);
    SpinD->SetPin(3);
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
}

///// UTILS ///////////

wxColour OpenAVRc_SimulatorFrame::SetColour()
{
  wxColourDialog ColourDlg(this);
  ColourDlg.GetColourData().SetChooseFull(true);
  if (ColourDlg.ShowModal() == wxID_OK) {
    Ini_Changed = true;
    return ColourDlg.GetColourData().GetColour().GetAsString();
  }
}

void OpenAVRc_SimulatorFrame::OnMenuLcdBackSelected(wxCommandEvent& event)
{
  Col_Lcd_Back = SetColour();
  DrawWxSimuLcd();
}

void OpenAVRc_SimulatorFrame::OnMenuLcdPixelSelected(wxCommandEvent& event)
{
  Col_Lcd_Front = SetColour();
  DrawWxSimuLcd();
}

void OpenAVRc_SimulatorFrame::OnMenuButOffSelected(wxCommandEvent& event)
{
  Col_Button_Off = SetColour();
}

void OpenAVRc_SimulatorFrame::OnMenuButOnSelected(wxCommandEvent& event)
{
  Col_Button_On = SetColour();
}

void OpenAVRc_SimulatorFrame::OnMenuStickBackSelected(wxCommandEvent& event)
{
  Col_Stick_Back = SetColour();
  Lstick->SetBackgroundColour(Col_Stick_Back);
  Rstick->SetBackgroundColour(Col_Stick_Back);
  Lstick->Refresh();
  Rstick->Refresh();
}

void OpenAVRc_SimulatorFrame::OnMenuStickStickSelected(wxCommandEvent& event)
{
  Col_Stick_Circle = SetColour();
  Lstick->SetForegroundColour(Col_Stick_Circle);
  Rstick->SetForegroundColour(Col_Stick_Circle);
}


void OpenAVRc_SimulatorFrame::OnLstickPaint(wxPaintEvent& event)
{
  int pos = (Lstick->GetSize().GetWidth())/2;
  PaintSticks(pos,pos,pos,pos,Lstick);
}

void OpenAVRc_SimulatorFrame::OnRstickPaint(wxPaintEvent& event)
{
  int pos = (Rstick->GetSize().GetWidth())/2;
  PaintSticks(pos,pos,pos,pos,Rstick);

}


void OpenAVRc_SimulatorFrame::OnButtonStartDesktopClick(wxCommandEvent& event)
{
  wxString desktop("OpenAVRc_Desktop.exe");
  wxExecute(desktop);
}

void OpenAVRc_SimulatorFrame::OnKey(wxKeyEvent& event)
{
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
  event.Skip();
}

/////////////////////////////////////////////////////
//               UTILS
/////////////////////////////////////////////////////

void ConvWxstrToCharFw(wxString str,char *fwchar, uint8_t length) //Convert wxString to Firmware chars[]
{
  char buff[length+1];

  str2zchar(buff, str.mb_str(wxConvUTF8),length);
  strncpy(fwchar, buff, length);
}

wxString ConvCharFwToWxstr(char *cstr, uint8_t length) //Convert Firmware chars[] to wxString
{
  char buff[length+1];
  zchar2str(buff, cstr, length);
  return wxString(buff,wxConvUTF8 );
}

wxString int2wxString(int integer)
{
  wxString intString = wxString::Format(wxT("%i"), integer);
  wxMessageBox(intString);
}

void OpenAVRc_SimulatorFrame::OnMixeurSelected(wxCommandEvent& event)//Draws the mixer display screen
{
  MixerFrame *MixFr = new  MixerFrame(this);
  MixFr->Show(TRUE);
}

void OpenAVRc_SimulatorFrame::OnOutputBarsSelected(wxCommandEvent& event)
{
  OutBarsFrame *BarFr = new  OutBarsFrame(this);
  BarFr->Show(TRUE);
}

void OpenAVRc_SimulatorFrame::OnOutputGvarsSelected(wxCommandEvent& event)
{
  GvarsFrame *GvFr = new  GvarsFrame(this);
  GvFr->Show(TRUE);
}
