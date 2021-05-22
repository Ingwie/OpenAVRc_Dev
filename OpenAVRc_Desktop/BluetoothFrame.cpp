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
#include "../OpenAVRc/thirdparty/xmodem/xmodem.cpp"

#define SD_ROOT ("/")

#define START_TIMOUT() \
 timout = true;        \
 TimerRX.StartOnce(350);

#define IS_SD_DIR(x)       \
 ((x.StartsWith("[")) && (x.EndsWith("]"))) // x is a wxString

#define IS_SD_ROOT(x)       \
 (x == SD_ROOT) // x is a wxString

Tserial *BTComPort;
//popu menu ID
enum MenuIDs {POPUP_ID_DELETE = wxID_HIGHEST + 1, POPUP_ID_CREATE_REPERTORY};


extern wxString AppPath;
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
const long BluetoothFrame::ID_GENERICDIRCTRL1 = wxNewId();
const long BluetoothFrame::ID_BITMAPBUTTONREFRESH = wxNewId();
const long BluetoothFrame::ID_GAUGE = wxNewId();
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
 StaticBoxCom = new wxStaticBox(Panel1, ID_STATICBOX1, _("Communication"), wxPoint(8,8), wxSize(624,88), 0, _T("ID_STATICBOX1"));
 ComboBoxCom = new wxComboBox(Panel1, ID_COMBOBOX1, wxEmptyString, wxPoint(64,32), wxSize(72,23), 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX1"));
 StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Port :"), wxPoint(16,32), wxSize(40,16), wxALIGN_RIGHT, _T("ID_STATICTEXT1"));
 StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("Mémoire libre :"), wxPoint(160,32), wxSize(96,16), wxALIGN_RIGHT, _T("ID_STATICTEXT2"));
 StaticTextFreeMem = new wxStaticText(Panel1, ID_STATICTEXT3, _("------"), wxPoint(264,32), wxSize(56,16), wxALIGN_LEFT, _T("ID_STATICTEXT3"));
 StaticTextVersion = new wxStaticText(Panel1, ID_STATICTEXT4, wxEmptyString, wxPoint(24,64), wxSize(360,16), 0, _T("ID_STATICTEXT4"));
 BitmapButtonReboot = new wxBitmapButton(Panel1, ID_REBOOTBUTTON, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_WARNING")),wxART_BUTTON), wxPoint(568,24), wxSize(48,24), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_REBOOTBUTTON"));
 BitmapButtonReboot->SetToolTip(_("Redémarrer"));
 StaticBoxLocal1 = new wxStaticBox(Panel1, ID_STATICBOX2, _("Local"), wxPoint(8,104), wxSize(216,296), 0, _T("ID_STATICBOX2"));
 StaticBoxSD = new wxStaticBox(Panel1, ID_STATICBOXSD, _("Carte SD"), wxPoint(232,104), wxSize(216,296), 0, _T("ID_STATICBOXSD"));
 TctrlSd = new wxTreeCtrl(Panel1, ID_TREECTRLSD, wxPoint(240,120), wxSize(200,272), wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TREECTRLSD"));
 DirCtrl = new wxGenericDirCtrl(Panel1, ID_GENERICDIRCTRL1, wxEmptyString, wxPoint(16,120), wxSize(200,272), 0, wxEmptyString, 0, _T("ID_GENERICDIRCTRL1"));
 BitmapButtonRefresh = new wxBitmapButton(Panel1, ID_BITMAPBUTTONREFRESH, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_REDO")),wxART_BUTTON), wxPoint(568,56), wxSize(48,24), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTONREFRESH"));
 BitmapButtonRefresh->SetToolTip(_("Rafraichir"));
 Gauge = new wxGauge(Panel1, ID_GAUGE, 100, wxPoint(328,24), wxSize(224,16), 0, wxDefaultValidator, _T("ID_GAUGE"));
 TimerRX.SetOwner(this, ID_TIMERRX);
 TimerRX.Start(200, true);

 Connect(ID_COMBOBOX1,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&BluetoothFrame::OnComboBoxComSelected);
 Connect(ID_COMBOBOX1,wxEVT_COMMAND_COMBOBOX_DROPDOWN,(wxObjectEventFunction)&BluetoothFrame::OnComboBoxComDropdown);
 Connect(ID_REBOOTBUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&BluetoothFrame::OnBitmapButtonRebootClick);
 Connect(ID_TREECTRLSD,wxEVT_COMMAND_TREE_BEGIN_DRAG,(wxObjectEventFunction)&BluetoothFrame::OnTctrlSdBeginDrag);
 Connect(ID_TREECTRLSD,wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,(wxObjectEventFunction)&BluetoothFrame::OnTctrlSdItemRightClick);
 Connect(ID_BITMAPBUTTONREFRESH,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&BluetoothFrame::OnBitmapButtonRefreshClick);
 Connect(ID_TIMERRX,wxEVT_TIMER,(wxObjectEventFunction)&BluetoothFrame::OnTimerRXTrigger);
 Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&BluetoothFrame::OnClose);
