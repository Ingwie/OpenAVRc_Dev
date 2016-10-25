/***************************************************************
 * Name:      NextStepRc_SimulatorMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    NextStepRc_TEAM ()
 * Created:   2016-09-12
 * Copyright: NextStepRc_TEAM ()
 * License:
 **************************************************************/

#include "NextStepRc_SimulatorMain.h"
#include <wx/msgdlg.h>
#include <wx/dcclient.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>
#include <wx/log.h>
#include <wx/file.h>



//(*InternalHeaders(NextStepRc_SimulatorFrame)
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

#include "avatarnext.xpm"

//helper functions
enum wxbuildinfoformat
{
    short_f, long_f
};

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
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


//(*IdInit(NextStepRc_SimulatorFrame)
const long NextStepRc_SimulatorFrame::ID_PANEL2 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_ONTGLBUTTON = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL4 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPMENU = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPH = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPEXIT = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL9 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL10 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL11 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL12 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL7 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL8 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL6 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL5 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPG = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPB = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPD = wxNewId();
const long NextStepRc_SimulatorFrame::ID_SPINBUTTON1 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_LSTICK = wxNewId();
const long NextStepRc_SimulatorFrame::ID_RSTICK = wxNewId();
const long NextStepRc_SimulatorFrame::ID_SIMULCD = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL14 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL15 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL16 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL17 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL18 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL19 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL20 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL13 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL3 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL1 = wxNewId();
const long NextStepRc_SimulatorFrame::IdMenuOpenEE = wxNewId();
const long NextStepRc_SimulatorFrame::IdMenuSaveEE = wxNewId();
const long NextStepRc_SimulatorFrame::idMenuQuit = wxNewId();
const long NextStepRc_SimulatorFrame::idMenuAbout = wxNewId();
const long NextStepRc_SimulatorFrame::ID_STATUSBAR = wxNewId();
const long NextStepRc_SimulatorFrame::ID_TIMER10MS = wxNewId();
const long NextStepRc_SimulatorFrame::ID_TIMERMAIN = wxNewId();
//*)

BEGIN_EVENT_TABLE(NextStepRc_SimulatorFrame,wxFrame)
    //(*EventTable(NextStepRc_SimulatorFrame)
    //*)
END_EVENT_TABLE()

