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


#include "OpenAVRc_DesktopMain.h"
#include "CommunicationsFrame.h"
#include "CompilerOptionsFrame.h"
#include "Voice_choice.h"
#include "VoiceEditFrame.h"
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/textdlg.h>
#include <wx/aboutdlg.h>
#include <wx/volume.h>
#include <wx/choicdlg.h>
#include <wx/busyinfo.h>



//(*InternalHeaders(OpenAVRc_DesktopFrame)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)


//helper functions
enum wxbuildinfoformat
{
    short_f, long_f
};

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
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
uint8_t LbmSplashOriginal[] =
{
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
wxString COMM = _("non défini");
wxString voice_Langue = "ES"; //TTS langue
//Const
wxString dude_c = (" -c ");
wxString dude_p = (" -p ");
wxString dude_D = (" -D ");
wxString dude_P = (" -P ");
wxString dude_space = (" ");
wxString dude_U = (" -U ");
wxString dude_eeprom = ("eeprom:");
wxString dude_flash = ("flash:");
wxString dude_raw = (":r");
wxString dude_write = ("w:");
//dude_verify = (" -v ");
wxString dude_read = ("r:");
wxString dude_intel = (":i");
wxString keepopen = ("cmd /k ");


//Define defaults
wxString PCB = _("non défini");
wxString LCD = _("non défini");
wxString VOICE = ("NO");
wxString EXT = ("STD");
bool AUDIO = 0;
bool HELI = 0;
wxString TTS = _("non défini");
wxString TRANSLATIONS = _("non défini");
wxString NAVIGATION = ("NO");
bool FRSKY_HUB = 0;
bool HAPTIC = 0;
wxString PPM_UNIT = ("PERCENT_PREC1");
bool GAUGES = 0;
bool GPS = 0;
bool VARIO = 0;
bool RTCLOCK = 0;
bool SPORT_FILE_LOG = 0;
bool PPM = 1;
bool PXX = 0;
bool DSM2SERIAL = 0;
bool DSM2PPM = 0;
bool SD_CARD = 0;
bool FAS_OFFSET = 0;
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
bool PPM_LIMITS_SYMETRICAL = 1;
bool FLIGHT_MODES = 0;
bool CURVES = 0;
bool GVARS = 0;
bool PCBREV = 0;
bool TURNIGY_TRANSMITTER_FIX = 0;
bool FRSKY_STICKS = 0;
bool CORRECT_NEGATIVE_VALUES = 0;
bool ARITHMETIC_OVERFLOW_CHECK = 0;
bool ACCURAT_THROTTLE_STATS = 0;
bool SP22 = 0;
bool PWM_BACKLIGHT = 0;
bool OVERRIDE_CHANNEL_FUNCTION = 0;
bool WS_HOW_HIGH = 0;
bool TX_CADDY = 0;
bool TOGGLETRIM = 0;
bool NOANDSECONDE = 1;
bool SHUTDOWN_CONFIRMATION = 0;


//(*IdInit(OpenAVRc_DesktopFrame)
const long OpenAVRc_DesktopFrame::ID_STATICBOXSPLASH = wxNewId();
const long OpenAVRc_DesktopFrame::ID_STATICBOXCONFIG = wxNewId();
const long OpenAVRc_DesktopFrame::ID_BUTTON1 = wxNewId();
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
const long OpenAVRc_DesktopFrame::ID_MENUITEM1 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUVOICECHOICE = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM3 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM5 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM4 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM6 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM9 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM10 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM7 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUCOMPILOMATIC = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM14 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM8 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM11 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM12 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM13 = wxNewId();
const long OpenAVRc_DesktopFrame::ID_MENUITEM16 = wxNewId();
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
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxMenu* Menu2;

    Create(parent, wxID_ANY, _("OpenAVRc Desktop"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(592,282));
    Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxSize(848,280), 0, _T("ID_PANEL1"));
    Panel1->SetFocus();
    StaticBoxSplash = new wxStaticBox(Panel1, ID_STATICBOXSPLASH, _("Ecran d\'accueil"), wxPoint(160,8), wxSize(272,192), 0, _T("ID_STATICBOXSPLASH"));
    StaticBoxConfig = new wxStaticBox(Panel1, ID_STATICBOXCONFIG, _("Configuration"), wxPoint(16,8), wxSize(136,192), 0, _T("ID_STATICBOXCONFIG"));
    Button1 = new wxButton(Panel1, ID_BUTTON1, _("SIMULATEUR"), wxPoint(8,208), wxSize(568,31), 0, wxDefaultValidator, _T("ID_BUTTON1"));
    ListBoxConfig = new wxListBox(Panel1, ID_LISTBOXCONFIG, wxPoint(24,32), wxSize(120,160), 0, 0, wxLB_SINGLE|wxDOUBLE_BORDER|wxVSCROLL, wxDefaultValidator, _T("ID_LISTBOXCONFIG"));
    PanelSplash = new wxPanel(Panel1, ID_SPLASH, wxPoint(169,64), wxSize(256,128), wxNO_BORDER, _T("ID_SPLASH"));
    PanelSplash->SetBackgroundColour(wxColour(255,255,255));
    ButtonPerso = new wxButton(Panel1, ID_BUTTONPERSO, _("Personnaliser"), wxPoint(192,30), wxSize(75,24), 0, wxDefaultValidator, _T("ID_BUTTONPERSO"));
    ButtonPerso->SetToolTip(_("Attention : Tous ce qui n\'est pas blanc sera converti en noir"));
    ButtonSplashDefault = new wxButton(Panel1, ID_BUTTONSPLASHDEFAULT, _("Par défaut"), wxPoint(312,30), wxSize(75,24), 0, wxDefaultValidator, _T("ID_BUTTONSPLASHDEFAULT"));
    ButtonSplashDefault->SetToolTip(_("Restaurer l\'écran par défaut"));
    StaticBox1 = new wxStaticBox(Panel1, ID_STATICBOX1, _("Fichiers Voix"), wxPoint(440,8), wxSize(136,128), 0, _T("ID_STATICBOX1"));
    ChoiceLangue = new wxChoice(Panel1, ID_CHOICE1, wxPoint(504,32), wxSize(48,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
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
    StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Langue"), wxPoint(456,40), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    StaticText1->SetToolTip(_("Choisissez la langue des mensages vocales"));
    ButtonEditeur = new wxButton(Panel1, ID_BUTTON2, _("Éditeur"), wxPoint(472,72), wxSize(83,23), 0, wxDefaultValidator, _T("ID_BUTTON2"));
    ButtonEditeur->SetToolTip(_("Pour changer les messages"));
    ButtonCarteSD = new wxButton(Panel1, ID_BUTTON4, _("Carte SD"), wxPoint(472,104), wxSize(83,23), 0, wxDefaultValidator, _T("ID_BUTTON4"));
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
    MenuItem3 = new wxMenuItem(Menu3, ID_MENUITEM1, _("Programmateur"), wxEmptyString, wxITEM_NORMAL);
    MenuItem3->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_EXECUTABLE_FILE")),wxART_OTHER));
    Menu3->Append(MenuItem3);
    MenuChoiceVoice = new wxMenuItem(Menu3, ID_MENUVOICECHOICE, _("Synthèse vocale"), wxEmptyString, wxITEM_NORMAL);
    MenuChoiceVoice->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_HELP")),wxART_OTHER));
    Menu3->Append(MenuChoiceVoice);
    MenuBar_main->Append(Menu3, _("Paramètres"));
    Menu4 = new wxMenu();
    Menu4->AppendSeparator();
    Menu5 = new wxMenuItem(Menu4, ID_MENUITEM3, _("Lire les réglages et le modèles de la radio (eeprom)"), wxEmptyString, wxITEM_NORMAL);
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
    Menu4->Append(ID_MENUITEM7, _("Fusibles et Bootloader"), MenuItem8, wxEmptyString);
    MenuBar_main->Append(Menu4, _("Lire/Écrire"));
    Menu7 = new wxMenu();
    ATMEGA2560Compiler = new wxMenuItem(Menu7, ID_MENUCOMPILOMATIC, _("Compil-O-matic"), wxEmptyString, wxITEM_NORMAL);
    ATMEGA2560Compiler->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_EXECUTABLE_FILE")),wxART_MENU));
    Menu7->Append(ATMEGA2560Compiler);
    MenuBar_main->Append(Menu7, _("Compilateur"));
    Menu2 = new wxMenu();
    MenuHtmlDoc = new wxMenu();
    MenuItem14 = new wxMenuItem(MenuHtmlDoc, ID_MENUITEM14, _("OpenAVRc M-2560 Docs"), wxEmptyString, wxITEM_NORMAL);
    MenuItem14->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_MENU));
    MenuHtmlDoc->Append(MenuItem14);
    MenuItem4 = new wxMenuItem(MenuHtmlDoc, ID_MENUITEM8, _("JQ6500 PCB (by Pyrall)"), wxEmptyString, wxITEM_NORMAL);
    MenuItem4->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_MENU));
    MenuHtmlDoc->Append(MenuItem4);
    MenuItem9 = new wxMenuItem(MenuHtmlDoc, ID_MENUITEM11, _("VOICE tuto (by JPZ)"), wxEmptyString, wxITEM_NORMAL);
    MenuItem9->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_MENU));
    MenuHtmlDoc->Append(MenuItem9);
    MenuItem12 = new wxMenuItem(MenuHtmlDoc, ID_MENUITEM12, _("VOICE/AUDIO PCB et RTC (by Pyrall)"), wxEmptyString, wxITEM_NORMAL);
    MenuItem12->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_MENU));
    MenuHtmlDoc->Append(MenuItem12);
    MenuItem13 = new wxMenuItem(MenuHtmlDoc, ID_MENUITEM13, _("WIN FIRMWARE Compilation (by JPZ)"), wxEmptyString, wxITEM_NORMAL);
    MenuItem13->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_MENU));
    MenuHtmlDoc->Append(MenuItem13);
    MenuItem15 = new wxMenu();
    MenuItem16 = new wxMenuItem(MenuItem15, ID_MENUITEM16, _("Mousse NSRc Naval"), wxEmptyString, wxITEM_NORMAL);
    MenuItem16->SetBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_MENU));
    MenuItem15->Append(MenuItem16);
    MenuHtmlDoc->Append(ID_MENUITEM15, _("Autres links"), MenuItem15, wxEmptyString);
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

    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnSimulateurClick2);
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
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnProgrammerSelected);
    Connect(ID_MENUVOICECHOICE,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnMenuChoiceVoiceSelected);
    Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnreadmodelsSelected);
    Connect(ID_MENUITEM5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnreadfirmwareSelected);
    Connect(ID_MENUITEM4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnWriteModelToRadioSelected);
    Connect(ID_MENUITEM6,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnWriteFirmwareToRadioSelected);
    Connect(ID_MENUITEM9,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnEcrirelesFuseesSelected);
    Connect(ID_MENUITEM10,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnEcrirelebootloaderSelected);
    Connect(ID_MENUCOMPILOMATIC,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnATMEGA2560CompilerSelected);
    Connect(ID_MENUITEM14,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnNexStepRc_M2560_docsSelected);
    Connect(ID_MENUITEM8,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnMenuJQ6500_PCBSelected);
    Connect(ID_MENUITEM11,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnMenuVoice_tutoSelected);
    Connect(ID_MENUITEM12,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnMenuVOICE_AUDIO_PCBSelected);
    Connect(ID_MENUITEM13,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnWIN_FIRMWARESelected);
    Connect(ID_MENUITEM16,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnMenuMousseSelected);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnAbout);
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&OpenAVRc_DesktopFrame::OnClose);
    //*)

    {
        wxIcon FrameIcon;
        SetIcon(wxICON(nsrcd_icon));
    }
    //App Path
    wxFileName appPathWithExeName = wxStandardPaths::Get().GetExecutablePath();
    AppPath = (appPathWithExeName.GetPath());


    defaut = wxT("Defaut");
    //Ini File
    Ini_Filename = wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator() + "OpenAVRcDesktop.ini";
    configFile = new wxFileConfig( "", "", Ini_Filename);
    LoadConfig("");
    if (avrdudepath == _("non défini")) wxMessageBox( _("Merci de vérifier les paramètres"), _("Programmeur :"), wxICON_WARNING | wxCENTRE);//Ini File
    if (SavedConfig.GetCount() == 0)
    {
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
    Aboutbox.SetName(_("OpenAVRc Desktop"));
    Aboutbox.SetVersion(_("0.68 Beta"));
    Aboutbox.SetLicence(" GPLv2 . Firmware basé sur NextStepRc 2.18 ");
    Aboutbox.SetDescription(_("Logiciel pour la personnalisation, la compilation, le flashage, la sauvegarde de votre radio OpenAVRc     "));
    Aboutbox.SetCopyright(wxT("(C) 2016 OpenAVRc Team"));
    Aboutbox.SetWebSite(wxT("https://github.com/Ingwie/OpenAVRc_Dev"));

    wxAboutBox(Aboutbox);
}

