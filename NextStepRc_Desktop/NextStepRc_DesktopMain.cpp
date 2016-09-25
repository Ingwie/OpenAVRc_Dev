/***************************************************************
 * Name:      NextStepRc_DesktopMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    NextStepRc_TEAM ()
 * Created:   2016-09-08
 * Copyright: NextStepRc_TEAM ()
 * License:
 **************************************************************/

#include "NextStepRc_DesktopMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(NextStepRc_DesktopFrame)
#include <wx/intl.h>
#include <wx/string.h>
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

//(*IdInit(NextStepRc_DesktopFrame)
const long NextStepRc_DesktopFrame::ID_BUTTON1 = wxNewId();
const long NextStepRc_DesktopFrame::ID_PANEL1 = wxNewId();
const long NextStepRc_DesktopFrame::idMenuQuit = wxNewId();
const long NextStepRc_DesktopFrame::idMenuAbout = wxNewId();
const long NextStepRc_DesktopFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(NextStepRc_DesktopFrame,wxFrame)
    //(*EventTable(NextStepRc_DesktopFrame)
    //*)
END_EVENT_TABLE()

NextStepRc_DesktopFrame::NextStepRc_DesktopFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(NextStepRc_DesktopFrame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxMenu* Menu2;

    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(240,40), wxDefaultSize, 0, _T("ID_PANEL1"));
    Panel1->SetFocus();
    MenuBar_main = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quitter\tAlt-F4"), _("Quitter l\'application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar_main->Append(Menu1, _("Action"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("A propos\tF1"), _("NestStepRc_Desktop V 0.00001 !"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar_main->Append(Menu2, _("Aide"));
    SetMenuBar(MenuBar_main);
    StatusBar_main = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar_main->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar_main->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar_main);

    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnAbout);
    //*)
}

NextStepRc_DesktopFrame::~NextStepRc_DesktopFrame()
{
    //(*Destroy(NextStepRc_DesktopFrame)
    //*)
}

void NextStepRc_DesktopFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void NextStepRc_DesktopFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg ="NestStepRc_Desktop V0.00002 !";
    wxMessageBox(msg, _("Bienvenue dans ..."));
}

