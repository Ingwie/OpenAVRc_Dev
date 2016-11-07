#include "CompilerOptionsFrame.h"
#include "NextStepRc_DesktopMain.h"
#include "DefaultFrame.h"
#include <wx/choice.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>

extern bool Ini_Changed;
wxString PCB = _("non défini");
wxString LCD = _("non défini");
wxString VOICE = _("non défini");
wxString EXT = _("non défini");
wxString AUDIO = _("non défini");
wxString HELI = _("non défini");
wxString TTS = _("non défini");
wxString TRANSLATIONS = _("non défini");
wxString NAVIGATION = _("non défini");
wxString FRSKY_HUB = _("non défini");
wxString HAPTIC = _("non défini");
wxString PPM_UNIT = _("non défini");
wxString PXX = _("non défini");
wxString DSM2 = _("non défini");
wxString GAUGES = _("non défini");
wxString GPS = _("non défini");
wxString VARIO = _("non défini");
wxString RTCLOCK = _("non défini");
wxString SPORT_FILE_LOG = _("non défini");

//helper functions
//enum wxbuildinfoformat
//{
  //  short_f, long_f
//};


//(*InternalHeaders(CompilerOptionsFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)


//(*IdInit(CompilerOptionsFrame)
const long CompilerOptionsFrame::ID_STATICBOX1 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE1 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE3 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX9 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX1 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX7 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE2 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT1 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT3 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX2 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX3 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX4 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX5 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX6 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX8 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON1 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON2 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT4 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT5 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE4 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE5 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX12 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE6 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT2 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE7 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT6 = wxNewId();
const long CompilerOptionsFrame::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(CompilerOptionsFrame,wxFrame)
	//(*EventTable(CompilerOptionsFrame)
	//*)
END_EVENT_TABLE()

