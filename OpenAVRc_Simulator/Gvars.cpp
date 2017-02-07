#include "Gvars.h"
#include "OpenAVRc_SimulatorMain.h"

//(*InternalHeaders(Gvars)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(Gvars)
const long Gvars::ID_STATICBOX3 = wxNewId();
const long Gvars::ID_STATICBOX2 = wxNewId();
const long Gvars::ID_GRID1 = wxNewId();
const long Gvars::ID_STATICTEXT4 = wxNewId();
const long Gvars::ID_STATICBOX1 = wxNewId();
const long Gvars::ID_PANEL8 = wxNewId();
const long Gvars::ID_PANEL21 = wxNewId();
const long Gvars::ID_PANEL20 = wxNewId();
const long Gvars::ID_PANEL19 = wxNewId();
const long Gvars::ID_PANEL18 = wxNewId();
const long Gvars::ID_PANEL17 = wxNewId();
const long Gvars::ID_PANEL16 = wxNewId();
const long Gvars::ID_PANEL15 = wxNewId();
const long Gvars::ID_PANEL14 = wxNewId();
const long Gvars::ID_PANEL13 = wxNewId();
const long Gvars::ID_PANEL12 = wxNewId();
const long Gvars::ID_PANEL11 = wxNewId();
const long Gvars::ID_PANEL10 = wxNewId();
const long Gvars::ID_PANEL9 = wxNewId();
const long Gvars::ID_PANEL2 = wxNewId();
const long Gvars::ID_PANEL4 = wxNewId();
const long Gvars::ID_PANEL7 = wxNewId();
const long Gvars::ID_PANEL6 = wxNewId();
const long Gvars::ID_PANEL5 = wxNewId();
const long Gvars::ID_PANEL3 = wxNewId();
const long Gvars::ID_SLIDER2 = wxNewId();
const long Gvars::ID_SLIDER5 = wxNewId();
const long Gvars::ID_SLIDER4 = wxNewId();
const long Gvars::ID_SLIDER3 = wxNewId();
const long Gvars::ID_SLIDER1 = wxNewId();
const long Gvars::ID_GAUGE2 = wxNewId();
const long Gvars::ID_GAUGE5 = wxNewId();
const long Gvars::ID_GAUGE4 = wxNewId();
const long Gvars::ID_GAUGE3 = wxNewId();
const long Gvars::ID_GAUGE1 = wxNewId();
const long Gvars::ID_STATICTEXT2 = wxNewId();
const long Gvars::ID_STATICTEXT5 = wxNewId();
const long Gvars::ID_STATICTEXT3 = wxNewId();
const long Gvars::ID_STATICTEXT1 = wxNewId();
const long Gvars::ID_STATICBOX4 = wxNewId();
const long Gvars::ID_TEXTCTRL1 = wxNewId();
const long Gvars::ID_STATICTEXT7 = wxNewId();
const long Gvars::ID_STATICTEXT8 = wxNewId();
const long Gvars::ID_STATICTEXT6 = wxNewId();
const long Gvars::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(Gvars,wxFrame)
	//(*EventTable(Gvars)
	//*)
END_EVENT_TABLE()

