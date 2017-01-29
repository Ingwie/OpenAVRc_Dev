#include "MixerFrame.h"
#include "OpenAVRc_SimulatorMain.h"

#include <wx/msgdlg.h>
#include <wx/arrstr.h>
#include <wx/string.h>

wxString mixStr = "";
extern wxString modeStr = "";

//(*InternalHeaders(MixerFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(MixerFrame)
const long MixerFrame::ID_TEXTCTRL1 = wxNewId();
const long MixerFrame::ID_PANEL1 = wxNewId();
const long MixerFrame::ID_TIMERREFRESHFRAME = wxNewId();
//*)

BEGIN_EVENT_TABLE(MixerFrame,wxFrame)
	//(*EventTable(MixerFrame)
	//*)
END_EVENT_TABLE()

MixerFrame::MixerFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(MixerFrame)
	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxVSCROLL|wxFULL_REPAINT_ON_RESIZE, _T("wxID_ANY"));
	SetClientSize(wxSize(738,308));
	Mixer = new wxPanel(this, ID_PANEL1, wxPoint(256,200), wxSize(1104,320), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	Mixerline1 = new wxTextCtrl(Mixer, ID_TEXTCTRL1, _("Texte"), wxPoint(0,0), wxSize(1104,320), wxTE_MULTILINE|wxTE_RICH, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	TimerRefreshFrame.SetOwner(this, ID_TIMERREFRESHFRAME);
	TimerRefreshFrame.Start(500, false);

	Connect(ID_TIMERREFRESHFRAME,wxEVT_TIMER,(wxObjectEventFunction)&MixerFrame::OnTimerRefreshFrameTrigger);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&MixerFrame::OnClose);
	//*)

  FillMixerFrame();
}

MixerFrame::~MixerFrame()
{
	//(*Destroy(MixerFrame)
	//*)
}

void MixerFrame::OnClose(wxCloseEvent& event)
{
  Destroy();
}

////////////////////////TOOLS FOR FLIGHT MODES////////////////

void ConvertToBinary(int n, wxString) //model flight modes in binary
{
  if (n / 2 != 0) {
    ConvertToBinary((n / 2), modeStr);
  }
  modeStr = modeStr + (wxString::Format(wxT("%d"),n % 2));
}


wxString verlen(const wxString &strSource)//reverse flight modes binary and change on screen presentation.
{
  wxString strTarget;
  for ( wxString::const_reverse_iterator it = strSource.rbegin(); it != strSource.rend(); ++it ){
    strTarget.Append( *it );
  }
  if (strTarget.Mid(0,1) == "1") modeStr = "__"; else modeStr = "0_";
  if (strTarget.Mid(1,1) == "1") modeStr = modeStr + "__"; else modeStr = modeStr + "1_";
  if (strTarget.Mid(2,1) == "1") modeStr = modeStr + "__"; else modeStr = modeStr + "2_";
  if (strTarget.Mid(3,1) == "1") modeStr = modeStr + "__"; else modeStr = modeStr + "3_";
  if (strTarget.Mid(4,1) == "1") modeStr = modeStr + "__"; else modeStr = modeStr + "4_";
  if (strTarget.Mid(5,1) == "1") modeStr = modeStr + "__"; else modeStr = modeStr + "5";
  return strTarget;
}


///////////////////////////END OF TOOLS TO FLIGHT MODES/////////////

  //wxArrayString *sticks = new wxArrayString;
  //wxArrayString Src;
  //Src.Add(wxString::Format(wxT("%d"),"Rud")); //scr does not name a type error (??)
  //wxString Rud ="RUDDER";
  //sticks->Add(_("Rud"));
  //Src.Add(wxT("Ele"));
  //Src.GetCount();
  //Src.Add(Rud);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* enum MixSources {
  MIXSRC_NONE,
  MIXSRC_FIRST_STICK,
  MIXSRC_Rud = MIXSRC_FIRST_STICK,
  MIXSRC_Ele,
  MIXSRC_Thr,
  MIXSRC_Ail,

  MIXSRC_FIRST_POT,
                                                 MIXSRC_P1 = MIXSRC_FIRST_POT,
  MIXSRC_P2,
  MIXSRC_P3,
  MIXSRC_LAST_POT = MIXSRC_P3,

#if   defined(CPUM2560)
  MIXSRC_REa, //8
  MIXSRC_REb,
#if ROTARY_ENCODERS > 2
  MIXSRC_REc,
  MIXSRC_REd,
                                                 MIXSRC_LAST_ROTARY_ENCODER = MIXSRC_REd,
#else
                                                 MIXSRC_LAST_ROTARY_ENCODER = MIXSRC_REb,
