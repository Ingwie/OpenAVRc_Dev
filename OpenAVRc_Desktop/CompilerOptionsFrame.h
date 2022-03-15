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


#ifndef COMPILEROPTIONSFRAME_H
#define COMPILEROPTIONSFRAME_H

#include "OpenAVRc_DesktopMain.h"

//(*Headers(CompilerOptionsFrame)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/frame.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

//SPLASH
extern uint8_t LbmSplash[SPLASHLENGHT];


extern wxString CompileBatFile;
//extern wxString LinkToWriteFirmware;

extern wxString PCB;//mandatory
extern wxString LCD;//mandatory
extern bool LCDROT180;//defaults to NO
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
extern wxString SERIAL_PROTOCOL;//defaults to NO
extern bool CC2500;//defaults to NO
extern bool CYRF6936;//defaults to NO
extern bool NRF24l01;//defaults to NO
extern bool A7105;//defaults to NO
extern bool SX1276;//defaults to NO
extern wxString CC2500PAG;//defaults to 0
extern wxString CYRF6936PAG;//defaults to 0
extern wxString NRF24l01PAG;//defaults to 0
extern wxString A7105PAG;//defaults to 0

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
extern bool CORRECT_NEGATIVE_VALUES;
extern bool ARITHMETIC_OVERFLOW_CHECK;
extern bool ACCURAT_THROTTLE_STATS;
extern bool PWM_BACKLIGHT;
extern bool OVERRIDE_CHANNEL_FUNCTION;
extern bool WS_HOW_HIGH;
extern bool TOGGLETRIM;
extern bool NOANDSECONDE;// Hardwired
extern bool SHUTDOWN_CONFIRMATION;
extern bool FRAM;
extern wxString BLUETOOTH;
extern bool XMODEM;
extern bool PERSONAMES;
extern bool DBLGAZSTICK;
extern wxString OTHERCOMPOPTIONS;

// Sticks
extern bool INV_STICK_RH;
extern bool INV_STICK_LV;
extern bool INV_STICK_RV;
extern bool INV_STICK_LH;

extern wxString switch1;
extern wxString switch2;
extern wxString switch3;
extern wxString switch4;
extern wxString switch5;
extern wxString switch6;
extern wxString switchID0;
extern wxString switchID1;
extern wxString switchID2;
extern wxString encoderA;
extern wxString encoderB;
extern wxString encoderNo;
extern wxString switchXD0;
extern wxString switchXD1;
extern wxString switchXD2;
extern wxString stickRud;
extern wxString stickEle;
extern wxString stickThr;
extern wxString stickAil;

//X ANY
extern wxString NUMXANY;


class CompilerOptionsFrame: public wxFrame
{
public:

    CompilerOptionsFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
    virtual ~CompilerOptionsFrame();

    //wxFileConfig* configFile;

