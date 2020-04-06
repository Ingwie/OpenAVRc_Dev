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

#define START_TIMOUT() \
 timout = true;        \
TimerRX.StartOnce();


//(*InternalHeaders(BluetoothFrame)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(BluetoothFrame)
const long BluetoothFrame::ID_STATICBOX1 = wxNewId();
const long BluetoothFrame::ID_COMBOBOX1 = wxNewId();
const long BluetoothFrame::ID_STATICTEXT1 = wxNewId();
const long BluetoothFrame::ID_STATICTEXT2 = wxNewId();
const long BluetoothFrame::ID_STATICTEXT3 = wxNewId();
const long BluetoothFrame::ID_STATICTEXT4 = wxNewId();
const long BluetoothFrame::ID_REBOOTBUTTON = wxNewId();
const long BluetoothFrame::ID_STATICBOX2 = wxNewId();
const long BluetoothFrame::ID_STATICBOXSD = wxNewId();
const long BluetoothFrame::ID_TREECTRLSD = wxNewId();
const long BluetoothFrame::ID_PANEL1 = wxNewId();
const long BluetoothFrame::ID_TIMERRX = wxNewId();
//*)

BEGIN_EVENT_TABLE(BluetoothFrame,wxFrame)
	//(*EventTable(BluetoothFrame)
	//*)
END_EVENT_TABLE()

BluetoothFrame::BluetoothFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(BluetoothFrame)
	Create(parent, wxID_ANY, _("Bluetooth"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(645,409));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(392,176), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	StaticBoxCom = new wxStaticBox(Panel1, ID_STATICBOX1, _("Communication"), wxPoint(8,8), wxSize(392,88), 0, _T("ID_STATICBOX1"));
	ComboBoxCom = new wxComboBox(Panel1, ID_COMBOBOX1, wxEmptyString, wxPoint(64,32), wxSize(72,23), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX1"));
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Port :"), wxPoint(16,32), wxSize(40,16), wxALIGN_RIGHT, _T("ID_STATICTEXT1"));
	StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("Mémoire libre :"), wxPoint(160,32), wxSize(96,16), wxALIGN_RIGHT, _T("ID_STATICTEXT2"));
	StaticTextFreeMem = new wxStaticText(Panel1, ID_STATICTEXT3, _("------"), wxPoint(264,32), wxSize(80,16), wxALIGN_LEFT, _T("ID_STATICTEXT3"));
	StaticTextVersion = new wxStaticText(Panel1, ID_STATICTEXT4, wxEmptyString, wxPoint(24,64), wxSize(360,16), 0, _T("ID_STATICTEXT4"));
	BitmapButtonReboot = new wxBitmapButton(Panel1, ID_REBOOTBUTTON, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_WARNING")),wxART_BUTTON), wxPoint(344,24), wxSize(40,23), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_REBOOTBUTTON"));
	BitmapButtonReboot->SetToolTip(_("Reboot"));
	StaticBoxLocal1 = new wxStaticBox(Panel1, ID_STATICBOX2, _("Local"), wxPoint(8,104), wxSize(200,296), 0, _T("ID_STATICBOX2"));
	StaticBoxSD = new wxStaticBox(Panel1, ID_STATICBOXSD, _("Carte SD"), wxPoint(216,104), wxSize(200,296), 0, _T("ID_STATICBOXSD"));
	TctrlSd = new wxTreeCtrl(Panel1, ID_TREECTRLSD, wxPoint(224,128), wxSize(184,264), wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TREECTRLSD"));
	TimerRX.SetOwner(this, ID_TIMERRX);
	TimerRX.Start(200, true);

	Connect(ID_COMBOBOX1,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&BluetoothFrame::OnComboBoxComSelected);
	Connect(ID_COMBOBOX1,wxEVT_COMMAND_COMBOBOX_DROPDOWN,(wxObjectEventFunction)&BluetoothFrame::OnComboBoxComDropdown);
	Connect(ID_REBOOTBUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&BluetoothFrame::OnBitmapButtonRebootClick);
	Connect(ID_TIMERRX,wxEVT_TIMER,(wxObjectEventFunction)&BluetoothFrame::OnTimerRXTrigger);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&BluetoothFrame::OnClose);
	//*)

 {
  SetIcon(wxICON(oavrc_icon));
 }

 BTComPort = new Tserial();
 comIsValid = false;
 uCLI = "uCLI>";
}

BluetoothFrame::~BluetoothFrame()
{
	//(*Destroy(BluetoothFrame)
	//*)
}

void BluetoothFrame::OnClose(wxCloseEvent& event)
{
  if (BTComPort != NULL) delete BTComPort;
  OpenAVRc_DesktopFrame *parent = wxDynamicCast(this->GetParent(), OpenAVRc_DesktopFrame);
  if(parent)
    parent->EnableBluetoothSelectedMenu();
  Destroy();
}

