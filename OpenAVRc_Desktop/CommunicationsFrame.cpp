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


#include <wx/filedlg.h>
#include <wx/msgdlg.h>

#include "CommunicationsFrame.h"
#include "OpenAVRc_DesktopMain.h"


//(*InternalHeaders(CommunicationsFrame)
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

extern bool Ini_Changed;
extern wxString avrdudepath;
extern wxString dude_programmer;
extern wxString dude_type;
extern wxString dude_port;
extern wxString AppPath;
extern wxString keepopen;
extern wxString dude_c;
extern wxString dude_P;
extern wxString dude_p;


//(*IdInit(CommunicationsFrame)
const long CommunicationsFrame::ID_STATICBOX1 = wxNewId();
const long CommunicationsFrame::ID_COMBOBOX1 = wxNewId();
const long CommunicationsFrame::ID_COMBOBOX2 = wxNewId();
const long CommunicationsFrame::ID_STATICTEXT1 = wxNewId();
const long CommunicationsFrame::ID_BUTTONENTER = wxNewId();
const long CommunicationsFrame::ID_STATICTEXT2 = wxNewId();
const long CommunicationsFrame::ID_COMBOBOX3 = wxNewId();
const long CommunicationsFrame::ID_STATICTEXT3 = wxNewId();
const long CommunicationsFrame::ID_STATICTEXT5 = wxNewId();
const long CommunicationsFrame::ID_TEXTCTRL1 = wxNewId();
const long CommunicationsFrame::ID_BUTTONESC = wxNewId();
const long CommunicationsFrame::ID_BUTTONSEARCHAVRDUDEPATH = wxNewId();
const long CommunicationsFrame::ID_BUTTONDETECT = wxNewId();
const long CommunicationsFrame::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(CommunicationsFrame,wxFrame)

    //(*EventTable(CommunicationsFrame)
    //*)
END_EVENT_TABLE()

CommunicationsFrame::CommunicationsFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(CommunicationsFrame)
    Create(parent, wxID_ANY, _("Communication avec la radio "), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(473,335));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    StaticBox1 = new wxStaticBox(this, ID_STATICBOX1, _("Paramètres de communication avec la radio"), wxPoint(24,24), wxSize(424,248), 0, _T("ID_STATICBOX1"));
    ComboBox1 = new wxComboBox(this, ID_COMBOBOX1, wxEmptyString, wxPoint(176,72), wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX1"));
    ComboBox1->Append(_("stk500v2"));
    ComboBox1->Append(_("stk500"));
    ComboBox1->Append(_("avrisp"));
    ComboBox1->Append(_("usbasp"));
    ComboBox2 = new wxComboBox(this, ID_COMBOBOX2, wxEmptyString, wxPoint(176,120), wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX2"));
    ComboBox2->Append(_("usb"));
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Programmateur"), wxPoint(64,80), wxSize(96,16), 0, _T("ID_STATICTEXT1"));
    ButtonEnter = new wxButton(this, ID_BUTTONENTER, _("Valider"), wxPoint(376,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONENTER"));
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Port"), wxPoint(112,128), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    ComboBox3 = new wxComboBox(this, ID_COMBOBOX3, wxEmptyString, wxPoint(176,168), wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX3"));
    ComboBox3->Append(_("m2560"));
    ComboBox3->Append(_("m2560fram"));
    StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Type de radio"), wxPoint(72,176), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Répertoire de avrdude"), wxPoint(32,224), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxPoint(176,216), wxSize(212,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    ButtonEsc = new wxButton(this, ID_BUTTONESC, _("Annuler"), wxPoint(264,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONESC"));
    ButtonSearchavrdudepath = new wxButton(this, ID_BUTTONSEARCHAVRDUDEPATH, _("..."), wxPoint(400,216), wxSize(40,23), 0, wxDefaultValidator, _T("ID_BUTTONSEARCHAVRDUDEPATH"));
    ButtonDetect = new wxButton(this, ID_BUTTONDETECT, _("Detection"), wxPoint(304,120), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONDETECT"));
    Button1 = new wxButton(this, ID_BUTTON1, _("Test"), wxPoint(152,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));

    Connect(ID_BUTTONENTER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CommunicationsFrame::OnButtonEnterClick);
    Connect(ID_BUTTONESC,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CommunicationsFrame::OnButtonEscClick);
    Connect(ID_BUTTONSEARCHAVRDUDEPATH,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CommunicationsFrame::OnButtonSearchavrdudepathClick);
    Connect(ID_BUTTONDETECT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CommunicationsFrame::OnButtonDetectClick);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CommunicationsFrame::OnTESTClick1);
    //*)

    {
        wxIcon FrameIcon;
        SetIcon(wxICON(oavrc_icon));
    }

    //fills the comboxes with ini file defaults

    ComboBox1->SetValue(dude_programmer);
    ComboBox2->SetValue(dude_port);
    ComboBox3->SetValue(dude_type);
    TextCtrl1->SetValue(avrdudepath);

}

CommunicationsFrame::~CommunicationsFrame()
{
    //(*Destroy(CommunicationsFrame)
    //*)
}

void CommunicationsFrame::OnButtonSearchavrdudepathClick(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, _("Emplacement d'AVRDUDE"), AppPath + "\\avrdude\\", "","AVRDUDE (*.exe)|*.exe", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) return;
    TextCtrl1->SetValue(openFileDialog.GetPath());
}

void CommunicationsFrame::OnButtonEscClick(wxCommandEvent& event)
{
    Destroy();
}

void CommunicationsFrame::OnButtonEnterClick(wxCommandEvent& event)//Saves new default values and closes
{
    dude_programmer = ComboBox1->GetValue();
    dude_port = ComboBox2->GetValue();
    dude_type = ComboBox3->GetValue();
    avrdudepath = TextCtrl1->GetValue();
    Ini_Changed = true;
    Destroy();
}


void CommunicationsFrame::OnButtonDetectClick(wxCommandEvent& event)
{
    int answer = wxMessageBox(_("Débrancher la radio du PC"), _("Auto Detection"), wxOK  | wxCENTRE, this);
    if (answer == wxOK)
    {
        DetectSerial();
        answer = wxMessageBox(_("Branchez votre radio"), _("Auto Detection"), wxOK  | wxCENTRE, this);
        if (answer == wxOK)
        {
            DetectSerial();
            ComboBox2->SetSelection(0);
            wxMessageBox(_("Terminé !"), _("Auto Detection"), wxOK  | wxCENTRE, this);
        }
    }
}

void CommunicationsFrame::DetectSerial()
{
    TCHAR Devices [5000];
    for(int i=0; i<255; i++) // checking ports from COM0 to COM255
    {
        wxString str;
        str = str.Format(wxT("%i"),i);
        wxString ComName = "COM"+str; // converting to COM0, COM1, COM2
        long test = QueryDosDevice(ComName.c_str(), Devices, 5000); //Win32(64) API only
        if (test!=0) //QueryDosDevice returns zero if it didn't find an object
        {
            ComboBox2->Insert(ComName,0); // add to the ComboBox
        }
    }
}

void CommunicationsFrame::OnTESTClick1(wxCommandEvent& event)
{
    dude_programmer = ComboBox1->GetValue();
    dude_port = ComboBox2->GetValue();
    dude_type = ComboBox3->GetValue();
    avrdudepath = TextCtrl1->GetValue();
    wxString dude_send = keepopen+avrdudepath+dude_c+dude_programmer+dude_p+dude_type+dude_P+dude_port;
    wxExecute(dude_send);
}