    //(*Declarations(CompilerOptionsFrame)
    wxButton* Button1;
    wxButton* Button2;
    wxButton* ButtonCOMPILE;
    wxButton* ButtonDefaut;
    wxButton* ButtonEXIT;
    wxButton* Compilerpage2;
    wxButton* Compilerpage3;
    wxButton* Sortirpage2;
    wxButton* Sortirpage3;
    wxCheckBox* CheckBoxA7105;
    wxCheckBox* CheckBoxACCURAT_THROTTLE_STATS;
    wxCheckBox* CheckBoxARITHMETIC_OVERFLOW_CHECK;
    wxCheckBox* CheckBoxAUDIO;
    wxCheckBox* CheckBoxAUTOSOURCE;
    wxCheckBox* CheckBoxAUTOSWITCH;
    wxCheckBox* CheckBoxBATTGRAPH;
    wxCheckBox* CheckBoxBOLD;
    wxCheckBox* CheckBoxCC2500;
    wxCheckBox* CheckBoxCORRECT_NEGATIVE_VALUES;
    wxCheckBox* CheckBoxCURVES;
    wxCheckBox* CheckBoxCYRF6936;
    wxCheckBox* CheckBoxDBLGAZSTICK;
    wxCheckBox* CheckBoxDBLKEYS;
    wxCheckBox* CheckBoxEEPROM_PROGRESS_BAR;
    wxCheckBox* CheckBoxFLIGHT_MODES;
    wxCheckBox* CheckBoxFRAM;
    wxCheckBox* CheckBoxGPS;
    wxCheckBox* CheckBoxGVARS;
    wxCheckBox* CheckBoxHAPTIC;
    wxCheckBox* CheckBoxHELI;
    wxCheckBox* CheckBoxInvStickLH;
    wxCheckBox* CheckBoxInvStickLV;
    wxCheckBox* CheckBoxInvStickRH;
    wxCheckBox* CheckBoxInvStickRV;
    wxCheckBox* CheckBoxLCDROT180;
    wxCheckBox* CheckBoxNOANDSECONDE;
    wxCheckBox* CheckBoxNRF24l01;
    wxCheckBox* CheckBoxOVERRIDE_CHANNEL_FUNCTION;
    wxCheckBox* CheckBoxPERSONAMES;
    wxCheckBox* CheckBoxPPM;
    wxCheckBox* CheckBoxPPM_CENTER_ADJUSTABLE;
    wxCheckBox* CheckBoxPPM_LIMITS_SYMETRICAL;
    wxCheckBox* CheckBoxPWM_BACKLIGHT;
    wxCheckBox* CheckBoxSD_CARD;
    wxCheckBox* CheckBoxSHUTDOWN_CONFIRMATION;
    wxCheckBox* CheckBoxSPLASH;
    wxCheckBox* CheckBoxSX1276;
    wxCheckBox* CheckBoxTEMPLATES;
    wxCheckBox* CheckBoxTOGGLETRIM;
    wxCheckBox* CheckBoxVARIO;
    wxCheckBox* CheckBoxWS_HOW_HIGH;
    wxCheckBox* CheckBoxXMODEM;
    wxChoice* ChoiceA7105PAG;
    wxChoice* ChoiceBLUETOOTH;
    wxChoice* ChoiceCC2500PAG;
    wxChoice* ChoiceCYRF6936PAG;
    wxChoice* ChoiceDEFAULT_MODE;
    wxChoice* ChoiceEXT;
    wxChoice* ChoiceFAI;
    wxChoice* ChoiceFONT;
    wxChoice* ChoiceLCD;
    wxChoice* ChoiceNAVIGATION;
    wxChoice* ChoiceNRF24l01PAG;
    wxChoice* ChoiceNUMXANY;
    wxChoice* ChoicePCB;
    wxChoice* ChoicePPM_UNIT ;
    wxChoice* ChoiceSERIAL_PROTOCOL;
    wxChoice* ChoiceTHREE_POS;
    wxChoice* ChoiceTRANSLATIONS;
    wxChoice* ChoiceTTS;
    wxChoice* ChoiceUNITS;
    wxChoice* ChoiceVOICE;
    wxComboBox* ComboBoxAil;
    wxComboBox* ComboBoxEle;
    wxComboBox* ComboBoxEncodeurA;
    wxComboBox* ComboBoxEncodeurB;
    wxComboBox* ComboBoxEncodeurNo;
    wxComboBox* ComboBoxID0;
    wxComboBox* ComboBoxID1;
    wxComboBox* ComboBoxID2;
    wxComboBox* ComboBoxRud;
    wxComboBox* ComboBoxThr;
    wxComboBox* ComboBoxXD0;
    wxComboBox* ComboBoxXD1;
    wxComboBox* ComboBoxXD2;
    wxComboBox* ComboBoxswitch1;
    wxComboBox* ComboBoxswitch2;
    wxComboBox* ComboBoxswitch3;
    wxComboBox* ComboBoxswitch4;
    wxComboBox* ComboBoxswitch5;
    wxComboBox* ComboBoxswitch6;
    wxNotebook* Notebook1;
    wxPanel* Panel1;
    wxPanel* Panel2;
    wxPanel* Panel3;
    wxPanel* Panel4;
    wxPanel* Panel5;
    wxStaticBox* StaticBox10;
    wxStaticBox* StaticBox11;
    wxStaticBox* StaticBox12;
    wxStaticBox* StaticBox13;
    wxStaticBox* StaticBox14;
    wxStaticBox* StaticBox1;
    wxStaticBox* StaticBox2;
    wxStaticBox* StaticBox3;
    wxStaticBox* StaticBox4;
    wxStaticBox* StaticBox5;
    wxStaticBox* StaticBox6;
    wxStaticBox* StaticBox7;
    wxStaticBox* StaticBox8;
    wxStaticBox* StaticBox9;
    wxStaticText* StaticText10;
    wxStaticText* StaticText11;
    wxStaticText* StaticText12;
    wxStaticText* StaticText13;
    wxStaticText* StaticText14;
    wxStaticText* StaticText15;
    wxStaticText* StaticText16;
    wxStaticText* StaticText17;
    wxStaticText* StaticText18;
    wxStaticText* StaticText19;
    wxStaticText* StaticText1;
    wxStaticText* StaticText20;
    wxStaticText* StaticText21;
    wxStaticText* StaticText22;
    wxStaticText* StaticText23;
    wxStaticText* StaticText24;
    wxStaticText* StaticText25;
    wxStaticText* StaticText26;
    wxStaticText* StaticText27;
    wxStaticText* StaticText28;
    wxStaticText* StaticText29;
    wxStaticText* StaticText2;
    wxStaticText* StaticText30;
    wxStaticText* StaticText31;
    wxStaticText* StaticText32;
    wxStaticText* StaticText33;
    wxStaticText* StaticText34;
    wxStaticText* StaticText35;
    wxStaticText* StaticText3;
    wxStaticText* StaticText4;
    wxStaticText* StaticText5;
    wxStaticText* StaticText6;
    wxStaticText* StaticText7;
    wxStaticText* StaticText8;
    wxStaticText* StaticText9;
    wxTextCtrl* TextCtrlOTHERCOMPOPTIONS;
    //*)

protected:

