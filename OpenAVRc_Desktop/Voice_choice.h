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
