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
const long CompilerOptionsFrame::ID_PANEL2 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX1 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX9 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICECC2500PAG = wxNewId();
const long CompilerOptionsFrame::ID_CHOICEA7105PAG = wxNewId();
const long CompilerOptionsFrame::ID_CHOICENRF24l01PAG = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX3 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOXNOANDSECONDE = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX2 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX10 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX11 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX13 = wxNewId();
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
const long CompilerOptionsFrame::ID_CHECKBOX28 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON1 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON4 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX23 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX24 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX25 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX29 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICECYRF6936PAG = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT28 = wxNewId();
const long CompilerOptionsFrame::ID_PANEL3 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX7 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX10 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX8 = wxNewId();
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
const long CompilerOptionsFrame::ID_COMBOBOX14 = wxNewId();
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
const long CompilerOptionsFrame::ID_PANEL4 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX4 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX11 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX8 = wxNewId();
const long CompilerOptionsFrame::ID_STATICBOX12 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX2 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX3 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON8 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON9 = wxNewId();
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
  SetClientSize(wxSize(807,377));
  Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(0,0), wxSize(752,327), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
  Panel1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
  Notebook1 = new wxNotebook(Panel1, ID_NOTEBOOK1, wxPoint(0,0), wxSize(808,376), 0, _T("ID_NOTEBOOK1"));
  Panel2 = new wxPanel(Notebook1, ID_PANEL2, wxDefaultPosition, wxSize(802,302), wxTAB_TRAVERSAL, _T("ID_PANEL2"));
  StaticBox6 = new wxStaticBox(Panel2, ID_STATICBOX6, _("Autres options"), wxPoint(464,8), wxSize(192,328), 0, _T("ID_STATICBOX6"));
  StaticBox4 = new wxStaticBox(Panel2, ID_STATICBOX4, _("Matériel"), wxPoint(24,8), wxSize(200,328), 0, _T("ID_STATICBOX4"));
  CheckBoxPPM_LIMITS_SYMETRICAL = new wxCheckBox(Panel2, ID_CHECKBOX17, _("PPM Simm limits"), wxPoint(528,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX17"));
  CheckBoxPPM_LIMITS_SYMETRICAL->SetValue(false);
  CheckBoxPPM_LIMITS_SYMETRICAL->SetToolTip(_("Option ajuster le gain fonction du subtrim"));
  StaticBox5 = new wxStaticBox(Panel2, ID_STATICBOX5, _("Interface"), wxPoint(232,8), wxSize(224,328), 0, _T("ID_STATICBOX5"));
  StaticText4 = new wxStaticText(Panel2, ID_STATICTEXT4, _("Unités"), wxPoint(264,304), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
  CheckBoxPPM_CENTER_ADJUSTABLE = new wxCheckBox(Panel2, ID_CHECKBOX16, _("PPM Center adjust"), wxPoint(528,272), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX16"));
  CheckBoxPPM_CENTER_ADJUSTABLE->SetValue(false);
  CheckBoxPPM_CENTER_ADJUSTABLE->SetToolTip(_("Option réglage des neutres indépendant"));
  ChoicePPM_UNIT  = new wxChoice(Panel2, ID_CHOICE4, wxPoint(336,296), wxSize(112,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE4"));
  ChoicePPM_UNIT ->SetSelection( ChoicePPM_UNIT ->Append(_("PERCENT_PREC1")) );
  ChoicePPM_UNIT ->Append(_("PERCENT_PREC0"));
  ChoicePPM_UNIT ->Append(_("US"));
  ChoicePPM_UNIT ->SetToolTip(_("Affichage des valeurs des voies en % ou uS"));
  ChoicePCB = new wxChoice(Panel2, ID_CHOICE6, wxPoint(112,32), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE6"));
  ChoicePCB->SetSelection( ChoicePCB->Append(_("MEGAMINI")) );
  ChoicePCB->SetToolTip(_("Type de radio"));
  ChoiceLCD = new wxChoice(Panel2, ID_CHOICE1, wxPoint(112,72), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
  ChoiceLCD->Append(_("ST7565P"));
  ChoiceLCD->Append(_("ST7565R "));
  ChoiceLCD->Append(_("ERC12864FSF"));
  ChoiceLCD->Append(_("ST7920"));
  ChoiceLCD->SetSelection( ChoiceLCD->Append(_("KS108")) );
  ChoiceLCD->Append(_("SSD1306"));
  ChoiceLCD->Append(_("SH1106"));
  ChoiceLCD->SetToolTip(_("Référence de l\'écran"));
  StaticText2 = new wxStaticText(Panel2, ID_STATICTEXT2, _("Processeur"), wxPoint(32,40), wxSize(64,13), 0, _T("ID_STATICTEXT2"));
  StaticText1 = new wxStaticText(Panel2, ID_STATICTEXT1, _("LCD"), wxPoint(64,80), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
  ChoiceVOICE = new wxChoice(Panel2, ID_CHOICE3, wxPoint(112,120), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
  ChoiceVOICE->SetSelection( ChoiceVOICE->Append(_("NO")) );
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
  CheckBoxSD_CARD = new wxCheckBox(Panel2, ID_CHECKBOX12, _("Carte SD"), wxPoint(112,224), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX12"));
  CheckBoxSD_CARD->SetValue(false);
  CheckBoxSD_CARD->SetToolTip(_("Option carte SD (Sauvegarde - Fichiers log)"));
  ChoiceTRANSLATIONS = new wxChoice(Panel2, ID_CHOICE7, wxPoint(336,32), wxSize(72,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE7"));
  ChoiceTRANSLATIONS->Append(_("CZ"));
  ChoiceTRANSLATIONS->Append(_("DE"));
  ChoiceTRANSLATIONS->Append(_("EN"));
  ChoiceTRANSLATIONS->Append(_("ES"));
  ChoiceTRANSLATIONS->SetSelection( ChoiceTRANSLATIONS->Append(_("FR")) );
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
  ChoiceTTS->Append(_("ES"));
  ChoiceTTS->SetSelection( ChoiceTTS->Append(_("FR")) );
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
  ChoiceUNITS = new wxChoice(Panel2, ID_CHOICE9, wxPoint(528,32), wxSize(72,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE9"));
  ChoiceUNITS->SetSelection( ChoiceUNITS->Append(_("METRIC")) );
  ChoiceUNITS->Append(_("IMPERIAL"));
  ChoiceUNITS->SetToolTip(_("Unitées"));
  StaticText8 = new wxStaticText(Panel2, ID_STATICTEXT8, _("Unités"), wxPoint(480,40), wxDefaultSize, 0, _T("ID_STATICTEXT8"));
  ChoiceDEFAULT_MODE = new wxChoice(Panel2, ID_CHOICE10, wxPoint(528,72), wxSize(72,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE10"));
  ChoiceDEFAULT_MODE->SetSelection( ChoiceDEFAULT_MODE->Append(_("NO")) );
  ChoiceDEFAULT_MODE->Append(_("1"));
  ChoiceDEFAULT_MODE->Append(_("2"));
  ChoiceDEFAULT_MODE->Append(_("3"));
  ChoiceDEFAULT_MODE->Append(_("4"));
  ChoiceDEFAULT_MODE->SetToolTip(_("Mode de la radio"));
  StaticText9 = new wxStaticText(Panel2, ID_STATICTEXT9, _("Mode"), wxPoint(480,80), wxDefaultSize, 0, _T("ID_STATICTEXT9"));
  CheckBoxFLIGHT_MODES = new wxCheckBox(Panel2, ID_CHECKBOX18, _("Flight modes"), wxPoint(528,200), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX18"));
  CheckBoxFLIGHT_MODES->SetValue(false);
  CheckBoxFLIGHT_MODES->SetToolTip(_("Option phases de vol"));
  CheckBoxCURVES = new wxCheckBox(Panel2, ID_CHECKBOX19, _("Curves"), wxPoint(528,248), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX19"));
  CheckBoxCURVES->SetValue(false);
  CheckBoxCURVES->SetToolTip(_("Option courbes"));
  CheckBoxGVARS = new wxCheckBox(Panel2, ID_CHECKBOX20, _("Global vars"), wxPoint(528,224), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX20"));
  CheckBoxGVARS->SetValue(false);
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
  ChoiceFAI = new wxChoice(Panel2, ID_CHOICE13, wxPoint(528,112), wxSize(72,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE13"));
  ChoiceFAI->SetSelection( ChoiceFAI->Append(_("NO")) );
  ChoiceFAI->Append(_("YES"));
  ChoiceFAI->Append(_("CHOICE"));
  ChoiceFAI->SetToolTip(_("Pour les compétitions"));
  StaticText12 = new wxStaticText(Panel2, ID_STATICTEXT12, _("FAI"), wxPoint(488,120), wxDefaultSize, 0, _T("ID_STATICTEXT12"));
  ChoiceTHREE_POS = new wxChoice(Panel2, ID_CHOICE12, wxPoint(112,288), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE12"));
  ChoiceTHREE_POS->Append(_("POT1"));
  ChoiceTHREE_POS->SetSelection( ChoiceTHREE_POS->Append(_("POT2")) );
  ChoiceTHREE_POS->Append(_("POT3"));
  ChoiceTHREE_POS->SetToolTip(_("Selection du potentiomètre utilisé pour XD0"));
  StaticText11 = new wxStaticText(Panel2, ID_STATICTEXT11, _("EXTRA 3POS"), wxPoint(32,296), wxDefaultSize, 0, _T("ID_STATICTEXT11"));
  Panel3 = new wxPanel(Notebook1, ID_PANEL3, wxDefaultPosition, wxSize(800,356), wxTAB_TRAVERSAL, _T("ID_PANEL3"));
  StaticBox1 = new wxStaticBox(Panel3, ID_STATICBOX1, _("Protocole"), wxPoint(8,24), wxSize(168,312), 0, _T("ID_STATICBOX1"));
  StaticBox9 = new wxStaticBox(Panel3, ID_STATICBOX9, _("Beta test"), wxPoint(24,160), wxSize(144,160), 0, _T("ID_STATICBOX9"));
  ChoiceCC2500PAG = new wxChoice(Panel3, ID_CHOICECC2500PAG, wxPoint(112,214), wxSize(40,8), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICECC2500PAG"));
  ChoiceCC2500PAG->SetSelection( ChoiceCC2500PAG->Append(_("0")) );
  ChoiceCC2500PAG->Append(_("10"));
  ChoiceCC2500PAG->Append(_("20"));
  ChoiceCC2500PAG->Append(_("22"));
  ChoiceA7105PAG = new wxChoice(Panel3, ID_CHOICEA7105PAG, wxPoint(112,288), wxSize(40,8), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICEA7105PAG"));
  ChoiceA7105PAG->SetSelection( ChoiceA7105PAG->Append(_("0")) );
  ChoiceA7105PAG->Append(_("10"));
  ChoiceA7105PAG->Append(_("20"));
  ChoiceA7105PAG->Append(_("22"));
  ChoiceA7105PAG->Disable();
  ChoiceNRF24l01PAG = new wxChoice(Panel3, ID_CHOICENRF24l01PAG, wxPoint(112,264), wxSize(40,8), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICENRF24l01PAG"));
  ChoiceNRF24l01PAG->SetSelection( ChoiceNRF24l01PAG->Append(_("0")) );
  ChoiceNRF24l01PAG->Append(_("10"));
  ChoiceNRF24l01PAG->Append(_("20"));
  ChoiceNRF24l01PAG->Append(_("22"));
  ChoiceNRF24l01PAG->Disable();
  StaticBox3 = new wxStaticBox(Panel3, ID_STATICBOX3, _("Autres paramètres"), wxPoint(328,16), wxSize(464,264), 0, _T("ID_STATICBOX3"));
  CheckBoxNOANDSECONDE = new wxCheckBox(Panel3, ID_CHECKBOXNOANDSECONDE, _("NOANDSECONDE"), wxPoint(336,184), wxSize(102,16), 0, wxDefaultValidator, _T("ID_CHECKBOXNOANDSECONDE"));
  CheckBoxNOANDSECONDE->SetValue(false);
  CheckBoxNOANDSECONDE->SetToolTip(_("Empèche la synthèse vocale de dire \"et\". Exemple : entre minutes \"et\" seconde"));
  StaticBox2 = new wxStaticBox(Panel3, ID_STATICBOX2, _("Télemetrie"), wxPoint(184,16), wxSize(136,312), 0, _T("ID_STATICBOX2"));
  CheckBoxPPM = new wxCheckBox(Panel3, ID_CHECKBOX10, _("PPM"), wxPoint(72,56), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX10"));
  CheckBoxPPM->SetValue(true);
  CheckBoxPPM->Disable();
  CheckBoxPPM->SetToolTip(_("Protocole PPM 8 à 16 voies"));
  CheckBoxSPIRF = new wxCheckBox(Panel3, ID_CHECKBOX11, _("SPI Xmitters"), wxPoint(48,176), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX11"));
  CheckBoxSPIRF->SetValue(false);
  CheckBoxSPIRF->Disable();
  CheckBoxSPIRF->SetToolTip(_("Protocole SPI RF "));
  CheckBoxDSM2SERIAL = new wxCheckBox(Panel3, ID_CHECKBOX13, _("DSM2 SERIAL"), wxPoint(72,88), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX13"));
  CheckBoxDSM2SERIAL->SetValue(false);
  CheckBoxDSM2SERIAL->SetToolTip(_("Protocoles DSM .."));
  ChoiceEXT = new wxChoice(Panel3, ID_CHOICE2, wxPoint(208,40), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
  ChoiceEXT->SetSelection( ChoiceEXT->Append(_("STD")) );
  ChoiceEXT->Append(_("FRSKY"));
  ChoiceEXT->SetToolTip(_("Télémétrie installée"));
  CheckBoxGPS = new wxCheckBox(Panel3, ID_CHECKBOX5, _("GPS"), wxPoint(208,112), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
  CheckBoxGPS->SetValue(false);
  CheckBoxVARIO = new wxCheckBox(Panel3, ID_CHECKBOX6, _("Vario"), wxPoint(208,144), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX6"));
  CheckBoxVARIO->SetValue(false);
  CheckBoxCORRECT_NEGATIVE_VALUES = new wxCheckBox(Panel3, ID_CHECKBOX26, _("CORRECT_NEGATIVE_VALUES"), wxPoint(456,152), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX26"));
  CheckBoxCORRECT_NEGATIVE_VALUES->SetValue(false);
  CheckBoxCORRECT_NEGATIVE_VALUES->SetToolTip(_("Symétrise les valeurs affichées"));
  CheckBoxARITHMETIC_OVERFLOW_CHECK = new wxCheckBox(Panel3, ID_CHECKBOX27, _("ARITHMETIC_OVERFLOW_CHECK"), wxPoint(456,184), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX27"));
  CheckBoxARITHMETIC_OVERFLOW_CHECK->SetValue(false);
  CheckBoxARITHMETIC_OVERFLOW_CHECK->SetToolTip(_("Ajoute du code pour prèvoir débordement aritmethique"));
  CheckBoxPWM_BACKLIGHT = new wxCheckBox(Panel3, ID_CHECKBOX30, _("PWM_BACKLIGHT"), wxPoint(456,56), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX30"));
  CheckBoxPWM_BACKLIGHT->SetValue(false);
  CheckBoxPWM_BACKLIGHT->Disable();
  CheckBoxOVERRIDE_CHANNEL_FUNCTION = new wxCheckBox(Panel3, ID_CHECKBOX31, _("Override channel function"), wxPoint(456,88), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX31"));
  CheckBoxOVERRIDE_CHANNEL_FUNCTION->SetValue(false);
  CheckBoxOVERRIDE_CHANNEL_FUNCTION->SetToolTip(_("Ajoute la fonction spéciale \"Remplacer CHx\""));
  CheckBoxWS_HOW_HIGH = new wxCheckBox(Panel3, ID_CHECKBOX32, _("WS_HOW_HIGH"), wxPoint(208,80), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX32"));
  CheckBoxWS_HOW_HIGH->SetValue(false);
  CheckBoxWS_HOW_HIGH->SetToolTip(_("WS HowHigh Altimeter"));
  CheckBoxTOGGLETRIM = new wxCheckBox(Panel3, ID_CHECKBOX36, _("Toggle trims"), wxPoint(456,120), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX36"));
  CheckBoxTOGGLETRIM->SetValue(false);
  CheckBoxTOGGLETRIM->SetToolTip(_("Change les trim de la main droite à la main gauche, et vice-versa"));
  CheckBoxFRAM = new wxCheckBox(Panel3, ID_CHECKBOX37, _("F-RAM"), wxPoint(336,216), wxSize(56,16), 0, wxDefaultValidator, _T("ID_CHECKBOX37"));
  CheckBoxFRAM->SetValue(false);
  CheckBoxFRAM->SetToolTip(_("Utilisation d\'une eeprom externe : FM24W256"));
  CheckBoxSHUTDOWN_CONFIRMATION = new wxCheckBox(Panel3, ID_CHECKBOX38, _("SHUTDOWN_CONFIRMATION"), wxPoint(456,248), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX38"));
  CheckBoxSHUTDOWN_CONFIRMATION->SetValue(true);
  CheckBoxSHUTDOWN_CONFIRMATION->Disable();
  CheckBoxACCURAT_THROTTLE_STATS = new wxCheckBox(Panel3, ID_CHECKBOX39, _("ACCURAT_THROTTLE_STATS"), wxPoint(456,216), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX39"));
  CheckBoxACCURAT_THROTTLE_STATS->SetValue(false);
  CheckBoxEEPROM_PROGRESS_BAR = new wxCheckBox(Panel3, ID_CHECKBOX43, _("EEPROM bar"), wxPoint(336,120), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX43"));
  CheckBoxEEPROM_PROGRESS_BAR->SetValue(false);
  CheckBoxEEPROM_PROGRESS_BAR->SetToolTip(_("Affiche une mini barre pour visualiser une sauvegarde en cours"));
  CheckBoxAUTOSWITCH = new wxCheckBox(Panel3, ID_CHECKBOX44, _("AUTOSWITCH"), wxPoint(336,88), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX44"));
  CheckBoxAUTOSWITCH->SetValue(false);
  CheckBoxAUTOSWITCH->SetToolTip(_("Auto detection des switchs lors de la programmatiion"));
  CheckBoxAUTOSOURCE = new wxCheckBox(Panel3, ID_CHECKBOX45, _("AUTOSOURCE"), wxPoint(336,56), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX45"));
  CheckBoxAUTOSOURCE->SetValue(false);
  CheckBoxAUTOSOURCE->SetFocus();
  CheckBoxAUTOSOURCE->SetToolTip(_("Auto detection des manches lors de la programmatiion"));
  CheckBoxDBLKEYS = new wxCheckBox(Panel3, ID_CHECKBOX47, _("Double keys"), wxPoint(336,152), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX47"));
  CheckBoxDBLKEYS->SetValue(false);
  CheckBoxDBLKEYS->SetToolTip(_("Raccourcis avec deux touches pour passer une valeur à min/max/0"));
  CheckBoxMULTI = new wxCheckBox(Panel3, ID_CHECKBOX28, _("MULTI"), wxPoint(72,120), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX28"));
  CheckBoxMULTI->SetValue(false);
  CheckBoxMULTI->SetToolTip(_("Protocole MULTI"));
  Compilerpage2 = new wxButton(Panel3, ID_BUTTON1, _("Compiler"), wxPoint(680,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
  Sortirpage2 = new wxButton(Panel3, ID_BUTTON4, _("Sauvegarder et sortir"), wxPoint(512,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
  CheckBoxCC2500 = new wxCheckBox(Panel3, ID_CHECKBOX23, _("CC2500"), wxPoint(36,222), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX23"));
  CheckBoxCC2500->SetValue(false);
  CheckBoxCYRF6936 = new wxCheckBox(Panel3, ID_CHECKBOX24, _("CYRF6936"), wxPoint(36,248), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX24"));
  CheckBoxCYRF6936->SetValue(false);
  CheckBoxNRF24l01 = new wxCheckBox(Panel3, ID_CHECKBOX25, _("NRF24l01"), wxPoint(36,272), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX25"));
  CheckBoxNRF24l01->SetValue(false);
  CheckBoxNRF24l01->Disable();
  CheckBoxA7105 = new wxCheckBox(Panel3, ID_CHECKBOX29, _("A7105"), wxPoint(36,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX29"));
  CheckBoxA7105->SetValue(false);
  CheckBoxA7105->Disable();
  ChoiceCYRF6936PAG = new wxChoice(Panel3, ID_CHOICECYRF6936PAG, wxPoint(112,240), wxSize(40,8), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICECYRF6936PAG"));
  ChoiceCYRF6936PAG->SetSelection( ChoiceCYRF6936PAG->Append(_("0")) );
  ChoiceCYRF6936PAG->Append(_("10"));
  ChoiceCYRF6936PAG->Append(_("20"));
  ChoiceCYRF6936PAG->Append(_("22"));
  StaticText28 = new wxStaticText(Panel3, ID_STATICTEXT28, _("PA Gain (DB)"), wxPoint(98,192), wxDefaultSize, 0, _T("ID_STATICTEXT28"));
  Panel4 = new wxPanel(Notebook1, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
  StaticBox7 = new wxStaticBox(Panel4, ID_STATICBOX7, _("Nom des interrupteurs"), wxPoint(72,8), wxSize(424,328), 0, _T("ID_STATICBOX7"));
  StaticBox10 = new wxStaticBox(Panel4, ID_STATICBOX10, _("3POS"), wxPoint(208,32), wxSize(160,104), 0, _T("ID_STATICBOX10"));
  StaticBox8 = new wxStaticBox(Panel4, ID_STATICBOX8, _("Extra 3POS"), wxPoint(352,32), wxSize(136,104), 0, _T("ID_STATICBOX8"));
  ComboBoxEncodeurNo = new wxComboBox(Panel4, ID_COMBOBOX12, wxEmptyString, wxPoint(288,224), wxSize(54,-1), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX12"));
  ComboBoxEncodeurNo->SetSelection( ComboBoxEncodeurNo->Append(_("REn")) );
  ComboBoxEncodeurNo->Append(_("---"));
  ComboBoxswitch1 = new wxComboBox(Panel4, ID_COMBOBOX1, wxEmptyString, wxPoint(136,48), wxSize(54,21), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX1"));
  ComboBoxswitch1->SetSelection( ComboBoxswitch1->Append(_("THR")) );
  ComboBoxswitch1->Append(_("---"));
  Sortirpage3 = new wxButton(Panel4, ID_BUTTON5, _("Sauvegarder et sortir"), wxPoint(544,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON5"));
  Compilerpage3 = new wxButton(Panel4, ID_BUTTON6, _("Compiler"), wxPoint(688,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
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
  CheckBoxPERSONAMES = new wxCheckBox(Panel4, ID_CHECKBOX22, _("PERSONAMES"), wxPoint(240,280), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX22"));
  CheckBoxPERSONAMES->SetValue(false);
  StaticText13 = new wxStaticText(Panel4, ID_STATICTEXT13, _("Inter1"), wxPoint(88,56), wxDefaultSize, 0, _T("ID_STATICTEXT13"));
  StaticText14 = new wxStaticText(Panel4, ID_STATICTEXT14, _("Inter2"), wxPoint(88,96), wxDefaultSize, 0, _T("ID_STATICTEXT14"));
  StaticText15 = new wxStaticText(Panel4, ID_STATICTEXT15, _("Inter3"), wxPoint(88,136), wxDefaultSize, 0, _T("ID_STATICTEXT15"));
  StaticText16 = new wxStaticText(Panel4, ID_STATICTEXT16, _("Inter4"), wxPoint(88,176), wxDefaultSize, 0, _T("ID_STATICTEXT16"));
  StaticText17 = new wxStaticText(Panel4, ID_STATICTEXT17, _("Inter5"), wxPoint(88,216), wxDefaultSize, 0, _T("ID_STATICTEXT17"));
  StaticText18 = new wxStaticText(Panel4, ID_STATICTEXT18, _("Inter6"), wxPoint(88,256), wxDefaultSize, 0, _T("ID_STATICTEXT18"));
  ButtonDefaut = new wxButton(Panel4, ID_BUTTON7, _("Défaut"), wxPoint(392,272), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
  StaticText25 = new wxStaticText(Panel4, ID_STATICTEXT25, _("ID0"), wxPoint(248,56), wxDefaultSize, 0, _T("ID_STATICTEXT25"));
  StaticText19 = new wxStaticText(Panel4, ID_STATICTEXT19, _("XD0"), wxPoint(376,56), wxDefaultSize, 0, _T("ID_STATICTEXT19"));
  ComboBoxXD0 = new wxComboBox(Panel4, ID_COMBOBOX14, wxEmptyString, wxPoint(416,48), wxSize(54,-1), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX14"));
  ComboBoxXD0->SetSelection( ComboBoxXD0->Append(_("XD0")) );
  ComboBoxXD0->Append(_("---"));
  ComboBoxXD2 = new wxComboBox(Panel4, ID_COMBOBOX15, wxEmptyString, wxPoint(416,96), wxSize(54,-1), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX15"));
  ComboBoxXD2->SetSelection( ComboBoxXD2->Append(_("XD2")) );
  ComboBoxXD2->Append(_("---"));
  ComboBoxXD1 = new wxComboBox(Panel4, ID_COMBOBOX13, wxEmptyString, wxPoint(416,72), wxSize(54,-1), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX13"));
  ComboBoxXD1->SetSelection( ComboBoxXD1->Append(_("XD1")) );
  ComboBoxXD1->Append(_("---"));
  ComboBoxID0 = new wxComboBox(Panel4, ID_COMBOBOX7, wxEmptyString, wxPoint(288,48), wxSize(54,-1), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX7"));
  ComboBoxID0->SetSelection( ComboBoxID0->Append(_("ID0")) );
  ComboBoxID0->Append(_("---"));
  StaticText20 = new wxStaticText(Panel4, ID_STATICTEXT20, _("EncodeurA"), wxPoint(216,152), wxSize(64,16), 0, _T("ID_STATICTEXT20"));
  StaticText24 = new wxStaticText(Panel4, ID_STATICTEXT24, _("EncodeurB"), wxPoint(216,192), wxSize(64,13), 0, _T("ID_STATICTEXT24"));
  StaticText21 = new wxStaticText(Panel4, ID_STATICTEXT21, _(" Encodeur à 0"), wxPoint(216,232), wxDefaultSize, 0, _T("ID_STATICTEXT21"));
  ComboBoxEncodeurA = new wxComboBox(Panel4, ID_COMBOBOX8, wxEmptyString, wxPoint(288,144), wxSize(54,-1), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX8"));
  ComboBoxEncodeurA->SetSelection( ComboBoxEncodeurA->Append(_("REa")) );
  ComboBoxEncodeurA->Append(_("---"));
  ComboBoxEncodeurB = new wxComboBox(Panel4, ID_COMBOBOX9, wxEmptyString, wxPoint(288,184), wxSize(54,-1), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX9"));
  ComboBoxEncodeurB->SetSelection( ComboBoxEncodeurB->Append(_("REb")) );
  ComboBoxEncodeurB->Append(_("---"));
  ComboBoxID1 = new wxComboBox(Panel4, ID_COMBOBOX10, wxEmptyString, wxPoint(288,72), wxSize(54,0), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX10"));
  ComboBoxID1->SetSelection( ComboBoxID1->Append(_("ID1")) );
  ComboBoxID1->Append(_("---"));
  ComboBoxID2 = new wxComboBox(Panel4, ID_COMBOBOX11, wxEmptyString, wxPoint(288,96), wxSize(54,0), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX11"));
  ComboBoxID2->SetSelection( ComboBoxID2->Append(_("ID2")) );
  ComboBoxID2->Append(_("---"));
  StaticText26 = new wxStaticText(Panel4, ID_STATICTEXT26, _("ID1"), wxPoint(248,80), wxDefaultSize, 0, _T("ID_STATICTEXT26"));
  StaticText22 = new wxStaticText(Panel4, ID_STATICTEXT22, _("XD1"), wxPoint(376,80), wxDefaultSize, 0, _T("ID_STATICTEXT22"));
  StaticText27 = new wxStaticText(Panel4, ID_STATICTEXT27, _("ID2"), wxPoint(248,104), wxDefaultSize, 0, _T("ID_STATICTEXT27"));
  StaticText23 = new wxStaticText(Panel4, ID_STATICTEXT23, _("XD2"), wxPoint(376,104), wxDefaultSize, 0, _T("ID_STATICTEXT23"));
  Panel5 = new wxPanel(Notebook1, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL5"));
  CheckBox1 = new wxCheckBox(Panel5, ID_CHECKBOX4, _("X-Angle"), wxPoint(64,96), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
  CheckBox1->SetValue(false);
  StaticBox11 = new wxStaticBox(Panel5, ID_STATICBOX11, _("Canal 1"), wxPoint(16,16), wxSize(360,264), 0, _T("ID_STATICBOX11"));
  CheckBox2 = new wxCheckBox(Panel5, ID_CHECKBOX8, _("X-Angle"), wxPoint(440,96), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX8"));
  CheckBox2->SetValue(false);
  StaticBox12 = new wxStaticBox(Panel5, ID_STATICBOX12, _("Canal 2"), wxPoint(392,16), wxSize(392,264), 0, _T("ID_STATICBOX12"));
  XSwitch_Channel1_CheckBox = new wxCheckBox(Panel5, ID_CHECKBOX2, _("X-Switch"), wxPoint(64,64), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
  XSwitch_Channel1_CheckBox->SetValue(false);
  XSwitch_Channel2_CheckBox = new wxCheckBox(Panel5, ID_CHECKBOX3, _("X-Switch"), wxPoint(440,64), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
  XSwitch_Channel2_CheckBox->SetValue(false);
  Button1 = new wxButton(Panel5, ID_BUTTON8, _("Sauvegarder et sortir"), wxPoint(112,304), wxSize(168,23), 0, wxDefaultValidator, _T("ID_BUTTON8"));
  Button2 = new wxButton(Panel5, ID_BUTTON9, _("Compiler"), wxPoint(488,304), wxSize(136,23), 0, wxDefaultValidator, _T("ID_BUTTON9"));
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
  Connect(ID_CHECKBOX29,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxPERSONAMESClick);
  Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonEXITClick);
  Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonCOMPILEClick);
  Connect(ID_CHECKBOX22,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBoxPERSONAMESClick);
  Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonDefautClick1);
  Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonEXITClick);
  Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonCOMPILEClick);
  Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&CompilerOptionsFrame::OnClose);
  //*)

  {
    wxIcon FrameIcon;
    SetIcon(wxICON(oavrc_icon));
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
  CheckBoxHAPTIC->SetValue(HAPTIC);
  ChoicePPM_UNIT->SetStringSelection(PPM_UNIT);
  CheckBoxGPS->SetValue(GPS);
  CheckBoxVARIO->SetValue(VARIO);
  CheckBoxPPM->SetValue(PPM);
  CheckBoxMULTI->SetValue(MULTI);
  CheckBoxDSM2SERIAL->SetValue(DSM2SERIAL);
  CheckBoxSPIRF->SetValue(SPIRF);
  CheckBoxCC2500->SetValue(CC2500);
  CheckBoxCYRF6936->SetValue(CYRF6936);
  CheckBoxNRF24l01->SetValue(NRF24l01);
  CheckBoxA7105->SetValue(A7105);
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
  ComboBoxXD0->SetMaxLength(3);
  ComboBoxXD1->SetMaxLength(3);
  ComboBoxXD2->SetMaxLength(3);

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
  ComboBoxXD0->SetValue(switchXD0);
  ComboBoxXD1->SetValue(switchXD1);
  ComboBoxXD2->SetValue(switchXD2);
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
  if (HAPTIC) CompiBat += (" HAPTIC=YES");// default should be NO
  CompiBat += (" PPM_UNIT=" + PPM_UNIT);
  if (GPS) CompiBat += (" GPS=YES");// default should be NO
  if (VARIO) CompiBat += (" VARIO=YES");// default should be NO
  if (PPM) CompiBat += (" PPM=YES");// default should be YES
  if (MULTI) CompiBat += (" MULTIMODULE=YES");// default should be NO
  if (DSM2SERIAL) CompiBat += (" DSM2=SERIAL");// default should be NO
  if (SPIRF) {
      CompiBat += (" SPIMODULES=YES");// default should be NO
      if (CC2500) { CompiBat += (" CC2500=YES");// default should be NO
      CompiBat += (" CC2500PAG=" + CC2500PAG);
      }
      if (CYRF6936) { CompiBat += (" CYRF6936=YES");// default should be NO
      CompiBat += (" CYRF6936PAG=" + CYRF6936PAG);
      }
      if (NRF24l01) { CompiBat += (" NRF24l01=YES");// default should be NO
      CompiBat += (" NRF24l01PAG=" + NRF24l01PAG);
      }
      if (A7105) { CompiBat += (" A7105=YES");// default should be NO
      CompiBat += (" A7105PAG=" + A7105PAG);
      }
  }
  if (SD_CARD) CompiBat += (" SDCARD=YES");// default should be NO
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
  if (PERSONAMES)
    {
      CompiBat += (" PERSONAMES=YES");
      CreatePersonames_H(); // Create the header file
    }
  wxMessageBox(CompiBat);
  CreateCompileBatFile(CompiBat);
  wxExecute(AppPath+ "\\CompileBatFile.bat",wxEXEC_ASYNC );// Create firmware
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
  HAPTIC = CheckBoxHAPTIC->GetValue();
  PPM_UNIT  = ChoicePPM_UNIT->GetString(ChoicePPM_UNIT->GetSelection());
  GPS = CheckBoxGPS->GetValue();
  VARIO = CheckBoxVARIO->GetValue();
  PPM = CheckBoxPPM->GetValue();
  MULTI = CheckBoxMULTI->GetValue();
  DSM2SERIAL = CheckBoxDSM2SERIAL->GetValue();
  CC2500 = CheckBoxCC2500->GetValue();
  CYRF6936 = CheckBoxCYRF6936->GetValue();
  NRF24l01 = CheckBoxNRF24l01->GetValue();
  A7105 = CheckBoxA7105->GetValue();
  CC2500PAG = ChoiceCC2500PAG->GetString(ChoiceCC2500PAG->GetSelection());
  CYRF6936PAG = ChoiceCYRF6936PAG->GetString(ChoiceCYRF6936PAG->GetSelection());
  NRF24l01PAG = ChoiceNRF24l01PAG->GetString(ChoiceNRF24l01PAG->GetSelection());
  A7105PAG = ChoiceA7105PAG->GetString(ChoiceA7105PAG->GetSelection());
  SPIRF = 0 | CC2500 | CYRF6936 | NRF24l01 | A7105;
  CheckBoxSPIRF->SetValue(SPIRF);

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
  switchXD0 = ComboBoxXD0->GetValue();
  switchXD1 = ComboBoxXD1->GetValue();
  switchXD2 = ComboBoxXD2->GetValue();

  wxString switchArray[] = {switch1, switch2, switch3, switch4, switch5, switch6, switchID0, switchID1, switchID2, encoderA, encoderB, encoderNo, switchXD0, switchXD1, switchXD2};

  for (uint16_t i=0; i<15; i++)
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

}
