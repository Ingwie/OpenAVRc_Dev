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


#include "OpenAVRc_DesktopMain.h"
#include "CommunicationsFrame.h"
#include "CompilerOptionsFrame.h"
#include "Voice_choice.h"
#include "VoiceEditFrame.h"
#include "BluetoothFrame.h"
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/textdlg.h>
#include <wx/aboutdlg.h>
#include <wx/volume.h>
#include <wx/choicdlg.h>
#include <wx/busyinfo.h>

#include "Bluetooth.xpm"

//(*InternalHeaders(OpenAVRc_DesktopFrame)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)


//helper functions
enum wxbuildinfoformat {
  short_f, long_f
};

wxString wxbuildinfo(wxbuildinfoformat format)
{
  wxString wxbuild(wxVERSION_STRING);

  if (format == long_f ) {
#if defined(__WXMSW__)
    wxbuild << _T("-Windows");
#elif defined(__UNIX__)
    wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
    wxbuild << _T("-Unicode build");
#else
    wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
  }

  return wxbuild;
}

//Splash
uint8_t LbmSplashOriginal[] = {
#include "../OpenAVRc/bitmaps/splash.lbm"
};
bool personalSplash = false;
uint8_t LbmSplash[SPLASHLENGHT] = {0};
#define LcdScale 2

//Global Var
bool Ini_Changed = false;
wxString AppPath;
//Voice
long Numvoice = 0;
wxString avrdudepath = _("non défini");
wxString dude_programmer = _("non défini");
wxString dude_type = _("non défini");
wxString dude_port = _("non défini");
//wxString COMM = _("non défini");
wxString voice_Langue = "ES"; //TTS langue
//Const
wxString dude_c = (" -c ");
wxString dude_p = (" -p ");
wxString dude_D = (" -D ");
wxString dude_P = (" -P ");
wxString dude_space = (" ");
wxString dude_U = (" -U ");
wxString dude_ComPrefix = ("\\\\.\\");
wxString dude_eeprom = ("eeprom:");
wxString dude_flash = ("flash:");
wxString dude_raw = (":r");
wxString dude_write = ("w:");
wxString dude_read = ("r:");
wxString dude_intel = (":i");
wxString keepopen = ("cmd /k ");


//Define defaults
wxString PCB = _("non défini");
wxString LCD = _("non défini");
bool LCDROT180 = 0;
wxString VOICE = ("NO");
wxString EXT = ("STD");
bool AUDIO = 0;
bool HELI = 0;
wxString TTS = _("non défini");
wxString TRANSLATIONS = _("non défini");
wxString NAVIGATION = ("NO");
bool HAPTIC = 0;
wxString PPM_UNIT = ("PERCENT_PREC1");
bool GPS = 0;
bool VARIO = 0;
bool PPM = 1;
wxString SERIAL_PROTOCOL = ("NO");
wxString PCM_PROTOCOL = ("NO");
bool CC2500 = 0;
bool CYRF6936 = 0;
bool NRF24l01 = 0;
bool A7105 = 0;
wxString CC2500PAG = ("0");
wxString CYRF6936PAG = ("0");
wxString NRF24l01PAG = ("0");
wxString A7105PAG = ("0");
bool SD_CARD = 0;
bool TEMPLATES = 0;
wxString THREE_POS = ("NO");
bool SPLASH =0;
wxString UNITS =("METRIC");
wxString DEFAULT_MODE =("NO");
wxString FONT = ("STD");
bool BOLD = 0;
bool BATTGRAPH = 0;
bool EEPROM_PROGRESS_BAR = 0;
wxString FAI = ("NO");
bool AUTOSWITCH = 0;
bool AUTOSOURCE = 0;
bool DBLKEYS = 0;
bool PPM_CENTER_ADJUSTABLE = 0;
bool PPM_LIMITS_SYMETRICAL = 0;
bool FLIGHT_MODES = 0;
bool CURVES = 0;
bool GVARS = 0;
bool CORRECT_NEGATIVE_VALUES = 0;
bool ARITHMETIC_OVERFLOW_CHECK = 0;
bool ACCURAT_THROTTLE_STATS = 0;
bool PWM_BACKLIGHT = 0;
bool OVERRIDE_CHANNEL_FUNCTION = 0;
bool WS_HOW_HIGH = 0;
bool TOGGLETRIM = 0;
bool NOANDSECONDE = 1;
bool SHUTDOWN_CONFIRMATION = 0;
bool FRAM = 0;
bool PERSONAMES = 0;
bool DBLGAZSTICK = 0;
wxString OTHERCOMPOPTIONS = "";
bool INV_STICK_RH = 0;
bool INV_STICK_LV = 0;
bool INV_STICK_RV = 0;
bool INV_STICK_LH = 0;
wxString BLUETOOTH = ("NO");
bool XMODEM = 0;
bool SCC = 0;

wxString switch1 = ("THR");
wxString switch2 = ("RUD");
wxString switch3 = ("ELE");
wxString switch4 = ("AIL");
wxString switch5 = ("GEA");
wxString switch6 = ("TRN");
wxString switchID0 = ("ID0");
wxString switchID1 = ("ID1");
wxString switchID2 = ("ID2");
wxString encoderA  = ("ENA");
wxString encoderB  = ("ENB");
wxString encoderNo = ("EN0");
wxString switchXD0 = ("XD0");
wxString switchXD1 = ("XD1");
wxString switchXD2 = ("XD2");
wxString stickRud = ("Rud");
wxString stickEle = ("Ele");
wxString stickThr = ("Thr");
wxString stickAil = ("Ail");

wxString NUMXANY = ("NO");

//(*IdInit(OpenAVRc_DesktopFrame)
const long OpenAVRc_DesktopFrame::ID_STATICBOXSPLASH = wxNewId();
const long OpenAVRc_DesktopFrame::ID_STATICBOXCONFIG = wxNewId();
const long OpenAVRc_DesktopFrame::ID_BUTTONSIMUSTD = wxNewId();
const long OpenAVRc_DesktopFrame::ID_BUTTONSIMUFRAM = wxNewId();
const long OpenAVRc_DesktopFrame::ID_LISTBOXCONFIG = wxNewId();
const long OpenAVRc_DesktopFrame::ID_SPLASH = wxNewId();
const long OpenAVRc_DesktopFrame::ID_BUTTONPERSO = wxNewId();
const long OpenAVRc_DesktopFrame::ID_BUTTONSPLASHDEFAULT = wxNewId();
const long OpenAVRc_DesktopFrame::ID_STATICBOX1 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_CHOICE1 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_STATICTEXT1 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_BUTTON2 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_BUTTON4 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_PANEL1 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEMNEWCONFIG = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUDELETEACTIVECONFIG = wxNewId();
const long OpenAVRc_DesktopFrame::idMenuQuit = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEMCOMMUNICATION = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUVOICECHOICE = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM3 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM5 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM4 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM6 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM9 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM10 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM13 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM7 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUBLUETOOTH = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUCOMPILOMATIC = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM11 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM14 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM16 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM8 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM12 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM15 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM2 = wxNewId();
const long OpenAVRc_DesktopFrame::idMenuAbout = wxNewId();
const long OpenAVRc_DesktopFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(OpenAVRc_DesktopFrame,wxFrame)
  //(*EventTable(OpenAVRc_DesktopFrame)
  //*)
END_EVENT_TABLE()


