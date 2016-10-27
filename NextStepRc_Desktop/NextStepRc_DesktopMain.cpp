/***************************************************************
 * Name:      NextStepRc_DesktopMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    NextStepRc_TEAM ()
 * Created:   2016-09-08
 * Copyright: NextStepRc_TEAM ()
 * License:
 **************************************************************/

#include "NextStepRc_DesktopMain.h"
#include <wx/msgdlg.h>
#include "DefaultFrame.h"
#include <wx/arrstr.h>
#include <iostream>
#include <fstream>
#include <string>
#include <wx/textfile.h>
#include <windows.h>
#include "Shlwapi.h"
#include <wx/filename.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>
#include <wx/string.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/dialog.h>

wxString dude_send;
wxString avrdude("\\avrdude.exe"), dude_c(" -v -c "),dude_p(" -p "), dude_D(" -D "), dude_P(" -P "),dude_space(" "),dude_U(" -U "),dude_eeprom("eeprom:"),dude_read("r:"),dude_intel(":i");
wxString dude_flash("flash:"), dude_raw(":r"),dude_write("w:"),dude_verify(" -v");
//wxString        file;
wxString        str;
wxString keepopen("cmd /k ");
  //tfile.Open("defaults.txt");
  //wxString dude_programmer;
  //wxString dude_port;
  //wxString dude_type;
  //wxString dude_path;

//(*InternalHeaders(NextStepRc_DesktopFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

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

//(*IdInit(NextStepRc_DesktopFrame)
const long NextStepRc_DesktopFrame::ID_BUTTON1 = wxNewId();
const long NextStepRc_DesktopFrame::ID_PANEL1 = wxNewId();
const long NextStepRc_DesktopFrame::idMenuQuit = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM3 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM5 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM4 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM6 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM9 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM10 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM7 = wxNewId();
const long NextStepRc_DesktopFrame::ID_MENUITEM1 = wxNewId();
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

    Create(parent, id, _("NextStepRc Desktop"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    SetClientSize(wxSize(504,241));
    Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(240,40), wxDefaultSize, 0, _T("ID_PANEL1"));
    Panel1->SetFocus();
    Button1 = new wxButton(Panel1, ID_BUTTON1, _("SIMULATEUR"), wxPoint(8,152), wxSize(488,31), 0, wxDefaultValidator, _T("ID_BUTTON1"));
    MenuBar_main = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quitter\tAlt-F4"), _("Quitter l\'application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar_main->Append(Menu1, _("Action"));
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
    MenuItem10 = new wxMenuItem(MenuItem8, ID_MENUITEM9, _("Écrire les fusées"), wxEmptyString, wxITEM_NORMAL);
    MenuItem8->Append(MenuItem10);
    MenuItem11 = new wxMenuItem(MenuItem8, ID_MENUITEM10, _("Écrire le bootloader"), wxEmptyString, wxITEM_NORMAL);
    MenuItem8->Append(MenuItem11);
    Menu4->Append(ID_MENUITEM7, _("Bootloader"), MenuItem8, wxEmptyString);
    MenuBar_main->Append(Menu4, _("Lire/Écrire"));
    Menu3 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu3, ID_MENUITEM1, _("Comm settings"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem3);
    MenuBar_main->Append(Menu3, _("Comm"));
    Menu6 = new wxMenu();
    MenuBar_main->Append(Menu6, _("Documentation en ligne"));
    Menu2 = new wxMenu();
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
    Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnreadmodelsSelected);
    Connect(ID_MENUITEM5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnreadfirmwareSelected);
    Connect(ID_MENUITEM4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnWriteModelToRadioSelected);
    Connect(ID_MENUITEM6,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnWriteFirmwareToRadioSelected);
    Connect(ID_MENUITEM9,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnEcrirelesFuseesSelected);
    Connect(ID_MENUITEM10,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnEcrirelebootloaderSelected);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnProgrammerSelected);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&NextStepRc_DesktopFrame::OnAbout);
    //*)

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
  wxString msg ="NestStepRc_Desktop V0.00002 !";
  wxMessageBox(msg, _("Bienvenue dans ..."));
}

