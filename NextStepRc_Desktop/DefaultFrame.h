#ifndef DEFAULTFRAME_H
#define DEFAULTFRAME_H
#include <wx/combobox.h>
#include <wx/string.h>


//(*Headers(DefaultFrame)
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/combobox.h>
//*)


class DefaultFrame: public wxFrame
{
	public:

		DefaultFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~DefaultFrame();

		//(*Declarations(DefaultFrame)
		wxStaticText* StaticText2;
		wxButton* ButtonEnter;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxButton* ButtonSearchavrdudepath;
		wxButton* ButtonEsc;
		wxComboBox* ComboBox3;
		wxStaticText* StaticText5;
		wxButton* ButtonDetect;
		wxComboBox* ComboBox1;
		wxTextCtrl* TextCtrl1;
		wxComboBox* ComboBox2;
		//*)

	protected:

		//(*Identifiers(DefaultFrame)
		static const long ID_COMBOBOX1;
		static const long ID_COMBOBOX2;
		static const long ID_STATICTEXT1;
		static const long ID_BUTTONENTER;
		static const long ID_STATICTEXT2;
		static const long ID_COMBOBOX3;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT5;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTONESC;
		static const long ID_BUTTONSEARCHAVRDUDEPATH;
		static const long ID_BUTTONDETECT;
		//*)
		wxWindow * Parent;

	private:

		//(*Handlers(DefaultFrame)
		void OnButton1Click(wxCommandEvent& event);
		void OnButtonSearchavrdudepathClick(wxCommandEvent& event);
		void OnButtonEscClick(wxCommandEvent& event);
		void OnButtonEnterClick(wxCommandEvent& event);
		void OnButtonDetectClick(wxCommandEvent& event);
		void GetPortList(wxArrayString *port_list);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