#endif
#endif

  MIXSRC_MAX, //10

  MIXSRC_FIRST_HELI,
                                                 MIXSRC_CYC1 = MIXSRC_FIRST_HELI,
  MIXSRC_CYC2,
  MIXSRC_CYC3,

  MIXSRC_FIRST_TRIM,
                                                MIXSRC_TrimRud = MIXSRC_FIRST_TRIM,
  MIXSRC_TrimEle,
  MIXSRC_TrimThr,
  MIXSRC_TrimAil,
  MIXSRC_LAST_TRIM = MIXSRC_TrimAil,

  MIXSRC_FIRST_SWITCH,

                                                 MIXSRC_3POS = MIXSRC_FIRST_SWITCH,
  MIXSRC_THR,
  MIXSRC_RUD,
  MIXSRC_ELE,
  MIXSRC_AIL,
  MIXSRC_GEA,
  MIXSRC_TRN,
  MIXSRC_LAST_SWITCH = MIXSRC_TRN,
  MIXSRC_FIRST_LOGICAL_SWITCH,
                                                  MIXSRC_SW1 = MIXSRC_FIRST_LOGICAL_SWITCH,
  MIXSRC_SW9 = MIXSRC_SW1 + 8,
  MIXSRC_SWA,
  MIXSRC_SWB,
  MIXSRC_SWC,
  MIXSRC_LAST_LOGICAL_SWITCH = MIXSRC_FIRST_LOGICAL_SWITCH+NUM_LOGICAL_SWITCH-1,

  MIXSRC_FIRST_TRAINER,
  MIXSRC_LAST_TRAINER = MIXSRC_FIRST_TRAINER+NUM_TRAINER-1,

  MIXSRC_FIRST_CH,
                                                   MIXSRC_CH1 = MIXSRC_FIRST_CH,
  MIXSRC_CH2,
  MIXSRC_CH3,
  MIXSRC_CH4,
  MIXSRC_CH5,
  MIXSRC_CH6,
  MIXSRC_CH7,
  MIXSRC_CH8,
  MIXSRC_CH9,
  MIXSRC_CH10,
  MIXSRC_CH11,
  MIXSRC_CH12,
  MIXSRC_CH13,
  MIXSRC_CH14,
  MIXSRC_CH15,
  MIXSRC_CH16,
  MIXSRC_LAST_CH = MIXSRC_CH1+NUM_CHNOUT-1,

  MIXSRC_FIRST_GVAR,
                                                          MIXSRC_GVAR1 = MIXSRC_FIRST_GVAR,
  MIXSRC_LAST_GVAR = MIXSRC_FIRST_GVAR+MAX_GVARS-1,


                                                          MIXSRC_FIRST_TELEM,
  MIXSRC_LAST_TELEM = MIXSRC_FIRST_TELEM+NUM_TELEMETRY-1
};
*/

 void MixerFrame::FillMixerFrame()
 {
  wxString mixStr = "\t\t\tMix\tOffset\tSwitch\tExpo\tCurve\tModes\t\tTrim\tWarning\tDlay(u/d)Speed(u/d)\n";
  for( int i = 0; i < NUM_CHNOUT; i++ ) {
    if ((g_model.mixData[i].weight) == 0) continue;
    if ((i == 0) || ((g_model.mixData[i].destCh) > (g_model.mixData[i-1].destCh))){
      mixStr = mixStr + "\n" + "OUT " + wxString::Format(wxT("%i"),(g_model.mixData[i].destCh) + 1)+ " = ";
    }
    else mixStr = mixStr + "\t";// Destination channel OK


    if ((g_model.mixData[i].mltpx) == 2) mixStr = mixStr + "Over ";
    else if ((g_model.mixData[i].mltpx) == 1) mixStr = mixStr + "Mult ";
    else if ((g_model.mixData[i].mltpx) == 0) mixStr = mixStr + "Add  ";// operator OK

    int indx = (g_model.mixData[i].srcRaw);
    if (indx == MIXSRC_FIRST_POT-4) mixStr = mixStr + "Rud input" + "\t";
    else if (indx == MIXSRC_FIRST_POT - 3) mixStr = mixStr + "Ele input" + "\t";
    else if (indx == MIXSRC_FIRST_POT - 2) mixStr = mixStr + "Thr input" + "\t";
    else if (indx == MIXSRC_FIRST_POT - 1) mixStr = mixStr + "Ail input" + "\t";

    else if (indx == MIXSRC_FIRST_POT) mixStr = mixStr + "P1\t\t";
    else if (indx == MIXSRC_FIRST_POT + 1) mixStr = mixStr + "P2\t\t";
    else if (indx == MIXSRC_FIRST_POT + 2) mixStr = mixStr + "P3\t\t";

    else if (indx == MIXSRC_LAST_ROTARY_ENCODER - 1) mixStr = mixStr + "REa\t\t";//review as per PERSONAMES.
    else if (indx == MIXSRC_LAST_ROTARY_ENCODER) mixStr = mixStr + "REb\t\t";

    else if (indx == MIXSRC_LAST_ROTARY_ENCODER + 1) mixStr = mixStr + "MAX\t\t";

    else if (indx == MIXSRC_FIRST_HELI) mixStr = mixStr + "CYC1";
    else if (indx == MIXSRC_FIRST_HELI + 1) mixStr = mixStr + "CYC2";
    else if (indx == MIXSRC_FIRST_HELI + 2) mixStr = mixStr + "CYC3";

    else if (indx == MIXSRC_FIRST_TRIM) mixStr = mixStr + "TrimRud";
    else if (indx == MIXSRC_FIRST_TRIM + 1) mixStr = mixStr + "TrimEle";
    else if (indx == MIXSRC_FIRST_TRIM + 2) mixStr = mixStr + "TrimThr";
    else if (indx == MIXSRC_FIRST_TRIM + 3) mixStr = mixStr + "TrimAil";

    else if (indx == MIXSRC_FIRST_SWITCH) mixStr = mixStr + "3POS"; //TR_9X_3POS_SWITCHES ????????????

    //else if (indx == MIXSRC_FIRST_SWITCH) mixStr = mixStr + TR_9X_3POS_SWITCHES[indx] + "\t"; //????????????
    ////////////////////////////////////////continue from here////////////////////////////
    //PHYSICAL SWITCHES   MIXSRC_FIRST_SWITCH
    //LOGICAL SWITCHES
    //TRAINER
    //CHANNELS
    //GVARS
    //TELEMETRY

    else (mixStr = mixStr + wxString::Format(wxT("%i"),(g_model.mixData[i].srcRaw))) + "\t";
    // TODO create a wxarraystring or similar to combine Mixsources and TR_PHYS_SWITCHES.


    mixStr = mixStr + wxString::Format(wxT("%i"),(g_model.mixData[i].weight)) + "% "; //weight OK


    mixStr = mixStr + "\t" + wxString::Format(wxT("%i"),(g_model.mixData[i].offset)) +"%\t";// offset OK


    if ((g_model.mixData[i].swtch)  > 4){
      for (int j = 0; j < 3; j++){
        mixStr = mixStr + TR_PHYS_SWITCHES[j + 3 * ((g_model.mixData[i].swtch)-4)];
      }
      mixStr = mixStr + "\t";
    }
    else mixStr = mixStr + "\t";// switch OK

      //if ((g_model.mixData[i].curveMode) == 0) mixStr = mixStr + "CURVE " + ",";
       //else mixStr = mixStr + "DIFF  " + ",";
      //mixStr = mixStr +   " CURVEmode " + wxString::Format(wxT("%i"),(g_model.mixData[i].curveMode)) + ",";
      //mixStr = mixStr +            "EXPO " + wxString::Format(wxT("%i"),(g_model.mixData[i].noExpo)) + ",";
      //mixStr = mixStr + "CURVE PARAM " + wxString::Format(wxT("%i"),(g_model.mixData[i].curveParam));

    if (((g_model.mixData[i].curveMode) == 0) && ((g_model.mixData[i].noExpo) == 0)){
      mixStr = mixStr + wxString::Format(wxT("%i"),(g_model.mixData[i].curveParam))+ "%\t\t" ;//MORE TODO WITH CURVE PARAM.
    }
    else mixStr = mixStr + "\t" + wxString::Format(wxT("%i"),(g_model.mixData[i].curveParam)-5)+ "\t" ;//to review, not fully working

      //mixStr = mixStr + " " + wxString::Format(wxT("%i"),(g_model.mixData[i].weightMode)) + ","; //??????
      //mixStr = mixStr + " " + wxString::Format(wxT("%i"),(g_model.mixData[i].offsetMode)) + ","; //??????


    ConvertToBinary(g_model.mixData[i].flightModes,mixStr);// TODO improve the output to make it comprehensible.
    modeStr = verlen(modeStr);
    if (modeStr == "") modeStr = "All";
    mixStr = mixStr + modeStr + "\t";
    modeStr ="";

    //mixStr = mixStr + wxString::Format(wxT("%i"),(g_model.mixData[i].flightModes)) + "\t";//TODO translate to binary and invert.


    indx = (g_model.mixData[i].carryTrim);
    if ((indx) == 1) mixStr = mixStr + "Off" + "\t";
    else if ((indx) == 0) mixStr = mixStr + "On" + "\t";
    else if ((indx) == -1) mixStr = mixStr + "RUD" + "\t";
    else if ((indx) == -2) mixStr = mixStr + "ELE" + "\t";
    else if ((indx) == -3) mixStr = mixStr + "THR" + "\t";
    else if ((indx) == -4) mixStr = mixStr + "AIL" + "\t";//OK


    mixStr = mixStr + wxString::Format(wxT("%i"),(g_model.mixData[i].mixWarn)) + "\t";// IS THIS NECESSARY FOR THIS SCREEN ??


    mixStr = mixStr + "(" + wxString::Format(wxT("%i"),(g_model.mixData[i].delayUp));
    mixStr = mixStr + "/" + wxString::Format(wxT("%i"),(g_model.mixData[i].delayDown)) + ")\t";
    mixStr = mixStr + "(" + wxString::Format(wxT("%i"),(g_model.mixData[i].speedUp));
    mixStr = mixStr + "/" + wxString::Format(wxT("%i"),(g_model.mixData[i].speedDown)) + ")"+ "\n";//Dlay u/d Speed u/d OK


    //mixStr = mixStr + " " + wxString::Format(wxT("%i"),(g_model.mixData[i].spare));

  }
  Mixerline1->SetValue(mixStr);
 }





void MixerFrame::OnTimerRefreshFrameTrigger(wxTimerEvent& event)
{
  FillMixerFrame();
}
