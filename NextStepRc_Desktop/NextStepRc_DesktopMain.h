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

class NextStepRc_DesktopFrame: public wxFrame
{
    public:

        NextStepRc_DesktopFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~NextStepRc_DesktopFrame();

    private:

        //(*Handlers(NextStepRc_DesktopFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        //*)

        //(*Identifiers(NextStepRc_DesktopFrame)
        static const long ID_BUTTON1;
        static const long ID_PANEL1;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(NextStepRc_DesktopFrame)
        wxButton* Button1;
        wxPanel* Panel1;
        wxMenuBar* MenuBar_main;
        wxStatusBar* StatusBar_main;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // NEXTSTEPRC_DESKTOPMAIN_H
