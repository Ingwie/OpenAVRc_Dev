 /*
 **************************************************************************
 *                                                                        *
 *                 ____                ___ _   _____                      *
 *                / __ \___  ___ ___  / _ | | / / _ \____                 *
 *               / /_/ / _ \/ -_) _ \/ __ | |/ / , _/ __/                 *
 *               \____/ .__/\__/_//_/_/ |_|___/_/|_|\__/                  *
 *                   /_/                                                  *
 *                                                                        *
 *              This file is part of the OpenAVRc project.                *
 *                                                                        *
 *                         Based on code(s) named :                       *
 *             OpenTx - https://github.com/opentx/opentx                  *
 *             Deviation - https://www.deviationtx.com/                   *
 *                                                                        *
 *                Only AVR code here for visibility ;-)                   *
 *                                                                        *
 *   OpenAVRc is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation, either version 2 of the License, or    *
 *   (at your option) any later version.                                  *
 *                                                                        *
 *   OpenAVRc is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *   GNU General Public License for more details.                         *
 *                                                                        *
 *       License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html          *
 *                                                                        *
 **************************************************************************
*/


#include "OpenAVRc_SimulatorMain.h"
#include "TelemetryFrame.h"

//(*InternalHeaders(TelemetryFrame)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

// Telemetry datas
extern float Tele_GPSLat;
extern float Tele_GPSLong;
extern float Tele_GPSAlt;
extern float Tele_GPSSpeed;
extern float Tele_GPSCourse;
extern int Tele_GPSYear;
extern int Tele_GPSMonth;
extern int Tele_GPSDay;
extern int Tele_GPSHour;
extern int Tele_GPSMinute;
extern int Tele_GPSSecond;

extern float Tele_AccelX;
extern float Tele_AccelY;
extern float Tele_AccelZ;

extern float Tele_VarioAlt;
extern float Tele_VarioAccelH;

extern int Tele_RSSITx;
extern int Tele_RSSIRx;

extern float Tele_Analog1;
extern float Tele_Analog2;
extern float Tele_Analog3;
extern float Tele_Analog4;

extern float Tele_BattVolt;
extern float Tele_BattCurrent;

extern float Tele_RPM;
extern float Tele_Fuel;
extern float Tele_Temp1;
extern float Tele_Temp2;
extern float Tele_Airspeed;

extern float Tele_Cell1;
extern float Tele_Cell2;
extern float Tele_Cell3;
extern float Tele_Cell4;
extern float Tele_Cell5;
extern float Tele_Cell6;
extern float Tele_Cell7;
extern float Tele_Cell8;
extern float Tele_Cell9;
extern float Tele_Cell10;
extern float Tele_Cell11;
extern float Tele_Cell12;


extern bool Ini_Changed;

