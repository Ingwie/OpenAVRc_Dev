#include "DefaultFrame.h"
#include <wx/msgdlg.h>
#include <wx/combo.h>
#include <iostream>
#include <fstream>
#include <string>
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/filedlg.h>


//(*InternalHeaders(DefaultFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include "NextStepRc_DesktopMain.h"

extern wxString avrdudepath;
extern wxString dude_programmer;
extern wxString dude_type;
extern wxString dude_port;


//(*IdInit(DefaultFrame)
const long DefaultFrame::ID_COMBOBOX1 = wxNewId();
const long DefaultFrame::ID_COMBOBOX2 = wxNewId();
const long DefaultFrame::ID_STATICTEXT1 = wxNewId();
const long DefaultFrame::ID_BUTTON1 = wxNewId();
const long DefaultFrame::ID_STATICTEXT2 = wxNewId();
const long DefaultFrame::ID_COMBOBOX3 = wxNewId();
const long DefaultFrame::ID_STATICTEXT3 = wxNewId();
const long DefaultFrame::ID_STATICTEXT5 = wxNewId();
const long DefaultFrame::ID_TEXTCTRL1 = wxNewId();
const long DefaultFrame::ID_BUTTON2 = wxNewId();
const long DefaultFrame::ID_BUTTON3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DefaultFrame,wxFrame)

    //(*EventTable(DefaultFrame)
    //*)
END_EVENT_TABLE()

DefaultFrame::DefaultFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(DefaultFrame)
    Create(parent, id, _("Communication avec la radio "), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxDEFAULT_FRAME_STYLE|wxSYSTEM_MENU|wxFULL_REPAINT_ON_RESIZE, _T("id"));
    SetClientSize(wxSize(473,335));
    Move(wxDefaultPosition);
    ComboBox1 = new wxComboBox(this, ID_COMBOBOX1, wxEmptyString, wxPoint(176,72), wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX1"));
    ComboBox1->Append(_("stk500v2"));
    ComboBox1->Append(_("stk500"));
    ComboBox1->Append(_("avrisp"));
    ComboBox1->Append(_("usbasp"));
    ComboBox2 = new wxComboBox(this, ID_COMBOBOX2, wxEmptyString, wxPoint(176,120), wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX2"));
    ComboBox2->Append(_("usb"));
    ComboBox2->Append(_("com1"));
    ComboBox2->Append(_("com2"));
    ComboBox2->Append(_("com3"));
    ComboBox2->Append(_("com4"));
    ComboBox2->Append(_("com5"));
    ComboBox2->Append(_("com6"));
    ComboBox2->Append(_("com7"));
    ComboBox2->Append(_("com8"));
    ComboBox2->Append(_("com9"));
    ComboBox2->Append(_("com10"));
    ComboBox2->Append(_("com11"));
    ComboBox2->Append(_("com12"));
    ComboBox2->Append(_("com13"));
    ComboBox2->Append(_("com14"));
    ComboBox2->Append(_("com15"));
    ComboBox2->Append(_("com16"));
    ComboBox2->Append(_("lpt1"));
    ComboBox2->Append(_("lpt2"));
    ComboBox2->Append(_("lpt3"));
    ComboBox2->Append(_("avrdoper"));
    ComboBox2->Append(_("/dev/ttyUSB0"));
    ComboBox2->Append(_("/dev/ttyUSB1"));
    ComboBox2->Append(_("/dev/ttyUSB2"));
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Programmeur"), wxPoint(72,80), wxSize(80,13), 0, _T("ID_STATICTEXT1"));
    Enter = new wxButton(this, ID_BUTTON1, _("Valider"), wxPoint(376,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Port"), wxPoint(112,128), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    ComboBox3 = new wxComboBox(this, ID_COMBOBOX3, wxEmptyString, wxPoint(176,168), wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX3"));
    ComboBox3->Append(_("m2560"));
    ComboBox3->Append(_("m128"));
    ComboBox3->Append(_("m64"));
    StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Type of radio"), wxPoint(72,176), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Directoire de avrdude"), wxPoint(32,224), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("avrdude path"), wxPoint(176,216), wxSize(212,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    Button1 = new wxButton(this, ID_BUTTON2, _("Annuler"), wxPoint(264,296), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    ButtonSearchavrdudepath = new wxButton(this, ID_BUTTON3, _("..."), wxPoint(400,216), wxSize(40,23), 0, wxDefaultValidator, _T("ID_BUTTON3"));

    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DefaultFrame::OnEnterClick);
    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&DefaultFrame::OnTextCtrl1Text);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DefaultFrame::OnButton1Click1);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DefaultFrame::OnButtonSearchavrdudepathClick);
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&DefaultFrame::OnClose);
    //*)


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

void DefaultFrame::TransfertConfig()
{
wxWindow *parent = this->GetParent();
    if(parent)
    {
        parent->SaveConfig();
    }
}

void DefaultFrame::OnEnterClick(wxCommandEvent& event)//Saves new default values and closes
{
    dude_programmer = ComboBox1->GetValue();
    dude_port = ComboBox2->GetValue();
    dude_type = ComboBox3->GetValue();
    avrdudepath = TextCtrl1->GetValue();
    TransfertConfig();
    Close();
}

void DefaultFrame::OnClose(wxCloseEvent& event)
{
    Close();
}

void DefaultFrame::OnTextCtrl1Text(wxCommandEvent& event)
{
    avrdudepath = TextCtrl1->GetValue();
}

void DefaultFrame::OnButton1Click1(wxCommandEvent& event)//Annuler
{
    Close();
}

void DefaultFrame::OnButtonSearchavrdudepathClick(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, _("Emplacement d'AVRDUDE"), "", "","AVRDUDE (*.bin)|*.exe", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) return;
    TextCtrl1->SetValue(openFileDialog.GetPath());
}

void DefaultFrame::OnPaint(wxPaintEvent& event)
{
}

