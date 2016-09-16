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

//(*InternalHeaders(NextStepRc_SimulatorFrame)
#include <wx/bitmap.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

//(*Firmware
#include "../NextStepRc/nextsteprc.h"
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

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
const long NextStepRc_SimulatorFrame::ID_STATICBITMAP1 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_PANEL2 = wxNewId();
const long NextStepRc_SimulatorFrame::ID_BUTTON1 = wxNewId();
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

    Create(parent, id, _("Simulateur"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    SetClientSize(wxSize(637,323));
    Move(wxDefaultPosition);
    SetMaxSize(wxSize(-1,-1));
    Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(360,16), wxDLG_UNIT(this,wxSize(442,206)), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    Panel1->SetMaxSize(wxSize(0,0));
    Panel1->Disable();
    Panel2 = new wxPanel(Panel1, ID_PANEL2, wxDLG_UNIT(Panel1,wxPoint(128,0)), wxDLG_UNIT(Panel1,wxSize(148,82)), wxDOUBLE_BORDER|wxCLIP_CHILDREN, _T("ID_PANEL2"));
    StaticBitmap1 = new wxStaticBitmap(Panel2, ID_STATICBITMAP1, wxBitmap(wxImage(_T("C:\\Users\\Christophe\\Desktop\\NextStepRc_Builder\\NextStepRc\\bitmaps\\splash.bmp")).Rescale(wxDLG_UNIT(Panel2,wxSize(128,64)).GetWidth(),wxDLG_UNIT(Panel2,wxSize(128,64)).GetHeight())), wxDLG_UNIT(Panel2,wxPoint(10,9)), wxDLG_UNIT(Panel2,wxSize(128,64)), wxSIMPLE_BORDER, _T("ID_STATICBITMAP1"));
    StaticBitmap1->SetFocus();
    Button1 = new wxButton(Panel1, ID_BUTTON1, _("start"), wxPoint(104,232), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quitter\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&Fichier"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Aide"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);

    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnButton1Click);
    Panel1->Connect(wxEVT_PAINT,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnPanel1Paint,0,this);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_SimulatorFrame::OnAbout);
    //*)
}

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
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void NextStepRc_SimulatorFrame::OnButton1Click(wxCommandEvent& event)
{
  per10ms();
}
