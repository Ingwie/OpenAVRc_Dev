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

#include <wx/msgdlg.h>

#include "BluetoothFrame.h"
#include "OpenAVRc_DesktopMain.h"

//(*InternalHeaders(BluetoothFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(BluetoothFrame)
const long BluetoothFrame::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(BluetoothFrame,wxFrame)
	//(*EventTable(BluetoothFrame)
	//*)
END_EVENT_TABLE()

BluetoothFrame::BluetoothFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(BluetoothFrame)
	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(568,327));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(392,176), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));

	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&BluetoothFrame::OnClose);
	//*)

 {
  wxIcon FrameIcon;
  SetIcon(wxICON(oavrc_icon));
 }

}

BluetoothFrame::~BluetoothFrame()
{
	//(*Destroy(BluetoothFrame)
	//*)
}


void BluetoothFrame::OnClose(wxCloseEvent& event)
{
  OpenAVRc_DesktopFrame *parent = wxDynamicCast(this->GetParent(), OpenAVRc_DesktopFrame);
  if(parent)
    parent->EnableBluetoothSelectedMenu();
  Destroy();
}
