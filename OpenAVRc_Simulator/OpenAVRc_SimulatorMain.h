 /*
 **************************************************************************
 *                                                                        *
 *                 ____                ___ _   _____                      *
 *                / __ \___  ___ ___  / _ | | / / _ \____                 *
 *               / /_/ / _ \/ -_) _ \/ __ | |/ / , _/ __/                 *
 *               \____/ .__/\__/_//_/_/ |_|___/_/|_|\__/                  *
 *                   /_/                                                  *
 *                                                                        *
 *              This file is part of the OpenAVRc project.                *
 *                                                                        *
 *                         Based on code(s) named :                       *
 *             OpenTx - https://github.com/opentx/opentx                  *
 *             Deviation - https://www.deviationtx.com/                   *
 *                                                                        *
 *                Only AVR code here for visibility ;-)                   *
 *                                                                        *
 *   OpenAVRc is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation, either version 2 of the License, or    *
 *   (at your option) any later version.                                  *
 *                                                                        *
 *   OpenAVRc is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *   GNU General Public License for more details.                         *
 *                                                                        *
 *       License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html          *
 *                                                                        *
 **************************************************************************
*/


#ifndef OpenAVRc_SIMULATORMAIN_H
#define OpenAVRc_SIMULATORMAIN_H

//(*Headers(OpenAVRc_SimulatorFrame)
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/slider.h>
#include <wx/spinbutt.h>
#include <wx/statusbr.h>
#include <wx/textctrl.h>
#include <wx/tglbtn.h>
#include <wx/timer.h>
//*)

#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/fileconf.h>
#include <wx/dcmemory.h>
#include <wx/process.h>


//(*Firmware
#include "../OpenAVRc/OpenAVRc.h"
#include "Spin.h"
//*)

//SD
wxString simu_dir;


extern wxString AppPath;
static wxLongLong Chronoval;

//UTILS
wxString ConvCharFwToWxstr(char *cstr, uint8_t length);
void ConvWxstrToCharFw(wxString str,char *fwchar, uint8_t length);
wxString int2wxString(int integer);

class wxBackgroundBitmap : public wxEvtHandler
{
  typedef wxEvtHandler Inherited;
public:
  wxBackgroundBitmap(const wxBitmap &B) : Bitmap(B), wxEvtHandler() { }
  virtual bool ProcessEvent(wxEvent &Event);
protected:
  wxBitmap Bitmap;
};

class OpenAVRc_SimulatorFrame: public wxFrame
{
public:

