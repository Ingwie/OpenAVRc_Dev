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


#include "GvarsFrame.h"
#include "OpenAVRc_SimulatorMain.h"
#include <wx/valtext.h>

//(*InternalHeaders(GvarsFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(GvarsFrame)
const long GvarsFrame::ID_GRID1 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar6 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar8 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar11 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar12 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar10 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar9 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar7 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoPhase0 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoPhase1 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoPhase2 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoPhase3 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoPhase4 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoPhase5 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar1 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar2 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar3 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar4 = wxNewId();
const long GvarsFrame::ID_TextCtrlpersoGvar5 = wxNewId();
const long GvarsFrame::ID_PANEL1 = wxNewId();
const long GvarsFrame::ID_TIMERGVARS = wxNewId();
//*)

BEGIN_EVENT_TABLE(GvarsFrame,wxFrame)
  //(*EventTable(GvarsFrame)
  //*)
END_EVENT_TABLE()

GvarsFrame::GvarsFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
  wxTextValidator validator(wxFILTER_INCLUDE_CHAR_LIST);
  wxString valid_chars(wxT(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_-.,"));
  validator.SetCharIncludes(valid_chars);

  //(*Initialize(GvarsFrame)
  Create(parent, wxID_ANY, _("Variables globales"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxSUNKEN_BORDER, _T("wxID_ANY"));
  SetClientSize(wxSize(421,289));
  Move(wxPoint(60,60));
  Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(0,48), wxSize(421,288), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
  GVARSGrid = new wxGrid(Panel1, ID_GRID1, wxPoint(0,0), wxSize(432,304), wxSUNKEN_BORDER, _T("ID_GRID1"));
  GVARSGrid->CreateGrid(12,6);
  GVARSGrid->Disable();
  GVARSGrid->EnableEditing(false);
  GVARSGrid->EnableGridLines(true);
  GVARSGrid->SetColLabelSize(22);
  GVARSGrid->SetRowLabelSize(60);
  GVARSGrid->SetDefaultRowSize(22, true);
  GVARSGrid->SetDefaultColSize(60, true);
  GVARSGrid->SetDefaultCellFont( GVARSGrid->GetFont() );
  GVARSGrid->SetDefaultCellTextColour( GVARSGrid->GetForegroundColour() );
  TextCtrlpersoGvar6 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar6, _("valvg6"), wxPoint(2,132), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoGvar6"));
  TextCtrlpersoGvar6->SetMaxLength(6);
  TextCtrlpersoGvar8 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar8, _("valvg8"), wxPoint(2,176), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoGvar8"));
  TextCtrlpersoGvar8->SetMaxLength(6);
  TextCtrlpersoGvar11 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar11, _("valvg11"), wxPoint(2,242), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoGvar11"));
  TextCtrlpersoGvar11->SetMaxLength(6);
  TextCtrlpersoGvar12 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar12, _("valvg12"), wxPoint(2,264), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoGvar12"));
  TextCtrlpersoGvar12->SetMaxLength(6);
  TextCtrlpersoGvar10 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar10, _("valvg10"), wxPoint(2,220), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoGvar10"));
  TextCtrlpersoGvar10->SetMaxLength(6);
  TextCtrlpersoGvar9 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar9, _("valvg9"), wxPoint(2,198), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoGvar9"));
  TextCtrlpersoGvar9->SetMaxLength(6);
  TextCtrlpersoGvar7 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar7, _("valvg7"), wxPoint(2,154), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoGvar7"));
  TextCtrlpersoGvar7->SetMaxLength(6);
  TextCtrlpersoPhase0 = new wxTextCtrl(Panel1, ID_TextCtrlpersoPhase0, _("npha0"), wxPoint(63,2), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoPhase0"));
  TextCtrlpersoPhase0->SetMaxLength(6);
  TextCtrlpersoPhase1 = new wxTextCtrl(Panel1, ID_TextCtrlpersoPhase1, _("npha1"), wxPoint(123,2), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoPhase1"));
  TextCtrlpersoPhase1->SetMaxLength(6);
  TextCtrlpersoPhase2 = new wxTextCtrl(Panel1, ID_TextCtrlpersoPhase2, _("npha2"), wxPoint(183,2), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoPhase2"));
  TextCtrlpersoPhase2->SetMaxLength(6);
  TextCtrlpersoPhase3 = new wxTextCtrl(Panel1, ID_TextCtrlpersoPhase3, _("npha3"), wxPoint(243,2), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoPhase3"));
  TextCtrlpersoPhase3->SetMaxLength(6);
  TextCtrlpersoPhase4 = new wxTextCtrl(Panel1, ID_TextCtrlpersoPhase4, _("npha4"), wxPoint(303,2), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoPhase4"));
  TextCtrlpersoPhase4->SetMaxLength(6);
  TextCtrlpersoPhase5 = new wxTextCtrl(Panel1, ID_TextCtrlpersoPhase5, _("npha5"), wxPoint(363,2), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoPhase5"));
  TextCtrlpersoPhase5->SetMaxLength(6);
  TextCtrlpersoGvar1 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar1, _("valvg1"), wxPoint(2,22), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS|wxFULL_REPAINT_ON_RESIZE, wxDefaultValidator, _T("ID_TextCtrlpersoGvar1"));
  TextCtrlpersoGvar1->SetMaxLength(6);
  TextCtrlpersoGvar2 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar2, _("valvg2"), wxPoint(2,44), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS|wxFULL_REPAINT_ON_RESIZE, wxDefaultValidator, _T("ID_TextCtrlpersoGvar2"));
  TextCtrlpersoGvar2->SetMaxLength(6);
  TextCtrlpersoGvar3 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar3, _("valvg3"), wxPoint(2,66), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS|wxFULL_REPAINT_ON_RESIZE, wxDefaultValidator, _T("ID_TextCtrlpersoGvar3"));
  TextCtrlpersoGvar3->SetMaxLength(6);
  TextCtrlpersoGvar4 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar4, _("valvg4"), wxPoint(2,88), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoGvar4"));
  TextCtrlpersoGvar4->SetMaxLength(6);
  TextCtrlpersoGvar5 = new wxTextCtrl(Panel1, ID_TextCtrlpersoGvar5, _("valvg5"), wxPoint(2,110), wxSize(58,22), wxTE_CENTRE|wxWANTS_CHARS, wxDefaultValidator, _T("ID_TextCtrlpersoGvar5"));
  TextCtrlpersoGvar5->SetMaxLength(6);
  gvarsTimer.SetOwner(this, ID_TIMERGVARS);
  gvarsTimer.Start(2000, false);

  Connect(ID_TextCtrlpersoGvar6,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar6,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar6TextEnter);
  Connect(ID_TextCtrlpersoGvar8,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar8,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar8TextEnter);
  Connect(ID_TextCtrlpersoGvar11,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar11,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar11TextEnter);
  Connect(ID_TextCtrlpersoGvar12,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar12,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar12TextEnter);
  Connect(ID_TextCtrlpersoGvar10,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar10,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar10TextEnter);
  Connect(ID_TextCtrlpersoGvar9,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar9,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar9TextEnter);
  Connect(ID_TextCtrlpersoGvar7,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar7,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar7TextEnter);
  Connect(ID_TextCtrlpersoPhase0,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoPhase0,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoPhase0TextEnter);
  Connect(ID_TextCtrlpersoPhase1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoPhase1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoPhase1TextEnter);
  Connect(ID_TextCtrlpersoPhase2,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoPhase2,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoPhase2TextEnter);
  Connect(ID_TextCtrlpersoPhase3,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoPhase3,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoPhase3TextEnter);
  Connect(ID_TextCtrlpersoPhase4,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoPhase4,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoPhase4TextEnter);
  Connect(ID_TextCtrlpersoPhase5,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoPhase5,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoPhase5TextEnter);
  Connect(ID_TextCtrlpersoGvar1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar1TextEnter);
  Connect(ID_TextCtrlpersoGvar2,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar2,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar2TextEnter);
  Connect(ID_TextCtrlpersoGvar3,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar3,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar3TextEnter);
  Connect(ID_TextCtrlpersoGvar4,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar4,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar4TextEnter);
  Connect(ID_TextCtrlpersoGvar5,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoAllText);
  Connect(ID_TextCtrlpersoGvar5,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&GvarsFrame::OnTextCtrlpersoGvar5TextEnter);
  Connect(ID_TIMERGVARS,wxEVT_TIMER,(wxObjectEventFunction)&GvarsFrame::OngvarsTimerTrigger);
  Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&GvarsFrame::OnClose);
  //*)

  {
    wxIcon FrameIcon;
    SetIcon(wxICON(oavrc_icon));
  }

  GVARSGrid->SetDefaultCellAlignment(wxALIGN_CENTER,wxALIGN_CENTER);

  PopulateGvarsFrame();

}

GvarsFrame::~GvarsFrame()
{
  //(*Destroy(GvarsFrame)
  //*)
}

void GvarsFrame::PopulateGvarsFrame()
{

  for (uint8_t p = 0; p < MAX_FLIGHT_MODES; p++) {
    for (int8_t x = 0; x < MAX_GVARS; x++) {
      int16_t gvarVal = (g_model.flightModeData[p].gvars[x]);
      wxString gvarStr;
      if (gvarVal == 0) gvarStr = "---";
      else if (gvarVal >= (CFN_GVAR_CST_MAX + MAX_GVARS - 0)) {
        gvarStr = "*PV";
        if (p <= (gvarVal - (CFN_GVAR_CST_MAX + MAX_GVARS - 0))) {
          ++gvarVal;                    // Flight phase can't refer to himself. Skip one phase.
        }
        gvarStr.Append (wxString::Format(wxT("%i"),(gvarVal - (CFN_GVAR_CST_MAX + MAX_GVARS - 1))));
      } else gvarStr.Append (wxString::Format(wxT("%i"),(gvarVal)));

      GVARSGrid->SetCellValue(x,p,gvarStr);
    }
  }
  // Noms des VG
  wxString myString = ConvCharFwToWxstr(g_model.gvars[0].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR1";
  TextCtrlpersoGvar1->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[1].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR2";
  TextCtrlpersoGvar2->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[2].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR3";
  TextCtrlpersoGvar3->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[3].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR4";
  TextCtrlpersoGvar4->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[4].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR5";
  TextCtrlpersoGvar5->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[5].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR6";
  TextCtrlpersoGvar6->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[6].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR7";
  TextCtrlpersoGvar7->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[7].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR8";
  TextCtrlpersoGvar8->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[8].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR9";
  TextCtrlpersoGvar9->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[9].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR10";
  TextCtrlpersoGvar10->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[10].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR11";
  TextCtrlpersoGvar11->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.gvars[11].name,LEN_GVAR_NAME);
  if (myString == "") myString = "GVAR12";
  TextCtrlpersoGvar12->SetValue(myString);

  TextCtrlpersoGvar1->Refresh();
  TextCtrlpersoGvar2->Refresh();
  TextCtrlpersoGvar3->Refresh();
  TextCtrlpersoGvar4->Refresh();
  TextCtrlpersoGvar5->Refresh(); // Refresh all vertical textctrl because wxgrid mask them
  TextCtrlpersoGvar6->Refresh();
  TextCtrlpersoGvar7->Refresh();
  TextCtrlpersoGvar8->Refresh();
  TextCtrlpersoGvar9->Refresh();
  TextCtrlpersoGvar10->Refresh();
  TextCtrlpersoGvar11->Refresh();
  TextCtrlpersoGvar12->Refresh();

  //Noms des phases de vol

  myString = ConvCharFwToWxstr(g_model.flightModeData[0].name,LEN_FLIGHT_MODE_NAME);
  if (myString == "") myString = "FM1";
  TextCtrlpersoPhase0->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.flightModeData[1].name,LEN_FLIGHT_MODE_NAME);
  if (myString == "") myString = "FM2";
  TextCtrlpersoPhase1->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.flightModeData[2].name,LEN_FLIGHT_MODE_NAME);
  if (myString == "") myString = "FM3";
  TextCtrlpersoPhase2->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.flightModeData[3].name,LEN_FLIGHT_MODE_NAME);
  if (myString == "") myString = "FM4";
  TextCtrlpersoPhase3->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.flightModeData[4].name,LEN_FLIGHT_MODE_NAME);
  if (myString == "") myString = "FM5";
  TextCtrlpersoPhase4->SetValue(myString);

  myString = ConvCharFwToWxstr(g_model.flightModeData[5].name,LEN_FLIGHT_MODE_NAME);
  if (myString == "") myString = "FM6";
  TextCtrlpersoPhase5->SetValue(myString);

  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}

