#ifndef MIXERFRAME_H
#define MIXERFRAME_H
//#include <wx/arrstr.h>

//(*Headers(MixerFrame)
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/arrstr.h>
//*)

class MixerFrame: public wxFrame
{
	public:

		MixerFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~MixerFrame();
		void FillMixerFrame();
    wxArrayString Src;
		//(*Declarations(MixerFrame)
		wxPanel* Mixer;
		wxTextCtrl* Mixerline1;
		//*)

	protected:

		//(*Identifiers(MixerFrame)
		static const long ID_TEXTCTRL1;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(MixerFrame)
		void OnChannelText(wxCommandEvent& event);
		void OnPanel1Paint(wxPaintEvent& event);
		void OnClose(wxCloseEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
