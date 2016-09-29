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


//(*InternalHeaders(NextStepRc_SimulatorFrame)
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

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
const long NextStepRc_SimulatorFrame::ID_STATICBITMAP1 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BUTTON1 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL3 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL4 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL1 = wxNewId();
const long NextStepRc_SimulatorFrame::idMenuQuit = wxNewId();
const long NextStepRc_SimulatorFrame::idMenuAbout = wxNewId();
const long NextStepRc_SimulatorFrame::ID_STATUSBAR1 = wxNewId();
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
        FrameIcon.CopyFromBitmap(wxBitmap(wxImage(_T("C:\\NextStepRc_Builder\\NextStepRc_Simulator\\avatarnext.xpm"))));
        SetIcon(FrameIcon);
    }
    PanelPrincipal = new wxPanel(this, ID_PANEL1, wxPoint(424,216), wxSize(800,400), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    Panel1 = new wxPanel(PanelPrincipal, ID_PANEL2, wxPoint(8,8), wxSize(784,64), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    Panel1->SetBackgroundColour(wxColour(151,234,194));
    Panel2 = new wxPanel(PanelPrincipal, ID_PANEL3, wxPoint(8,72), wxSize(784,248), wxSUNKEN_BORDER, _T("ID_PANEL3"));
    Panel2->SetBackgroundColour(wxColour(131,216,243));
    StaticBitmap1 = new wxStaticBitmap(Panel2, ID_STATICBITMAP1, wxBitmap(wxImage(_T("splash.xpm")).Rescale(wxSize(256,128).GetWidth(),wxSize(256,128).GetHeight())), wxPoint(272,8), wxSize(256,128), wxDOUBLE_BORDER, _T("ID_STATICBITMAP1"));
    Button1 = new wxButton(Panel2, ID_BUTTON1, _("test"), wxPoint(104,112), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    Panel3 = new wxPanel(PanelPrincipal, ID_PANEL4, wxPoint(8,312), wxSize(784,64), wxRAISED_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    Panel3->SetBackgroundColour(wxColour(50,167,237));
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
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
    Center();

    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnButton1Click);
    Panel2->Connect(wxEVT_MOTION,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnPanel2MouseMove,0,this);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnAbout);
    Bind(EVT_DRAW_LCD, (wxObjectEventFunction)&NextStepRc_SimulatorFrame::DispatchMessages, this);
    //*)
}

NextStepRc_SimulatorFrame::~NextStepRc_SimulatorFrame()
{
    //(*Destroy(NextStepRc_SimulatorFrame)
    //*)
}

void NextStepRc_SimulatorFrame::DispatchMessages(wxCommandEvent& event)
{
    ProcessWindowEvent(event);
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
    nextsteprcInit(simu_mcusr);
    //perMain();
    per10ms();
    per10ms();
    per10ms();
    per10ms();

}

void NextStepRc_SimulatorFrame::OnPanel2MouseMove(wxMouseEvent& event)
{
    if (event.Dragging())
    {
        wxClientDC dc(Panel2);
        wxBrush brush(*wxRED, wxBRUSHSTYLE_SOLID  ); // red pen of width 1
        dc.SetBrush(brush);
        dc.DrawRectangle(00,00,20,20);
        dc.SetPen(wxNullPen);
        // wxMessageBox( _("NextStepRc Simulateur"), _("Bienvenue dans..."));
        simu_pina = 0xFF;
    }

}

void NextStepRc_SimulatorFrame::DrawLcd(wxCommandEvent& event)
{
    wxClientDC dc(Panel2);
    wxBrush brush(*wxRED, wxBRUSHSTYLE_SOLID  ); // red pen of width 1
    dc.SetBrush(brush);
    dc.DrawRectangle(00,00,20,20);
    dc.SetPen(wxNullPen);

}