OpenAVRc_DesktopFrame::OpenAVRc_DesktopFrame(wxWindow* parent,wxWindowID id)
{
  //(*Initialize(OpenAVRc_DesktopFrame)
  wxMenu* Menu1;
  wxMenu* Menu2;
  wxMenuItem* MenuItem1;
  wxMenuItem* MenuItem2;

  Create(parent, wxID_ANY, _("OpenAVRc Desktop V3"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
  SetClientSize(wxSize(592,282));
  Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxSize(848,280), 0, _T("ID_PANEL1"));
  Panel1->SetFocus();
  StaticBoxSplash = new wxStaticBox(Panel1, ID_STATICBOXSPLASH, _("Ecran d\'accueil"), wxPoint(160,8), wxSize(272,192), 0, _T("ID_STATICBOXSPLASH"));
  StaticBoxConfig = new wxStaticBox(Panel1, ID_STATICBOXCONFIG, _("Configuration"), wxPoint(16,8), wxSize(136,192), 0, _T("ID_STATICBOXCONFIG"));
  ButtonSimuSTD = new wxButton(Panel1, ID_BUTTONSIMUSTD, _("SIMULATEUR STANDARD"), wxPoint(24,204), wxSize(248,31), 0, wxDefaultValidator, _T("ID_BUTTONSIMUSTD"));
  ButtonSimuFram = new wxButton(Panel1, ID_BUTTONSIMUFRAM, _("SIMULATEUR F-RAM"), wxPoint(320,204), wxSize(248,31), 0, wxDefaultValidator, _T("ID_BUTTONSIMUFRAM"));
  ListBoxConfig = new wxListBox(Panel1, ID_LISTBOXCONFIG, wxPoint(24,32), wxSize(120,160), 0, 0, wxLB_SINGLE|wxBORDER_DOUBLE|wxVSCROLL, wxDefaultValidator, _T("ID_LISTBOXCONFIG"));
  PanelSplash = new wxPanel(Panel1, ID_SPLASH, wxPoint(169,64), wxSize(256,128), wxBORDER_NONE, _T("ID_SPLASH"));
  PanelSplash->SetBackgroundColour(wxColour(255,255,255));
  ButtonPerso = new wxButton(Panel1, ID_BUTTONPERSO, _("Personnaliser"), wxPoint(192,32), wxSize(80,24), 0, wxDefaultValidator, _T("ID_BUTTONPERSO"));
  ButtonPerso->SetToolTip(_("Attention : Tous ce qui n\'est pas blanc sera converti en noir"));
  ButtonSplashDefault = new wxButton(Panel1, ID_BUTTONSPLASHDEFAULT, _("Par défaut"), wxPoint(320,32), wxSize(80,24), 0, wxDefaultValidator, _T("ID_BUTTONSPLASHDEFAULT"));
  ButtonSplashDefault->SetToolTip(_("Restaurer l\'écran par défaut"));
  StaticBox1 = new wxStaticBox(Panel1, ID_STATICBOX1, _("Fichiers Voix"), wxPoint(440,8), wxSize(136,144), 0, _T("ID_STATICBOX1"));
  ChoiceLangue = new wxChoice(Panel1, ID_CHOICE1, wxPoint(520,32), wxSize(40,24), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
  ChoiceLangue->SetSelection( ChoiceLangue->Append(_("CZ")) );
  ChoiceLangue->Append(_("DE"));
  ChoiceLangue->Append(_("EN"));
  ChoiceLangue->Append(_("ES"));
  ChoiceLangue->Append(_("FR"));
  ChoiceLangue->Append(_("HU"));
  ChoiceLangue->Append(_("IT"));
  ChoiceLangue->Append(_("NL"));
  ChoiceLangue->Append(_("PL"));
  ChoiceLangue->Append(_("PT"));
  ChoiceLangue->Append(_("SE"));
  ChoiceLangue->Append(_("SK"));
  ChoiceLangue->SetToolTip(_("Choisissez la langue des mensages vocales"));
  StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Langue"), wxPoint(448,40), wxSize(64,16), wxALIGN_RIGHT, _T("ID_STATICTEXT1"));
  StaticText1->SetToolTip(_("Choisissez la langue des mensages vocales"));
  ButtonEditeur = new wxButton(Panel1, ID_BUTTON2, _("Éditeur"), wxPoint(480,72), wxSize(80,24), 0, wxDefaultValidator, _T("ID_BUTTON2"));
  ButtonEditeur->SetToolTip(_("Pour changer les messages"));
  ButtonCarteSD = new wxButton(Panel1, ID_BUTTON4, _("Carte SD"), wxPoint(480,112), wxSize(80,24), 0, wxDefaultValidator, _T("ID_BUTTON4"));
  ButtonCarteSD->SetHelpText(_("Écriture dans la carte SD"));
  MenuBar_main = new wxMenuBar();
  Menu1 = new wxMenu();
  MenuNewconfig = new wxMenuItem(Menu1, ID_MENUITEMNEWCONFIG, _("Nouvelle configuration"), wxEmptyString, wxITEM_NORMAL);
  MenuNewconfig->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_NEW")),wxART_MENU));
  Menu1->Append(MenuNewconfig);
  MenuDeleteActiveConfig = new wxMenuItem(Menu1, ID_MENUDELETEACTIVECONFIG, _("Supprimer la configuration active"), wxEmptyString, wxITEM_NORMAL);
  MenuDeleteActiveConfig->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_CROSS_MARK")),wxART_MENU));
  Menu1->Append(MenuDeleteActiveConfig);
  MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quitter\tAlt-F4"), _("Quitter l\'application"), wxITEM_NORMAL);
  MenuItem1->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_QUIT")),wxART_MENU));
  Menu1->Append(MenuItem1);
  MenuBar_main->Append(Menu1, _("Fichier"));
  Menu3 = new wxMenu();
  MenuItem3 = new wxMenuItem(Menu3, ID_MENUITEMCOMMUNICATION, _("Programmateur"), wxEmptyString, wxITEM_NORMAL);
  MenuItem3->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_EXECUTABLE_FILE")),wxART_OTHER));
  Menu3->Append(MenuItem3);
  MenuChoiceVoice = new wxMenuItem(Menu3, ID_MENUVOICECHOICE, _("Synthèse vocale"), wxEmptyString, wxITEM_NORMAL);
  MenuChoiceVoice->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_HELP")),wxART_OTHER));
  Menu3->Append(MenuChoiceVoice);
  MenuBar_main->Append(Menu3, _("Paramètres"));
  Menu4 = new wxMenu();
  Menu4->AppendSeparator();
  Menu5 = new wxMenuItem(Menu4, ID_MENUITEM3, _("Lire les réglages et les modèles de la radio (eeprom)"), wxEmptyString, wxITEM_NORMAL);
  Menu5->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_FORWARD")),wxART_MENU));
  Menu4->Append(Menu5);
  MenuItem6 = new wxMenuItem(Menu4, ID_MENUITEM5, _("Lire le firmware de la radio"), wxEmptyString, wxITEM_NORMAL);
  MenuItem6->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_FORWARD")),wxART_MENU));
  Menu4->Append(MenuItem6);
  Menu4->AppendSeparator();
  MenuItem5 = new wxMenuItem(Menu4, ID_MENUITEM4, _("Écrire les réglages et les modèles dans la radio (eeprom)"), wxEmptyString, wxITEM_NORMAL);
  MenuItem5->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_BACK")),wxART_MENU));
  Menu4->Append(MenuItem5);
  MenuItem7 = new wxMenuItem(Menu4, ID_MENUITEM6, _("Écrire le firmware dans la radio"), wxEmptyString, wxITEM_NORMAL);
  MenuItem7->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_BACK")),wxART_MENU));
  Menu4->Append(MenuItem7);
  Menu4->AppendSeparator();
  MenuItem8 = new wxMenu();
  MenuItem10 = new wxMenuItem(MenuItem8, ID_MENUITEM9, _("Écrire les fusibles"), wxEmptyString, wxITEM_NORMAL);
  MenuItem10->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_BACK")),wxART_OTHER));
  MenuItem8->Append(MenuItem10);
  MenuItem11 = new wxMenuItem(MenuItem8, ID_MENUITEM10, _("Écrire le bootloader"), wxEmptyString, wxITEM_NORMAL);
  MenuItem11->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_BACK")),wxART_OTHER));
  MenuItem8->Append(MenuItem11);
  MenuItem13 = new wxMenuItem(MenuItem8, ID_MENUITEM13, _("Écrire le bootloader F-RAM"), wxEmptyString, wxITEM_NORMAL);
  MenuItem13->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_GO_BACK")),wxART_MENU));
  MenuItem8->Append(MenuItem13);
  Menu4->Append(ID_MENUITEM7, _("Fusibles et Bootloader"), MenuItem8, wxEmptyString);
  Menu4->AppendSeparator();
  MenuBluetooth = new wxMenuItem(Menu4, ID_MENUBLUETOOTH, _("Bluetooth"), wxEmptyString, wxITEM_NORMAL);
  Menu4->Append(MenuBluetooth);
  MenuBar_main->Append(Menu4, _("Lire/Écrire"));
  Menu7 = new wxMenu();
  CompilOMatic = new wxMenuItem(Menu7, ID_MENUCOMPILOMATIC, _("Compil-O-matic"), wxEmptyString, wxITEM_NORMAL);
  CompilOMatic->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_EXECUTABLE_FILE")),wxART_MENU));
  Menu7->Append(CompilOMatic);
  MenuBar_main->Append(Menu7, _("Compilateur"));
  Menu2 = new wxMenu();
  MenuHtmlDoc = new wxMenu();
  MenuItem9 = new wxMenuItem(MenuHtmlDoc, ID_MENUITEM11, _("OpenAVRc Manual (JPZ)"), wxEmptyString, wxITEM_NORMAL);
  MenuHtmlDoc->Append(MenuItem9);
  MenuItem14 = new wxMenuItem(MenuHtmlDoc, ID_MENUITEM14, _("Mégamini shield"), wxEmptyString, wxITEM_NORMAL);
  MenuHtmlDoc->Append(MenuItem14);
  MenuItem16 = new wxMenuItem(MenuHtmlDoc, ID_MENUITEM16, _("Mégamini schema"), wxEmptyString, wxITEM_NORMAL);
  MenuHtmlDoc->Append(MenuItem16);
  MenuItem4 = new wxMenuItem(MenuHtmlDoc, ID_MENUITEM8, _("JQ6500 PCB (by Pyrall)"), wxEmptyString, wxITEM_NORMAL);
  MenuItem4->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_MENU));
  MenuHtmlDoc->Append(MenuItem4);
  MenuItem12 = new wxMenuItem(MenuHtmlDoc, ID_MENUITEM12, _("VOICE/AUDIO PCB et RTC (by Pyrall)"), wxEmptyString, wxITEM_NORMAL);
  MenuItem12->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_MENU));
  MenuHtmlDoc->Append(MenuItem12);
  MenuItem15 = new wxMenuItem(MenuHtmlDoc, ID_MENUITEM15, _("Autres links"), wxEmptyString, wxITEM_NORMAL);
  MenuHtmlDoc->Append(MenuItem15);
  Menu2->Append(ID_MENUITEM2, _("Documentation en ligne"), MenuHtmlDoc, wxEmptyString);
  MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("A propos\tF1"), wxEmptyString, wxITEM_NORMAL);
  MenuItem2->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_INFORMATION")),wxART_MENU));
  Menu2->Append(MenuItem2);
  MenuBar_main->Append(Menu2, _("Aide"));
  SetMenuBar(MenuBar_main);
  StatusBar_main = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
  int __wxStatusBarWidths_1[1] = { -1 };
  int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
  StatusBar_main->SetFieldsCount(1,__wxStatusBarWidths_1);
  StatusBar_main->SetStatusStyles(1,__wxStatusBarStyles_1);
  SetStatusBar(StatusBar_main);

  Connect(ID_BUTTONSIMUSTD,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnButtonSimuSTDClick);
  Connect(ID_BUTTONSIMUFRAM,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnButtonSimuFramClick);
  Connect(ID_LISTBOXCONFIG,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnListBoxConfigDClick);
  Connect(ID_LISTBOXCONFIG,wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnListBoxConfigDClick);
  PanelSplash->Connect(wxEVT_PAINT,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnPanelSplashPaint,0,this);
  Connect(ID_BUTTONPERSO,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnButtonPersoClick);
  Connect(ID_BUTTONSPLASHDEFAULT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnButtonSplashDefaultClick);
  Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnChoiceLangueSelect);
  Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnEDITEURClick);
  Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnButtonCarteSDClick);
  Connect(ID_MENUITEMNEWCONFIG,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnMenuNewconfigSelected);
  Connect(ID_MENUDELETEACTIVECONFIG,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnMenuDeleteActiveConfigSelected);
  Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnQuit);
  Connect(ID_MENUITEMCOMMUNICATION,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnProgrammerSelected);
  Connect(ID_MENUVOICECHOICE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnMenuChoiceVoiceSelected);
  Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnreadmodelsSelected);
  Connect(ID_MENUITEM5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnreadfirmwareSelected);
  Connect(ID_MENUITEM4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnWriteModelToRadioSelected);
  Connect(ID_MENUITEM6,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnWriteFirmwareToRadioSelected);
  Connect(ID_MENUITEM9,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnEcrirelesFuseesSelected);
  Connect(ID_MENUITEM10,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnEcrirelebootloaderSelected);
  Connect(ID_MENUITEM13,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnEcrirelebootloaderF_RAMSelected);
  Connect(ID_MENUBLUETOOTH,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnMenuBluetoothSelected);
  Connect(ID_MENUCOMPILOMATIC,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnCompilOMaticSelected);
  Connect(ID_MENUITEM11,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnMenuItem9Selected);
  Connect(ID_MENUITEM14,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnMenuItem14Selected);
  Connect(ID_MENUITEM16,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnMenuItem16Selected);
  Connect(ID_MENUITEM8,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnMenuJQ6500_PCBSelected);
  Connect(ID_MENUITEM12,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnMenuVOICE_AUDIO_PCBSelected);
  Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnAbout);
  Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnClose);
  //*)

  {
    SetIcon(wxICON(oavrc_icon));
  }
  MenuBluetooth->SetBitmap(wxBitmap(wxImage(Bluetooth_icon)));

  //App Path
  wxFileName appPathWithExeName = wxStandardPaths::Get().GetExecutablePath();
  AppPath = (appPathWithExeName.GetPath());


  defaut = wxT("Defaut");
  //Ini File
  Ini_Filename = wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator() + "OpenAVRcDesktopV3.ini";
  configFile = new wxFileConfig( "", "", Ini_Filename);
  LoadConfig("");
  if (avrdudepath == _("non défini"))
    wxMessageBox( _("Merci de vérifier les paramètres"), _("Programmeur :"), wxICON_WARNING | wxCENTRE);//Ini File
  if (SavedConfig.GetCount() == 0) {
    SavedConfig.Add(defaut,1);
    Profil = defaut;
  }
  ListBoxConfig->InsertItems(SavedConfig,0);
  ListBoxConfig->SetStringSelection(Profil);

}


