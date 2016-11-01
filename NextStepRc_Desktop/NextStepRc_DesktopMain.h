/***************************************************************
 * Name:      NextStepRc_DesktopMain.h
 * Purpose:   Defines Application Frame
 * Author:    NextStepRc_TEAM ()
 * Created:   2016-09-08
 * Copyright: NextStepRc_TEAM ()
 * License:
 **************************************************************/

#ifndef NEXTSTEPRC_DESKTOPMAIN_H
#define NEXTSTEPRC_DESKTOPMAIN_H


//(*Headers(NextStepRc_DesktopFrame)
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/fileconf.h>

class NextStepRc_DesktopFrame: public wxFrame
{
public:

    NextStepRc_DesktopFrame(wxWindow* parent,wxWindowID id = -1);
    virtual ~NextStepRc_DesktopFrame();

    void LoadConfig();
    void SaveConfig();

    wxFileConfig* configFile;


private:

    //(*Handlers(NextStepRc_DesktopFrame)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnButton1Click(wxCommandEvent& event);
    void OnavrdudeSelected(wxCommandEvent& event);
    void OnProgrammerSelected(wxCommandEvent& event);
    void OnreadmodelsSelected(wxCommandEvent& event);
    void OnreadfirmwareSelected(wxCommandEvent& event);
    void OnEcrirelesFuseesSelected(wxCommandEvent& event);
    void OnEcrirelebootloaderSelected(wxCommandEvent& event);
    void OnWriteModelToRadioSelected(wxCommandEvent& event);
    void OnWriteFirmwareToRadioSelected(wxCommandEvent& event);
    void OnButton1Click1(wxCommandEvent& event);
    void OnSimulateurClick2(wxCommandEvent& event);
    void OnPanel1Paint(wxPaintEvent& event);
    void OnPanel1Paint1(wxPaintEvent& event);
    void OnClose(wxCloseEvent& event);
    //*)

    //(*Identifiers(NextStepRc_DesktopFrame)
    static const long ID_BUTTON1;
    static const long ID_PANEL1;
    static const long idMenuQuit;
    static const long ID_MENUITEM3;
    static const long ID_MENUITEM5;
    static const long ID_MENUITEM4;
    static const long ID_MENUITEM6;
    static const long ID_MENUITEM9;
    static const long ID_MENUITEM10;
    static const long ID_MENUITEM7;
    static const long ID_MENUITEM1;
    static const long idMenuAbout;
    static const long ID_STATUSBAR1;
    //*)

    //(*Declarations(NextStepRc_DesktopFrame)
    wxMenuItem* MenuItem7;
    wxMenuItem* MenuItem5;
    wxMenu* Menu3;
    wxButton* Button1;
    wxMenuItem* MenuItem11;
    wxPanel* Panel1;
    wxMenu* MenuItem8;
    wxMenuItem* MenuItem10;
    wxMenuBar* MenuBar_main;
    wxMenuItem* MenuItem3;
    wxMenu* Menu7;
    wxMenuItem* Menu5;
    wxStatusBar* StatusBar_main;
    wxMenuItem* MenuItem6;
    wxMenu* Menu6;
    wxMenu* Menu4;
    //*)

    wxString dude_c = (" -c ");
    wxString dude_p = (" -p ");
    wxString dude_D = (" -D ");
    wxString dude_P = (" -P "); //same as _p ?????
    wxString dude_space = (" ");
    wxString dude_U = (" -U ");
    wxString dude_eeprom = ("eeprom:");
    wxString dude_flash = ("flash:");
    wxString dude_raw = (":r");
    wxString dude_write = ("w:");
    wxString dude_verify = (" -v ");
    wxString dude_read = ("r:");
    wxString dude_intel = (":i");
    wxString keepopen = ("cmd /k ");

    DECLARE_EVENT_TABLE()
};

#endif // NEXTSTEPRC_DESKTOPMAIN_H

