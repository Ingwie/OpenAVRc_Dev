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

#include "uCliFrame.h"

//(*InternalHeaders(uCliFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(uCliFrame)
const long uCliFrame::ID_TEXTCTRL = wxNewId();
//*)

BEGIN_EVENT_TABLE(uCliFrame,wxFrame)
	//(*EventTable(uCliFrame)
	//*)
END_EVENT_TABLE()

uCliFrame::uCliFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(uCliFrame)
	Create(parent, id, _("uCli"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	TextCtrl = new wxTextCtrl(this, ID_TEXTCTRL, wxEmptyString, wxPoint(224,320), wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_MULTILINE|wxDOUBLE_BORDER, wxDefaultValidator, _T("ID_TEXTCTRL"));

	Connect(ID_TEXTCTRL,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&uCliFrame::OnTextCtrlTextEnter);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&uCliFrame::OnClose);
	//*)
  {
    wxIcon FrameIcon;
    SetIcon(wxICON(oavrc_icon));
  }
  LastPrompt = "";
}

uCliFrame::~uCliFrame()
{
	//(*Destroy(uCliFrame)
	//*)
}


void uCliFrame::OnClose(wxCloseEvent& event)
{
  OpenAVRc_SimulatorFrame *parent = wxDynamicCast(this->GetParent(), OpenAVRc_SimulatorFrame);
  if(parent)
    parent->EnableuCliMenu();
  Destroy();
}

void uCliFrame::HwSerialByte(uint8_t c)
{
  TextCtrl->WriteText((char)c);
  int pos = TextCtrl->GetNumberOfLines();
  LastPrompt = TextCtrl->GetLineText(pos-1);

}

void uCliFrame::OnTextCtrlTextEnter(wxCommandEvent& event)
{
  int pos = TextCtrl->GetNumberOfLines();
  wxString txt = TextCtrl->GetLineText(pos-1);
  txt = txt.Right(txt.length()-LastPrompt.length());
  char cstring[100];
  strncpy(cstring, (const char*)txt.mb_str(wxConvUTF8), 99);

  for (int i=0; i < txt.length(); i++)
  {
    simu_udr1 = cstring[i];
    Serial1._rx_complete_irq();
  }

  simu_udr1 = '\n';
  Serial1._rx_complete_irq();

  TextCtrl->AppendText("\n");
}
