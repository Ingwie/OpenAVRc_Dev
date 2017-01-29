#include "OutBarsFrame.h"
#include "OpenAVRc_SimulatorMain.h"

//(*InternalHeaders(OutBarsFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(OutBarsFrame)
const long OutBarsFrame::ID_GAUGE1 = wxNewId();
const long OutBarsFrame::ID_GAUGE2 = wxNewId();
const long OutBarsFrame::ID_GAUGE3 = wxNewId();
const long OutBarsFrame::ID_GAUGE4 = wxNewId();
const long OutBarsFrame::ID_GAUGE5 = wxNewId();
const long OutBarsFrame::ID_GAUGE6 = wxNewId();
const long OutBarsFrame::ID_GAUGE7 = wxNewId();
const long OutBarsFrame::ID_GAUGE8 = wxNewId();
const long OutBarsFrame::ID_GAUGE9 = wxNewId();
const long OutBarsFrame::ID_GAUGE10 = wxNewId();
const long OutBarsFrame::ID_GAUGE11 = wxNewId();
const long OutBarsFrame::ID_GAUGE12 = wxNewId();
const long OutBarsFrame::ID_GAUGE13 = wxNewId();
const long OutBarsFrame::ID_GAUGE14 = wxNewId();
const long OutBarsFrame::ID_GAUGE15 = wxNewId();
const long OutBarsFrame::ID_GAUGE16 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL1 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL2 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL3 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL4 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL5 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL6 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL7 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL8 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL9 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL10 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL11 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL12 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL13 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL14 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL15 = wxNewId();
const long OutBarsFrame::ID_TEXTCTRL16 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT1 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT2 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT3 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT4 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT5 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT6 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT7 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT8 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT9 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT10 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT11 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT12 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT13 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT14 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT15 = wxNewId();
const long OutBarsFrame::ID_STATICTEXT16 = wxNewId();
const long OutBarsFrame::ID_PANEL1 = wxNewId();
const long OutBarsFrame::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(OutBarsFrame,wxFrame)
	//(*EventTable(OutBarsFrame)
	//*)
END_EVENT_TABLE()

