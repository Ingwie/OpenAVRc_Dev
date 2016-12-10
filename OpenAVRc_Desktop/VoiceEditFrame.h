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
		wxGrid* VoiceGrid;
		wxPanel* Panel1;
		wxStaticBox* StaticBox1;
		wxButton* Retour;
		//*)

	protected:

		//(*Identifiers(VoiceEditFrame)
		static const long ID_STATICBOX1;
		static const long ID_GRID1;
		static const long ID_BUTTON1;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(VoiceEditFrame)
		void OnRetourClick(wxCommandEvent& event);
		void Load(); //Load textfile language
		void OnVoiceGridCellLeftDClick(wxGridEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