//(*IdInit(TelemetryFrame)
const long TelemetryFrame::ID_STATICBOX13 = wxNewId();
const long TelemetryFrame::ID_STATICBOX11 = wxNewId();
const long TelemetryFrame::ID_STATICBOX9 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT23 = wxNewId();
const long TelemetryFrame::ID_GPSBOX = wxNewId();
const long TelemetryFrame::ID_STATICBOX2 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL5 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL6 = wxNewId();
const long TelemetryFrame::ID_STATICBOX1 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT4 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT1 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT5 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT2 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL1 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL2 = wxNewId();
const long TelemetryFrame::ID_STATICBOX3 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL7 = wxNewId();
const long TelemetryFrame::ID_STATICBOX4 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL8 = wxNewId();
const long TelemetryFrame::ID_STATICBOX5 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL9 = wxNewId();
const long TelemetryFrame::ID_STATICBOX6 = wxNewId();
const long TelemetryFrame::ID_DATEPICKERCTRL1 = wxNewId();
const long TelemetryFrame::ID_TIMEPICKERCTRL1 = wxNewId();
const long TelemetryFrame::ID_STATICBOX7 = wxNewId();
const long TelemetryFrame::ID_SLIDER1 = wxNewId();
const long TelemetryFrame::ID_SLIDER2 = wxNewId();
const long TelemetryFrame::ID_SLIDER3 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT3 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT6 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT7 = wxNewId();
const long TelemetryFrame::ID_STATICBOX8 = wxNewId();
const long TelemetryFrame::ID_SLIDER5 = wxNewId();
const long TelemetryFrame::ID_SLIDER4 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT8 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT9 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL4 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL3 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT11 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT10 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT30 = wxNewId();
const long TelemetryFrame::ID_STATICBOX10 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL11 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL10 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT13 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT12 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL13 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL12 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT14 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT15 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL14 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL15 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT16 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT17 = wxNewId();
const long TelemetryFrame::ID_STATICBOX12 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL16 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL17 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL18 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL19 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT18 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT19 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT20 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT21 = wxNewId();
const long TelemetryFrame::ID_SLIDER9 = wxNewId();
const long TelemetryFrame::ID_SLIDER17 = wxNewId();
const long TelemetryFrame::ID_SLIDER16 = wxNewId();
const long TelemetryFrame::ID_SLIDER15 = wxNewId();
const long TelemetryFrame::ID_SLIDER14 = wxNewId();
const long TelemetryFrame::ID_SLIDER13 = wxNewId();
const long TelemetryFrame::ID_SLIDER12 = wxNewId();
const long TelemetryFrame::ID_SLIDER11 = wxNewId();
const long TelemetryFrame::ID_SLIDER10 = wxNewId();
const long TelemetryFrame::ID_SLIDER6 = wxNewId();
const long TelemetryFrame::ID_SLIDER8 = wxNewId();
const long TelemetryFrame::ID_SLIDER7 = wxNewId();
const long TelemetryFrame::ID_STATICBOX14 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT27 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT28 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT32 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT35 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT33 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT31 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT29 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT25 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT26 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT24 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT22 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL20 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT34 = wxNewId();
const long TelemetryFrame::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(TelemetryFrame,wxFrame)
	//(*EventTable(TelemetryFrame)
	//*)
END_EVENT_TABLE()

