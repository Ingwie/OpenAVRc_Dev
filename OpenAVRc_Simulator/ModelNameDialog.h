#ifndef MODELNAMEDIALOG_H
#define MODELNAMEDIALOG_H

//(*Headers(ModelNameDialog)
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/dialog.h>
//*)

class ModelNameDialog: public wxDialog
{
	public:

		ModelNameDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~ModelNameDialog();

		void CheckEntryValues(wxString);

		//(*Declarations(ModelNameDialog)
		wxPanel* Panel1;
		wxStaticBox* StaticBox1;
		wxTextCtrl* TextCtrlnewName;
		//*)

	protected:

		//(*Identifiers(ModelNameDialog)
		static const long ID_STATICBOX1;
		static const long ID_TEXTCTRLNEWNAME;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(ModelNameDialog)
		void OnInit(wxInitDialogEvent& event);
		void OnTextCtrlnewNameTextEnter(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
