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
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

//(*Firmware
#include "../NextStepRc/nextsteprc.h"

wxDEFINE_EVENT(EVT_DRAW_LCD, wxCommandEvent);

//*)


class NextStepRc_SimulatorFrame: public wxFrame
{
    public:

        NextStepRc_SimulatorFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~NextStepRc_SimulatorFrame();

        void DrawWxSimuLcd();

    private:
        //(*Handlers(NextStepRc_SimulatorFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnPanel2MouseMove(wxMouseEvent& event);
        void DrawLcd(wxCommandEvent& event);
        void OnwxlcdKeyDown(wxKeyEvent& event);
        void OnwxsimulcdPaint(wxPaintEvent& event);
        //*)

        //(*Identifiers(NextStepRc_SimulatorFrame)
        static const long ID_PANEL2;
        static const long ID_BUTTON1;
        static const long ID_WXSIMULCD;
        static const long ID_PANEL3;
        static const long ID_PANEL4;
        static const long ID_PANEL1;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

     //(*Variables(NextStepRc_SimulatorFrame)
       uint8_t SimuLcdScale = 2;
        //*)


        //(*Declarations(NextStepRc_SimulatorFrame)
        wxButton* Button1;
        wxPanel* Panel1;
        wxPanel* wxsimulcd;
        wxPanel* Panel3;
        wxStatusBar* StatusBar1;
        wxPanel* Panel2;
        wxPanel* PanelPrincipal;
        //*)

        DECLARE_EVENT_TABLE()


};

#endif // NEXTSTEPRC_SIMULATORMAIN_H
