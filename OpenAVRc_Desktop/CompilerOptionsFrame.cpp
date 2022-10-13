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


#include "CompilerOptionsFrame.h"
#include "OpenAVRc_DesktopMain.h"
#include "CommunicationsFrame.h"
#include <wx/msgdlg.h>
#include <wx/filedlg.h>


extern wxString AppPath;
extern bool Ini_Changed;
bool doNotClose = 0;


//(*InternalHeaders(CompilerOptionsFrame)
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)


//(*IdInit(CompilerOptionsFrame)
const long CompilerOptionsFrame::ID_STATICBOX13 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX3 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX8 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX6 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX4 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX17 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX5 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT4 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX16 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE4 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE6 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE1 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT2 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT1 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE3 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT3 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX7 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX14 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX1 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX15 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX12 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE7 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT6 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE5 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT5 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE8 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT7 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON3 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON2 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE9 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT8 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE10 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT9 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX18 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX19 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX20 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX21 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE11 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT10 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX40 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX41 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE13 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT12 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE12 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT11 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX4 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX2 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX9 = wxNewId();
const long CompilerOptionsFrame::ID_PANEL2 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX3 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX1 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX9 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICECC2500PAG = wxNewId();
const long CompilerOptionsFrame::ID_CHOICEA7105PAG = wxNewId();
const long CompilerOptionsFrame::ID_CHOICENRF24l01PAG = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOXNOANDSECONDE = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX2 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX10 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE2 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX5 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX6 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX26 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX27 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX30 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX31 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX32 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX36 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX37 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX38 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX39 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX43 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX44 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX45 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX47 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON1 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON4 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX23 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX24 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX25 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX35 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX29 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICECYRF6936PAG = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT28 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX34 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICEBLUETOOTH = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT30 = wxNewId();
const long CompilerOptionsFrame::ID_PROTOPCMCHOICE = wxNewId();
const long CompilerOptionsFrame::ID_PROTOSERIALCHOICE = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT200 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT31 = wxNewId();
const long CompilerOptionsFrame::ID_PANEL3 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX7 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX14 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX11 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX18 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT35 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX16 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT33 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT34 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX10 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX12 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX1 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON5 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON6 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX2 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX3 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX4 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX5 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX6 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX22 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT13 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT14 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT15 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT16 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT17 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT18 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON7 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT25 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT19 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX17 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX19 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX20 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX15 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX13 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX7 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT20 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT24 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT21 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX8 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX9 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX10 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX11 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT26 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT22 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT27 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT23 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX12 = wxNewId();
const long CompilerOptionsFrame::ID_TEXTCTRL1 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT32 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX8 = wxNewId();
const long CompilerOptionsFrame::ID_PANEL4 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX11 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON8 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON9 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICENUMXANY = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT29 = wxNewId();
const long CompilerOptionsFrame::ID_PANEL5 = wxNewId();
const long CompilerOptionsFrame::ID_NOTEBOOK1 = wxNewId();
const long CompilerOptionsFrame::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(CompilerOptionsFrame,wxFrame)
  //(*EventTable(CompilerOptionsFrame)
  //*)
END_EVENT_TABLE()

