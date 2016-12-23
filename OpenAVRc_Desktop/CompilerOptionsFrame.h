 /*
 **************************************************************************
 *                                                                        *
 *              This file is part of the OpenAVRc project.                *
 *                                                                        *
 *                         Based on code named                            *
 *             OpenTx - https://github.com/opentx/opentx                  *
 *                                                                        *
 *                Only AVR code here for lisibility ;-)                   *
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


#ifndef COMPILEROPTIONSFRAME_H
#define COMPILEROPTIONSFRAME_H

#include "OpenAVRc_DesktopMain.h"


//(*Headers(CompilerOptionsFrame)
#include <wx/notebook.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/frame.h>
//*)

//SPLASH
extern uint8_t LbmSplash[SPLASHLENGHT];


extern wxString CompileBatFile;
//extern wxString LinkToWriteFirmware;

extern wxString PCB;//mandatory
extern wxString LCD;//mandatory
extern wxString VOICE;//defaults to NO
extern wxString EXT;//defaults to STD (no telemetry)
extern bool AUDIO;//defaults to NO
extern bool HELI;//defaults to NO
extern wxString TTS;//mandatory
extern wxString TRANSLATIONS;//mandatory
extern wxString NAVIGATION;//defaults to NO
extern bool FRSKY_HUB;//defaults to NO
extern bool HAPTIC;//defaults to NO
extern wxString PPM_UNIT;//defaults to 1
extern bool GAUGES;//defaults to NO
extern bool GPS;//defaults to NO
extern bool VARIO;//defaults to NO
extern bool RTCLOCK;//defaults to NO
extern bool SPORT_FILE_LOG;//defaults to NO
extern bool PPM;//defaults to YES
extern bool PXX;//defaults to NO
extern bool DSM2SERIAL;//defaults to NO
extern bool DSM2PPM;//defaults to NO
extern bool SD_CARD;//defaults to NO
extern bool FAS_OFFSET;//defaults to NO
extern bool TEMPLATES;//defaults to NO
extern wxString THREE_POS;// defaults to NO
extern bool SPLASH;
extern wxString UNITS;
extern wxString DEFAULT_MODE; //  ??????????yes no?
extern wxString FONT;
extern bool BOLD;
extern bool BATTGRAPH;
extern bool EEPROM_PROGRESS_BAR;
extern wxString FAI;
extern bool AUTOSWITCH;
extern bool AUTOSOURCE;
extern bool DBLKEYS;
extern bool PPM_CENTER_ADJUSTABLE;
extern bool PPM_LIMITS_SYMETRICAL;
extern bool FLIGHT_MODES;//Hardwired ?
extern bool CURVES;//Hardwired ?
extern bool GVARS;//Hardwired ?
extern bool PCBREV;
extern bool TURNIGY_TRANSMITTER_FIX;
extern bool FRSKY_STICKS;
extern bool CORRECT_NEGATIVE_VALUES;
extern bool ARITHMETIC_OVERFLOW_CHECK;
extern bool ACCURAT_THROTTLE_STATS;
extern bool SP22;
extern bool PWM_BACKLIGHT;
extern bool OVERRIDE_CHANNEL_FUNCTION;
extern bool WS_HOW_HIGH;
extern bool TX_CADDY;
extern bool TOGGLETRIM;
extern bool NOANDSECONDE;// Hardwired
extern bool SHUTDOWN_CONFIRMATION;
//DANGEROUS_MODULE_FUNCTIONS// does not compile.


class CompilerOptionsFrame: public wxFrame
{
public:

    CompilerOptionsFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
    virtual ~CompilerOptionsFrame();

    //wxFileConfig* configFile;

    //(*Declarations(CompilerOptionsFrame)
    wxCheckBox* CheckBoxTX_CADDY;
    wxStaticText* StaticText10;
    wxStaticText* StaticText9;
    wxCheckBox* CheckBoxBATTGRAPH;
    wxCheckBox* CheckBoxSHUTDOWN_CONFIRMATION;
    wxCheckBox* CheckBoxSD_CARD;
    wxCheckBox* CheckBoxNOANDSECONDE;
    wxCheckBox* CheckBoxTEMPLATES;
    wxStaticBox* StaticBox5;
    wxCheckBox* CheckBoxPXX;
    wxCheckBox* CheckBoxPPM;
    wxChoice* ChoiceUNITS;
    wxNotebook* Notebook1;
    wxCheckBox* CheckBoxDSM2SERIAL;
    wxCheckBox* CheckBoxPCBREV;
    wxStaticText* StaticText2;
    wxChoice* ChoiceTRANSLATIONS;
    wxChoice* ChoiceFAI;
    wxCheckBox* CheckBoxFRSKY_HUB;
    wxCheckBox* CheckBoxGAUGES;
    wxStaticText* StaticText6;
    wxCheckBox* CheckBoxDBLKEYS;
    wxCheckBox* CheckBoxOVERRIDE_CHANNEL_FUNCTION;
    wxCheckBox* CheckBoxCURVES;
    wxCheckBox* CheckBoxFAS_OFFSET;
    wxStaticText* StaticText8;
    wxStaticText* StaticText11;
    wxCheckBox* CheckBoxWS_HOW_HIGH;
    wxCheckBox* CheckBoxTOGGLETRIM;
    wxPanel* Panel1;
    wxCheckBox* CheckBoxPPM_CENTER_ADJUSTABLE;
    wxCheckBox* CheckBoxAUDIO;
    wxStaticText* StaticText1;
    wxCheckBox* CheckBoxPPM_LIMITS_SYMETRICAL;
    wxStaticBox* StaticBox6;
    wxStaticText* StaticText3;
    wxStaticBox* StaticBox1;
    wxButton* Sortirpage2;
    wxChoice* ChoiceTHREE_POS;
    wxCheckBox* CheckBoxTURNIGY_TRANSMITTER_FIX;
    wxChoice* ChoiceNAVIGATION;
    wxButton* ButtonEXIT;
    wxPanel* Panel3;
    wxCheckBox* CheckBoxFRSKY_STICKS;
    wxStaticBox* StaticBox2;
    wxCheckBox* CheckBoxGPS;
    wxButton* ButtonCOMPILE;
    wxChoice* ChoiceTTS;
    wxCheckBox* CheckBoxVARIO;
    wxChoice* ChoicePPM_UNIT ;
    wxStaticText* StaticText5;
    wxStaticText* StaticText7;
    wxChoice* ChoiceEXT;
    wxCheckBox* CheckBoxSPLASH;
    wxStaticBox* StaticBox3;
    wxCheckBox* CheckBoxARITHMETIC_OVERFLOW_CHECK;
    wxChoice* ChoiceFONT;
    wxCheckBox* CheckBoxSPORT_FILE_LOG;
    wxCheckBox* CheckBoxHELI;
    wxStaticText* StaticText12;
    wxCheckBox* CheckBoxFLIGHT_MODES;
    wxCheckBox* CheckBoxACCURAT_THROTTLE_STATS;
    wxCheckBox* CheckBoxPWM_BACKLIGHT;
    wxPanel* Panel2;
    wxCheckBox* CheckBoxSP22;
    wxChoice* ChoiceLCD;
    wxChoice* ChoiceVOICE;
    wxStaticText* StaticText4;
    wxCheckBox* CheckBoxAUTOSWITCH;
    wxCheckBox* CheckBoxCORRECT_NEGATIVE_VALUES;
    wxCheckBox* CheckBoxDSM2PPM;
    wxCheckBox* CheckBoxBOLD;
    wxButton* Compilerpage2;
    wxCheckBox* CheckBoxRTCLOCK;
    wxChoice* ChoicePCB;
    wxCheckBox* CheckBoxHAPTIC;
    wxCheckBox* CheckBoxGVARS;
    wxChoice* ChoiceDEFAULT_MODE;
    wxCheckBox* CheckBoxAUTOSOURCE;
    wxStaticBox* StaticBox4;
    wxCheckBox* CheckBoxEEPROM_PROGRESS_BAR;
    //*)

protected:

    //(*Identifiers(CompilerOptionsFrame)
    static const long ID_STATICBOX6;
    static const long ID_STATICBOX4;
    static const long ID_CHECKBOX17;
    static const long ID_STATICBOX5;
    static const long ID_STATICTEXT4;
    static const long ID_CHECKBOX16;
    static const long ID_CHOICE4;
    static const long ID_CHOICE6;
    static const long ID_CHOICE1;
    static const long ID_STATICTEXT2;
    static const long ID_STATICTEXT1;
    static const long ID_CHOICE3;
    static const long ID_STATICTEXT3;
    static const long ID_CHECKBOX7;
    static const long ID_CHECKBOX14;
    static const long ID_CHECKBOX1;
    static const long ID_CHECKBOX15;
    static const long ID_CHECKBOX9;
    static const long ID_CHECKBOX12;
    static const long ID_CHOICE7;
    static const long ID_STATICTEXT6;
    static const long ID_CHOICE5;
    static const long ID_STATICTEXT5;
    static const long ID_CHOICE8;
    static const long ID_STATICTEXT7;
    static const long ID_BUTTON3;
    static const long ID_BUTTON2;
    static const long ID_CHOICE9;
    static const long ID_STATICTEXT8;
    static const long ID_CHOICE10;
    static const long ID_STATICTEXT9;
    static const long ID_CHECKBOX18;
    static const long ID_CHECKBOX19;
    static const long ID_CHECKBOX20;
    static const long ID_CHECKBOX21;
    static const long ID_CHOICE11;
    static const long ID_STATICTEXT10;
    static const long ID_CHECKBOX40;
    static const long ID_CHECKBOX41;
    static const long ID_CHOICE13;
    static const long ID_STATICTEXT12;
    static const long ID_CHOICE12;
    static const long ID_STATICTEXT11;
    static const long ID_PANEL2;
    static const long ID_STATICBOX3;
    static const long ID_STATICBOX2;
    static const long ID_STATICBOX1;
    static const long ID_CHECKBOX10;
    static const long ID_CHECKBOX11;
    static const long ID_CHECKBOX13;
    static const long ID_CHOICE2;
    static const long ID_CHECKBOX2;
    static const long ID_CHECKBOX3;
    static const long ID_CHECKBOX4;
    static const long ID_CHECKBOX5;
    static const long ID_CHECKBOX6;
    static const long ID_CHECKBOX8;
    static const long ID_CHECKBOX23;
    static const long ID_CHECKBOX24;
    static const long ID_CHECKBOX25;
    static const long ID_CHECKBOX26;
    static const long ID_CHECKBOX27;
    static const long ID_CHECKBOX29;
    static const long ID_CHECKBOX30;
    static const long ID_CHECKBOX31;
    static const long ID_CHECKBOX32;
    static const long ID_CHECKBOX34;
    static const long ID_CHECKBOX36;
    static const long ID_CHECKBOX37;
    static const long ID_CHECKBOX38;
    static const long ID_CHECKBOX39;
    static const long ID_CHECKBOX43;
    static const long ID_CHECKBOX44;
    static const long ID_CHECKBOX45;
    static const long ID_CHECKBOX47;
    static const long ID_CHECKBOX28;
    static const long ID_BUTTON1;
    static const long ID_BUTTON4;
    static const long ID_PANEL3;
    static const long ID_NOTEBOOK1;
    static const long ID_PANEL1;
    //*)

private:

    //(*Handlers(CompilerOptionsFrame)
    void OnCheckListBox1Toggled(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnCheckBox1Click(wxCommandEvent& event);
    void OnCheckBox2Click(wxCommandEvent& event);
    void OnCheckBox3Click(wxCommandEvent& event);
    void OnCheckBox11Click(wxCommandEvent& event);
    void OnCheckBox10Click(wxCommandEvent& event);
    void OnChoice3Select(wxCommandEvent& event);
    void OnChoice5Select(wxCommandEvent& event);
    void OnChoice4Select(wxCommandEvent& event);
    void OnButton1Click(wxCommandEvent& event);
    void OnCheckBox5Click(wxCommandEvent& event);
    void OnCheckBox8Click(wxCommandEvent& event);
    void OnCheckBox4Click(wxCommandEvent& event);
    void OnCheckBox6Click(wxCommandEvent& event);
    void OnCheckBox9Click(wxCommandEvent& event);
    void OnChoice6Select(wxCommandEvent& event);
    void OnCheckBox12Click(wxCommandEvent& event);
    void OnButton2Click(wxCommandEvent& event);
    void OnLCDSelect(wxCommandEvent& event);
    void SaveConfig();
    void LoadConfig();
    void OnVOICESelect1(wxCommandEvent& event);
    void OnChoicePCBSelect(wxCommandEvent& event);
    void OnChoiceEXTSelect(wxCommandEvent& event);
    void OnChoicePPM_UNITSelect(wxCommandEvent& event);
    void OnAUDIOClick(wxCommandEvent& event);
    void OnCheckBoxAUDIOClick(wxCommandEvent& event);
    void OnCheckBoxRTCLOCKClick(wxCommandEvent& event);
    void OnButtonCOMPILEClick(wxCommandEvent& event);
    void OnButtonEXITClick(wxCommandEvent& event);
    void OnCheckBox2Click1(wxCommandEvent& event);
    void OnCheckBox3Click1(wxCommandEvent& event);
    void OnCheckBox1Click1(wxCommandEvent& event);
    void OnCheckBoxHUBClick(wxCommandEvent& event);
    void OnChoiceTTSSelect(wxCommandEvent& event);
    void CollectDatas();
    void BatFunction();
    void WriteSplashFile();
    void CreateCompileBatFile(wxString line2);
    void OnCheckBoxSD_CARDClick(wxCommandEvent& event);
    void OnCheckBoxPPMClick(wxCommandEvent& event);
    void OnCheckBoxSPORT_FILE_LOGClick(wxCommandEvent& event);
    void OnCheckBoxPXXClick(wxCommandEvent& event);
    void OnChoiceTTSSelect1(wxCommandEvent& event);
    void OnCheckBoxDSM2Click(wxCommandEvent& event);
    void OnCheckBox1Click2(wxCommandEvent& event);
    void OnChoice1Select(wxCommandEvent& event);
    void OnCheckBoxTURNIGY_TRANSMITTER_FIXClick(wxCommandEvent& event);
    void OnCheckBoxPCBREVClick(wxCommandEvent& event);
    void OnCheckBoxPPM_LIMITS_SYMETRICALClick(wxCommandEvent& event);
    void OnChoiceEXTSelect1(wxCommandEvent& event);
    void OnCheckBoxFAS_OFFSETClick(wxCommandEvent& event);
    void OnCheckBoxGPSClick(wxCommandEvent& event);
    void OnCheckBoxVARIOClick(wxCommandEvent& event);
    void OnCheckBoxWS_HOW_HIGHClick(wxCommandEvent& event);
    void OnCheckBoxFRSKY_STICKSClick(wxCommandEvent& event);
    void OnCheckBoxCORRECT_NEGATIVE_VALUESClick(wxCommandEvent& event);
    void OnPanel3Paint(wxPaintEvent& event);
    void OnCheckBoxDOUBLEKEYSClick(wxCommandEvent& event);
    void OnCheckBoxDOUBLEKEYSClick1(wxCommandEvent& event);
    void OnCheckBox2Click2(wxCommandEvent& event);
    void OnNotebook1PageChanged(wxNotebookEvent& event);
    void OnCheckBoxHELIClick(wxCommandEvent& event);
    void OnCheckBoxSD_CARDClick1(wxCommandEvent& event);
    void OnCheckBoxHAPTICClick(wxCommandEvent& event);
    void OnPanel2Paint(wxPaintEvent& event);
    void OnChoiceTRANSLATIONSSelect(wxCommandEvent& event);
    void OnChoiceTHREE_POSSelect(wxCommandEvent& event);
    void OnCheckBoxWS_HOW_HIGHClick1(wxCommandEvent& event);
    void OnCheckBoxCORRECT_NEGATIVE_VALUESClick1(wxCommandEvent& event);
    void OnCheckBoxTX_CADDYClick(wxCommandEvent& event);
    void OnCheckBoxPCBREVClick1(wxCommandEvent& event);
    void OnCheckBoxPPM_LIMITS_SYMETRICALClick1(wxCommandEvent& event);
    void OnCheckBoxVARIOClick1(wxCommandEvent& event);
    void OnCheckBoxPPM_CENTER_ADJUSTABLEClick(wxCommandEvent& event);
    void OnCheckBoxGAUGESClick(wxCommandEvent& event);
    void OnCheckBoxNOANDSECONDEClick(wxCommandEvent& event);
    void OnCheckBoxPPMClick1(wxCommandEvent& event);
    void OnCheckBoxFAS_OFFSETClick1(wxCommandEvent& event);
    void OnCheckBoxDBLKEYSClick(wxCommandEvent& event);
    void OnCheckBoxTOGGLETRIMClick(wxCommandEvent& event);
    void OnCheckBoxFRSKY_STICKSClick1(wxCommandEvent& event);
    void OnCheckBoxAUTOSOURCEClick(wxCommandEvent& event);
    void OnCheckBoxBOLDClick(wxCommandEvent& event);
    void OnCheckBoxSPLASHClick(wxCommandEvent& event);
    void OnChoiceLCDSelect(wxCommandEvent& event);
    void OnCheckBoxPPM_CENTER_ADJUSTABLEClick1(wxCommandEvent& event);
    void OnCheckBoxDSM2Click1(wxCommandEvent& event);
    void OnCheckBoxDSM2PPMClick(wxCommandEvent& event);
    void OnCheckBoxAUTOSWITCHClick(wxCommandEvent& event);
    void OnCheckBoxSPLASHClick1(wxCommandEvent& event);
    void OnChoiceFAISelect(wxCommandEvent& event);
    void OnCheckBoxOVERRIDE_CHANNEL_FUNCTIONClick(wxCommandEvent& event);
    void OnCheckBoxAUTOSOURCEClick1(wxCommandEvent& event);
    void OnChoiceNAVIGATIONSelect(wxCommandEvent& event);
    void OnCheckBoxDSM2SERIALClick(wxCommandEvent& event);
    void OnChoiceVOICESelect(wxCommandEvent& event);
    void OnCheckBoxNOANDSECONDEClick1(wxCommandEvent& event);
    void OnChoiceEXTSelect2(wxCommandEvent& event);
    void OnCheckBoxFRSKY_HUBClick(wxCommandEvent& event);
    void OnCheckBoxFAS_OFFSETClick2(wxCommandEvent& event);
    void OnCheckBoxGAUGESClick1(wxCommandEvent& event);
    void OnCheckBoxGPSClick1(wxCommandEvent& event);
    void OnCheckBoxVARIOClick2(wxCommandEvent& event);
    void OnCheckBoxSPORT_FILE_LOGClick1(wxCommandEvent& event);
    void OnCheckBoxWS_HOW_HIGHClick2(wxCommandEvent& event);
    //*)

    DECLARE_EVENT_TABLE()
};

#endif