NextStepRc_SimulatorFrame::NextStepRc_SimulatorFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(NextStepRc_SimulatorFrame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxMenuBar* MenuBar1;
    wxMenu* Menu2;

    Create(parent, wxID_ANY, _("Simulateur"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxSUNKEN_BORDER|wxRAISED_BORDER, _T("wxID_ANY"));
    SetClientSize(wxSize(800,420));
    Move(wxPoint(25,25));
    SetMaxSize(wxSize(-1,-1));
    {
        wxIcon FrameIcon;
        FrameIcon.CopyFromBitmap(avatarnext);
        SetIcon(FrameIcon);
    }
    PanelPrincipal = new wxPanel(this, ID_PANEL1, wxPoint(424,216), wxSize(800,400), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    Panel1 = new wxPanel(PanelPrincipal, ID_PANEL2, wxPoint(8,8), wxSize(784,64), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    Panel1->SetBackgroundColour(wxColour(151,234,194));
    Panel3 = new wxPanel(PanelPrincipal, ID_PANEL4, wxPoint(8,312), wxSize(784,64), wxRAISED_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    Panel3->SetBackgroundColour(wxColour(50,167,237));
    OnTglButton = new wxToggleButton(Panel3, ID_ONTGLBUTTON, _("ON"), wxPoint(16,8), wxSize(45,21), wxDOUBLE_BORDER, wxDefaultValidator, _T("ID_ONTGLBUTTON"));
    Panel2 = new wxPanel(PanelPrincipal, ID_PANEL3, wxPoint(8,72), wxSize(784,248), wxDOUBLE_BORDER, _T("ID_PANEL3"));
    Panel2->SetBackgroundColour(wxColour(131,216,243));
    BPmenu = new wxPanel(Panel2, ID_BPMENU, wxPoint(440,170), wxSize(50,20), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_BPMENU"));
    BPmenu->SetForegroundColour(wxColour(255,255,128));
    BPmenu->SetBackgroundColour(wxColour(0,0,0));
    BPh = new wxPanel(Panel2, ID_BPH, wxPoint(288,160), wxSize(25,25), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_BPH"));
    BPh->SetForegroundColour(wxColour(255,255,128));
    BPh->SetBackgroundColour(wxColour(0,0,0));
    BPexit = new wxPanel(Panel2, ID_BPEXIT, wxPoint(440,200), wxSize(50,20), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_BPEXIT"));
    BPexit->SetForegroundColour(wxColour(255,255,128));
    BPexit->SetBackgroundColour(wxColour(0,0,0));
    LlTrim = new wxPanel(Panel2, ID_PANEL9, wxPoint(102,200), wxSize(25,12), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL9"));
    LlTrim->SetForegroundColour(wxColour(255,255,128));
    LlTrim->SetBackgroundColour(wxColour(0,0,0));
    LuTrim = new wxPanel(Panel2, ID_PANEL10, wxPoint(224,72), wxSize(12,25), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL10"));
    LuTrim->SetForegroundColour(wxColour(255,255,128));
    LuTrim->SetBackgroundColour(wxColour(0,0,0));
    LdTrim = new wxPanel(Panel2, ID_PANEL11, wxPoint(224,128), wxSize(12,25), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL11"));
    LdTrim->SetForegroundColour(wxColour(255,255,128));
    LdTrim->SetBackgroundColour(wxColour(0,0,0));
    RdTrim = new wxPanel(Panel2, ID_PANEL12, wxPoint(536,128), wxSize(12,25), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL12"));
    RdTrim->SetForegroundColour(wxColour(255,255,128));
    RdTrim->SetBackgroundColour(wxColour(0,0,0));
    RuTrim = new wxPanel(Panel2, ID_PANEL7, wxPoint(536,72), wxSize(12,25), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL7"));
    RuTrim->SetForegroundColour(wxColour(255,255,128));
    RuTrim->SetBackgroundColour(wxColour(0,0,0));
    RlTrim = new wxPanel(Panel2, ID_PANEL8, wxPoint(592,200), wxSize(25,12), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL8"));
    RlTrim->SetForegroundColour(wxColour(255,255,128));
    RlTrim->SetBackgroundColour(wxColour(0,0,0));
    RrTrim = new wxPanel(Panel2, ID_PANEL6, wxPoint(644,200), wxSize(25,12), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL6"));
    RrTrim->SetForegroundColour(wxColour(255,255,128));
    RrTrim->SetBackgroundColour(wxColour(0,0,0));
    LrTrim = new wxPanel(Panel2, ID_PANEL5, wxPoint(152,200), wxSize(25,12), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL5"));
    LrTrim->SetForegroundColour(wxColour(255,255,128));
    LrTrim->SetBackgroundColour(wxColour(0,0,0));
    BPg = new wxPanel(Panel2, ID_BPG, wxPoint(264,184), wxSize(25,25), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_BPG"));
    BPg->SetForegroundColour(wxColour(255,255,128));
    BPg->SetBackgroundColour(wxColour(0,0,0));
    BPb = new wxPanel(Panel2, ID_BPB, wxPoint(288,208), wxSize(25,25), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_BPB"));
    BPb->SetForegroundColour(wxColour(255,255,128));
    BPb->SetBackgroundColour(wxColour(0,0,0));
    BPd = new wxPanel(Panel2, ID_BPD, wxPoint(312,184), wxSize(25,25), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_BPD"));
    BPd->SetForegroundColour(wxColour(255,255,128));
    BPd->SetBackgroundColour(wxColour(0,0,0));
    Lstick = new wxPanel(Panel2, ID_LSTICK, wxPoint(60,32), wxSize(158,158), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_LSTICK"));
    Lstick->SetForegroundColour(wxColour(255,0,0));
    Lstick->SetBackgroundColour(wxColour(0,0,0));
    SpinButton1 = new wxSpinButton(Lstick, ID_SPINBUTTON1, wxPoint(160,40), wxSize(32,66), wxSP_VERTICAL|wxSP_ARROW_KEYS, _T("ID_SPINBUTTON1"));
    SpinButton1->SetRange(0, 100);
    Rstick = new wxPanel(Panel2, ID_RSTICK, wxPoint(552,32), wxSize(158,158), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_RSTICK"));
    Rstick->SetForegroundColour(wxColour(255,0,0));
    Rstick->SetBackgroundColour(wxColour(0,0,0));
    Simulcd = new wxPanel(Panel2, ID_SIMULCD, wxPoint(256,20), wxSize(260,132), wxNO_BORDER, _T("ID_SIMULCD"));
    Simulcd->SetBackgroundColour(wxColour(120,210,30));
    PbThr = new wxPanel(Panel2, ID_PANEL14, wxPoint(25,48), wxSize(25,25), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL14"));
    PbThr->SetForegroundColour(wxColour(255,255,128));
    PbThr->SetBackgroundColour(wxColour(0,0,0));
    BpRud = new wxPanel(Panel2, ID_PANEL15, wxPoint(25,88), wxSize(25,25), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL15"));
    BpRud->SetForegroundColour(wxColour(255,255,128));
    BpRud->SetBackgroundColour(wxColour(0,0,0));
    BpEle = new wxPanel(Panel2, ID_PANEL16, wxPoint(25,128), wxSize(25,25), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL16"));
    BpEle->SetForegroundColour(wxColour(255,255,128));
    BpEle->SetBackgroundColour(wxColour(0,0,0));
    BpTrn = new wxPanel(Panel2, ID_PANEL17, wxPoint(16,208), wxSize(25,25), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL17"));
    BpTrn->SetForegroundColour(wxColour(255,255,128));
    BpTrn->SetBackgroundColour(wxColour(0,0,0));
    BpAil = new wxPanel(Panel2, ID_PANEL18, wxPoint(720,48), wxSize(25,25), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL18"));
    BpAil->SetForegroundColour(wxColour(255,255,128));
    BpAil->SetBackgroundColour(wxColour(0,0,0));
    BpGea = new wxPanel(Panel2, ID_PANEL19, wxPoint(720,88), wxSize(25,25), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL19"));
    BpGea->SetForegroundColour(wxColour(255,255,128));
    BpGea->SetBackgroundColour(wxColour(0,0,0));
    BpId1 = new wxPanel(Panel2, ID_PANEL20, wxPoint(720,128), wxSize(25,20), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL20"));
    BpId1->SetForegroundColour(wxColour(255,255,128));
    BpId1->SetBackgroundColour(wxColour(0,0,0));
    BpId2 = new wxPanel(Panel2, ID_PANEL13, wxPoint(720,148), wxSize(25,20), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL13"));
    BpId2->SetForegroundColour(wxColour(255,255,128));
    BpId2->SetBackgroundColour(wxColour(0,0,0));
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu1, IdMenuOpenEE, _("Charger Eeprom"), _("Charger fichier BIN"), wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem4 = new wxMenuItem(Menu1, IdMenuSaveEE, _("Sauver Eeprom"), _("Sauvegarde du fichier BIN"), wxITEM_NORMAL);
    Menu1->Append(MenuItem4);
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quitter\tAlt-F4"), _("Quitter le simulateur"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&Fichier"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("A propos ...\tF1"), _("C\'est quoi donc \?"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("&Aide"));
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

    Connect(ID_ONTGLBUTTON,wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnOnTglButtonToggle);
    BPmenu->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPmenuLeftDown,0,this);
    BPmenu->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPmenuLeftUp,0,this);
    BPh->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPhLeftDown,0,this);
    BPh->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPhLeftUp,0,this);
    BPexit->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPexitLeftDown,0,this);
    BPexit->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPexitLeftUp,0,this);
    LlTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnLlTrimLeftDown,0,this);
    LlTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnLlTrimLeftUp,0,this);
    LuTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnLuTrimLeftDown,0,this);
    LuTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnLuTrimLeftUp,0,this);
    LdTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnLdTrimLeftDown,0,this);
    LdTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnLdTrimLeftUp,0,this);
    RdTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnRdTrimLeftDown,0,this);
    RdTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnRdTrimLeftUp,0,this);
    RuTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnRuTrimLeftDown,0,this);
    RuTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnRuTrimLeftUp,0,this);
    RlTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnRlTrimLeftDown,0,this);
    RlTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnRlTrimLeftUp,0,this);
    RrTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnRrTrimLeftDown,0,this);
    RrTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnRrTrimLeftUp,0,this);
    LrTrim->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnLrTrimLeftDown,0,this);
    LrTrim->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnLrTrimLeftUp,0,this);
    BPg->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPgLeftDown,0,this);
    BPg->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPgLeftUp,0,this);
    BPb->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPbLeftDown,0,this);
    BPb->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPbLeftUp,0,this);
    BPd->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPdLeftDown,0,this);
    BPd->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPdLeftUp,0,this);
    Lstick->Connect(wxEVT_MOTION,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnLstickMouseMove,0,this);
    Rstick->Connect(wxEVT_MOTION,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnRstickMouseMove,0,this);
    Simulcd->Connect(wxEVT_PAINT,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnwxsimulcdPaint,0,this);
    Simulcd->Connect(wxEVT_LEFT_DCLICK,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnSimulcdLeftDClick,0,this);
    PbThr->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnPbThrLeftDown,0,this);
    BpRud->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBpRudLeftDown1,0,this);
    BpEle->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBpEleLeftDown,0,this);
    BpTrn->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBpTrnLeftDown,0,this);
    BpTrn->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBpTrnLeftUp,0,this);
    BpAil->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBpAilLeftDown,0,this);
    BpGea->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBpGeaLeftDown,0,this);
    BpId1->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBpId1LeftDown,0,this);
    BpId2->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBpId2LeftDown,0,this);
    Connect(IdMenuOpenEE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnMenuLoadEeprom);
    Connect(IdMenuSaveEE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnMenuItem4Selected);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnAbout);
    Connect(ID_TIMER10MS,wxEVT_TIMER,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnTimer10msTrigger);
    Connect(ID_TIMERMAIN,wxEVT_TIMER,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnTimerMainTrigger);
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnClose);
    //*)

    //LCD var
    SimuLcdScale = 2;

    SimuLcd_ClientDC = new wxClientDC(Simulcd);
    SimuLcd_Bitmap = wxBitmap(Simulcd->GetClientSize().GetWidth(), Simulcd->GetClientSize().GetHeight());
    SimuLcd_MemoryDC = new wxMemoryDC(SimuLcd_Bitmap);
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

}

