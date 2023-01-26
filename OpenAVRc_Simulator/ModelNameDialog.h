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


#ifndef MODELNAMEDIALOG_H
#define MODELNAMEDIALOG_H

//(*Headers(ModelNameDialog)
#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/statbox.h>
#include <wx/textctrl.h>
//*)

#define wxDEFAULT_DIALOG_STYLE  (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)

class ModelNameDialog: public wxDialog
{
	public:

		ModelNameDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~ModelNameDialog();

		void CheckEntryValues(wxString);

		//(*Declarations(ModelNameDialog)
		wxPanel* Panel1;
		wxStaticBox* StaticBox1;
		wxTextCtrl* TextCtrlnewName;
		//*)

	protected:

		//(*Identifiers(ModelNameDialog)
		static const long ID_STATICBOX1;
		static const long ID_TEXTCTRLNEWNAME;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(ModelNameDialog)
		void OnInit(wxInitDialogEvent& event);
		void OnTextCtrlnewNameTextEnter(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
