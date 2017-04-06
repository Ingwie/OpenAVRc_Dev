#include "ModelNameDialog.h"
#include "OpenAVRc_SimulatorMain.h"
#include <wx/msgdlg.h>
#include <wx/valtext.h>

//(*InternalHeaders(ModelNameDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(ModelNameDialog)
const long ModelNameDialog::ID_STATICBOX1 = wxNewId();
const long ModelNameDialog::ID_TEXTCTRLNEWNAME = wxNewId();
const long ModelNameDialog::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ModelNameDialog,wxDialog)
	//(*EventTable(ModelNameDialog)
	//*)
END_EVENT_TABLE()

ModelNameDialog::ModelNameDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
  wxTextValidator validator(wxFILTER_INCLUDE_CHAR_LIST);
  wxString valid_chars(wxT(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_-"));
  validator.SetCharIncludes(valid_chars);

	//(*Initialize(ModelNameDialog)
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxSize(181,67));
	Move(wxDefaultPosition);
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(0,0), wxSize(288,176), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	StaticBox1 = new wxStaticBox(Panel1, ID_STATICBOX1, _("Nom du modèle"), wxPoint(0,0), wxSize(176,64), 0, _T("ID_STATICBOX1"));
	TextCtrlnewName = new wxTextCtrl(Panel1, ID_TEXTCTRLNEWNAME, _("10 Chars Max"), wxPoint(40,24), wxDefaultSize, wxWANTS_CHARS, validator, _T("ID_TEXTCTRLNEWNAME"));
	TextCtrlnewName->SetMaxLength(10);
	Connect(ID_TEXTCTRLNEWNAME,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&ModelNameDialog::OnTextCtrlnewNameTextEnter);
	Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&ModelNameDialog::OnInit);
	//*)

  {
    wxIcon FrameIcon;
    SetIcon(wxICON(oavrc_icon));
  }
}

ModelNameDialog::~ModelNameDialog()
{
	//(*Destroy(ModelNameDialog)
	//*)
}

/*void ModelNameDialog::CheckEntryValues(wxString inString)
{
  std::string  x(inString);
  if (x.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_-+:*=? ") != std::string::npos){
      wxMessageBox(x +_(" :\nIl y a des caractères spéciaux non autorisés.\nVeuillez le changer.\nCaractères spéciaux autorisés : _-+:*=?"));
  }
}
*/

void ModelNameDialog::OnInit(wxInitDialogEvent& event)
{
 wxString oldName = ConvCharFwToWxstr(g_model.header.name, LEN_MODEL_NAME);
 TextCtrlnewName->SetValue(oldName);
}

void ModelNameDialog::OnTextCtrlnewNameTextEnter(wxCommandEvent& event)
{
  wxString newName;
  newName = TextCtrlnewName->GetValue();
  //CheckEntryValues(newName);
  ConvWxstrToCharFw(newName, g_model.header.name, LEN_MODEL_NAME);
  Close();
}