void OpenAVRc_DesktopFrame::OnProgrammerSelected(wxCommandEvent& event)
{
    CommunicationsFrame * DudeFrame = new CommunicationsFrame(NULL);
    DudeFrame->Show(TRUE);//opens CommunicationsFrame
}

void OpenAVRc_DesktopFrame::OnreadmodelsSelected(wxCommandEvent& event)//READ MODELS FROM RADIO.
{
    wxFileDialog saveDialog(this, _("Choisir le fichier pour importer les modèles des la radio."), AppPath + "\\eeprom\\", "",  "Fichiers BIN (*.bin)|*.bin|Tous (*.*)|*.*", wxFD_SAVE);
    if (saveDialog.ShowModal() == wxID_CANCEL) return;
    wxString dude_tmpfile = (saveDialog.GetPath());
    //wxString dude_send = keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_eeprom+dude_read+dude_tmpfile+dude_raw+dude_verify;
    wxString dude_send = keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_eeprom+dude_read+dude_tmpfile+dude_raw;
    //wxMessageBox(dude_send);
    wxExecute(dude_send);//send command
}

void OpenAVRc_DesktopFrame::OnreadfirmwareSelected(wxCommandEvent& event)//read firmware from radio
{
    wxFileDialog saveDialog(this, _("Choisir le fichier pour importer le Firmware des la radio."), AppPath + "\\firmware\\", "","Fichiers HEX (*.hex)|*.hex", wxFD_SAVE);
    if (saveDialog.ShowModal() == wxID_CANCEL) return;
    wxString dude_tmpfile = (saveDialog.GetPath());
    wxString dude_send = keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_flash+dude_read+dude_tmpfile+dude_intel;
    wxExecute(dude_send);
}

