#ifndef VOICEEDITFRAME_H
#define VOICEEDITFRAME_H

#include "OpenAVRc_DesktopMain.h"


//(*Headers(VoiceEditFrame)
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/button.h>
#include <wx/frame.h>
//*)


class VoiceEditFrame: public wxFrame
{
	public:

		VoiceEditFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~VoiceEditFrame();

		//(*Declarations(VoiceEditFrame)
		wxButton* ButtonGenerer;
		wxGrid* VoiceGrid;
		wxPanel* Panel1;
		wxStaticBox* StaticBox1;
		wxButton* ButtonSauvegarder;
		//*)

	protected:

		//(*Identifiers(VoiceEditFrame)
		static const long ID_STATICBOX1;
		static const long ID_GRID1;
		static const long ID_BUTTONSAUVEGARDER;
		static const long ID_BUTTONGENERER;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(VoiceEditFrame)
		void Load(); //Load textfile language
		void OnButtonSauvegarderClick(wxCommandEvent& event);
		void OnButtonGenererClick(wxCommandEvent& event);
		void OnVoiceGridCellRightClick(wxGridEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