OpenAVRc_DesktopFrame::~OpenAVRc_DesktopFrame()
{
  //(*Destroy(OpenAVRc_DesktopFrame)
  //*)
}

void OpenAVRc_DesktopFrame::OnQuit(wxCommandEvent& event)
{
  Close();
}

void OpenAVRc_DesktopFrame::OnAbout(wxCommandEvent& event)
{
  wxAboutDialogInfo Aboutbox;
  Aboutbox.SetName("OpenAVRc Desktop");
  Aboutbox.SetVersion("V 3.10 Beta");
  Aboutbox.SetLicence(_(" GPLv2 . Firmware basé sur NextStepRc 2.18 "));
  Aboutbox.SetDescription(_("Logiciel pour la personnalisation, la compilation, le flashage, la sauvegarde de votre radio OpenAVRc     "));
  Aboutbox.SetCopyright(wxT("(C) 2016-2022 OpenAVRc Team"));
  Aboutbox.SetWebSite(wxT("https://github.com/Ingwie/OpenAVRc_Dev"));
  Aboutbox.AddDeveloper(wxT(
    "OpenAVRc Team :\n\n"
    "Firmware : Bracame, Payalneg, Pierrotm777, RC-Navy, Sloped Soarer, Supiiik.\n"
    "Applications : Bracame, Mentero.\n"
    "PCB Shield : Anthobreizh, Pierrotm777, Pyrall.\n"
    "Documentation : JPZ(Testeur68), Pierrotm777, Pyrall.\n"
    "Translations : Mentero, StanekTM.\n"
    "Beta tests : JPZ(Testeur68), Pyrall.\n\n"
    "Forum contributors :\n\n"
    "ElectoPete (English Voice file).\n"
    "Ievgen (Ukrainian translation).\n"
    "TomTheBuilder (English documentation translation).\n"
    "IvanH (Slovak firmware translation)."
  ));

  wxAboutBox(Aboutbox);
}