Gvars::Gvars(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(Gvars)
	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(615,398));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(0,48), wxSize(615,409), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	StaticBox3 = new wxStaticBox(Panel1, ID_STATICBOX3, _("Simulation Télémesure"), wxPoint(8,304), wxSize(600,96), 0, _T("ID_STATICBOX3"));
	StaticBox2 = new wxStaticBox(Panel1, ID_STATICBOX2, _("GVARS et Etats de vol"), wxPoint(8,72), wxSize(600,224), 0, _T("ID_STATICBOX2"));
	Grid1 = new wxGrid(Panel1, ID_GRID1, wxPoint(24,88), wxSize(568,184), 0, _T("ID_GRID1"));
	Grid1->CreateGrid(6,8);
	Grid1->EnableEditing(true);
	Grid1->EnableGridLines(true);
	Grid1->SetColLabelSize(60);
	Grid1->SetRowLabelSize(50);
	Grid1->SetDefaultColSize(80, true);
	Grid1->SetColLabelValue(0, _("Variables"));
	Grid1->SetColLabelValue(1, _("PV0"));
	Grid1->SetColLabelValue(2, _("PV1"));
	Grid1->SetColLabelValue(3, _("PV2"));
	Grid1->SetColLabelValue(4, _("PV3"));
	Grid1->SetColLabelValue(5, _("PV4"));
	Grid1->SetColLabelValue(6, _("PV5"));
	Grid1->SetColLabelValue(7, _("PV6"));
	Grid1->SetRowLabelValue(0, _("VG1"));
	Grid1->SetRowLabelValue(1, _("VG2"));
	Grid1->SetRowLabelValue(2, _("VG3"));
	Grid1->SetRowLabelValue(3, _("VG4"));
	Grid1->SetRowLabelValue(4, _("VG5"));
	Grid1->SetRowLabelValue(5, _("VG6"));
	Grid1->SetDefaultCellFont( Grid1->GetFont() );
	Grid1->SetDefaultCellTextColour( Grid1->GetForegroundColour() );
	StaticText4 = new wxStaticText(Panel1, ID_STATICTEXT4, _("Courant: 0-50A"), wxPoint(40,328), wxSize(75,16), 0, _T("ID_STATICTEXT4"));
	StaticBox1 = new wxStaticBox(Panel1, ID_STATICBOX1, _("Variables L1 à L15"), wxPoint(8,8), wxSize(488,56), 0, _T("ID_STATICBOX1"));
	Panel8 = new wxPanel(Panel1, ID_PANEL8, wxPoint(16,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL8"));
	Panel8->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel21 = new wxPanel(Panel1, ID_PANEL21, wxPoint(272,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL21"));
	Panel21->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel20 = new wxPanel(Panel1, ID_PANEL20, wxPoint(304,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL20"));
	Panel20->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel19 = new wxPanel(Panel1, ID_PANEL19, wxPoint(208,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL19"));
	Panel19->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel18 = new wxPanel(Panel1, ID_PANEL18, wxPoint(240,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL18"));
	Panel18->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel17 = new wxPanel(Panel1, ID_PANEL17, wxPoint(176,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL17"));
	Panel17->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel16 = new wxPanel(Panel1, ID_PANEL16, wxPoint(400,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL16"));
	Panel16->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel15 = new wxPanel(Panel1, ID_PANEL15, wxPoint(432,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL15"));
	Panel15->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel14 = new wxPanel(Panel1, ID_PANEL14, wxPoint(464,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL14"));
	Panel14->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel13 = new wxPanel(Panel1, ID_PANEL13, wxPoint(336,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL13"));
	Panel13->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel12 = new wxPanel(Panel1, ID_PANEL12, wxPoint(368,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL12"));
	Panel12->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel11 = new wxPanel(Panel1, ID_PANEL11, wxPoint(144,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL11"));
	Panel11->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel10 = new wxPanel(Panel1, ID_PANEL10, wxPoint(112,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL10"));
	Panel10->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel9 = new wxPanel(Panel1, ID_PANEL9, wxPoint(80,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL9"));
	Panel9->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel2 = new wxPanel(Panel1, ID_PANEL2, wxPoint(48,32), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL2"));
	Panel2->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel4 = new wxPanel(Panel1, ID_PANEL4, wxPoint(208,122), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL4"));
	Panel4->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel7 = new wxPanel(Panel1, ID_PANEL7, wxPoint(528,122), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL7"));
	Panel7->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel6 = new wxPanel(Panel1, ID_PANEL6, wxPoint(448,122), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL6"));
	Panel6->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel5 = new wxPanel(Panel1, ID_PANEL5, wxPoint(368,122), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL5"));
	Panel5->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Panel3 = new wxPanel(Panel1, ID_PANEL3, wxPoint(288,122), wxDefaultSize, wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL3"));
	Panel3->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRADIENTACTIVECAPTION));
	Slider2 = new wxSlider(Panel1, ID_SLIDER2, 0, 0, 100, wxPoint(24,344), wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER2"));
	Slider5 = new wxSlider(Panel1, ID_SLIDER5, 0, 0, 100, wxPoint(488,344), wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER5"));
	Slider4 = new wxSlider(Panel1, ID_SLIDER4, 0, 0, 100, wxPoint(376,344), wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER4"));
	Slider3 = new wxSlider(Panel1, ID_SLIDER3, 0, 0, 100, wxPoint(256,344), wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER3"));
	Slider1 = new wxSlider(Panel1, ID_SLIDER1, 0, 0, 100, wxPoint(136,344), wxDefaultSize, 0, wxDefaultValidator, _T("ID_SLIDER1"));
	Gauge2 = new wxGauge(Panel1, ID_GAUGE2, 100, wxPoint(32,368), wxSize(88,12), 0, wxDefaultValidator, _T("ID_GAUGE2"));
	Gauge2->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
	Gauge5 = new wxGauge(Panel1, ID_GAUGE5, 100, wxPoint(496,368), wxSize(88,12), 0, wxDefaultValidator, _T("ID_GAUGE5"));
	Gauge5->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
	Gauge4 = new wxGauge(Panel1, ID_GAUGE4, 100, wxPoint(384,368), wxSize(88,12), 0, wxDefaultValidator, _T("ID_GAUGE4"));
	Gauge4->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
	Gauge3 = new wxGauge(Panel1, ID_GAUGE3, 100, wxPoint(264,368), wxSize(88,12), 0, wxDefaultValidator, _T("ID_GAUGE3"));
	Gauge3->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
	Gauge1 = new wxGauge(Panel1, ID_GAUGE1, 100, wxPoint(144,368), wxSize(88,12), 0, wxDefaultValidator, _T("ID_GAUGE1"));
	Gauge1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
	StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("Altitude: 0-150m"), wxPoint(384,328), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	StaticText5 = new wxStaticText(Panel1, ID_STATICTEXT5, _("Réserve"), wxPoint(520,328), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("Cons.: 0-5000mAh"), wxPoint(144,328), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Tension Lipo: 0-20V"), wxPoint(264,328), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	StaticBox4 = new wxStaticBox(Panel1, ID_STATICBOX4, _("N° Voice"), wxPoint(504,8), wxSize(104,56), 0, _T("ID_STATICBOX4"));
	TextCtrl1 = new wxTextCtrl(Panel1, ID_TEXTCTRL1, _("Texte"), wxPoint(512,32), wxSize(88,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	StaticText7 = new wxStaticText(Panel1, ID_STATICTEXT7, _("5"), wxPoint(152,16), wxSize(8,13), 0, _T("ID_STATICTEXT7"));
	StaticText8 = new wxStaticText(Panel1, ID_STATICTEXT8, _("15"), wxPoint(464,16), wxSize(16,13), 0, _T("ID_STATICTEXT8"));
	StaticText6 = new wxStaticText(Panel1, ID_STATICTEXT6, _("10"), wxPoint(304,16), wxSize(16,13), 0, _T("ID_STATICTEXT6"));
	//*)
}

Gvars::~Gvars()
{
	//(*Destroy(Gvars)
	//*)
}


void Gvars::OnPanel4Paint(wxPaintEvent& event)
{
}
