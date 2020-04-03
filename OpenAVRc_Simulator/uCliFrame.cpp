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

extern wxString BtSimuName;
extern wxString BtSimuPin;

extern bool Ini_Changed;

//BT
Tserial *BTComPort;
bool SimuBTComIsValid;
void ConnectBTCom(wxString name);
void SendByteBTCom(uint8_t data);

//(*InternalHeaders(uCliFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(uCliFrame)
const long uCliFrame::ID_TEXTCTRL = wxNewId();
const long uCliFrame::ID_TIMERBTRX = wxNewId();
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
	TimerBTRX.SetOwner(this, ID_TIMERBTRX);
	TimerBTRX.Start(20, false);

	Connect(ID_TEXTCTRL,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&uCliFrame::OnTextCtrlTextEnter);
	Connect(ID_TIMERBTRX,wxEVT_TIMER,(wxObjectEventFunction)&uCliFrame::OnTimerBTRXTrigger);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&uCliFrame::OnClose);
	//*)
  {
    wxIcon FrameIcon;
    SetIcon(wxICON(oavrc_icon));
  }
  LastPrompt = "";

  //BT
  BTComPort = new Tserial();
  char BTComNum[5] = {'C','O','M','7',0};
  assert(BTComPort);
  SimuBTComIsValid = (BTComPort->connect(BTComNum, 115200, spNONE) == 0);
}

uCliFrame::~uCliFrame()
{
	//(*Destroy(uCliFrame)
	//*)
  if (BTComPort != NULL) delete BTComPort;
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
  /*Destroy();*/
}

void uCliFrame::HwSerialByte(uint8_t c)
{
#define SEND()\
    SendToHwSerial();\
    TextCtrl->WriteText("OK");\
    SendToHwSerial()

  wxColor color;
  wxString inputValue = "";

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
 if (c == '\n') //for self test
  {
   wxString cmd = TextCtrl->GetLineText(TextCtrl->GetNumberOfLines()-2);
   if (cmd == "tf s3DCs5E4s5E6s5C5s5EBs5D1s61Ds5DC:76")
    {
     TextCtrl->WriteText("tf s3DCs5E4s5E6s5C5s5EBs5D1s61Ds5DC:76");
     SendToHwSerial();
    }
  }
 if ((c == '\n') && (SimuBTComIsValid) && (!(simu_portb & OUT_B_BT_KEY)))
  {
   wxString cmda = TextCtrl->GetLineText(TextCtrl->GetNumberOfLines()-2);
   int16_t l = cmda.length();
   if (l != 0)
    {
     char cstring[40];
     strncpy(cstring, (const char*)cmda.mb_str(wxConvUTF8), l);
     char CRLF[2] = {'\r','\n'};
     BTComPort->sendArray(cstring, l);
     BTComPort->sendArray(CRLF, 2);
    }
  }

 if ((c == '\n') && (simu_portb & OUT_B_BT_KEY) && !(simu_portg & OUT_G_BT_ONOFF)) // Virtual BT module ON and AT mode actived
    {
      wxString cmd = TextCtrl->GetLineText(TextCtrl->GetNumberOfLines()-2);

      if ((cmd == "AT") || (cmd == "AT+UART=115200,0,0") || (cmd == "AT+CLASS=0") || (cmd == "AT+INQM=0,4,4")
          || (cmd == "AT+IPSCAN=1024,1,1024,1") || (cmd == "AT+ROLE=1") || (cmd == "AT+ROLE=0") || (cmd == "AT+CMODE=0")
          || (cmd == "AT+RMAAD") || (cmd == "AT+INIT") || (cmd == "AT+RESET"))
        {
          TextCtrl->WriteText("OK");
          SendToHwSerial();
        }
      else if (cmd == "AT+NAME?")
        {
          TextCtrl->WriteText("+NAME:" + BtSimuName);
          SEND();
        }
      else if (cmd == "AT+PSWD?")
        {
          TextCtrl->WriteText("+PIN:\"" + BtSimuPin + "\"");
          SEND();
        }
      else if (cmd == "AT+RNAME?1234,56,789ABC")
        {
          TextCtrl->WriteText("+RNAME:RC-NAVY");
          SEND();
        }
      else if (cmd == "AT+RNAME?EFCB,AA,123456")
        {
          TextCtrl->WriteText("+RNAME:INGWIE");
          SEND();
        }
      else if (cmd == "AT+RNAME?01AB,22,000777")
        {
          TextCtrl->WriteText("+RNAME:PIERROTM");
          SEND();
        }
      else if (cmd == "AT+INQ")
        {
          TextCtrl->WriteText("+INQ:1234:56:789ABC,1C010C,7FFF");
          SendToHwSerial();
          TextCtrl->WriteText("+INQ:EFCB,AA,123456,1C010C,7FFF");
          SendToHwSerial();
          TextCtrl->WriteText("+INQ:1AB,22,00777,1C010C,7FFF");
          SEND();
        }
      else if ((cmd == "AT+LINK=1234,56,789ABC") || (cmd == "AT+LINK=EFCB,AA,123456") || (cmd == "AT+LINK=01AB,22,000777"))
        {
          TextCtrl->WriteText("OK");
          SendToHwSerial();
        }
      else if (cmd.StartsWith(("AT+NAME="), &inputValue))
        {
          BtSimuName = inputValue;
          TextCtrl->WriteText("OK");
          SendToHwSerial();
          Ini_Changed = true;
        }
      else if (cmd.StartsWith(("AT+PSWD=\""), &inputValue))
        {
          BtSimuPin = inputValue.Left(4);
          TextCtrl->WriteText("OK");
          SendToHwSerial();
          Ini_Changed = true;
        }
    }
}

void uCliFrame::OnTextCtrlTextEnter(wxCommandEvent& event)
{
  SendToHwSerial();
}

void uCliFrame::OnTimerBTRXTrigger(wxTimerEvent& event)
{
#define BFSZ 100
 if (SimuBTComIsValid)
  {
   int Num = BTComPort->getNbrOfBytes();
   if (Num)
    {
     //(Num > BFSZ)? Num = BFSZ : Num = Num;
     char buffer[Num+1];
     BTComPort->getArray(buffer, Num);
     for (int i=0; i <= Num; i++)
      {
       simu_udr1 = buffer[i];
       Serial1._rx_complete_irq();
       //HwSerialByte(buffer[i]);
      }
    }
  }
 else
  {
   TimerBTRX.Stop();
  }
}
