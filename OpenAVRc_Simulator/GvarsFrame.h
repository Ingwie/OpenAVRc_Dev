#ifndef GVARS_H
#define GVARS_H

//(*Headers(GvarsFrame)
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/frame.h>
#include <wx/gauge.h>
//*)

class GvarsFrame: public wxFrame
{
	public:

		GvarsFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~GvarsFrame();

    GvarsFrame *GvFr;

		//(*Declarations(GvarsFrame)
		wxSlider* Slider1;
		wxPanel* Panel20;
		wxPanel* Panel5;
		wxGauge* Gauge3;
		wxSlider* Slider2;
		wxStaticText* StaticText2;
		wxPanel* Panel4;
		wxSlider* Slider5;
		wxStaticText* StaticText6;
		wxGauge* Gauge4;
		wxGauge* Gauge1;
		wxPanel* Panel21;
		wxPanel* Panel11;
		wxSlider* Slider3;
		wxPanel* Panel9;
		wxStaticText* StaticText8;
		wxPanel* Panel8;
		wxGauge* Gauge5;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxStaticBox* StaticBox1;
		wxGrid* Grid1;
		wxPanel* Panel6;
		wxPanel* Panel3;
		wxStaticBox* StaticBox2;
		wxPanel* Panel16;
		wxStaticText* StaticText5;
		wxStaticText* StaticText7;
		wxPanel* Panel7;
		wxSlider* Slider4;
		wxPanel* Panel18;
		wxPanel* Panel15;
		wxStaticBox* StaticBox3;
		wxTextCtrl* TextCtrl1;
		wxGauge* Gauge2;
		wxPanel* Panel14;
		wxPanel* Panel2;
		wxPanel* Panel10;
		wxStaticText* StaticText4;
		wxPanel* Panel12;
		wxPanel* Panel17;
		wxStaticBox* StaticBox4;
		wxPanel* Panel13;
		wxPanel* Panel19;
		//*)

	protected:

		//(*Identifiers(GvarsFrame)
		static const long ID_STATICBOX3;
		static const long ID_STATICBOX2;
		static const long ID_GRID1;
		static const long ID_STATICTEXT4;
		static const long ID_STATICBOX1;
		static const long ID_PANEL8;
		static const long ID_PANEL21;
		static const long ID_PANEL20;
		static const long ID_PANEL19;
		static const long ID_PANEL18;
		static const long ID_PANEL17;
		static const long ID_PANEL16;
		static const long ID_PANEL15;
		static const long ID_PANEL14;
		static const long ID_PANEL13;
		static const long ID_PANEL12;
		static const long ID_PANEL11;
		static const long ID_PANEL10;
		static const long ID_PANEL9;
		static const long ID_PANEL2;
		static const long ID_PANEL4;
		static const long ID_PANEL7;
		static const long ID_PANEL6;
		static const long ID_PANEL5;
		static const long ID_PANEL3;
		static const long ID_SLIDER2;
		static const long ID_SLIDER5;
		static const long ID_SLIDER4;
		static const long ID_SLIDER3;
		static const long ID_SLIDER1;
		static const long ID_GAUGE2;
		static const long ID_GAUGE5;
		static const long ID_GAUGE4;
		static const long ID_GAUGE3;
		static const long ID_GAUGE1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT5;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT1;
		static const long ID_STATICBOX4;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT7;
		static const long ID_STATICTEXT8;
		static const long ID_STATICTEXT6;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(GvarsFrame)
		void OnPanel4Paint(wxPaintEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
