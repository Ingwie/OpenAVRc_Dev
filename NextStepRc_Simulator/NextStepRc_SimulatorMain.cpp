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
const long NextStepRc_SimulatorFrame::ID_WXSIMULCD = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPMENU = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPH = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPEXIT = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPG = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPB = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPD = wxNewId();
const long NextStepRc_SimulatorFrame::ID_LSTICK = wxNewId();
const long NextStepRc_SimulatorFrame::ID_RSTICK = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL3 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_ONTGLBUTTON = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL4 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL1 = wxNewId();
const long NextStepRc_SimulatorFrame::IdMenuOpenEE = wxNewId();
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
    Move(wxPoint(-1,-1));
    SetMaxSize(wxSize(-1,-1));
    {
    	wxIcon FrameIcon;
    	FrameIcon.CopyFromBitmap(avatarnext);
    	SetIcon(FrameIcon);
    }
    PanelPrincipal = new wxPanel(this, ID_PANEL1, wxPoint(424,216), wxSize(800,400), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    Panel1 = new wxPanel(PanelPrincipal, ID_PANEL2, wxPoint(8,8), wxSize(784,64), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    Panel1->SetBackgroundColour(wxColour(151,234,194));
    Panel2 = new wxPanel(PanelPrincipal, ID_PANEL3, wxPoint(8,72), wxSize(784,248), wxSUNKEN_BORDER, _T("ID_PANEL3"));
    Panel2->SetBackgroundColour(wxColour(131,216,243));
    wxsimulcd = new wxPanel(Panel2, ID_WXSIMULCD, wxPoint(256,20), wxSize(256,128), 0, _T("ID_WXSIMULCD"));
    wxsimulcd->SetForegroundColour(wxColour(19,216,14));
    wxsimulcd->SetBackgroundColour(wxColour(120,210,30));
    BPmenu = new wxPanel(Panel2, ID_BPMENU, wxPoint(448,184), wxSize(46,14), wxTAB_TRAVERSAL, _T("ID_BPMENU"));
    BPmenu->SetBackgroundColour(wxColour(0,0,0));
    BPh = new wxPanel(Panel2, ID_BPH, wxPoint(288,168), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_BPH"));
    BPh->SetBackgroundColour(wxColour(0,0,0));
    BPexit = new wxPanel(Panel2, ID_BPEXIT, wxPoint(448,208), wxSize(46,14), wxTAB_TRAVERSAL, _T("ID_BPEXIT"));
    BPexit->SetBackgroundColour(wxColour(0,0,0));
    BPg = new wxPanel(Panel2, ID_BPG, wxPoint(264,192), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_BPG"));
    BPg->SetBackgroundColour(wxColour(0,0,0));
    BPb = new wxPanel(Panel2, ID_BPB, wxPoint(288,216), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_BPB"));
    BPb->SetBackgroundColour(wxColour(0,0,0));
    BPd = new wxPanel(Panel2, ID_BPD, wxPoint(312,192), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_BPD"));
    BPd->SetBackgroundColour(wxColour(0,0,0));
    Lstick = new wxPanel(Panel2, ID_LSTICK, wxPoint(60,32), wxSize(158,158), wxTAB_TRAVERSAL, _T("ID_LSTICK"));
    Lstick->SetBackgroundColour(wxColour(0,0,0));
    Rstick = new wxPanel(Panel2, ID_RSTICK, wxPoint(552,34), wxSize(158,158), wxTAB_TRAVERSAL, _T("ID_RSTICK"));
    Rstick->SetBackgroundColour(wxColour(0,0,0));
    Panel3 = new wxPanel(PanelPrincipal, ID_PANEL4, wxPoint(8,312), wxSize(784,64), wxRAISED_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    Panel3->SetBackgroundColour(wxColour(50,167,237));
    OnTglButton = new wxToggleButton(Panel3, ID_ONTGLBUTTON, _("ON"), wxPoint(16,8), wxSize(45,21), wxDOUBLE_BORDER, wxDefaultValidator, _T("ID_ONTGLBUTTON"));
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu1, IdMenuOpenEE, _("Charger Eeprom"), _("Charger fichier BIN"), wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
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

    wxsimulcd->Connect(wxEVT_PAINT,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnwxsimulcdPaint,0,this);
    BPmenu->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPmenuLeftDown,0,this);
    BPmenu->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPmenuLeftUp,0,this);
    BPh->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPhLeftDown,0,this);
    BPh->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPhLeftUp,0,this);
    BPexit->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPexitLeftDown,0,this);
    BPexit->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPexitLeftUp,0,this);
    BPg->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPgLeftDown,0,this);
    BPg->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPgLeftUp,0,this);
    BPb->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPbLeftDown,0,this);
    BPb->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPbLeftUp,0,this);
    BPd->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPdLeftDown,0,this);
    BPd->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnBPdLeftUp,0,this);
    Lstick->Connect(wxEVT_MOTION,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnLstickMouseMove,0,this);
    Rstick->Connect(wxEVT_MOTION,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnRstickMouseMove,0,this);
    Connect(ID_ONTGLBUTTON,wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnOnTglButtonToggle);
    Connect(IdMenuOpenEE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnMenuLoadEeprom);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnAbout);
    Connect(ID_TIMER10MS,wxEVT_TIMER,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnTimer10msTrigger);
    Connect(ID_TIMERMAIN,wxEVT_TIMER,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnTimerMainTrigger);
    //*)

    //LCD var
    SimuLcdScale = 2;

    SimuLcd_ClientDC = new wxClientDC(wxsimulcd);
    SimuLcd_Bitmap = wxBitmap(wxsimulcd->GetClientSize().GetWidth(), wxsimulcd->GetClientSize().GetHeight());
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
    SimuLcd_MemoryDC->DrawRectangle(0,0,LCD_W*SimuLcdScale,LCD_H*SimuLcdScale);
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
                if (bit & 0x01) SimuLcd_MemoryDC->DrawRectangle(x*SimuLcdScale,(y*8*SimuLcdScale) +(i*SimuLcdScale),SimuLcdScale,SimuLcdScale);
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
        delete ChronoMain;
    }
    if (Chrono10ms != NULL)
    {
        Chrono10ms->Pause();
        delete Chrono10ms;
    }
    if (SimuLcd_MemoryDC != NULL) delete SimuLcd_MemoryDC;
    if (SimuLcd_ClientDC != NULL)	delete SimuLcd_ClientDC;
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
    int xmul = 2048 / Lstick->GetSize().GetWidth();
    int ymul = 2048 / Lstick->GetSize().GetHeight();
    wxPoint pt(event.GetPosition());
    int x = (pt.x * xmul);
    int y = 2048 - (pt.y * ymul);

    if (event.LeftUp()) ; //TODO
    if (event.LeftDown()) ; //TODO
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
    if (event.LeftDown()) ; //TODO
    {
        s_anaFilt[0] = (uint16_t)x;
        s_anaFilt[2] = (uint16_t)y;
    };
}
