/***************************************************************
 * Name:      NextStepRc_DesktopMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    NextStepRc_TEAM ()
 * Created:   2016-09-08
 * Copyright: NextStepRc_TEAM ()
 * License:
 **************************************************************/

#include "DefaultFrame.h"
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include "NextStepRc_DesktopMain.h"

//(*InternalHeaders(DefaultFrame)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

extern bool Ini_Changed;
extern wxString avrdudepath;
extern wxString dude_programmer;
extern wxString dude_type;
extern wxString dude_port;


//(*IdInit(DefaultFrame)
const long DefaultFrame::ID_COMBOBOX1 = wxNewId();
const long DefaultFrame::ID_COMBOBOX2 = wxNewId();
const long DefaultFrame::ID_STATICTEXT1 = wxNewId();
const long DefaultFrame::ID_BUTTONENTER = wxNewId();
const long DefaultFrame::ID_STATICTEXT2 = wxNewId();
const long DefaultFrame::ID_COMBOBOX3 = wxNewId();
const long DefaultFrame::ID_STATICTEXT3 = wxNewId();
const long DefaultFrame::ID_STATICTEXT5 = wxNewId();
const long DefaultFrame::ID_TEXTCTRL1 = wxNewId();
const long DefaultFrame::ID_BUTTONESC = wxNewId();
const long DefaultFrame::ID_BUTTONSEARCHAVRDUDEPATH = wxNewId();
const long DefaultFrame::ID_BUTTONDETECT = wxNewId();
//*)

BEGIN_EVENT_TABLE(DefaultFrame,wxFrame)

    //(*EventTable(DefaultFrame)
    //*)
END_EVENT_TABLE()

DefaultFrame::DefaultFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(DefaultFrame)
    Create(parent, id, _("Communication avec la radio "), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxDEFAULT_DIALOG_STYLE, _T("id"));
    SetClientSize(wxSize(473,335));
    Move(wxDefaultPosition);
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    ComboBox1 = new wxComboBox(this, ID_COMBOBOX1, wxEmptyString, wxPoint(176,72), wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX1"));
    ComboBox1->Append(_("stk500v2"));
    ComboBox1->Append(_("stk500"));
    ComboBox1->Append(_("avrisp"));
    ComboBox1->Append(_("usbasp"));
    ComboBox2 = new wxComboBox(this, ID_COMBOBOX2, wxEmptyString, wxPoint(176,120), wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX2"));
    ComboBox2->Append(_("usb"));
    ComboBox2->Append(_("avrdoper"));
    ComboBox2->Append(_("/dev/ttyUSB0"));
    ComboBox2->Append(_("/dev/ttyUSB1"));
    ComboBox2->Append(_("/dev/ttyUSB2"));
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Programmeur"), wxPoint(72,80), wxSize(80,13), 0, _T("ID_STATICTEXT1"));
    ButtonEnter = new wxButton(this, ID_BUTTONENTER, _("Valider"), wxPoint(376,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONENTER"));
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Port"), wxPoint(112,128), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    ComboBox3 = new wxComboBox(this, ID_COMBOBOX3, wxEmptyString, wxPoint(176,168), wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX3"));
    ComboBox3->Append(_("m2560"));
    ComboBox3->Append(_("m128"));
    ComboBox3->Append(_("m64"));
    StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Type de radio"), wxPoint(72,176), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Reprtoire de avrdude"), wxPoint(32,224), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxPoint(176,216), wxSize(212,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    ButtonEsc = new wxButton(this, ID_BUTTONESC, _("Annuler"), wxPoint(264,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONESC"));
    ButtonSearchavrdudepath = new wxButton(this, ID_BUTTONSEARCHAVRDUDEPATH, _("..."), wxPoint(400,216), wxSize(40,23), 0, wxDefaultValidator, _T("ID_BUTTONSEARCHAVRDUDEPATH"));
    ButtonDetect = new wxButton(this, ID_BUTTONDETECT, _("Detection"), wxPoint(304,120), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONDETECT"));

    Connect(ID_BUTTONENTER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DefaultFrame::OnButtonEnterClick);
    Connect(ID_BUTTONESC,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DefaultFrame::OnButtonEscClick);
    Connect(ID_BUTTONSEARCHAVRDUDEPATH,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DefaultFrame::OnButtonSearchavrdudepathClick);
    Connect(ID_BUTTONDETECT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DefaultFrame::OnButtonDetectClick);
    //*)

    {
    	wxIcon FrameIcon;
    	SetIcon(wxICON(nsrcd_icon));
    }

    //fills the comboxes with ini file defaults

    ComboBox1->SetValue(dude_programmer);
    ComboBox2->SetValue(dude_port);
    ComboBox3->SetValue(dude_type);
    TextCtrl1->SetValue(avrdudepath);

}

DefaultFrame::~DefaultFrame()
{
    //(*Destroy(DefaultFrame)
    //*)
}

void DefaultFrame::OnButtonSearchavrdudepathClick(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, _("Emplacement d'AVRDUDE"), "", "","AVRDUDE (*.bin)|*.exe", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) return;
    TextCtrl1->SetValue(openFileDialog.GetPath());
}

void DefaultFrame::OnButtonEscClick(wxCommandEvent& event)
{
    Destroy();
}

void DefaultFrame::OnButtonEnterClick(wxCommandEvent& event)//Saves new default values and closes
{
    dude_programmer = ComboBox1->GetValue();
    dude_port = ComboBox2->GetValue();
    dude_type = ComboBox3->GetValue();
    avrdudepath = TextCtrl1->GetValue();
    Ini_Changed = true;
    Destroy();
}


void DefaultFrame::OnButtonDetectClick(wxCommandEvent& event)
{
    int answer = wxMessageBox(_("Dérancher la radio du PC"), _("Auto Detection"), wxOK, this);
    if (answer == wxOK)
    {
        DetectSerial();
        answer = wxMessageBox(_("Branchez votre radio"), _("Auto Detection"), wxOK, this);
        if (answer == wxOK)
        {
            DetectSerial();
            ComboBox2->SetSelection(0);
            wxMessageBox(_("Terminé !"), _("Auto Detection"), wxOK, this);
        }
    }
}

void DefaultFrame::DetectSerial()
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