//// FW Functions ///////////////////////////////////////////////////


void NextStepRc_SimulatorFrame::StartFirmwareCode()
{
    boardInit(); // Is called by simumain but needed for Spin init
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
    s_anaFilt[0] = 1024;
    s_anaFilt[1] = 1024;
    s_anaFilt[2] = 1024;
    s_anaFilt[3] = 1024;
    Timer10ms.Start(10, false); //Simulate 10mS Interrupt vector
    simumain();
    TimerMain.Start(100, false); // Simulate ?mS cycle for main function
}

void NextStepRc_SimulatorFrame::OnTimerMainTrigger(wxTimerEvent& event)
{
    ChronoMain->Start(0);
    SimuMainLoop();
    Chronoval = ChronoMain->TimeInMicro();
    ChronoMain->Pause();
    StatusBar->SetStatusText(_T("MAIN ")+Chronoval.ToString()+_T(" uS"),1);
}

void NextStepRc_SimulatorFrame::OnTimer10msTrigger(wxTimerEvent& event)
{
    CheckInputs();
    Chrono10ms->Start(0);
    if (OnTglButton->GetValue()) TIMER_10MS_VECT();
    else
    {
        TimerMain.Stop();
        shutDownSimu();
    }
    if (simu_off)
    {
        Timer10ms.Stop();
    }
    Chronoval = Chrono10ms->TimeInMicro();
    Chrono10ms->Pause();
    StatusBar->SetStatusText(_T("10 mS IRQ ")+Chronoval.ToString()+_T(" uS"),2);
}

