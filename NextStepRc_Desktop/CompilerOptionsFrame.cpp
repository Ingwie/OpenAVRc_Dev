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
bool AUDIO = 0;
bool HELI = 0;
wxString TTS = _("non défini");
wxString TRANSLATIONS = _("non défini");
wxString NAVIGATION = _("non défini");
wxString FRSKY_HUB = _("non défini");
wxString HAPTIC = _("non défini");
wxString PPM_UNIT = _("non défini");
bool GAUGES = 0;
bool GPS = 0;
bool VARIO = 0;
bool RTCLOCK = 0;
bool SPORT_FILE_LOG = 0;
bool PPM = 1;
bool PXX = 0;
bool DSM2 = 0;


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
const long CompilerOptionsFrame::ID_STATICBOX2 = wxNewId();
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
const long CompilerOptionsFrame::ID_CHOICE4 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX12 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE6 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT2 = wxNewId();
const long CompilerOptionsFrame::ID_CHOICE7 = wxNewId();
const long CompilerOptionsFrame::ID_STATICTEXT6 = wxNewId();
const long CompilerOptionsFrame::ID_BUTTON3 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX10 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX11 = wxNewId();
const long CompilerOptionsFrame::ID_CHECKBOX13 = wxNewId();
const long CompilerOptionsFrame::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(CompilerOptionsFrame,wxFrame)
	//(*EventTable(CompilerOptionsFrame)
	//*)
END_EVENT_TABLE()

