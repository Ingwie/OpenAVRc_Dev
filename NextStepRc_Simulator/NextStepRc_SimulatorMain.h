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
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/statusbr.h>
//*)

//(*Firmware
#include "../NextStepRc/nextsteprc.h"
#include "Spin.h"
//*)


class NextStepRc_SimulatorFrame: public wxFrame
{
    public:

        NextStepRc_SimulatorFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~NextStepRc_SimulatorFrame();
        const void DrawWxSimuLcd();

    private:
        //(*Handlers(NextStepRc_SimulatorFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnPanel2MouseMove(wxMouseEvent& event);
        void DrawLcd(wxCommandEvent& event);
        void OnwxlcdKeyDown(wxKeyEvent& event);
        void OnwxsimulcdPaint(wxPaintEvent& event);
        void StartFirmwareCode();
        void OnMenuLoadEeprom(wxCommandEvent& event);
        void OnButtonMenuClick(wxCommandEvent& event);
        void OnButtonMenuClick1(wxCommandEvent& event);
        void OnPanel2Paint(wxPaintEvent& event);
        void OnPanel2LeftUp(wxMouseEvent& event);
        void OnPanel2LeftUp1(wxMouseEvent& event);
        void OnBPmenuLeftDown(wxMouseEvent& event);
        void OnBPmenuLeftUp(wxMouseEvent& event);
        void OnBPexitLeftDown(wxMouseEvent& event);
        void OnBPexitLeftUp(wxMouseEvent& event);
        void OnBPhLeftDown(wxMouseEvent& event);
        void OnBPhLeftUp(wxMouseEvent& event);
        void OnBPbLeftDown(wxMouseEvent& event);
        void OnBPbLeftUp(wxMouseEvent& event);
        void OnBPgLeftDown(wxMouseEvent& event);
        void OnBPgLeftUp(wxMouseEvent& event);
        void OnBPdLeftDown(wxMouseEvent& event);
        void OnBPdLeftUp(wxMouseEvent& event);
        void OnTimer10msTrigger(wxTimerEvent& event);
        //*)

        //(*Identifiers(NextStepRc_SimulatorFrame)
        static const long ID_PANEL2;
        static const long ID_WXSIMULCD;
        static const long ID_BPMENU;
        static const long ID_BPH;
        static const long ID_BPEXIT;
        static const long ID_BPG;
        static const long ID_BPB;
        static const long ID_BPD;
        static const long ID_SLIDER1;
        static const long ID_PANEL3;
        static const long ID_BUTTONON;
        static const long ID_PANEL4;
        static const long ID_PANEL1;
        static const long IdMenuOpenEE;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        static const long ID_TIMER10MS;
        //*)

     //(*Variables(NextStepRc_SimulatorFrame)
       uint8_t SimuLcdScale;
      Spin* SpinA;
       Spin* SpinB;
       Spin* SpinC;
       Spin* SpinD;
       Spin* SpinE;
       Spin* SpinF;
       Spin* SpinG;
       Spin* SpinH;
       Spin* SpinJ;
       Spin* SpinK;
       Spin* SpinL;
       //*)


        //(*Declarations(NextStepRc_SimulatorFrame)
        wxPanel* BPg;
        wxTimer Timer10ms;
        wxPanel* BPexit;
        wxPanel* BPd;
        wxPanel* BPmenu;
        wxButton* ButtonON;
        wxPanel* Panel1;
        wxPanel* wxsimulcd;
        wxPanel* BPh;
        wxPanel* Panel3;
        wxMenuItem* MenuItem3;
        wxPanel* BPb;
        wxStatusBar* StatusBar1;
        wxPanel* Panel2;
        wxSlider* Slidergaz;
        wxPanel* PanelPrincipal;
        //*)

        DECLARE_EVENT_TABLE()


};

#endif // NEXTSTEPRC_SIMULATORMAIN_H
