#ifndef MIXERFRAME_H
#define MIXERFRAME_H

#include <wx/string.h>

wxString mixStr1, mixStr2, mixStr4, mixStr6, mixStr7, mixStr8, mixStr9, mixStr10, mixStr11, mixStr14, mixStr16,
mixStr18, mixStr19, mixStr20, mixStr22 ;



//(*Headers(MixerFrame)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/frame.h>
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
		wxTimer TimerRefreshFrame;
		wxPanel* Panel1;
		wxTextCtrl* Headerline;
		wxTextCtrl* Mixerline1;
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
