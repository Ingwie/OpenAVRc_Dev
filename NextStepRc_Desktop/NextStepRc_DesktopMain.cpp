/***************************************************************
 * Name:      NextStepRc_DesktopMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    NextStepRc_TEAM ()
 * Created:   2016-09-08
 * Copyright: NextStepRc_TEAM ()
 * License:
 **************************************************************/

#include "NextStepRc_DesktopMain.h"
#include "DefaultFrame.h"
#include "CompilerOptionsFrame.h"
#include <wx/msgdlg.h>
//#include <wx/arrstr.h>
//#include <iostream>
//#include <fstream>
#include <string>
#include <wx/textfile.h>
#include <windows.h>
#include "Shlwapi.h"
#include <wx/filedlg.h>
#include <wx/wfstream.h>
#include <wx/string.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/dialog.h>


//(*InternalHeaders(NextStepRc_DesktopFrame)
#include <wx/intl.h>
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

bool Ini_Changed = false;

wxString avrdudepath = _("non défini");
wxString dude_programmer = _("non défini");
wxString dude_type = _("non défini");
wxString dude_port = _("non défini");



//(*IdInit(NextStepRc_DesktopFrame)
const long NextStepRc_DesktopFrame::ID_BUTTON1 = wxNewId();
const long NextStepRc_DesktopFrame::ID_PANEL1 = wxNewId();
const long NextStepRc_DesktopFrame::idMenuQuit = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM1 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM3 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM5 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM4 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM6 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM9 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM10 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM7 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUAT2560 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEMHTMLDOC = wxNewId();
const long NextStepRc_DesktopFrame::idMenuAbout = wxNewId();
const long NextStepRc_DesktopFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(NextStepRc_DesktopFrame,wxFrame)
    //(*EventTable(NextStepRc_DesktopFrame)
    //*)
END_EVENT_TABLE()


