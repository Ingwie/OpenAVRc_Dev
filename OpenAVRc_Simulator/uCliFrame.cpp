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

void uCliFrame::SendToHwSerial()
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

  simu_udr1 = '\r';
  Serial1._rx_complete_irq();
  simu_udr1 = '\n';
  Serial1._rx_complete_irq();

  TextCtrl->AppendText("\n");
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
#define SEND()\
    SendToHwSerial();\
    TextCtrl->WriteText("OK");\
    SendToHwSerial()\

  wxColor color;
  if (simu_portb & OUT_B_BT_KEY)
    {
      color = *wxBLUE;  // Bluetooth dialog
    }
  else
    {
      color = *wxBLACK;
    }
  TextCtrl->SetForegroundColour(color);

  if (c != '\r')
    {
      TextCtrl->WriteText((char)c);
      LastPrompt = TextCtrl->GetLineText(TextCtrl->GetNumberOfLines()-1);
    }
  if ((c == '\n') && (simu_portb & OUT_B_BT_KEY) && (simu_portg & OUT_G_BT_ONOFF)) // Virtuel BT module ON and AT mode actived
    {
      wxString cmd = TextCtrl->GetLineText(TextCtrl->GetNumberOfLines()-2);

      if ((cmd == "AT") || (cmd == "AT+UART=115200,0,0") || (cmd == "AT+CLASS=0") || (cmd == "AT+INQM=0,4,4"))
        {
          TextCtrl->WriteText("OK");
          SendToHwSerial();
        }
      else if (cmd == "AT+NAME?")
        {
          TextCtrl->WriteText("+NAME:TOtoTAta");
          SEND();
        }
      else if (cmd == "AT+PSWD?")
        {
          TextCtrl->WriteText("+PIN:\"1234\"");
          SEND();
        }
      else if (cmd == "AT+RNAME?0000,00,000000")
        {
          TextCtrl->WriteText("+RNAME:RC-NAVY");
          SEND();
        }
      else if (cmd == "AT+RNAME?1111,11,111111")
        {
          TextCtrl->WriteText("+RNAME:INGWIE");
          SEND();
        }
      else if (cmd == "AT+RNAME?2222,22,222222")
        {
          TextCtrl->WriteText("+RNAME:PIERROTM");
          SEND();
        }
      else if (cmd == "AT+INQ")
        {
          TextCtrl->WriteText("+INQ:0000,00,000000");
          SendToHwSerial();
          TextCtrl->WriteText("+INQ:1111,11,111111");
          SendToHwSerial();
          TextCtrl->WriteText("+INQ:2222,22,222222");
          SEND();
        }
    }
}

void uCliFrame::OnTextCtrlTextEnter(wxCommandEvent& event)
{
  SendToHwSerial();
}