const void NextStepRc_SimulatorFrame::DrawWxSimuLcd()
{
    uint8_t *p;
    p = displayBuf;
    wxBrush brush_back(*wxGREEN, wxBRUSHSTYLE_SOLID );
    SimuLcd_MemoryDC->SetBrush(brush_back);
    SimuLcd_MemoryDC->DrawRectangle(0,0,4+LCD_W*SimuLcdScale,4+LCD_H*SimuLcdScale);
    wxBrush brush_top(*wxBLACK, wxBRUSHSTYLE_SOLID  );
    SimuLcd_MemoryDC->SetBrush(brush_top);

    for (uint8_t y=0; y < (LCD_H / 8); y++)
    {
        for (uint8_t x=0; x < LCD_W; x++)
        {
            uint8_t bit = *p;
            p++;
            for (uint8_t i=0; i < 8; i++)
            {
                if (bit & 0x01) SimuLcd_MemoryDC->DrawRectangle(2+ x*SimuLcdScale,2+ (y*8*SimuLcdScale) +(i*SimuLcdScale),SimuLcdScale,SimuLcdScale);
                bit >>=1;
            }
        }
    }

    SimuLcd_ClientDC->Blit(0,0,SimuLcd_Bitmap.GetWidth(),SimuLcd_Bitmap.GetHeight(),SimuLcd_MemoryDC,0,0);
}