  OpenAVRc_SimulatorFrame(wxWindow* parent,wxWindowID id = -1);
  virtual ~OpenAVRc_SimulatorFrame();
  const void DrawWxSimuLcd();
  void OnKey(wxKeyEvent& Event);
  void EnableMixerFrameMenu();
  void EnableOutputBarsMenu();
  void EnableOutputGvarsMenu();
  void EnableRadioDataMenu();



private:
  //(*Handlers(OpenAVRc_SimulatorFrame)
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void DrawLcd(wxCommandEvent& event);
  void OnwxlcdKeyDown(wxKeyEvent& event);
  void OnwxsimulcdPaint(wxPaintEvent& event);
  void StartFirmwareCode();
  void CheckInputs();
  void LoadEeprom();
  void OnMenuLoadEeprom(wxCommandEvent& event);
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
  void OnBpThrLeftDown(wxMouseEvent& event);
  void OnPbRudLeftDown(wxMouseEvent& event);
  void OnPbEleLeftDown(wxMouseEvent& event);
  void OnPbAilLeftDown(wxMouseEvent& event);
  void OnBpGeaLeftDown(wxMouseEvent& event);
  void OnBpId1LeftDown(wxMouseEvent& event);
  void OnBpId2LeftDown(wxMouseEvent& event);
  void OnBpTrnLeftDown(wxMouseEvent& event);
  void OnBpTrnLeftUp(wxMouseEvent& event);
  void OnBpEleLeftDown(wxMouseEvent& event);
  void OnBpAilLeftDown(wxMouseEvent& event);
  void LoadEepromFile(wxString path);
  wxColour SetColour(wxColour mem);
  void LoadConfig();
  void SaveConfig();
  void ResetSimuLcd();
  void PaintSticks(int x, int y, int xm, int ym, wxPanel* stick);
  void OnClose(wxCloseEvent& event);
  void OnMenuLcdBackSelected(wxCommandEvent& event);
  void OnMenuLcdPixelSelected(wxCommandEvent& event);
  void OnMenuButOffSelected(wxCommandEvent& event);
  void OnMenuButOnSelected(wxCommandEvent& event);
  void OnMenuStickBackSelected(wxCommandEvent& event);
  void OnMenuStickStickSelected(wxCommandEvent& event);
  void OnMenuSaveeeSelected(wxCommandEvent& event);
  void OnLstickPaint(wxPaintEvent& event);
  void OnRstickPaint(wxPaintEvent& event);
  void OnBpTrnRightDown(wxMouseEvent& event);
  void OnButtonStartDesktopClick(wxCommandEvent& event);
  void OnBpRudLeftDown(wxMouseEvent& event);
  void PlayTts();
  void OnMixeurSelected(wxCommandEvent& event);
  void OnOutputBarsSelected(wxCommandEvent& event);
  void OnOutputGvarsSelected(wxCommandEvent& event);
  void OnRadioDataSelected(wxCommandEvent& event);
  void OnBpReaLeftDown(wxMouseEvent& event);
  void OnBpReaLeftUp(wxMouseEvent& event);
  void OnBpRebLeftDown(wxMouseEvent& event);
  void OnBpRebLeftUp(wxMouseEvent& event);
  void OnMenuExportEepromSelected(wxCommandEvent& event);
  void ExportEeprom();
  void ImportEeprom();
  void load_EEGeneral_217();
  void load_ModelData_217();
  void save_EEGeneral_30(EEGeneral General);
  void save_ModelData_30();
  void load_EEGeneral_30();
  void load_ModelData_30();
  void OnMenuImportEepromSelected(wxCommandEvent& event);
  //*)

  //(*Identifiers(OpenAVRc_SimulatorFrame)
  static const long ID_PANELH;
  static const long ID_POT1;
  static const long ID_POT2;
  static const long ID_POT3;
  static const long ID_BPMENU;
  static const long ID_BPH;
  static const long ID_BPEXIT;
  static const long ID_LLTRIM;
  static const long ID_LUTRIM;
  static const long ID_LDTRIM;
  static const long ID_RDTRIM;
  static const long ID_RUTRIM;
  static const long ID_RLTRIM;
  static const long ID_RRTRIM;
  static const long ID_LRTRIM;
  static const long ID_BPG;
  static const long ID_BPB;
  static const long ID_BPD;
  static const long ID_RSTICK;
  static const long ID_SIMULCD;
  static const long ID_BPTHR;
  static const long ID_BPRUD;
  static const long ID_BPELE;
  static const long ID_BPTRN;
  static const long ID_BPAIL;
  static const long ID_BPGEA;
  static const long ID_PBID1;
  static const long ID_BOID2;
  static const long ID_LSTICK;
  static const long ID_SPINREA;
  static const long ID_SPINREB;
  static const long ID_TEXTCTRLDUMMY;
  static const long ID_BPREA;
  static const long ID_BPREB;
  static const long ID_PANELMAIN;
  static const long ID_ONTGLBUTTON;
  static const long ID_BUTTONSTARTDESKTOP;
  static const long ID_PANELL;
  static const long ID_PANELPRINCIPAL;
  static const long IdMenuOpenEE;
  static const long IdMenuSaveEE;
  static const long IDMENUEXPORTEEPROM;
  static const long IDMENUIMPORTEEPROM;
  static const long idMenuQuit;
  static const long ID_LCDB;
  static const long ID_LCDF;
  static const long ID_BUTOFF;
  static const long ID_BUTON;
  static const long ID_STICKB;
  static const long ID_STICKF;
  static const long ID_COLOURS;
  static const long ID_MENUITEMOUTPUTMIXER;
  static const long ID_MENUITEMOUTPUTOUTPUT;
  static const long ID_MENUITEMOUTPUTGVARS;
  static const long ID_MENUITEMRADIODATA;
  static const long idMenuAbout;
  static const long ID_STATUSBAR;
  static const long ID_TIMER10MS;
  static const long ID_TIMERMAIN;
  //*)

