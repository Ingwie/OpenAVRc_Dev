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

#ifndef BLUETOOTHFRAME_H
#define BLUETOOTHFRAME_H

#include <wx/dataobj.h>
#include <wx/dnd.h>
#include <wx/file.h>


#include "serial/tserial.h"

//(*Headers(BluetoothFrame)
#include <wx/bmpbuttn.h>
#include <wx/combobox.h>
#include <wx/dirctrl.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/timer.h>
#include <wx/treectrl.h>
//*)

class BluetoothFrame: public wxFrame
{
	public:

    Tserial *BTComPort;
		BluetoothFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~BluetoothFrame();
    wxString GetFullPathTctrlItem(wxTreeItemId item);
    void SdToSdCpy(wxString dest, wxString file);
    void HddToSdCpy(wxString dest, wxString file);
    void SDToHddCpy(wxString dest, wxString file);

		//(*Declarations(BluetoothFrame)
		wxBitmapButton* BitmapButtonReboot;
		wxComboBox* ComboBoxCom;
		wxGenericDirCtrl* DirCtrl;
		wxPanel* Panel1;
		wxStaticBox* StaticBoxCom;
		wxStaticBox* StaticBoxLocal1;
		wxStaticBox* StaticBoxSD;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticTextFreeMem;
		wxStaticText* StaticTextVersion;
		wxTimer TimerRX;
		wxTreeCtrl* TctrlSd;
		//*)

	protected:

		//(*Identifiers(BluetoothFrame)
		static const long ID_STATICBOX1;
		static const long ID_COMBOBOX1;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT4;
		static const long ID_REBOOTBUTTON;
		static const long ID_STATICBOX2;
		static const long ID_STATICBOXSD;
		static const long ID_TREECTRLSD;
		static const long ID_GENERICDIRCTRL1;
		static const long ID_PANEL1;
		static const long ID_TIMERRX;
		//*)

    bool comIsValid;
    volatile bool timout;
    void DetectSerial();
    wxString uCLI;
    void ConnectBTCom(wxString name);
    wxString sendCmdAndWaitForResp(wxString BTcommand, wxString* BTanwser);
     // D & D
    wxDropSource dragSource;
    wxDragResult dragResult;



    wxString getRam();
    wxString getVer();
    void Populate_SD();
    void Populate_Dir(wxTreeItemId * dir);



	private:

		//(*Handlers(BluetoothFrame)
		void OnClose(wxCloseEvent& event);
		void OnComboBoxComSelected(wxCommandEvent& event);
		void OnComboBoxComDropdown(wxCommandEvent& event);
		void OnTimerRXTrigger(wxTimerEvent& event);
		void OnBitmapButtonRebootClick(wxCommandEvent& event);
		void OnTctrlSdBeginDrag(wxTreeEvent& event);
		//*)

		void OnDirCtrlBeginDrag(wxTreeEvent& event);

		DECLARE_EVENT_TABLE()
};

class DnD_TctrlSd_Txt : public wxTextDropTarget
{
public:
    DnD_TctrlSd_Txt(BluetoothFrame * BluetoothFrame_P) {BluetoothFrame = BluetoothFrame_P;}
    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text) wxOVERRIDE;

private:
   BluetoothFrame * BluetoothFrame;
};

class DnD_DirCtrl_Txt : public wxTextDropTarget
{
public:
    DnD_DirCtrl_Txt(BluetoothFrame * BluetoothFrame_P) {BluetoothFrame = BluetoothFrame_P;}
    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text) wxOVERRIDE;

private:
   BluetoothFrame * BluetoothFrame;
};

//////////// XMODEM /////////////
  int write_file(wxFile* fd, const uint8_t* buffer, int buffer_len);
  int seek_file(wxFile* fd, int32_t* offset, uint8_t whence = 0);
  int read_file(wxFile* fd, uint8_t* buffer, uintptr_t buffer_len);
  int FileExists(char * FullFileName);
  int delete_file(char * FullFileName);
  wxFile *FileOpenForWrite(char *FullFileName);
  wxFile *FileOpenForRead(char *FullFileName);


// Specific definitions
#define SERIAL_TYPE                        Tserial *
#define FILE_DESC                          wxFile *
#define FILE_EXISTS(FullFileName)          FileExists((char *)FullFileName)
#define FILE_OPEN_FOR_READ(FullFileName)   FileOpenForRead((char *)FullFileName)
#define FILE_OPEN_FOR_WRITE(FullFileName)  FileOpenForWrite((char *)FullFileName)
#define FILE_SEEK(fd, Pos)                 seek_file(fd, (int32_t *)&Pos)
#define FILE_READ_CHUNK(fd,  Buf, Size)    read_file(fd, (uint8_t *)Buf, Size)
#define FILE_WRITE_CHUNK(fd, Buf, Size)    write_file(fd, (const uint8_t *)Buf, Size)
#define FILE_SIZE(fd)                      fd->Length()
#define FILE_CLOSE(fd)                     fd->Close()
#define FILE_DELETE(FullFileName)          delete_file(( char *)FullFileName)

#define DELAY_MS(ms)                       wxMilliSleep(ms)
#define GET_TICK()                         (uint32_t)clock()
#define MS_TO_TICK(ms)                     (ms)

#define YIELD_TO_PRIO_TASK()               wxYieldIfNeeded()

  extern int8_t XReceive(SERIAL_TYPE pSer, const char *szFilename);
  extern int8_t XSend(SERIAL_TYPE pSer, const char *szFilename);

#endif
