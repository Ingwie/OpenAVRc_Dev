#include "GvarsFrame.h"
#include "OpenAVRc_SimulatorMain.h"

void PopulateGvarsFrame();

//(*InternalHeaders(GvarsFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(GvarsFrame)
const long GvarsFrame::ID_GRID1 = wxNewId();
const long GvarsFrame::ID_PANEL1 = wxNewId();
const long GvarsFrame::ID_TIMERGVARS = wxNewId();
//*)

BEGIN_EVENT_TABLE(GvarsFrame,wxFrame)
	//(*EventTable(GvarsFrame)
	//*)
END_EVENT_TABLE()

GvarsFrame::GvarsFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(GvarsFrame)
	Create(parent, wxID_ANY, _("Variables globales"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxSUNKEN_BORDER, _T("wxID_ANY"));
	SetClientSize(wxSize(300,135));
	Move(wxPoint(60,60));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(0,48), wxSize(312,136), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	GVARSGrid = new wxGrid(Panel1, ID_GRID1, wxPoint(8,8), wxSize(312,128), wxSUNKEN_BORDER, _T("ID_GRID1"));
	GVARSGrid->CreateGrid(5,6);
	GVARSGrid->Disable();
	GVARSGrid->EnableEditing(false);
	GVARSGrid->EnableGridLines(true);
	GVARSGrid->SetColLabelSize(29);
	GVARSGrid->SetRowLabelSize(50);
	GVARSGrid->SetDefaultColSize(40, true);
	GVARSGrid->SetColLabelValue(0, _("PV0"));
	GVARSGrid->SetColLabelValue(1, _("PV1"));
	GVARSGrid->SetColLabelValue(2, _("PV2"));
	GVARSGrid->SetColLabelValue(3, _("PV3"));
	GVARSGrid->SetColLabelValue(4, _("PV4"));
	GVARSGrid->SetColLabelValue(5, _("PV5"));
	GVARSGrid->SetRowLabelValue(0, _("VG1"));
	GVARSGrid->SetRowLabelValue(1, _("VG2"));
	GVARSGrid->SetRowLabelValue(2, _("VG3"));
	GVARSGrid->SetRowLabelValue(3, _("VG4"));
	GVARSGrid->SetRowLabelValue(4, _("VG5"));
	GVARSGrid->SetDefaultCellFont( GVARSGrid->GetFont() );
	GVARSGrid->SetDefaultCellTextColour( GVARSGrid->GetForegroundColour() );
	gvarsTimer.SetOwner(this, ID_TIMERGVARS);
	gvarsTimer.Start(2000, false);

	Connect(ID_TIMERGVARS,wxEVT_TIMER,(wxObjectEventFunction)&GvarsFrame::OngvarsTimerTrigger);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&GvarsFrame::OnClose);
	//*)

  {
    wxIcon FrameIcon;
    SetIcon(wxICON(nsrcs_icon));
  }
  PopulateGvarsFrame();
}

GvarsFrame::~GvarsFrame()
{
	//(*Destroy(GvarsFrame)
	//*)
}

void GvarsFrame::PopulateGvarsFrame()
{
                                       //#define CFN_GVAR_CST_MAX    125 (from myeeprom.h)
                                       //#define MAX_GVARS            5
                                       //#define MAX_FLIGHT_MODES     6

  for (int8_t p = 0; p < MAX_FLIGHT_MODES; p++){
    for (int8_t x = 0; x < MAX_GVARS; x++){
      int16_t gvarVal = (g_model.flightModeData[p].gvars[x]);
      wxString gvarStr;
      if (gvarVal == 0) gvarStr = "---";
      else if (gvarVal >= (CFN_GVAR_CST_MAX + MAX_GVARS - 1)){
        gvarStr = "*PV";
        if (p <= (gvarVal - (CFN_GVAR_CST_MAX + MAX_GVARS - 1))){
         ++gvarVal;                    // Flight phase can't refer to himself. Skip one phase.
        }
        gvarStr.Append (wxString::Format(wxT("%i"),(gvarVal - (CFN_GVAR_CST_MAX + MAX_GVARS - 1))));
      }
      else gvarStr.Append (wxString::Format(wxT("%i"),(gvarVal)));

      GVARSGrid->SetCellValue(x,p,gvarStr);
    }
  }
}

void GvarsFrame::OnClose(wxCloseEvent& event)
{
  OpenAVRc_SimulatorFrame *parent = wxDynamicCast(this->GetParent(), OpenAVRc_SimulatorFrame);
  if(parent)
    parent->EnableOutputGvarsMenu();
  Destroy();
}

void GvarsFrame::OngvarsTimerTrigger(wxTimerEvent& event)
{
  PopulateGvarsFrame();
}