void OpenAVRc_DesktopFrame::OnWriteModelToRadioSelected(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, _("Choisir le fichier (.bin) pour transferer les modêles à la radio."), AppPath + "\\eeprom\\", "","Fichiers BIN (*.bin)|*.bin", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) return;
    wxString dude_tmpfile = (openFileDialog.GetPath());
    //wxString dude_send = keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_eeprom+dude_write+dude_tmpfile+dude_raw+dude_verify;
    wxString dude_send = keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_eeprom+dude_write+dude_tmpfile+dude_raw;
    //wxMessageBox(dude_send);
    wxExecute(dude_send);
}

void OpenAVRc_DesktopFrame::OnWriteFirmwareToRadioSelected(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, _("Choisir le fichier pour transferer le Firmware à la radio."), AppPath + "\\firmware\\", "","Fichiers HEX (*.hex)|*.hex", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) return;
    wxMessageDialog *bkup = new wxMessageDialog(NULL,wxT("Il est recommande de sauvegarder vos modeles avant, voulez vous continuer ?"), wxT("Firmware"),wxOK | wxICON_WARNING | wxCANCEL | wxCANCEL_DEFAULT);
    //bkup->SetEventHandler(bkup);
    if (bkup->ShowModal()!= wxID_OK) return;
    wxString dude_tmpfile = (openFileDialog.GetPath());//write firmware
    //wxString dude_send =keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_flash+dude_write+dude_tmpfile+dude_intel+dude_verify;
    wxString dude_send =keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_flash+dude_write+dude_tmpfile+dude_intel;
    //wxMessageBox(dude_send);

    wxExecute(dude_send);
}