    //(*Identifiers(CompilerOptionsFrame)
    static const long ID_STATICBOX13;
    static const long ID_CHECKBOX3;
    static const long ID_CHECKBOX8;
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
    static const long ID_CHECKBOX4;
    static const long ID_CHECKBOX2;
    static const long ID_CHECKBOX9;
    static const long ID_PANEL2;
    static const long ID_STATICBOX3;
    static const long ID_STATICBOX1;
    static const long ID_STATICBOX9;
    static const long ID_CHOICECC2500PAG;
    static const long ID_CHOICEA7105PAG;
    static const long ID_CHOICENRF24l01PAG;
    static const long ID_CHECKBOXNOANDSECONDE;
    static const long ID_STATICBOX2;
    static const long ID_CHECKBOX10;
    static const long ID_CHOICE2;
    static const long ID_CHECKBOX5;
    static const long ID_CHECKBOX6;
    static const long ID_CHECKBOX26;
    static const long ID_CHECKBOX27;
    static const long ID_CHECKBOX30;
    static const long ID_CHECKBOX31;
    static const long ID_CHECKBOX32;
    static const long ID_CHECKBOX36;
    static const long ID_CHECKBOX37;
    static const long ID_CHECKBOX38;
    static const long ID_CHECKBOX39;
    static const long ID_CHECKBOX43;
    static const long ID_CHECKBOX44;
    static const long ID_CHECKBOX45;
    static const long ID_CHECKBOX47;
    static const long ID_BUTTON1;
    static const long ID_BUTTON4;
    static const long ID_CHECKBOX23;
    static const long ID_CHECKBOX24;
    static const long ID_CHECKBOX25;
    static const long ID_CHECKBOX35;
    static const long ID_CHECKBOX29;
    static const long ID_CHOICECYRF6936PAG;
    static const long ID_STATICTEXT28;
    static const long ID_CHECKBOX34;
    static const long ID_CHOICEBLUETOOTH;
    static const long ID_STATICTEXT30;
    static const long ID_PROTOSERIALCHOICE;
    static const long ID_STATICTEXT31;
    static const long ID_PANEL3;
    static const long ID_STATICBOX7;
    static const long ID_STATICBOX14;
    static const long ID_STATICBOX8;
    static const long ID_CHECKBOX11;
    static const long ID_COMBOBOX18;
    static const long ID_STATICTEXT35;
    static const long ID_COMBOBOX16;
    static const long ID_STATICTEXT33;
    static const long ID_STATICTEXT34;
    static const long ID_STATICBOX10;
    static const long ID_COMBOBOX12;
    static const long ID_COMBOBOX1;
    static const long ID_BUTTON5;
    static const long ID_BUTTON6;
    static const long ID_COMBOBOX2;
    static const long ID_COMBOBOX3;
    static const long ID_COMBOBOX4;
    static const long ID_COMBOBOX5;
    static const long ID_COMBOBOX6;
    static const long ID_CHECKBOX22;
    static const long ID_STATICTEXT13;
    static const long ID_STATICTEXT14;
    static const long ID_STATICTEXT15;
    static const long ID_STATICTEXT16;
    static const long ID_STATICTEXT17;
    static const long ID_STATICTEXT18;
    static const long ID_BUTTON7;
    static const long ID_STATICTEXT25;
    static const long ID_STATICTEXT19;
    static const long ID_COMBOBOX17;
    static const long ID_COMBOBOX19;
    static const long ID_COMBOBOX20;
    static const long ID_COMBOBOX15;
    static const long ID_COMBOBOX13;
    static const long ID_COMBOBOX7;
    static const long ID_STATICTEXT20;
    static const long ID_STATICTEXT24;
    static const long ID_STATICTEXT21;
    static const long ID_COMBOBOX8;
    static const long ID_COMBOBOX9;
    static const long ID_COMBOBOX10;
    static const long ID_COMBOBOX11;
    static const long ID_STATICTEXT26;
    static const long ID_STATICTEXT22;
    static const long ID_STATICTEXT27;
    static const long ID_STATICTEXT23;
    static const long ID_STATICBOX12;
    static const long ID_TEXTCTRL1;
    static const long ID_STATICTEXT32;
    static const long ID_PANEL4;
    static const long ID_STATICBOX11;
    static const long ID_BUTTON8;
    static const long ID_BUTTON9;
    static const long ID_CHOICENUMXANY;
    static const long ID_STATICTEXT29;
    static const long ID_PANEL5;
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
    void CreateCompileBatFile(wxString line4);
    void CreatePersonames_H();
    void OnCheckBoxPPMClick(wxCommandEvent& event);
    void OnCheckBoxSPORT_FILE_LOGClick(wxCommandEvent& event);
    void OnChoiceTTSSelect1(wxCommandEvent& event);
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
    void OnCheckBoxAUTOSWITCHClick(wxCommandEvent& event);
    void OnCheckBoxSPLASHClick1(wxCommandEvent& event);
    void OnChoiceFAISelect(wxCommandEvent& event);
    void OnCheckBoxOVERRIDE_CHANNEL_FUNCTIONClick(wxCommandEvent& event);
    void OnCheckBoxAUTOSOURCEClick1(wxCommandEvent& event);
    void OnChoiceNAVIGATIONSelect(wxCommandEvent& event);
    void OnChoiceVOICESelect(wxCommandEvent& event);
    void OnCheckBoxNOANDSECONDEClick1(wxCommandEvent& event);
    void OnChoiceEXTSelect2(wxCommandEvent& event);
    void OnCheckBoxGPSClick1(wxCommandEvent& event);
    void OnCheckBoxVARIOClick2(wxCommandEvent& event);
    void OnCheckBoxWS_HOW_HIGHClick2(wxCommandEvent& event);
    void OnSortirpage3Click(wxCommandEvent& event);
    void Onswitch3Selected(wxCommandEvent& event);
    void Onswitch2Selected(wxCommandEvent& event);
    void OnComboBoxswitch1Selected(wxCommandEvent& event);
    void OnCheckBox1Click3(wxCommandEvent& event);
    void OnPanel4Paint(wxPaintEvent& event);
    void OnCheckBoxPERSONAMESClick(wxCommandEvent& event);
    void OnComboBox3Selected(wxCommandEvent& event);
    void OnButton1Click1(wxCommandEvent& event);
    void OnButtonDefautClick(wxCommandEvent& event);
    void OnButtonDefautClick1(wxCommandEvent& event);
    void OnComboBoxEncodeurASelected(wxCommandEvent& event);
    void OnComboBoxEncodeurBSelected(wxCommandEvent& event);
    void OnCheckBoxSD_CARDClick(wxCommandEvent& event);
    void OnCheckBoxXMODEMClick(wxCommandEvent& event);
    //*)

    DECLARE_EVENT_TABLE()
};

#endif