void BluetoothFrame::DetectSerial()
{
  TCHAR Devices [5000];
  for(int i=0; i<255; i++) // checking ports from COM0 to COM255
  {
    wxString str;
    str = str.Format(wxT("%i"),i);
    wxString ComName = "COM"+str; // converting to COM0, COM1, COM2
    long test = QueryDosDevice(ComName.c_str(), Devices, 5000); //Win32(64) API only
    if (test!=0) //QueryDosDevice returns zero if it didn't find an object
    {
      ComboBoxCom->Insert(ComName,0); // add to the ComboBox
    }
  }
}

void BluetoothFrame::ConnectBTCom(wxString name)
{
  int error;
  char comMame[10];
  strncpy(comMame, (const char*)name.mb_str(wxConvUTF8), 10);
  assert(BTComPort);
  wxBusyCursor wait;
  error = BTComPort->connect(comMame, 115200, spNONE);
  if (error == 0) {
      comIsValid = true;
      StaticTextFreeMem->SetLabel(getRam());
      StaticTextFreeMem->Update();
      Sleep(20);
      StaticTextVersion->SetLabel(getVer());
      StaticTextVersion->Update();
      Sleep(20);
      Populate_SD();
  }
  else {
    wxString intString = wxString::Format(wxT("%i"), error);
    wxMessageBox("Erreur N°"+ intString + " port COM");
    }
}

void BluetoothFrame::OnComboBoxComDropdown(wxCommandEvent& event)
{
 BTComPort->disconnect();
 ComboBoxCom->Clear();
 StaticTextFreeMem->SetLabel("------");
 StaticTextFreeMem->Update();
 StaticTextVersion->SetLabel("");
 StaticTextVersion->Update();
 TctrlSd->DeleteAllItems();
 DetectSerial();
}

void BluetoothFrame::OnComboBoxComSelected(wxCommandEvent& event)
{
  ConnectBTCom(ComboBoxCom->GetValue());
}

void BluetoothFrame::OnTimerRXTrigger(wxTimerEvent& event)
{
  timout = false;
}

void BluetoothFrame::sendCmdAndWaitForResp(wxString BTcommand, wxString* BTanwser)
{
 if (comIsValid)
  {
   int16_t l = BTcommand.length();
   if (l != 0)
    {
     char cstring[40];
     strncpy(cstring, (const char*)BTcommand.mb_str(wxConvUTF8), l);
     char CRLF[2] = {'\r','\n'};
     BTComPort->sendArray(cstring, l); // Send BTcommand
     BTComPort->sendArray(CRLF, 2); // Send EOL+CR
     START_TIMOUT();
     wxBusyCursor wait;
     do
      {
       wxYieldIfNeeded();
      }
     while (timout);

      int Num = BTComPort->getNbrOfBytes();
     if (Num)
      {
       char buffer[Num+1];
       BTComPort->getArray(buffer, Num);
       *BTanwser = (const char*)(buffer);
       if(!(BTanwser->StartsWith(uCLI))) return;
       *BTanwser = BTanwser->Mid(uCLI.length()+BTcommand.length()+1); // remove uCLI> + command echo
      }
    }
  }
}

void BluetoothFrame::OnBitmapButtonRebootClick(wxCommandEvent& event)
{
 char Reboot[] = {'r','e','b','o','o','t','\r','\n'};
 BTComPort->sendArray(Reboot, sizeof(Reboot)); // Send BTcommand
 OnComboBoxComDropdown(event);
}

wxString BluetoothFrame::getRam()
{
 wxString ram;
 sendCmdAndWaitForResp("ram", &ram);
 ram.BeforeFirst('\r'); // remove all after \r (\n)
 return ram;
}

wxString BluetoothFrame::getVer()
{
 wxString ver;
 sendCmdAndWaitForResp("ver", &ver);
 ver.BeforeFirst('\r'); // remove all after \r (\n)
 return ver;
}

void BluetoothFrame::Populate_SD()
{
 wxTreeItemId rootId = TctrlSd->AddRoot("/");
 wxString dirEnt = "";
 sendCmdAndWaitForResp("ls /", &dirEnt); // [MODELS]\r\n[LOGS]\r\n[VOICE]\r\n
 dirEnt.Replace("\r\n", "\r", 1);
 START_TIMOUT(); // use timout to prevent errors
 wxString tmp = "";
 do
 {
   tmp = dirEnt.BeforeFirst('\r');
   dirEnt.BeforeFirst('\r',&dirEnt);
   TctrlSd->AppendItem(rootId, tmp);
 } while ((dirEnt != "") && timout);

 TctrlSd->Expand(rootId);
}