void GvarsFrame::OnClose(wxCloseEvent& event)
{
  event.Skip();
  OpenAVRc_SimulatorFrame *parent = wxDynamicCast(this->GetParent(), OpenAVRc_SimulatorFrame);
  if(parent)
    parent->EnableOutputGvarsMenu();
  Destroy();
}

void GvarsFrame::OngvarsTimerTrigger(wxTimerEvent& event)
{
  event.Skip();
  PopulateGvarsFrame();
}

// Gestion affichage nom des Variables Globales
// VG1 *************************************
void GvarsFrame::OnTextCtrlpersoGvar1TextEnter(wxCommandEvent& event)
{
  event.Skip();
  wxString persoGvar1 = TextCtrlpersoGvar1->GetValue();
  ConvWxstrToCharFw(persoGvar1,g_model.gvars[0].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}
// VG2 *************************************
void GvarsFrame::OnTextCtrlpersoGvar2TextEnter(wxCommandEvent& event)
{
  event.Skip();
  wxString persoGvar2 = TextCtrlpersoGvar2->GetValue();
  ConvWxstrToCharFw(persoGvar2,g_model.gvars[1].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}
// VG3 *************************************
void GvarsFrame::OnTextCtrlpersoGvar3TextEnter(wxCommandEvent& event)
{
  event.Skip();
  wxString persoGvar3 = TextCtrlpersoGvar3->GetValue();
  ConvWxstrToCharFw(persoGvar3,g_model.gvars[2].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}
// VG4 *************************************
void GvarsFrame::OnTextCtrlpersoGvar4TextEnter(wxCommandEvent& event)
{
  event.Skip();
  wxString persoGvar4 = TextCtrlpersoGvar4->GetValue();
  ConvWxstrToCharFw(persoGvar4,g_model.gvars[3].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}
// VG5 *************************************
void GvarsFrame::OnTextCtrlpersoGvar5TextEnter(wxCommandEvent& event)
{
  event.Skip();
  wxString persoGvar5 = TextCtrlpersoGvar5->GetValue();
  ConvWxstrToCharFw(persoGvar5,g_model.gvars[4].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}
// VG6 *************************************
void GvarsFrame::OnTextCtrlpersoGvar6TextEnter(wxCommandEvent& event)
{
  event.Skip();
  wxString persoGvar6 = TextCtrlpersoGvar6->GetValue();
  ConvWxstrToCharFw(persoGvar6,g_model.gvars[5].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}
// VG7 *************************************
void GvarsFrame::OnTextCtrlpersoGvar7TextEnter(wxCommandEvent& event)
{
  event.Skip();
#if defined(EXTERNALEEPROM)
  wxString persoGvar7 = TextCtrlpersoGvar7->GetValue();
  ConvWxstrToCharFw(persoGvar7,g_model.gvars[6].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
#endif
}
// VG8 *************************************
void GvarsFrame::OnTextCtrlpersoGvar8TextEnter(wxCommandEvent& event)
{
  event.Skip();
#if defined(EXTERNALEEPROM)
  wxString persoGvar8 = TextCtrlpersoGvar8->GetValue();
  ConvWxstrToCharFw(persoGvar8,g_model.gvars[7].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
#endif
}
// VG9 *************************************
void GvarsFrame::OnTextCtrlpersoGvar9TextEnter(wxCommandEvent& event)
{
  event.Skip();
#if defined(EXTERNALEEPROM)
  wxString persoGvar9 = TextCtrlpersoGvar9->GetValue();
  ConvWxstrToCharFw(persoGvar9,g_model.gvars[8].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
#endif
}
// VG10 *************************************
void GvarsFrame::OnTextCtrlpersoGvar10TextEnter(wxCommandEvent& event)
{
  event.Skip();
#if defined(EXTERNALEEPROM)
  wxString persoGvar10 = TextCtrlpersoGvar10->GetValue();
  ConvWxstrToCharFw(persoGvar10,g_model.gvars[9].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
#endif
}
// VG11 *************************************
void GvarsFrame::OnTextCtrlpersoGvar11TextEnter(wxCommandEvent& event)
{
  event.Skip();
#if defined(EXTERNALEEPROM)
  wxString persoGvar11 = TextCtrlpersoGvar11->GetValue();
  ConvWxstrToCharFw(persoGvar11,g_model.gvars[10].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
#endif
}
// VG12 *************************************
void GvarsFrame::OnTextCtrlpersoGvar12TextEnter(wxCommandEvent& event)
{
  event.Skip();
#if defined(EXTERNALEEPROM)
  wxString persoGvar12 = TextCtrlpersoGvar12->GetValue();
  ConvWxstrToCharFw(persoGvar12,g_model.gvars[11].name, LEN_GVAR_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
#endif
}

// Gestion affichage du nom des phases de vol
// Phase de vol PV0 **********************
void GvarsFrame::OnTextCtrlpersoPhase0TextEnter(wxCommandEvent& event)
{
  event.Skip();
  wxString persoPhase0 = TextCtrlpersoPhase0->GetValue();
  ConvWxstrToCharFw(persoPhase0,g_model.flightModeData[0].name, LEN_FLIGHT_MODE_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}

// Phase de vol PV1 **********************
void GvarsFrame::OnTextCtrlpersoPhase1TextEnter(wxCommandEvent& event)
{
  event.Skip();
  wxString persoPhase1 = TextCtrlpersoPhase1->GetValue();
  ConvWxstrToCharFw(persoPhase1,g_model.flightModeData[1].name, LEN_FLIGHT_MODE_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}

// Phase de vol PV2 **********************
void GvarsFrame::OnTextCtrlpersoPhase2TextEnter(wxCommandEvent& event)
{
  event.Skip();
  wxString persoPhase2 = TextCtrlpersoPhase2->GetValue();
  ConvWxstrToCharFw(persoPhase2,g_model.flightModeData[2].name, LEN_FLIGHT_MODE_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}

// Phase de vol PV3 **********************
void GvarsFrame::OnTextCtrlpersoPhase3TextEnter(wxCommandEvent& event)
{
  event.Skip();
  wxString persoPhase3 = TextCtrlpersoPhase3->GetValue();
  ConvWxstrToCharFw(persoPhase3,g_model.flightModeData[3].name, LEN_FLIGHT_MODE_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}
// Phase de vol PV4 **********************
void GvarsFrame::OnTextCtrlpersoPhase4TextEnter(wxCommandEvent& event)
{
  event.Skip();
  wxString persoPhase4 = TextCtrlpersoPhase4->GetValue();
  ConvWxstrToCharFw(persoPhase4,g_model.flightModeData[4].name, LEN_FLIGHT_MODE_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}

// Phase de vol PV5 **********************
void GvarsFrame::OnTextCtrlpersoPhase5TextEnter(wxCommandEvent& event)
{
  event.Skip();
  wxString persoPhase5 = TextCtrlpersoPhase5->GetValue();
  ConvWxstrToCharFw(persoPhase5,g_model.flightModeData[5].name, LEN_FLIGHT_MODE_NAME);
  gvarsTimer.Start(-1,wxTIMER_CONTINUOUS);
}

// Fin

void GvarsFrame::OnTextCtrlpersoAllText(wxCommandEvent& event)
{
  event.Skip();
  gvarsTimer.Stop();  // Stop timer in "edit" mode
}
