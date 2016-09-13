#include "Panel_main.h"

//(*InternalHeaders(Panel_main)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(Panel_main)
//*)

BEGIN_EVENT_TABLE(Panel_main,wxPanel)
	//(*EventTable(Panel_main)
	//*)
END_EVENT_TABLE()

Panel_main::Panel_main(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(Panel_main)
	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	//*)
}

Panel_main::~Panel_main()
{
	//(*Destroy(Panel_main)
	//*)
}