void OpenAVRc_DesktopFrame::OnEcrirelesFuseesSelected(wxCommandEvent& event)// Write fuses
{
    wxMessageDialog *susto = new wxMessageDialog(NULL,
            ("Sûr? Vous voulez continuer?"), wxT("Programmation des fusibles"),
            wxOK | wxICON_WARNING | wxCANCEL | wxCANCEL_DEFAULT);
    susto->SetEventHandler(susto);
    if (susto->ShowModal()!= wxID_OK) return;
    wxString FUSES(" -c usbasp -P usb -F -e -u -Ulfuse:w:0xFF:m -Uhfuse:w:0xD8:m -Uefuse:w:0xFD:m -v");
    wxString dude_send = (keepopen+avrdudepath+dude_p+dude_type+FUSES);
    //wxMessageBox(dude_send);
    wxExecute(dude_send);
}

void OpenAVRc_DesktopFrame::OnEcrirelebootloaderSelected(wxCommandEvent& event) // Write bootloader
{
    wxMessageDialog *susto = new wxMessageDialog(NULL,
            ("Sûr? Vous voulez continuer?"), wxT("Programmation du Bootloader"),wxOK | wxICON_WARNING | wxCANCEL | wxCANCEL_DEFAULT);
    susto->SetEventHandler(susto);
    if (susto->ShowModal()!= wxID_OK) return;
    wxString BOOTLOADER(" -c usbasp -P usb -U lock:w:0x3F:m -V -U flash:w:mega2560_stk500v2boot_opentx.hex -U lock:w:0x0F:m");
    wxString dude_send = (keepopen+avrdudepath+dude_p+dude_type+BOOTLOADER);
    //wxMessageBox(dude_send);
    wxExecute(dude_send);
}

