/*
**************************************************************************
*                                                                        *
*              This file is part of the OpenAVRc project.                *
*                                                                        *
*                         Based on code named                            *
*             OpenTx - https://github.com/opentx/opentx                  *
*                                                                        *
*                Only AVR code here for lisibility ;-)                   *
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


#ifndef VOICEEDITFRAME_H
#define VOICEEDITFRAME_H

#include "OpenAVRc_DesktopMain.h"


//(*Headers(VoiceEditFrame)
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/button.h>
#include <wx/frame.h>
//*)


class VoiceEditFrame: public wxFrame
{
public:

  VoiceEditFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
  virtual ~VoiceEditFrame();

  //(*Declarations(VoiceEditFrame)
  wxButton* ButtonGenerer;
  wxGrid* VoiceGrid;
  wxPanel* Panel1;
  wxStaticBox* StaticBox1;
  wxButton* ButtonSauvegarder;
  wxButton* ButtonJouer;
  //*)

protected:

  //(*Identifiers(VoiceEditFrame)
  static const long ID_STATICBOX1;
  static const long ID_GRID1;
  static const long ID_BUTTONSAUVEGARDER;
  static const long ID_BUTTONGENERER;
  static const long ID_BUTTONJOUER;
  static const long ID_PANEL1;
  //*)

private:

  //(*Handlers(VoiceEditFrame)
  void Load(); //Load textfile language
  void OnButtonSauvegarderClick(wxCommandEvent& event);
  void OnButtonGenererClick(wxCommandEvent& event);
  void OnVoiceGridCellSelect(wxGridEvent& event);
  void OnButtonJouerClick(wxCommandEvent& event);
  //*)

  DECLARE_EVENT_TABLE()
};

#endif
