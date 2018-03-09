#ifndef TELEMETRYFRAME_H
#define TELEMETRYFRAME_H


//(*Headers(TelemetryFrame)
#include <wx/combobox.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/timectrl.h>
//*)

#include "OpenAVRc_SimulatorMain.h"

#define wxDEFAULT_DIALOG_STYLE  (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)

class TelemetryFrame: public wxFrame
{
	public:

		TelemetryFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~TelemetryFrame();

		//(*Declarations(TelemetryFrame)
		wxComboBox* ComboBoxCom;
		wxDatePickerCtrl* GPSDate;
		wxPanel* Panel1;
		wxRadioButton* RBFrskyd;
		wxRadioButton* RBFrskysport;
		wxSlider* AcclX;
		wxSlider* AcclY;
		wxSlider* AcclZ;
		wxSlider* Cell10;
		wxSlider* Cell11;
		wxSlider* Cell12;
		wxSlider* Cell1;
		wxSlider* Cell2;
		wxSlider* Cell3;
		wxSlider* Cell4;
		wxSlider* Cell5;
		wxSlider* Cell6;
		wxSlider* Cell7;
		wxSlider* Cell8;
		wxSlider* Cell9;
		wxSlider* VarioAlt;
		wxSlider* VarioHSpeed;
		wxSpinCtrl* AirSpeed;
		wxSpinCtrl* Analog1;
		wxSpinCtrl* Analog2;
		wxSpinCtrl* Analog3;
		wxSpinCtrl* Analog4;
		wxSpinCtrl* BattAmp;
		wxSpinCtrl* BattVolts;
		wxSpinCtrl* FUEL;
		wxSpinCtrl* GPSAltitude;
		wxSpinCtrl* GPSCourse;
		wxSpinCtrl* GPSLatDegres;
		wxSpinCtrl* GPSLatMinute;
		wxSpinCtrl* GPSLongDEgres;
		wxSpinCtrl* GPSLongMinute;
		wxSpinCtrl* GPSSpeed;
		wxSpinCtrl* RPM;
		wxSpinCtrl* RSSIRx;
		wxSpinCtrl* RSSITx;
		wxSpinCtrl* Temp1;
		wxSpinCtrl* Temp2;
		wxStaticBox* StaticBox10;
		wxStaticBox* StaticBox11;
		wxStaticBox* StaticBox12;
		wxStaticBox* StaticBox13;
		wxStaticBox* StaticBox14;
		wxStaticBox* StaticBox1;
		wxStaticBox* StaticBox2;
		wxStaticBox* StaticBox3;
		wxStaticBox* StaticBox4;
		wxStaticBox* StaticBox5;
		wxStaticBox* StaticBox6;
		wxStaticBox* StaticBox7;
		wxStaticBox* StaticBox8;
		wxStaticBox* StaticBox9;
		wxStaticBox* gpsbox;
		wxStaticText* StaticText10;
		wxStaticText* StaticText11;
		wxStaticText* StaticText12;
		wxStaticText* StaticText13;
		wxStaticText* StaticText14;
		wxStaticText* StaticText15;
		wxStaticText* StaticText16;
		wxStaticText* StaticText17;
		wxStaticText* StaticText18;
		wxStaticText* StaticText19;
		wxStaticText* StaticText1;
		wxStaticText* StaticText20;
		wxStaticText* StaticText21;
		wxStaticText* StaticText22;
		wxStaticText* StaticText23;
		wxStaticText* StaticText24;
		wxStaticText* StaticText25;
		wxStaticText* StaticText26;
		wxStaticText* StaticText27;
		wxStaticText* StaticText28;
		wxStaticText* StaticText29;
		wxStaticText* StaticText2;
		wxStaticText* StaticText30;
		wxStaticText* StaticText31;
		wxStaticText* StaticText32;
		wxStaticText* StaticText33;
		wxStaticText* StaticText34;
		wxStaticText* StaticText35;
		wxStaticText* StaticText36;
		wxStaticText* StaticText37;
		wxStaticText* StaticText3;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxStaticText* StaticText6;
		wxStaticText* StaticText7;
		wxStaticText* StaticText8;
		wxStaticText* StaticText9;
		wxTimePickerCtrl* GPSTime;
		//*)

	protected:

