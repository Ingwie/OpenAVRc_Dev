#ifndef MODELSFRAME_H
#define MODELSFRAME_H

//(*Headers(ModelsFrame)
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/frame.h>
//*)

class ModelsFrame: public wxFrame
{
	public:

		ModelsFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~ModelsFrame();

		void PopulateModelsFrame();

		//(*Declarations(ModelsFrame)
		wxPanel* Panel1;
		wxGrid* ModelsGrid;
		//*)

	protected:

		//(*Identifiers(ModelsFrame)
		static const long ID_MODELSGRID;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(ModelsFrame)
		void OnClose(wxCloseEvent& event);
		void OnModelsGridCellChange(wxGridEvent& event);
		void OnModelsGridCellLeftClick(wxGridEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