void Read_default_values ()//reads comm defaults from defaults.txt file
{
  wxTextFile      tfile;
  tfile.Open("defaults.txt");
  str = tfile.GetFirstLine(); wxString dude_programmer = str; // read the first line
  str = tfile.GetNextLine();  wxString dude_port = str;
  str = tfile.GetNextLine();  wxString dude_type = str;
  str = tfile.GetNextLine();  wxString dude_path = str;
}

void NextStepRc_DesktopFrame::OnProgrammerSelected(wxCommandEvent& event)
{
  DefaultFrame * DudeFrame = new DefaultFrame(NULL);
  DudeFrame->Show(TRUE);//opens DefaultFrame
}

void NextStepRc_DesktopFrame::OnreadmodelsSelected(wxCommandEvent& event)//read models from radio
{
    wxFileDialog saveDialog(this, _("Choisir le fichier pour importer les modèles dès la radio."), "", "",  "Fichiers bin (*.bin)|*.bin|Tous (*.*)|*.*", wxFD_SAVE);//wxFD_OPEN);//|wxFD_FILE_MUST_EXIST);
    if (saveDialog.ShowModal() == wxID_CANCEL) return;
    wxString 	GetCurrentlySelectedFilename(saveDialog.GetPath());
    wxString dude_tmpfile = GetCurrentlySelectedFilename;

    //Read_default_values(); Does not pass the wxString values.

    wxTextFile      tfile;
    tfile.Open("defaults.txt");
    str = tfile.GetFirstLine(); wxString dude_programmer = str; // read the first line
    str = tfile.GetNextLine();  wxString dude_port = str;
    str = tfile.GetNextLine();  wxString dude_type = str;
    str = tfile.GetNextLine();  wxString dude_path = str;
    dude_send = keepopen+dude_path+avrdude+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_eeprom+dude_read+dude_tmpfile+dude_raw;
    wxMessageBox(dude_send);
    wxExecute(dude_send);//send to control line
}