//// GUI Functions ///////////////////////////////////////////////////

NextStepRc_SimulatorFrame::~NextStepRc_SimulatorFrame()
{
    //(*Destroy(NextStepRc_SimulatorFrame)
    //*)
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
    if (ChronoMain != NULL)
    {
        ChronoMain->Pause();
        Sleep(1);
        delete ChronoMain;
    }
    if (Chrono10ms != NULL)
    {
        Chrono10ms->Pause();
        Sleep(1);
        delete Chrono10ms;
    }
    Sleep(10);
    if (SimuLcd_MemoryDC != NULL) delete SimuLcd_MemoryDC;
    if (SimuLcd_ClientDC != NULL)	delete SimuLcd_ClientDC;
}

void NextStepRc_SimulatorFrame::OnClose(wxCloseEvent& event)
{

}


void NextStepRc_SimulatorFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void NextStepRc_SimulatorFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox( _("NextStepRc Simulateur"), _("Bienvenue dans..."));
}

void NextStepRc_SimulatorFrame::OnwxsimulcdPaint(wxPaintEvent& event)
{
    DrawWxSimuLcd();
}

void NextStepRc_SimulatorFrame::LoadEeprom()
{
    wxFileDialog openFileDialog(this, _("Ouvrir Fichier BIN"), "", "","Fichiers BIN (*.bin)|*.bin", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) return;
    wxFileInputStream input_stream(openFileDialog.GetPath());
    if (!input_stream.IsOk())
    {
        wxLogError("Impossible d'ouvrir le fichier '%s'.", openFileDialog.GetPath());
        return;
    }
    wxFile bin_file(openFileDialog.GetPath());
    if(bin_file.IsOpened())
    {
        bin_file.Read(&simu_eeprom[0], EESIZE);
        bin_file.Close();
    }
}

void NextStepRc_SimulatorFrame::OnMenuLoadEeprom(wxCommandEvent& event)
{
    LoadEeprom();
}

void NextStepRc_SimulatorFrame::OnBPmenuLeftDown(wxMouseEvent& event)
{
    SpinL->ResetPin(4);
}

void NextStepRc_SimulatorFrame::OnBPmenuLeftUp(wxMouseEvent& event)
{
    SpinL->SetPin(4);
}

void NextStepRc_SimulatorFrame::OnBPexitLeftDown(wxMouseEvent& event)
{
    SpinL->ResetPin(5);
}

void NextStepRc_SimulatorFrame::OnBPexitLeftUp(wxMouseEvent& event)
{
    SpinL->SetPin(5);
}

void NextStepRc_SimulatorFrame::OnBPhLeftDown(wxMouseEvent& event)
{
    SpinL->ResetPin(1);
}

void NextStepRc_SimulatorFrame::OnBPhLeftUp(wxMouseEvent& event)
{
    SpinL->SetPin(1);
}

void NextStepRc_SimulatorFrame::OnBPbLeftDown(wxMouseEvent& event)
{
    SpinL->ResetPin(0);
}

void NextStepRc_SimulatorFrame::OnBPbLeftUp(wxMouseEvent& event)
{
    SpinL->SetPin(0);
}