CompilerOptionsFrame::CompilerOptionsFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(CompilerOptionsFrame)
	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(807,313));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(0,0), wxSize(800,313), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	StaticBox1 = new wxStaticBox(Panel1, ID_STATICBOX1, _("Télémetrie"), wxPoint(280,16), wxSize(192,248), 0, _T("ID_STATICBOX1"));
	ChoiceLCD = new wxChoice(Panel1, ID_CHOICE1, wxPoint(120,72), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	ChoiceLCD->Append(_("DEFAULT "));
	ChoiceLCD->Append(_("ST7565P"));
	ChoiceLCD->Append(_("ST7565R "));
	ChoiceLCD->Append(_("ERC12864FSF"));
	ChoiceLCD->Append(_("ST7920 "));
	ChoiceLCD->Append(_("KS108"));
	ChoiceLCD->Append(_("SSD1306"));
	ChoiceVOICE = new wxChoice(Panel1, ID_CHOICE3, wxPoint(120,112), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
	ChoiceVOICE->Append(_("NO"));
	ChoiceVOICE->Append(_("WTV20SD"));
	ChoiceVOICE->Append(_("JQ6500"));
	CheckBox9 = new wxCheckBox(Panel1, ID_CHECKBOX9, _("RTC Clock"), wxPoint(120,184), wxSize(67,21), 0, wxDefaultValidator, _T("ID_CHECKBOX9"));
	CheckBox9->SetValue(false);
	CheckBox1 = new wxCheckBox(Panel1, ID_CHECKBOX1, _("HELI"), wxPoint(120,248), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBox1->SetValue(false);
	CheckBox7 = new wxCheckBox(Panel1, ID_CHECKBOX7, _("AUDIO"), wxPoint(120,152), wxSize(64,16), 0, wxDefaultValidator, _T("ID_CHECKBOX7"));
	CheckBox7->SetValue(false);
	ChoiceEXT = new wxChoice(Panel1, ID_CHOICE2, wxPoint(352,32), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
	ChoiceEXT->Append(_("STD"));
	ChoiceEXT->Append(_("FRSKY"));
	ChoiceEXT->Append(_("FRSKY_SPORT"));
	ChoiceEXT->Append(_("JETI"));
	ChoiceEXT->Append(_("NMEA"));
	ChoiceEXT->Append(_("ARDUPILOT"));
	ChoiceEXT->Append(_("MAVLINK"));
	ChoiceEXT->Append(_("TELEMETREZ"));
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("LCD"), wxPoint(80,72), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("VOICE"), wxPoint(64,120), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	CheckBox2 = new wxCheckBox(Panel1, ID_CHECKBOX2, _("FrSky Hub"), wxPoint(352,72), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	CheckBox2->SetValue(false);
	CheckBox3 = new wxCheckBox(Panel1, ID_CHECKBOX3, _("FAS Offset"), wxPoint(352,104), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
	CheckBox3->SetValue(false);
	CheckBox4 = new wxCheckBox(Panel1, ID_CHECKBOX4, _("Gauges"), wxPoint(352,136), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
	CheckBox4->SetValue(false);
	CheckBox5 = new wxCheckBox(Panel1, ID_CHECKBOX5, _("GPS"), wxPoint(352,168), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
	CheckBox5->SetValue(false);
	CheckBox6 = new wxCheckBox(Panel1, ID_CHECKBOX6, _("Vario"), wxPoint(352,200), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX6"));
	CheckBox6->SetValue(false);
	CheckBox8 = new wxCheckBox(Panel1, ID_CHECKBOX8, _("Sport file log"), wxPoint(352,232), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX8"));
	CheckBox8->SetValue(false);
	Button1 = new wxButton(Panel1, ID_BUTTON1, _("Advanced"), wxPoint(584,256), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	Button2 = new wxButton(Panel1, ID_BUTTON2, _("COMPILE"), wxPoint(688,256), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	StaticText4 = new wxStaticText(Panel1, ID_STATICTEXT4, _("PPM Units"), wxPoint(552,40), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	StaticText5 = new wxStaticText(Panel1, ID_STATICTEXT5, _("PROTOCOL"), wxPoint(544,88), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	ChoicePPM_UNIT  = new wxChoice(Panel1, ID_CHOICE4, wxPoint(624,32), wxSize(112,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE4"));
	ChoicePPM_UNIT ->Append(_("PERCENT_PREC1"));
	ChoicePPM_UNIT ->Append(_("PERCENT_PREC0"));
	ChoicePPM_UNIT ->Append(_("US"));
	Choice5 = new wxChoice(Panel1, ID_CHOICE5, wxPoint(624,80), wxSize(112,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE5"));
	Choice5->Append(_("PPM"));
	Choice5->Append(_("PXX"));
	Choice5->Append(_("DSM2"));
	CheckBox12 = new wxCheckBox(Panel1, ID_CHECKBOX12, _("SD CARD"), wxPoint(120,216), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX12"));
	CheckBox12->SetValue(false);
	ChoicePCB = new wxChoice(Panel1, ID_CHOICE6, wxPoint(120,32), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE6"));
	ChoicePCB->Append(_("m2560"));
	ChoicePCB->Append(_("m128"));
	ChoicePCB->Append(_("m64"));
	StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("PROCESSOR"), wxPoint(40,40), wxSize(64,13), 0, _T("ID_STATICTEXT2"));
	Choice7 = new wxChoice(Panel1, ID_CHOICE7, wxPoint(624,136), wxSize(112,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE7"));
	StaticText6 = new wxStaticText(Panel1, ID_STATICTEXT6, _("LANGUAGE"), wxPoint(552,144), wxDefaultSize, 0, _T("ID_STATICTEXT6"));

	Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CompilerOptionsFrame::OnLCDSelect);
	Connect(ID_CHOICE3,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CompilerOptionsFrame::OnVOICESelect1);
	Connect(ID_CHECKBOX9,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBox9Click);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBox1Click);
	Connect(ID_CHOICE2,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CompilerOptionsFrame::OnChoiceEXTSelect);
	Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBox2Click);
	Connect(ID_CHECKBOX3,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBox3Click);
	Connect(ID_CHECKBOX4,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBox4Click);
	Connect(ID_CHECKBOX5,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBox5Click);
	Connect(ID_CHECKBOX6,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBox6Click);
	Connect(ID_CHECKBOX8,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBox8Click);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButton1Click);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButton2Click);
	Connect(ID_CHOICE4,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CompilerOptionsFrame::OnChoicePPM_UNITSelect);
	Connect(ID_CHOICE5,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CompilerOptionsFrame::OnChoice5Select);
	Connect(ID_CHECKBOX12,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnCheckBox12Click);
	Connect(ID_CHOICE6,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CompilerOptionsFrame::OnChoicePCBSelect);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&CompilerOptionsFrame::OnClose);
	//*)
	ChoicePCB->SetStringSelection(PCB);
	ChoiceLCD->SetStringSelection(LCD);
	ChoiceVOICE->SetStringSelection(VOICE);
	ChoiceEXT->SetStringSelection(EXT);
	ChoicePPM_UNIT->SetStringSelection(PPM_UNIT);
}

CompilerOptionsFrame::~CompilerOptionsFrame()
{
	//(*Destroy(CompilerOptionsFrame)
	//*)
}

void CompilerOptionsFrame::OnChoicePCBSelect(wxCommandEvent& event)
{
    PCB  = ChoicePCB->GetString(ChoicePCB->GetSelection());
}

void CompilerOptionsFrame::OnLCDSelect(wxCommandEvent& event)
{
    LCD  = ChoiceLCD->GetString(ChoiceLCD->GetSelection());
}

void CompilerOptionsFrame::OnVOICESelect1(wxCommandEvent& event)
{
    VOICE  = ChoiceVOICE->GetString(ChoiceVOICE->GetSelection());
}

void CompilerOptionsFrame::OnChoiceEXTSelect(wxCommandEvent& event)
{
    EXT  = ChoiceEXT->GetString(ChoiceEXT->GetSelection());
}

void CompilerOptionsFrame::OnChoicePPM_UNITSelect(wxCommandEvent& event)
{
    PPM_UNIT  = ChoicePPM_UNIT->GetString(ChoicePPM_UNIT->GetSelection());
}

void CompilerOptionsFrame::OnClose(wxCloseEvent& event)
{
  Destroy();
}

void CompilerOptionsFrame::OnCheckBox1Click(wxCommandEvent& event)
{
}

void CompilerOptionsFrame::OnCheckBox2Click(wxCommandEvent& event)
{
}

void CompilerOptionsFrame::OnCheckBox3Click(wxCommandEvent& event)
{
}

void CompilerOptionsFrame::OnCheckBox11Click(wxCommandEvent& event)
{
}

void CompilerOptionsFrame::OnCheckBox10Click(wxCommandEvent& event)
{
}

void CompilerOptionsFrame::OnChoice5Select(wxCommandEvent& event)
{
}



void CompilerOptionsFrame::OnButton1Click(wxCommandEvent& event)
{
}

void CompilerOptionsFrame::OnCheckBox5Click(wxCommandEvent& event)
{
}

void CompilerOptionsFrame::OnCheckBox8Click(wxCommandEvent& event)
{
}

void CompilerOptionsFrame::OnCheckBox4Click(wxCommandEvent& event)
{
}

void CompilerOptionsFrame::OnCheckBox6Click(wxCommandEvent& event)
{
}

void CompilerOptionsFrame::OnCheckBox9Click(wxCommandEvent& event)
{
}

void CompilerOptionsFrame::OnCheckBox12Click(wxCommandEvent& event)
{
}

void CompilerOptionsFrame::OnButton2Click(wxCommandEvent& event)
{
   //configFile->Write(wxT("LCD"),LCD);// how to make configFile visible : The owner is the main frame ;-)
   //SaveConfig();
   Ini_Changed = true;
}