void NextStepRc_DesktopFrame::OnreadfirmwareSelected(wxCommandEvent& event)//read firmware from radio
{
    wxFileDialog saveDialog(this, _("Choisir le fichier pour importer le Firmware dès la radio."), "", "","Fichiers BIN (*.bin)|*.bin", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (saveDialog.ShowModal() == wxID_CANCEL) return;
    wxFileInputStream input_stream(saveDialog.GetPath());
    wxString 	GetCurrentlySelectedFilename(saveDialog.GetPath());
    wxString dude_tmpfile = GetCurrentlySelectedFilename;
    wxTextFile      tfile;
    tfile.Open("defaults.txt");
    str = tfile.GetFirstLine(); wxString dude_programmer = str; // read the first line
    str = tfile.GetNextLine();  wxString dude_port = str;
    str = tfile.GetNextLine();  wxString dude_type = str;
    str = tfile.GetNextLine();  wxString dude_path = str;
    dude_send = keepopen+dude_path+avrdude+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_flash+dude_read+dude_tmpfile+dude_raw;
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


    wxString 	GetCurrentlySelectedFilename(openFileDialog.GetPath());
    wxString dude_tmpfile = GetCurrentlySelectedFilename;
    wxTextFile      tfile;
    tfile.Open("defaults.txt");
    str = tfile.GetFirstLine(); wxString dude_programmer = str; // read the first line
    str = tfile.GetNextLine();  wxString dude_port = str;
    str = tfile.GetNextLine();  wxString dude_type = str;
    str = tfile.GetNextLine();  wxString dude_path = str;
    dude_send = keepopen+dude_path+avrdude+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_eeprom+dude_write+dude_tmpfile+dude_raw+dude_verify;
    //wxMessageBox(dude_send);
    wxExecute(dude_send);
}

void NextStepRc_DesktopFrame::OnWriteFirmwareToRadioSelected(wxCommandEvent& event)
{
  wxFileDialog openFileDialog(this, _("Choisir le fichier pour transferer le Firmware à la radio."), "", "","Fichiers BIN (*.bin)|*.bin", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() == wxID_CANCEL) return;

  //........ A REVOIR voulez vous faire une sauvegarde des modeles?..........
    wxMessageDialog *bkup = new wxMessageDialog(NULL,wxT("Sauvergarder les modeles? Ne fonctionne pas encore"), wxT("Firmware"),wxOK | wxICON_WARNING | wxCANCEL | wxCANCEL_DEFAULT);bkup->SetEventHandler(bkup);
    if (bkup->ShowModal()!= wxID_OK) return;
  //.........................................................................

    wxString 	GetCurrentlySelectedFilename(openFileDialog.GetPath());
    wxTextFile      tfile;
    wxString        dude_pause(" pause");
    tfile.Open("defaults.txt");
    str = tfile.GetFirstLine(); wxString dude_programmer = str; // read the first line
    str = tfile.GetNextLine();  wxString dude_port = str;
    str = tfile.GetNextLine();  wxString dude_type = str;
    str = tfile.GetNextLine();  wxString dude_path = str;

    //wxString dude_bkupfile = ("tmp.bin");
    //dude_send = keepopen+dude_path+avrdude+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_eeprom+dude_read+dude_bkupfile+dude_raw;
    //wxMessageBox(dude_send);
    //wxExecute(dude_send);

    wxString dude_tmpfile = GetCurrentlySelectedFilename;
    dude_send = keepopen+dude_path+avrdude+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_flash+dude_write+dude_tmpfile+dude_raw+dude_verify+dude_pause;
    wxMessageBox(dude_send);
    wxExecute(dude_send);

    //wxString dude_bkupfile1 = ("tmp.bin");
    //dude_send = keepopen+dude_path+avrdude+dude_c+dude_programmer+dude_p+dude_type+dude_D+dude_P+dude_port+dude_U+dude_eeprom+dude_write+dude_bkupfile1+dude_raw+dude_verify;
    //wxMessageBox(dude_send);
    //wxExecute(dude_send);

    //wxRemoveFile("tmp.bin");

}

void NextStepRc_DesktopFrame::OnEcrirelesFuseesSelected(wxCommandEvent& event)// Write fuses
{
  wxMessageDialog *susto = new wxMessageDialog(NULL,
      wxT("Sur? Tu veut continuer?"), wxT("Burn Fuses"),
      wxOK | wxICON_WARNING | wxCANCEL | wxCANCEL_DEFAULT);
  susto->SetEventHandler(susto);
  if (susto->ShowModal()!= wxID_OK) return;
  wxTextFile      tfile;
  tfile.Open("defaults.txt");
  str = tfile.GetFirstLine(); //wxString dude_programmer = str; // read the first line
  str = tfile.GetNextLine();  //wxString dude_port = str;
  str = tfile.GetNextLine();  wxString dude_type = str;
  str = tfile.GetNextLine();  wxString dude_path = str;
  wxString FUSES(" -c usbasp -P usb -F -e -u -Ulfuse:w:0xFF:m -Uhfuse:w:0xD8:m -Uefuse:w:0xFD:m -v");
  wxString dude_send = (keepopen+dude_path+avrdude+dude_p+dude_type+FUSES);
  wxMessageBox(dude_send);
  wxExecute(dude_send);
}

void NextStepRc_DesktopFrame::OnEcrirelebootloaderSelected(wxCommandEvent& event) // Write bootloader
{
  wxMessageDialog *susto = new wxMessageDialog(NULL,
  wxT("Sur? Tu veut continuer?"), wxT("Burn bootloader"),wxOK | wxICON_WARNING | wxCANCEL | wxCANCEL_DEFAULT);
  susto->SetEventHandler(susto);
  if (susto->ShowModal()!= wxID_OK) return;
  wxTextFile      tfile;
  tfile.Open("defaults.txt");
  str = tfile.GetFirstLine(); //wxString dude_programmer = str; // read the first line
  str = tfile.GetNextLine();  //wxString dude_port = str;
  str = tfile.GetNextLine();  wxString dude_type = str;
  str = tfile.GetNextLine();  wxString dude_path = str;
  wxString BOOTLOADER(" -c usbasp -P usb -U lock:w:0x3F:m -U flash:w:mega2560_stk500v2boot_opentx.hex -U lock:w:0x0F:m -v");
  wxString dude_send = (keepopen+dude_path+avrdude+dude_p+dude_type+BOOTLOADER);
  wxMessageBox(dude_send);
  wxExecute(dude_send);
}

void NextStepRc_DesktopFrame::OnButton1Click1(wxCommandEvent& event)
{
}

void NextStepRc_DesktopFrame::OnSimulateurClick2(wxCommandEvent& event)
{
  wxString simu("NextStepRc_Simulator.exe");
  wxExecute (simu);
}