void NextStepRc_SimulatorFrame::OnBPgLeftDown(wxMouseEvent& event)
{
    SpinL->ResetPin(3);
}

void NextStepRc_SimulatorFrame::OnBPgLeftUp(wxMouseEvent& event)
{
    SpinL->SetPin(3);
}

void NextStepRc_SimulatorFrame::OnBPdLeftDown(wxMouseEvent& event)
{
    SpinL->ResetPin(2);
}

void NextStepRc_SimulatorFrame::OnBPdLeftUp(wxMouseEvent& event)
{
    SpinL->SetPin(2);
}

void NextStepRc_SimulatorFrame::OnLlTrimLeftDown(wxMouseEvent& event)
{
    SpinF->ResetPin(7);
}

void NextStepRc_SimulatorFrame::OnLlTrimLeftUp(wxMouseEvent& event)
{
    SpinF->SetPin(7);
}

void NextStepRc_SimulatorFrame::OnLrTrimLeftDown(wxMouseEvent& event)
{
    SpinF->ResetPin(6);
}

void NextStepRc_SimulatorFrame::OnLrTrimLeftUp(wxMouseEvent& event)
{
    SpinF->SetPin(6);
}

void NextStepRc_SimulatorFrame::OnLdTrimLeftDown(wxMouseEvent& event)
{
    SpinF->ResetPin(5);
}

void NextStepRc_SimulatorFrame::OnLdTrimLeftUp(wxMouseEvent& event)
{
    SpinF->SetPin(5);
}

void NextStepRc_SimulatorFrame::OnLuTrimLeftDown(wxMouseEvent& event)
{
    SpinF->ResetPin(4);
}

void NextStepRc_SimulatorFrame::OnLuTrimLeftUp(wxMouseEvent& event)
{
    SpinF->SetPin(4);
}

void NextStepRc_SimulatorFrame::OnRdTrimLeftDown(wxMouseEvent& event)
{
    SpinF->ResetPin(3);
}

void NextStepRc_SimulatorFrame::OnRdTrimLeftUp(wxMouseEvent& event)
{
    SpinF->SetPin(3);
}

void NextStepRc_SimulatorFrame::OnRuTrimLeftDown(wxMouseEvent& event)
{
    SpinF->ResetPin(2);
}

void NextStepRc_SimulatorFrame::OnRuTrimLeftUp(wxMouseEvent& event)
{
    SpinF->SetPin(2);
}

void NextStepRc_SimulatorFrame::OnRlTrimLeftDown(wxMouseEvent& event)
{
    SpinF->ResetPin(1);
}

void NextStepRc_SimulatorFrame::OnRlTrimLeftUp(wxMouseEvent& event)
{
    SpinF->SetPin(1);
}

void NextStepRc_SimulatorFrame::OnRrTrimLeftDown(wxMouseEvent& event)
{
    SpinF->ResetPin(0);
}

void NextStepRc_SimulatorFrame::OnRrTrimLeftUp(wxMouseEvent& event)
{
    SpinF->SetPin(0);
}

void NextStepRc_SimulatorFrame::OnBpTrnLeftDown(wxMouseEvent& event)
{
    SpinL->ResetPin(7);
}

void NextStepRc_SimulatorFrame::OnBpTrnLeftUp(wxMouseEvent& event)
{
    SpinL->SetPin(7);
}

void NextStepRc_SimulatorFrame::OnPbThrLeftDown(wxMouseEvent& event)
{
    SpinG->TogglePin(2);
}

void NextStepRc_SimulatorFrame::OnBpRudLeftDown1(wxMouseEvent& event)
{
    SpinG->TogglePin(0);
}

void NextStepRc_SimulatorFrame::OnBpEleLeftDown(wxMouseEvent& event)
{
    SpinL->TogglePin(6);
}

void NextStepRc_SimulatorFrame::OnBpAilLeftDown(wxMouseEvent& event)
{
    SpinD->TogglePin(7);
}

void NextStepRc_SimulatorFrame::OnBpGeaLeftDown(wxMouseEvent& event)
{
    SpinG->TogglePin(1);
}

