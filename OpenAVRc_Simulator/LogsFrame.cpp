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

#include "LogsFrame.h"
#include <wx/tokenzr.h>
#include <wx/filedlg.h>
#include <wx/numformatter.h>

extern wxString googleearthPath;
extern bool Ini_Changed;

//(*InternalHeaders(LogsFrame)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(LogsFrame)
const long LogsFrame::ID_STATICBOX1 = wxNewId();
const long LogsFrame::ID_FIELDLISTBOX = wxNewId();
const long LogsFrame::ID_BUTTONRESET = wxNewId();
const long LogsFrame::ID_STATICTEXT1 = wxNewId();
const long LogsFrame::ID_BUTTONKML = wxNewId();
const long LogsFrame::ID_PANEL2 = wxNewId();
const long LogsFrame::ID_STATICBOX2 = wxNewId();
const long LogsFrame::ID_MATHPLOT1 = wxNewId();
const long LogsFrame::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(LogsFrame,wxFrame)
  //(*EventTable(LogsFrame)
  //*)
END_EVENT_TABLE()

LogsFrame::LogsFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
  //(*Initialize(LogsFrame)
  Create(parent, wxID_ANY, _("Logs"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxSUNKEN_BORDER|wxRAISED_BORDER, _T("wxID_ANY"));
  SetClientSize(wxSize(1180,512));
  Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(224,112), wxSize(1024,512), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
  StaticBox1 = new wxStaticBox(Panel1, ID_STATICBOX1, _("Champs"), wxPoint(8,8), wxSize(128,496), wxDOUBLE_BORDER, _T("ID_STATICBOX1"));
  FieldListBox = new wxListBox(Panel1, ID_FIELDLISTBOX, wxPoint(16,32), wxSize(112,464), 0, 0, wxLB_MULTIPLE, wxDefaultValidator, _T("ID_FIELDLISTBOX"));
  Panel2 = new wxPanel(Panel1, ID_PANEL2, wxPoint(144,8), wxSize(1032,48), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL2"));
  ResetButton = new wxButton(Panel2, ID_BUTTONRESET, _("RAZ"), wxPoint(16,10), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONRESET"));
  LogName = new wxStaticText(Panel2, ID_STATICTEXT1, wxEmptyString, wxPoint(360,10), wxSize(30,25), 0, _T("ID_STATICTEXT1"));
  LogName->SetFocus();
  LogName->SetForegroundColour(wxColour(0,0,255));
  wxFont LogNameFont(16,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Arial Narrow"),wxFONTENCODING_DEFAULT);
  LogName->SetFont(LogNameFont);
  KMLButton = new wxButton(Panel2, ID_BUTTONKML, _("KML"), wxPoint(928,10), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTONKML"));
  KMLButton->Disable();
  StaticBox2 = new wxStaticBox(Panel1, ID_STATICBOX2, _("Données"), wxPoint(144,64), wxSize(1024,440), wxDOUBLE_BORDER, _T("ID_STATICBOX2"));
  MathPlot = new mpWindow(Panel1, ID_MATHPLOT1, wxPoint(152,80), wxSize(1008,416), wxRAISED_BORDER|wxTAB_TRAVERSAL);
  MathPlot->UpdateAll();
  MathPlot->Fit();

  Connect(ID_FIELDLISTBOX,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&LogsFrame::OnFieldListBoxSelect);
  Connect(ID_BUTTONRESET,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LogsFrame::OnResetButtonClick);
  Connect(ID_BUTTONKML,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LogsFrame::OnKMLButtonClick);
  Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&LogsFrame::OnClose);
  //*)

  {
    wxIcon FrameIcon;
    SetIcon(wxICON(oavrc_icon));
  }

  numRecord = 0;
  numField = 0;
  FirstTime = "";

  wxColour colors[] = { wxColour(204, 102, 102), wxColour(102, 204, 102), wxColour(102, 102, 204),
                        wxColour(204, 204, 102), wxColour(204, 102, 204), wxColour(102, 204, 204), wxColour(218, 170, 0),
                        wxColour(248, 159, 171), wxColour(121, 252, 121), wxColour(121, 121, 252), wxColour(252, 252, 121),
                        wxColour(252, 121, 252), wxColour(121, 252, 252), wxColour(252, 198, 0), wxColour(128, 59, 59),
                        wxColour(59, 128, 59), wxColour(59, 59, 128), wxColour(128, 128, 59), wxColour(128, 59, 128),
                        wxColour(59, 128, 128), wxColour(128, 98, 0)
                      };
  size_t color_num = 0;

  wxFileDialog openFileDialog(this, _("Ouvrir Fichier BIN"), AppPath+ "\\SD\\LOGS\\", "","Fichiers CSV (*.csv)|*.csv", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() == wxID_CANCEL)
    {
      CloseFrame();
    }

  wxTextFile tfile(openFileDialog.GetPath());
  if (tfile.Exists())   //avoid crash if file is not found
    {
      wxBusyCursor busy;
      tfile.Open(openFileDialog.GetPath());
      size_t numLine = tfile.GetLineCount();
      wxString line = tfile.GetLine(0);
      wxStringTokenizer tokenizer(line, ",");
      size_t num = 0;
      while (tokenizer.HasMoreTokens())
        {
          FieldListBox->Append(tokenizer.GetNextToken());
          ++num;
        }
      numField = num;
      numRecord = numLine - 1;

      for (size_t j = 1; j<numLine; j++ )
        {
          line = tfile.GetLine(j);
          wxStringTokenizer tokenizer(line, ",");
          num = 0;
          while ( tokenizer.HasMoreTokens() )
            {
              CheckLogValue(FieldListBox->GetString(num), j, num, tokenizer.GetNextToken());
              ++num;
            }
        }

      wxFileName tfileName = tfile.GetName();
      LogName->SetLabel(tfileName.GetName()+" @ " + FirstTime);
      LogName->Update();

      for (int j = 0; j<numField; j++ )
        {
          vectorLayer[j] = new mpFXYVector("");
          vectorLayer[j]->SetData(vectorx[j], vectory[j]);
          vectorLayer[j]->SetContinuity(true);
          wxPen vectorpen(colors[color_num++], 2, wxSOLID);
          if (color_num > DIM(colors))
            color_num = 0;
          vectorLayer[j]->SetPen(vectorpen);
          vectorLayer[j]->SetDrawOutsideMargins(true);
        }
      wxFont graphFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
      mpScaleX* xaxis = new mpScaleX("", mpALIGN_BOTTOM, true, mpX_HOURS );
      mpScaleY* yaxis = new mpScaleY("", mpALIGN_LEFT, true);
      xaxis->SetFont(graphFont);
      yaxis->SetFont(graphFont);
      xaxis->SetDrawOutsideMargins(false);
      yaxis->SetDrawOutsideMargins(false);
      MathPlot->SetMargins(10, 10, 30, 50);
      MathPlot->AddLayer(xaxis);
      MathPlot->AddLayer(yaxis);

      for (int j = 0; j<numField; j++ )
        {
          MathPlot->AddLayer(vectorLayer[j], false);
          vectorLayer[j]->SetVisible(false);
        }

      MathPlot->AddLayer( leg = new mpInfoLegend(wxRect(100,20,40,40), wxTRANSPARENT_BRUSH));
      leg->SetVisible(true);
      MathPlot->EnableDoubleBuffer(true);
      MathPlot->Fit();
    }
}

LogsFrame::~LogsFrame()
{
  //(*Destroy(LogsFrame)
  //*)
}


void LogsFrame::OnClose(wxCloseEvent& event)
{
  CloseFrame();
}

void LogsFrame::CloseFrame()
{
  OpenAVRc_SimulatorFrame *parent = wxDynamicCast(this->GetParent(), OpenAVRc_SimulatorFrame);
  if(parent)
    parent->EnableLogsMenu();
  Destroy();
}

double convertGPSCoord(double coord)
{
  coord /= 100;
  uint32_t Deg = coord;
  coord -= Deg; // Keep only minutes
  coord *= 100000000;
  coord /= 6;
  coord += (double)Deg*10000000;
  coord /= 10000000;
  return coord;
}

void LogsFrame::CheckLogValue(wxString name, size_t line, size_t col, wxString data)
{
//  #define TR_LOGS_TELEMETRYHUB  "Buffer,RX,TX,A1,A2,GPS Date,GPS Time,Long,Lat,Course,GPS Speed(kts),GPS Alt,Baro Alt(" TELEMETRY_BARO_ALT_UNIT "),Vertical Speed,Air Speed(kts),Temp1,Temp2,RPM,Fuel," TELEMETRY_CELLS_LABEL "Current,Consumption,Vfas,AccelX,AccelY,AccelZ,"

  // Set first time found
  if ((FirstTime.IsEmpty()) && (name.IsSameAs("Time")))
    {
      FirstTime = data;
    }
  data.Replace(".",wxNumberFormatter::GetDecimalSeparator());
  double value;
  data.ToDouble(&value);

  if ((name.IsSameAs("Date")) || (name.IsSameAs("Time")) || (name.IsSameAs("GPS Date")) || (name.IsSameAs("GPS Time")))
    {
      value = 0;
    }
  if ((name.IsSameAs("Long")) || (name.IsSameAs("Lat")))
    {
      if (value && (!KMLButton->IsEnabled()))
      {
        KMLButton->Enable();
      }
      value = convertGPSCoord(value);

      if (data.EndsWith("W"))
        {
          value = -value;
        }
      if (data.EndsWith("S"))
        {
          value = -value;
        }
    }
  if ((name.IsSameAs("GPS Speed(kts)")) || (name.IsSameAs("Air Speed(kts)")))
    {
      value = ((value * 1852)/ 1000);
    }

  vectory[col].push_back(value);
  if (name == "Time")
    {
      wxStringTokenizer tokenizer(data, ":");
      double temp;
      tokenizer.GetNextToken().ToDouble(&temp);
      value = temp*3600;
      tokenizer.GetNextToken().ToDouble(&temp);
      value += temp*60;
      tokenizer.GetNextToken().ToDouble(&temp);
      value += temp;
      if (line == 1)
        timeZero = value;
      value = value - timeZero;


      for (int i=0; i<numField; i++)
        {
          vectorx[i].push_back(value);
        }
    }

}

void LogsFrame::PopulateMathPlot()
{
  int numSelected = 0;
  for (int i=0; i<FieldListBox->GetCount(); i++)
    {
      if (FieldListBox->IsSelected(i))
        {
          vectorLayer[i]->SetVisible(true);
          vectorLayer[i]->SetName(FieldListBox->GetString(i));
          vectorLayer[i]->ShowName(false); // only leg show the name
          numSelected++;
        }
      else
        {
          vectorLayer[i]->SetVisible(false);
        }
    }
  if (!numSelected)
    {
      MathPlot->Refresh();
    }
  MathPlot->Fit();
}

void LogsFrame::OnFieldListBoxSelect(wxCommandEvent& event)
{
  PopulateMathPlot();
}

void LogsFrame::OnResetButtonClick(wxCommandEvent& event)
{
  FieldListBox->DeselectAll();
  PopulateMathPlot();
}

void LogsFrame::OnKMLButtonClick(wxCommandEvent& event)
{
  if (googleearthPath.IsEmpty())
    {

      wxFileDialog openFileDialogge(this, _("Emplacement de Google earth"), "C:\\Program Files\\", "","googleearth (*.exe)|*.exe", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
      if (openFileDialogge.ShowModal() == wxID_CANCEL)
        {
          return;
        }
      googleearthPath = openFileDialogge.GetPath();
      Ini_Changed = true;
    }

  // Find the GPS values
  int longFieldNum, latFieldNum, altFieldNum = 0;

  longFieldNum = FieldListBox->FindString("Long");
  latFieldNum = FieldListBox->FindString("Lat");
  altFieldNum = FieldListBox->FindString("GPS Alt");
  if ((longFieldNum == wxNOT_FOUND) || (latFieldNum == wxNOT_FOUND) || (altFieldNum == wxNOT_FOUND))
    {
      return; // Not GPS datas
    }

  wxString dataLong, dataLat, dataAlt;
  // Write the file
  wxString file = AppPath + "\\OpenAVRc.kml";
  wxTextFile tfile(file);
  if (!tfile.Exists())
    tfile.Create(); //avoid crash if file doesn't exist
  tfile.Open(file);
  tfile.Clear();
  tfile.AddLine("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
  tfile.AddLine("<kml xmlns=\"http://www.opengis.net/kml/2.2\">");
  tfile.AddLine("<Document><name>" + LogName->GetLabel() + "</name><description>" + LogName->GetLabel() + "</description>");
  tfile.AddLine("<LookAt>");
  dataLong = wxString::Format("%lf", vectory[longFieldNum][0]);
  dataLong.Replace(wxNumberFormatter::GetDecimalSeparator(),".");
  tfile.AddLine("<longitude> " + dataLong + "</longitude>");
  dataLat = wxString::Format("%lf", vectory[latFieldNum][0]);
  dataLat.Replace(wxNumberFormatter::GetDecimalSeparator(),".");
  tfile.AddLine("<latitude> " + dataLat + "</latitude>");
  tfile.AddLine("<altitude>0</altitude>");
  tfile.AddLine("<altitudeMode>relativeToGround</altitudeMode>");
  tfile.AddLine("<range> 1006</range>");
  tfile.AddLine("<tilt>80</tilt>");
  tfile.AddLine("<heading>0</heading>");
  tfile.AddLine("</LookAt>");
  tfile.AddLine("<Placemark>");
  tfile.AddLine("<Point><coordinates> " + dataLong + "," + dataLat + "</coordinates></Point>");
  tfile.AddLine("</Placemark>");
  tfile.AddLine("<Placemark><Style><LineStyle><width>2</width><color>ff0fff0f</color></LineStyle></Style>");
  tfile.AddLine("<LineString><altitudeMode>absolute</altitudeMode><coordinates>");
  for (int i = 0; i < numRecord; i++ )
    {
      dataLong = wxString::Format("%lf", vectory[longFieldNum][i]);
      dataLat = wxString::Format("%lf", vectory[latFieldNum][i]);
      dataAlt = wxString::Format("%lf", vectory[altFieldNum][i]);
      dataLong.Replace(wxNumberFormatter::GetDecimalSeparator(),".");
      dataLat.Replace(wxNumberFormatter::GetDecimalSeparator(),".");
      dataAlt.Replace(wxNumberFormatter::GetDecimalSeparator(),".");
      tfile.AddLine(dataLong + "," + dataLat + "," + dataAlt);
    }
  tfile.AddLine("</coordinates></LineString></Placemark>");
  tfile.AddLine("<Placemark><Style><LineStyle><width>2</width><color>fbfbfbfb</color></LineStyle></Style>");
  tfile.AddLine("<LineString><altitudeMode>clamptoGround</altitudeMode><coordinates>");
  for (int i = 0; i < numRecord; i++ )
    {
      dataLong = wxString::Format("%lf", vectory[longFieldNum][i]);
      dataLat = wxString::Format("%lf", vectory[latFieldNum][i]);
      dataLong.Replace(wxNumberFormatter::GetDecimalSeparator(),".");
      dataLat.Replace(wxNumberFormatter::GetDecimalSeparator(),".");
      tfile.AddLine(dataLong + "," + dataLat);
    }
  tfile.AddLine("</coordinates></LineString></Placemark></Document></kml>");
  //tfile.AddLine("");

  tfile.Write();
  tfile.Close();
  wxString earth = googleearthPath + " ";
  wxString run = AppPath + "\\OpenAVRc.kml";
  wxExecute(earth + run);
}

