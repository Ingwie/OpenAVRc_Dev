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


#ifndef GVARS_H
#define GVARS_H

//(*Headers(GvarsFrame)
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/frame.h>
#include <wx/timer.h>
//*)
#define wxDEFAULT_DIALOG_STYLE  (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)

class GvarsFrame: public wxFrame
{
	public:

		GvarsFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~GvarsFrame();

    GvarsFrame *GvFr;
    void PopulateGvarsFrame();

		//(*Declarations(GvarsFrame)
		wxTextCtrl* TextCtrlpersoPhase2;
		wxTextCtrl* TextCtrlpersoPhase5;
		wxTextCtrl* TextCtrlpersoGvar2;
		wxTextCtrl* TextCtrlpersoGvar3;
		wxTextCtrl* TextCtrlpersoPhase3;
		wxTextCtrl* TextCtrlpersoPhase1;
		wxGrid* GVARSGrid;
		wxPanel* Panel1;
		wxTextCtrl* TextCtrlpersoPhase4;
		wxTextCtrl* TextCtrlpersoGvar4;
		wxTextCtrl* TextCtrlpersoGvar1;
		wxTextCtrl* TextCtrlpersoPhase0;
		wxTextCtrl* TextCtrlpersoGvar5;
		wxTimer gvarsTimer;
		wxTextCtrl* TextCtrlpersoGvar6;
		//*)

	protected:

		//(*Identifiers(GvarsFrame)
		static const long ID_GRID1;
		static const long ID_TextCtrlpersoPhase0;
		static const long ID_TextCtrlpersoPhase1;
		static const long ID_TextCtrlpersoPhase2;
		static const long ID_TextCtrlpersoPhase3;
		static const long ID_TextCtrlpersoPhase4;
		static const long ID_TextCtrlpersoPhase5;
		static const long ID_TextCtrlpersoGvar1;
		static const long ID_TextCtrlpersoGvar2;
		static const long ID_TextCtrlpersoGvar3;
		static const long ID_TextCtrlpersoGvar4;
		static const long ID_TextCtrlpersoGvar5;
		static const long ID_TextCtrlpersoGvar6;
		static const long ID_PANEL1;
		static const long ID_TIMERGVARS;
		//*)

	private:

		//(*Handlers(GvarsFrame)
		void OnClose(wxCloseEvent& event);
		void OngvarsTimerTrigger(wxTimerEvent& event);
		void OnTextCtrlpersoGvar1TextEnter(wxCommandEvent& event);
		void OnTextCtrlpersoGvar2TextEnter(wxCommandEvent& event);
		void OnTextCtrlpersoGvar3TextEnter(wxCommandEvent& event);
		void OnTextCtrlpersoGvar4TextEnter(wxCommandEvent& event);
		void OnTextCtrlpersoGvar5TextEnter(wxCommandEvent& event);
		void OnTextCtrlpersoPhase0TextEnter(wxCommandEvent& event);
		void OnTextCtrlpersoPhase1TextEnter(wxCommandEvent& event);
		void OnTextCtrlpersoPhase2TextEnter(wxCommandEvent& event);
		void OnTextCtrlpersoPhase3TextEnter(wxCommandEvent& event);
		void OnTextCtrlpersoPhase4TextEnter(wxCommandEvent& event);
		void OnTextCtrlpersoPhase5TextEnter(wxCommandEvent& event);
		void OnTextCtrlpersoAllText(wxCommandEvent& event);
		void OnTextCtrlpersoGvar6TextEnter(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
