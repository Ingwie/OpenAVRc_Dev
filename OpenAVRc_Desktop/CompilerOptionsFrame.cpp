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


#include "CompilerOptionsFrame.h"
#include "OpenAVRc_DesktopMain.h"
#include "CommunicationsFrame.h"
#include <wx/msgdlg.h>
#include <wx/filedlg.h>


extern wxString AppPath;
extern bool Ini_Changed;
extern bool doNotClose = 0;


//(*InternalHeaders(CompilerOptionsFrame)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)


//(*IdInit(CompilerOptionsFrame)
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
const long CompilerOptionsFrame::ID_CHECKBOX9 = wxNewId();
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
const long CompilerOptionsFrame::ID_PANEL2 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX3 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX2 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX1 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX10 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX11 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX13 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE2 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX2 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX3 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX4 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX5 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX6 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX8 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX23 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX24 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX25 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX26 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX27 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX29 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX30 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX31 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX32 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX34 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX36 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX37 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX38 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX39 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX43 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX44 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX45 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX47 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX28 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON1 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON4 = wxNewId();
const long CompilerOptionsFrame::ID_PANEL3 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX8 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX7 = wxNewId();
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
const long CompilerOptionsFrame::ID_STATICTEXT19 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX7 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT20 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT21 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX8 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX9 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX10 = wxNewId();
const long CompilerOptionsFrame::ID_COMBOBOX11 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT22 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT23 = wxNewId();
const long CompilerOptionsFrame::ID_PANEL4 = wxNewId();
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
  SetClientSize(wxSize(807,377));
  SetToolTip(_("Référence de l\'écran"));
  SetHelpText(_("Protocoles DSM"));
  Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(0,0), wxSize(752,327), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
  Panel1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
  Notebook1 = new wxNotebook(Panel1, ID_NOTEBOOK1, wxPoint(0,0), wxSize(808,376), 0, _T("ID_NOTEBOOK1"));
  Panel2 = new wxPanel(Notebook1, ID_PANEL2, wxDefaultPosition, wxSize(802,302), wxTAB_TRAVERSAL, _T("ID_PANEL2"));
  StaticBox6 = new wxStaticBox(Panel2, ID_STATICBOX6, _("Autres options"), wxPoint(464,8), wxSize(192,328), 0, _T("ID_STATICBOX6"));
  StaticBox4 = new wxStaticBox(Panel2, ID_STATICBOX4, _("Matériel"), wxPoint(24,8), wxSize(200,328), 0, _T("ID_STATICBOX4"));
  CheckBoxPPM_LIMITS_SYMETRICAL = new wxCheckBox(Panel2, ID_CHECKBOX17, _("PPM Simm limits"), wxPoint(528,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX17"));
  CheckBoxPPM_LIMITS_SYMETRICAL->SetValue(true);
  CheckBoxPPM_LIMITS_SYMETRICAL->SetToolTip(_("Option ajuster le gain fonction du subtrim"));
  StaticBox5 = new wxStaticBox(Panel2, ID_STATICBOX5, _("Interface"), wxPoint(232,8), wxSize(224,328), 0, _T("ID_STATICBOX5"));
  StaticText4 = new wxStaticText(Panel2, ID_STATICTEXT4, _("Unités"), wxPoint(264,304), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
  CheckBoxPPM_CENTER_ADJUSTABLE = new wxCheckBox(Panel2, ID_CHECKBOX16, _("PPM Center adjust"), wxPoint(528,272), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX16"));
  CheckBoxPPM_CENTER_ADJUSTABLE->SetValue(true);
  CheckBoxPPM_CENTER_ADJUSTABLE->SetToolTip(_("Option réglage des neutres indépendant"));
  ChoicePPM_UNIT  = new wxChoice(Panel2, ID_CHOICE4, wxPoint(336,296), wxSize(112,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE4"));
  ChoicePPM_UNIT ->SetSelection( ChoicePPM_UNIT ->Append(_("PERCENT_PREC1")) );
  ChoicePPM_UNIT ->Append(_("PERCENT_PREC0"));
  ChoicePPM_UNIT ->Append(_("US"));
  ChoicePPM_UNIT ->SetToolTip(_("Affichage des valeurs des voies en % ou uS"));
  ChoicePCB = new wxChoice(Panel2, ID_CHOICE6, wxPoint(112,32), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE6"));
  ChoicePCB->SetSelection( ChoicePCB->Append(_("MEGA2560")) );
  ChoicePCB->Append(_("9X"));
  ChoicePCB->Append(_("9X128"));
  ChoicePCB->Append(_("9X2561"));
  ChoicePCB->Append(_("9XR"));
  ChoicePCB->Append(_("9XR128"));
  ChoicePCB->Append(_("9XR2561"));
  ChoicePCB->Append(_("GRUVIN9X"));
  ChoicePCB->SetToolTip(_("Type de radio"));
  ChoiceLCD = new wxChoice(Panel2, ID_CHOICE1, wxPoint(112,72), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
  ChoiceLCD->Append(_("DEFAULT"));
  ChoiceLCD->Append(_("ST7565P"));
  ChoiceLCD->Append(_("ST7565R "));
  ChoiceLCD->Append(_("ERC12864FSF"));
  ChoiceLCD->SetSelection( ChoiceLCD->Append(_("ST7920")) );
  ChoiceLCD->Append(_("KS108"));
  ChoiceLCD->Append(_("SSD1306"));
  ChoiceLCD->SetToolTip(_("Référence de l\'écran"));
  StaticText2 = new wxStaticText(Panel2, ID_STATICTEXT2, _("Processeur"), wxPoint(32,40), wxSize(64,13), 0, _T("ID_STATICTEXT2"));
  StaticText1 = new wxStaticText(Panel2, ID_STATICTEXT1, _("LCD"), wxPoint(64,80), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
  ChoiceVOICE = new wxChoice(Panel2, ID_CHOICE3, wxPoint(112,120), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
  ChoiceVOICE->SetSelection( ChoiceVOICE->Append(_("NO")) );
  ChoiceVOICE->Append(_("YES"));
  ChoiceVOICE->Append(_("WTV20SD"));
  ChoiceVOICE->Append(_("JQ6500"));
  ChoiceVOICE->SetToolTip(_("Option annonces vocales"));
  StaticText3 = new wxStaticText(Panel2, ID_STATICTEXT3, _("Voice"), wxPoint(56,128), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
  CheckBoxAUDIO = new wxCheckBox(Panel2, ID_CHECKBOX7, _("Audio"), wxPoint(112,160), wxSize(48,16), 0, wxDefaultValidator, _T("ID_CHECKBOX7"));
  CheckBoxAUDIO->SetValue(false);
  CheckBoxAUDIO->SetToolTip(_("Option audio"));
  CheckBoxHAPTIC = new wxCheckBox(Panel2, ID_CHECKBOX14, _("Haptic"), wxPoint(112,192), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX14"));
  CheckBoxHAPTIC->SetValue(false);
  CheckBoxHAPTIC->SetToolTip(_("Option Vibreur"));
  CheckBoxHELI = new wxCheckBox(Panel2, ID_CHECKBOX1, _("HELI"), wxPoint(528,152), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
  CheckBoxHELI->SetValue(false);
  CheckBoxHELI->SetToolTip(_("Option hélicoptère"));
  CheckBoxTEMPLATES = new wxCheckBox(Panel2, ID_CHECKBOX15, _("TEMPLATES"), wxPoint(528,176), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX15"));
  CheckBoxTEMPLATES->SetValue(false);
  CheckBoxTEMPLATES->SetToolTip(_("Option modèles pré programmés"));
  CheckBoxRTCLOCK = new wxCheckBox(Panel2, ID_CHECKBOX9, _("RTC Clock"), wxPoint(112,248), wxSize(80,21), 0, wxDefaultValidator, _T("ID_CHECKBOX9"));
  CheckBoxRTCLOCK->SetValue(false);
  CheckBoxRTCLOCK->SetToolTip(_("Option horloge temps réel"));
  CheckBoxSD_CARD = new wxCheckBox(Panel2, ID_CHECKBOX12, _("Carte SD"), wxPoint(112,224), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX12"));
  CheckBoxSD_CARD->SetValue(false);
  CheckBoxSD_CARD->SetToolTip(_("Option carte SD (Sauvegarde - Fichiers log)"));
  ChoiceTRANSLATIONS = new wxChoice(Panel2, ID_CHOICE7, wxPoint(336,32), wxSize(72,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE7"));
  ChoiceTRANSLATIONS->Append(_("CZ"));
  ChoiceTRANSLATIONS->Append(_("DE"));
  ChoiceTRANSLATIONS->Append(_("EN"));
  ChoiceTRANSLATIONS->SetSelection( ChoiceTRANSLATIONS->Append(_("ES")) );
  ChoiceTRANSLATIONS->Append(_("FR"));
  ChoiceTRANSLATIONS->Append(_("HU"));
  ChoiceTRANSLATIONS->Append(_("IT"));
  ChoiceTRANSLATIONS->Append(_("NL"));
  ChoiceTRANSLATIONS->Append(_("PL"));
  ChoiceTRANSLATIONS->Append(_("PT"));
  ChoiceTRANSLATIONS->Append(_("SE"));
  ChoiceTRANSLATIONS->Append(_("SK"));
  ChoiceTRANSLATIONS->SetToolTip(_("Langue affichée"));
  StaticText6 = new wxStaticText(Panel2, ID_STATICTEXT6, _("Langue"), wxPoint(264,40), wxDefaultSize, 0, _T("ID_STATICTEXT6"));
  ChoiceTTS = new wxChoice(Panel2, ID_CHOICE5, wxPoint(336,72), wxSize(72,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE5"));
  ChoiceTTS->Append(_("CZ"));
  ChoiceTTS->Append(_("DE"));
  ChoiceTTS->Append(_("EN"));
  ChoiceTTS->SetSelection( ChoiceTTS->Append(_("ES")) );
  ChoiceTTS->Append(_("FR"));
  ChoiceTTS->Append(_("HU"));
  ChoiceTTS->Append(_("IT"));
  ChoiceTTS->Append(_("NL"));
  ChoiceTTS->Append(_("PL"));
  ChoiceTTS->Append(_("PT"));
  ChoiceTTS->Append(_("SE"));
  ChoiceTTS->Append(_("SK"));
  ChoiceTTS->SetToolTip(_("Langue de la synthése vocale"));
  StaticText5 = new wxStaticText(Panel2, ID_STATICTEXT5, _("Langue voice"), wxPoint(240,80), wxSize(80,13), 0, _T("ID_STATICTEXT5"));
  ChoiceNAVIGATION = new wxChoice(Panel2, ID_CHOICE8, wxPoint(336,248), wxSize(80,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE8"));
  ChoiceNAVIGATION->SetSelection( ChoiceNAVIGATION->Append(_("NO")) );
  ChoiceNAVIGATION->Append(_("POT1"));
  ChoiceNAVIGATION->Append(_("POT2"));
  ChoiceNAVIGATION->Append(_("POT3"));
  ChoiceNAVIGATION->Append(_("POTS"));
  ChoiceNAVIGATION->Append(_("STICKS"));
  ChoiceNAVIGATION->Append(_("ROTENC"));
  ChoiceNAVIGATION->SetToolTip(_("Peut remplacer les boutons de navigation"));
  StaticText7 = new wxStaticText(Panel2, ID_STATICTEXT7, _("Navigation"), wxPoint(248,256), wxDefaultSize, 0, _T("ID_STATICTEXT7"));
  ButtonEXIT = new wxButton(Panel2, ID_BUTTON3, _("Sauvegarder et sortir"), wxPoint(664,272), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
  ButtonCOMPILE = new wxButton(Panel2, ID_BUTTON2, _("Compiler"), wxPoint(704,208), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
  ChoiceUNITS = new wxChoice(Panel2, ID_CHOICE9, wxPoint(528,32), wxSize(64,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE9"));
  ChoiceUNITS->SetSelection( ChoiceUNITS->Append(_("METRIC")) );
  ChoiceUNITS->Append(_("IMPERIAL"));
  ChoiceUNITS->SetToolTip(_("Unitées"));
  StaticText8 = new wxStaticText(Panel2, ID_STATICTEXT8, _("Unités"), wxPoint(480,40), wxDefaultSize, 0, _T("ID_STATICTEXT8"));
  ChoiceDEFAULT_MODE = new wxChoice(Panel2, ID_CHOICE10, wxPoint(528,72), wxSize(64,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE10"));
  ChoiceDEFAULT_MODE->SetSelection( ChoiceDEFAULT_MODE->Append(_("NO")) );
  ChoiceDEFAULT_MODE->Append(_("1"));
  ChoiceDEFAULT_MODE->Append(_("2"));
  ChoiceDEFAULT_MODE->Append(_("3"));
  ChoiceDEFAULT_MODE->Append(_("4"));
  ChoiceDEFAULT_MODE->SetToolTip(_("Mode de la radio"));
  StaticText9 = new wxStaticText(Panel2, ID_STATICTEXT9, _("Mode"), wxPoint(480,80), wxDefaultSize, 0, _T("ID_STATICTEXT9"));
  CheckBoxFLIGHT_MODES = new wxCheckBox(Panel2, ID_CHECKBOX18, _("Flight modes"), wxPoint(528,200), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX18"));
  CheckBoxFLIGHT_MODES->SetValue(true);
  CheckBoxFLIGHT_MODES->SetToolTip(_("Option phases de vol"));
  CheckBoxCURVES = new wxCheckBox(Panel2, ID_CHECKBOX19, _("Curves"), wxPoint(528,248), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX19"));
  CheckBoxCURVES->SetValue(true);
  CheckBoxCURVES->SetToolTip(_("Option courbes"));
  CheckBoxGVARS = new wxCheckBox(Panel2, ID_CHECKBOX20, _("Global vars"), wxPoint(528,224), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX20"));
  CheckBoxGVARS->SetValue(true);
  CheckBoxGVARS->SetToolTip(_("Option variables globales"));
  CheckBoxSPLASH = new wxCheckBox(Panel2, ID_CHECKBOX21, _("Écran d\'accueil"), wxPoint(336,216), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX21"));
  CheckBoxSPLASH->SetValue(false);
  CheckBoxSPLASH->SetToolTip(_("Ecran d\'accueil"));
  ChoiceFONT = new wxChoice(Panel2, ID_CHOICE11, wxPoint(336,176), wxSize(80,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE11"));
  ChoiceFONT->SetSelection( ChoiceFONT->Append(_("STD")) );
  ChoiceFONT->Append(_("SQT5"));
  ChoiceFONT->SetToolTip(_("Police de caractére"));
  StaticText10 = new wxStaticText(Panel2, ID_STATICTEXT10, _("Font"), wxPoint(272,184), wxDefaultSize, 0, _T("ID_STATICTEXT10"));
  CheckBoxBOLD = new wxCheckBox(Panel2, ID_CHECKBOX40, _("Bold"), wxPoint(336,112), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX40"));
  CheckBoxBOLD->SetValue(false);
  CheckBoxBOLD->SetToolTip(_("Vrai police en gras"));
  CheckBoxBATTGRAPH = new wxCheckBox(Panel2, ID_CHECKBOX41, _("Batt graph"), wxPoint(336,144), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX41"));
  CheckBoxBATTGRAPH->SetValue(false);
  CheckBoxBATTGRAPH->SetToolTip(_("Affichage graphique de l\'état de la batterie"));
  ChoiceFAI = new wxChoice(Panel2, ID_CHOICE13, wxPoint(528,112), wxSize(64,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE13"));
  ChoiceFAI->SetSelection( ChoiceFAI->Append(_("NO")) );
  ChoiceFAI->Append(_("YES"));
  ChoiceFAI->Append(_("CHOICE"));
  ChoiceFAI->SetToolTip(_("Pour les compétitions"));
  StaticText12 = new wxStaticText(Panel2, ID_STATICTEXT12, _("FAI"), wxPoint(488,120), wxDefaultSize, 0, _T("ID_STATICTEXT12"));
  ChoiceTHREE_POS = new wxChoice(Panel2, ID_CHOICE12, wxPoint(112,288), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE12"));
  ChoiceTHREE_POS->SetSelection( ChoiceTHREE_POS->Append(_("NO")) );
  ChoiceTHREE_POS->Append(_("POT1"));
  ChoiceTHREE_POS->Append(_("POT2"));
  ChoiceTHREE_POS->Append(_("POT3"));
  StaticText11 = new wxStaticText(Panel2, ID_STATICTEXT11, _("3POS"), wxPoint(64,296), wxDefaultSize, 0, _T("ID_STATICTEXT11"));
  Panel3 = new wxPanel(Notebook1, ID_PANEL3, wxDefaultPosition, wxSize(800,356), wxTAB_TRAVERSAL, _T("ID_PANEL3"));
  StaticBox3 = new wxStaticBox(Panel3, ID_STATICBOX3, _("Autres paramètres"), wxPoint(328,16), wxSize(464,264), 0, _T("ID_STATICBOX3"));
  StaticBox2 = new wxStaticBox(Panel3, ID_STATICBOX2, _("Télemetrie"), wxPoint(184,16), wxSize(136,312), 0, _T("ID_STATICBOX2"));
  StaticBox1 = new wxStaticBox(Panel3, ID_STATICBOX1, _("Protocole"), wxPoint(8,16), wxSize(168,264), 0, _T("ID_STATICBOX1"));
  CheckBoxPPM = new wxCheckBox(Panel3, ID_CHECKBOX10, _("PPM"), wxPoint(72,56), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX10"));
  CheckBoxPPM->SetValue(true);
  CheckBoxPPM->SetToolTip(_("Protocole PPM 8 à 16 voies"));
  CheckBoxPXX = new wxCheckBox(Panel3, ID_CHECKBOX11, _("PXX"), wxPoint(72,88), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX11"));
  CheckBoxPXX->SetValue(false);
  CheckBoxPXX->Disable();
  CheckBoxPXX->SetToolTip(_("Protocole PXX "));
  CheckBoxDSM2SERIAL = new wxCheckBox(Panel3, ID_CHECKBOX13, _("DSM2 SERIAL"), wxPoint(72,120), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX13"));
  CheckBoxDSM2SERIAL->SetValue(false);
  CheckBoxDSM2SERIAL->SetToolTip(_("Protocoles DSM .."));
  ChoiceEXT = new wxChoice(Panel3, ID_CHOICE2, wxPoint(208,40), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
  ChoiceEXT->SetSelection( ChoiceEXT->Append(_("STD")) );
  ChoiceEXT->Append(_("FRSKY"));
  ChoiceEXT->Append(_("FRSKY_SPORT"));
  ChoiceEXT->Append(_("JETI"));
  ChoiceEXT->Append(_("NMEA"));
  ChoiceEXT->Append(_("ARDUPILOT"));
  ChoiceEXT->Append(_("MAVLINK"));
  ChoiceEXT->Append(_("TELEMETREZ"));
  ChoiceEXT->SetToolTip(_("Télémétrie installée"));
  CheckBoxFRSKY_HUB = new wxCheckBox(Panel3, ID_CHECKBOX2, _("FrSky Hub"), wxPoint(208,88), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
  CheckBoxFRSKY_HUB->SetValue(false);
  CheckBoxFAS_OFFSET = new wxCheckBox(Panel3, ID_CHECKBOX3, _("FAS Offset"), wxPoint(208,120), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
  CheckBoxFAS_OFFSET->SetValue(false);
  CheckBoxGAUGES = new wxCheckBox(Panel3, ID_CHECKBOX4, _("Gauges"), wxPoint(208,152), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
  CheckBoxGAUGES->SetValue(false);
  CheckBoxGPS = new wxCheckBox(Panel3, ID_CHECKBOX5, _("GPS"), wxPoint(208,184), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
  CheckBoxGPS->SetValue(false);
  CheckBoxVARIO = new wxCheckBox(Panel3, ID_CHECKBOX6, _("Vario"), wxPoint(208,216), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX6"));
  CheckBoxVARIO->SetValue(false);
  CheckBoxSPORT_FILE_LOG = new wxCheckBox(Panel3, ID_CHECKBOX8, _("Sport file log"), wxPoint(208,248), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX8"));
  CheckBoxSPORT_FILE_LOG->SetValue(false);
  CheckBoxPCBREV = new wxCheckBox(Panel3, ID_CHECKBOX23, _("PCB Rev0"), wxPoint(656,184), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX23"));
  CheckBoxPCBREV->SetValue(false);
  CheckBoxPCBREV->SetToolTip(_("REV1 pour gruvin9x si pas coché"));
  CheckBoxTURNIGY_TRANSMITTER_FIX = new wxCheckBox(Panel3, ID_CHECKBOX24, _("TURNIGY Fix"), wxPoint(656,88), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX24"));
  CheckBoxTURNIGY_TRANSMITTER_FIX->SetValue(false);
  CheckBoxFRSKY_STICKS = new wxCheckBox(Panel3, ID_CHECKBOX25, _("FRSKY_Sticks"), wxPoint(656,120), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX25"));
  CheckBoxFRSKY_STICKS->SetValue(false);
  CheckBoxCORRECT_NEGATIVE_VALUES = new wxCheckBox(Panel3, ID_CHECKBOX26, _("CORRECT_NEGATIVE_VALUES"), wxPoint(456,152), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX26"));
  CheckBoxCORRECT_NEGATIVE_VALUES->SetValue(false);
  CheckBoxARITHMETIC_OVERFLOW_CHECK = new wxCheckBox(Panel3, ID_CHECKBOX27, _("ARITHMETIC_OVERFLOW_CHECK"), wxPoint(456,184), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX27"));
  CheckBoxARITHMETIC_OVERFLOW_CHECK->SetValue(false);
  CheckBoxARITHMETIC_OVERFLOW_CHECK->SetToolTip(_("Ajoute du code pour prèvoir débordement aritmethique"));
  CheckBoxSP22 = new wxCheckBox(Panel3, ID_CHECKBOX29, _("SP22"), wxPoint(656,56), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX29"));
  CheckBoxSP22->SetValue(false);
  CheckBoxSP22->SetToolTip(_("Smartie Parts Backlight"));
  CheckBoxPWM_BACKLIGHT = new wxCheckBox(Panel3, ID_CHECKBOX30, _("PWM_BACKLIGHT"), wxPoint(456,56), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX30"));
  CheckBoxPWM_BACKLIGHT->SetValue(false);
  CheckBoxOVERRIDE_CHANNEL_FUNCTION = new wxCheckBox(Panel3, ID_CHECKBOX31, _("Override channel function"), wxPoint(456,88), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX31"));
  CheckBoxOVERRIDE_CHANNEL_FUNCTION->SetValue(false);
  CheckBoxWS_HOW_HIGH = new wxCheckBox(Panel3, ID_CHECKBOX32, _("WS_HOW_HIGH"), wxPoint(208,280), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX32"));
  CheckBoxWS_HOW_HIGH->SetValue(false);
  CheckBoxWS_HOW_HIGH->SetToolTip(_("WS HowHigh Altimeter"));
  CheckBoxTX_CADDY = new wxCheckBox(Panel3, ID_CHECKBOX34, _("TX_CADDY"), wxPoint(656,152), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX34"));
  CheckBoxTX_CADDY->SetValue(false);
  CheckBoxTX_CADDY->SetToolTip(_("Pour gruvin9x seulement"));
  CheckBoxTOGGLETRIM = new wxCheckBox(Panel3, ID_CHECKBOX36, _("Toggle trims"), wxPoint(456,120), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX36"));
  CheckBoxTOGGLETRIM->SetValue(false);
  CheckBoxTOGGLETRIM->SetToolTip(_("Change les trim de la main droite à la main gauche, et vice-versa"));
  CheckBoxNOANDSECONDE = new wxCheckBox(Panel3, ID_CHECKBOX37, _("NOANDSECONDE"), wxPoint(336,184), wxSize(102,16), 0, wxDefaultValidator, _T("ID_CHECKBOX37"));
  CheckBoxNOANDSECONDE->SetValue(false);
  CheckBoxSHUTDOWN_CONFIRMATION = new wxCheckBox(Panel3, ID_CHECKBOX38, _("SHUTDOWN_CONFIRMATION"), wxPoint(456,248), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX38"));
  CheckBoxSHUTDOWN_CONFIRMATION->SetValue(false);
  CheckBoxACCURAT_THROTTLE_STATS = new wxCheckBox(Panel3, ID_CHECKBOX39, _("ACCURAT_THROTTLE_STATS"), wxPoint(456,216), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX39"));
  CheckBoxACCURAT_THROTTLE_STATS->SetValue(false);
  CheckBoxEEPROM_PROGRESS_BAR = new wxCheckBox(Panel3, ID_CHECKBOX43, _("EEPROM bar"), wxPoint(336,120), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX43"));
  CheckBoxEEPROM_PROGRESS_BAR->SetValue(false);
  CheckBoxAUTOSWITCH = new wxCheckBox(Panel3, ID_CHECKBOX44, _("AUTOSWITCH"), wxPoint(336,88), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX44"));
  CheckBoxAUTOSWITCH->SetValue(false);
  CheckBoxAUTOSOURCE = new wxCheckBox(Panel3, ID_CHECKBOX45, _("AUTOSOURCE"), wxPoint(336,56), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX45"));
  CheckBoxAUTOSOURCE->SetValue(false);
  CheckBoxAUTOSOURCE->SetFocus();
  CheckBoxDBLKEYS = new wxCheckBox(Panel3, ID_CHECKBOX47, _("Double keys"), wxPoint(336,152), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX47"));
  CheckBoxDBLKEYS->SetValue(false);
  CheckBoxDSM2PPM = new wxCheckBox(Panel3, ID_CHECKBOX28, _("DSM2 PPM"), wxPoint(72,152), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX28"));
  CheckBoxDSM2PPM->SetValue(false);
  Compilerpage2 = new wxButton(Panel3, ID_BUTTON1, _("Compiler"), wxPoint(680,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
  Sortirpage2 = new wxButton(Panel3, ID_BUTTON4, _("Sauvegarder et sortir"), wxPoint(512,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
  Panel4 = new wxPanel(Notebook1, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
  StaticBox8 = new wxStaticBox(Panel4, ID_STATICBOX8, _("3POS"), wxPoint(208,32), wxSize(160,104), 0, _T("ID_STATICBOX8"));
  StaticBox7 = new wxStaticBox(Panel4, ID_STATICBOX7, _("Nom des interrupteurs"), wxPoint(72,8), wxSize(312,328), 0, _T("ID_STATICBOX7"));
  ComboBoxswitch1 = new wxComboBox(Panel4, ID_COMBOBOX1, wxEmptyString, wxPoint(136,48), wxSize(54,21), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX1"));
  ComboBoxswitch1->SetSelection( ComboBoxswitch1->Append(_("THR")) );
  ComboBoxswitch1->Append(_("---"));
  Sortirpage3 = new wxButton(Panel4, ID_BUTTON5, _("Sauvegarder et sortir"), wxPoint(512,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON5"));
  Compilerpage3 = new wxButton(Panel4, ID_BUTTON6, _("Compiler"), wxPoint(680,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
  ComboBoxswitch2 = new wxComboBox(Panel4, ID_COMBOBOX2, wxEmptyString, wxPoint(136,88), wxSize(54,21), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX2"));
  ComboBoxswitch2->SetSelection( ComboBoxswitch2->Append(_("RUD")) );
  ComboBoxswitch2->Append(_("---"));
  ComboBoxswitch3 = new wxComboBox(Panel4, ID_COMBOBOX3, wxEmptyString, wxPoint(136,128), wxSize(54,21), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX3"));
  ComboBoxswitch3->SetSelection( ComboBoxswitch3->Append(_("ELE")) );
  ComboBoxswitch3->Append(_("---"));
  ComboBoxswitch4 = new wxComboBox(Panel4, ID_COMBOBOX4, wxEmptyString, wxPoint(136,168), wxSize(54,21), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX4"));
  ComboBoxswitch4->SetSelection( ComboBoxswitch4->Append(_("AIL")) );
  ComboBoxswitch4->Append(_("---"));
  ComboBoxswitch5 = new wxComboBox(Panel4, ID_COMBOBOX5, wxEmptyString, wxPoint(136,208), wxSize(54,21), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX5"));
  ComboBoxswitch5->SetSelection( ComboBoxswitch5->Append(_("GEA")) );
  ComboBoxswitch5->Append(_("---"));
  ComboBoxswitch6 = new wxComboBox(Panel4, ID_COMBOBOX6, wxEmptyString, wxPoint(136,248), wxSize(54,21), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX6"));
  ComboBoxswitch6->SetSelection( ComboBoxswitch6->Append(_("TRN")) );
  ComboBoxswitch6->Append(_("---"));
  CheckBoxPERSONAMES = new wxCheckBox(Panel4, ID_CHECKBOX22, _("PERSONAMES"), wxPoint(256,248), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX22"));
  CheckBoxPERSONAMES->SetValue(false);
  StaticText13 = new wxStaticText(Panel4, ID_STATICTEXT13, _("Inter1"), wxPoint(88,56), wxDefaultSize, 0, _T("ID_STATICTEXT13"));
  StaticText14 = new wxStaticText(Panel4, ID_STATICTEXT14, _("Inter2"), wxPoint(88,96), wxDefaultSize, 0, _T("ID_STATICTEXT14"));
  StaticText15 = new wxStaticText(Panel4, ID_STATICTEXT15, _("Inter3"), wxPoint(88,136), wxDefaultSize, 0, _T("ID_STATICTEXT15"));
  StaticText16 = new wxStaticText(Panel4, ID_STATICTEXT16, _("Inter4"), wxPoint(88,176), wxDefaultSize, 0, _T("ID_STATICTEXT16"));
  StaticText17 = new wxStaticText(Panel4, ID_STATICTEXT17, _("Inter5"), wxPoint(88,216), wxDefaultSize, 0, _T("ID_STATICTEXT17"));
  StaticText18 = new wxStaticText(Panel4, ID_STATICTEXT18, _("Inter6"), wxPoint(88,256), wxDefaultSize, 0, _T("ID_STATICTEXT18"));
  ButtonDefaut = new wxButton(Panel4, ID_BUTTON7, _("Défaut"), wxPoint(272,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
  StaticText19 = new wxStaticText(Panel4, ID_STATICTEXT19, _("ID0"), wxPoint(248,56), wxDefaultSize, 0, _T("ID_STATICTEXT19"));
  ComboBoxID0 = new wxComboBox(Panel4, ID_COMBOBOX7, wxEmptyString, wxPoint(288,48), wxSize(54,-1), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX7"));
  ComboBoxID0->SetSelection( ComboBoxID0->Append(_("ID0")) );
  ComboBoxID0->Append(_("---"));
  StaticText20 = new wxStaticText(Panel4, ID_STATICTEXT20, _("EncodeurA"), wxPoint(224,168), wxDefaultSize, 0, _T("ID_STATICTEXT20"));
  StaticText21 = new wxStaticText(Panel4, ID_STATICTEXT21, _("EncodeurB"), wxPoint(224,208), wxDefaultSize, 0, _T("ID_STATICTEXT21"));
  ComboBoxEncodeurA = new wxComboBox(Panel4, ID_COMBOBOX8, wxEmptyString, wxPoint(288,160), wxSize(54,-1), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX8"));
  ComboBoxEncodeurA->SetSelection( ComboBoxEncodeurA->Append(_("ReA")) );
  ComboBoxEncodeurA->Append(_("---"));
  ComboBoxEncodeurB = new wxComboBox(Panel4, ID_COMBOBOX9, wxEmptyString, wxPoint(288,200), wxSize(54,-1), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX9"));
  ComboBoxEncodeurB->SetSelection( ComboBoxEncodeurB->Append(_("ReB")) );
  ComboBoxEncodeurB->Append(_("---"));
  ComboBoxID1 = new wxComboBox(Panel4, ID_COMBOBOX10, wxEmptyString, wxPoint(288,72), wxSize(54,0), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX10"));
  ComboBoxID1->SetSelection( ComboBoxID1->Append(_("ID1")) );
  ComboBoxID1->Append(_("---"));
  ComboBoxID2 = new wxComboBox(Panel4, ID_COMBOBOX11, wxEmptyString, wxPoint(288,96), wxSize(54,0), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX11"));
  ComboBoxID2->SetSelection( ComboBoxID2->Append(_("ID2")) );
  ComboBoxID2->Append(_("---"));
  StaticText22 = new wxStaticText(Panel4, ID_STATICTEXT22, _("ID1"), wxPoint(248,80), wxDefaultSize, 0, _T("ID_STATICTEXT22"));
  StaticText23 = new wxStaticText(Panel4, ID_STATICTEXT23, _("ID2"), wxPoint(248,104), wxDefaultSize, 0, _T("ID_STATICTEXT23"));
  Notebook1->AddPage(Panel2, _("Réglages"), false);
  Notebook1->AddPage(Panel3, _("Options avancées"), false);
  Notebook1->AddPage(Panel4, _("Personnalisation"), false);

  Connect(ID_CHOICE3,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CompilerOptionsFrame::OnChoiceVOICESelect);
  Connect(ID_CHOICE8,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CompilerOptionsFrame::OnChoiceNAVIGATIONSelect);
  Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonEXITClick);
  Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonCOMPILEClick);
  Connect(ID_CHECKBOX13,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxDSM2SERIALClick);
  Connect(ID_CHOICE2,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CompilerOptionsFrame::OnChoiceEXTSelect2);
  Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxFRSKY_HUBClick);
  Connect(ID_CHECKBOX3,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxFAS_OFFSETClick2);
  Connect(ID_CHECKBOX4,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxGAUGESClick1);
  Connect(ID_CHECKBOX5,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxGPSClick1);
  Connect(ID_CHECKBOX6,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxVARIOClick2);
  Connect(ID_CHECKBOX8,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxSPORT_FILE_LOGClick1);
  Connect(ID_CHECKBOX32,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxWS_HOW_HIGHClick2);
  Connect(ID_CHECKBOX37,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxNOANDSECONDEClick1);
  Connect(ID_CHECKBOX45,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxAUTOSOURCEClick1);
  Connect(ID_CHECKBOX28,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxDSM2PPMClick);
  Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonCOMPILEClick);
  Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonEXITClick);
  Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonEXITClick);
  Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonCOMPILEClick);
  Connect(ID_CHECKBOX22,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxPERSONAMESClick);
  Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonDefautClick1);
  Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&CompilerOptionsFrame::OnClose);
  //*)

  {
    wxIcon FrameIcon;
    SetIcon(wxICON(nsrcd_icon));
  }

  ChoicePCB->SetStringSelection(PCB);
  ChoiceLCD->SetStringSelection(LCD);
  ChoiceVOICE->SetStringSelection(VOICE);
  ChoiceEXT->SetStringSelection(EXT);
  CheckBoxAUDIO->SetValue(AUDIO);
  CheckBoxHELI->SetValue(HELI);
  ChoiceTTS->SetStringSelection(TTS);
  ChoiceTRANSLATIONS->SetStringSelection(TRANSLATIONS);
  ChoiceNAVIGATION->SetStringSelection(NAVIGATION);
  CheckBoxFRSKY_HUB->SetValue(FRSKY_HUB);
  CheckBoxHAPTIC->SetValue(HAPTIC);
  ChoicePPM_UNIT->SetStringSelection(PPM_UNIT);
  CheckBoxGAUGES->SetValue(GAUGES);
  CheckBoxGPS->SetValue(GPS);
  CheckBoxVARIO->SetValue(VARIO);
  CheckBoxRTCLOCK->SetValue(RTCLOCK);
  CheckBoxSPORT_FILE_LOG->SetValue(SPORT_FILE_LOG);
  CheckBoxPPM->SetValue(PPM);
  CheckBoxPXX->SetValue(PXX);
  CheckBoxDSM2SERIAL->SetValue(DSM2SERIAL);
  CheckBoxDSM2PPM->SetValue(DSM2PPM);
  CheckBoxSD_CARD->SetValue(SD_CARD);
  CheckBoxFAS_OFFSET->SetValue(FAS_OFFSET);
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
  CheckBoxPCBREV->SetValue(PCBREV);
  CheckBoxTURNIGY_TRANSMITTER_FIX->SetValue(TURNIGY_TRANSMITTER_FIX);
  CheckBoxFRSKY_STICKS->SetValue(FRSKY_STICKS);
  CheckBoxCORRECT_NEGATIVE_VALUES->SetValue(CORRECT_NEGATIVE_VALUES);
  CheckBoxARITHMETIC_OVERFLOW_CHECK->SetValue(ARITHMETIC_OVERFLOW_CHECK);
  CheckBoxACCURAT_THROTTLE_STATS->SetValue(ACCURAT_THROTTLE_STATS);
  CheckBoxSP22->SetValue(SP22);
  CheckBoxPWM_BACKLIGHT->SetValue(PWM_BACKLIGHT);
  CheckBoxOVERRIDE_CHANNEL_FUNCTION->SetValue(OVERRIDE_CHANNEL_FUNCTION);
  CheckBoxWS_HOW_HIGH->SetValue(WS_HOW_HIGH);
  CheckBoxTX_CADDY->SetValue(TX_CADDY);
  CheckBoxTOGGLETRIM->SetValue(TOGGLETRIM);
  CheckBoxNOANDSECONDE->SetValue(NOANDSECONDE);// Hardwired
  CheckBoxSHUTDOWN_CONFIRMATION->SetValue(SHUTDOWN_CONFIRMATION);
  CheckBoxPERSONAMES->SetValue(PERSONAMES);

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
}

CompilerOptionsFrame::~CompilerOptionsFrame()
{
  //(*Destroy(CompilerOptionsFrame)
  //*)
}

void CompilerOptionsFrame::OnClose(wxCloseEvent& event)
{
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
  CompiBat += (" VOICE=" + VOICE);
  CompiBat += (" EXT=" + EXT);
  if (AUDIO) CompiBat += (" AUDIO=YES");// default should be NO
  if (HELI) CompiBat += (" HELI=YES");// default should be NO
  CompiBat += (" TTS=" + TTS);
  CompiBat += (" TRANSLATIONS=" + TRANSLATIONS);
  CompiBat += (" NAVIGATION=" + NAVIGATION);
  if (FRSKY_HUB) CompiBat += (" FRSKY_HUB=YES");// default should be NO
  if (HAPTIC) CompiBat += (" HAPTIC=YES");// default should be NO
  CompiBat += (" PPM_UNIT=" + PPM_UNIT);
  if (GAUGES) CompiBat += (" GAUGES=YES");// default should be NO
  if (GPS) CompiBat += (" GPS=YES");// default should be NO
  if (VARIO) CompiBat += (" VARIO=YES");// default should be NO
  if (RTCLOCK) CompiBat += (" RTCLOCK=YES");// default should be NO
  if (SPORT_FILE_LOG) CompiBat += (" SPORT_FILE_LOG=YES");// default should be NO
  if (PPM) CompiBat += (" PPM=YES");// default should be YES
  if (PXX) CompiBat += (" PXX=YES");// default should be NO
  if (DSM2SERIAL) CompiBat += (" DSM2=SERIAL");// default should be NO
  if (DSM2PPM) CompiBat += (" DSM2=PPM");// default should be NO
  if (SD_CARD) CompiBat += (" SDCARD=YES");// default should be NO
  if (FAS_OFFSET) CompiBat += (" FAS_OFFSET=YES");// default should be NO
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
  if (PCBREV) CompiBat += (" PCBREV=REV0");
  if (TURNIGY_TRANSMITTER_FIX) CompiBat += (" TURNIGY_TRANSMITTER_FIX=YES");
  if (FRSKY_STICKS) CompiBat += (" FRSKY_STICKS=YES");
  if (CORRECT_NEGATIVE_VALUES) CompiBat += (" CORRECT_NEGATIVE_VALUES=YES");
  if (ARITHMETIC_OVERFLOW_CHECK) CompiBat += (" ARITHMETIC_OVERFLOW_CHECK=YES");
  if (ACCURAT_THROTTLE_STATS) CompiBat += (" ACCURAT_THROTTLE_STATS=YES");
  if (SP22) CompiBat += (" SP22=YES");
  if (PWM_BACKLIGHT) CompiBat += (" PWM_BACKLIGHT=YES");
  if (OVERRIDE_CHANNEL_FUNCTION) CompiBat += (" OVERRIDE_CHANNEL_FUNCTION=YES");
  if (WS_HOW_HIGH) CompiBat += (" WS_HOW_HIGH=YES");
  if (TX_CADDY) CompiBat += (" TX_CADDY=YES");
  if (TOGGLETRIM) CompiBat += (" TOGGLETRIM=YES");
  if (NOANDSECONDE) CompiBat += (" NOANDSECONDE=YES");
  if (SHUTDOWN_CONFIRMATION) CompiBat += (" SHUTDOWN_CONFIRMATION=YES");
  if (PERSONAMES)
    {
      CompiBat += (" PERSONAMES=YES");
      CreatePersonames_H(); // Create the header file
    }
  wxMessageBox(CompiBat);
  CreateCompileBatFile(CompiBat);
  wxExecute(AppPath+ "\\CompileBatFile.bat",wxEXEC_SYNC | wxEXEC_NODISABLE );// Create firmware
  Close();
  wxMessageBox(AppPath+ "\\firmware\\OpenAVRc.hex","Le nouveau Firmware est Le fichier:");
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
  VOICE  = ChoiceVOICE->GetString(ChoiceVOICE->GetSelection());
  EXT  = ChoiceEXT->GetString(ChoiceEXT->GetSelection());
  AUDIO = CheckBoxAUDIO->GetValue();
  HELI = CheckBoxHELI->GetValue();
  TTS  = ChoiceTTS->GetString(ChoiceTTS->GetSelection());
  TRANSLATIONS  = ChoiceTRANSLATIONS->GetString(ChoiceTRANSLATIONS->GetSelection());
  NAVIGATION  = ChoiceNAVIGATION->GetString(ChoiceNAVIGATION->GetSelection());
  FRSKY_HUB = CheckBoxFRSKY_HUB->GetValue();
  HAPTIC = CheckBoxHAPTIC->GetValue();
  PPM_UNIT  = ChoicePPM_UNIT->GetString(ChoicePPM_UNIT->GetSelection());
  GAUGES = CheckBoxGAUGES->GetValue();
  GPS = CheckBoxGPS->GetValue();
  VARIO = CheckBoxVARIO->GetValue();
  RTCLOCK = CheckBoxRTCLOCK->GetValue();
  SPORT_FILE_LOG = CheckBoxSPORT_FILE_LOG->GetValue();
  PPM = CheckBoxPPM->GetValue();
  PXX = CheckBoxPXX->GetValue();
  DSM2SERIAL = CheckBoxDSM2SERIAL->GetValue();
  DSM2PPM = CheckBoxDSM2PPM->GetValue();
  SD_CARD = CheckBoxSD_CARD->GetValue();
  FAS_OFFSET = CheckBoxFAS_OFFSET->GetValue();
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
  PCBREV = CheckBoxPCBREV->GetValue();
  TURNIGY_TRANSMITTER_FIX = CheckBoxTURNIGY_TRANSMITTER_FIX->GetValue();
  FRSKY_STICKS = CheckBoxFRSKY_STICKS->GetValue();
  CORRECT_NEGATIVE_VALUES = CheckBoxCORRECT_NEGATIVE_VALUES->GetValue();
  ARITHMETIC_OVERFLOW_CHECK = CheckBoxARITHMETIC_OVERFLOW_CHECK->GetValue();
  ACCURAT_THROTTLE_STATS = CheckBoxACCURAT_THROTTLE_STATS->GetValue();
  SP22 = CheckBoxSP22->GetValue();
  PWM_BACKLIGHT = CheckBoxPWM_BACKLIGHT->GetValue();
  OVERRIDE_CHANNEL_FUNCTION = CheckBoxOVERRIDE_CHANNEL_FUNCTION->GetValue();
  WS_HOW_HIGH = CheckBoxWS_HOW_HIGH->GetValue();
  TX_CADDY = CheckBoxTX_CADDY->GetValue();
  TOGGLETRIM = CheckBoxTOGGLETRIM->GetValue();
  NOANDSECONDE = CheckBoxNOANDSECONDE->GetValue();
  SHUTDOWN_CONFIRMATION = CheckBoxSHUTDOWN_CONFIRMATION->GetValue();
  PERSONAMES = CheckBoxPERSONAMES->GetValue();

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

  switch1 = switch1.SubString(0, 2);
  switch2 = switch2.SubString(0, 2);
  switch3 = switch3.SubString(0, 2);
  switch4 = switch4.SubString(0, 2);
  switch5 = switch5.SubString(0, 2);
  switch6 = switch6.SubString(0, 2);
  switchID0 = switchID0.SubString(0, 2);
  switchID1 = switchID1.SubString(0, 2);
  switchID2 = switchID2.SubString(0, 2);
  encoderA  = encoderA.SubString(0, 2);
  encoderB  = encoderB.SubString(0, 2);

  wxString switchArray[] = {switch1, switch2, switch3, switch4, switch5, switch6, switchID0, switchID1, switchID2, encoderA, encoderB};

  for (int i=0; i<12; i++)
  {
     for (int j=0; j<12; j++)
     {
        if (i == j)
     {
        continue;
  }
    if (switchArray[i] == switchArray[j])
    {
       doNotClose = 1;
    }
   }
  }
  if (doNotClose)
  {
     wxMessageBox(_("Il y a au moins un nom répété ou non défini, veuillez le changer"));
  }
}

void CompilerOptionsFrame::OnButtonEXITClick(wxCommandEvent& event)
{
  CollectDatas();
  Ini_Changed = true;
  if (!doNotClose) Close();
}

void CompilerOptionsFrame::WriteSplashFile()
{
  wxString data;
  wxString line;
  wxTextFile Splashfile( AppPath + "\\sources\\bitmaps\\splash.lbm");
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
          data.Printf("0x%02x",LbmSplash[2+(i*LCD_W)+j]);
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
  CompileBatFile.AddLine("Path " + AppPath + "\\AVR-GCC\\bin;C:\\OpenAVRc\\AVR-GCC\\utils\\bin");
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
  wxTextFile CompileBatFile(Filename);

  if(CompileBatFile.Exists()) CompileBatFile.Open(Filename);
  else CompileBatFile.Create(Filename);

  CompileBatFile.Open();
  CompileBatFile.Clear();
  CompileBatFile.AddLine("#ifndef PERSONAMES_H_INCLUDED");
  CompileBatFile.AddLine("#define PERSONAMES_H_INCLUDED");
  CompileBatFile.AddLine("#undef TR_PHYS_SWITCHES");
  CompileBatFile.AddLine("#define TR_PHYS_SWITCHES "+g+switch1+g+g+switch2+g+g+switch3+g+g+switch4+g+g+switch5+g+g+switch6+g);
//#undef TR_9X_3POS_SWITCHES //"ID0""ID1""ID2" Futur use ?
//#define TR_9X_3POS_SWITCHES  "3P0""3P1""3P2"
  CompileBatFile.AddLine("#endif");
  CompileBatFile.Write();
  CompileBatFile.Close();
}

void CompilerOptionsFrame::OnCheckBoxDSM2SERIALClick(wxCommandEvent& event)
{
  CheckBoxDSM2PPM->SetValue(0);
}

void CompilerOptionsFrame::OnCheckBoxDSM2PPMClick(wxCommandEvent& event)
{
  CheckBoxDSM2SERIAL->SetValue(0);
}

void CompilerOptionsFrame::OnCheckBoxAUTOSOURCEClick1(wxCommandEvent& event)//pas compatible avec navigation=sticks
{
  CollectDatas();
  if ((NAVIGATION == wxT("STICKS")) & (AUTOSOURCE))
    {
      wxMessageBox("Non compatible avec NAVIGATION = STICKS");
      CheckBoxAUTOSOURCE->SetValue(0);
    }
}

void CompilerOptionsFrame::OnChoiceNAVIGATIONSelect(wxCommandEvent& event)
{
  CollectDatas();
  if ((AUTOSOURCE) & (NAVIGATION == wxT("STICKS")))
    {
      wxMessageBox("Non compatible avec AUTOSOURCE. AUTOSOURCE décoché.");
      CheckBoxAUTOSOURCE->SetValue(0);
    }
}

void CompilerOptionsFrame::OnChoiceVOICESelect(wxCommandEvent& event)
{
  CollectDatas();
  if (VOICE == (_("NO")))
    {
      CheckBoxNOANDSECONDE->SetValue(0);
      Panel3->Refresh ();
    }

}

void CompilerOptionsFrame::OnCheckBoxNOANDSECONDEClick1(wxCommandEvent& event)
{
  CollectDatas();
  if (VOICE == (_("NO")))
    {
      wxMessageBox("Non compatible avec VOICE=NO.");
      CheckBoxAUTOSOURCE->SetValue(0);
    }
}


void CompilerOptionsFrame::OnChoiceEXTSelect2(wxCommandEvent& event)
{
  CollectDatas();
  if (EXT == (_("STD")))
    {
      CheckBoxFRSKY_HUB->SetValue(0);
      CheckBoxFAS_OFFSET->SetValue(0);
      CheckBoxGAUGES->SetValue(0);
      CheckBoxGPS->SetValue(0);
      CheckBoxVARIO->SetValue(0);
      CheckBoxSPORT_FILE_LOG->SetValue(0);
      CheckBoxWS_HOW_HIGH->SetValue(0);
    }
}

void CompilerOptionsFrame::OnCheckBoxFRSKY_HUBClick(wxCommandEvent& event)
{
  CollectDatas();
  if (EXT == (_("STD")))
    {
      wxMessageBox("Non compatible avec TELEMETRY = STD.");
      CheckBoxFRSKY_HUB->SetValue(0);
    }
}

void CompilerOptionsFrame::OnCheckBoxFAS_OFFSETClick2(wxCommandEvent& event)
{
  CollectDatas();
  if (EXT == (_("STD")))
    {
      wxMessageBox("Non compatible avec TELEMETRY = STD.");
      CheckBoxFAS_OFFSET->SetValue(0);
    }

}

void CompilerOptionsFrame::OnCheckBoxGAUGESClick1(wxCommandEvent& event)
{
  CollectDatas();
  if (EXT == (_("STD")))
    {
      wxMessageBox("Non compatible avec TELEMETRY = STD.");
      CheckBoxGAUGES->SetValue(0);
    }
}

void CompilerOptionsFrame::OnCheckBoxGPSClick1(wxCommandEvent& event)
{
  CollectDatas();
  if (EXT == (_("STD")))
    {
      wxMessageBox("Non compatible avec TELEMETRY = STD.");
      CheckBoxGPS->SetValue(0);
    }
}

void CompilerOptionsFrame::OnCheckBoxVARIOClick2(wxCommandEvent& event)
{
  CollectDatas();
  if (EXT == (_("STD")))
    {
      wxMessageBox("Non compatible avec TELEMETRY = STD.");
      CheckBoxVARIO->SetValue(0);
    }
}

void CompilerOptionsFrame::OnCheckBoxSPORT_FILE_LOGClick1(wxCommandEvent& event)
{
  CollectDatas();
  if (EXT == (_("STD")))
    {
      wxMessageBox("Non compatible avec TELEMETRY = STD.");
      CheckBoxSPORT_FILE_LOG->SetValue(0);
    }
}

void CompilerOptionsFrame::OnCheckBoxWS_HOW_HIGHClick2(wxCommandEvent& event)
{
  CollectDatas();
  if (EXT == (_("STD")))
    {
      wxMessageBox("Non compatible avec TELEMETRY = STD.");
      CheckBoxWS_HOW_HIGH->SetValue(0);
    }
}

void CompilerOptionsFrame::OnCheckBoxPERSONAMESClick(wxCommandEvent& event)
{
  CollectDatas();
}


void CompilerOptionsFrame::OnButtonDefautClick1(wxCommandEvent& event)
{
  ComboBoxswitch1->SetValue("THR");
  ComboBoxswitch2->SetValue("RUD");
  ComboBoxswitch3->SetValue("ELE");
  ComboBoxswitch4->SetValue("AIL");
  ComboBoxswitch5->SetValue("GEA");
  ComboBoxswitch6->SetValue("TRN");
  ComboBoxID0->SetValue("ID0");
  ComboBoxID1->SetValue("ID1");
  ComboBoxID2->SetValue("ID2");
  ComboBoxEncodeurA->SetValue("ReA");
  ComboBoxEncodeurB->SetValue("ReB");
}
