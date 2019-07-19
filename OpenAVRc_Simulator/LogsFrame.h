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

#ifndef LOGSFRAME_H
#define LOGSFRAME_H

//(*Headers(LogsFrame)
#include <mathplot.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
//*)

#include "OpenAVRc_SimulatorMain.h"

#define NUM_FIELD 60

class LogsFrame: public wxFrame
{
public:

  LogsFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
  virtual ~LogsFrame();

  //(*Declarations(LogsFrame)
  mpWindow* MathPlot;
  wxButton* KMLButton;
  wxButton* ResetButton;
  wxListBox* FieldListBox;
  wxPanel* Panel1;
  wxPanel* Panel2;
  wxStaticBox* StaticBox1;
  wxStaticBox* StaticBox2;
  wxStaticText* LogName;
  //*)

protected:

  //(*Identifiers(LogsFrame)
  static const long ID_STATICBOX1;
  static const long ID_FIELDLISTBOX;
  static const long ID_BUTTONRESET;
  static const long ID_STATICTEXT1;
  static const long ID_BUTTONKML;
  static const long ID_PANEL2;
  static const long ID_STATICBOX2;
  static const long ID_MATHPLOT1;
  static const long ID_PANEL1;
  //*)

private:

  //(*Handlers(LogsFrame)
  void OnClose(wxCloseEvent& event);
  void OnFieldListBoxSelect(wxCommandEvent& event);
  void OnResetButtonClick(wxCommandEvent& event);
  void OnKMLButtonClick(wxCommandEvent& event);
  //*)

  std::vector<double> vectorx[NUM_FIELD], vectory[NUM_FIELD];
  mpFXYVector* vectorLayer[NUM_FIELD];
  mpInfoLegend* leg;
  size_t numField;
  size_t numRecord;
  double timeZero;
  wxString FirstTime;
  void CheckLogValue(wxString name, size_t line, size_t col, wxString data);
  bool computeUsefullField(wxString name, wxString data, double* pvalue);
  void PopulateMathPlot();

  DECLARE_EVENT_TABLE()
};

#endif
