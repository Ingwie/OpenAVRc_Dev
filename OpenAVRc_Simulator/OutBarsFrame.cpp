#include "OutBarsFrame.h"
#include "OpenAVRc_SimulatorMain.h"

//(*InternalHeaders(OutBarsFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(OutBarsFrame)
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
const long OutBarsFrame::ID_SLIDER1 = wxNewId();
const long OutBarsFrame::ID_SLIDER2 = wxNewId();
const long OutBarsFrame::ID_SLIDER3 = wxNewId();
const long OutBarsFrame::ID_SLIDER4 = wxNewId();
const long OutBarsFrame::ID_SLIDER5 = wxNewId();
const long OutBarsFrame::ID_SLIDER6 = wxNewId();
const long OutBarsFrame::ID_SLIDER7 = wxNewId();
const long OutBarsFrame::ID_SLIDER8 = wxNewId();
const long OutBarsFrame::ID_SLIDER9 = wxNewId();
const long OutBarsFrame::ID_SLIDER10 = wxNewId();
const long OutBarsFrame::ID_SLIDER11 = wxNewId();
const long OutBarsFrame::ID_SLIDER12 = wxNewId();
const long OutBarsFrame::ID_SLIDER13 = wxNewId();
const long OutBarsFrame::ID_SLIDER14 = wxNewId();
const long OutBarsFrame::ID_SLIDER15 = wxNewId();
const long OutBarsFrame::ID_SLIDER16 = wxNewId();
const long OutBarsFrame::ID_STATICLINE1 = wxNewId();
const long OutBarsFrame::ID_PANELOUTPUTSLIDEERS = wxNewId();
const long OutBarsFrame::ID_TIMERREFRESHOUTBAR = wxNewId();
//*)

BEGIN_EVENT_TABLE(OutBarsFrame,wxFrame)
	//(*EventTable(OutBarsFrame)
	//*)
END_EVENT_TABLE()

