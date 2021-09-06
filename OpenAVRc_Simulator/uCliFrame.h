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

#ifndef UCLIFRAME_H
#define UCLIFRAME_H

//(*Headers(uCliFrame)
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
//*)

#if defined(USE_DDE_LINK)
#define wxUSE_DDE_FOR_IPC 0 // use Windows DDE
#include <wx/ipc.h>
#include <wx/utils.h>
#include <wx/msgdlg.h>
#include <wx/log.h>
#endif


#include "OpenAVRc_SimulatorMain.h"

#define wxDEFAULT_DIALOG_STYLE  (wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)

extern void USART_RX_vect_N(TLM_USART1)();

class uCliFrame: public wxFrame
{
public:
 uCliFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
 virtual ~uCliFrame();

 void HwSerialByte(uint8_t c);
 void SendToBtSerial();
 void DdeSendBufferIfNeeded();
 wxString LastPrompt;

#if defined(USE_DDE_LINK)
// DDE exchange
 void DdeLink();
 bool DdeConnectTo(wxString ExtServerName);
#endif

//(*Declarations(uCliFrame)
wxTextCtrl* TextCtrl;
wxTimer TimerBTRX;
//*)

protected:

//(*Identifiers(uCliFrame)
static const long ID_TEXTCTRL;
static const long ID_TIMERBTRX;
//*)

private:

//(*Handlers(uCliFrame)
 void OnClose(wxCloseEvent& event);
 void OnTextCtrlTextEnter(wxCommandEvent& event);
 void OnTimerBTRXTrigger(wxTimerEvent& event);
//*)

 DECLARE_EVENT_TABLE()
};

#if defined(USE_DDE_LINK)
class DdeConnectionOut: public wxConnection
{
public:
 DdeConnectionOut() {};
 ~DdeConnectionOut() {};
 //virtual bool OnDisconnect();
};

class DdeConnectionIn: public wxConnection
{
public:
 DdeConnectionIn(uCliFrame * tFrame) {UCliFrame = tFrame;};
 ~DdeConnectionIn() {};
 virtual bool OnPoke(const wxString &topic, const wxString &item, const void *data, size_t size, wxIPCFormat format);
private:
 uCliFrame * UCliFrame;
};

class DdeServer: public wxServer
{
public:
 DdeServer(uCliFrame * tFrame):wxServer() {UCliFrame = tFrame;};
 virtual wxConnectionBase * OnAcceptConnection(const wxString& topic);
private:
 uCliFrame * UCliFrame;
};

class DdeClient: public wxClient
{
public:
 virtual wxConnectionBase * OnMakeConnection(void)
 {
  return (wxConnectionBase *)new DdeConnectionOut;
 };
};
#endif

#endif
