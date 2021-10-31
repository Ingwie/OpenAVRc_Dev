#ifndef XANYCONFIGURATORFRAME_H
#define XANYCONFIGURATORFRAME_H

//(*Headers(XanyConfiguratorFrame)
#include <wx/frame.h>
#include <wx/grid.h>
#include <wx/statbox.h>
//*)

class XanyConfiguratorFrame: public wxFrame
{
	public:

		XanyConfiguratorFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~XanyConfiguratorFrame();

		//(*Declarations(XanyConfiguratorFrame)
		wxGrid* Grid1;
		wxStaticBox* StaticBoxXanyConfig;
		//*)

	protected:

		//(*Identifiers(XanyConfiguratorFrame)
		static const long ID_STATICBOX;
		static const long ID_GRID1;
		//*)

	private:

		//(*Handlers(XanyConfiguratorFrame)
		void OnListBox1Select(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnGrid1CellLeftClick(wxGridEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
