#ifndef GVARSFRAME_H
#define GVARSFRAME_H

//(*Headers(GVARSFrame)
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/combobox.h>
//*)

class GVARSFrame: public wxFrame
{
	public:

		GVARSFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~GVARSFrame();

		//(*Declarations(GVARSFrame)
		wxPanel* Panel1;
		wxComboBox* ComboBoxFlightMode0;
		//*)

	protected:

		//(*Identifiers(GVARSFrame)
		static const long ID_COMBOBOX1;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(GVARSFrame)
		void OnClose(wxCloseEvent& event);
		void OnClose1(wxCloseEvent& event);
		FillGvarsFrame();
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