void OpenAVRc_DesktopFrame::OnProgrammerSelected(wxCommandEvent& event)
{
  CommunicationsFrame * DudeFrame = new CommunicationsFrame(this);
  DudeFrame->Show(TRUE);//opens CommunicationsFrame
  MenuBar_main->Enable(ID_MENUITEMCOMMUNICATION, false);
}

void OpenAVRc_DesktopFrame::EnableProgrammerSelectedMenu()
{
  MenuBar_main->Enable(ID_MENUITEMCOMMUNICATION, true);
}

bool OpenAVRc_DesktopFrame::CheckIfSerialAvailable()
{
  if (dude_port == "usb")
  {
    wxMessageBox(_(" Opération annulée \n\nCette opération nécessite un port COM."));
    return false;
  }
  return true;
}

void OpenAVRc_DesktopFrame::OnreadmodelsSelected(wxCommandEvent& event)//READ MODELS FROM RADIO.
{
  if (CheckIfSerialAvailable())
  {
    wxFileDialog saveDialog(this, _("Choisir le fichier pour importer les modèles des la radio."), AppPath + "\\eeprom\\", "", _("Fichiers BIN (*.bin)|*.bin|Tous (*.*)|*.*"), wxFD_SAVE);
    if (saveDialog.ShowModal() == wxID_CANCEL)
      return;
    wxString dude_tmpfile = (saveDialog.GetPath());
    wxString dude_send = keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_ComPrefix+dude_port+dude_U+dude_eeprom+dude_read+dude_tmpfile+dude_raw;
    wxExecute(dude_send);//send command
  }
}

void OpenAVRc_DesktopFrame::OnreadfirmwareSelected(wxCommandEvent& event)//read firmware from radio
{
  if (CheckIfSerialAvailable())
  {
    wxFileDialog saveDialog(this, _("Choisir le fichier pour importer le Firmware des la radio."), AppPath + "\\firmware\\", "", _("Fichiers HEX (*.hex)|*.hex"), wxFD_SAVE);
    if (saveDialog.ShowModal() == wxID_CANCEL)
      return;
    wxString dude_tmpfile = (saveDialog.GetPath());
    wxString dude_send = keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_ComPrefix+dude_port+dude_U+dude_flash+dude_read+dude_tmpfile+dude_intel;
    wxExecute(dude_send);
  }
}