void NextStepRc_SimulatorFrame::OnBpId1LeftDown(wxMouseEvent& event)
{
    if (!SpinC->GetPin(1)) SpinC->TogglePin(1);
    else SpinC->TogglePin(0);
}

void NextStepRc_SimulatorFrame::OnBpId2LeftDown(wxMouseEvent& event)
{
    if (!SpinC->GetPin(0)) SpinC->TogglePin(0);
    else SpinC->TogglePin(1);
}

void NextStepRc_SimulatorFrame::CheckInputs()
{
    if (!SpinL->GetPin(4)) BPmenu->SetBackgroundColour(wxColour(* wxWHITE));
    else BPmenu->SetBackgroundColour(wxColour(* wxBLACK));
    BPmenu->Refresh();

    if (!SpinL->GetPin(5)) BPexit->SetBackgroundColour(wxColour(* wxWHITE));
    else BPexit->SetBackgroundColour(wxColour(* wxBLACK));
    BPexit->Refresh();

    if (!SpinL->GetPin(1)) BPh->SetBackgroundColour(wxColour(* wxWHITE));
    else BPh->SetBackgroundColour(wxColour(* wxBLACK));
    BPh->Refresh();

    if (!SpinL->GetPin(0)) BPb->SetBackgroundColour(wxColour(* wxWHITE));
    else BPb->SetBackgroundColour(wxColour(* wxBLACK));
    BPb->Refresh();

    if (!SpinL->GetPin(3)) BPg->SetBackgroundColour(wxColour(* wxWHITE));
    else BPg->SetBackgroundColour(wxColour(* wxBLACK));
    BPg->Refresh();

    if (!SpinL->GetPin(2)) BPd->SetBackgroundColour(wxColour(* wxWHITE));
    else BPd->SetBackgroundColour(wxColour(* wxBLACK));
    BPd->Refresh();

    if (!SpinF->GetPin(7)) LlTrim->SetBackgroundColour(wxColour(* wxWHITE));
    else LlTrim->SetBackgroundColour(wxColour(* wxBLACK));
    LlTrim->Refresh();

    if (!SpinF->GetPin(6)) LrTrim->SetBackgroundColour(wxColour(* wxWHITE));
    else LrTrim->SetBackgroundColour(wxColour(* wxBLACK));
    LrTrim->Refresh();

    if (!SpinF->GetPin(5)) LdTrim->SetBackgroundColour(wxColour(* wxWHITE));
    else LdTrim->SetBackgroundColour(wxColour(* wxBLACK));
    LdTrim->Refresh();

    if (!SpinF->GetPin(4)) LuTrim->SetBackgroundColour(wxColour(* wxWHITE));
    else LuTrim->SetBackgroundColour(wxColour(* wxBLACK));
    LuTrim->Refresh();

    if (!SpinF->GetPin(3)) RdTrim->SetBackgroundColour(wxColour(* wxWHITE));
    else RdTrim->SetBackgroundColour(wxColour(* wxBLACK));
    RdTrim->Refresh();

    if (!SpinF->GetPin(2)) RuTrim->SetBackgroundColour(wxColour(* wxWHITE));
    else RuTrim->SetBackgroundColour(wxColour(* wxBLACK));
    RuTrim->Refresh();

    if (!SpinF->GetPin(1)) RlTrim->SetBackgroundColour(wxColour(* wxWHITE));
    else RlTrim->SetBackgroundColour(wxColour(* wxBLACK));
    RlTrim->Refresh();

    if (!SpinF->GetPin(0)) RrTrim->SetBackgroundColour(wxColour(* wxWHITE));
    else RrTrim->SetBackgroundColour(wxColour(* wxBLACK));
    RrTrim->Refresh();

    if (!SpinG->GetPin(2)) PbThr->SetBackgroundColour(wxColour(* wxWHITE));
    else PbThr->SetBackgroundColour(wxColour(* wxBLACK));
    PbThr->Refresh();

    if (!SpinG->GetPin(0)) BpRud->SetBackgroundColour(wxColour(* wxWHITE));
    else BpRud->SetBackgroundColour(wxColour(* wxBLACK));
    BpRud->Refresh();

    if (!SpinL->GetPin(6)) BpEle->SetBackgroundColour(wxColour(* wxWHITE));
    else BpEle->SetBackgroundColour(wxColour(* wxBLACK));
    BpEle->Refresh();

    if (!SpinD->GetPin(7)) BpAil->SetBackgroundColour(wxColour(* wxWHITE));
    else BpAil->SetBackgroundColour(wxColour(* wxBLACK));
    BpAil->Refresh();

    if (!SpinG->GetPin(1)) BpGea->SetBackgroundColour(wxColour(* wxWHITE));
    else BpGea->SetBackgroundColour(wxColour(* wxBLACK));
    BpGea->Refresh();

    if (!SpinL->GetPin(7)) BpTrn->SetBackgroundColour(wxColour(* wxWHITE));
    else BpTrn->SetBackgroundColour(wxColour(* wxBLACK));
    BpTrn->Refresh();

    if (!SpinC->GetPin(0)) BpId1->SetBackgroundColour(wxColour(* wxWHITE));
    else BpId1->SetBackgroundColour(wxColour(* wxBLACK));
    BpId1->Refresh();

    if (!SpinC->GetPin(1)) BpId2->SetBackgroundColour(wxColour(* wxWHITE));
    else BpId2->SetBackgroundColour(wxColour(* wxBLACK));
    BpId2->Refresh();
}

