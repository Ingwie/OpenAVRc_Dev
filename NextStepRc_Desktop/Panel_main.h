#ifndef PANEL_MAIN_H
#define PANEL_MAIN_H

//(*Headers(Panel_main)
#include <wx/panel.h>
//*)

class Panel_main: public wxPanel
{
	public:

		Panel_main(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~Panel_main();

		//(*Declarations(Panel_main)
		//*)

	protected:

		//(*Identifiers(Panel_main)
		//*)

	private:

		//(*Handlers(Panel_main)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
