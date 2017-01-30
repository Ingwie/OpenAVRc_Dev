#ifndef OUTBARSFRAME_H
#define OUTBARSFRAME_H

//(*Headers(OutBarsFrame)
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/timer.h>
//*)

class OutBarsFrame: public wxFrame
{
	public:

		OutBarsFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~OutBarsFrame();

    void FillBarFrame();

		//(*Declarations(OutBarsFrame)
		wxSlider* Slider1;
		wxStaticText* StaticText10;
		wxStaticText* StaticText9;
		wxTextCtrl* output6;
		wxTextCtrl* output12;
		wxTextCtrl* output1;
		wxSlider* Slider10;
		wxSlider* Slider2;
		wxStaticText* StaticText13;
		wxStaticText* StaticText2;
		wxTextCtrl* output8;
		wxStaticText* StaticText14;
		wxSlider* Slider5;
		wxTimer TimerRefreshFrame;
		wxStaticText* StaticText6;
		wxTextCtrl* output11;
		wxTextCtrl* output7;
		wxTextCtrl* output2;
		wxTextCtrl* output3;
		wxSlider* Slider3;
		wxStaticText* StaticText8;
		wxStaticText* StaticText11;
		wxTextCtrl* output13;
		wxSlider* Slider7;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		wxTextCtrl* output16;
		wxStaticText* StaticText3;
		wxTextCtrl* output4;
		wxSlider* Slider13;
		wxTextCtrl* output15;
		wxSlider* Slider11;
		wxSlider* Slider9;
		wxStaticText* StaticText5;
		wxStaticText* StaticText7;
		wxTextCtrl* output14;
		wxSlider* Slider15;
		wxSlider* Slider16;
		wxSlider* Slider4;
		wxStaticLine* StaticLine1;
		wxSlider* Slider12;
		wxSlider* Slider8;
		wxTextCtrl* output10;
		wxStaticText* StaticText15;
		wxStaticText* StaticText12;
		wxTextCtrl* output9;
		wxSlider* Slider6;
		wxTextCtrl* output5;
		wxStaticText* StaticText4;
		wxStaticText* StaticText16;
		wxSlider* Slider14;
		//*)

	protected:

		//(*Identifiers(OutBarsFrame)
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
		static const long ID_SLIDER1;
		static const long ID_SLIDER2;
		static const long ID_SLIDER3;
		static const long ID_SLIDER4;
		static const long ID_SLIDER5;
		static const long ID_SLIDER6;
		static const long ID_SLIDER7;
		static const long ID_SLIDER8;
		static const long ID_SLIDER9;
		static const long ID_SLIDER10;
		static const long ID_SLIDER11;
		static const long ID_SLIDER12;
		static const long ID_SLIDER13;
		static const long ID_SLIDER14;
		static const long ID_SLIDER15;
		static const long ID_SLIDER16;
		static const long ID_STATICLINE1;
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
		void OnPanel1Paint(wxPaintEvent& event);
		void OnSlider13CmdScroll(wxScrollEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
