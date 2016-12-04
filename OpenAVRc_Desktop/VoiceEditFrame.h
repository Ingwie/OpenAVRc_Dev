#ifndef VOICEEDITFRAME_H
#define VOICEEDITFRAME_H

//(*Headers(VoiceEditFrame)
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/button.h>
#include <wx/frame.h>
//*)

extern wxString voice_Langue;

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
		void OnPanel1Paint(wxPaintEvent& event);
		void OnGrid1CellLeftClick(wxGridEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnRetourClick(wxCommandEvent& event);
		void OnVoiceGridCellChange(wxGridEvent& event);
		void OnVoiceGridKillFocus(wxFocusEvent& event);
		void OnVoiceGridCellChange1(wxGridEvent& event);
		void OnVoiceGridSetFocus(wxFocusEvent& event);
		void OnVoiceGridKillFocus1(wxFocusEvent& event);
		void Readpromptsfile(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