		//(*Identifiers(TelemetryFrame)
		static const long ID_STATICBOX14;
		static const long ID_STATICBOX13;
		static const long ID_STATICBOX11;
		static const long ID_STATICBOX9;
		static const long ID_STATICTEXT23;
		static const long ID_GPSBOX;
		static const long ID_STATICBOX2;
		static const long ID_SPINCTRL5;
		static const long ID_SPINCTRL6;
		static const long ID_STATICBOX1;
		static const long ID_STATICTEXT4;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT5;
		static const long ID_STATICTEXT2;
		static const long ID_SPINCTRL1;
		static const long ID_SPINCTRL2;
		static const long ID_STATICBOX3;
		static const long ID_SPINCTRL7;
		static const long ID_STATICBOX4;
		static const long ID_SPINCTRL8;
		static const long ID_STATICBOX5;
		static const long ID_SPINCTRL9;
		static const long ID_STATICBOX6;
		static const long ID_DATEPICKERCTRL1;
		static const long ID_TIMEPICKERCTRL1;
		static const long ID_STATICBOX7;
		static const long ID_SLIDER1;
		static const long ID_SLIDER2;
		static const long ID_SLIDER3;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT6;
		static const long ID_STATICTEXT7;
		static const long ID_STATICBOX8;
		static const long ID_SLIDER5;
		static const long ID_SLIDER4;
		static const long ID_STATICTEXT8;
		static const long ID_STATICTEXT9;
		static const long ID_SPINCTRL4;
		static const long ID_SPINCTRL3;
		static const long ID_STATICTEXT11;
		static const long ID_STATICTEXT10;
		static const long ID_STATICTEXT30;
		static const long ID_STATICBOX10;
		static const long ID_SPINCTRL11;
		static const long ID_SPINCTRL10;
		static const long ID_STATICTEXT13;
		static const long ID_STATICTEXT12;
		static const long ID_SPINCTRL13;
		static const long ID_SPINCTRL12;
		static const long ID_STATICTEXT14;
		static const long ID_STATICTEXT15;
		static const long ID_SPINCTRL14;
		static const long ID_SPINCTRL15;
		static const long ID_STATICTEXT16;
		static const long ID_STATICTEXT17;
		static const long ID_STATICBOX12;
		static const long ID_SPINCTRL16;
		static const long ID_SPINCTRL17;
		static const long ID_SPINCTRL18;
		static const long ID_SPINCTRL19;
		static const long ID_STATICTEXT18;
		static const long ID_STATICTEXT19;
		static const long ID_STATICTEXT20;
		static const long ID_STATICTEXT21;
		static const long ID_SLIDER9;
		static const long ID_SLIDER17;
		static const long ID_SLIDER16;
		static const long ID_SLIDER15;
		static const long ID_SLIDER14;
		static const long ID_SLIDER13;
		static const long ID_SLIDER12;
		static const long ID_SLIDER11;
		static const long ID_SLIDER10;
		static const long ID_SLIDER6;
		static const long ID_SLIDER8;
		static const long ID_SLIDER7;
		static const long ID_STATICTEXT27;
		static const long ID_STATICTEXT28;
		static const long ID_STATICTEXT32;
		static const long ID_STATICTEXT35;
		static const long ID_STATICTEXT33;
		static const long ID_STATICTEXT31;
		static const long ID_STATICTEXT29;
		static const long ID_STATICTEXT25;
		static const long ID_STATICTEXT26;
		static const long ID_STATICTEXT24;
		static const long ID_STATICTEXT22;
		static const long ID_SPINCTRL20;
		static const long ID_STATICTEXT34;
		static const long ID_STATICTEXT36;
		static const long ID_COMBOBOXCOM;
		static const long ID_STATICTEXT37;
		static const long ID_RADIOBUTTON2;
		static const long ID_RADIOBUTTON1;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(TelemetryFrame)
		void OnClose(wxCloseEvent& event);
		void OnTeleChange(wxSpinEvent& event);
		void OnGPSDateTimeChanged(wxDateEvent& event);
		void OnTeleScrollChanged(wxScrollEvent& event);
		void OnComboBoxComDropdown(wxCommandEvent& event);
		void OnRBProtoSelect(wxCommandEvent& event);
		void OnComboBoxComSelected(wxCommandEvent& event);
		//*)

		void DetectSerial();
		void WriteDatas();
		DECLARE_EVENT_TABLE()
};

#endif
