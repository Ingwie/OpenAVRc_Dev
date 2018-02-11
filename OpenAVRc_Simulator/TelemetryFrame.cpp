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
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(TelemetryFrame)
const long TelemetryFrame::ID_GPSBOX = wxNewId();
const long TelemetryFrame::ID_STATICBOX2 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL4 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL5 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL6 = wxNewId();
const long TelemetryFrame::ID_STATICBOX1 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT4 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT1 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT5 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT2 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT6 = wxNewId();
const long TelemetryFrame::ID_STATICTEXT3 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL1 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL2 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL3 = wxNewId();
const long TelemetryFrame::ID_STATICBOX3 = wxNewId();
const long TelemetryFrame::ID_SPINCTRL7 = wxNewId();
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
	SetClientSize(wxSize(1152,248));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(472,72), wxSize(1152,232), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	gpsbox = new wxStaticBox(Panel1, ID_GPSBOX, _("GPS"), wxPoint(8,8), wxSize(1136,96), wxDOUBLE_BORDER, _T("ID_GPSBOX"));
	StaticBox2 = new wxStaticBox(Panel1, ID_STATICBOX2, _("Latitude"), wxPoint(16,24), wxSize(192,72), wxRAISED_BORDER, _T("ID_STATICBOX2"));
	GPSLatSecond = new wxSpinCtrl(Panel1, ID_SPINCTRL4, _T("0"), wxPoint(136,64), wxSize(64,21), 0, 0, 60000, 0, _T("ID_SPINCTRL4"));
	GPSLatSecond->SetValue(_T("0"));
	GPSLatMinute = new wxSpinCtrl(Panel1, ID_SPINCTRL5, _T("0"), wxPoint(80,64), wxSize(48,21), 0, 0, 60, 0, _T("ID_SPINCTRL5"));
	GPSLatMinute->SetValue(_T("0"));
	GPSLatDegres = new wxSpinCtrl(Panel1, ID_SPINCTRL6, _T("0"), wxPoint(24,64), wxSize(48,21), 0, -90, 90, 0, _T("ID_SPINCTRL6"));
	GPSLatDegres->SetValue(_T("0"));
	StaticBox1 = new wxStaticBox(Panel1, ID_STATICBOX1, _("Longitude"), wxPoint(224,24), wxSize(192,72), wxRAISED_BORDER, _T("ID_STATICBOX1"));
	StaticText4 = new wxStaticText(Panel1, ID_STATICTEXT4, _("Degrés"), wxPoint(24,40), wxSize(37,13), 0, _T("ID_STATICTEXT4"));
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Degrés"), wxPoint(232,40), wxSize(37,13), 0, _T("ID_STATICTEXT1"));
	StaticText5 = new wxStaticText(Panel1, ID_STATICTEXT5, _("Minutes"), wxPoint(80,40), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("Minutes"), wxPoint(288,40), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	StaticText6 = new wxStaticText(Panel1, ID_STATICTEXT6, _("Secondes"), wxPoint(144,40), wxSize(48,13), 0, _T("ID_STATICTEXT6"));
	StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("Secondes"), wxPoint(352,40), wxSize(48,13), 0, _T("ID_STATICTEXT3"));
	GPSLongDEgres = new wxSpinCtrl(Panel1, ID_SPINCTRL1, _T("0"), wxPoint(232,64), wxSize(48,21), 0, -180, 180, 0, _T("ID_SPINCTRL1"));
	GPSLongDEgres->SetValue(_T("0"));
	GPSLongMinute = new wxSpinCtrl(Panel1, ID_SPINCTRL2, _T("0"), wxPoint(288,64), wxSize(48,21), 0, 0, 60, 0, _T("ID_SPINCTRL2"));
	GPSLongMinute->SetValue(_T("0"));
	GPSLongSecond = new wxSpinCtrl(Panel1, ID_SPINCTRL3, _T("0"), wxPoint(344,64), wxSize(64,21), 0, 0, 60000, 0, _T("ID_SPINCTRL3"));
	GPSLongSecond->SetValue(_T("0"));
	StaticBox3 = new wxStaticBox(Panel1, ID_STATICBOX3, _("Altitude"), wxPoint(424,32), wxSize(88,64), 0, _T("ID_STATICBOX3"));
	GPSAltitude = new wxSpinCtrl(Panel1, ID_SPINCTRL7, _T("0"), wxPoint(432,64), wxSize(64,21), 0, -100, 1000, 0, _T("ID_SPINCTRL7"));
	GPSAltitude->SetValue(_T("0"));

	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&TelemetryFrame::OnClose);
	//*)

  {
    wxIcon FrameIcon;
    SetIcon(wxICON(oavrc_icon));
  }
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