TelemetryFrame::TelemetryFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(TelemetryFrame)
	Create(parent, wxID_ANY, _("Télémétrie"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(1152,239));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(472,72), wxSize(1152,232), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	StaticBox13 = new wxStaticBox(Panel1, ID_STATICBOX13, _("Batterie"), wxPoint(416,104), wxSize(304,128), 0, _T("ID_STATICBOX13"));
	StaticBox11 = new wxStaticBox(Panel1, ID_STATICBOX11, _("Variateur"), wxPoint(136,104), wxSize(128,64), 0, _T("ID_STATICBOX11"));
	StaticBox9 = new wxStaticBox(Panel1, ID_STATICBOX9, _("RSSI"), wxPoint(8,104), wxSize(120,64), 0, _T("ID_STATICBOX9"));
	StaticText23 = new wxStaticText(Panel1, ID_STATICTEXT23, _("X10"), wxPoint(185,128), wxDefaultSize, 0, _T("ID_STATICTEXT23"));
	wxFont StaticText23Font(5,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText23->SetFont(StaticText23Font);
	gpsbox = new wxStaticBox(Panel1, ID_GPSBOX, _("GPS"), wxPoint(8,8), wxSize(744,96), 0, _T("ID_GPSBOX"));
	StaticBox2 = new wxStaticBox(Panel1, ID_STATICBOX2, _("Latitude"), wxPoint(16,24), wxSize(168,72), 0, _T("ID_STATICBOX2"));
	GPSLatMinute = new wxSpinCtrl(Panel1, ID_SPINCTRL5, _T("0"), wxPoint(80,64), wxSize(80,21), 0, 0, 999999, 0, _T("ID_SPINCTRL5"));
	GPSLatMinute->SetValue(_T("0"));
	GPSLatDegres = new wxSpinCtrl(Panel1, ID_SPINCTRL6, _T("0"), wxPoint(24,64), wxSize(48,21), 0, -90, 90, 0, _T("ID_SPINCTRL6"));
	GPSLatDegres->SetValue(_T("0"));
	StaticBox1 = new wxStaticBox(Panel1, ID_STATICBOX1, _("Longitude"), wxPoint(192,24), wxSize(176,72), 0, _T("ID_STATICBOX1"));
	StaticText4 = new wxStaticText(Panel1, ID_STATICTEXT4, _("Degrés"), wxPoint(24,40), wxSize(37,13), 0, _T("ID_STATICTEXT4"));
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Degrés"), wxPoint(208,40), wxSize(37,13), 0, _T("ID_STATICTEXT1"));
	StaticText5 = new wxStaticText(Panel1, ID_STATICTEXT5, _("Minutes - Secondes"), wxPoint(80,40), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("Minutes - Secondes"), wxPoint(264,40), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	GPSLongDEgres = new wxSpinCtrl(Panel1, ID_SPINCTRL1, _T("0"), wxPoint(208,64), wxSize(48,21), 0, -180, 180, 0, _T("ID_SPINCTRL1"));
	GPSLongDEgres->SetValue(_T("0"));
	GPSLongMinute = new wxSpinCtrl(Panel1, ID_SPINCTRL2, _T("0"), wxPoint(264,64), wxSize(80,21), 0, 0, 999999, 0, _T("ID_SPINCTRL2"));
	GPSLongMinute->SetValue(_T("0"));
	StaticBox3 = new wxStaticBox(Panel1, ID_STATICBOX3, _("Altitude"), wxPoint(376,32), wxSize(88,64), 0, _T("ID_STATICBOX3"));
	GPSAltitude = new wxSpinCtrl(Panel1, ID_SPINCTRL7, _T("0"), wxPoint(384,64), wxSize(72,21), 0, -1000, 10000, 0, _T("ID_SPINCTRL7"));
	GPSAltitude->SetValue(_T("0"));
	StaticBox4 = new wxStaticBox(Panel1, ID_STATICBOX4, _("Vitesse (m/s)"), wxPoint(472,32), wxSize(72,64), 0, _T("ID_STATICBOX4"));
	GPSSpeed = new wxSpinCtrl(Panel1, ID_SPINCTRL8, _T("0"), wxPoint(480,64), wxSize(56,21), 0, 0, 1000, 0, _T("ID_SPINCTRL8"));
	GPSSpeed->SetValue(_T("0"));
	StaticBox5 = new wxStaticBox(Panel1, ID_STATICBOX5, _("Déplacement °"), wxPoint(552,32), wxSize(80,64), 0, _T("ID_STATICBOX5"));
	GPSCourse = new wxSpinCtrl(Panel1, ID_SPINCTRL9, _T("0"), wxPoint(560,64), wxSize(72,21), 0, 0, 36000, 0, _T("ID_SPINCTRL9"));
	GPSCourse->SetValue(_T("0"));
	StaticBox6 = new wxStaticBox(Panel1, ID_STATICBOX6, _("Date - Heure"), wxPoint(640,24), wxSize(96,72), 0, _T("ID_STATICBOX6"));
	GPSDate = new wxDatePickerCtrl(Panel1, ID_DATEPICKERCTRL1, wxDefaultDateTime, wxPoint(648,48), wxSize(80,21), wxDP_DEFAULT|wxDP_SHOWCENTURY, wxDefaultValidator, _T("ID_DATEPICKERCTRL1"));
	GPSTime = new wxTimePickerCtrl(Panel1, ID_TIMEPICKERCTRL1, wxDateTime::Now(), wxPoint(648,72), wxSize(80,21), 0, wxDefaultValidator, _T("ID_TIMEPICKERCTRL1"));
	StaticBox7 = new wxStaticBox(Panel1, ID_STATICBOX7, _("Accéléromètres"), wxPoint(760,8), wxSize(184,96), 0, _T("ID_STATICBOX7"));
	AcclX = new wxSlider(Panel1, ID_SLIDER1, 0, -300, 300, wxPoint(792,32), wxSize(144,20), 0, wxDefaultValidator, _T("ID_SLIDER1"));
	AcclY = new wxSlider(Panel1, ID_SLIDER2, 0, -300, 300, wxPoint(792,56), wxSize(144,20), 0, wxDefaultValidator, _T("ID_SLIDER2"));
	AcclZ = new wxSlider(Panel1, ID_SLIDER3, 0, -300, 300, wxPoint(792,80), wxSize(144,20), 0, wxDefaultValidator, _T("ID_SLIDER3"));
	StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("X"), wxPoint(776,32), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	StaticText6 = new wxStaticText(Panel1, ID_STATICTEXT6, _("Y"), wxPoint(776,56), wxSize(16,16), 0, _T("ID_STATICTEXT6"));
	StaticText7 = new wxStaticText(Panel1, ID_STATICTEXT7, _("Z"), wxPoint(776,80), wxSize(16,13), 0, _T("ID_STATICTEXT7"));
	StaticBox8 = new wxStaticBox(Panel1, ID_STATICBOX8, _("Variomètre"), wxPoint(952,8), wxSize(192,96), 0, _T("ID_STATICBOX8"));
	VarioAlt = new wxSlider(Panel1, ID_SLIDER5, 0, -50, 2000, wxPoint(960,40), wxSize(176,20), 0, wxDefaultValidator, _T("ID_SLIDER5"));
	VarioHSpeed = new wxSlider(Panel1, ID_SLIDER4, 0, -200, 200, wxPoint(960,80), wxSize(176,20), 0, wxDefaultValidator, _T("ID_SLIDER4"));
	StaticText8 = new wxStaticText(Panel1, ID_STATICTEXT8, _("Altitude"), wxPoint(976,24), wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	StaticText9 = new wxStaticText(Panel1, ID_STATICTEXT9, _("Vitesse Verticale"), wxPoint(976,64), wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	RSSITx = new wxSpinCtrl(Panel1, ID_SPINCTRL4, _T("0"), wxPoint(16,136), wxSize(48,21), 0, 0, 100, 0, _T("ID_SPINCTRL4"));
	RSSITx->SetValue(_T("0"));
	RSSIRx = new wxSpinCtrl(Panel1, ID_SPINCTRL3, _T("0"), wxPoint(72,136), wxSize(48,21), 0, 0, 100, 0, _T("ID_SPINCTRL3"));
	RSSIRx->SetValue(_T("0"));
	StaticText11 = new wxStaticText(Panel1, ID_STATICTEXT11, _("TX"), wxPoint(24,120), wxDefaultSize, 0, _T("ID_STATICTEXT11"));
	StaticText10 = new wxStaticText(Panel1, ID_STATICTEXT10, _("RX"), wxPoint(80,120), wxDefaultSize, 0, _T("ID_STATICTEXT10"));
	StaticText30 = new wxStaticText(Panel1, ID_STATICTEXT30, _("X10"), wxPoint(96,192), wxDefaultSize, 0, _T("ID_STATICTEXT30"));
	wxFont StaticText30Font(5,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText30->SetFont(StaticText30Font);
	StaticBox10 = new wxStaticBox(Panel1, ID_STATICBOX10, _("Analog"), wxPoint(8,168), wxSize(256,64), 0, _T("ID_STATICBOX10"));
	Analog1 = new wxSpinCtrl(Panel1, ID_SPINCTRL11, _T("0"), wxPoint(16,200), wxSize(48,21), 0, 0, 1000, 0, _T("ID_SPINCTRL11"));
	Analog1->SetValue(_T("0"));
	Analog2 = new wxSpinCtrl(Panel1, ID_SPINCTRL10, _T("0"), wxPoint(72,200), wxSize(48,21), 0, 0, 1000, 0, _T("ID_SPINCTRL10"));
	Analog2->SetValue(_T("0"));
	StaticText13 = new wxStaticText(Panel1, ID_STATICTEXT13, _("A1"), wxPoint(24,184), wxDefaultSize, 0, _T("ID_STATICTEXT13"));
	StaticText12 = new wxStaticText(Panel1, ID_STATICTEXT12, _("A2"), wxPoint(80,184), wxDefaultSize, 0, _T("ID_STATICTEXT12"));
	BattVolts = new wxSpinCtrl(Panel1, ID_SPINCTRL13, _T("0"), wxPoint(144,136), wxSize(48,21), 0, 0, 900, 0, _T("ID_SPINCTRL13"));
	BattVolts->SetValue(_T("0"));
	BattAmp = new wxSpinCtrl(Panel1, ID_SPINCTRL12, _T("0"), wxPoint(200,136), wxSize(56,21), 0, 0, 2000, 0, _T("ID_SPINCTRL12"));
	BattAmp->SetValue(_T("0"));
	StaticText14 = new wxStaticText(Panel1, ID_STATICTEXT14, _("Tension"), wxPoint(144,120), wxDefaultSize, 0, _T("ID_STATICTEXT14"));
	StaticText15 = new wxStaticText(Panel1, ID_STATICTEXT15, _("Courant"), wxPoint(208,120), wxDefaultSize, 0, _T("ID_STATICTEXT15"));
	Analog3 = new wxSpinCtrl(Panel1, ID_SPINCTRL14, _T("0"), wxPoint(136,200), wxSize(48,21), 0, 0, 3300, 0, _T("ID_SPINCTRL14"));
	Analog3->SetValue(_T("0"));
	Analog4 = new wxSpinCtrl(Panel1, ID_SPINCTRL15, _T("0"), wxPoint(192,200), wxSize(48,21), 0, 0, 3300, 0, _T("ID_SPINCTRL15"));
	Analog4->SetValue(_T("0"));
	StaticText16 = new wxStaticText(Panel1, ID_STATICTEXT16, _("A3"), wxPoint(144,184), wxDefaultSize, 0, _T("ID_STATICTEXT16"));
	StaticText17 = new wxStaticText(Panel1, ID_STATICTEXT17, _("A4"), wxPoint(200,184), wxDefaultSize, 0, _T("ID_STATICTEXT17"));
	StaticBox12 = new wxStaticBox(Panel1, ID_STATICBOX12, _("Autres"), wxPoint(264,104), wxSize(152,128), 0, _T("ID_STATICBOX12"));
	RPM = new wxSpinCtrl(Panel1, ID_SPINCTRL16, _T("0"), wxPoint(280,136), wxSize(64,21), 0, 0, 10000, 0, _T("ID_SPINCTRL16"));
	RPM->SetValue(_T("0"));
	FUEL = new wxSpinCtrl(Panel1, ID_SPINCTRL17, _T("0"), wxPoint(352,136), wxSize(48,21), 0, 0, 1000, 0, _T("ID_SPINCTRL17"));
	FUEL->SetValue(_T("0"));
	Temp1 = new wxSpinCtrl(Panel1, ID_SPINCTRL18, _T("0"), wxPoint(280,178), wxSize(56,21), 0, 0, 1000, 0, _T("ID_SPINCTRL18"));
	Temp1->SetValue(_T("0"));
	Temp2 = new wxSpinCtrl(Panel1, ID_SPINCTRL19, _T("0"), wxPoint(344,178), wxSize(56,21), 0, 0, 1000, 0, _T("ID_SPINCTRL19"));
	Temp2->SetValue(_T("0"));
	StaticText18 = new wxStaticText(Panel1, ID_STATICTEXT18, _("RPM"), wxPoint(296,120), wxDefaultSize, 0, _T("ID_STATICTEXT18"));
	StaticText19 = new wxStaticText(Panel1, ID_STATICTEXT19, _("FUEL"), wxPoint(360,120), wxDefaultSize, 0, _T("ID_STATICTEXT19"));
	StaticText20 = new wxStaticText(Panel1, ID_STATICTEXT20, _("Temp1"), wxPoint(280,160), wxDefaultSize, 0, _T("ID_STATICTEXT20"));
	StaticText21 = new wxStaticText(Panel1, ID_STATICTEXT21, _("Temp2"), wxPoint(344,160), wxDefaultSize, 0, _T("ID_STATICTEXT21"));
	Cell1 = new wxSlider(Panel1, ID_SLIDER9, 0, 0, 500, wxPoint(432,118), wxSize(20,112), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER9"));
	Cell12 = new wxSlider(Panel1, ID_SLIDER17, 0, 0, 500, wxPoint(696,118), wxSize(20,112), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER17"));
	Cell11 = new wxSlider(Panel1, ID_SLIDER16, 0, 0, 500, wxPoint(672,118), wxSize(20,112), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER16"));
	Cell10 = new wxSlider(Panel1, ID_SLIDER15, 0, 0, 500, wxPoint(648,118), wxSize(20,112), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER15"));
	Cell9 = new wxSlider(Panel1, ID_SLIDER14, 0, 0, 500, wxPoint(624,118), wxSize(20,112), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER14"));
	Cell8 = new wxSlider(Panel1, ID_SLIDER13, 0, 0, 500, wxPoint(600,118), wxSize(20,112), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER13"));
	Cell7 = new wxSlider(Panel1, ID_SLIDER12, 0, 0, 500, wxPoint(576,118), wxSize(20,112), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER12"));
	Cell6 = new wxSlider(Panel1, ID_SLIDER11, 0, 0, 500, wxPoint(552,118), wxSize(20,112), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER11"));
	Cell5 = new wxSlider(Panel1, ID_SLIDER10, 0, 0, 500, wxPoint(528,118), wxSize(20,112), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER10"));
	Cell4 = new wxSlider(Panel1, ID_SLIDER6, 0, 0, 500, wxPoint(504,118), wxSize(20,112), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER6"));
	Cell2 = new wxSlider(Panel1, ID_SLIDER8, 0, 0, 500, wxPoint(456,118), wxSize(20,112), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER8"));
	Cell3 = new wxSlider(Panel1, ID_SLIDER7, 0, 0, 500, wxPoint(480,118), wxSize(20,112), wxSL_VERTICAL|wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER7"));
	StaticBox14 = new wxStaticBox(Panel1, ID_STATICBOX14, _("Réglages"), wxPoint(728,104), wxSize(416,128), 0, _T("ID_STATICBOX14"));
	StaticText27 = new wxStaticText(Panel1, ID_STATICTEXT27, _("X10"), wxPoint(249,128), wxDefaultSize, 0, _T("ID_STATICTEXT27"));
	wxFont StaticText27Font(5,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText27->SetFont(StaticText27Font);
	StaticText28 = new wxStaticText(Panel1, ID_STATICTEXT28, _("X10"), wxPoint(40,192), wxDefaultSize, 0, _T("ID_STATICTEXT28"));
	wxFont StaticText28Font(5,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText28->SetFont(StaticText28Font);
	StaticText32 = new wxStaticText(Panel1, ID_STATICTEXT32, _("X10"), wxPoint(320,168), wxSize(13,39), 0, _T("ID_STATICTEXT32"));
	wxFont StaticText32Font(5,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText32->SetFont(StaticText32Font);
	StaticText35 = new wxStaticText(Panel1, ID_STATICTEXT35, _("X10"), wxPoint(384,168), wxDefaultSize, 0, _T("ID_STATICTEXT35"));
	wxFont StaticText35Font(5,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText35->SetFont(StaticText35Font);
	StaticText33 = new wxStaticText(Panel1, ID_STATICTEXT33, _("AIR X10"), wxPoint(316,216), wxDefaultSize, 0, _T("ID_STATICTEXT33"));
	wxFont StaticText33Font(5,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText33->SetFont(StaticText33Font);
	StaticText31 = new wxStaticText(Panel1, ID_STATICTEXT31, _("X10"), wxPoint(384,128), wxDefaultSize, 0, _T("ID_STATICTEXT31"));
	wxFont StaticText31Font(5,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText31->SetFont(StaticText31Font);
	StaticText29 = new wxStaticText(Panel1, ID_STATICTEXT29, _("X10"), wxPoint(216,192), wxDefaultSize, 0, _T("ID_STATICTEXT29"));
	wxFont StaticText29Font(5,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText29->SetFont(StaticText29Font);
	StaticText25 = new wxStaticText(Panel1, ID_STATICTEXT25, _("X10"), wxPoint(160,192), wxDefaultSize, 0, _T("ID_STATICTEXT25"));
	wxFont StaticText25Font(5,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText25->SetFont(StaticText25Font);
	StaticText26 = new wxStaticText(Panel1, ID_STATICTEXT26, _("X10"), wxPoint(520,56), wxDefaultSize, 0, _T("ID_STATICTEXT26"));
	wxFont StaticText26Font(5,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText26->SetFont(StaticText26Font);
	StaticText24 = new wxStaticText(Panel1, ID_STATICTEXT24, _("X100"), wxPoint(600,56), wxDefaultSize, 0, _T("ID_STATICTEXT24"));
	wxFont StaticText24Font(5,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText24->SetFont(StaticText24Font);
	StaticText22 = new wxStaticText(Panel1, ID_STATICTEXT22, _("X10"), wxPoint(432,56), wxDefaultSize, 0, _T("ID_STATICTEXT22"));
	wxFont StaticText22Font(5,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	StaticText22->SetFont(StaticText22Font);
	AirSpeed = new wxSpinCtrl(Panel1, ID_SPINCTRL20, _T("0"), wxPoint(344,204), wxSize(56,21), 0, 0, 3000, 0, _T("ID_SPINCTRL20"));
	AirSpeed->SetValue(_T("0"));
	StaticText34 = new wxStaticText(Panel1, ID_STATICTEXT34, _("Vitesse"), wxPoint(280,208), wxDefaultSize, 0, _T("ID_STATICTEXT34"));

	Connect(ID_SPINCTRL5,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL6,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL7,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL8,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL9,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_DATEPICKERCTRL1,wxEVT_DATE_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnGPSDateTimeChanged);
	Connect(ID_TIMEPICKERCTRL1,wxEVT_DATE_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnGPSDateTimeChanged);
	Connect(ID_SLIDER1,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER2,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER3,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER5,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER4,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SPINCTRL4,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL3,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL11,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL10,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL13,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL12,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL14,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL15,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL16,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL17,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL18,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SPINCTRL19,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(ID_SLIDER9,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER17,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER16,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER15,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER14,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER13,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER12,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER11,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER10,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER6,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER8,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SLIDER7,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&TelemetryFrame::OnTeleScrollChanged);
	Connect(ID_SPINCTRL20,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&TelemetryFrame::OnTeleChange);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&TelemetryFrame::OnClose);
	//*)

  {
    wxIcon FrameIcon;
    SetIcon(wxICON(oavrc_icon));
  }

  GPSLatDegres->SetValue((int)Tele_GPSLat);
  GPSLatMinute->SetValue((Tele_GPSLat - GPSLatDegres->GetValue())*1000000);
  GPSLongDEgres->SetValue((int)Tele_GPSLong);
  GPSLongMinute->SetValue((Tele_GPSLong - GPSLongDEgres->GetValue())*1000000);
  GPSAltitude->SetValue(Tele_GPSAlt*10);
  GPSSpeed->SetValue(Tele_GPSSpeed*10);
  GPSCourse->SetValue(Tele_GPSCourse*1000);

  AcclX->SetValue(Tele_AccelX*10);
  AcclY->SetValue(Tele_AccelY*10);
  AcclZ->SetValue(Tele_AccelZ*10);

  VarioAlt->SetValue(Tele_VarioAlt*10);
  VarioHSpeed->SetValue(Tele_VarioAccelH*10);

  RSSITx->SetValue(Tele_RSSITx);
  RSSIRx->SetValue(Tele_RSSIRx);

  Analog1->SetValue(Tele_Analog1*100);
  Analog2->SetValue(Tele_Analog2*100);
  Analog3->SetValue(Tele_Analog3*100);
  Analog4->SetValue(Tele_Analog4*100);

  BattVolts->SetValue(Tele_BattVolt*10);
  BattAmp->SetValue(Tele_BattCurrent*10);

  RPM->SetValue(Tele_RPM);
  FUEL->SetValue(Tele_Fuel*10);
  Temp1->SetValue(Tele_Temp1*10);
  Temp2->SetValue(Tele_Temp2*10);
  AirSpeed->SetValue(Tele_Airspeed*10);

  Cell1->SetValue(Tele_Cell1*100);
  Cell2->SetValue(Tele_Cell2*100);
  Cell3->SetValue(Tele_Cell3*100);
  Cell4->SetValue(Tele_Cell4*100);
  Cell5->SetValue(Tele_Cell5*100);
  Cell6->SetValue(Tele_Cell6*100);
  Cell7->SetValue(Tele_Cell7*100);
  Cell8->SetValue(Tele_Cell8*100);
  Cell9->SetValue(Tele_Cell9*100);
  Cell10->SetValue(Tele_Cell10*100);
  Cell11->SetValue(Tele_Cell11*100);
  Cell12->SetValue(Tele_Cell12*100);

}

TelemetryFrame::~TelemetryFrame()
{
  //(*Destroy(TelemetryFrame)
  //*)

}

void TelemetryFrame::OnClose(wxCloseEvent& event)
{
  OpenAVRc_SimulatorFrame *parent = wxDynamicCast(this->GetParent(), OpenAVRc_SimulatorFrame);
  if(parent)
    parent->EnableTelemetryMenu();
  Destroy();
}

void TelemetryFrame::WriteDatas()
{
  Tele_GPSLat = GPSLatDegres->GetValue() + (float)GPSLatMinute->GetValue()/1000000;
  Tele_GPSLong = GPSLongDEgres->GetValue() + (float)GPSLongMinute->GetValue()/1000000;
  Tele_GPSAlt = (float)GPSAltitude->GetValue()/10;
  Tele_GPSSpeed = (float)GPSSpeed->GetValue()/10;
  Tele_GPSCourse = (float)GPSCourse->GetValue()/100;
  Tele_GPSYear = GPSDate->GetValue().GetYear();
  Tele_GPSMonth = GPSDate->GetValue().GetMonth();
  Tele_GPSDay = GPSDate->GetValue().GetDay();
  Tele_GPSHour = GPSTime->GetValue().GetHour();
  Tele_GPSMinute = GPSTime->GetValue().GetMinute();
  Tele_GPSSecond = GPSTime->GetValue().GetSecond();

  Tele_AccelX = (float)AcclX->GetValue()/10;
  Tele_AccelY = (float)AcclY->GetValue()/10;
  Tele_AccelZ = (float)AcclZ->GetValue()/10;

  Tele_VarioAlt = (float)VarioAlt->GetValue()/10;
  Tele_VarioAccelH = (float)VarioHSpeed->GetValue()/10;

  Tele_RSSITx = RSSITx->GetValue();
  Tele_RSSIRx = RSSIRx->GetValue();

  Tele_Analog1 = (float)Analog1->GetValue()/100;
  Tele_Analog2 = (float)Analog2->GetValue()/100;
  Tele_Analog3 = (float)Analog3->GetValue()/100;
  Tele_Analog4 = (float)Analog4->GetValue()/100;

  Tele_BattVolt = (float)BattVolts->GetValue()/10;
  Tele_BattCurrent = (float)BattAmp->GetValue()/10;

  Tele_RPM = RPM->GetValue();
  Tele_Fuel = (float)FUEL->GetValue()/10;
  Tele_Temp1 = (float)Temp1->GetValue()/10;
  Tele_Temp2 = (float)Temp2->GetValue()/10;
  Tele_Airspeed = (float)AirSpeed->GetValue()/10;

  Tele_Cell1 = (float)Cell1->GetValue()/100;
  Tele_Cell2 = (float)Cell2->GetValue()/100;
  Tele_Cell3 = (float)Cell3->GetValue()/100;
  Tele_Cell4 = (float)Cell4->GetValue()/100;
  Tele_Cell5 = (float)Cell5->GetValue()/100;
  Tele_Cell6 = (float)Cell6->GetValue()/100;
  Tele_Cell7 = (float)Cell7->GetValue()/100;
  Tele_Cell8 = (float)Cell8->GetValue()/100;
  Tele_Cell9 = (float)Cell9->GetValue()/100;
  Tele_Cell10 = (float)Cell10->GetValue()/100;
  Tele_Cell11 = (float)Cell11->GetValue()/100;
  Tele_Cell12 = (float)Cell12->GetValue()/100;

  Ini_Changed = true;
}
void TelemetryFrame::OnTeleChange(wxSpinEvent& event)
{
  WriteDatas();
}

void TelemetryFrame::OnGPSDateTimeChanged(wxDateEvent& event)
{
  WriteDatas();
}

void TelemetryFrame::OnTeleScrollChanged(wxScrollEvent& event)
{
  WriteDatas();
}