void OpenAVRc_DesktopFrame::OnWriteModelToRadioSelected(wxCommandEvent& event)
{
  if (CheckIfSerialAvailable())
  {
    wxFileDialog openFileDialog(this, _("Choisir le fichier (.bin) pour transferer les modêles à la radio."), AppPath + "\\eeprom\\", "", _("Fichiers BIN (*.bin)|*.bin"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
      return;
    wxString dude_tmpfile = (openFileDialog.GetPath());
    wxString dude_send = keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_ComPrefix+dude_port+dude_U+dude_eeprom+dude_write+dude_tmpfile+dude_raw;
    wxExecute(dude_send);
  }
}

void OpenAVRc_DesktopFrame::OnWriteFirmwareToRadioSelected(wxCommandEvent& event)
{
  if (CheckIfSerialAvailable())
  {
    wxFileDialog openFileDialog(this, _("Choisir le fichier pour transferer le Firmware à la radio."), AppPath + "\\firmware\\", "", _("Fichiers HEX (*.hex)|*.hex"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
      return;
    wxMessageDialog *bkup = new wxMessageDialog(NULL,_("Il est recommande de sauvegarder vos modeles avant, voulez vous continuer ?"), wxT("Firmware"),wxOK | wxICON_WARNING | wxCANCEL | wxCANCEL_DEFAULT);
    if (bkup->ShowModal()!= wxID_OK)
      return;
    wxString dude_tmpfile = (openFileDialog.GetPath());//write firmware
    wxString dude_send =keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_ComPrefix+dude_port+dude_U+dude_flash+dude_write+dude_tmpfile+dude_intel;
    wxExecute(dude_send);
  }
}


void OpenAVRc_DesktopFrame::OnEcrirelesFuseesSelected(wxCommandEvent& event)// Write fuses
{
  wxMessageDialog *susto = new wxMessageDialog(NULL,
      _("Sûr? Vous voulez continuer?"), _("Programmation des fusibles"),
      wxOK | wxICON_WARNING | wxCANCEL | wxCANCEL_DEFAULT);
  susto->SetEventHandler(susto);
  if (susto->ShowModal()!= wxID_OK)
    return;
  wxString FUSES(" -F -e -u -Ulfuse:w:0xFF:m -Uhfuse:w:0xD8:m -Uefuse:w:0xFD:m -v");
  wxString dude_send = (keepopen+avrdudepath +dude_c+dude_programmer +dude_p+dude_type +dude_P+dude_port +FUSES);
  wxExecute(dude_send);
}

void OpenAVRc_DesktopFrame::OnEcrirelebootloaderSelected(wxCommandEvent& event) // Write bootloader
{
  if (!(dude_type.Cmp("m2560"))) { // Verify radio type selected

    wxMessageDialog *susto = new wxMessageDialog(NULL,
        _("Sûr? Vous voulez continuer?"), _("Programmation du Bootloader"),wxOK | wxICON_WARNING | wxCANCEL | wxCANCEL_DEFAULT);
    susto->SetEventHandler(susto);
    if (susto->ShowModal()!= wxID_OK)
      return;
    wxString BOOTLOADER(" -U lock:w:0x3F:m -U flash:w:OpenAVRcBootLoaderM2560.hex:i -V -U lock:w:0x0F:m");
    wxString dude_send = (keepopen+avrdudepath +dude_c+dude_programmer +dude_p+dude_type +dude_P+dude_port +BOOTLOADER);
    //wxMessageBox(dude_send);
    wxExecute(dude_send);
  } else {
    wxMessageDialog *susto = new wxMessageDialog(NULL,
        _("Mauvais type de radio selectionné"), _("Programmation du Bootloader"),wxICON_WARNING | wxOK);
    susto->SetEventHandler(susto);
    if (susto->ShowModal()== wxID_OK)
      return;
  }
}


void OpenAVRc_DesktopFrame::OnEcrirelebootloaderF_RAMSelected(wxCommandEvent& event)
{
  if (!(dude_type.Cmp("m2560fram"))) { // Verify radio type selected

    wxMessageDialog *susto = new wxMessageDialog(NULL,
        ("Sûr? Vous voulez continuer?"), _("Programmation du Bootloader"),wxOK | wxICON_WARNING | wxCANCEL | wxCANCEL_DEFAULT);
    susto->SetEventHandler(susto);
    if (susto->ShowModal()!= wxID_OK)
      return;
    wxString BOOTLOADER(" -U lock:w:0x3F:m -U flash:w:OpenAVRcBootLoaderM2560_FRAM.hex:i -V -U lock:w:0x0F:m");
    wxString dude_send = (keepopen+avrdudepath +dude_c+dude_programmer +dude_p+dude_type +dude_P+dude_port +BOOTLOADER);
    //wxMessageBox(dude_send);
    wxExecute(dude_send);
  } else {
    wxMessageDialog *susto = new wxMessageDialog(NULL,
        _("Mauvais type de radio selectionné"), _("Programmation du Bootloader"),wxICON_WARNING | wxOK);
    susto->SetEventHandler(susto);
    if (susto->ShowModal()== wxID_OK)
      return;
  }
}

void OpenAVRc_DesktopFrame::LoadConfig(wxString temp)
{

  configFile->SetPath("/");
  if (temp == "")
    configFile->Read(wxT("Latestprofil"),&Profil);
  else
    Profil = temp;
  configFile->SetPath("/PROFILS/");
  long dummy;
  bool again = configFile->GetFirstEntry(temp, dummy);
  while (again) {
    SavedConfig.Add(temp);
    again = configFile->GetNextEntry(temp, dummy);
  }
  configFile->SetPath("/"+Profil+"/");
  configFile->SetPath("COMM/");
  configFile->Read(wxT("Programmer"),&dude_programmer);
  configFile->Read(wxT("Port"),&dude_port);
  configFile->Read(wxT("Type"),&dude_type);
  configFile->Read(wxT("avrdudepath"),&avrdudepath);
  configFile->SetPath("/"+Profil+"/");
  configFile->SetPath("TTS/");
  configFile->Read(wxT("LANGTTS"),&voice_Langue);
  ChoiceLangue->SetStringSelection(voice_Langue);
  Numvoice = configFile->ReadLong(wxT("NUMVOICE"),0);
  configFile->SetPath("/"+Profil+"/");
  configFile->SetPath("COMPILATION_CHOICES/");
  configFile->Read(wxT("PCB"),&PCB);
  configFile->Read(wxT("LCD"),&LCD);
  configFile->Read(wxT("LCDROT180"),&LCDROT180);
  configFile->Read(wxT("VOICE"),&VOICE);
  configFile->Read(wxT("EXT"),&EXT);
  configFile->Read(wxT("AUDIO"),&AUDIO);
  configFile->Read(wxT("HELI"),&HELI);
  configFile->Read(wxT("TTS"),&TTS);
  configFile->Read(wxT("TRANSLATIONS"),&TRANSLATIONS);
  configFile->Read(wxT("NAVIGATION"),&NAVIGATION);
  configFile->Read(wxT("HAPTIC"),&HAPTIC);
  configFile->Read(wxT("PPM_UNIT"),&PPM_UNIT);
  configFile->Read(wxT("GPS"),&GPS);
  configFile->Read(wxT("VARIO"),&VARIO);
  configFile->Read(wxT("PPM"),&PPM);
  configFile->Read(wxT("PCM_PROTOCOL"),&PCM_PROTOCOL);
  configFile->Read(wxT("SERIAL_PROTOCOL"),&SERIAL_PROTOCOL);
  configFile->Read(wxT("CC2500"),&CC2500);
  configFile->Read(wxT("CYRF6936"),&CYRF6936);
  configFile->Read(wxT("NRF24l01"),&NRF24l01);
  configFile->Read(wxT("A7105"),&A7105);
  configFile->Read(wxT("CC2500PAG"),&CC2500PAG);
  configFile->Read(wxT("CYRF6936PAG"),&CYRF6936PAG);
  configFile->Read(wxT("NRF24l01PAG"),&NRF24l01PAG);
  configFile->Read(wxT("A7105PAG"),&A7105PAG);
  configFile->Read(wxT("SD_CARD"),&SD_CARD);
  configFile->Read(wxT("TEMPLATES"),&TEMPLATES);
  configFile->Read(wxT("THREE_POS"),&THREE_POS);
  configFile->Read(wxT("SPLASH"),&SPLASH);
  configFile->Read(wxT("UNITS"),&UNITS);
  configFile->Read(wxT("DEFAULT_MODE"),&DEFAULT_MODE);
  configFile->Read(wxT("FONT"),&FONT);
  configFile->Read(wxT("BOLD"),&BOLD);
  configFile->Read(wxT("BATTGRAPH"),&BATTGRAPH);
  configFile->Read(wxT("EEPROM_PROGRESS_BAR"),&EEPROM_PROGRESS_BAR);
  configFile->Read(wxT("FAI"),&FAI);
  configFile->Read(wxT("AUTOSWITCH"),&AUTOSWITCH);
  configFile->Read(wxT("AUTOSOURCE"),&AUTOSOURCE);
  configFile->Read(wxT("DBLKEYS"),&DBLKEYS);
  configFile->Read(wxT("PPM_CENTER_ADJUSTABLE"),&PPM_CENTER_ADJUSTABLE);
  configFile->Read(wxT("PPM_LIMITS_SYMETRICAL"),&PPM_LIMITS_SYMETRICAL);
  configFile->Read(wxT("FLIGHT_MODES"),&FLIGHT_MODES);
  configFile->Read(wxT("CURVES"),&CURVES);
  configFile->Read(wxT("GVARS"),&GVARS);
  configFile->Read(wxT("CORRECT_NEGATIVE_VALUES"),&CORRECT_NEGATIVE_VALUES);
  configFile->Read(wxT("ARITHMETIC_OVERFLOW_CHECK"),&ARITHMETIC_OVERFLOW_CHECK);
  configFile->Read(wxT("ACCURAT_THROTTLE_STATS"),&ACCURAT_THROTTLE_STATS);
  configFile->Read(wxT("PWM_BACKLIGHT"),&PWM_BACKLIGHT);
  configFile->Read(wxT("OVERRIDE_CHANNEL_FUNCTION"),&OVERRIDE_CHANNEL_FUNCTION);
  configFile->Read(wxT("WS_HOW_HIGH"),&WS_HOW_HIGH);
  configFile->Read(wxT("TOGGLETRIM"),&TOGGLETRIM);
  configFile->Read(wxT("NOANDSECONDE"),&NOANDSECONDE);
  configFile->Read(wxT("SHUTDOWN_CONFIRMATION"),&SHUTDOWN_CONFIRMATION);
  configFile->Read(wxT("FRAM"),&FRAM);
  configFile->Read(wxT("BLUETOOTH"),&BLUETOOTH);
  configFile->Read(wxT("XMODEM"),&XMODEM);
  configFile->Read(wxT("SCC"),&SCC);
  configFile->Read(wxT("PERSONAMES"),&PERSONAMES);
  configFile->Read(wxT("DBLGAZSTICK"),&DBLGAZSTICK);
  configFile->Read(wxT("OTHERCOMPOPTIONS"),&OTHERCOMPOPTIONS);
  configFile->Read(wxT("INV_STICK_RH"),&INV_STICK_RH);
  configFile->Read(wxT("INV_STICK_LV"),&INV_STICK_LV);
  configFile->Read(wxT("INV_STICK_RV"),&INV_STICK_RV);
  configFile->Read(wxT("INV_STICK_LH"),&INV_STICK_LH);
  configFile->Read(wxT("switch1"),&switch1);
  configFile->Read(wxT("switch2"),&switch2);
  configFile->Read(wxT("switch3"),&switch3);
  configFile->Read(wxT("switch4"),&switch4);
  configFile->Read(wxT("switch5"),&switch5);
  configFile->Read(wxT("switch6"),&switch6);
  configFile->Read(wxT("switchID0"),&switchID0);
  configFile->Read(wxT("switchID1"),&switchID1);
  configFile->Read(wxT("switchID2"),&switchID2);
  configFile->Read(wxT("encoderA"),&encoderA);
  configFile->Read(wxT("encoderB"),&encoderB);
  configFile->Read(wxT("encoderNo"),&encoderNo);
  configFile->Read(wxT("switchXD0"),&switchXD0);
  configFile->Read(wxT("switchXD1"),&switchXD1);
  configFile->Read(wxT("switchXD2"),&switchXD2);
  configFile->Read(wxT("stickRud"),&stickRud);
  configFile->Read(wxT("stickEle"),&stickEle);
  configFile->Read(wxT("stickThr"),&stickThr);
  configFile->Read(wxT("stickAil"),&stickAil);

  //XANY
  configFile->Read(wxT("NUMXANY"),&NUMXANY);

  // [SPLASH]
  configFile->SetPath("/"+Profil+"/");
  configFile->SetPath("SPLASH/");
  configFile->Read(wxT("SPLASH_DEFINED"),&personalSplash);
  if (personalSplash) {
    for (int i=0; (i < (SPLASHLENGHT)); ++i) {
      wxString lineName;
      lineName.Printf("DATA_N%u",i);
      long num;
      num = configFile->ReadLong(lineName,0);
      LbmSplash[i] = (num & 0xFF);
    }
  } else {
    RestoreDefaultSplash();
  }
}

void OpenAVRc_DesktopFrame::RestoreDefaultSplash()
{
  memcpy(LbmSplash,LbmSplashOriginal,SPLASHLENGHT);
}

extern void OpenAVRc_DesktopFrame::SaveConfig()
{
  if (Profil == "")
    return;
  configFile->SetPath("/");
  configFile->Write(wxT("Latestprofil"),Profil);
  //PROFILS
  configFile->SetPath("/PROFILS/");
  configFile->Write(Profil,"");

  configFile->SetPath("/"+Profil+"/");
  // [COMM]
  configFile->SetPath("COMM/");
  configFile->Write(wxT("Programmer"),dude_programmer);
  configFile->Write(wxT("Port"),dude_port);
  configFile->Write(wxT("Type"),dude_type);
  configFile->Write(wxT("avrdudepath"),avrdudepath);

  // [TTS]
  configFile->SetPath("/"+Profil+"/");
  configFile->SetPath("TTS/");

  configFile->Write(wxT("LANGTTS"),voice_Langue);
  configFile->Write(wxT("NUMVOICE"),Numvoice);

  // [COMPILATION_CHOICES]
  configFile->SetPath("/"+Profil+"/");
  configFile->SetPath("COMPILATION_CHOICES/");
  configFile->Write(wxT("PCB"),PCB);
  configFile->Write(wxT("LCD"),LCD);
  configFile->Write(wxT("LCDROT180"),LCDROT180);
  configFile->Write(wxT("VOICE"),VOICE);
  configFile->Write(wxT("EXT"),EXT);
  configFile->Write(wxT("AUDIO"),AUDIO);
  configFile->Write(wxT("HELI"),HELI);
  configFile->Write(wxT("TTS"),TTS);
  configFile->Write(wxT("TRANSLATIONS"),TRANSLATIONS);
  configFile->Write(wxT("NAVIGATION"),NAVIGATION);
  configFile->Write(wxT("HAPTIC"),HAPTIC);
  configFile->Write(wxT("PPM_UNIT"),PPM_UNIT);
  configFile->Write(wxT("GPS"),GPS);
  configFile->Write(wxT("VARIO"),VARIO);
  configFile->Write(wxT("PPM"),PPM);
  configFile->Write(wxT("PCM_PROTOCOL"),PCM_PROTOCOL);
  configFile->Write(wxT("SERIAL_PROTOCOL"),SERIAL_PROTOCOL);
  configFile->Write(wxT("CC2500"),CC2500);
  configFile->Write(wxT("CYRF6936"),CYRF6936);
  configFile->Write(wxT("NRF24l01"),NRF24l01);
  configFile->Write(wxT("A7105"),A7105);
  configFile->Write(wxT("CC2500PAG"),CC2500PAG);
  configFile->Write(wxT("CYRF6936PAG"),CYRF6936PAG);
  configFile->Write(wxT("NRF24l01PAG"),NRF24l01PAG);
  configFile->Write(wxT("A7105PAG"),A7105PAG);
  configFile->Write(wxT("SD_CARD"),SD_CARD);
  configFile->Write(wxT("TEMPLATES"),TEMPLATES);
  configFile->Write(wxT("THREE_POS"),THREE_POS);
  configFile->Write(wxT("SPLASH"),SPLASH);
  configFile->Write(wxT("UNITS"),UNITS);
  configFile->Write(wxT("DEFAULT_MODE"),DEFAULT_MODE);
  configFile->Write(wxT("FONT"),FONT);
  configFile->Write(wxT("BOLD"),BOLD);
  configFile->Write(wxT("BATTGRAPH"),BATTGRAPH);
  configFile->Write(wxT("EEPROM_PROGRESS_BAR"),EEPROM_PROGRESS_BAR);
  configFile->Write(wxT("FAI"),FAI);
  configFile->Write(wxT("AUTOSWITCH"),AUTOSWITCH);
  configFile->Write(wxT("AUTOSOURCE"),AUTOSOURCE);
  configFile->Write(wxT("DBLKEYS"),DBLKEYS);
  configFile->Write(wxT("PPM_CENTER_ADJUSTABLE"),PPM_CENTER_ADJUSTABLE);
  configFile->Write(wxT("PPM_LIMITS_SYMETRICAL"),PPM_LIMITS_SYMETRICAL);
  configFile->Write(wxT("FLIGHT_MODES"),FLIGHT_MODES);
  configFile->Write(wxT("CURVES"),CURVES);
  configFile->Write(wxT("GVARS"),GVARS);
  configFile->Write(wxT("CORRECT_NEGATIVE_VALUES"),CORRECT_NEGATIVE_VALUES);
  configFile->Write(wxT("ARITHMETIC_OVERFLOW_CHECK"),ARITHMETIC_OVERFLOW_CHECK);
  configFile->Write(wxT("ACCURAT_THROTTLE_STATS"),ACCURAT_THROTTLE_STATS);
  configFile->Write(wxT("PWM_BACKLIGHT"),PWM_BACKLIGHT);
  configFile->Write(wxT("OVERRIDE_CHANNEL_FUNCTION"),OVERRIDE_CHANNEL_FUNCTION);
  configFile->Write(wxT("WS_HOW_HIGH"),WS_HOW_HIGH);
  configFile->Write(wxT("TOGGLETRIM"),TOGGLETRIM);
  configFile->Write(wxT("NOANDSECONDE"),NOANDSECONDE);
  configFile->Write(wxT("SHUTDOWN_CONFIRMATION"),SHUTDOWN_CONFIRMATION);
  configFile->Write(wxT("FRAM"),FRAM);
  configFile->Write(wxT("BLUETOOTH"),BLUETOOTH);
  configFile->Write(wxT("XMODEM"),XMODEM);
  configFile->Write(wxT("SCC"),SCC);
  configFile->Write(wxT("PERSONAMES"),PERSONAMES);
  configFile->Write(wxT("DBLGAZSTICK"),DBLGAZSTICK);
  configFile->Write(wxT("OTHERCOMPOPTIONS"),OTHERCOMPOPTIONS);
  configFile->Write(wxT("INV_STICK_RH"),INV_STICK_RH);
  configFile->Write(wxT("INV_STICK_LV"),INV_STICK_LV);
  configFile->Write(wxT("INV_STICK_RV"),INV_STICK_RV);
  configFile->Write(wxT("INV_STICK_LH"),INV_STICK_LH);
  configFile->Write(wxT("switch1"),switch1);
  configFile->Write(wxT("switch2"),switch2);
  configFile->Write(wxT("switch3"),switch3);
  configFile->Write(wxT("switch4"),switch4);
  configFile->Write(wxT("switch5"),switch5);
  configFile->Write(wxT("switch6"),switch6);
  configFile->Write(wxT("switchID0"),switchID0);
  configFile->Write(wxT("switchID1"),switchID1);
  configFile->Write(wxT("switchID2"),switchID2);
  configFile->Write(wxT("encoderA"),encoderA);
  configFile->Write(wxT("encoderB"),encoderB);
  configFile->Write(wxT("encoderNo"),encoderNo);
  configFile->Write(wxT("switchXD0"),switchXD0);
  configFile->Write(wxT("switchXD1"),switchXD1);
  configFile->Write(wxT("switchXD2"),switchXD2);
  configFile->Write(wxT("stickRud"),stickRud);
  configFile->Write(wxT("stickEle"),stickEle);
  configFile->Write(wxT("stickThr"),stickThr);
  configFile->Write(wxT("stickAil"),stickAil);

  //XANY
  configFile->Write(wxT("NUMXANY"),NUMXANY);

  // [SPLASH]
  configFile->SetPath("/"+Profil+"/");
  configFile->SetPath("SPLASH/");
  configFile->Write(wxT("SPLASH_DEFINED"),personalSplash);
  if (personalSplash) {
    for (int i=0; (i < (SPLASHLENGHT)); ++i) {
      wxString lineName;
      lineName.Printf("DATA_N%u",i);
      configFile->Write(lineName,LbmSplash[i]);
    }
  } else {
    for (int i=0; (i < (SPLASHLENGHT)); ++i) {
      wxString lineName;
      lineName.Printf("DATA_N%u",i);
      configFile->DeleteEntry(lineName,1);
    }
  }

  configFile->Flush();
  configFile->SetPath("/");
}

void OpenAVRc_DesktopFrame::OnClose(wxCloseEvent& event)
{
  if (Ini_Changed)
    SaveConfig();
  Destroy();
}

void OpenAVRc_DesktopFrame::OnCompilOMaticSelected(wxCommandEvent& event)
{
  CompilerOptionsFrame* CompilOMatic = new CompilerOptionsFrame(this);
  CompilOMatic->Show(TRUE);//opens CommunicationsFrame
  MenuBar_main->Enable(ID_MENUCOMPILOMATIC, false);
}

void OpenAVRc_DesktopFrame::EnableCompilOMaticSelectedMenu()
{
  MenuBar_main->Enable(ID_MENUCOMPILOMATIC, true);
}

void OpenAVRc_DesktopFrame::OnMenuItem9Selected(wxCommandEvent& event)
{
  wxLaunchDefaultBrowser(wxT("https://raw.githubusercontent.com/Ingwie/OpenAVRc_Dev/V3/documentation/Compilez%20votre%20FW%20OpenAVRc_V3.pdf"), 0);
}

void OpenAVRc_DesktopFrame::OnMenuJQ6500_PCBSelected(wxCommandEvent& event)
{
  wxLaunchDefaultBrowser(wxT("https://github.com/Ingwie/OpenAVRc_Dev/tree/master/documentation/Beta%20tester%20files/JQ6500%20PCB%20by%20Pyrall"), 0);
}

void OpenAVRc_DesktopFrame::OnMenuVOICE_AUDIO_PCBSelected(wxCommandEvent& event)
{
  wxLaunchDefaultBrowser(wxT("https://github.com/Ingwie/OpenAVRc_Dev/tree/master/documentation/Beta%20tester%20files/Voice-audio%20pcb%20and%20rtc%20by%20Pyrall"), 0);
}

void OpenAVRc_DesktopFrame::OnMenuItem16Selected(wxCommandEvent& event)
{
  wxLaunchDefaultBrowser(wxT("https://raw.githubusercontent.com/Ingwie/OpenAVRc_Dev/V3/documentation/Schema_MegaMini.pdf"), 0);
}

void OpenAVRc_DesktopFrame::OnMenuItem14Selected(wxCommandEvent& event)
{
  wxLaunchDefaultBrowser(wxT("https://raw.githubusercontent.com/Ingwie/OpenAVRc_Dev/V3/documentation/Doc_Shield.pdf"), 0);
}

void OpenAVRc_DesktopFrame::OnListBoxConfigDClick(wxCommandEvent& event)
{
  wxString temp = ListBoxConfig->GetString(ListBoxConfig->GetSelection());
  wxMessageDialog *Select_config = new wxMessageDialog(NULL,_("Charger la configuration ") + temp,wxT("Configuration"), wxICON_QUESTION | wxCENTRE | wxCANCEL | wxOK );
  if (Select_config->ShowModal() == wxID_OK) {
    SaveConfig();
    Profil = temp;
    LoadConfig(Profil);
    Ini_Changed = 1;
  }
  ListBoxConfig->SetStringSelection(Profil);
  DrawLbmSplash();
}

void OpenAVRc_DesktopFrame::OnMenuNewconfigSelected(wxCommandEvent& event)
{
  wxString temp = wxGetTextFromUser(wxT(""), _("Appelation de la nouvelle configuration"), wxT(""));
  if (temp != "") {
    SaveConfig();
    SavedConfig.Add(temp,1);
    ListBoxConfig->InsertItems(1,&temp,ListBoxConfig->GetCount());
    ListBoxConfig->SetStringSelection(temp);
    Profil = temp;
    SaveConfig();
    Ini_Changed = 1;
  }
}

void OpenAVRc_DesktopFrame::OnMenuDeleteActiveConfigSelected(wxCommandEvent& event)
{
  if (Profil == defaut) {
    wxMessageBox(_("Defaut ne peut être effacé"), "",wxICON_EXCLAMATION | wxCENTRE, this);
    return;
  } else {
    int answer = wxMessageBox((_("Supprimer ")) + Profil + " ?", "",wxOK | wxCANCEL | wxICON_EXCLAMATION | wxCENTRE | wxCANCEL_DEFAULT, this);
    if (answer == wxOK) {
      SavedConfig.Remove(Profil);
      ListBoxConfig->Delete(ListBoxConfig->FindString(Profil));
      configFile->DeleteEntry(Profil,true);
      configFile->SetPath("/PROFILS/");
      configFile->DeleteEntry(Profil,true); //X2 if it is the latest
      configFile->SetPath("/");
      configFile->DeleteGroup(Profil);
      configFile->Flush();
      Profil = defaut;
      ListBoxConfig->SetStringSelection(Profil);
      SaveConfig();
    }
  }
}

void OpenAVRc_DesktopFrame::DrawLbmSplash()
{
  int p = 2;

  wxClientDC dc(PanelSplash);
  wxBrush brush_back(*wxWHITE, wxBRUSHSTYLE_SOLID  );
  dc.SetBrush(brush_back);
  dc.DrawRectangle(0,0,LCD_W*LcdScale,LCD_H*LcdScale);
  wxBrush brush_top(*wxBLACK, wxBRUSHSTYLE_SOLID  );
  dc.SetBrush(brush_top);
  for (uint8_t y=0; y < (LCD_H / 8); y++) {
    for (uint8_t x=0; x < LCD_W; x++) {
      uint8_t bit = LbmSplash[p];
      ++p;
      for (uint8_t i=0; i < 8; i++) {
        if (bit & 0x01)
          dc.DrawRectangle(x*LcdScale,(y*8*LcdScale) +(i*LcdScale),LcdScale,LcdScale);
        bit >>=1;
      }
    }
  }
}

void OpenAVRc_DesktopFrame::OnPanelSplashPaint(wxPaintEvent& event)
{
  DrawLbmSplash();
}

void OpenAVRc_DesktopFrame::OnButtonSplashDefaultClick(wxCommandEvent& event)
{
  personalSplash = false;
  RestoreDefaultSplash();
  DrawLbmSplash();
  SaveConfig();
}

void OpenAVRc_DesktopFrame::OnButtonPersoClick(wxCommandEvent& event)
{
  wxFileDialog openFileDialog(this, _("Fichier image"), "", "", _("Fichier image (*.*)|*.*"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() == wxID_CANCEL)
    return;

  wxInitAllImageHandlers();
  wxImage ImageSplash;
  //ImageSplash.
  ImageSplash.LoadFile(openFileDialog.GetPath(), wxBITMAP_TYPE_ANY );
  ImageSplash.Rescale(LCD_W,LCD_H,wxIMAGE_QUALITY_NORMAL);
  ImageSplash = ImageSplash.ConvertToMono(255,255,255);
  uint8_t* imgraw = (uint8_t*) malloc(LCD_W*LCD_H*3);
  imgraw = ImageSplash.GetData();
  uint8_t imgpix[SPLASHLENGHT] = {0};
  imgpix[0] = LCD_W;
  imgpix[1] = LCD_H;

  uint16_t j = 0;
  uint16_t line = 0;

  for (uint32_t i=0; (i<(LCD_W*LCD_H*3)); i+=3) {
    imgpix[j+2] >>= 1;
    if (imgraw[i] == 0)
      imgpix[j+2] |= 0x80;
    ++j;
    if ((j%LCD_W) == 0) {
      ++line;
      if (line != LCD_H/8) {
        j-=LCD_W;
      } else {
        line = 0;
      }
    }
  }
  imgpix[j+2] >>= 1;
  memcpy(LbmSplash,imgpix,SPLASHLENGHT);
  DrawLbmSplash();
  personalSplash = true;
  SaveConfig();
}

void OpenAVRc_DesktopFrame::OnEDITEURClick(wxCommandEvent& event)
{
  VoiceEditFrame* voiceFrame = new VoiceEditFrame(this);
  voiceFrame->Show(TRUE);//opens voice edit
}


void OpenAVRc_DesktopFrame::OnButtonGenerateurClick(wxCommandEvent& event)
{
}

void OpenAVRc_DesktopFrame::OnButtonCarteSDClick(wxCommandEvent& event)
{
  wxArrayString DriveList;

  wxFSVolume DriveSearch;

// Copy Voice files

  int answer = wxMessageBox(_("Voulez vous copier les fichiers sur la carte du JQ ? \n Insérez la avant de cliquer sur ""OUI"""), _("SD ""VOICE"""), wxYES | wxNO | wxCENTRE, this);
  if (answer == wxYES)
    {

      DriveList = DriveSearch.GetVolumes( wxFS_VOL_MOUNTED | wxFS_VOL_REMOVABLE );

      wxSingleChoiceDialog DriveName(this,
                                     _("La carte SD doit avoir été formatée pour\n un bon fonctionnement sur la radio"),
                                     _("Selectionnez le lecteur"),
                                     DriveList
                                    );

      if (DriveName.ShowModal() == wxID_OK)
        {
          wxString Drive = DriveName.GetStringSelection();
          wxString SourceDirectory;
          wxString DestinationDirectory;
          wxString FileName;
          wxString Ext;

          SourceDirectory = "\\VOICEMP3\\";
          DestinationDirectory = "\\VOICEMP3\\";
          Ext = "mp3";
          wxMkdir(Drive + DestinationDirectory);

          wxBusyInfo wait(_("Copie en cours, attendez SVP......"));

          for (int i = 0; i < 512 ; i++)
            {
              FileName.Printf("%04d.",i);
              wxCopyFile(AppPath + SourceDirectory + FileName + Ext, Drive + DestinationDirectory + FileName + Ext, false);
            }
        }
    }

// Copy text prompt

  answer = wxMessageBox(_("Avez vous une carte SD de sauvegarde ? \n Insérez la avant de cliquer sur ""OUI"""), _("SD ""DATA"""), wxYES | wxNO | wxCENTRE, this);
  if (answer == wxYES) {
    DriveList = DriveSearch.GetVolumes( wxFS_VOL_MOUNTED | wxFS_VOL_REMOVABLE );

    wxSingleChoiceDialog DataDriveName(this,
                                       _("Choisissez la carte SD de sauvegarde de la radio"),
                                       _("Selectionnez le lecteur"),
                                       DriveList
                                      );

    if (DataDriveName.ShowModal() == wxID_OK) {
      // Now copy prompt
      wxString DataDrive = DataDriveName.GetStringSelection();
      wxString DestinationDirectory = DataDrive + "\\VOICE\\";
      if (!wxDirExists(DestinationDirectory))
        wxMkdir(DestinationDirectory);
      wxCopyFile(AppPath + "\\" + VOICETXT_FILE, DestinationDirectory + VOICETXT_FILE, true); // overwrite on
      wxBusyInfo wait(_("Copie en cours, attendez SVP......"));
    }
  }
  Sleep(500);
  DrawLbmSplash();
}

void OpenAVRc_DesktopFrame::OnChoiceLangueSelect(wxCommandEvent& event)
{
  voice_Langue = ChoiceLangue->GetString(ChoiceLangue->GetSelection());
  Ini_Changed = true;
}

void OpenAVRc_DesktopFrame::OnMenuChoiceVoiceSelected(wxCommandEvent& event)
{
  if (wxSetWorkingDirectory(AppPath))
    wxExecute("CMD /c" + AppPath + "\\tools\\tts.exe -V ->voices.txt", wxEXEC_HIDE_CONSOLE | wxEXEC_SYNC);
  Ini_Changed = true;
  Sleep(500);
  Voice_choice* voiceChoiceFrame = new Voice_choice(this);
  voiceChoiceFrame->Show(TRUE);
  MenuBar_main->Enable(ID_MENUVOICECHOICE, false);
}

void OpenAVRc_DesktopFrame::EnableChoiceVoiceMenu()
{
  MenuBar_main->Enable(ID_MENUVOICECHOICE, true);
}

void OpenAVRc_DesktopFrame::OnButtonSimuFramClick(wxCommandEvent& event)
{
  wxString simu("OpenAVRc_Simulator_FRAM.exe");
  wxExecute(simu);
}

void OpenAVRc_DesktopFrame::OnButtonSimuSTDClick(wxCommandEvent& event)
{
  wxString simu("OpenAVRc_Simulator.exe");
  wxExecute(simu);
}

void OpenAVRc_DesktopFrame::OnMenuBluetoothSelected(wxCommandEvent& event)
{
  BluetoothFrame* BTFrame = new BluetoothFrame(this);
  BTFrame->Show(TRUE);
  MenuBar_main->Enable(ID_MENUBLUETOOTH, false);
}

void OpenAVRc_DesktopFrame::EnableBluetoothSelectedMenu()
{
  MenuBar_main->Enable(ID_MENUBLUETOOTH, true);
}