void NextStepRc_SimulatorFrame::OnOnTglButtonToggle(wxCommandEvent& event)
{
    if (OnTglButton->GetValue())
    {

        if (simu_eeprom[1] == 0)
        {
            int answer = wxMessageBox(_("Charger un fichier eeprom ?"), _("EEPROM VIDE"), wxYES_NO, this);
            if (answer == wxYES) LoadEeprom();
        }
    }
    StartFirmwareCode();
}


void NextStepRc_SimulatorFrame::OnLstickMouseMove(wxMouseEvent& event)
{
    int xmul = 2048000 / (Lstick->GetSize().GetWidth() - 5);
    int ymul = 2048000 / (Lstick->GetSize().GetHeight() -5);
    wxPoint pt(event.GetPosition());
    int x = (pt.x * xmul)/1000;
    int y = 2048 - (pt.y * ymul)/1000;

    if (event.LeftUp()) ; //TODO
    if (event.LeftIsDown())
    {
        s_anaFilt[3] = (uint16_t)x;
        s_anaFilt[1] = (uint16_t)y;
    };
}

void NextStepRc_SimulatorFrame::OnRstickMouseMove(wxMouseEvent& event)
{
    int xmul = 2048 / Rstick->GetSize().GetWidth();
    int ymul = 2048 / Rstick->GetSize().GetHeight();
    wxPoint pt(event.GetPosition());
    int x = (pt.x * xmul);
    int y = 2048 - (pt.y * ymul);

    if (event.LeftUp()) ; //TODO
    if (event.LeftIsDown())
    {
        s_anaFilt[0] = (uint16_t)x;
        s_anaFilt[2] = (uint16_t)y;
    };
}

void NextStepRc_SimulatorFrame::OnSimulcdLeftDClick(wxMouseEvent& event)
{
    wxFileDialog saveFileDialog(this, _("Sauver Capture écran"), "", "", "Fichier BMP (*.bmp)|*.bmp", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;     // the user changed idea...
    wxFileOutputStream output_stream(saveFileDialog.GetPath());
    if (!output_stream.IsOk())
    {
        wxLogError("Ne peut écrire le fichier '%s'.", saveFileDialog.GetPath());
        return;
    }
    SimuLcd_Bitmap.SaveFile(saveFileDialog.GetPath(), wxBITMAP_TYPE_BMP, NULL);

}


void NextStepRc_SimulatorFrame::OnMenuItem4Selected(wxCommandEvent& event)
{
    wxFileDialog saveFileDialog(this, _("Sauver Eeprom"), "", "", "Fichier BIN (*.bin)|*.bin", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;     // the user changed idea...
    wxFile bin_file;
    bin_file.Create(saveFileDialog.GetPath(), true);
    eeFlush(); //Save eeprom immediatly
    if(bin_file.IsOpened())
    {
        bin_file.Seek(0);
        bin_file.Write(&simu_eeprom[0], EESIZE);
        bin_file.Close();
    }
}