NextStepRc_DesktopFrame::NextStepRc_DesktopFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(NextStepRc_DesktopFrame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxMenu* Menu2;

    Create(0, id, _("NextStepRc Desktop"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    SetClientSize(wxSize(589,282));
    Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(240,40), wxSize(576,280), 0, _T("ID_PANEL1"));
    Panel1->SetFocus();
    Button1 = new wxButton(Panel1, ID_BUTTON1, _("SIMULATEUR"), wxPoint(8,208), wxSize(568,31), 0, wxDefaultValidator, _T("ID_BUTTON1"));
    MenuBar_main = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quitter\tAlt-F4"), _("Quitter l\'application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar_main->Append(Menu1, _("Action"));
    Menu3 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu3, ID_MENUITEM1, _("Programmeur"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem3);
    MenuBar_main->Append(Menu3, _("Paramètres"));
    Menu4 = new wxMenu();
    Menu4->AppendSeparator();
    Menu5 = new wxMenuItem(Menu4, ID_MENUITEM3, _("Lire modèles de la radio"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(Menu5);
    MenuItem6 = new wxMenuItem(Menu4, ID_MENUITEM5, _("Lire le firmware de la radio"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem6);
    Menu4->AppendSeparator();
    MenuItem5 = new wxMenuItem(Menu4, ID_MENUITEM4, _("Écrire les modêles dans la radio"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem5);
    MenuItem7 = new wxMenuItem(Menu4, ID_MENUITEM6, _("Écrire le firmware dans la radio"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem7);
    Menu4->AppendSeparator();
    MenuItem8 = new wxMenu();
    MenuItem10 = new wxMenuItem(MenuItem8, ID_MENUITEM9, _("Écrire les fusibles"), wxEmptyString, wxITEM_NORMAL);
    MenuItem8->Append(MenuItem10);
    MenuItem11 = new wxMenuItem(MenuItem8, ID_MENUITEM10, _("Écrire le bootloader"), wxEmptyString, wxITEM_NORMAL);
    MenuItem8->Append(MenuItem11);
    Menu4->Append(ID_MENUITEM7, _("Bootloader"), MenuItem8, wxEmptyString);
    MenuBar_main->Append(Menu4, _("Lire/Écrire"));
    Menu7 = new wxMenu();
    ATMEGA2560Compiler = new wxMenuItem(Menu7, ID_MENUAT2560, _("ATMEGA2560"), wxEmptyString, wxITEM_NORMAL);
    Menu7->Append(ATMEGA2560Compiler);
    MenuBar_main->Append(Menu7, _("Compilateur"));
    Menu2 = new wxMenu();
    MenuHtmlDoc = new wxMenuItem(Menu2, ID_MENUITEMHTMLDOC, _("Documentation en ligne"), wxEmptyString, wxITEM_NORMAL);
    Menu2->Append(MenuHtmlDoc);
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("A propos\tF1"), _("NestStepRc_Desktop V 0.00001 !"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar_main->Append(Menu2, _("Aide"));
    SetMenuBar(MenuBar_main);
    StatusBar_main = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar_main->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar_main->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar_main);

    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnSimulateurClick2);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnQuit);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnProgrammerSelected);
    Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnreadmodelsSelected);
    Connect(ID_MENUITEM5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnreadfirmwareSelected);
    Connect(ID_MENUITEM4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnWriteModelToRadioSelected);
    Connect(ID_MENUITEM6,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnWriteFirmwareToRadioSelected);
    Connect(ID_MENUITEM9,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnEcrirelesFuseesSelected);
    Connect(ID_MENUITEM10,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnEcrirelebootloaderSelected);
    Connect(ID_MENUAT2560,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnATMEGA2560CompilerSelected);
    Connect(ID_MENUITEMHTMLDOC,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnMenuHtmlDocSelected);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnAbout);
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnClose);
    //*)

    //Const
    dude_c = (" -c ");
    dude_p = (" -p ");
    dude_D = (" -D ");
    dude_P = (" -P ");
    dude_space = (" ");
    dude_U = (" -U ");
    dude_eeprom = ("eeprom:");
    dude_flash = ("flash:");
    dude_raw = (":r");
    dude_write = ("w:");
    dude_verify = (" -v ");
    dude_read = ("r:");
    dude_intel = (":i");
    keepopen = ("cmd /k ");


    //Ini File
    Ini_Filename = wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator() + "NextStepRcDesktop.ini";
    configFile = new wxFileConfig( "", "", Ini_Filename);
    LoadConfig();
    if (avrdudepath == _("non défini")) wxMessageBox( _("Merci de vérifier les paramètres"), _("Programmeur :"), wxICON_WARNING);
}


NextStepRc_DesktopFrame::~NextStepRc_DesktopFrame()
{
    //(*Destroy(NextStepRc_DesktopFrame)
    //*)
}

void NextStepRc_DesktopFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void NextStepRc_DesktopFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg ="NestStepRc_Desktop V0.3B Merci Miguel !";
    wxMessageBox(msg, _("Bienvenue dans ..."));
}

void NextStepRc_DesktopFrame::OnProgrammerSelected(wxCommandEvent& event)
{
    DefaultFrame * DudeFrame = new DefaultFrame(NULL);
    DudeFrame->Show(TRUE);//opens DefaultFrame
}

void NextStepRc_DesktopFrame::OnreadmodelsSelected(wxCommandEvent& event)//READ MODELS FROM RADIO.
{
    wxFileDialog saveDialog(this, _("Choisir le fichier pour importer les modèles dès la radio."), "", "",  "Fichiers bin (*.bin)|*.bin|Tous (*.*)|*.*", wxFD_SAVE);
    if (saveDialog.ShowModal() == wxID_CANCEL) return;
    wxString dude_tmpfile = (saveDialog.GetPath());

    //wxString dude_mode("mode com5 DTR=ON RTS=ON");
    wxString dude_send = keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_eeprom+dude_read+dude_tmpfile+dude_raw+dude_verify;

    wxMessageBox(dude_send);
    wxExecute(dude_send);//send command
}

