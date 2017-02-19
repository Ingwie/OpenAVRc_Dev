#ifndef GVARS_H
#define GVARS_H

//(*Headers(GvarsFrame)
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/frame.h>
#include <wx/timer.h>
//*)

class GvarsFrame: public wxFrame
{
	public:

		GvarsFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~GvarsFrame();

    GvarsFrame *GvFr;
    void PopulateGvarsFrame();

		//(*Declarations(GvarsFrame)
		wxGrid* GVARSGrid;
		wxPanel* Panel1;
		wxTimer gvarsTimer;
		//*)

	protected:

		//(*Identifiers(GvarsFrame)
		static const long ID_GRID1;
		static const long ID_PANEL1;
		static const long ID_TIMERGVARS;
		//*)

	private:

		//(*Handlers(GvarsFrame)
		void OnPanel4Paint(wxPaintEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnGrid1CellLeftClick(wxGridEvent& event);
		void OnGrid1CellLeftClick1(wxGridEvent& event);
		void OngvarsTimerTrigger(wxTimerEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
