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


#ifndef MIXERFRAME_H
#define MIXERFRAME_H

#include <wx/string.h>

wxString mixStr1, mixStr2, mixStr4, mixStr6, mixStr7, mixStr8, mixStr9, mixStr10, mixStr11, mixStr14, mixStr16,
mixStr18, mixStr19, mixStr20, mixStr22 ;



//(*Headers(MixerFrame)
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
//*)

class MixerFrame: public wxFrame
{
	public:

		MixerFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~MixerFrame();
    void FillMixerFrame();
    //wxArrayString Src;
    MixerFrame *MixFr;

		//(*Declarations(MixerFrame)
		wxPanel* Panel1;
		wxTextCtrl* Headerline;
		wxTextCtrl* Mixerline1;
		wxTimer TimerRefreshFrame;
		//*)

	protected:

		//(*Identifiers(MixerFrame)
		static const long ID_TEXTCTRLHEADERLINE;
		static const long ID_TEXTCTRLMIXERLINE;
		static const long ID_PANEL1;
		static const long ID_TIMERREFRESHFRAME;
		//*)

	private:

		//(*Handlers(MixerFrame)
		void OnClose(wxCloseEvent& event);
		void OnTimerRefreshFrameTrigger(wxTimerEvent& event);
		void OnTextCtrl1Text(wxCommandEvent& event);
		void OnTextCtrl1Text1(wxCommandEvent& event);
		void OnMixerline1Text(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