OutBarsFrame::OutBarsFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(OutBarsFrame)
	Create(parent, wxID_ANY, _("État des sorties du mixeur."), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(263,284));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(64,32), wxSize(400,368), 0, _T("ID_PANEL1"));
	Gauge1 = new wxGauge(Panel1, ID_GAUGE1, 2356, wxPoint(48,16), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE1"));
	Gauge2 = new wxGauge(Panel1, ID_GAUGE2, 2356, wxPoint(48,32), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE2"));
	Gauge3 = new wxGauge(Panel1, ID_GAUGE3, 2356, wxPoint(48,48), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE3"));
	Gauge4 = new wxGauge(Panel1, ID_GAUGE4, 2356, wxPoint(48,64), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE4"));
	Gauge5 = new wxGauge(Panel1, ID_GAUGE5, 2356, wxPoint(48,80), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE5"));
	Gauge6 = new wxGauge(Panel1, ID_GAUGE6, 2356, wxPoint(48,96), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE6"));
	Gauge7 = new wxGauge(Panel1, ID_GAUGE7, 2356, wxPoint(48,112), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE7"));
	Gauge8 = new wxGauge(Panel1, ID_GAUGE8, 2356, wxPoint(48,128), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE8"));
	Gauge9 = new wxGauge(Panel1, ID_GAUGE9, 2356, wxPoint(48,144), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE9"));
	Gauge10 = new wxGauge(Panel1, ID_GAUGE10, 2356, wxPoint(48,160), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE10"));
	Gauge11 = new wxGauge(Panel1, ID_GAUGE11, 2356, wxPoint(48,176), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE11"));
	Gauge12 = new wxGauge(Panel1, ID_GAUGE12, 2356, wxPoint(48,192), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE12"));
	Gauge13 = new wxGauge(Panel1, ID_GAUGE13, 2356, wxPoint(48,208), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE13"));
	Gauge14 = new wxGauge(Panel1, ID_GAUGE14, 2356, wxPoint(48,224), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE14"));
	Gauge15 = new wxGauge(Panel1, ID_GAUGE15, 2356, wxPoint(48,240), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE15"));
	Gauge16 = new wxGauge(Panel1, ID_GAUGE16, 2356, wxPoint(48,256), wxSize(160,8), 0, wxDefaultValidator, _T("ID_GAUGE16"));
	output1 = new wxTextCtrl(Panel1, ID_TEXTCTRL1, _("Texte"), wxPoint(216,10), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	output2 = new wxTextCtrl(Panel1, ID_TEXTCTRL2, _("Texte"), wxPoint(216,26), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	output3 = new wxTextCtrl(Panel1, ID_TEXTCTRL3, _("Texte"), wxPoint(216,42), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	output4 = new wxTextCtrl(Panel1, ID_TEXTCTRL4, _("Texte"), wxPoint(216,58), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	output5 = new wxTextCtrl(Panel1, ID_TEXTCTRL5, _("Texte"), wxPoint(216,74), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL5"));
	output6 = new wxTextCtrl(Panel1, ID_TEXTCTRL6, _("Texte"), wxPoint(216,90), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL6"));
	output7 = new wxTextCtrl(Panel1, ID_TEXTCTRL7, _("Texte"), wxPoint(216,106), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL7"));
	output8 = new wxTextCtrl(Panel1, ID_TEXTCTRL8, _("Texte"), wxPoint(216,122), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL8"));
	output9 = new wxTextCtrl(Panel1, ID_TEXTCTRL9, _("Texte"), wxPoint(216,138), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL9"));
	output10 = new wxTextCtrl(Panel1, ID_TEXTCTRL10, _("Texte"), wxPoint(216,154), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL10"));
	output11 = new wxTextCtrl(Panel1, ID_TEXTCTRL11, _("Texte"), wxPoint(216,170), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL11"));
	output12 = new wxTextCtrl(Panel1, ID_TEXTCTRL12, _("Texte"), wxPoint(216,186), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL12"));
	output13 = new wxTextCtrl(Panel1, ID_TEXTCTRL13, _("Texte"), wxPoint(216,202), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL13"));
	output14 = new wxTextCtrl(Panel1, ID_TEXTCTRL14, _("Texte"), wxPoint(216,218), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL14"));
	output15 = new wxTextCtrl(Panel1, ID_TEXTCTRL15, _("Texte"), wxPoint(216,234), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL15"));
	output16 = new wxTextCtrl(Panel1, ID_TEXTCTRL16, _("Texte"), wxPoint(216,250), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL16"));
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("CH1"), wxPoint(16,13), wxSize(32,16), 0, _T("ID_STATICTEXT1"));
	StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("CH2"), wxPoint(16,29), wxSize(32,16), 0, _T("ID_STATICTEXT2"));
	StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("CH3"), wxPoint(16,45), wxSize(32,16), 0, _T("ID_STATICTEXT3"));
	StaticText4 = new wxStaticText(Panel1, ID_STATICTEXT4, _("CH4"), wxPoint(16,61), wxSize(32,16), 0, _T("ID_STATICTEXT4"));
	StaticText5 = new wxStaticText(Panel1, ID_STATICTEXT5, _("CH5"), wxPoint(16,77), wxSize(32,16), 0, _T("ID_STATICTEXT5"));
	StaticText6 = new wxStaticText(Panel1, ID_STATICTEXT6, _("CH6"), wxPoint(16,93), wxSize(32,16), 0, _T("ID_STATICTEXT6"));
	StaticText7 = new wxStaticText(Panel1, ID_STATICTEXT7, _("CH7"), wxPoint(16,109), wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	StaticText8 = new wxStaticText(Panel1, ID_STATICTEXT8, _("CH8"), wxPoint(16,125), wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	StaticText9 = new wxStaticText(Panel1, ID_STATICTEXT9, _("CH9"), wxPoint(16,141), wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	StaticText10 = new wxStaticText(Panel1, ID_STATICTEXT10, _("CH10"), wxPoint(16,157), wxDefaultSize, 0, _T("ID_STATICTEXT10"));
	StaticText11 = new wxStaticText(Panel1, ID_STATICTEXT11, _("CH11"), wxPoint(16,173), wxDefaultSize, 0, _T("ID_STATICTEXT11"));
	StaticText12 = new wxStaticText(Panel1, ID_STATICTEXT12, _("CH12"), wxPoint(16,189), wxDefaultSize, 0, _T("ID_STATICTEXT12"));
	StaticText13 = new wxStaticText(Panel1, ID_STATICTEXT13, _("CH13"), wxPoint(16,205), wxDefaultSize, 0, _T("ID_STATICTEXT13"));
	StaticText14 = new wxStaticText(Panel1, ID_STATICTEXT14, _("CH14"), wxPoint(16,221), wxDefaultSize, 0, _T("ID_STATICTEXT14"));
	StaticText15 = new wxStaticText(Panel1, ID_STATICTEXT15, _("CH15"), wxPoint(16,237), wxDefaultSize, 0, _T("ID_STATICTEXT15"));
	StaticText16 = new wxStaticText(Panel1, ID_STATICTEXT16, _("CH16"), wxPoint(16,253), wxDefaultSize, 0, _T("ID_STATICTEXT16"));
	TimerRefreshFrame.SetOwner(this, ID_TIMER1);
	TimerRefreshFrame.Start(500, false);

	Connect(ID_TEXTCTRL9,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&OutBarsFrame::OnTextCtrl1Text1);
	Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&OutBarsFrame::OnTimerRefreshFrameTrigger);
	//*)

	FillBarFrame();
}

OutBarsFrame::~OutBarsFrame()
{
	//(*Destroy(OutBarsFrame)
	//*)
}

void OutBarsFrame::FillBarFrame()
{
  wxString mystring = wxString::Format(wxT("%i"),channelOutputs[0] * 100/1024) + "%";
	output1->SetValue(mystring);
	Gauge1->SetValue(channelOutputs[0] + 2356/2);

  mystring = wxString::Format(wxT("%i"),channelOutputs[1] * 100/1024) + "%";
	output2->SetValue(mystring);
	Gauge2->SetValue(channelOutputs[1] + 2356/2);

  mystring = wxString::Format(wxT("%i"),channelOutputs[2] * 100/1024) + "%";
	output3->SetValue(mystring);
	Gauge3->SetValue(channelOutputs[2] + 2356/2);

  mystring = wxString::Format(wxT("%i"),channelOutputs[3] * 100/1024) + "%";
	output4->SetValue(mystring);
	Gauge4->SetValue(channelOutputs[3] + 2356/2);

  mystring = wxString::Format(wxT("%i"),channelOutputs[4] * 100/1024) + "%";
	output5->SetValue(mystring);
	Gauge5->SetValue(channelOutputs[4] + 2356/2);

  mystring = wxString::Format(wxT("%i"),channelOutputs[5] * 100/1024) + "%";
	output6->SetValue(mystring);
	Gauge6->SetValue(channelOutputs[5] + 2356/2);

  mystring = wxString::Format(wxT("%i"),channelOutputs[6] * 100/1024) + "%";
	output7->SetValue(mystring);
	Gauge7->SetValue(channelOutputs[6] + 2356/2);

  mystring = wxString::Format(wxT("%i"),channelOutputs[7] * 100/1024) + "%";
	output8->SetValue(mystring);
	Gauge8->SetValue(channelOutputs[7] + 2356/2);

	mystring = wxString::Format(wxT("%i"),channelOutputs[8] * 100/1024) + "%";
	output9->SetValue(mystring);
	Gauge9->SetValue(channelOutputs[8] + 2356/2);

  mystring = wxString::Format(wxT("%i"),channelOutputs[9] * 100/1024) + "%";
	output10->SetValue(mystring);
	Gauge10->SetValue(channelOutputs[9] + 2356/2);

  mystring = wxString::Format(wxT("%i"),channelOutputs[10] * 100/1024) + "%";
	output11->SetValue(mystring);
	Gauge11->SetValue(channelOutputs[10] + 2356/2);

  mystring = wxString::Format(wxT("%i"),channelOutputs[11] * 100/1024) + "%";
	output12->SetValue(mystring);
	Gauge12->SetValue(channelOutputs[11] + 2356/2);

  mystring = wxString::Format(wxT("%i"),channelOutputs[12] * 100/1024) + "%";
	output13->SetValue(mystring);
	Gauge13->SetValue(channelOutputs[12] + 2356/2);

  mystring = wxString::Format(wxT("%i"),channelOutputs[13] * 100/1024) + "%";
	output14->SetValue(mystring);
	Gauge14->SetValue(channelOutputs[13] + 2356/2);

  mystring = wxString::Format(wxT("%i"),channelOutputs[14] * 100/1024) + "%";
	output15->SetValue(mystring);
	Gauge15->SetValue(channelOutputs[14] + 2356/2);

  mystring = wxString::Format(wxT("%i"),channelOutputs[15] * 100/1024) + "%";
	output16->SetValue(mystring);
	Gauge16->SetValue(channelOutputs[15] + 2356/2);

}


void OutBarsFrame::OnTextCtrl1Text1(wxCommandEvent& event)
{
}

void OutBarsFrame::OnTimerRefreshFrameTrigger(wxTimerEvent& event)
{
  FillBarFrame();
}

