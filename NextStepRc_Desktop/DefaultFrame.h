#ifndef DEFAULTFRAME_H
#define DEFAULTFRAME_H
#include <wx/combobox.h>
#include <wx/string.h>

//wxString dude_programmer;
//wxString dude_port;

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
		wxButton* Button1;
		wxButton* Enter;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxButton* ButtonSearchavrdudepath;
		wxComboBox* ComboBox3;
		wxStaticText* StaticText5;
		wxComboBox* ComboBox1;
		wxTextCtrl* TextCtrl1;
		wxComboBox* ComboBox2;
		//*)

	protected:

		//(*Identifiers(DefaultFrame)
		static const long ID_COMBOBOX1;
		static const long ID_COMBOBOX2;
		static const long ID_STATICTEXT1;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT2;
		static const long ID_COMBOBOX3;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT5;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		//*)

	private:

		//(*Handlers(DefaultFrame)
		void OnComboBox1Selected(wxCommandEvent& event);
		void OnEnterClick(wxCommandEvent& event);
		void OnComboBox2Selected(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnCheckListBox1Toggled(wxCommandEvent& event);
		void OnEnterWindow(wxMouseEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnClose1(wxCloseEvent& event);
		void OnComboBox4Selected(wxCommandEvent& event);
		void OnPanel1Paint(wxPaintEvent& event);
		void OnTextCtrl1Text(wxCommandEvent& event);
		void OnComboBox2Selected1(wxCommandEvent& event);
		void OnComboBox3Selected(wxCommandEvent& event);
		void OnButton1Click1(wxCommandEvent& event);
		void OnButtonSearchavrdudepathClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
