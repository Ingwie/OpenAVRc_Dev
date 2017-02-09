#include "RadioDataFrame.h"
#include "OpenAVRc_SimulatorMain.h"

wxString vTxt;

//(*InternalHeaders(RadioDataFrame)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(RadioDataFrame)
const long RadioDataFrame::ID_TEXTCTRLVERSION = wxNewId();
const long RadioDataFrame::ID_TEXTCTRL2 = wxNewId();
const long RadioDataFrame::ID_TEXTCTRL3 = wxNewId();
const long RadioDataFrame::ID_TEXTCTRLBATT = wxNewId();
const long RadioDataFrame::ID_TEXTCTRLSTICKMODE = wxNewId();
const long RadioDataFrame::ID_TEXTCTRLCalibration = wxNewId();
const long RadioDataFrame::ID_STATICTEXT1 = wxNewId();
const long RadioDataFrame::ID_STATICTEXT2 = wxNewId();
const long RadioDataFrame::ID_PANEL1 = wxNewId();
const long RadioDataFrame::ID_TIMERRADIODATA = wxNewId();
//*)

BEGIN_EVENT_TABLE(RadioDataFrame,wxFrame)
	//(*EventTable(RadioDataFrame)
	//*)
END_EVENT_TABLE()

RadioDataFrame::RadioDataFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(RadioDataFrame)
	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(209,297));
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(184,128), wxSize(400,288), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	TextCtrlversion = new wxTextCtrl(Panel1, ID_TEXTCTRLVERSION, _("Texte"), wxPoint(8,8), wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRLVERSION"));
	TextCtrlvariant = new wxTextCtrl(Panel1, ID_TEXTCTRL2, _("Texte"), wxPoint(112,8), wxSize(88,21), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	TextCtrlcurrModel = new wxTextCtrl(Panel1, ID_TEXTCTRL3, _("Texte"), wxPoint(8,32), wxDefaultSize, wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	TextCtrlBatt = new wxTextCtrl(Panel1, ID_TEXTCTRLBATT, _("Texte"), wxPoint(8,88), wxSize(192,21), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRLBATT"));
	TextCtrlstickMode = new wxTextCtrl(Panel1, ID_TEXTCTRLSTICKMODE, _("Texte"), wxPoint(8,112), wxSize(144,21), wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRLSTICKMODE"));
	TextCtrlcalibration = new wxTextCtrl(Panel1, ID_TEXTCTRLCalibration, _("Texte"), wxPoint(8,176), wxSize(184,104), wxTE_NO_VSCROLL|wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRLCalibration"));
	TextCtrlcalibration->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("                     Max         Min       Alarm"), wxPoint(8,64), wxSize(192,24), 0, _T("ID_STATICTEXT1"));
	StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("Calibration"), wxPoint(16,152), wxSize(168,21), wxALIGN_CENTRE, _T("ID_STATICTEXT2"));
	RadioData.SetOwner(this, ID_TIMERRADIODATA);
	RadioData.Start(2000, false);
	//*)

	FillRadioDataFrame();
}

RadioDataFrame::~RadioDataFrame()
{
	//(*Destroy(RadioDataFrame)
	//*)
}

void RadioDataFrame::FillRadioDataFrame()
{
  TextCtrlversion->SetValue(vTxt = (_T("Version:\t")) + wxString::Format(wxT("%i"),(g_eeGeneral.version)));
  TextCtrlvariant->SetValue(vTxt = (_T("Variante:\t")) + wxString::Format(wxT("%i"),(g_eeGeneral.variant)));
  TextCtrlcurrModel->SetValue(vTxt = (_T("Modele:\t")) + wxString::Format(wxT("%i"),(g_eeGeneral.currModel)));

  vTxt = _T("Batterie\t") + wxString::Format(wxT("%i"),(g_eeGeneral.vBatMax)+120) + "\t";
  vTxt = vTxt.Append(wxString::Format(wxT("%i"),(g_eeGeneral.vBatMin)+90) + "\t");
  vTxt = vTxt.Append(wxString::Format(wxT("%i"),(g_eeGeneral.vBatWarn)));
  TextCtrlBatt->SetValue(vTxt);

  TextCtrlstickMode->SetValue(vTxt = (_T("Mode:\t")) + wxString::Format(wxT("%i"),(g_eeGeneral.stickMode)+1));

  vTxt = "";
  for (int8_t i = 0; i < (NUM_STICKS+NUM_POTS); i++){
    CalibData * calib = &g_eeGeneral.calib[i];
    vTxt.Append(wxString::Format(wxT("%i"),(calib->spanNeg)) + "\t");
    vTxt.Append(wxString::Format(wxT("%i"),(calib->mid)) + "\t");
    vTxt.Append(wxString::Format(wxT("%i"),(calib->spanPos)) + "\n");
    TextCtrlcalibration->SetValue(vTxt);
  }


}


/*PACK(typedef struct {
                                     uint8_t   version;;
                                     uint16_t  variant;
  CalibData calib[NUM_STICKS+NUM_POTS];
                                                  uint16_t  chkSum;
                                                  int8_t    currModel;
  uint8_t   contrast;
                                                  uint8_t   vBatWarn;
  int8_t    txVoltageCalibration;
  int8_t    backlightMode;
  TrainerData trainer;
  uint8_t   view;            // index of view in main screen
  int8_t    buzzerMode:2;    // -2=quiet, -1=only alarms, 0=no keys, 1=all
  uint8_t   fai:1;
  int8_t    beepMode:2;      // -2=quiet, -1=only alarms, 0=no keys, 1=all
  uint8_t   alarmsFlash:1;
  uint8_t   disableMemoryWarning:1;
  uint8_t   disableAlarmWarning:1;
                                            uint8_t   stickMode:2;
  int8_t    timezone:5;
  uint8_t   adjustRTC:1;
  uint8_t   inactivityTimer;
  uint8_t   mavbaud:3;
  SPLASH_MODE; /* 3bits
  int8_t    hapticMode:2;    // -2=quiet, -1=only alarms, 0=no keys, 1=all
  uint8_t blOffBright:4;
  uint8_t blOnBright:4;
  uint8_t   lightAutoOff;
  uint8_t   templateSetup;   // RETA order for receiver channels
  int8_t    PPM_Multiplier;
  int8_t    hapticLength;
  N_PCBSTD_FIELD( uint8_t   reNavigation)
  uint8_t   stickReverse;
  int8_t    beepLength:3;
  int8_t    hapticStrength:3;
  uint8_t   gpsFormat:1;
  uint8_t   unexpectedShutdown:1;
  uint8_t   speakerPitch;
  int8_t    speakerVolume;
                                                 int8_t    vBatMin;
                                                 int8_t    vBatMax;

  EXTRA_GENERAL_FIELDS

}) EEGeneral;
*/
