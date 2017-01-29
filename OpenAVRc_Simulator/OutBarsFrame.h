#ifndef OUTBARSFRAME_H
#define OUTBARSFRAME_H

//(*Headers(OutBarsFrame)
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/gauge.h>
//*)

class OutBarsFrame: public wxFrame
{
	public:

		OutBarsFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~OutBarsFrame();

    void FillBarFrame();

		//(*Declarations(OutBarsFrame)
		wxStaticText* StaticText10;
		wxStaticText* StaticText9;
		wxTextCtrl* output6;
		wxTextCtrl* output12;
		wxGauge* Gauge3;
		wxTextCtrl* output1;
		wxStaticText* StaticText13;
		wxStaticText* StaticText2;
		wxTextCtrl* output8;
		wxStaticText* StaticText14;
		wxTimer TimerRefreshFrame;
		wxStaticText* StaticText6;
		wxGauge* Gauge4;
		wxTextCtrl* output11;
		wxTextCtrl* output7;
		wxGauge* Gauge1;
		wxTextCtrl* output2;
		wxGauge* Gauge12;
		wxTextCtrl* output3;
		wxGauge* Gauge6;
		wxStaticText* StaticText8;
		wxStaticText* StaticText11;
		wxTextCtrl* output13;
		wxGauge* Gauge5;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		wxTextCtrl* output16;
		wxStaticText* StaticText3;
		wxTextCtrl* output4;
		wxGauge* Gauge10;
		wxTextCtrl* output15;
		wxGauge* Gauge11;
		wxStaticText* StaticText5;
		wxStaticText* StaticText7;
		wxTextCtrl* output14;
		wxTextCtrl* output10;
		wxStaticText* StaticText15;
		wxStaticText* StaticText12;
		wxGauge* Gauge2;
		wxGauge* Gauge8;
		wxGauge* Gauge15;
		wxGauge* Gauge16;
		wxTextCtrl* output9;
		wxTextCtrl* output5;
		wxStaticText* StaticText4;
		wxGauge* Gauge14;
		wxGauge* Gauge7;
		wxGauge* Gauge9;
		wxStaticText* StaticText16;
		wxGauge* Gauge13;
		//*)

	protected:

		//(*Identifiers(OutBarsFrame)
		static const long ID_GAUGE1;
		static const long ID_GAUGE2;
		static const long ID_GAUGE3;
		static const long ID_GAUGE4;
		static const long ID_GAUGE5;
		static const long ID_GAUGE6;
		static const long ID_GAUGE7;
		static const long ID_GAUGE8;
		static const long ID_GAUGE9;
		static const long ID_GAUGE10;
		static const long ID_GAUGE11;
		static const long ID_GAUGE12;
		static const long ID_GAUGE13;
		static const long ID_GAUGE14;
		static const long ID_GAUGE15;
		static const long ID_GAUGE16;
		static const long ID_TEXTCTRL1;
		static const long ID_TEXTCTRL2;
		static const long ID_TEXTCTRL3;
		static const long ID_TEXTCTRL4;
		static const long ID_TEXTCTRL5;
		static const long ID_TEXTCTRL6;
		static const long ID_TEXTCTRL7;
		static const long ID_TEXTCTRL8;
		static const long ID_TEXTCTRL9;
		static const long ID_TEXTCTRL10;
		static const long ID_TEXTCTRL11;
		static const long ID_TEXTCTRL12;
		static const long ID_TEXTCTRL13;
		static const long ID_TEXTCTRL14;
		static const long ID_TEXTCTRL15;
		static const long ID_TEXTCTRL16;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT4;
		static const long ID_STATICTEXT5;
		static const long ID_STATICTEXT6;
		static const long ID_STATICTEXT7;
		static const long ID_STATICTEXT8;
		static const long ID_STATICTEXT9;
		static const long ID_STATICTEXT10;
		static const long ID_STATICTEXT11;
		static const long ID_STATICTEXT12;
		static const long ID_STATICTEXT13;
		static const long ID_STATICTEXT14;
		static const long ID_STATICTEXT15;
		static const long ID_STATICTEXT16;
		static const long ID_PANEL1;
		static const long ID_TIMER1;
		//*)

	private:

		//(*Handlers(OutBarsFrame)
		void OnTextCtrl1Text(wxCommandEvent& event);
		void OnTextCtrl1Text1(wxCommandEvent& event);
		void OnTextCtrl5Text(wxCommandEvent& event);
		void OnTextCtrl5Text1(wxCommandEvent& event);
		void OnTimerRefreshFrameTrigger(wxTimerEvent& event);
		void Onoutput10Text(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