//*)

 {
  SetIcon(wxICON(oavrc_icon));
 }

 Connect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&BluetoothFrame::OnSdPopupChoice);
 DirCtrl->Connect(wxID_ANY, wxEVT_TREE_BEGIN_DRAG, wxTreeEventHandler(BluetoothFrame::OnDirCtrlBeginDrag), NULL, this);
 DirCtrl->SetPath(AppPath + "\\SD\\");
 BTComPort = new Tserial();
 comIsValid = false;
 uCLI = "uCLI>";

 DnD_TctrlSd_Txt * txtTctrlSdDropTarget = new DnD_TctrlSd_Txt(this);
 txtTctrlSdDropTarget->SetDataObject(new wxTextDataObject());
 TctrlSd->SetDropTarget(txtTctrlSdDropTarget); //Enable droping objects on SD

 DnD_DirCtrl_Txt * txtDirCtrlDropTarget = new DnD_DirCtrl_Txt(this);
 txtDirCtrlDropTarget->SetDataObject(new wxTextDataObject());
 DirCtrl->SetDropTarget(txtDirCtrlDropTarget); //Enable droping objects on HDD
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
 if (error == 0)
  {
   comIsValid = true;
   Gauge->SetRange(100);
   Gauge->SetValue(0);
   getAndShowRam();
   Gauge->SetValue(10);
   getAndShowVer();
   Gauge->SetValue(20);
   Populate_SD();
  }
 else
  {
   wxString intString = wxString::Format(wxT("%i"), error);
   wxMessageBox("Erreur N°"+ intString + " port COM");
  }
}

