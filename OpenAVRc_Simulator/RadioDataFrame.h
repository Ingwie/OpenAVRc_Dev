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


#ifndef RADIODATAFRAME_H
#define RADIODATAFRAME_H

#include "OpenAVRc_SimulatorApp.h"

//(*Headers(RadioDataFrame)
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
//*)
#define wxDEFAULT_DIALOG_STYLE  (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)

class RadioDataFrame: public wxFrame
{
	public:

		RadioDataFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~RadioDataFrame();

		void PopulateRadioDataFrame();

		RadioDataFrame *RaFr;

		//(*Declarations(RadioDataFrame)
		wxPanel* Panel1;
		wxStaticBox* StaticBox1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxTextCtrl* TextCtrlBatt;
		wxTextCtrl* TextCtrlcalibration;
		wxTextCtrl* TextCtrlcontrast;
		wxTextCtrl* TextCtrlcurrModel;
		wxTextCtrl* TextCtrlhapticMode;
		wxTextCtrl* TextCtrlstickMode;
		wxTextCtrl* TextCtrlversion;
		wxTimer RadioData;
		//*)

	protected:

		//(*Identifiers(RadioDataFrame)
		static const wxWindowID ID_STATICBOX1;
		static const wxWindowID ID_TEXTCTRLVERSION;
		static const wxWindowID ID_TEXTCTRL3;
		static const wxWindowID ID_TEXTCTRLBATT;
		static const wxWindowID ID_TEXTCTRLSTICKMODE;
		static const wxWindowID ID_TEXTCTRLCalibration;
		static const wxWindowID ID_STATICTEXT1;
		static const wxWindowID ID_STATICTEXT2;
		static const wxWindowID ID_TEXTCTRLCONTRAST;
		static const wxWindowID ID_TEXTCTRLHAPTICMODE;
		static const wxWindowID ID_PANEL1;
		static const wxWindowID ID_TIMERRADIODATA;
		//*)

	private:

		//(*Handlers(RadioDataFrame)
		void OnTextCtrl1Text(wxCommandEvent& event);
		void OnTextCtrlvariantText(wxCommandEvent& event);
		void OnPanel1Paint(wxPaintEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnRadioDataTrigger(wxTimerEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
