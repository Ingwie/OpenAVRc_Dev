#include "Voice_choice.h"

//(*InternalHeaders(Voice_choice)
#include <wx/intl.h>
#include <wx/string.h>
//*)

extern wxString AppPath;
extern long Numvoice;

//(*IdInit(Voice_choice)
const long Voice_choice::ID_BUTTONSAVEVOICE = wxNewId();
const long Voice_choice::ID_STATICTEXT1 = wxNewId();
const long Voice_choice::ID_COMBOBOXVOICE = wxNewId();
const long Voice_choice::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(Voice_choice,wxFrame)
	//(*EventTable(Voice_choice)
	//*)
END_EVENT_TABLE()

Voice_choice::Voice_choice(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(Voice_choice)
	Create(parent, wxID_ANY, _("Synthèse vocale"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxCLOSE_BOX, _T("wxID_ANY"));
	SetClientSize(wxSize(399,150));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(72,56), wxSize(399,160), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	Buttonsavevoice = new wxButton(Panel1, ID_BUTTONSAVEVOICE, _("Sauvegarder réglage"), wxPoint(248,112), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONSAVEVOICE"));
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Choisissez votre moteur de synthèse vocale"), wxPoint(88,24), wxSize(224,16), 0, _T("ID_STATICTEXT1"));
	ComboBoxvoice = new wxComboBox(Panel1, ID_COMBOBOXVOICE, wxEmptyString, wxPoint(32,56), wxSize(336,21), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOXVOICE"));

	Connect(ID_BUTTONSAVEVOICE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Voice_choice::OnButtonsavevoiceClick);
	//*)

	Load();
}

Voice_choice::~Voice_choice()
{
	//(*Destroy(Voice_choice)
	//*)
}

void Voice_choice::Load()
{
      wxTextFile tfile(AppPath + "\\voices.txt");
    if (tfile.Exists()) //avoid crash if file is not found
    {
        tfile.Open(AppPath + "\\voices.txt");
        for (int j = 0; j < tfile.GetLineCount(); j++ )
        {
          ComboBoxvoice->Append(tfile.GetLine(j));
        }
        tfile.Close();
    }
    ComboBoxvoice->SetSelection(Numvoice);
    Show();
}

void Voice_choice::OnButtonsavevoiceClick(wxCommandEvent& event)
{
  Numvoice = ComboBoxvoice->GetSelection();
  Close();
}