  //(*Variables(OpenAVRc_SimulatorFrame)
  wxFileConfig* configFile;
  wxString Ini_Filename;
  wxFileConfig* eepromfile;
  wxString CurrentEEPath;

  wxColour Col_Lcd_Back;
  wxColour Col_Lcd_Front;
  wxColour Col_Button_Off;
  wxColour Col_Button_On;
  wxColour Col_Stick_Back;
  wxColour Col_Stick_Circle;




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


  //(*Declarations(OpenAVRc_SimulatorFrame)
  wxButton* ButtonStartDesktop;
  wxMenu* MenuColours;
  wxMenu* MenuFile;
  wxMenu* MenuFrame;
  wxMenu* MenuOption;
  wxMenuItem* MenuButOff;
  wxMenuItem* MenuButOn;
  wxMenuItem* MenuExportEeprom;
  wxMenuItem* MenuImportEeprom;
  wxMenuItem* MenuLcdBack;
  wxMenuItem* MenuLcdPixel;
  wxMenuItem* MenuLoadee;
  wxMenuItem* MenuSaveee;
  wxMenuItem* MenuStickBack;
  wxMenuItem* MenuStickStick;
  wxMenuItem* OutputBars;
  wxMenuItem* OutputGvars;
  wxMenuItem* OutputMixeur;
  wxMenuItem* RadioData;
  wxPanel* BPb;
  wxPanel* BPd;
  wxPanel* BPexit;
  wxPanel* BPg;
  wxPanel* BPh;
  wxPanel* BPmenu;
  wxPanel* BpAil;
  wxPanel* BpEle;
  wxPanel* BpGea;
  wxPanel* BpId1;
  wxPanel* BpId2;
  wxPanel* BpRea;
  wxPanel* BpReb;
  wxPanel* BpRud;
  wxPanel* BpThr;
  wxPanel* BpTrn;
  wxPanel* LdTrim;
  wxPanel* LlTrim;
  wxPanel* LrTrim;
  wxPanel* Lstick;
  wxPanel* LuTrim;
  wxPanel* PanelH;
  wxPanel* PanelL;
  wxPanel* PanelMain;
  wxPanel* PanelPrincipal;
  wxPanel* RdTrim;
  wxPanel* RlTrim;
  wxPanel* RrTrim;
  wxPanel* Rstick;
  wxPanel* RuTrim;
  wxPanel* Simulcd;
  wxSlider* Pot1;
  wxSlider* Pot2;
  wxSlider* Pot3;
  wxSpinButton* SpinRea;
  wxSpinButton* SpinReb;
  wxStatusBar* StatusBar;
  wxTextCtrl* TextCtrlgetkbinput;
  wxTimer Timer10ms;
  wxTimer TimerMain;
  wxToggleButton* OnTglButton;
  //*)


  // To paint texture on objects
  wxBackgroundBitmap* PanelMainBackground;
  wxBackgroundBitmap* PanelHBackckground;
  wxBackgroundBitmap* PanelBBackckground;

  wxProcess* Mp3process;


  DECLARE_EVENT_TABLE()
};


#endif // OpenAVRc_SIMULATORMAIN_H