void NextStepRc_DesktopFrame::OnreadfirmwareSelected(wxCommandEvent& event)//read firmware from radio
{
    wxFileDialog saveDialog(this, _("Choisir le fichier pour importer le Firmware dès la radio."), "", "","Fichiers bin (*.bin)|*.bin|Tous (*.*)|*.*", wxFD_SAVE);
    if (saveDialog.ShowModal() == wxID_CANCEL) return;
    //wxFileInputStream input_stream(saveDialog.GetPath());
    wxString dude_tmpfile = (saveDialog.GetPath());

    wxString dude_send = keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_flash+dude_read+dude_tmpfile+dude_raw;
    wxExecute(dude_send);
}

void NextStepRc_DesktopFrame::OnWriteModelToRadioSelected(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, _("Choisir le fichier (.bin) pour transferer les modêles à la radio."), "", "","Fichiers BIN (*.bin)|*.bin", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) return;

    //verify the file size to see if it is an eeprom image

    //wxMessageDialog *susto = new wxMessageDialog(NULL,
    //wxT("Cela ne parait pas un fichier de modeles"), wxT("Importer les modeles"),
    //wxOK | wxICON_WARNING | wxCANCEL | wxCANCEL_DEFAULT);
    //susto->SetEventHandler(susto);
    //if (susto->ShowModal()!= wxID_OK) return;


    wxString dude_tmpfile = (openFileDialog.GetPath());

    wxString dude_send = keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_eeprom+dude_write+dude_tmpfile+dude_raw+dude_verify;

    wxMessageBox(dude_send);
    wxExecute(dude_send);
}

void NextStepRc_DesktopFrame::OnWriteFirmwareToRadioSelected(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, _("Choisir le fichier pour transferer le Firmware à la radio."), "", "","Fichiers BIN (*.bin)|*.bin", wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL) return;
    wxMessageDialog *bkup = new wxMessageDialog(NULL,wxT("Il est recommande de sauvegarder vos modeles avant, voulez vous continuer ?"), wxT("Firmware"),wxOK | wxICON_WARNING | wxCANCEL | wxCANCEL_DEFAULT);
    bkup->SetEventHandler(bkup);
    if (bkup->ShowModal()!= wxID_OK) return;

    /*  std::remove("batch.txt"); // delete old batch
      std::ofstream myfile;
      myfile.open ("batch.bat"); // write new batch
      wxString batch ("batch.bat"); //myfile << dude_programmer<< "\n";
      wxString one ("mode "), two(": DTR=ON RTS=ON \n"), dude_rstcom =(one+dude_port+two);

      wxString dude_bkupfile = ("tmp.bin");// save model and settings
      dude_send = dude_rstcom + dude_path+avrdude+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_eeprom+dude_read+dude_bkupfile+dude_raw;
      myfile << dude_send<< "\n";

      wxString dude_tmpfile = GetCurrentlySelectedFilename;//write firmware
      dude_send = dude_path+avrdude+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_flash+dude_write+dude_tmpfile+dude_raw+dude_verify;
      myfile << dude_send<< "\n";

      wxString dude_bkupfile1 = ("tmp.bin");// restaure models and settings
      dude_send = dude_rstcom +dude_path+avrdude+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_eeprom+dude_write+dude_bkupfile1+dude_raw+dude_verify;
      myfile << dude_send<< "\n";

      myfile.close();
      wxExecute(batch);
      std::remove("tmp.bin");
      std::remove("batch.txt");
      //wxExecute(dude_send); //if non backup is chosen
      */

    wxString dude_tmpfile = (openFileDialog.GetPath());//write firmware
    wxString dude_send =keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_flash+dude_write+dude_tmpfile+dude_raw+dude_verify;

    wxMessageBox(dude_send);
    wxExecute(dude_send);

}