CompilerOptionsFrame::CompilerOptionsFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(CompilerOptionsFrame)
	Create(parent, wxID_ANY, _("Compil-O-matic"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(807,313));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(0,0), wxSize(800,313), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	StaticBoxPROTOCOL = new wxStaticBox(Panel1, ID_STATICBOX2, _("Protocol"), wxPoint(552,16), wxSize(184,88), 0, _T("ID_STATICBOX2"));
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
	CheckBoxRTCLOCK = new wxCheckBox(Panel1, ID_CHECKBOX9, _("RTC Clock"), wxPoint(120,184), wxSize(80,21), 0, wxDefaultValidator, _T("ID_CHECKBOX9"));
	CheckBoxRTCLOCK->SetValue(false);
	CheckBoxHELI = new wxCheckBox(Panel1, ID_CHECKBOX1, _("HELI"), wxPoint(120,248), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBoxHELI->SetValue(false);
	CheckBoxAUDIO = new wxCheckBox(Panel1, ID_CHECKBOX7, _("AUDIO"), wxPoint(120,152), wxSize(64,16), 0, wxDefaultValidator, _T("ID_CHECKBOX7"));
	CheckBoxAUDIO->SetValue(false);
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
	CheckBoxHUB = new wxCheckBox(Panel1, ID_CHECKBOX2, _("FrSky Hub"), wxPoint(352,72), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	CheckBoxHUB->SetValue(false);
	CheckBoxFASOFFSET = new wxCheckBox(Panel1, ID_CHECKBOX3, _("FAS Offset"), wxPoint(352,104), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
	CheckBoxFASOFFSET->SetValue(false);
	CheckBoxGAUGES = new wxCheckBox(Panel1, ID_CHECKBOX4, _("Gauges"), wxPoint(352,136), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
	CheckBoxGAUGES->SetValue(false);
	CheckBoxGPS = new wxCheckBox(Panel1, ID_CHECKBOX5, _("GPS"), wxPoint(352,168), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
	CheckBoxGPS->SetValue(false);
	CheckBoxVARIO = new wxCheckBox(Panel1, ID_CHECKBOX6, _("Vario"), wxPoint(352,200), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX6"));
	CheckBoxVARIO->SetValue(false);
	CheckBoxSPORT_FILE_LOG = new wxCheckBox(Panel1, ID_CHECKBOX8, _("Sport file log"), wxPoint(352,232), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX8"));
	CheckBoxSPORT_FILE_LOG->SetValue(false);
	ButtonADVANCED = new wxButton(Panel1, ID_BUTTON1, _("Advanced"), wxPoint(688,216), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	ButtonCOMPILE = new wxButton(Panel1, ID_BUTTON2, _("COMPILE"), wxPoint(688,256), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	StaticText4 = new wxStaticText(Panel1, ID_STATICTEXT4, _("PPM Units"), wxPoint(560,128), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	ChoicePPM_UNIT  = new wxChoice(Panel1, ID_CHOICE4, wxPoint(624,120), wxSize(112,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE4"));
	ChoicePPM_UNIT ->Append(_("PERCENT_PREC1"));
	ChoicePPM_UNIT ->Append(_("PERCENT_PREC0"));
	ChoicePPM_UNIT ->Append(_("US"));
	CheckBoxSD_CARD = new wxCheckBox(Panel1, ID_CHECKBOX12, _("SD CARD"), wxPoint(120,216), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX12"));
	CheckBoxSD_CARD->SetValue(false);
	ChoicePCB = new wxChoice(Panel1, ID_CHOICE6, wxPoint(120,32), wxSize(96,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE6"));
	ChoicePCB->Append(_("m2560"));
	ChoicePCB->Append(_("m128"));
	ChoicePCB->Append(_("m64"));
	StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("PROCESSOR"), wxPoint(40,40), wxSize(64,13), 0, _T("ID_STATICTEXT2"));
	Choice7 = new wxChoice(Panel1, ID_CHOICE7, wxPoint(624,160), wxSize(112,21), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE7"));
	StaticText6 = new wxStaticText(Panel1, ID_STATICTEXT6, _("LANGUAGE"), wxPoint(552,160), wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	ButtonEXIT = new wxButton(Panel1, ID_BUTTON3, _("EXIT"), wxPoint(576,256), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	CheckBoxPPM = new wxCheckBox(Panel1, ID_CHECKBOX10, _("PPM"), wxPoint(624,32), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX10"));
	CheckBoxPPM->SetValue(true);
	CheckBoxPXX = new wxCheckBox(Panel1, ID_CHECKBOX11, _("PXX"), wxPoint(624,56), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX11"));
	CheckBoxPXX->SetValue(false);
	CheckBoxDSM2 = new wxCheckBox(Panel1, ID_CHECKBOX13, _("DSM2"), wxPoint(624,80), wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX13"));
	CheckBoxDSM2->SetValue(false);

	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonCOMPILEClick);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CompilerOptionsFrame::OnButtonEXITClick);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&CompilerOptionsFrame::OnClose);
	//*)

  CheckBoxRTCLOCK->SetValue(RTCLOCK);
  CheckBoxPPM->SetValue(PPM);
  CheckBoxPXX->SetValue(PXX);
  CheckBoxDSM2->SetValue(DSM2);
  CheckBoxAUDIO->SetValue(AUDIO);
	ChoicePCB->SetStringSelection(PCB);
	ChoiceLCD->SetStringSelection(LCD);
	ChoiceVOICE->SetStringSelection(VOICE);
	ChoiceEXT->SetStringSelection(EXT);
	ChoicePPM_UNIT->SetStringSelection(PPM_UNIT);
  CheckBoxHELI->SetValue(HELI);
  CheckBoxGAUGES->SetValue(GAUGES);
  CheckBoxGPS->SetValue(GPS);
  CheckBoxVARIO->SetValue(VARIO);
  CheckBoxSPORT_FILE_LOG->SetValue(SPORT_FILE_LOG);
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

void CompilerOptionsFrame::OnButtonCOMPILEClick(wxCommandEvent& event)
{
  PCB  = ChoicePCB->GetString(ChoicePCB->GetSelection());
  LCD  = ChoiceLCD->GetString(ChoiceLCD->GetSelection());
  VOICE  = ChoiceVOICE->GetString(ChoiceVOICE->GetSelection());
  EXT  = ChoiceEXT->GetString(ChoiceEXT->GetSelection());
  PPM_UNIT  = ChoicePPM_UNIT->GetString(ChoicePPM_UNIT->GetSelection());
  AUDIO = CheckBoxAUDIO->GetValue();
  RTCLOCK = CheckBoxRTCLOCK->GetValue();
  HELI = CheckBoxHELI->GetValue();
  GAUGES = CheckBoxGAUGES->GetValue();
  GPS = CheckBoxGPS->GetValue();
  VARIO = CheckBoxVARIO->GetValue();
  SPORT_FILE_LOG = CheckBoxSPORT_FILE_LOG->GetValue();
  PPM = CheckBoxPPM->GetValue();
  PXX = CheckBoxPXX->GetValue();
  DSM2 = CheckBoxDSM2->GetValue();

   //configFile->Write(wxT("LCD"),LCD);// how to make configFile visible : The owner is the main frame ;-)
   //SaveConfig();
   Ini_Changed = true;

    // makeclean before
    wxString CompiBat = "makefile";
    CompiBat += (" LCD=" + LCD);
    CompiBat += (" PCB=" + PCB);
    CompiBat += (" VOICE=" + VOICE);
    CompiBat += (" EXT=" + EXT);
    CompiBat += (" PPM_UNIT=" + PPM_UNIT);
    if (AUDIO) CompiBat += (" AUDIO=YES");// defauls should ve NO
    if (RTCLOCK) CompiBat += (" RTCLOCK=YES");
    if (HELI) CompiBat += (" HELI=YES");
    if (GAUGES) CompiBat += (" GAUGES=YES");
    if (GPS) CompiBat += (" GPS=YES");
    if (VARIO) CompiBat += (" VARIO=YES");
    if (SPORT_FILE_LOG) CompiBat += (" SPORT_FILE_LOG=YES");
    if (PPM) CompiBat += (" PPM=YES");
    if (PXX) CompiBat += (" PXX=YES");
    if (DSM2) CompiBat += (" DSM2=YES");

    wxMessageBox(CompiBat);
}

void CompilerOptionsFrame::OnButtonEXITClick(wxCommandEvent& event)
{
    Close();
}


void Compile()//no use yet
{

    //wxString CompiBat = "makefile ";
    //BatString = "";
    //CompiBat += ("LCD=" + LCD);
    //BatString  += toto.getvalue();
    //BatString += configFile->Read(wxT("LCD"),&LCD); config file was not declared in this scope.
    //BatString  += tata.getvalue();
    //If (audioIsChecked) batString += "AUDIO=YES ";
}
