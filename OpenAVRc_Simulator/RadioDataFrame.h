#ifndef RADIODATAFRAME_H
#define RADIODATAFRAME_H

//(*Headers(RadioDataFrame)
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/timer.h>
//*)

class RadioDataFrame: public wxFrame
{
	public:

		RadioDataFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~RadioDataFrame();

		void FillRadioDataFrame();

		RadioDataFrame *RaFr;

		//(*Declarations(RadioDataFrame)
		wxTextCtrl* TextCtrlstickMode;
		wxStaticText* StaticText2;
		wxTextCtrl* TextCtrlvariant;
		wxTextCtrl* TextCtrlversion;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		wxTextCtrl* TextCtrlBatt;
		wxTextCtrl* TextCtrlcalibration;
		wxTimer RadioData;
		wxTextCtrl* TextCtrlcurrModel;
		//*)

	protected:

		//(*Identifiers(RadioDataFrame)
		static const long ID_TEXTCTRLVERSION;
		static const long ID_TEXTCTRL2;
		static const long ID_TEXTCTRL3;
		static const long ID_TEXTCTRLBATT;
		static const long ID_TEXTCTRLSTICKMODE;
		static const long ID_TEXTCTRLCalibration;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_PANEL1;
		static const long ID_TIMERRADIODATA;
		//*)

	private:

		//(*Handlers(RadioDataFrame)
		void OnTextCtrl1Text(wxCommandEvent& event);
		void OnTextCtrlvariantText(wxCommandEvent& event);
		void OnPanel1Paint(wxPaintEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