void BluetoothFrame::OnComboBoxComDropdown(wxCommandEvent& event)
{
 Gauge->Pulse();
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

wxString BluetoothFrame::sendCmdAndWaitForResp(wxString BTcommand, wxString* BTanwser)
{
 if (comIsValid)
  {
   BTComPort->flush();  // flush buffer

   int16_t l = BTcommand.length();
   if (l != 0)
    {
     char cstring[40];
     strncpy(cstring, (const char*)BTcommand.mb_str(wxConvUTF8), l);
     char CRLF[2] = {'\r','\n'};
     BTComPort->sendArray(cstring, l); // Send uCli command
     BTComPort->sendArray(CRLF, 2); // Send EOL+CR
     wxBusyCursor wait;
     int Num = BTComPort->getNbrOfBytes();

     for( int i=0; i<10; ++i)
      {
       START_TIMOUT();
       do
        {
         wxYieldIfNeeded();
        }
       while (timout);
       int newNum = BTComPort->getNbrOfBytes();
       if (newNum > Num) Num = newNum;
       else break;
      }
     if (Num)
      {
       char buffer[Num+1] = {0};
       BTComPort->getArray(buffer, Num);
       *BTanwser = (const char*)(buffer);
       if(!(BTanwser->StartsWith(uCLI))) return "ERR";
       BTcommand = BTanwser->BeforeFirst(wxUniChar('\r'));
       BTcommand = BTcommand.AfterFirst(wxUniChar('>'));
       *BTanwser = BTanwser->AfterFirst(wxUniChar('\n'));
       Sleep(200);
       return BTcommand;
      }
    }
  }
 return "ERR";
}

void BluetoothFrame::OnBitmapButtonRebootClick(wxCommandEvent& event)
{
 char Reboot[] = {'r','e','b','o','o','t','\r','\n'};
 BTComPort->sendArray(Reboot, sizeof(Reboot)); // Send BTcommand
 OnComboBoxComDropdown(event);
}

void BluetoothFrame::OnBitmapButtonRefreshClick(wxCommandEvent& event)
{
 if (comIsValid)
  {
   wxBusyCursor wait;
   Gauge->SetRange(100);
   Gauge->SetValue(0);
   getAndShowRam();
   Gauge->SetValue(10);
   Populate_SD();
   Gauge->SetValue(100);
  }
}

wxString BluetoothFrame::getAndShowRam()
{
 wxString ram;
 sendCmdAndWaitForResp("ram", &ram);
 ram.BeforeFirst('\r'); // remove all after \r (\n)
 StaticTextFreeMem->SetLabel(ram);
 StaticTextFreeMem->Update();
 return ram;
}

wxString BluetoothFrame::getAndShowVer()
{
 wxString ver;
 sendCmdAndWaitForResp("ver", &ver);
 ver.BeforeFirst('\r'); // remove all after \r (\n)
 ver.Replace("\036"," ");
 ver.Replace("\037"," ");
 ver.Replace("\033"," ");
 StaticTextVersion->SetLabel(ver);
 StaticTextVersion->Update();
 return ver;
}

void BluetoothFrame::Populate_Dir(wxTreeItemId * dir)
{
 wxString sourceName = TctrlSd->GetItemText(*dir);
 if IS_SD_DIR(sourceName)
  {
   sourceName.Replace("[","/");
   sourceName.Replace("]","/");
  }
 wxString dirEnt = "";
 sendCmdAndWaitForResp("ls " + sourceName, &dirEnt); // [MODELS]\r\n[LOGS]\r\n[VOICE]\r\n
 dirEnt.Replace("\r\n", "\r", 1);
 wxString tmp = "";
 do
  {
   tmp = dirEnt.BeforeFirst('\r');
   dirEnt.BeforeFirst('\r',&dirEnt);
   TctrlSd->AppendItem(*dir, tmp);
  }
 while (dirEnt != "");
}

void BluetoothFrame::Populate_SD()
{
 TctrlSd->DeleteAllItems(); // first reset all
 wxTreeItemId rootId = TctrlSd->AddRoot(SD_ROOT);
 wxBusyCursor wait;
 Populate_Dir(&rootId);
 Gauge->SetValue(50);
 wxTreeItemIdValue cookie;
 wxTreeItemId child = TctrlSd->GetFirstChild(rootId, cookie);

 while (child.IsOk())
  {
   if IS_SD_DIR(TctrlSd->GetItemText(child))
    {
     Populate_Dir(&child);
    }
   child = TctrlSd->GetNextSibling(child);
  }
 Gauge->SetValue(100);
 TctrlSd->Expand(rootId);
}

wxString BluetoothFrame::GetFullPathTctrlItem(wxTreeItemId item)
{
 wxString path = "";
 wxTreeItemId root = TctrlSd->GetRootItem();
 if (item != root)
  {
   if (item.IsOk())
    {
     path = TctrlSd->GetItemText(item);
     if IS_SD_DIR(path)
      {
       path.Replace("[","");
       path.Replace("]","/");
      }
     wxTreeItemId tmp = item;
     wxString tmpPath;
     do
      {
       tmp = TctrlSd->GetItemParent(tmp);
       if (tmp.IsOk())
        {
         tmpPath = TctrlSd->GetItemText(tmp);
        }
       if IS_SD_DIR(tmpPath)
        {
         tmpPath.Replace("[","");
         tmpPath.Replace("]","/");
        }
       path = tmpPath + path;
      }
     while (tmp != root);
    }
   return path;
  }
 return SD_ROOT;
}

void BluetoothFrame::OnTctrlSdBeginDrag(wxTreeEvent& event) // SD drag
{
 wxTreeItemId item = event.GetItem();
 if (item.IsOk())
  {
   wxTextDataObject dragData(GetFullPathTctrlItem(item));
   if (dragData.GetTextLength())
    {
     dragSource.SetData(dragData);
     dragResult = dragSource.DoDragDrop(true);
    }
  }
}

void BluetoothFrame::OnDirCtrlBeginDrag(wxTreeEvent& event) // HDD Drag
{
 wxTreeItemId item = event.GetItem();
 if (item.IsOk())
  {
   DirCtrl->GetTreeCtrl()->SetFocusedItem(item);
   wxString path = DirCtrl->GetFilePath();
   if (path != "")  // Drag only files
    {
     wxTextDataObject dragData(path);
     if (dragData.GetTextLength())
      {
       dragSource.SetData(dragData);
       dragResult = dragSource.DoDragDrop(true);
      }
    }
  }
}

void BluetoothFrame::SdToSdCpy(wxString dest, wxString file)
{
 if (comIsValid)
  {
   wxBusyCursor wait;
   if IS_SD_DIR(dest)
    {
     dest.Replace("[","/");
     dest.Replace("]","/");
    }
   wxString uCliCommand = "cp SD" + file + " SD" + dest + file.AfterLast('/');
//wxMessageBox(uCliCommand);
   wxString BTanwser = "";
   sendCmdAndWaitForResp(uCliCommand, &BTanwser);
//wxMessageBox(BTanwser);
   Gauge->SetValue(0);
   Gauge->SetRange(100);
   Sleep(200);
   Populate_SD();
  }
}

void BluetoothFrame::HddToSdCpy(wxString dest, wxString file)
{
 if (comIsValid)
  {
   wxBusyCursor wait;
   if IS_SD_DIR(dest)
    {
     dest.Replace("[","/");
     dest.Replace("]","/");
    }
   wxString file2 = file.AfterLast('\\');
   file2.Replace(" ","_");
   wxString uCliCommand = "cp xmdm SD" + dest + file2;
//wxMessageBox(uCliCommand);
   wxString BTanwser = "";
   wxString retVal = sendCmdAndWaitForResp(uCliCommand, &BTanwser);
   Sleep(200);
   Set_BluetoothFrame_Gauge_Pointer(Gauge);
   int ret = XSend(file.c_str());
   if (retVal == "-8") wxMessageBox(_("Le fichier existe déjà"));
   if (ret) wxMessageBox(wxString::Format(wxT("%i"),ret));
   Gauge->SetValue(0);
   Gauge->SetRange(100);
   Sleep(200);
   Populate_SD();
  }
}

void BluetoothFrame::SDToHddCpy(wxString dest, wxString file)
{
 if (comIsValid)
  {
   wxBusyCursor wait;
   if IS_SD_DIR(file)
    {
     file.Replace("[","/");
     file.Replace("]","/");
    }
   wxString uCliCommand = "cp SD" + file + " xmdm";
//wxMessageBox(uCliCommand);
   wxString BTanwser = "";
   wxString retVal = sendCmdAndWaitForResp(uCliCommand, &BTanwser);
   Sleep(200);
   if (wxDirExists(dest)) // this is a dir ?
    {
     dest += "\\";
    }
   else if (wxFileExists(dest)) // this this a file ?
    {
     dest = dest.BeforeLast('\\'); // remove the file name to keep the dir.
     dest += "\\";
    }
   else // this is a drive
    {
     dest += "\\";
    }
   dest += file.AfterLast('/');
   Set_BluetoothFrame_Gauge_Pointer(Gauge);
   int ret = XReceive(dest.c_str());
//wxMessageBox(retVal);
   if (ret) wxMessageBox(wxString::Format(wxT("%i"),ret));
   Gauge->SetValue(0);
   Gauge->SetRange(100);
   DirCtrl->ReCreateTree();
  }
}

bool DnD_TctrlSd_Txt::OnDropText(wxCoord x, wxCoord y, const wxString& text) // SD Drop
{
 wxPoint point(x,y);
 int flag = wxTREE_HITTEST_ABOVE | wxTREE_HITTEST_ONITEMLABEL;
 wxTreeItemId dest = BluetoothFrame->TctrlSd->HitTest(point, flag);
 if (dest.IsOk())
  {
   wxString destName = BluetoothFrame->TctrlSd->GetItemText(dest);
   if (!(IS_SD_DIR(destName) || IS_SD_ROOT(destName)))
    {
     dest = BluetoothFrame->TctrlSd->GetItemParent(dest);
    }
   if (dest.IsOk())
    {
     wxString destName = BluetoothFrame->TctrlSd->GetItemText(dest);
     if (text.StartsWith(SD_ROOT)) // Sender is SD
      {
       BluetoothFrame->SdToSdCpy(destName, text);
      }
     else
      {
       BluetoothFrame->HddToSdCpy(destName, text);
      }
    }
   //BluetoothFrame->TctrlSd->AppendItem(dest, text);
   return true;
  }
 return false;
}

bool DnD_DirCtrl_Txt::OnDropText(wxCoord x, wxCoord y, const wxString& text) // HDD Drop
{
 wxPoint point(x,y);
 int flag = wxTREE_HITTEST_ABOVE | wxTREE_HITTEST_ONITEMLABEL;
 wxTreeItemId dest = BluetoothFrame->DirCtrl->GetTreeCtrl()->HitTest(point, flag);
 if (dest.IsOk())
  {
   wxString path = BluetoothFrame->DirCtrl->GetPath(dest);
   if (text.StartsWith(SD_ROOT)) // Verify sender is SD
    {
     BluetoothFrame->SDToHddCpy(path, text);
     //BluetoothFrame->DirCtrl->GetTreeCtrl()->AppendItem(dest, text);
    }
   return true;
  }
 return false;
}

void BluetoothFrame::OnTctrlSdItemRightClick(wxTreeEvent& event)
{
 wxTreeItemId item = event.GetItem();
 if (item.IsOk())
  {
   TctrlSd->SelectItem(item);
   wxString destName = TctrlSd->GetItemText(item);
   wxMenu pop;
   pop.Append(POPUP_ID_DELETE, _("Supprimer"));
   if (IS_SD_ROOT(destName))
    {
     pop.Append(POPUP_ID_CREATE_REPERTORY, _("Créer un répertoire")); // We don't support sub dir.
    }
   PopupMenu(&pop);
  }
}

void BluetoothFrame::SdDeleteFile(wxString file)
{
 if ((file != SD_ROOT) && (file != ""))
  {
   wxString BTanwser = "";
   //wxMessageBox(file);
   wxString retVal = sendCmdAndWaitForResp("rm SD" + file, &BTanwser);
   //wxMessageBox(BTanwser);
   //wxMessageBox(retVal);
   Sleep(200);
   Populate_SD();
  }
}

void BluetoothFrame::OnSdPopupChoice(wxCommandEvent& event)
{
 wxTreeItemId item = TctrlSd->GetSelection();
 if (item.IsOk())
  {
   wxString itemText = GetFullPathTctrlItem(item);
   switch (event.GetId())
    {
    case POPUP_ID_DELETE:
    {
     SdDeleteFile(itemText);
    }
    break;
    case POPUP_ID_CREATE_REPERTORY:
     wxMessageBox("md");
     break;
    }
  }
}

///////// XMODEM FILES OPERATIONS   /////////

ReusableBuffer ReBuff;

write_file(wxFile* fd, const uint8_t* buffer, int buffer_len)
{
 int ret = 0;

 ret = fd->Write(buffer, buffer_len);
 if (fd->Flush())
  {
   return ret;
  }
 return 0;
}

int seek_file(wxFile* fd, int32_t* offset, uint8_t whence)
{
 int ret = 0;

 if (fd->Seek(*offset,(wxSeekMode)whence) != wxInvalidOffset)
  {
   ret = 1;
  }
 return ret;
}

int read_file(wxFile* fd, uint8_t* buffer, uintptr_t buffer_len)
{
 return (int)fd->Read(buffer,buffer_len);
}

int FileExists(char * FullFileName)
{
 return wxFile::Exists(wxString::FromUTF8(FullFileName));
}

int delete_file(char * FullFileName)
{
 return wxRemoveFile(wxString::FromUTF8(FullFileName));
}

wxFile * FileOpenForWrite(char *FullFileName)
{
 wxFile *fd = NULL;
 fd = new(wxFile);
 if (fd->Create(wxString::FromUTF8(FullFileName), 0, wxS_DEFAULT))
  {
   return fd;
  }
 return NULL;
}

wxFile * FileOpenForRead(char *FullFileName)
{
 wxFile *fd = NULL;
 fd = new(wxFile);
 wxString path = (wxString::FromUTF8(FullFileName));
 if (wxFile::Exists(path))
  {
   (fd->Open(path, wxFile::read_write, wxS_DEFAULT));
   return fd;
  }
 return NULL;
}
