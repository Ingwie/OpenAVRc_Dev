 /*
 **************************************************************************
 *                                                                        *
 *              This file is part of the OpenAVRc project.                *
 *                                                                        *
 *                         Based on code named                            *
 *             OpenTx - https://github.com/opentx/opentx                  *
 *                                                                        *
 *                Only AVR code here for lisibility ;-)                   *
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


#ifndef VOICE_CHOICE_H
#define VOICE_CHOICE_H

#include "OpenAVRc_DesktopMain.h"


//(*Headers(Voice_choice)
#include <wx/stattext.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/combobox.h>
//*)

class Voice_choice: public wxFrame
{
	public:

		Voice_choice(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~Voice_choice();

		//(*Declarations(Voice_choice)
		wxButton* Buttonsavevoice;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		wxComboBox* ComboBoxvoice;
		//*)

	protected:

		//(*Identifiers(Voice_choice)
		static const long ID_BUTTONSAVEVOICE;
		static const long ID_STATICTEXT1;
		static const long ID_COMBOBOXVOICE;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(Voice_choice)
		void OnButtonsavevoiceClick(wxCommandEvent& event);
		//*)
		void Load();

		DECLARE_EVENT_TABLE()
};

#endif
