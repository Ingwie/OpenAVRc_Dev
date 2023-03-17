 /*
 **************************************************************************
 *                                                                        *
 *                 ____                ___ _   _____                      *
 *                / __ \___  ___ ___  / _ | | / / _ \____                 *
 *               / /_/ / _ \/ -_) _ \/ __ | |/ / , _/ __/                 *
 *               \____/ .__/\__/_//_/_/ |_|___/_/|_|\__/                  *
 *                   /_/                                                  *
 *                                                                        *
 *              This file is part of the OpenAVRc project.                *
 *                                                                        *
 *                         Based on code(s) named :                       *
 *             OpenTx - https://github.com/opentx/opentx                  *
 *             Deviation - https://www.deviationtx.com/                   *
 *                                                                        *
 *                Only AVR code here for visibility ;-)                   *
 *                                                                        *
 *   OpenAVRc is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation, either version 2 of the License, or    *
 *   (at your option) any later version.                                  *
 *                                                                        *
 *   OpenAVRc is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *   GNU General Public License for more details.                         *
 *                                                                        *
 *       License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html          *
 *                                                                        *
 **************************************************************************
*/


#include "ModelNameDialog.h"
#include "OpenAVRc_SimulatorMain.h"
#include <wx/msgdlg.h>
#include <wx/valtext.h>

//(*InternalHeaders(ModelNameDialog)
#include <wx/frame.h>
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
  wxString valid_chars(wxT(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_-.,"));
  validator.SetCharIncludes(valid_chars);

	//(*Initialize(ModelNameDialog)
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxSize(181,67));
	Move(wxDefaultPosition);
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(0,0), wxSize(288,176), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	StaticBox1 = new wxStaticBox(Panel1, ID_STATICBOX1, _("Nom du mod�le"), wxPoint(0,0), wxSize(176,64), 0, _T("ID_STATICBOX1"));
	TextCtrlnewName = new wxTextCtrl(Panel1, ID_TEXTCTRLNEWNAME, _("10 Chars Max"), wxPoint(40,24), wxDefaultSize, wxWANTS_CHARS, validator, _T("ID_TEXTCTRLNEWNAME"));
	TextCtrlnewName->SetMaxLength(10);
	Connect(ID_TEXTCTRLNEWNAME,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&ModelNameDialog::OnTextCtrlnewNameTextEnter);
	Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&ModelNameDialog::OnInit);
	//*)

  {
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
      wxMessageBox(x +_(" :\nIl y a des caract�res sp�ciaux non autoris�s.\nVeuillez le changer.\nCaract�res sp�ciaux autoris�s : _-+:*=?"));
  }
}
*/

void ModelNameDialog::OnInit(wxInitDialogEvent& event)
{
 wxString oldName = ConvCharFwToWxstr(g_model.name, LEN_MODEL_NAME);
 TextCtrlnewName->SetValue(oldName);
}

void ModelNameDialog::OnTextCtrlnewNameTextEnter(wxCommandEvent& event)
{
  wxString newName;
  newName = TextCtrlnewName->GetValue();
  //CheckEntryValues(newName);
  ConvWxstrToCharFw(newName, g_model.name, LEN_MODEL_NAME);
  Close();
}
