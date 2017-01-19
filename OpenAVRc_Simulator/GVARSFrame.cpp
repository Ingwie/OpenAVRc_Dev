#include "GVARSFrame.h"
//#include "myeeprom.h" //not found (?)
#include "OpenAVRc_SimulatorMain.h"
#include <wx/msgdlg.h>

void FillGvarsFrame();

//(*InternalHeaders(GVARSFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(GVARSFrame)
const long GVARSFrame::ID_COMBOBOX1 = wxNewId();
const long GVARSFrame::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(GVARSFrame,wxFrame)
	//(*EventTable(GVARSFrame)
	//*)
END_EVENT_TABLE()

GVARSFrame::GVARSFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(GVARSFrame)
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
	SetClientSize(wxSize(399,308));
	Move(wxDefaultPosition);
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(216,208), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 3, 0, 0);
	ComboBoxFlightMode0 = new wxComboBox(Panel1, ID_COMBOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX1"));
	FlexGridSizer1->Add(ComboBoxFlightMode0, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Panel1->SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(Panel1);
	FlexGridSizer1->SetSizeHints(Panel1);

	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&GVARSFrame::OnClose);
	//*)

	FillGvarsFrame();

}

 GVARSFrame::FillGvarsFrame()
{
  //ComboBoxFlightMode0->SetValue("mododevuelo");//this should be the name of the flighphase0 (flightmode
  //char phaseName[LEN_FLIGHT_MODE_NAME];
  //int phaseName;
  //ModelData * phaseName;
  //phaseName = FlightModeData[1].name[];

  //int Name = phaseName->protocol;
  //  g_model.flightModeData[1];
  wxString phaseName = g_model.header.name;
  //char phaseName = g_model.flightModeData[1].name[LEN_FLIGHT_MODE_NAME];
  //ComboBoxFlightMode0->SetValue(phaseName);
  //wxString phaseName(phase, wxConvUTF8);
  //wxString Nam = wxString::FromUTF8(&phaseName);
  //wxString mystring = wxString::Format(wxT("%ii"),Name);
  //wxString mystring;
  //mystring << Name;
  wxMessageBox(phaseName);

  //FlightModeData *flightModeAddress(uint8_t idx)
//{
  //return &g_model.flightModeData[idx];
//}
  //g_model.flightModeData[mixerCurrentFlightMode].name

  //char phaseName[LEN_FLIGHT_MODE_NAME];
  //int phaseName;
  //char phaseName = "012345";
  //phaseName = FlightModeData[1].name[];
  //ComboBoxFlightMode0->SetValue(phaseName);
  //FlightModeData *phase = &g_model.flightModeData[i];
  //wxMessageBox(g_model.flightModeData[0]);
  //#define GVAR_VALUE(x, p) g_model.flightModeData[p].gvars[x]
  //return &g_model.flightModeData[idx];
  //FlightModeData *p = flightModeAddress(phase);
  //myeeprom.h //uint8_t flightModes:6;

  /*
  PHASE_GVARS_DATA;
  PACK(typedef struct {
  TRIMS_ARRAY;
  int8_t swtch;       // swtch of phase[0] is not used
  char name[LEN_FLIGHT_MODE_NAME];
  uint8_t fadeIn:4;
  uint8_t fadeOut:4;
  ROTARY_ENCODER_ARRAY;
  PHASE_GVARS_DATA;
}) FlightModeData;

 extern ModelData g_model;
*/
}


GVARSFrame::~GVARSFrame()
{
	//(*Destroy(GVARSFrame)
	//*)
}


void GVARSFrame::OnClose(wxCloseEvent& event)
{
  Destroy();
}
