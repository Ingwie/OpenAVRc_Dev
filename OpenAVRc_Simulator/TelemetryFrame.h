#ifndef TELEMETRYFRAME_H
#define TELEMETRYFRAME_H

//(*Headers(TelemetryFrame)
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
//*)

#define wxDEFAULT_DIALOG_STYLE  (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)

class TelemetryFrame: public wxFrame
{
	public:

		TelemetryFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~TelemetryFrame();

		//(*Declarations(TelemetryFrame)
		wxPanel* Panel1;
		wxSpinCtrl* GPSAltitude;
		wxSpinCtrl* GPSLatDegres;
		wxSpinCtrl* GPSLatMinute;
		wxSpinCtrl* GPSLatSecond;
		wxSpinCtrl* GPSLongDEgres;
		wxSpinCtrl* GPSLongMinute;
		wxSpinCtrl* GPSLongSecond;
		wxStaticBox* StaticBox1;
		wxStaticBox* StaticBox2;
		wxStaticBox* StaticBox3;
		wxStaticBox* gpsbox;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxStaticText* StaticText6;
		//*)

	protected:

		//(*Identifiers(TelemetryFrame)
		static const long ID_GPSBOX;
		static const long ID_STATICBOX2;
		static const long ID_SPINCTRL4;
		static const long ID_SPINCTRL5;
		static const long ID_SPINCTRL6;
		static const long ID_STATICBOX1;
		static const long ID_STATICTEXT4;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT5;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT6;
		static const long ID_STATICTEXT3;
		static const long ID_SPINCTRL1;
		static const long ID_SPINCTRL2;
		static const long ID_SPINCTRL3;
		static const long ID_STATICBOX3;
		static const long ID_SPINCTRL7;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(TelemetryFrame)
		void OnClose(wxCloseEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