CompilerOptionsFrame::CompilerOptionsFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
  //(*Initialize(CompilerOptionsFrame)
  Create(parent, wxID_ANY, _("Compil-O-matic"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
  SetClientSize(wxSize(790,367));
  Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(0,0), wxSize(830,368), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
  Panel1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
  Notebook1 = new wxNotebook(Panel1, ID_NOTEBOOK1, wxPoint(0,0), wxSize(792,368), 0, _T("ID_NOTEBOOK1"));
  Panel2 = new wxPanel(Notebook1, ID_PANEL2, wxDefaultPosition, wxSize(788,342), wxTAB_TRAVERSAL, _T("ID_PANEL2"));
  StaticBox13 = new wxStaticBox(Panel2, ID_STATICBOX13, _("Manches"), wxPoint(648,8), wxSize(128,128), 0, _T("ID_STATICBOX13"));
  CheckBoxInvStickLH = new wxCheckBox(Panel2, ID_CHECKBOX3, _("Inversion Stick_LH"), wxPoint(656,32), wxSize(112,24), 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
  CheckBoxInvStickLH->SetValue(false);
  CheckBoxInvStickRH = new wxCheckBox(Panel2, ID_CHECKBOX8, _("Inversion Stick_RH"), wxPoint(656,104), wxSize(112,24), 0, wxDefaultValidator, _T("ID_CHECKBOX8"));
  CheckBoxInvStickRH->SetValue(false);
  StaticBox6 = new wxStaticBox(Panel2, ID_STATICBOX6, _("Autres options"), wxPoint(448,8), wxSize(192,328), 0, _T("ID_STATICBOX6"));
  StaticBox4 = new wxStaticBox(Panel2, ID_STATICBOX4, _("Matériel"), wxPoint(8,8), wxSize(200,328), 0, _T("ID_STATICBOX4"));
  CheckBoxPPM_LIMITS_SYMETRICAL = new wxCheckBox(Panel2, ID_CHECKBOX17, _("PPM Simm limits"), wxPoint(456,296), wxSize(176,24), 0, wxDefaultValidator, _T("ID_CHECKBOX17"));
  CheckBoxPPM_LIMITS_SYMETRICAL->SetValue(false);
  CheckBoxPPM_LIMITS_SYMETRICAL->SetToolTip(_("Option ajuster le gain fonction du subtrim"));
  StaticBox5 = new wxStaticBox(Panel2, ID_STATICBOX5, _("Interface"), wxPoint(216,8), wxSize(224,328), 0, _T("ID_STATICBOX5"));
  StaticText4 = new wxStaticText(Panel2, ID_STATICTEXT4, _("Unités"), wxPoint(224,304), wxSize(87,16), wxALIGN_RIGHT, _T("ID_STATICTEXT4"));
  CheckBoxPPM_CENTER_ADJUSTABLE = new wxCheckBox(Panel2, ID_CHECKBOX16, _("PPM Center adjust"), wxPoint(456,272), wxSize(176,24), 0, wxDefaultValidator, _T("ID_CHECKBOX16"));
  CheckBoxPPM_CENTER_ADJUSTABLE->SetValue(false);
  CheckBoxPPM_CENTER_ADJUSTABLE->SetToolTip(_("Option réglage des neutres indépendant"));
  ChoicePPM_UNIT  = new wxChoice(Panel2, ID_CHOICE4, wxPoint(320,296), wxSize(112,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE4"));
  ChoicePPM_UNIT ->SetToolTip(_("Affichage des valeurs des voies en % ou uS"));
  ChoicePCB = new wxChoice(Panel2, ID_CHOICE6, wxPoint(96,32), wxSize(96,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE6"));
  ChoicePCB->SetSelection( ChoicePCB->Append(wxEmptyString) );
  ChoicePCB->SetToolTip(_("Type de radio"));
  ChoiceLCD = new wxChoice(Panel2, ID_CHOICE1, wxPoint(96,72), wxSize(96,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
  ChoiceLCD->SetToolTip(_("Référence de l\'écran"));
  StaticText2 = new wxStaticText(Panel2, ID_STATICTEXT2, _("Processeur"), wxPoint(16,40), wxSize(72,16), wxALIGN_RIGHT, _T("ID_STATICTEXT2"));
  StaticText1 = new wxStaticText(Panel2, ID_STATICTEXT1, _("LCD"), wxPoint(16,80), wxSize(72,16), wxALIGN_RIGHT, _T("ID_STATICTEXT1"));
  ChoiceVOICE = new wxChoice(Panel2, ID_CHOICE3, wxPoint(96,144), wxSize(96,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
  ChoiceVOICE->SetToolTip(_("Option annonces vocales"));
  StaticText3 = new wxStaticText(Panel2, ID_STATICTEXT3, _("Voice"), wxPoint(16,144), wxSize(72,16), wxALIGN_RIGHT, _T("ID_STATICTEXT3"));
  CheckBoxAUDIO = new wxCheckBox(Panel2, ID_CHECKBOX7, _("Audio"), wxPoint(96,176), wxSize(104,24), 0, wxDefaultValidator, _T("ID_CHECKBOX7"));
  CheckBoxAUDIO->SetValue(false);
  CheckBoxAUDIO->SetToolTip(_("Option audio"));
  CheckBoxHAPTIC = new wxCheckBox(Panel2, ID_CHECKBOX14, _("Haptic"), wxPoint(96,216), wxSize(104,24), 0, wxDefaultValidator, _T("ID_CHECKBOX14"));
  CheckBoxHAPTIC->SetValue(false);
  CheckBoxHAPTIC->SetToolTip(_("Option Vibreur"));
  CheckBoxHELI = new wxCheckBox(Panel2, ID_CHECKBOX1, _("HELI"), wxPoint(456,152), wxSize(176,24), 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
  CheckBoxHELI->SetValue(false);
  CheckBoxHELI->SetToolTip(_("Option hélicoptère"));
  CheckBoxTEMPLATES = new wxCheckBox(Panel2, ID_CHECKBOX15, _("TEMPLATES"), wxPoint(456,176), wxSize(176,24), 0, wxDefaultValidator, _T("ID_CHECKBOX15"));
  CheckBoxTEMPLATES->SetValue(false);
  CheckBoxTEMPLATES->SetToolTip(_("Option modèles pré programmés"));
  CheckBoxSD_CARD = new wxCheckBox(Panel2, ID_CHECKBOX12, _("Carte SD"), wxPoint(96,256), wxSize(104,24), 0, wxDefaultValidator, _T("ID_CHECKBOX12"));
  CheckBoxSD_CARD->SetValue(false);
  CheckBoxSD_CARD->SetToolTip(_("Option carte SD (Sauvegarde - Fichiers log)"));
  ChoiceTRANSLATIONS = new wxChoice(Panel2, ID_CHOICE7, wxPoint(320,32), wxSize(40,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE7"));
  ChoiceTRANSLATIONS->SetToolTip(_("Langue affichée"));
  StaticText6 = new wxStaticText(Panel2, ID_STATICTEXT6, _("Langue"), wxPoint(224,40), wxSize(88,16), wxALIGN_RIGHT, _T("ID_STATICTEXT6"));
  ChoiceTTS = new wxChoice(Panel2, ID_CHOICE5, wxPoint(320,72), wxSize(40,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE5"));
  ChoiceTTS->SetToolTip(_("Langue de la synthése vocale"));
  StaticText5 = new wxStaticText(Panel2, ID_STATICTEXT5, _("Langue voice"), wxPoint(224,80), wxSize(88,16), wxALIGN_RIGHT, _T("ID_STATICTEXT5"));
  ChoiceNAVIGATION = new wxChoice(Panel2, ID_CHOICE8, wxPoint(320,256), wxSize(112,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE8"));
  ChoiceNAVIGATION->SetToolTip(_("Peut remplacer les boutons de navigation"));
  StaticText7 = new wxStaticText(Panel2, ID_STATICTEXT7, _("Navigation"), wxPoint(224,264), wxSize(88,16), wxALIGN_RIGHT, _T("ID_STATICTEXT7"));
  ButtonEXIT = new wxButton(Panel2, ID_BUTTON3, _("Sauvegarder et sortir"), wxPoint(648,296), wxSize(128,24), 0, wxDefaultValidator, _T("ID_BUTTON3"));
  ButtonCOMPILE = new wxButton(Panel2, ID_BUTTON2, _("Compiler"), wxPoint(648,256), wxSize(128,24), 0, wxDefaultValidator, _T("ID_BUTTON2"));
  ChoiceUNITS = new wxChoice(Panel2, ID_CHOICE9, wxPoint(512,32), wxSize(80,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE9"));
  ChoiceUNITS->SetToolTip(_("Unitées"));
  StaticText8 = new wxStaticText(Panel2, ID_STATICTEXT8, _("Unités"), wxPoint(456,40), wxSize(47,16), wxALIGN_RIGHT, _T("ID_STATICTEXT8"));
  ChoiceDEFAULT_MODE = new wxChoice(Panel2, ID_CHOICE10, wxPoint(512,72), wxSize(80,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE10"));
  ChoiceDEFAULT_MODE->SetToolTip(_("Mode de la radio"));
  StaticText9 = new wxStaticText(Panel2, ID_STATICTEXT9, _("Mode"), wxPoint(456,80), wxSize(48,16), wxALIGN_RIGHT, _T("ID_STATICTEXT9"));
  CheckBoxFLIGHT_MODES = new wxCheckBox(Panel2, ID_CHECKBOX18, _("Flight modes"), wxPoint(456,200), wxSize(176,24), 0, wxDefaultValidator, _T("ID_CHECKBOX18"));
  CheckBoxFLIGHT_MODES->SetValue(false);
  CheckBoxFLIGHT_MODES->SetToolTip(_("Option phases de vol"));
  CheckBoxCURVES = new wxCheckBox(Panel2, ID_CHECKBOX19, _("Curves"), wxPoint(456,248), wxSize(176,24), 0, wxDefaultValidator, _T("ID_CHECKBOX19"));
  CheckBoxCURVES->SetValue(false);
  CheckBoxCURVES->SetToolTip(_("Option courbes"));
  CheckBoxGVARS = new wxCheckBox(Panel2, ID_CHECKBOX20, _("Global vars"), wxPoint(456,224), wxSize(176,24), 0, wxDefaultValidator, _T("ID_CHECKBOX20"));
  CheckBoxGVARS->SetValue(false);
  CheckBoxGVARS->SetToolTip(_("Option variables globales"));
  CheckBoxSPLASH = new wxCheckBox(Panel2, ID_CHECKBOX21, _("Écran d\'accueil"), wxPoint(320,176), wxSize(112,24), 0, wxDefaultValidator, _T("ID_CHECKBOX21"));
  CheckBoxSPLASH->SetValue(false);
  CheckBoxSPLASH->SetToolTip(_("Ecran d\'accueil"));
  ChoiceFONT = new wxChoice(Panel2, ID_CHOICE11, wxPoint(320,216), wxSize(112,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE11"));
  ChoiceFONT->SetToolTip(_("Police de caractére"));
  StaticText10 = new wxStaticText(Panel2, ID_STATICTEXT10, _("Font"), wxPoint(224,224), wxSize(88,16), wxALIGN_RIGHT, _T("ID_STATICTEXT10"));
  CheckBoxBOLD = new wxCheckBox(Panel2, ID_CHECKBOX40, _("Bold"), wxPoint(320,112), wxSize(112,24), 0, wxDefaultValidator, _T("ID_CHECKBOX40"));
  CheckBoxBOLD->SetValue(false);
  CheckBoxBOLD->SetToolTip(_("Vrai police en gras"));
  CheckBoxBATTGRAPH = new wxCheckBox(Panel2, ID_CHECKBOX41, _("Batt graph"), wxPoint(320,144), wxSize(112,24), 0, wxDefaultValidator, _T("ID_CHECKBOX41"));
  CheckBoxBATTGRAPH->SetValue(false);
  CheckBoxBATTGRAPH->SetToolTip(_("Affichage graphique de l\'état de la batterie"));
  ChoiceFAI = new wxChoice(Panel2, ID_CHOICE13, wxPoint(512,112), wxSize(80,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE13"));
  ChoiceFAI->SetToolTip(_("Pour les compétitions"));
  StaticText12 = new wxStaticText(Panel2, ID_STATICTEXT12, _("FAI"), wxPoint(456,120), wxSize(48,16), wxALIGN_RIGHT, _T("ID_STATICTEXT12"));
  ChoiceTHREE_POS = new wxChoice(Panel2, ID_CHOICE12, wxPoint(96,296), wxSize(96,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE12"));
  ChoiceTHREE_POS->SetToolTip(_("Selection du potentiomètre utilisé pour XD0"));
  StaticText11 = new wxStaticText(Panel2, ID_STATICTEXT11, _("EXTRA 3POS"), wxPoint(16,304), wxSize(72,16), wxALIGN_RIGHT, _T("ID_STATICTEXT11"));
  CheckBoxInvStickLV = new wxCheckBox(Panel2, ID_CHECKBOX4, _("Inversion Stick_LV"), wxPoint(656,80), wxSize(112,24), 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
  CheckBoxInvStickLV->SetValue(false);
  CheckBoxInvStickRV = new wxCheckBox(Panel2, ID_CHECKBOX2, _("Inversion Stick_RV"), wxPoint(656,56), wxSize(112,24), 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
  CheckBoxInvStickRV->SetValue(false);
  CheckBoxLCDROT180 = new wxCheckBox(Panel2, ID_CHECKBOX9, _("Rotation Lcd 180°"), wxPoint(96,104), wxSize(104,24), 0, wxDefaultValidator, _T("ID_CHECKBOX9"));
  CheckBoxLCDROT180->SetValue(false);
  Panel3 = new wxPanel(Notebook1, ID_PANEL3, wxDefaultPosition, wxSize(800,364), wxTAB_TRAVERSAL, _T("ID_PANEL3"));
  StaticBox3 = new wxStaticBox(Panel3, ID_STATICBOX3, _("Autres paramètres"), wxPoint(304,8), wxSize(472,256), 0, _T("ID_STATICBOX3"));
  StaticBox1 = new wxStaticBox(Panel3, ID_STATICBOX1, _("Protocole"), wxPoint(8,8), wxSize(152,328), 0, _T("ID_STATICBOX1"));
  StaticBox9 = new wxStaticBox(Panel3, ID_STATICBOX9, _("SPI Xmitters"), wxPoint(16,152), wxSize(136,176), 0, _T("ID_STATICBOX9"));
  ChoiceCC2500PAG = new wxChoice(Panel3, ID_CHOICECC2500PAG, wxPoint(96,200), wxSize(45,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICECC2500PAG"));
  ChoiceA7105PAG = new wxChoice(Panel3, ID_CHOICEA7105PAG, wxPoint(96,272), wxSize(45,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICEA7105PAG"));
  ChoiceNRF24l01PAG = new wxChoice(Panel3, ID_CHOICENRF24l01PAG, wxPoint(96,248), wxSize(45,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICENRF24l01PAG"));
  CheckBoxNOANDSECONDE = new wxCheckBox(Panel3, ID_CHECKBOXNOANDSECONDE, _("NOANDSECONDE"), wxPoint(312,160), wxSize(144,24), 0, wxDefaultValidator, _T("ID_CHECKBOXNOANDSECONDE"));
  CheckBoxNOANDSECONDE->SetValue(false);
  CheckBoxNOANDSECONDE->SetToolTip(_("Empèche la synthèse vocale de dire \"et\". Exemple : entre minutes \"et\" seconde"));
  StaticBox2 = new wxStaticBox(Panel3, ID_STATICBOX2, _("Télemetrie"), wxPoint(168,8), wxSize(128,328), 0, _T("ID_STATICBOX2"));
  CheckBoxPPM = new wxCheckBox(Panel3, ID_CHECKBOX10, _("PPM"), wxPoint(48,32), wxSize(104,24), 0, wxDefaultValidator, _T("ID_CHECKBOX10"));
  CheckBoxPPM->SetValue(true);
  CheckBoxPPM->Disable();
  CheckBoxPPM->SetToolTip(_("Protocole PPM 8 à 16 voies"));
  ChoiceEXT = new wxChoice(Panel3, ID_CHOICE2, wxPoint(184,32), wxSize(96,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
  ChoiceEXT->SetToolTip(_("Télémétrie installée"));
  CheckBoxGPS = new wxCheckBox(Panel3, ID_CHECKBOX5, _("GPS"), wxPoint(184,96), wxSize(96,24), 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
  CheckBoxGPS->SetValue(false);
  CheckBoxVARIO = new wxCheckBox(Panel3, ID_CHECKBOX6, _("Vario"), wxPoint(184,128), wxSize(96,24), 0, wxDefaultValidator, _T("ID_CHECKBOX6"));
  CheckBoxVARIO->SetValue(false);
  CheckBoxCORRECT_NEGATIVE_VALUES = new wxCheckBox(Panel3, ID_CHECKBOX26, _("CORRECT_NEGATIVE_VALUES"), wxPoint(480,128), wxSize(200,24), 0, wxDefaultValidator, _T("ID_CHECKBOX26"));
  CheckBoxCORRECT_NEGATIVE_VALUES->SetValue(false);
  CheckBoxCORRECT_NEGATIVE_VALUES->SetToolTip(_("Symétrise les valeurs affichées"));
  CheckBoxARITHMETIC_OVERFLOW_CHECK = new wxCheckBox(Panel3, ID_CHECKBOX27, _("ARITHMETIC_OVERFLOW_CHECK"), wxPoint(480,160), wxSize(200,24), 0, wxDefaultValidator, _T("ID_CHECKBOX27"));
  CheckBoxARITHMETIC_OVERFLOW_CHECK->SetValue(false);
  CheckBoxARITHMETIC_OVERFLOW_CHECK->SetToolTip(_("Ajoute du code pour prèvoir débordement aritmethique"));
  CheckBoxPWM_BACKLIGHT = new wxCheckBox(Panel3, ID_CHECKBOX30, _("PWM_BACKLIGHT"), wxPoint(480,32), wxSize(200,24), 0, wxDefaultValidator, _T("ID_CHECKBOX30"));
  CheckBoxPWM_BACKLIGHT->SetValue(false);
  CheckBoxPWM_BACKLIGHT->Disable();
  CheckBoxOVERRIDE_CHANNEL_FUNCTION = new wxCheckBox(Panel3, ID_CHECKBOX31, _("Override channel function"), wxPoint(480,64), wxSize(200,24), 0, wxDefaultValidator, _T("ID_CHECKBOX31"));
  CheckBoxOVERRIDE_CHANNEL_FUNCTION->SetValue(false);
  CheckBoxOVERRIDE_CHANNEL_FUNCTION->SetToolTip(_("Ajoute la fonction spéciale \"Remplacer CHx\""));
  CheckBoxWS_HOW_HIGH = new wxCheckBox(Panel3, ID_CHECKBOX32, _("WS_HOW_HIGH"), wxPoint(184,64), wxSize(104,24), 0, wxDefaultValidator, _T("ID_CHECKBOX32"));
  CheckBoxWS_HOW_HIGH->SetValue(false);
  CheckBoxWS_HOW_HIGH->SetToolTip(_("WS HowHigh Altimeter"));
  CheckBoxTOGGLETRIM = new wxCheckBox(Panel3, ID_CHECKBOX36, _("Toggle trims"), wxPoint(480,96), wxSize(200,24), 0, wxDefaultValidator, _T("ID_CHECKBOX36"));
  CheckBoxTOGGLETRIM->SetValue(false);
  CheckBoxTOGGLETRIM->SetToolTip(_("Change les trim de la main droite à la main gauche, et vice-versa"));
  CheckBoxFRAM = new wxCheckBox(Panel3, ID_CHECKBOX37, _("F-RAM"), wxPoint(312,192), wxSize(144,24), 0, wxDefaultValidator, _T("ID_CHECKBOX37"));
  CheckBoxFRAM->SetValue(false);
  CheckBoxFRAM->SetToolTip(_("Utilisation d\'une eeprom externe : FM24W256"));
  CheckBoxSHUTDOWN_CONFIRMATION = new wxCheckBox(Panel3, ID_CHECKBOX38, _("SHUTDOWN_CONFIRMATION"), wxPoint(480,224), wxSize(200,24), 0, wxDefaultValidator, _T("ID_CHECKBOX38"));
  CheckBoxSHUTDOWN_CONFIRMATION->SetValue(true);
  CheckBoxSHUTDOWN_CONFIRMATION->Disable();
  CheckBoxACCURAT_THROTTLE_STATS = new wxCheckBox(Panel3, ID_CHECKBOX39, _("ACCURAT_THROTTLE_STATS"), wxPoint(480,192), wxSize(200,24), 0, wxDefaultValidator, _T("ID_CHECKBOX39"));
  CheckBoxACCURAT_THROTTLE_STATS->SetValue(false);
  CheckBoxEEPROM_PROGRESS_BAR = new wxCheckBox(Panel3, ID_CHECKBOX43, _("EEPROM bar"), wxPoint(312,96), wxSize(144,24), 0, wxDefaultValidator, _T("ID_CHECKBOX43"));
  CheckBoxEEPROM_PROGRESS_BAR->SetValue(false);
  CheckBoxEEPROM_PROGRESS_BAR->SetToolTip(_("Affiche une mini barre pour visualiser une sauvegarde en cours"));
  CheckBoxAUTOSWITCH = new wxCheckBox(Panel3, ID_CHECKBOX44, _("AUTOSWITCH"), wxPoint(312,64), wxSize(144,24), 0, wxDefaultValidator, _T("ID_CHECKBOX44"));
  CheckBoxAUTOSWITCH->SetValue(false);
  CheckBoxAUTOSWITCH->SetToolTip(_("Auto detection des switchs lors de la programmatiion"));
  CheckBoxAUTOSOURCE = new wxCheckBox(Panel3, ID_CHECKBOX45, _("AUTOSOURCE"), wxPoint(312,32), wxSize(144,24), 0, wxDefaultValidator, _T("ID_CHECKBOX45"));
  CheckBoxAUTOSOURCE->SetValue(false);
  CheckBoxAUTOSOURCE->SetFocus();
  CheckBoxAUTOSOURCE->SetToolTip(_("Auto detection des manches lors de la programmatiion"));
  CheckBoxDBLKEYS = new wxCheckBox(Panel3, ID_CHECKBOX47, _("Double keys"), wxPoint(312,128), wxSize(144,24), 0, wxDefaultValidator, _T("ID_CHECKBOX47"));
  CheckBoxDBLKEYS->SetValue(false);
  CheckBoxDBLKEYS->SetToolTip(_("Raccourcis avec deux touches pour passer une valeur à min/max/0"));
  Compilerpage2 = new wxButton(Panel3, ID_BUTTON1, _("Compiler"), wxPoint(504,296), wxSize(128,24), 0, wxDefaultValidator, _T("ID_BUTTON1"));
  Sortirpage2 = new wxButton(Panel3, ID_BUTTON4, _("Sauvegarder et sortir"), wxPoint(648,296), wxSize(128,24), 0, wxDefaultValidator, _T("ID_BUTTON4"));
  CheckBoxCC2500 = new wxCheckBox(Panel3, ID_CHECKBOX23, _("CC2500"), wxPoint(24,200), wxSize(72,24), 0, wxDefaultValidator, _T("ID_CHECKBOX23"));
  CheckBoxCC2500->SetValue(false);
  CheckBoxCYRF6936 = new wxCheckBox(Panel3, ID_CHECKBOX24, _("CYRF6936"), wxPoint(24,224), wxSize(72,24), 0, wxDefaultValidator, _T("ID_CHECKBOX24"));
  CheckBoxCYRF6936->SetValue(false);
  CheckBoxNRF24l01 = new wxCheckBox(Panel3, ID_CHECKBOX25, _("NRF24l01"), wxPoint(24,248), wxSize(72,24), 0, wxDefaultValidator, _T("ID_CHECKBOX25"));
  CheckBoxNRF24l01->SetValue(false);
  CheckBoxSX1276 = new wxCheckBox(Panel3, ID_CHECKBOX35, _("SX1276"), wxPoint(24,296), wxSize(72,24), 0, wxDefaultValidator, _T("ID_CHECKBOX35"));
  CheckBoxSX1276->SetValue(false);
  CheckBoxSX1276->Disable();
  CheckBoxA7105 = new wxCheckBox(Panel3, ID_CHECKBOX29, _("A7105"), wxPoint(24,272), wxSize(72,24), 0, wxDefaultValidator, _T("ID_CHECKBOX29"));
  CheckBoxA7105->SetValue(false);
  ChoiceCYRF6936PAG = new wxChoice(Panel3, ID_CHOICECYRF6936PAG, wxPoint(96,224), wxSize(45,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICECYRF6936PAG"));
  StaticText28 = new wxStaticText(Panel3, ID_STATICTEXT28, _("PA Gain (DB)"), wxPoint(24,176), wxSize(118,16), wxALIGN_RIGHT, _T("ID_STATICTEXT28"));
  CheckBoxXMODEM = new wxCheckBox(Panel3, ID_CHECKBOX34, _("XMODEM"), wxPoint(688,32), wxSize(80,24), 0, wxDefaultValidator, _T("ID_CHECKBOX34"));
  CheckBoxXMODEM->SetValue(false);
  CheckBoxXMODEM->SetToolTip(_("Transfert de données"));
  ChoiceBLUETOOTH = new wxChoice(Panel3, ID_CHOICEBLUETOOTH, wxPoint(392,224), wxSize(56,23), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICEBLUETOOTH"));
  StaticText30 = new wxStaticText(Panel3, ID_STATICTEXT30, _("BLUETOOTH"), wxPoint(312,230), wxDefaultSize, 0, _T("ID_STATICTEXT30"));
  ChoicePCM_PROTOCOL = new wxChoice(Panel3, ID_PROTOPCMCHOICE, wxPoint(24,80), wxSize(112,23), 0, 0, 0, wxDefaultValidator, _T("ID_PROTOPCMCHOICE"));
  ChoicePCM_PROTOCOL->SetToolTip(_("Protocoles PCM"));
  ChoiceSERIAL_PROTOCOL = new wxChoice(Panel3, ID_PROTOSERIALCHOICE, wxPoint(24,120), wxSize(112,23), 0, 0, 0, wxDefaultValidator, _T("ID_PROTOSERIALCHOICE"));
  ChoiceSERIAL_PROTOCOL->SetToolTip(_("Protocoles Série"));
  StaticText200 = new wxStaticText(Panel3, ID_STATICTEXT200, _("Protocole Pcm"), wxPoint(24,64), wxDefaultSize, 0, _T("ID_STATICTEXT200"));
  StaticText31 = new wxStaticText(Panel3, ID_STATICTEXT31, _("Protocole Série"), wxPoint(24,104), wxDefaultSize, 0, _T("ID_STATICTEXT31"));
  Panel4 = new wxPanel(Notebook1, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
  StaticBox7 = new wxStaticBox(Panel4, ID_STATICBOX7, _("Nom des interrupteurs"), wxPoint(8,8), wxSize(504,328), 0, _T("ID_STATICBOX7"));
  StaticBox14 = new wxStaticBox(Panel4, ID_STATICBOX14, _("Manches"), wxPoint(368,32), wxSize(128,176), 0, _T("ID_STATICBOX14"));
  CheckBoxDBLGAZSTICK = new wxCheckBox(Panel4, ID_CHECKBOX11, _("DBLGAZSTICK"), wxPoint(384,184), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX11"));
  CheckBoxDBLGAZSTICK->SetValue(false);
  CheckBoxDBLGAZSTICK->SetToolTip(_("Affiche un double manche vertical des gaz"));
  ComboBoxEle = new wxComboBox(Panel4, ID_COMBOBOX18, wxEmptyString, wxPoint(424,88), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX18"));
  ComboBoxEle->SetSelection( ComboBoxEle->Append(_("Ele")) );
  ComboBoxEle->Append(_("---"));
  StaticText35 = new wxStaticText(Panel4, ID_STATICTEXT35, _("Thr"), wxPoint(384,128), wxSize(24,16), wxALIGN_RIGHT, _T("ID_STATICTEXT35"));
  ComboBoxXD0 = new wxComboBox(Panel4, ID_COMBOBOX16, wxEmptyString, wxPoint(304,40), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX16"));
  ComboBoxXD0->SetSelection( ComboBoxXD0->Append(_("XD0")) );
  ComboBoxXD0->Append(_("---"));
  StaticText33 = new wxStaticText(Panel4, ID_STATICTEXT33, _("Rud"), wxPoint(384,64), wxSize(24,16), wxALIGN_RIGHT, _T("ID_STATICTEXT33"));
  StaticText34 = new wxStaticText(Panel4, ID_STATICTEXT34, _("Ail"), wxPoint(384,160), wxSize(24,16), wxALIGN_RIGHT, _T("ID_STATICTEXT34"));
  StaticBox10 = new wxStaticBox(Panel4, ID_STATICBOX10, _("3POS"), wxPoint(160,24), wxSize(96,112), 0, _T("ID_STATICBOX10"));
  ComboBoxEncodeurNo = new wxComboBox(Panel4, ID_COMBOBOX12, wxEmptyString, wxPoint(232,216), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX12"));
  ComboBoxEncodeurNo->SetSelection( ComboBoxEncodeurNo->Append(_("REn")) );
  ComboBoxEncodeurNo->Append(_("---"));
  ComboBoxswitch1 = new wxComboBox(Panel4, ID_COMBOBOX1, wxEmptyString, wxPoint(64,40), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX1"));
  ComboBoxswitch1->SetSelection( ComboBoxswitch1->Append(_("THR")) );
  ComboBoxswitch1->Append(_("---"));
  Sortirpage3 = new wxButton(Panel4, ID_BUTTON5, _("Sauvegarder et sortir"), wxPoint(640,296), wxSize(128,24), 0, wxDefaultValidator, _T("ID_BUTTON5"));
  Compilerpage3 = new wxButton(Panel4, ID_BUTTON6, _("Compiler"), wxPoint(640,256), wxSize(128,24), 0, wxDefaultValidator, _T("ID_BUTTON6"));
  ComboBoxswitch2 = new wxComboBox(Panel4, ID_COMBOBOX2, wxEmptyString, wxPoint(64,72), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX2"));
  ComboBoxswitch2->SetSelection( ComboBoxswitch2->Append(_("RUD")) );
  ComboBoxswitch2->Append(_("---"));
  ComboBoxswitch3 = new wxComboBox(Panel4, ID_COMBOBOX3, wxEmptyString, wxPoint(64,104), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX3"));
  ComboBoxswitch3->SetSelection( ComboBoxswitch3->Append(_("ELE")) );
  ComboBoxswitch3->Append(_("---"));
  ComboBoxswitch4 = new wxComboBox(Panel4, ID_COMBOBOX4, wxEmptyString, wxPoint(64,136), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX4"));
  ComboBoxswitch4->SetSelection( ComboBoxswitch4->Append(_("AIL")) );
  ComboBoxswitch4->Append(_("---"));
  ComboBoxswitch5 = new wxComboBox(Panel4, ID_COMBOBOX5, wxEmptyString, wxPoint(64,168), wxSize(48,21), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX5"));
  ComboBoxswitch5->SetSelection( ComboBoxswitch5->Append(_("GEA")) );
  ComboBoxswitch5->Append(_("---"));
  ComboBoxswitch6 = new wxComboBox(Panel4, ID_COMBOBOX6, wxEmptyString, wxPoint(64,200), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX6"));
  ComboBoxswitch6->SetSelection( ComboBoxswitch6->Append(_("TRN")) );
  ComboBoxswitch6->Append(_("---"));
  CheckBoxPERSONAMES = new wxCheckBox(Panel4, ID_CHECKBOX22, _("PERSONAMES"), wxPoint(360,240), wxSize(120,24), 0, wxDefaultValidator, _T("ID_CHECKBOX22"));
  CheckBoxPERSONAMES->SetValue(false);
  CheckBoxPERSONAMES->SetToolTip(_("Change l\'appellation des boutons et des manches"));
  StaticText13 = new wxStaticText(Panel4, ID_STATICTEXT13, _("Inter1"), wxPoint(16,48), wxSize(40,16), wxALIGN_RIGHT, _T("ID_STATICTEXT13"));
  StaticText14 = new wxStaticText(Panel4, ID_STATICTEXT14, _("Inter2"), wxPoint(16,80), wxSize(40,16), wxALIGN_RIGHT, _T("ID_STATICTEXT14"));
  StaticText15 = new wxStaticText(Panel4, ID_STATICTEXT15, _("Inter3"), wxPoint(16,112), wxSize(40,16), wxALIGN_RIGHT, _T("ID_STATICTEXT15"));
  StaticText16 = new wxStaticText(Panel4, ID_STATICTEXT16, _("Inter4"), wxPoint(16,144), wxSize(40,16), wxALIGN_RIGHT, _T("ID_STATICTEXT16"));
  StaticText17 = new wxStaticText(Panel4, ID_STATICTEXT17, _("Inter5"), wxPoint(16,176), wxSize(40,16), wxALIGN_RIGHT, _T("ID_STATICTEXT17"));
  StaticText18 = new wxStaticText(Panel4, ID_STATICTEXT18, _("Inter6"), wxPoint(16,208), wxSize(40,16), wxALIGN_RIGHT, _T("ID_STATICTEXT18"));
  ButtonDefaut = new wxButton(Panel4, ID_BUTTON7, _("Défaut"), wxPoint(360,280), wxSize(104,24), 0, wxDefaultValidator, _T("ID_BUTTON7"));
  StaticText25 = new wxStaticText(Panel4, ID_STATICTEXT25, _("ID0"), wxPoint(168,48), wxSize(24,16), wxALIGN_RIGHT, _T("ID_STATICTEXT25"));
  StaticText19 = new wxStaticText(Panel4, ID_STATICTEXT19, _("XD0"), wxPoint(272,48), wxSize(24,16), wxALIGN_RIGHT, _T("ID_STATICTEXT19"));
  ComboBoxRud = new wxComboBox(Panel4, ID_COMBOBOX17, wxEmptyString, wxPoint(424,56), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX17"));
  ComboBoxRud->SetSelection( ComboBoxRud->Append(_("Rud")) );
  ComboBoxRud->Append(_("---"));
  ComboBoxThr = new wxComboBox(Panel4, ID_COMBOBOX19, wxEmptyString, wxPoint(424,120), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX19"));
  ComboBoxThr->SetSelection( ComboBoxThr->Append(_("Thr")) );
  ComboBoxThr->Append(_("---"));
  ComboBoxAil = new wxComboBox(Panel4, ID_COMBOBOX20, wxEmptyString, wxPoint(424,152), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX20"));
  ComboBoxAil->SetSelection( ComboBoxAil->Append(_("Ail")) );
  ComboBoxAil->Append(_("---"));
  ComboBoxXD2 = new wxComboBox(Panel4, ID_COMBOBOX15, wxEmptyString, wxPoint(304,104), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX15"));
  ComboBoxXD2->SetSelection( ComboBoxXD2->Append(_("XD2")) );
  ComboBoxXD2->Append(_("---"));
  ComboBoxXD1 = new wxComboBox(Panel4, ID_COMBOBOX13, wxEmptyString, wxPoint(304,72), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX13"));
  ComboBoxXD1->SetSelection( ComboBoxXD1->Append(_("XD1")) );
  ComboBoxXD1->Append(_("---"));
  ComboBoxID0 = new wxComboBox(Panel4, ID_COMBOBOX7, wxEmptyString, wxPoint(200,40), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX7"));
  ComboBoxID0->SetSelection( ComboBoxID0->Append(_("ID0")) );
  ComboBoxID0->Append(_("---"));
  StaticText20 = new wxStaticText(Panel4, ID_STATICTEXT20, _("EncodeurA"), wxPoint(152,160), wxSize(72,16), wxALIGN_RIGHT, _T("ID_STATICTEXT20"));
  StaticText24 = new wxStaticText(Panel4, ID_STATICTEXT24, _("EncodeurB"), wxPoint(152,192), wxSize(72,16), wxALIGN_RIGHT, _T("ID_STATICTEXT24"));
  StaticText21 = new wxStaticText(Panel4, ID_STATICTEXT21, _(" Encodeur à 0"), wxPoint(152,224), wxSize(72,16), wxALIGN_RIGHT, _T("ID_STATICTEXT21"));
  ComboBoxEncodeurA = new wxComboBox(Panel4, ID_COMBOBOX8, wxEmptyString, wxPoint(232,152), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX8"));
  ComboBoxEncodeurA->SetSelection( ComboBoxEncodeurA->Append(_("REa")) );
  ComboBoxEncodeurA->Append(_("---"));
  ComboBoxEncodeurB = new wxComboBox(Panel4, ID_COMBOBOX9, wxEmptyString, wxPoint(232,184), wxSize(48,21), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX9"));
  ComboBoxEncodeurB->SetSelection( ComboBoxEncodeurB->Append(_("REb")) );
  ComboBoxEncodeurB->Append(_("---"));
  ComboBoxID1 = new wxComboBox(Panel4, ID_COMBOBOX10, wxEmptyString, wxPoint(200,72), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX10"));
  ComboBoxID1->SetSelection( ComboBoxID1->Append(_("ID1")) );
  ComboBoxID1->Append(_("---"));
  ComboBoxID2 = new wxComboBox(Panel4, ID_COMBOBOX11, wxEmptyString, wxPoint(200,104), wxSize(48,21), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX11"));
  ComboBoxID2->SetSelection( ComboBoxID2->Append(_("ID2")) );
  ComboBoxID2->Append(_("---"));
  StaticText26 = new wxStaticText(Panel4, ID_STATICTEXT26, _("ID1"), wxPoint(168,80), wxSize(24,16), wxALIGN_RIGHT, _T("ID_STATICTEXT26"));
  StaticText22 = new wxStaticText(Panel4, ID_STATICTEXT22, _("XD1"), wxPoint(272,80), wxSize(24,16), wxALIGN_RIGHT, _T("ID_STATICTEXT22"));
  StaticText27 = new wxStaticText(Panel4, ID_STATICTEXT27, _("ID2"), wxPoint(168,112), wxSize(24,16), wxALIGN_RIGHT, _T("ID_STATICTEXT27"));
  StaticText23 = new wxStaticText(Panel4, ID_STATICTEXT23, _("XD2"), wxPoint(272,112), wxSize(24,16), wxALIGN_RIGHT, _T("ID_STATICTEXT23"));
  StaticBox12 = new wxStaticBox(Panel4, ID_STATICBOX12, _("Autres options de compilation"), wxPoint(520,16), wxSize(256,136), 0, _T("ID_STATICBOX12"));
  TextCtrlOTHERCOMPOPTIONS = new wxTextCtrl(Panel4, ID_TEXTCTRL1, wxEmptyString, wxPoint(528,40), wxSize(232,96), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
  StaticText32 = new wxStaticText(Panel4, ID_STATICTEXT32, _("Ele"), wxPoint(384,96), wxSize(24,16), wxALIGN_RIGHT, _T("ID_STATICTEXT32"));
  StaticBox8 = new wxStaticBox(Panel4, ID_STATICBOX8, _("Extra 3POS"), wxPoint(264,24), wxSize(96,112), 0, _T("ID_STATICBOX8"));
  Panel5 = new wxPanel(Notebook1, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL5"));
  StaticBox11 = new wxStaticBox(Panel5, ID_STATICBOX11, _("X-Any Options"), wxPoint(8,8), wxSize(280,328), 0, _T("ID_STATICBOX11"));
  Button1 = new wxButton(Panel5, ID_BUTTON8, _("Sauvegarder et sortir"), wxPoint(640,296), wxSize(128,24), 0, wxDefaultValidator, _T("ID_BUTTON8"));
  Button2 = new wxButton(Panel5, ID_BUTTON9, _("Compiler"), wxPoint(640,256), wxSize(128,24), 0, wxDefaultValidator, _T("ID_BUTTON9"));
  ChoiceNUMXANY = new wxChoice(Panel5, ID_CHOICENUMXANY, wxPoint(168,80), wxSize(48,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICENUMXANY"));
  ChoiceNUMXANY->SetToolTip(_("Nombre d\'instances"));
  StaticText29 = new wxStaticText(Panel5, ID_STATICTEXT29, _("Nb X-ANY"), wxPoint(112,88), wxSize(48,16), wxALIGN_RIGHT, _T("ID_STATICTEXT29"));
  Notebook1->AddPage(Panel2, _("Réglages"), false);
  Notebook1->AddPage(Panel3, _("Options avancées"), false);
  Notebook1->AddPage(Panel4, _("Personnalisation"), false);
  Notebook1->AddPage(Panel5, _("X-Any"), false);

  Connect(ID_CHOICE3,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CompilerOptionsFrame::OnChoiceVOICESelect);
  Connect(ID_CHECKBOX12,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxSD_CARDClick);
  Connect(ID_CHOICE8,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CompilerOptionsFrame::OnChoiceNAVIGATIONSelect);
  Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonEXITClick);
  Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonCOMPILEClick);
  Connect(ID_CHECKBOXNOANDSECONDE,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxNOANDSECONDEClick1);
  Connect(ID_CHOICE2,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CompilerOptionsFrame::OnChoiceEXTSelect2);
  Connect(ID_CHECKBOX5,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxGPSClick1);
  Connect(ID_CHECKBOX6,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxVARIOClick2);
  Connect(ID_CHECKBOX32,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxWS_HOW_HIGHClick2);
  Connect(ID_CHECKBOX45,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxAUTOSOURCEClick1);
  Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonCOMPILEClick);
  Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonEXITClick);
  Connect(ID_CHECKBOX23,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxPERSONAMESClick);
  Connect(ID_CHECKBOX24,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxPERSONAMESClick);
  Connect(ID_CHECKBOX25,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxPERSONAMESClick);
  Connect(ID_CHECKBOX35,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxPERSONAMESClick);
  Connect(ID_CHECKBOX29,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxPERSONAMESClick);
  Connect(ID_CHECKBOX34,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxXMODEMClick);
  Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonEXITClick);
  Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonCOMPILEClick);
  Connect(ID_CHECKBOX22,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxPERSONAMESClick);
  Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonDefautClick1);
  Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonEXITClick);
  Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonCOMPILEClick);
  Connect(ID_CHOICENUMXANY,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxPERSONAMESClick);
  Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&CompilerOptionsFrame::OnClose);
  //*)

  {
    SetIcon(wxICON(oavrc_icon));
  }

  ///////////////////////// avoid compilation option translation ///////////////////
  ChoicePPM_UNIT ->SetSelection( ChoicePPM_UNIT ->Append("PERCENT_PREC1") );
  ChoicePPM_UNIT ->Append("PERCENT_PREC0");
  ChoicePPM_UNIT ->Append("US");

  ChoicePCB->SetSelection( ChoicePCB->Append("MEGAMINI") );

  ChoiceLCD->SetSelection( ChoiceLCD->Append("KS108") );
  ChoiceLCD->Append("ST7565P");
  ChoiceLCD->Append("ST7565R ");
  ChoiceLCD->Append("ERC12864FSF");
  ChoiceLCD->Append("ST7920");
  ChoiceLCD->Append("SSD1306");
  ChoiceLCD->Append("SH1106");

  ChoiceVOICE->SetSelection( ChoiceVOICE->Append("NO") );
  ChoiceVOICE->Append("JQ6500");

  ChoiceTRANSLATIONS->SetSelection( ChoiceTRANSLATIONS->Append("FR") );
  ChoiceTRANSLATIONS->Append("CZ");
  ChoiceTRANSLATIONS->Append("DE");
  ChoiceTRANSLATIONS->Append("EN");
  ChoiceTRANSLATIONS->Append("ES");
  ChoiceTRANSLATIONS->Append("HU");
  ChoiceTRANSLATIONS->Append("IT");
  ChoiceTRANSLATIONS->Append("NL");
  ChoiceTRANSLATIONS->Append("PL");
  ChoiceTRANSLATIONS->Append("PT");
  ChoiceTRANSLATIONS->Append("SE");
  ChoiceTRANSLATIONS->Append("SK");

  ChoiceTTS->SetSelection( ChoiceTTS->Append("FR") );
  ChoiceTTS->Append("CZ");
  ChoiceTTS->Append("DE");
  ChoiceTTS->Append("EN");
  ChoiceTTS->Append("ES");
  ChoiceTTS->Append("HU");
  ChoiceTTS->Append("IT");
  ChoiceTTS->Append("NL");
  ChoiceTTS->Append("PL");
  ChoiceTTS->Append("PT");
  ChoiceTTS->Append("SE");
  ChoiceTTS->Append("SK");

  ChoiceNAVIGATION->SetSelection( ChoiceNAVIGATION->Append("NO") );
  ChoiceNAVIGATION->Append("POT1");
  ChoiceNAVIGATION->Append("POT2");
  ChoiceNAVIGATION->Append("POT3");
  ChoiceNAVIGATION->Append("POTS");
  ChoiceNAVIGATION->Append("STICKS");
  ChoiceNAVIGATION->Append("ROTENC");

  ChoiceUNITS->SetSelection( ChoiceUNITS->Append("METRIC") );
  ChoiceUNITS->Append("IMPERIAL");

  ChoiceDEFAULT_MODE->SetSelection( ChoiceDEFAULT_MODE->Append("NO") );
  ChoiceDEFAULT_MODE->Append("1");
  ChoiceDEFAULT_MODE->Append("2");
  ChoiceDEFAULT_MODE->Append("3");
  ChoiceDEFAULT_MODE->Append("4");

  ChoiceFONT->SetSelection( ChoiceFONT->Append("STD") );
  ChoiceFONT->Append("SQT5");

  ChoiceFAI->SetSelection( ChoiceFAI->Append("NO") );
  ChoiceFAI->Append("YES");
  ChoiceFAI->Append("CHOICE");

  ChoiceTHREE_POS->Append("POT1");
  ChoiceTHREE_POS->SetSelection( ChoiceTHREE_POS->Append("POT2") );
  ChoiceTHREE_POS->Append("POT3");

  ChoiceEXT->SetSelection( ChoiceEXT->Append("STD") );
  ChoiceEXT->Append("FRSKY");

  ChoiceCC2500PAG->SetSelection( ChoiceCC2500PAG->Append("20") );
  ChoiceCC2500PAG->Append("22");

  ChoiceA7105PAG->SetSelection( ChoiceA7105PAG->Append("20") );
  ChoiceA7105PAG->Append("22");

  ChoiceNRF24l01PAG->SetSelection( ChoiceNRF24l01PAG->Append("20") );
  ChoiceNRF24l01PAG->Append("22");

  ChoiceCYRF6936PAG->SetSelection( ChoiceCYRF6936PAG->Append("20") );
  ChoiceCYRF6936PAG->Append("22");

  ChoiceBLUETOOTH->SetSelection( ChoiceBLUETOOTH->Append("NO") );
  ChoiceBLUETOOTH->Append("HC05");
  ChoiceBLUETOOTH->Append("HM10"); //todo add hm10

  ChoiceNUMXANY->SetSelection( ChoiceNUMXANY->Append("NO") );
  ChoiceNUMXANY->Append("1");
  ChoiceNUMXANY->Append("2");
  ChoiceNUMXANY->Append("3");
  ChoiceNUMXANY->Append("4");

  ChoiceSERIAL_PROTOCOL->Append("NO");
  ChoiceSERIAL_PROTOCOL->Append("DSM");
  ChoiceSERIAL_PROTOCOL->Append("MULTIMODULE");
  ChoiceSERIAL_PROTOCOL->Append("CRSF");
  ChoiceSERIAL_PROTOCOL->Append("SBUS");
  ChoiceSERIAL_PROTOCOL->Append("SUMD");

  ChoicePCM_PROTOCOL->Append("NO");
  ChoicePCM_PROTOCOL->Append("FUTPCM1K");
  ChoicePCM_PROTOCOL->Append("GRAUSPCM");
  ChoicePCM_PROTOCOL->Append("MULMPCM");
  ChoicePCM_PROTOCOL->Append("WALKPCM");

///////////////////////// End of avoid compilation option translation ///////////////////

  ChoicePCB->SetStringSelection(PCB);
  ChoiceLCD->SetStringSelection(LCD);
  CheckBoxLCDROT180->SetValue(LCDROT180);
  ChoiceVOICE->SetStringSelection(VOICE);
  ChoiceEXT->SetStringSelection(EXT);
  CheckBoxAUDIO->SetValue(AUDIO);
  CheckBoxHELI->SetValue(HELI);
  ChoiceTTS->SetStringSelection(TTS);
  ChoiceTRANSLATIONS->SetStringSelection(TRANSLATIONS);
  ChoiceNAVIGATION->SetStringSelection(NAVIGATION);
  CheckBoxHAPTIC->SetValue(HAPTIC);
  ChoicePPM_UNIT->SetStringSelection(PPM_UNIT);
  CheckBoxGPS->SetValue(GPS);
  CheckBoxVARIO->SetValue(VARIO);
  CheckBoxPPM->SetValue(PPM);
  ChoicePCM_PROTOCOL->SetStringSelection(PCM_PROTOCOL);
  ChoiceSERIAL_PROTOCOL->SetStringSelection(SERIAL_PROTOCOL);
  CheckBoxCC2500->SetValue(CC2500);
  CheckBoxCYRF6936->SetValue(CYRF6936);
  CheckBoxNRF24l01->SetValue(NRF24l01);
  CheckBoxA7105->SetValue(A7105);
  CheckBoxSX1276->SetValue(SX1276);
  ChoiceCC2500PAG->SetStringSelection(CC2500PAG);
  ChoiceCYRF6936PAG->SetStringSelection(CYRF6936PAG);
  ChoiceNRF24l01PAG->SetStringSelection(NRF24l01PAG);
  ChoiceA7105PAG->SetStringSelection(A7105PAG);
  CheckBoxSD_CARD->SetValue(SD_CARD);
  CheckBoxTEMPLATES->SetValue(TEMPLATES);
  ChoiceTHREE_POS->SetStringSelection(THREE_POS);
  CheckBoxSPLASH->SetValue(SPLASH);
  ChoiceUNITS->SetStringSelection(UNITS);
  ChoiceDEFAULT_MODE->SetStringSelection(DEFAULT_MODE);
  ChoiceFONT->SetStringSelection(FONT);
  CheckBoxBOLD->SetValue(BOLD);
  CheckBoxBATTGRAPH->SetValue(BATTGRAPH);
  CheckBoxEEPROM_PROGRESS_BAR->SetValue(EEPROM_PROGRESS_BAR);
  ChoiceFAI->SetStringSelection(FAI);
  CheckBoxAUTOSWITCH->SetValue(AUTOSWITCH);
  CheckBoxAUTOSOURCE->SetValue(AUTOSOURCE);
  CheckBoxDBLKEYS->SetValue(DBLKEYS);
  CheckBoxPPM_CENTER_ADJUSTABLE->SetValue(PPM_CENTER_ADJUSTABLE);
  CheckBoxPPM_LIMITS_SYMETRICAL->SetValue(PPM_LIMITS_SYMETRICAL);
  CheckBoxFLIGHT_MODES->SetValue(FLIGHT_MODES);
  CheckBoxCURVES->SetValue(CURVES);
  CheckBoxGVARS->SetValue(GVARS);
  CheckBoxCORRECT_NEGATIVE_VALUES->SetValue(CORRECT_NEGATIVE_VALUES);
  CheckBoxARITHMETIC_OVERFLOW_CHECK->SetValue(ARITHMETIC_OVERFLOW_CHECK);
  CheckBoxACCURAT_THROTTLE_STATS->SetValue(ACCURAT_THROTTLE_STATS);
  CheckBoxPWM_BACKLIGHT->SetValue(PWM_BACKLIGHT);
  CheckBoxOVERRIDE_CHANNEL_FUNCTION->SetValue(OVERRIDE_CHANNEL_FUNCTION);
  CheckBoxWS_HOW_HIGH->SetValue(WS_HOW_HIGH);
  CheckBoxTOGGLETRIM->SetValue(TOGGLETRIM);
  CheckBoxNOANDSECONDE->SetValue(NOANDSECONDE);// Hardwired
  CheckBoxSHUTDOWN_CONFIRMATION->SetValue(SHUTDOWN_CONFIRMATION);
  CheckBoxFRAM->SetValue(FRAM);
  ChoiceBLUETOOTH->SetStringSelection(BLUETOOTH);
  CheckBoxXMODEM->SetValue(XMODEM);
  CheckBoxPERSONAMES->SetValue(PERSONAMES);
  CheckBoxDBLGAZSTICK->SetValue(DBLGAZSTICK);
  TextCtrlOTHERCOMPOPTIONS->SetValue(OTHERCOMPOPTIONS);
  CheckBoxInvStickRH->SetValue(INV_STICK_RH);
  CheckBoxInvStickLV->SetValue(INV_STICK_LV);
  CheckBoxInvStickRV->SetValue(INV_STICK_RV);
  CheckBoxInvStickLH->SetValue(INV_STICK_LH);

  //X-Any
  ChoiceNUMXANY->SetStringSelection(NUMXANY);


  ComboBoxswitch1->SetMaxLength(3); // Limit 3 caracteres
  ComboBoxswitch2->SetMaxLength(3);
  ComboBoxswitch3->SetMaxLength(3);
  ComboBoxswitch4->SetMaxLength(3);
  ComboBoxswitch5->SetMaxLength(3);
  ComboBoxswitch6->SetMaxLength(3);
  ComboBoxID0->SetMaxLength(3);
  ComboBoxID1->SetMaxLength(3);
  ComboBoxID2->SetMaxLength(3);
  ComboBoxEncodeurA->SetMaxLength(3);
  ComboBoxEncodeurB->SetMaxLength(3);
  ComboBoxXD0->SetMaxLength(3);
  ComboBoxXD1->SetMaxLength(3);
  ComboBoxXD2->SetMaxLength(3);
  ComboBoxRud->SetMaxLength(3);
  ComboBoxEle->SetMaxLength(3);
  ComboBoxThr->SetMaxLength(3);
  ComboBoxAil->SetMaxLength(3);

  ComboBoxswitch1->SetValue(switch1);//personnalized names for switches.
  ComboBoxswitch2->SetValue(switch2);
  ComboBoxswitch3->SetValue(switch3);
  ComboBoxswitch4->SetValue(switch4);
  ComboBoxswitch5->SetValue(switch5);
  ComboBoxswitch6->SetValue(switch6);
  ComboBoxID0->SetValue(switchID0);
  ComboBoxID1->SetValue(switchID1);
  ComboBoxID2->SetValue(switchID2);
  ComboBoxEncodeurA->SetValue(encoderA);
  ComboBoxEncodeurB->SetValue(encoderB);
  ComboBoxEncodeurNo->SetValue(encoderNo);
  ComboBoxXD0->SetValue(switchXD0);
  ComboBoxXD1->SetValue(switchXD1);
  ComboBoxXD2->SetValue(switchXD2);
  ComboBoxRud->SetValue(stickRud);
  ComboBoxEle->SetValue(stickEle);
  ComboBoxThr->SetValue(stickThr);
  ComboBoxAil->SetValue(stickAil);
}

CompilerOptionsFrame::~CompilerOptionsFrame()
{
  //(*Destroy(CompilerOptionsFrame)
  //*)
}

void CompilerOptionsFrame::OnClose(wxCloseEvent& event)
{
  OpenAVRc_DesktopFrame *parent = wxDynamicCast(this->GetParent(), OpenAVRc_DesktopFrame);
  if(parent)
    parent->EnableCompilOMaticSelectedMenu();
  Destroy();
}

void CompilerOptionsFrame::BatFunction()
{
  // Write splash file
  WriteSplashFile();


  // .bat file for compilation
  wxString CompiBat = "make";
  CompiBat += (" PCB=" + PCB);
  CompiBat += (" LCD=" + LCD);
  if (LCDROT180) CompiBat += (" LCDROT180=YES");// default should be NO
  CompiBat += (" VOICE=" + VOICE);
  CompiBat += (" EXT=" + EXT);
  if (AUDIO) CompiBat += (" AUDIO=YES");// default should be NO
  if (HELI) CompiBat += (" HELI=YES");// default should be NO
  CompiBat += (" TTS=" + TTS);
  CompiBat += (" TRANSLATIONS=" + TRANSLATIONS);
  CompiBat += (" NAVIGATION=" + NAVIGATION);
  if (HAPTIC) CompiBat += (" HAPTIC=YES");// default should be NO
  CompiBat += (" PPM_UNIT=" + PPM_UNIT);
  if (GPS) CompiBat += (" GPS=YES");// default should be NO
  if (VARIO) CompiBat += (" VARIO=YES");// default should be NO
  if (PPM) CompiBat += (" PPM=YES");// default should be YES
  CompiBat += (" SERIAL_PROTOCOL=") + SERIAL_PROTOCOL;// default should be NO
  CompiBat += (" PCM_PROTOCOL=") + PCM_PROTOCOL;// default should be NO
  if   (CC2500 | CYRF6936 | NRF24l01 | A7105 | SX1276)
    {
      CompiBat += (" SPIMODULES=YES");// default should be NO
      if (CC2500)
        {
          CompiBat += (" CC2500=YES");// default should be NO
          CompiBat += (" CC2500PAG=" + CC2500PAG);
        }
      if (CYRF6936)
        {
          CompiBat += (" CYRF6936=YES");// default should be NO
          CompiBat += (" CYRF6936PAG=" + CYRF6936PAG);
        }
      if (NRF24l01)
        {
          CompiBat += (" NRF24L01=YES");// default should be NO
          CompiBat += (" NRF24L01PAG=" + NRF24l01PAG);
        }
      if (A7105)
        {
          CompiBat += (" A7105=YES");// default should be NO
          CompiBat += (" A7105PAG=" + A7105PAG);
        }
      if (SX1276) // todo ...
        {
          CompiBat += (" SX1276=YES");// default should be NO
        }
    }
  if (SD_CARD)
    CompiBat += (" SDCARD=YES");// default should be NO
  if (TEMPLATES) CompiBat += (" TEMPLATES=YES");// default should be NO
  CompiBat += (" THREE_POS=" + THREE_POS);
  if (SPLASH) CompiBat += (" SPLASH=YES");
  CompiBat += (" UNITS=" + UNITS);
  CompiBat += (" DEFAULT_MODE=" + DEFAULT_MODE);
  CompiBat += (" FONT=" + FONT);
  if (BOLD) CompiBat += (" BOLD=YES");
  if (BATTGRAPH) CompiBat += (" BATTGRAPH=YES");
  if (EEPROM_PROGRESS_BAR) CompiBat += (" EEPROM_PROGRESS_BAR=YES");
  CompiBat += (" FAI=" + FAI);
  if (AUTOSWITCH) CompiBat += (" AUTOSWITCH=YES");
  if (AUTOSOURCE) CompiBat += (" AUTOSOURCE=YES");
  if (DBLKEYS) CompiBat += (" DBLKEYS=YES");
  if (PPM_CENTER_ADJUSTABLE) CompiBat += (" PPM_CENTER_ADJUSTABLE=YES");
  if (PPM_LIMITS_SYMETRICAL) CompiBat += (" PPM_LIMITS_SYMETRICAL=YES");
  if (FLIGHT_MODES) CompiBat += (" FLIGHT_MODES=YES");
  if (CURVES) CompiBat += (" CURVES=YES");
  if (GVARS) CompiBat += (" GVARS=YES");
  if (CORRECT_NEGATIVE_VALUES) CompiBat += (" CORRECT_NEGATIVE_VALUES=YES");
  if (ARITHMETIC_OVERFLOW_CHECK) CompiBat += (" ARITHMETIC_OVERFLOW_CHECK=YES");
  if (ACCURAT_THROTTLE_STATS) CompiBat += (" ACCURAT_THROTTLE_STATS=YES");
  if (PWM_BACKLIGHT) CompiBat += (" PWM_BACKLIGHT=YES");
  if (OVERRIDE_CHANNEL_FUNCTION) CompiBat += (" OVERRIDE_CHANNEL_FUNCTION=YES");
  if (WS_HOW_HIGH) CompiBat += (" WS_HOW_HIGH=YES");
  if (TOGGLETRIM) CompiBat += (" TOGGLETRIM=YES");
  if (NOANDSECONDE) CompiBat += (" NOANDSECONDE=YES");
  if (SHUTDOWN_CONFIRMATION) CompiBat += (" SHUTDOWN_CONFIRMATION=YES");
  if (FRAM) CompiBat += (" EXTERNALEEPROM=YES");
  if (INV_STICK_RH) CompiBat += (" INV_STICK_RH=YES");
  if (INV_STICK_LV) CompiBat += (" INV_STICK_LV=YES");
  if (INV_STICK_RV) CompiBat += (" INV_STICK_RV=YES");
  if (INV_STICK_LH) CompiBat += (" INV_STICK_LH=YES");
  CompiBat += (" BLUETOOTH=" + BLUETOOTH);
  if (XMODEM) CompiBat += (" XMODEM=YES");
  if (PERSONAMES)
    {
      CompiBat += (" PERSONAMES=YES");
      CreatePersonames_H(); // Create the header file
    }
  if (DBLGAZSTICK) CompiBat += (" DBLGAZSTICK=YES");
  if (NUMXANY != "NO")
    {
      CompiBat += (" X_ANY=" + NUMXANY);
    }
  if (!OTHERCOMPOPTIONS.IsEmpty())
    {
      wxString tempOtherCompOptions = OTHERCOMPOPTIONS;
      tempOtherCompOptions.Replace("\n", "*");
      CompiBat += " OTHERCOMPOPTIONS=" + tempOtherCompOptions;
    }

  wxMessageBox(CompiBat);
  CreateCompileBatFile(CompiBat);
  wxExecute(AppPath+ "\\CompileBatFile.bat",wxEXEC_ASYNC );// Create firmware
  Close();
  //wxMessageBox(AppPath+ "\\firmware\\OpenAVRc.hex", _("Le nouveau Firmware est Le fichier :"));
}

void CompilerOptionsFrame::OnButtonCOMPILEClick(wxCommandEvent& event)
{
  CollectDatas();
  Ini_Changed = true;
  if (!doNotClose) BatFunction();
}

void CompilerOptionsFrame::CollectDatas()
{
  doNotClose = 0;
  PCB  = ChoicePCB->GetString(ChoicePCB->GetSelection());
  LCD  = ChoiceLCD->GetString(ChoiceLCD->GetSelection());
  LCDROT180 = CheckBoxLCDROT180->GetValue();
  VOICE  = ChoiceVOICE->GetString(ChoiceVOICE->GetSelection());
  EXT  = ChoiceEXT->GetString(ChoiceEXT->GetSelection());
  AUDIO = CheckBoxAUDIO->GetValue();
  HELI = CheckBoxHELI->GetValue();
  TTS  = ChoiceTTS->GetString(ChoiceTTS->GetSelection());
  TRANSLATIONS  = ChoiceTRANSLATIONS->GetString(ChoiceTRANSLATIONS->GetSelection());
  NAVIGATION  = ChoiceNAVIGATION->GetString(ChoiceNAVIGATION->GetSelection());
  HAPTIC = CheckBoxHAPTIC->GetValue();
  PPM_UNIT  = ChoicePPM_UNIT->GetString(ChoicePPM_UNIT->GetSelection());
  GPS = CheckBoxGPS->GetValue();
  VARIO = CheckBoxVARIO->GetValue();
  PPM = CheckBoxPPM->GetValue();
  SERIAL_PROTOCOL = ChoiceSERIAL_PROTOCOL->GetString(ChoiceSERIAL_PROTOCOL->GetSelection());
  PCM_PROTOCOL = ChoicePCM_PROTOCOL->GetString(ChoicePCM_PROTOCOL->GetSelection());
  CC2500 = CheckBoxCC2500->GetValue();
  CYRF6936 = CheckBoxCYRF6936->GetValue();
  NRF24l01 = CheckBoxNRF24l01->GetValue();
  A7105 = CheckBoxA7105->GetValue();
  CC2500PAG = ChoiceCC2500PAG->GetString(ChoiceCC2500PAG->GetSelection());
  CYRF6936PAG = ChoiceCYRF6936PAG->GetString(ChoiceCYRF6936PAG->GetSelection());
  NRF24l01PAG = ChoiceNRF24l01PAG->GetString(ChoiceNRF24l01PAG->GetSelection());
  A7105PAG = ChoiceA7105PAG->GetString(ChoiceA7105PAG->GetSelection());
  SX1276 = CheckBoxSX1276->GetValue();

  SD_CARD = CheckBoxSD_CARD->GetValue();
  TEMPLATES = CheckBoxTEMPLATES->GetValue();
  THREE_POS = ChoiceTHREE_POS->GetString(ChoiceTHREE_POS->GetSelection());// defaults to NO
  SPLASH = CheckBoxSPLASH->GetValue();
  UNITS = ChoiceUNITS->GetString(ChoiceUNITS->GetSelection());
  DEFAULT_MODE = ChoiceDEFAULT_MODE->GetString(ChoiceDEFAULT_MODE->GetSelection());
  FONT = ChoiceFONT->GetString(ChoiceFONT->GetSelection());
  BOLD = CheckBoxBOLD->GetValue();
  BATTGRAPH = CheckBoxBATTGRAPH->GetValue();
  EEPROM_PROGRESS_BAR = CheckBoxEEPROM_PROGRESS_BAR->GetValue();
  FAI = ChoiceFAI->GetString(ChoiceFAI->GetSelection());
  AUTOSWITCH = CheckBoxAUTOSWITCH->GetValue();
  AUTOSOURCE = CheckBoxAUTOSOURCE->GetValue();
  DBLKEYS = CheckBoxDBLKEYS->GetValue();
  PPM_CENTER_ADJUSTABLE = CheckBoxPPM_CENTER_ADJUSTABLE->GetValue();
  PPM_LIMITS_SYMETRICAL = CheckBoxPPM_LIMITS_SYMETRICAL->GetValue();
  FLIGHT_MODES = CheckBoxFLIGHT_MODES->GetValue();
  CURVES = CheckBoxCURVES->GetValue();
  GVARS = CheckBoxGVARS->GetValue();
  CORRECT_NEGATIVE_VALUES = CheckBoxCORRECT_NEGATIVE_VALUES->GetValue();
  ARITHMETIC_OVERFLOW_CHECK = CheckBoxARITHMETIC_OVERFLOW_CHECK->GetValue();
  ACCURAT_THROTTLE_STATS = CheckBoxACCURAT_THROTTLE_STATS->GetValue();
  PWM_BACKLIGHT = CheckBoxPWM_BACKLIGHT->GetValue();
  OVERRIDE_CHANNEL_FUNCTION = CheckBoxOVERRIDE_CHANNEL_FUNCTION->GetValue();
  WS_HOW_HIGH = CheckBoxWS_HOW_HIGH->GetValue();
  TOGGLETRIM = CheckBoxTOGGLETRIM->GetValue();
  NOANDSECONDE = CheckBoxNOANDSECONDE->GetValue();
  SHUTDOWN_CONFIRMATION = CheckBoxSHUTDOWN_CONFIRMATION->GetValue();
  FRAM = CheckBoxFRAM->GetValue();
  BLUETOOTH = ChoiceBLUETOOTH->GetString(ChoiceBLUETOOTH->GetSelection());
  XMODEM = CheckBoxXMODEM->GetValue();
  PERSONAMES = CheckBoxPERSONAMES->GetValue();
  DBLGAZSTICK = CheckBoxDBLGAZSTICK->GetValue();
  OTHERCOMPOPTIONS = TextCtrlOTHERCOMPOPTIONS->GetValue();
  INV_STICK_RH = CheckBoxInvStickRH->GetValue();
  INV_STICK_LV = CheckBoxInvStickLV->GetValue();
  INV_STICK_RV = CheckBoxInvStickRV->GetValue();
  INV_STICK_LH = CheckBoxInvStickLH->GetValue();

  //X-Any
  NUMXANY = ChoiceNUMXANY->GetString(ChoiceNUMXANY->GetSelection());


  switch1  = ComboBoxswitch1->GetValue();
  switch2  = ComboBoxswitch2->GetValue();
  switch3  = ComboBoxswitch3->GetValue();
  switch4  = ComboBoxswitch4->GetValue();
  switch5  = ComboBoxswitch5->GetValue();
  switch6  = ComboBoxswitch6->GetValue();
  switchID0 = ComboBoxID0->GetValue();
  switchID1 = ComboBoxID1->GetValue();
  switchID2 = ComboBoxID2->GetValue();
  encoderA  = ComboBoxEncodeurA->GetValue();
  encoderB  = ComboBoxEncodeurB->GetValue();
  encoderNo  = ComboBoxEncodeurNo->GetValue();
  switchXD0 = ComboBoxXD0->GetValue();
  switchXD1 = ComboBoxXD1->GetValue();
  switchXD2 = ComboBoxXD2->GetValue();
  stickRud = ComboBoxRud->GetValue();
  stickEle = ComboBoxEle->GetValue();
  stickThr = ComboBoxThr->GetValue();
  stickAil = ComboBoxAil->GetValue();

  wxString switchArray[] = {switch1, switch2, switch3, switch4, switch5, switch6, switchID0, switchID1, switchID2, encoderA, encoderB, encoderNo, switchXD0, switchXD1, switchXD2, stickAil, stickEle, stickRud, stickThr};

  for (uint16_t i=0; i<19; i++)
  {
     std::string  x(switchArray[i]);
     if (x.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_-+:*=?") != std::string::npos)
     {
        doNotClose = 1;
        wxMessageBox(switchArray[i]+":\n"+_(": Il y a au moins ce nom avec des caractères spéciaux non autorisés.\nVeuillez le changer.\nCaractères spéciaux autorisés : _-+:*=?"));
        break;
     }

     if (switchArray[i].length() < 3)
     {
        doNotClose = 1;
        wxMessageBox(switchArray[i]+":\n"+_(": Il y a au moins ce nom trop court, veuillez le compléter."));
        break;
     }

     for (uint16_t j=0; j<15; j++)
     {
        if (i == j) continue;
        if (switchArray[i] == switchArray[j])
        {
           doNotClose = 1;
           wxMessageBox(switchArray[i]+":\n"+_(": Il y a au moins ce nom répété , veuillez le changer."));
           break;
        }
     }
  if (doNotClose) break;
  }
}

void CompilerOptionsFrame::OnButtonEXITClick(wxCommandEvent& event)
{
  CollectDatas();
  Ini_Changed = true;
  if (!doNotClose) Close();
}

uint8_t invertByte(uint8_t a)
{
	a = ((a & 0xaa) >>  1) | ((a & 0x55) <<  1);
	a = ((a & 0xcc) >>  2) | ((a & 0x33) <<  2);
	a = ((a & 0xf0) >>  4) | ((a & 0x0f) <<  4);
	return a;
}

void CompilerOptionsFrame::WriteSplashFile()
{
  wxString data;
  wxString line;
  wxTextFile Splashfile( LCDROT180? AppPath + "\\sources\\bitmaps\\splash.lbmi": AppPath + "\\sources\\bitmaps\\splash.lbm");
  Splashfile.Open();
  Splashfile.Clear();
  line = "";
  data.Printf("%u",LbmSplash[0]);
  line += data.ToUTF8();
  line += ",";
  data.Printf("%u",LbmSplash[1]);
  line += data.ToUTF8();
  line += ",";
  Splashfile.AddLine(line,wxTextFileType_Dos);

  for (uint16_t i=0; i<(LCD_H/8); ++i)
    {
      line = "";
      for (uint16_t j=0; j<(LCD_W); ++j)
        {
          if (LCDROT180) // Inverted LCD Splash
          {
            data.Printf("0x%02x",invertByte(LbmSplash[2+(i*LCD_W)+j]));
          }
          else
          {
            data.Printf("0x%02x",LbmSplash[2+(i*LCD_W)+j]);
          }
          line += data.ToUTF8();
          line += ",";
        }
      Splashfile.AddLine(line,wxTextFileType_Dos);
    }
  Splashfile.Write();
  Splashfile.Close();
}


void CompilerOptionsFrame::CreateCompileBatFile(wxString line4)
{
  wxString Filename = AppPath + "\\CompileBatFile.bat";
  wxTextFile CompileBatFile(Filename);

  if(CompileBatFile.Exists()) CompileBatFile.Open(Filename);
  else CompileBatFile.Create(Filename);

  CompileBatFile.Open();
  CompileBatFile.Clear();
  CompileBatFile.AddLine("cd sources");
  CompileBatFile.AddLine("Path " + AppPath + "\\AVR-GCC\\bin;"+ AppPath + "\\AVR-GCC\\utils\\bin");
  CompileBatFile.AddLine("make clean");
  CompileBatFile.AddLine(line4);
  CompileBatFile.AddLine("copy OpenAVRc.hex ..\\firmware\\OpenAVRc.hex");
  CompileBatFile.AddLine("pause");
  CompileBatFile.Write();
  CompileBatFile.Close();
}

void CompilerOptionsFrame::CreatePersonames_H()
{
  wxString g = "\"";
  wxString Filename = AppPath + "\\sources\\personames.h";
  wxTextFile PersoNameFile(Filename);

  if(PersoNameFile.Exists()) PersoNameFile.Open(Filename);
  else PersoNameFile.Create(Filename);

  PersoNameFile.Open();
  PersoNameFile.Clear();
  PersoNameFile.AddLine("#ifndef PERSONAMES_H_INCLUDED");
  PersoNameFile.AddLine("#define PERSONAMES_H_INCLUDED");
  PersoNameFile.AddLine("#undef TR_PHYS_SWITCHES");
  PersoNameFile.AddLine("#define TR_PHYS_SWITCHES "+g+switch1+g+g+switch2+g+g+switch3+g+g+switch4+g+g+switch5+g+g+switch6+g);
  PersoNameFile.AddLine("#undef TR_9X_3POS_SWITCHES");
  PersoNameFile.AddLine("#define TR_9X_3POS_SWITCHES "+g+switchID0+g+g+switchID1+g+g+switchID2+g);
  PersoNameFile.AddLine("#undef TR_ROTARY_ENCODERS");
  PersoNameFile.AddLine("#define TR_ROTARY_ENCODERS "+g+encoderA+"\\"+"0"+g+g+encoderB+"\\"+"0"+g);
  PersoNameFile.AddLine("#undef TR_ROTENC_SWITCHES");
  PersoNameFile.AddLine("#define TR_ROTENC_SWITCHES "+g+encoderA.Upper()+g+g+encoderB.Upper()+g+g+encoderNo.Upper()+g);
  PersoNameFile.AddLine("#undef TR_EXTRA_3POS_SWITCHES");
  PersoNameFile.AddLine("#define TR_EXTRA_3POS_SWITCHES "+g+switchXD0+g+g+switchXD1+g+g+switchXD2+g);
  PersoNameFile.AddLine("#undef TR_VMIXTRIMS");
  PersoNameFile.AddLine("#define TR_VMIXTRIMS "+g+"OFF"+g+g+"ON"+"\\"+"0"+g+g+stickRud+g+g+stickEle+g+g+stickThr+g+g+stickAil+g);
  PersoNameFile.AddLine("#undef TR_STICKS_VSRCRAW");
  PersoNameFile.AddLine("#define TR_STICKS_VSRCRAW "+g+stickRud+"\\"+"0"+g+g+stickEle+"\\"+"0"+g+g+stickThr+"\\"+"0"+g+g+stickAil+"\\"+"0"+g);
  PersoNameFile.AddLine("#endif");
  PersoNameFile.Write();
  PersoNameFile.Close();
}

void CompilerOptionsFrame::OnCheckBoxAUTOSOURCEClick1(wxCommandEvent& event)//pas compatible avec navigation=sticks
{
  CollectDatas();
  if ((NAVIGATION == wxT("STICKS")) & (AUTOSOURCE))
    {
      wxMessageBox(_("Non compatible avec NAVIGATION = STICKS"));
      CheckBoxAUTOSOURCE->SetValue(0);
    }
}

void CompilerOptionsFrame::OnChoiceNAVIGATIONSelect(wxCommandEvent& event)
{
  CollectDatas();
  if ((AUTOSOURCE) & (NAVIGATION == wxT("STICKS")))
    {
      wxMessageBox(_("Non compatible avec AUTOSOURCE. AUTOSOURCE décoché."));
      CheckBoxAUTOSOURCE->SetValue(0);
    }
}

void CompilerOptionsFrame::OnChoiceVOICESelect(wxCommandEvent& event)
{
  CollectDatas();
  if (VOICE == ("NO"))
    {
      CheckBoxNOANDSECONDE->SetValue(0);
      Panel3->Refresh ();
    }

}

void CompilerOptionsFrame::OnCheckBoxNOANDSECONDEClick1(wxCommandEvent& event)
{
  CollectDatas();
  if (VOICE == ("NO"))
    {
      wxMessageBox("Non compatible avec VOICE=NO.");
      CheckBoxAUTOSOURCE->SetValue(0);
    }
}


void CompilerOptionsFrame::OnChoiceEXTSelect2(wxCommandEvent& event)
{
  CollectDatas();
  if (EXT == ("STD"))
    {
      CheckBoxGPS->SetValue(0);
      CheckBoxVARIO->SetValue(0);
      CheckBoxWS_HOW_HIGH->SetValue(0);
    }
}

void CompilerOptionsFrame::OnCheckBoxGPSClick1(wxCommandEvent& event)
{
  CollectDatas();
  if (EXT == ("STD"))
    {
      wxMessageBox(_("Non compatible avec TELEMETRY = STD."));
      CheckBoxGPS->SetValue(0);
    }
}

void CompilerOptionsFrame::OnCheckBoxVARIOClick2(wxCommandEvent& event)
{
  CollectDatas();
  if (EXT == ("STD"))
    {
      wxMessageBox(_("Non compatible avec TELEMETRY = STD."));
      CheckBoxVARIO->SetValue(0);
    }
}

void CompilerOptionsFrame::OnCheckBoxWS_HOW_HIGHClick2(wxCommandEvent& event)
{
  CollectDatas();
  if (EXT == ("STD"))
    {
      wxMessageBox(_("Non compatible avec TELEMETRY = STD."));
      CheckBoxWS_HOW_HIGH->SetValue(0);
    }
}

void CompilerOptionsFrame::OnCheckBoxPERSONAMESClick(wxCommandEvent& event)
{
  CollectDatas();
}


void CompilerOptionsFrame::OnCheckBoxSD_CARDClick(wxCommandEvent& event)
{
  CollectDatas();
  if (XMODEM)
    {
      CheckBoxXMODEM->SetValue(0);
    }
}

void CompilerOptionsFrame::OnCheckBoxXMODEMClick(wxCommandEvent& event)
{
  CollectDatas();
  if (XMODEM && (!SD_CARD))
    {
      wxMessageBox(_("Necessite SD_CARD"));
      CheckBoxXMODEM->SetValue(0);
    }
}

void CompilerOptionsFrame::OnButtonDefautClick1(wxCommandEvent& event)
{
  ComboBoxswitch1->SetSelection(0);
  ComboBoxswitch2->SetSelection(0);
  ComboBoxswitch3->SetSelection(0);
  ComboBoxswitch4->SetSelection(0);
  ComboBoxswitch5->SetSelection(0);
  ComboBoxswitch6->SetSelection(0);
  ComboBoxID0->SetSelection(0);
  ComboBoxID1->SetSelection(0);
  ComboBoxID2->SetSelection(0);
  ComboBoxEncodeurA->SetSelection(0);
  ComboBoxEncodeurB->SetSelection(0);
  ComboBoxEncodeurNo->SetSelection(0);
  ComboBoxXD0->SetSelection(0);
  ComboBoxXD1->SetSelection(0);
  ComboBoxXD2->SetSelection(0);
  ComboBoxRud->SetSelection(0);
  ComboBoxEle->SetSelection(0);
  ComboBoxThr->SetSelection(0);
  ComboBoxAil->SetSelection(0);
}
