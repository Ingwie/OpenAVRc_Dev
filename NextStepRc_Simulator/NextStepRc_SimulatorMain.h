/***************************************************************
 * Name:      NextStepRc_SimulatorMain.h
 * Purpose:   Defines Application Frame
 * Author:    NextStepRc_TEAM ()
 * Created:   2016-09-12
 * Copyright: NextStepRc_TEAM ()
 * License:
 **************************************************************/

#ifndef NEXTSTEPRC_SIMULATORMAIN_H
#define NEXTSTEPRC_SIMULATORMAIN_H

//(*Headers(NextStepRc_SimulatorFrame)
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/statbmp.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

class NextStepRc_SimulatorFrame: public wxFrame
{
    public:

        NextStepRc_SimulatorFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~NextStepRc_SimulatorFrame();

    private:

        //(*Handlers(NextStepRc_SimulatorFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        //*)

        //(*Identifiers(NextStepRc_SimulatorFrame)
        static const long ID_STATICBITMAP1;
        static const long ID_PANEL2;
        static const long ID_BUTTON1;
        static const long ID_PANEL1;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(NextStepRc_SimulatorFrame)
        wxButton* Button1;
        wxStaticBitmap* StaticBitmap1;
        wxPanel* Panel1;
        wxStatusBar* StatusBar1;
        wxPanel* Panel2;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // NEXTSTEPRC_SIMULATORMAIN_H