OutBarsFrame::OutBarsFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(OutBarsFrame)
	Create(parent, wxID_ANY, _("État des sorties."), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(270,296));
	Move(wxPoint(40,40));
	PanelOutputSliders = new wxPanel(this, ID_PANELOUTPUTSLIDEERS, wxDefaultPosition, wxSize(263,312), 0, _T("ID_PANELOUTPUTSLIDEERS"));
	output1 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL1, _("Texte"), wxPoint(216,10), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	output2 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL2, _("Texte"), wxPoint(216,26), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	output3 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL3, _("Texte"), wxPoint(216,42), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	output4 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL4, _("Texte"), wxPoint(216,58), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	output5 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL5, _("Texte"), wxPoint(216,74), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL5"));
	output6 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL6, _("Texte"), wxPoint(216,90), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL6"));
	output7 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL7, _("Texte"), wxPoint(216,106), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL7"));
	output8 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL8, _("Texte"), wxPoint(216,122), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL8"));
	output9 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL9, _("Texte"), wxPoint(216,138), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL9"));
	output10 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL10, _("Texte"), wxPoint(216,154), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL10"));
	output11 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL11, _("Texte"), wxPoint(216,170), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL11"));
	output12 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL12, _("Texte"), wxPoint(216,186), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL12"));
	output13 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL13, _("Texte"), wxPoint(216,202), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL13"));
	output14 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL14, _("Texte"), wxPoint(216,218), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL14"));
	output15 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL15, _("Texte"), wxPoint(216,234), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL15"));
	output16 = new wxTextCtrl(PanelOutputSliders, ID_TEXTCTRL16, _("Texte"), wxPoint(216,250), wxSize(56,16), wxTE_READONLY|wxTE_RICH|wxNO_BORDER|wxTRANSPARENT_WINDOW, wxDefaultValidator, _T("ID_TEXTCTRL16"));
	StaticText1 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT1, _("Ch1"), wxPoint(14,13), wxSize(32,16), 0, _T("ID_STATICTEXT1"));
	StaticText2 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT2, _("Ch2"), wxPoint(14,29), wxSize(32,16), 0, _T("ID_STATICTEXT2"));
	StaticText3 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT3, _("Ch3"), wxPoint(14,45), wxSize(32,16), 0, _T("ID_STATICTEXT3"));
	StaticText4 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT4, _("Ch4"), wxPoint(14,61), wxSize(32,16), 0, _T("ID_STATICTEXT4"));
	StaticText5 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT5, _("Ch5"), wxPoint(14,77), wxSize(32,16), 0, _T("ID_STATICTEXT5"));
	StaticText6 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT6, _("Ch6"), wxPoint(14,93), wxSize(32,16), 0, _T("ID_STATICTEXT6"));
	StaticText7 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT7, _("Ch7"), wxPoint(14,109), wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	StaticText8 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT8, _("Ch8"), wxPoint(14,125), wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	StaticText9 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT9, _("Ch9"), wxPoint(14,141), wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	StaticText10 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT10, _("Ch10"), wxPoint(14,157), wxDefaultSize, 0, _T("ID_STATICTEXT10"));
	StaticText11 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT11, _("Ch11"), wxPoint(14,173), wxDefaultSize, 0, _T("ID_STATICTEXT11"));
	StaticText12 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT12, _("Ch12"), wxPoint(14,189), wxDefaultSize, 0, _T("ID_STATICTEXT12"));
	StaticText13 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT13, _("Ch13"), wxPoint(14,205), wxDefaultSize, 0, _T("ID_STATICTEXT13"));
	StaticText14 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT14, _("Ch14"), wxPoint(14,221), wxDefaultSize, 0, _T("ID_STATICTEXT14"));
	StaticText15 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT15, _("Ch15"), wxPoint(14,237), wxDefaultSize, 0, _T("ID_STATICTEXT15"));
	StaticText16 = new wxStaticText(PanelOutputSliders, ID_STATICTEXT16, _("Ch16"), wxPoint(14,253), wxDefaultSize, 0, _T("ID_STATICTEXT16"));
	Slider1 = new wxSlider(PanelOutputSliders, ID_SLIDER1, 0, -115, 115, wxPoint(40,16), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER1"));
	Slider1->SetExtraStyle( Slider1->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider2 = new wxSlider(PanelOutputSliders, ID_SLIDER2, 0, -115, 115, wxPoint(40,32), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER2"));
	Slider2->SetExtraStyle( Slider2->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider3 = new wxSlider(PanelOutputSliders, ID_SLIDER3, 0, -115, 115, wxPoint(40,48), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER3"));
	Slider3->SetExtraStyle( Slider3->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider4 = new wxSlider(PanelOutputSliders, ID_SLIDER4, 0, -115, 115, wxPoint(40,64), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER4"));
	Slider4->SetExtraStyle( Slider4->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider5 = new wxSlider(PanelOutputSliders, ID_SLIDER5, 0, -115, 115, wxPoint(40,80), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER5"));
	Slider5->SetExtraStyle( Slider5->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider6 = new wxSlider(PanelOutputSliders, ID_SLIDER6, 0, -115, 115, wxPoint(40,96), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER6"));
	Slider6->SetExtraStyle( Slider6->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider7 = new wxSlider(PanelOutputSliders, ID_SLIDER7, 0, -115, 115, wxPoint(40,112), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER7"));
	Slider7->SetExtraStyle( Slider7->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider8 = new wxSlider(PanelOutputSliders, ID_SLIDER8, 0, -115, 115, wxPoint(40,128), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER8"));
	Slider8->SetExtraStyle( Slider8->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider9 = new wxSlider(PanelOutputSliders, ID_SLIDER9, 0, -115, 115, wxPoint(40,144), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER9"));
	Slider9->SetExtraStyle( Slider9->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider10 = new wxSlider(PanelOutputSliders, ID_SLIDER10, 0, -115, 115, wxPoint(40,160), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER10"));
	Slider10->SetExtraStyle( Slider10->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider11 = new wxSlider(PanelOutputSliders, ID_SLIDER11, 0, -115, 115, wxPoint(40,176), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER11"));
	Slider11->SetExtraStyle( Slider11->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider12 = new wxSlider(PanelOutputSliders, ID_SLIDER12, 0, -115, 115, wxPoint(40,192), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER12"));
	Slider12->SetExtraStyle( Slider12->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider13 = new wxSlider(PanelOutputSliders, ID_SLIDER13, 0, -115, 115, wxPoint(40,208), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER13"));
	Slider13->SetExtraStyle( Slider13->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider14 = new wxSlider(PanelOutputSliders, ID_SLIDER14, 0, -115, 115, wxPoint(40,224), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER14"));
	Slider14->SetExtraStyle( Slider14->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider15 = new wxSlider(PanelOutputSliders, ID_SLIDER15, 0, -115, 115, wxPoint(40,240), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER15"));
	Slider15->SetExtraStyle( Slider15->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	Slider16 = new wxSlider(PanelOutputSliders, ID_SLIDER16, 0, -115, 115, wxPoint(40,256), wxSize(176,16), 0, wxDefaultValidator, _T("ID_SLIDER16"));
	Slider16->SetExtraStyle( Slider16->GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
	StaticLine1 = new wxStaticLine(PanelOutputSliders, ID_STATICLINE1, wxPoint(127,8), wxSize(2,272), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
	TimerRefreshFrame.SetOwner(this, ID_TIMERREFRESHOUTBAR);
	TimerRefreshFrame.Start(200, false);

	Connect(ID_TIMERREFRESHOUTBAR,wxEVT_TIMER,(wxObjectEventFunction)&OutBarsFrame::OnTimerRefreshFrameTrigger);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&OutBarsFrame::OnClose);
	//*)

  {
    wxIcon FrameIcon;
    SetIcon(wxICON(nsrcs_icon));
  }

	PopulateBarFrame();
}

OutBarsFrame::~OutBarsFrame()
{
	//(*Destroy(OutBarsFrame)
	//*)
}

int16_t calcRESXto1000(int16_t x)  // return x/1.024
{
  // *1000/1024 = x - x/32 + x/128
  return (x - (x>>5) + (x>>7));
}

void OutBarsFrame::PopulateBarFrame()
{
  float valor = (calcRESXto1000(channelOutputs[0]));
  valor /= 10;
  wxString mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output1->SetValue(mystring);
	Slider1->SetValue(valor);

  valor = (calcRESXto1000(channelOutputs[1]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output2->SetValue(mystring);
	Slider2->SetValue(valor);

  valor = (calcRESXto1000(channelOutputs[2]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output3->SetValue(mystring);
	Slider3->SetValue(valor);

  valor = (calcRESXto1000(channelOutputs[3]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output4->SetValue(mystring);
	Slider4->SetValue(valor);

  valor = (calcRESXto1000(channelOutputs[4]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output5->SetValue(mystring);
	Slider5	->SetValue(valor);

  valor = (calcRESXto1000(channelOutputs[5]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output6->SetValue(mystring);
	Slider6->SetValue(valor);

  valor = (calcRESXto1000(channelOutputs[6]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output7->SetValue(mystring);
	Slider7->SetValue(valor);

  valor = (calcRESXto1000(channelOutputs[7]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output8->SetValue(mystring);
	Slider8->SetValue(valor);

  valor = (calcRESXto1000(channelOutputs[8]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output9->SetValue(mystring);
	Slider9->SetValue(valor);

  valor = (calcRESXto1000(channelOutputs[9]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output10->SetValue(mystring);
	Slider10->SetValue(valor);

  valor = (calcRESXto1000(channelOutputs[10]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output11->SetValue(mystring);
	Slider11->SetValue(valor);

  valor = (calcRESXto1000(channelOutputs[11]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output12->SetValue(mystring);
	Slider12->SetValue(valor);

  valor = (calcRESXto1000(channelOutputs[12]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output13->SetValue(mystring);
	Slider13->SetValue(valor);

  valor = (calcRESXto1000(channelOutputs[13]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output14->SetValue(mystring);
	Slider14->SetValue(valor);

  valor = (calcRESXto1000(channelOutputs[14]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output15->SetValue(mystring);
	Slider15->SetValue(valor);

	valor = (calcRESXto1000(channelOutputs[15]));
  valor /= 10;
  mystring = wxString::Format(wxT("%.1f"), valor) + "%";
  output16->SetValue(mystring);
	Slider16->SetValue(valor);
}

void OutBarsFrame::OnTimerRefreshFrameTrigger(wxTimerEvent& event)
{
  PopulateBarFrame();
}

void OutBarsFrame::OnClose(wxCloseEvent& event)
{
  OpenAVRc_SimulatorFrame *parent = wxDynamicCast(this->GetParent(), OpenAVRc_SimulatorFrame);
  if(parent)
    parent->EnableOutputBarsMenu();
  Destroy();
}
