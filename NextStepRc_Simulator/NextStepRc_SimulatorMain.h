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
#include <wx/tglbtn.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/statusbr.h>
//*)

#include <wx/dcmemory.h>



//(*Firmware
#include "../NextStepRc/nextsteprc.h"
#include "Spin.h"
//*)

static wxLongLong Chronoval;

class wxBackgroundBitmap : public wxEvtHandler
{
    typedef wxEvtHandler Inherited;
public:
    wxBackgroundBitmap(const wxBitmap &B) : Bitmap(B), wxEvtHandler() { }
    virtual bool ProcessEvent(wxEvent &Event);
protected:
    wxBitmap Bitmap;
};

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
    void DrawLcd(wxCommandEvent& event);
    void OnwxlcdKeyDown(wxKeyEvent& event);
    void OnwxsimulcdPaint(wxPaintEvent& event);
    void StartFirmwareCode();
    void CheckInputs();
    void LoadEeprom();
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
    void OnTimerMainTrigger(wxTimerEvent& event);
    void OnOnTglButtonToggle(wxCommandEvent& event);
    void OnLstickMouseMove(wxMouseEvent& event);
    void OnRstickMouseMove(wxMouseEvent& event);
    void OnLlTrimLeftUp(wxMouseEvent& event);
    void OnLlTrimLeftDown(wxMouseEvent& event);
    void OnLrTrimLeftDown(wxMouseEvent& event);
    void OnLrTrimLeftUp(wxMouseEvent& event);
    void OnLdTrimLeftDown(wxMouseEvent& event);
    void OnLdTrimLeftUp(wxMouseEvent& event);
    void OnLuTrimLeftDown(wxMouseEvent& event);
    void OnLuTrimLeftUp(wxMouseEvent& event);
    void OnRdTrimLeftDown(wxMouseEvent& event);
    void OnRdTrimLeftUp(wxMouseEvent& event);
    void OnRuTrimLeftDown(wxMouseEvent& event);
    void OnRuTrimLeftUp(wxMouseEvent& event);
    void OnRlTrimLeftDown(wxMouseEvent& event);
    void OnRlTrimLeftUp(wxMouseEvent& event);
    void OnRrTrimLeftDown(wxMouseEvent& event);
    void OnRrTrimLeftUp(wxMouseEvent& event);
    void OnSimulcdLeftDClick(wxMouseEvent& event);
    void OnPbThrLeftDown(wxMouseEvent& event);
    void OnPbRudLeftDown(wxMouseEvent& event);
    void OnPbEleLeftDown(wxMouseEvent& event);
    void OnPbAilLeftDown(wxMouseEvent& event);
    void OnBpRudLeftDown(wxMouseEvent& event);
    void OnBpGeaLeftDown(wxMouseEvent& event);
    void OnBpId1LeftDown(wxMouseEvent& event);
    void OnBpId2LeftDown(wxMouseEvent& event);
    void OnBptrnLeftDown(wxMouseEvent& event);
    void OnBptrnLeftUp(wxMouseEvent& event);
    void OnBpTrnLeftDown(wxMouseEvent& event);
    void OnBpTrnLeftUp(wxMouseEvent& event);
    void OnBpRudLeftDown1(wxMouseEvent& event);
    void OnBpEleLeftDown(wxMouseEvent& event);
    void OnBpAilLeftDown(wxMouseEvent& event);
    void OnMenuItem4Selected(wxCommandEvent& event);
    //*)

    //(*Identifiers(NextStepRc_SimulatorFrame)
    static const long ID_PANELH;
    static const long ID_ONTGLBUTTON;
    static const long ID_PANELL;
    static const long ID_BPMENU;
    static const long ID_BPH;
    static const long ID_BPEXIT;
    static const long ID_PANEL9;
    static const long ID_PANEL10;
    static const long ID_PANEL11;
    static const long ID_PANEL12;
    static const long ID_PANEL7;
    static const long ID_PANEL8;
    static const long ID_PANEL6;
    static const long ID_PANEL5;
    static const long ID_BPG;
    static const long ID_BPB;
    static const long ID_BPD;
    static const long ID_RSTICK;
    static const long ID_SIMULCD;
    static const long ID_PANEL14;
    static const long ID_PANEL15;
    static const long ID_PANEL16;
    static const long ID_PANEL17;
    static const long ID_PANEL18;
    static const long ID_PANEL19;
    static const long ID_PANEL20;
    static const long ID_PANEL13;
    static const long ID_LSTICK;
    static const long ID_PANELMAIN;
    static const long ID_PANEL1;
    static const long IdMenuOpenEE;
    static const long IdMenuSaveEE;
    static const long idMenuQuit;
    static const long idMenuAbout;
    static const long ID_STATUSBAR;
    static const long ID_TIMER10MS;
    static const long ID_TIMERMAIN;
    //*)

    //(*Variables(NextStepRc_SimulatorFrame)
    uint8_t SimuLcdScale;
    wxClientDC* SimuLcd_ClientDC;
    wxBitmap SimuLcd_Bitmap;
    wxMemoryDC* SimuLcd_MemoryDC;
    wxStopWatch* ChronoMain;
    wxStopWatch* Chrono10ms;

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
    wxPanel* BpId2;
    wxPanel* BPg;
    wxTimer Timer10ms;
    wxPanel* RrTrim;
    wxPanel* LdTrim;
    wxPanel* RdTrim;
    wxPanel* Lstick;
    wxPanel* BpGea;
    wxPanel* BpRud;
    wxPanel* BPexit;
    wxPanel* BpEle;
    wxPanel* PanelH;
    wxMenuItem* MenuItem4;
    wxPanel* BPd;
    wxPanel* BPmenu;
    wxPanel* LlTrim;
    wxPanel* LrTrim;
    wxPanel* PanelL;
    wxPanel* PbThr;
    wxPanel* Simulcd;
    wxPanel* BPh;
    wxPanel* BpAil;
    wxMenuItem* MenuItem3;
    wxPanel* BPb;
    wxPanel* RuTrim;
    wxPanel* BpTrn;
    wxPanel* PanelMain;
    wxToggleButton* OnTglButton;
    wxTimer TimerMain;
    wxPanel* Rstick;
    wxPanel* RlTrim;
    wxPanel* LuTrim;
    wxPanel* BpId1;
    wxStatusBar* StatusBar;
    wxPanel* PanelPrincipal;
    //*)

    wxBackgroundBitmap* PanelMainBackground;
    wxBackgroundBitmap* PanelHBackckground;


    DECLARE_EVENT_TABLE()
};


#endif // NEXTSTEPRC_SIMULATORMAIN_H