void OpenAVRc_DesktopFrame::LoadConfig(wxString temp)
{

    configFile->SetPath("/");
    if (temp == "") configFile->Read(wxT("Latestprofil"),&Profil);
    else Profil = temp;
    configFile->SetPath("/PROFILS/");
    long dummy;
    bool again = configFile->GetFirstEntry(temp, dummy);
    while (again)
    {
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
    configFile->Read(wxT("VOICE"),&VOICE);
    configFile->Read(wxT("EXT"),&EXT);
    configFile->Read(wxT("AUDIO"),&AUDIO);
    configFile->Read(wxT("HELI"),&HELI);
    configFile->Read(wxT("TTS"),&TTS);
    configFile->Read(wxT("TRANSLATIONS"),&TRANSLATIONS);
    configFile->Read(wxT("NAVIGATION"),&NAVIGATION);
    configFile->Read(wxT("FRSKY_HUB"),&FRSKY_HUB);
    configFile->Read(wxT("HAPTIC"),&HAPTIC);
    configFile->Read(wxT("PPM_UNIT"),&PPM_UNIT);
    configFile->Read(wxT("GAUGES"),&GAUGES);
    configFile->Read(wxT("GPS"),&GPS);
    configFile->Read(wxT("VARIO"),&VARIO);
    configFile->Read(wxT("RTCLOCK"),&RTCLOCK);
    configFile->Read(wxT("SPORT_FILE_LOG"),&SPORT_FILE_LOG);
    configFile->Read(wxT("PPM"),&PPM);
    configFile->Read(wxT("PXX"),&PXX);
    configFile->Read(wxT("DSM2"),&DSM2SERIAL);
    configFile->Read(wxT("DSM2PPM"),&DSM2PPM);
    configFile->Read(wxT("SD_CARD"),&SD_CARD);
    configFile->Read(wxT("FAS_OFFSET"),&FAS_OFFSET);
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
    configFile->Read(wxT("PCBREV"),&PCBREV);
    configFile->Read(wxT("TURNIGY_TRANSMITTER_FIX"),&TURNIGY_TRANSMITTER_FIX);
    configFile->Read(wxT("FRSKY_STICKS"),&FRSKY_STICKS);
    configFile->Read(wxT("CORRECT_NEGATIVE_VALUES"),&CORRECT_NEGATIVE_VALUES);
    configFile->Read(wxT("ARITHMETIC_OVERFLOW_CHECK"),&ARITHMETIC_OVERFLOW_CHECK);
    configFile->Read(wxT("ACCURAT_THROTTLE_STATS"),&ACCURAT_THROTTLE_STATS);
    configFile->Read(wxT("SP22"),&SP22);
    configFile->Read(wxT("PWM_BACKLIGHT"),&PWM_BACKLIGHT);
    configFile->Read(wxT("OVERRIDE_CHANNEL_FUNCTION"),&OVERRIDE_CHANNEL_FUNCTION);
    configFile->Read(wxT("WS_HOW_HIGH"),&WS_HOW_HIGH);
    configFile->Read(wxT("TX_CADDY"),&TX_CADDY);
    configFile->Read(wxT("TOGGLETRIM"),&TOGGLETRIM);
    configFile->Read(wxT("NOANDSECONDE"),&NOANDSECONDE);
    configFile->Read(wxT("SHUTDOWN_CONFIRMATION"),&SHUTDOWN_CONFIRMATION);
    // [SPLASH]
    configFile->SetPath("/"+Profil+"/");
    configFile->SetPath("SPLASH/");
    configFile->Read(wxT("SPLASH_DEFINED"),&personalSplash);
    if (personalSplash)
    {
        for (int i=0; (i < (SPLASHLENGHT)); ++i)
        {
            wxString lineName;
            lineName.Printf("DATA_N%u",i);
            long num;
            num = configFile->ReadLong(lineName,0);
            LbmSplash[i] = (num & 0xFF);
        }
    }
    else
    {
        RestoreDefaultSplash();
    }
}

void OpenAVRc_DesktopFrame::RestoreDefaultSplash()
{
    memcpy(LbmSplash,LbmSplashOriginal,SPLASHLENGHT);
}

extern void OpenAVRc_DesktopFrame::SaveConfig()
{
    //wxMessageBox( Ini_Filename, _("Les paramètres sont sauvé dans :"));
    if (Profil == "") return;
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
    configFile->Write(wxT("VOICE"),VOICE);
    configFile->Write(wxT("EXT"),EXT);
    configFile->Write(wxT("AUDIO"),AUDIO);
    configFile->Write(wxT("HELI"),HELI);
    configFile->Write(wxT("TTS"),TTS);
    configFile->Write(wxT("TRANSLATIONS"),TRANSLATIONS);
    configFile->Write(wxT("NAVIGATION"),NAVIGATION);
    configFile->Write(wxT("FRSKY_HUB"),FRSKY_HUB);
    configFile->Write(wxT("HAPTIC"),HAPTIC);
    configFile->Write(wxT("PPM_UNIT"),PPM_UNIT);
    configFile->Write(wxT("GAUGES"),GAUGES);
    configFile->Write(wxT("GPS"),GPS);
    configFile->Write(wxT("VARIO"),VARIO);
    configFile->Write(wxT("RTCLOCK"),RTCLOCK);
    configFile->Write(wxT("SPORT_FILE_LOG"),SPORT_FILE_LOG);
    configFile->Write(wxT("PPM"),PPM);
    configFile->Write(wxT("PXX"),PXX);
    configFile->Write(wxT("DSM2"),DSM2SERIAL);
    configFile->Write(wxT("DSM2PPM"),DSM2PPM);
    configFile->Write(wxT("SD_CARD"),SD_CARD);
    configFile->Write(wxT("FAS_OFFSET"),FAS_OFFSET);
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
    configFile->Write(wxT("PCBREV"),PCBREV);
    configFile->Write(wxT("TURNIGY_TRANSMITTER_FIX"),TURNIGY_TRANSMITTER_FIX);
    configFile->Write(wxT("FRSKY_STICKS"),FRSKY_STICKS);
    configFile->Write(wxT("CORRECT_NEGATIVE_VALUES"),CORRECT_NEGATIVE_VALUES);
    configFile->Write(wxT("ARITHMETIC_OVERFLOW_CHECK"),ARITHMETIC_OVERFLOW_CHECK);
    configFile->Write(wxT("ACCURAT_THROTTLE_STATS"),ACCURAT_THROTTLE_STATS);
    configFile->Write(wxT("SP22"),SP22);
    configFile->Write(wxT("PWM_BACKLIGHT"),PWM_BACKLIGHT);
    configFile->Write(wxT("OVERRIDE_CHANNEL_FUNCTION"),OVERRIDE_CHANNEL_FUNCTION);
    configFile->Write(wxT("WS_HOW_HIGH"),WS_HOW_HIGH);
    configFile->Write(wxT("TX_CADDY"),TX_CADDY);
    configFile->Write(wxT("TOGGLETRIM"),TOGGLETRIM);
    configFile->Write(wxT("NOANDSECONDE"),NOANDSECONDE);
    configFile->Write(wxT("SHUTDOWN_CONFIRMATION"),SHUTDOWN_CONFIRMATION);
    // [SPLASH]
    configFile->SetPath("/"+Profil+"/");
    configFile->SetPath("SPLASH/");
    configFile->Write(wxT("SPLASH_DEFINED"),personalSplash);
    if (personalSplash)
    {
        for (int i=0; (i < (SPLASHLENGHT)); ++i)
        {
            wxString lineName;
            lineName.Printf("DATA_N%u",i);
            configFile->Write(lineName,LbmSplash[i]);
        }
    }
    else
    {
        for (int i=0; (i < (SPLASHLENGHT)); ++i)
        {
            wxString lineName;
            lineName.Printf("DATA_N%u",i);
            configFile->DeleteEntry(lineName,1);
        }
    }

    configFile->Flush();
    configFile->SetPath("/");
}


void OpenAVRc_DesktopFrame::OnSimulateurClick2(wxCommandEvent& event)
{
    wxString simu("OpenAVRc_Simulator.exe");
    wxExecute(simu);
}

void OpenAVRc_DesktopFrame::OnClose(wxCloseEvent& event)
{
    if (Ini_Changed) SaveConfig();
    Destroy();
}

void OpenAVRc_DesktopFrame::OnATMEGA2560CompilerSelected(wxCommandEvent& event)
{
    CompilerOptionsFrame* atmegaFrame = new CompilerOptionsFrame(NULL);
    atmegaFrame->Show(TRUE);//opens CommunicationsFrame
}

void OpenAVRc_DesktopFrame::OnNexStepRc_M2560_docsSelected(wxCommandEvent& event)
{
    wxLaunchDefaultBrowser(wxT("https://github.com/Ingwie/NextStepRc-2.18/tree/master/documentation/NextStepRC_M2560"), NULL);
}

void OpenAVRc_DesktopFrame::OnMenuJQ6500_PCBSelected(wxCommandEvent& event)
{
    wxLaunchDefaultBrowser(wxT("https://github.com/Ingwie/NextStepRc-2.18/tree/master/documentation/Beta%20tester%20files/JQ6500%20PCB%20by%20Pyrall"), NULL);
}

void OpenAVRc_DesktopFrame::OnMenuVoice_tutoSelected(wxCommandEvent& event)
{
    wxLaunchDefaultBrowser(wxT("https://github.com/Ingwie/NextStepRc-2.18/tree/master/documentation/Beta%20tester%20files/Voice%20by%20JPZ"), NULL);
}

void OpenAVRc_DesktopFrame::OnMenuVOICE_AUDIO_PCBSelected(wxCommandEvent& event)
{
    wxLaunchDefaultBrowser(wxT("https://github.com/Ingwie/NextStepRc-2.18/tree/master/documentation/Beta%20tester%20files/Voice-audio%20pcb%20and%20rtc%20by%20Pyrall"), NULL);
}

void OpenAVRc_DesktopFrame::OnWIN_FIRMWARESelected(wxCommandEvent& event)
{
    wxLaunchDefaultBrowser(wxT("https://github.com/Ingwie/NextStepRc-2.18/tree/master/documentation/Beta%20tester%20files/Win%20firmware%20compilation%20by%20JPZ"), NULL);
}

void OpenAVRc_DesktopFrame::OnMenuMousseSelected(wxCommandEvent& event)
{
    wxLaunchDefaultBrowser(wxT("http://www.sitakiki.fr/modnaval/nextsteprc.htm"), NULL);
}

void OpenAVRc_DesktopFrame::OnListBoxConfigDClick(wxCommandEvent& event)
{
    wxString temp = ListBoxConfig->GetString(ListBoxConfig->GetSelection());
    wxMessageDialog *Select_config = new wxMessageDialog(NULL,wxT("Charger la configuration ") + temp,wxT("Configuration"), wxICON_QUESTION | wxCENTRE | wxCANCEL | wxOK );
    if (Select_config->ShowModal() == wxID_OK)
    {
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
    wxString temp = wxGetTextFromUser(wxT(""), wxT("Appelation de la nouvelle configuration"), wxT(""));
    if (temp != "")
    {
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
    if (Profil == defaut)
    {
        wxMessageBox(_("Defaut ne peut être effacé"), "",wxICON_EXCLAMATION | wxCENTRE, this);
        return;
    }
    else
    {
        int answer = wxMessageBox(_("Supprimer " + Profil + " ?"), "",wxOK | wxCANCEL | wxICON_EXCLAMATION | wxCENTRE | wxCANCEL_DEFAULT, this);
        if (answer == wxOK)
        {
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
    for (uint8_t y=0; y < (LCD_H / 8); y++)
    {
        for (uint8_t x=0; x < LCD_W; x++)
        {
            uint8_t bit = LbmSplash[p];
            ++p;
            for (uint8_t i=0; i < 8; i++)
            {
                if (bit & 0x01) dc.DrawRectangle(x*LcdScale,(y*8*LcdScale) +(i*LcdScale),LcdScale,LcdScale);
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
    wxFileDialog openFileDialog(this, _("Fichier image"), "", "","Fichier image (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) return;

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

    for (uint32_t i=0; (i<(LCD_W*LCD_H*3)); i+=3)
    {
        imgpix[j+2] >>= 1;
        if (imgraw[i] == 0) imgpix[j+2] |= 0x80;
        ++j;
        if ((j%LCD_W) == 0)
        {
            ++line;
            if (line != LCD_H/8)
            {
                j-=LCD_W;
            }
            else
            {
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
    VoiceEditFrame* voiceFrame = new VoiceEditFrame(NULL);
    voiceFrame->Show(TRUE);//opens voice edit
}


void OpenAVRc_DesktopFrame::OnButtonGenerateurClick(wxCommandEvent& event)
{
}

void OpenAVRc_DesktopFrame::OnButtonCarteSDClick(wxCommandEvent& event)
{
    wxArrayString DriveList, VoiceModule;

    VoiceModule.Add("JQ6500");
    VoiceModule.Add("WTV20SD");
    VoiceModule.Add("SOMO14D");

    wxFSVolume DriveSearch;

    DriveList = DriveSearch.GetVolumes( wxFS_VOL_MOUNTED | wxFS_VOL_REMOVABLE );

    wxSingleChoiceDialog DriveName(this,
                                   _("La carte SD doit avoir été formatée pour\n un bon fonctionnement sur la radio"),
                                   _("Selectionnez le lecteur"),
                                   DriveList
                                  );

    if (DriveName.ShowModal() == wxID_OK)
    {
        wxSingleChoiceDialog ModuleName(this,
                                        _("Quel module ""VOICE"" est dans la radio"),
                                        _("Selectionnez le module"),
                                        VoiceModule
                                       );

        if (ModuleName.ShowModal() == wxID_OK)
        {
            wxString Drive = DriveName.GetStringSelection();
            wxString SourceDirectory;
            wxString DestinationDirectory;
            wxString FileName;
            wxString Ext;

            if (ModuleName.GetStringSelection() == "JQ6500")
            {
                SourceDirectory = "\\VOICEMP3\\";
                DestinationDirectory = "\\VOICEMP3\\";
                Ext = "mp3";
                wxMkdir(Drive + DestinationDirectory);
            }
            else
            {
                SourceDirectory = "\\VOICEAD4\\";
                DestinationDirectory = "\\";
                Ext = "AD4";
            }

            wxBusyInfo wait("Copie en cours, attendez SVP......");

            for (int i = 0; i < 512 ; i++)
            {
                FileName.Printf("%04d.",i);
                wxCopyFile(AppPath + SourceDirectory + FileName + Ext, Drive + DestinationDirectory + FileName + Ext, false);
            }
        }
    }
    Sleep(50);
    DrawLbmSplash();
}

void OpenAVRc_DesktopFrame::OnChoiceLangueSelect(wxCommandEvent& event)
{
    voice_Langue = ChoiceLangue->GetString(ChoiceLangue->GetSelection());
    Ini_Changed = true;
}

void OpenAVRc_DesktopFrame::OnMenuChoiceVoiceSelected(wxCommandEvent& event)
{
    if (wxSetWorkingDirectory(AppPath)) wxExecute("CMD /c tts.exe -V ->voices.txt", wxEXEC_HIDE_CONSOLE | wxEXEC_SYNC);
    Ini_Changed = true;
    Sleep(500);
    Voice_choice* voiceChoiceFrame = new Voice_choice(NULL);
    voiceChoiceFrame->Show(TRUE);
}