void NextStepRc_DesktopFrame::OnEcrirelesFuseesSelected(wxCommandEvent& event)// Write fuses
{
    wxMessageDialog *susto = new wxMessageDialog(NULL,
            wxT("Sur? Tu veut continuer?"), wxT("Burn Fuses"),
            wxOK | wxICON_WARNING | wxCANCEL | wxCANCEL_DEFAULT);
    susto->SetEventHandler(susto);
    if (susto->ShowModal()!= wxID_OK) return;

    wxString FUSES(" -c usbasp -P usb -F -e -u -Ulfuse:w:0xFF:m -Uhfuse:w:0xD8:m -Uefuse:w:0xFD:m -v");
    wxString dude_send = (keepopen+avrdudepath+dude_p+dude_type+FUSES);
    wxMessageBox(dude_send);
    wxExecute(dude_send);
}

void NextStepRc_DesktopFrame::OnEcrirelebootloaderSelected(wxCommandEvent& event) // Write bootloader
{
    wxMessageDialog *susto = new wxMessageDialog(NULL,
            wxT("Sur? Tu veut continuer?"), wxT("Burn bootloader"),wxOK | wxICON_WARNING | wxCANCEL | wxCANCEL_DEFAULT);
    susto->SetEventHandler(susto);
    if (susto->ShowModal()!= wxID_OK) return;

    wxString BOOTLOADER(" -c usbasp -P usb -U lock:w:0x3F:m -U flash:w:mega2560_stk500v2boot_opentx.hex -U lock:w:0x0F:m -v");
    wxString dude_send = (keepopen+avrdudepath+dude_p+dude_type+BOOTLOADER);
    wxMessageBox(dude_send);
    wxExecute(dude_send);
}

void NextStepRc_DesktopFrame::LoadConfig()
{
    configFile->Read("Programmer",&dude_programmer);
    configFile->Read(wxT("Port"),&dude_port);
    configFile->Read(wxT("Type"),&dude_type);
    configFile->Read(wxT("avrdudepath"),&avrdudepath);
//////////////////////////////////////////////////
    configFile->Read(wxT("PCB"),&PCB);
    configFile->Read(wxT("LCD"),&LCD);
    configFile->Read(wxT("VOICE"),&VOICE);
    configFile->Read(wxT("EXT"),&EXT);
    configFile->Read(wxT("PPM_UNIT"),&PPM_UNIT);
}

void NextStepRc_DesktopFrame::SaveConfig()
{
    wxMessageBox( Ini_Filename, _("Les paramètres sont sauvé dans :"));

    configFile->Write(wxT("Programmer"),dude_programmer);
    configFile->Write(wxT("Port"),dude_port);
    configFile->Write(wxT("Type"),dude_type);
    configFile->Write(wxT("avrdudepath"),avrdudepath);
///////////////////////////////////////////////////////
    configFile->Write(wxT("PCB"),PCB);
    configFile->Write(wxT("LCD"),LCD);
    configFile->Write(wxT("VOICE"),VOICE);
    configFile->Write(wxT("EXT"),EXT);
    configFile->Write(wxT("PPM_UNIT"),PPM_UNIT);
///////////////////////////////////////////////////////
    configFile->Flush();
}

void NextStepRc_DesktopFrame::OnSimulateurClick2(wxCommandEvent& event)
{
    wxString simu("NextStepRc_Simulator.exe");
    wxExecute(simu);
}


void NextStepRc_DesktopFrame::OnClose(wxCloseEvent& event)
{
    if (Ini_Changed) SaveConfig();
    Destroy();
}

void NextStepRc_DesktopFrame::OnATMEGA2560CompilerSelected(wxCommandEvent& event)
{
    CompilerOptionsFrame* atmegaFrame = new CompilerOptionsFrame(NULL);
    atmegaFrame->Show(TRUE);//opens DefaultFrame
}

void NextStepRc_DesktopFrame::OnMenuHtmlDocSelected(wxCommandEvent& event)
{
  wxLaunchDefaultBrowser(wxT("https://github.com/Ingwie/NextStepRc-2.18/tree/master/documentation"), NULL);
}
