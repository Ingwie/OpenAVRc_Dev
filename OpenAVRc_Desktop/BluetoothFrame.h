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

#ifndef BLUETOOTHFRAME_H
#define BLUETOOTHFRAME_H

#include "serial/tserial.h"

//(*Headers(BluetoothFrame)
#include <wx/combobox.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/timer.h>
//*)

class BluetoothFrame: public wxFrame
{
	public:

		BluetoothFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~BluetoothFrame();

		//(*Declarations(BluetoothFrame)
		wxComboBox* ComboBoxCom;
		wxPanel* Panel1;
		wxStaticBox* StaticBoxCom;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticTextFreeMem;
		wxTimer TimerRX;
		//*)

	protected:

		//(*Identifiers(BluetoothFrame)
		static const long ID_STATICBOX1;
		static const long ID_COMBOBOX1;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_PANEL1;
		static const long ID_TIMERRX;
		//*)

    Tserial *BTComPort;
    bool comIsValid;
    volatile bool timout;
    void DetectSerial();
    wxString uCLI;
    void ConnectBTCom(wxString name);
    void sendCmdAndWaitForResp(wxString BTcommand, wxString* BTanwser);
	private:

		//(*Handlers(BluetoothFrame)
		void OnClose(wxCloseEvent& event);
		void OnComboBoxComSelected(wxCommandEvent& event);
		void OnComboBoxComDropdown(wxCommandEvent& event);
		void OnTimerRXTrigger(wxTimerEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
