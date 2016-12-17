#ifndef SDVOICEFRAME_H
#define SDVOICEFRAME_H

//(*Headers(SDVoiceFrame)
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/combobox.h>
//*)

class SDVoiceFrame: public wxFrame
{
	public:

		SDVoiceFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~SDVoiceFrame();

		//(*Declarations(SDVoiceFrame)
		wxPanel* Panel1;
		wxStaticBox* StaticBox1;
		wxStaticBox* StaticBox2;
		wxButton* EnrgisterlacarteSD;
		wxComboBox* Drive;
		wxChoice* SoundCard;
		//*)

	protected:

		//(*Identifiers(SDVoiceFrame)
		static const long ID_STATICBOX2;
		static const long ID_STATICBOX1;
		static const long ID_BUTTON1;
		static const long ID_CHOICE1;
		static const long ID_COMBOBOX1;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(SDVoiceFrame)
		void OnChoice1Select(wxCommandEvent& event);
		void OnwhereSDSelect(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnSoundCardSelect(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
