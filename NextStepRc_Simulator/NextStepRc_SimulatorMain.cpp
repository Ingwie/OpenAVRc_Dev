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
#include <wx/settings.h>
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
const long NextStepRc_SimulatorFrame::ID_BUTTONON = wxNewId();
const long NextStepRc_SimulatorFrame::ID_WXSIMULCD = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPMENU = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPH = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPEXIT = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPG = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPB = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BPD = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL3 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL4 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL1 = wxNewId();
const long NextStepRc_SimulatorFrame::IdMenuOpenEE = wxNewId();
const long NextStepRc_SimulatorFrame::idMenuQuit = wxNewId();
const long NextStepRc_SimulatorFrame::idMenuAbout = wxNewId();
const long NextStepRc_SimulatorFrame::ID_STATUSBAR1 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_TIMER10MS = wxNewId();
//*)

BEGIN_EVENT_TABLE(NextStepRc_SimulatorFrame,wxFrame)
    //(*EventTable(NextStepRc_SimulatorFrame)
    //*)
END_EVENT_TABLE()

NextStepRc_SimulatorFrame::NextStepRc_SimulatorFrame(wxWindow* parent,wxWindowID id)
{
    //LCD var
    SimuLcdScale = 2;
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
    ButtonON = new wxButton(Panel2, ID_BUTTONON, _("ON"), wxPoint(8,8), wxSize(38,22), 0, wxDefaultValidator, _T("ID_BUTTONON"));
    wxsimulcd = new wxPanel(Panel2, ID_WXSIMULCD, wxPoint(256,32), wxSize(256,128), wxTAB_TRAVERSAL, _T("ID_WXSIMULCD"));
    wxsimulcd->SetForegroundColour(wxColour(19,216,14));
    wxsimulcd->SetBackgroundColour(wxColour(120,210,30));
    BPmenu = new wxPanel(Panel2, ID_BPMENU, wxPoint(448,184), wxSize(46,14), wxTAB_TRAVERSAL, _T("ID_BPMENU"));
    BPmenu->SetFocus();
    BPmenu->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_SCROLLBAR));
    BPh = new wxPanel(Panel2, ID_BPH, wxPoint(288,168), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_BPH"));
    BPh->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_SCROLLBAR));
    BPexit = new wxPanel(Panel2, ID_BPEXIT, wxPoint(448,208), wxSize(46,14), wxTAB_TRAVERSAL, _T("ID_BPEXIT"));
    BPexit->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_SCROLLBAR));
    BPg = new wxPanel(Panel2, ID_BPG, wxPoint(264,192), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_BPG"));
    BPg->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_SCROLLBAR));
    BPb = new wxPanel(Panel2, ID_BPB, wxPoint(288,216), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_BPB"));
    BPb->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_SCROLLBAR));
    BPd = new wxPanel(Panel2, ID_BPD, wxPoint(312,192), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_BPD"));
    BPd->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_SCROLLBAR));
    Panel3 = new wxPanel(PanelPrincipal, ID_PANEL4, wxPoint(8,312), wxSize(784,64), wxRAISED_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    Panel3->SetBackgroundColour(wxColour(50,167,237));
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu1, IdMenuOpenEE, _("Charger Eeprom"), _("Charger fichier EEPE"), wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quitter\tAlt-F4"), _("Quitter le simulateur"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&Fichier"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("A propos ...\tF1"), _("C\'est quoi donc \?"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("&Aide"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    Timer10ms.SetOwner(this, ID_TIMER10MS);
    Center();

    Connect(ID_BUTTONON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnButton1Click);
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
    Connect(IdMenuOpenEE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnMenuLoadEeprom);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnAbout);
    Connect(ID_TIMER10MS,wxEVT_TIMER,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnTimer10msTrigger);
    //*)
}

//// FW Functions ///////////////////////////////////////////////////


const void NextStepRc_SimulatorFrame::DrawWxSimuLcd()
{
    uint8_t *p;
    p = displayBuf;
    wxClientDC dc(wxsimulcd);
    wxBrush brush_back(*wxGREEN, wxBRUSHSTYLE_SOLID  );
    dc.SetBrush(brush_back);
    dc.DrawRectangle(0,0,LCD_W*SimuLcdScale,LCD_H*SimuLcdScale);
    wxBrush brush(*wxBLACK, wxBRUSHSTYLE_SOLID  );
    dc.SetBrush(brush);

    for (uint8_t y=0; y < (LCD_H / 8); y++)
    {
        for (uint8_t x=0; x < LCD_W; x++)
        {
            uint8_t bit = *p;
            p++;
            for (uint8_t i=0; i < 8; i++)
            {
                if (bit & 0x01) dc.DrawRectangle(x*SimuLcdScale,(y*8*SimuLcdScale) +(i*SimuLcdScale),SimuLcdScale,SimuLcdScale);
                bit >>=1;
            }
        }
    }
    dc.SetPen(wxNullPen);
}

void NextStepRc_SimulatorFrame::StartFirmwareCode()
{
    // memset(displayBuf, 0, DISPLAY_BUFER_SIZE); //empty display
    // doSplash();
    boardInit();
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

    Timer10ms.Start(10, false);
    nextsteprcInit(simu_mcusr);
    perMain();
}

//// GUI Functions ///////////////////////////////////////////////////

NextStepRc_SimulatorFrame::~NextStepRc_SimulatorFrame()
{
    //(*Destroy(NextStepRc_SimulatorFrame)
    //*)
}

void NextStepRc_SimulatorFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void NextStepRc_SimulatorFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox( _("NextStepRc Simulateur"), _("Bienvenue dans..."));
}

void NextStepRc_SimulatorFrame::OnButton1Click(wxCommandEvent& event)
{
    StartFirmwareCode();
}

void NextStepRc_SimulatorFrame::OnwxsimulcdPaint(wxPaintEvent& event)
{
    DrawWxSimuLcd();
}



void NextStepRc_SimulatorFrame::OnMenuLoadEeprom(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, _("Ouvrir Fichier EEPE"), "", "","Fichiers EEPE (*.eepe)|*.eepe", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
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

void NextStepRc_SimulatorFrame::OnTimer10msTrigger(wxTimerEvent& event)
{
    TIMER_10MS_VECT();
}
