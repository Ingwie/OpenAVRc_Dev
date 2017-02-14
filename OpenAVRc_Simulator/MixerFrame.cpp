#include "MixerFrame.h"
#include "OpenAVRc_SimulatorMain.h"

#include <wx/msgdlg.h>

wxString mixStr = "";
extern wxString modeStr = "";


#define data g_model.mixData


class INPUTClass {
public:
    const static char* inputText[];
};

const char* INPUTClass::inputText[] = { "Dir\t","Prf\t","Gaz\t","Ail\t",
            "POT1","POT2","POT3",
            "REa\t","REb\t",
            "MAX\0",
            "CYC1","CYC2","CYC3",
            "TrmD","TrmP","TrmG","TrmA",
            "3POS","THR","RUD","ELE\t","AIL\t","GEA","TRN",
            "L1\t","L2\t","L3\t","L4\t","L5\t","L6\t","L7\t","L8\t","L9\t","L10\t","L11\t","L12\t",
            "TR1","TR2","TR3","TR4","TR5","TR6","TR7","TR8",
            "CH1","CH2","CH3","CH4","CH5","CH6","CH7","CH8","CH9","CH10","CH11","CH12","CH13","CH14","CH15","CH16"};


//(*InternalHeaders(MixerFrame)
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(MixerFrame)
const long MixerFrame::ID_TEXTCTRLHEADERLINE = wxNewId();
const long MixerFrame::ID_TEXTCTRLMIXERLINE = wxNewId();
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
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Mixeur"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxCLOSE_BOX|wxSUNKEN_BORDER|wxRAISED_BORDER|wxFULL_REPAINT_ON_RESIZE, _T("wxID_ANY"));
	SetClientSize(wxSize(720,140));
	Move(wxPoint(20,20));
	SetMaxSize(wxSize(720,900));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, Panel1, wxEmptyString);
	Headerline = new wxTextCtrl(Panel1, ID_TEXTCTRLHEADERLINE, _("Texte"), wxDefaultPosition, wxSize(623,31), wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxTE_LEFT, wxDefaultValidator, _T("ID_TEXTCTRLHEADERLINE"));
	Headerline->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
	StaticBoxSizer1->Add(Headerline, 0, wxALL|wxEXPAND, 2);
	Mixerline1 = new wxTextCtrl(Panel1, ID_TEXTCTRLMIXERLINE, _("Texte"), wxDefaultPosition, wxSize(725,67), wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxTE_LEFT, wxDefaultValidator, _T("ID_TEXTCTRLMIXERLINE"));
	Mixerline1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
	StaticBoxSizer1->Add(Mixerline1, 1, wxALL|wxEXPAND, 2);
	Panel1->SetSizer(StaticBoxSizer1);
	StaticBoxSizer1->Fit(Panel1);
	StaticBoxSizer1->SetSizeHints(Panel1);
	BoxSizer1->Add(Panel1, 1, wxALL|wxEXPAND, 5);
	SetSizer(BoxSizer1);
	TimerRefreshFrame.SetOwner(this, ID_TIMERREFRESHFRAME);
	TimerRefreshFrame.Start(500, false);
	SetSizer(BoxSizer1);
	Layout();

	Connect(ID_TIMERREFRESHFRAME,wxEVT_TIMER,(wxObjectEventFunction)&MixerFrame::OnTimerRefreshFrameTrigger);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&MixerFrame::OnClose);
	//*)

  {
    wxIcon FrameIcon;
    SetIcon(wxICON(nsrcs_icon));
  }

  FillMixerFrame();
}

MixerFrame::~MixerFrame()
{
	//(*Destroy(MixerFrame)
	//*)
}

void MixerFrame::OnClose(wxCloseEvent& event)
{
 OpenAVRc_SimulatorFrame *parent = wxDynamicCast(this->GetParent(), OpenAVRc_SimulatorFrame);
if(parent)
    parent->EnableMixerFrameMenu();
 Destroy();
}

wxString int2wxString(int integer)
{
  wxString intString = wxString::Format(wxT("%i"), integer);
  return intString;
  //wxMessageBox(intString);
}

//////////////////////// TOOLS FOR FLIGHT MODES ////////////////

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
  if (strTarget.Mid(1,1) == "1") modeStr.Append("__"); else modeStr.Append("1_");
  if (strTarget.Mid(2,1) == "1") modeStr.Append("__"); else modeStr.Append("2_");
  if (strTarget.Mid(3,1) == "1") modeStr.Append("__"); else modeStr.Append("3_");
  if (strTarget.Mid(4,1) == "1") modeStr.Append("__"); else modeStr.Append("4_");
  if (strTarget.Mid(5,1) == "1") modeStr.Append("__"); else modeStr.Append("5");
  return modeStr;
}


 void MixerFrame::FillMixerFrame()
{
  wxString Header = "\t\t\tMix\tOffset\tSwitch\tDiff\tModes\t\tTrim\tDR/Expo\tDlay(u/d)Speed(u/d)\0";
  Headerline->SetValue(Header);
  wxString mixerL;
  wxString txt;



  for( uint8_t i = 0; i < MAX_MIXERS; i++ ) {
    mixerL = "";
    mixStr1 = "";
    //------------------------------------- OUTPUT CHANNEL-------------------------------------
      if (((data[i].weight) == 0) && ((data[i].weightMode) != 1)) continue;
      if (i == 0) mixStr1 = mixStr1 + "OUT " + int2wxString((data[i].destCh) + 1)+ " = ";
      else if ((data[i].destCh) > (data[i-1].destCh)){
      mixStr1 = mixStr1 + "\n" +"OUT " + int2wxString((data[i].destCh) + 1) + " = ";
      }
      else mixStr1 = mixStr1 + "\t";

    //---------------------------------------OPERATOR--------------------------------------
    mixStr2 = "";
    if ((data[i].mltpx) == 2) mixStr2 = "Over ";
    else if ((data[i].mltpx) == 1) mixStr2 = "Mult ";
    else if ((data[i].mltpx) == 0) mixStr2 = "Add  ";// operator OK

    //---------------------------------------SOURCE-----------------------------------
    mixStr4 = "";
    int indx = (data[i].srcRaw);
    mixStr4 = mixStr4 + INPUTClass::inputText[indx-1] + "\t";

    /*for (int j = 0; j < 3; j++){
    //if (TR_SOURCE[j + 3 * indx] == '\0') continue;
    //mixStr4 = mixStr4 + TR_SOURCE[j + 3 * indx];
    //}
    //mixStr = mixStr + TR_VSRCRAW[j + 4 * indx];
    //}
    */
    //---------------------------------------------WEIGHT-------------------------------------------
    mixStr6 = "";
    mixStr7 = "mix7";
    int weight = (data[i].weight);
    int8_t mode = (data[i].weightMode);
    wxString percent = "%";

    weight = u_gvarint_t(data[i].weight, data[i].weightMode).gvword; // Work but GVAR bug now :(

    if (weight > GV_RANGELARGE + MAX_GVARS){
      mixStr6 = mixStr6 + "-" + STR_GV;
      weight = 256 - weight;
      percent = "";
    }
    else if (weight < GV_RANGELARGE_NEG - MAX_GVARS){
      mixStr6 = mixStr6 + STR_GV;
      weight = 256 + 1 + weight;
      percent = "";
    }
    mixStr7 = int2wxString(weight) + percent + "\t";// + int2wxString (GV_RANGELARGE);

    //---------------------------------------------OFFSET-----------------------------------------------
    mixStr8 = "";
    mixStr9 = "---\t";
    int8_t offset = (data[i].offset);
    mode = (data[i].offsetMode);
    percent = "%";

    if ((mode == 1) && (offset >= 0)){
      mixStr8 = mixStr8 + STR_GV;
      offset = offset + 1;
      percent = "";
    }
    else if ((mode == 0) && (offset < 0)){
      mixStr8 = mixStr8 + "-" + STR_GV;
      offset = abs(offset);
      percent = "";
    }
    mixStr9 = int2wxString(offset) + percent + "\t";

    //---------------------------------------------SWITCHES-----------------------------------------------

    wxString mixStr10 = "";
    wxString mixStr11 = "";

    int idx = (data[i].swtch);
    if (idx < 0){
      mixStr10 = "!";
      idx = abs(idx);
    }

    for (int j = 0; j < 3; j++){
      if (STR_VSWITCHES[1 + j + 3 * idx] == '\0') continue;
      mixStr11.Append(STR_VSWITCHES[1 + j + 3 * idx]);
    }
    mixStr11.Append("\t");

    //----------------------------------------------------CURVE-----------------------------------------

    mixStr14 = "";
    int ind = (data[i].curveParam);
    if ((data[i].curveMode) == 0){
      if (ind > 122){
        ind = - ind + 128;
        mixStr14 = "-";
        mixStr14 = mixStr14 + STR_GV + int2wxString(ind);
      }
      else if (ind < -123){
        ind = ind + 129;
        mixStr14 = STR_GV + int2wxString(ind);
      }
      else mixStr14 = mixStr14 + int2wxString(ind) +"%";
      mixStr14.Append("\t");
    }

    else {
      ind = (data[i].curveParam);
      //mixStr14.Append("\t");
      if (ind < 0){
        mixStr14.Append("!");
        ind = -ind + CURVE_BASE-1;
      }
      if(ind < CURVE_BASE){
        for (int j = 0; j < 3; j++){
          mixStr14 = mixStr14 + TR_VCURVEFUNC[ j + 3 * (ind)];
        }
      }
      else mixStr14 = mixStr14 + STR_CV + int2wxString(ind-6);

      mixStr14.Append("\t");
    }

    //---------------------------------------------FLIGHT MODES-------------------------------------------------------
    mixStr16 = "";
    ConvertToBinary(data[i].flightModes,mixStr16);
    modeStr = verlen(modeStr);
    mixStr16 = modeStr + "\t";
    modeStr ="";

    //-----------------------------------------------TRIM-----------------------------------------------

    const char * TRIMS[] = { "Ail", "Thr", "Ele", "Rud", "On", "---"};// This shall be somewhere with translation.

    mixStr18 = "";
    indx = (data[i].carryTrim);
    if (((data[i].srcRaw) > 4) && (indx == 0)) indx = 1;// TRIM OFF if input is not a stick

    mixStr18.Append(TRIMS[indx + 4]);
    mixStr18.Append("\t");

    //-----------------------------------------------DUAL RATE /EXPO-------------------------------------------
    mixStr19 = "";
    mixStr19.Append(int2wxString((data[i].noExpo)) + "\t");
    //-----------------------------------------DELAY/SLOW---------------------------------------------
    mixStr20 = "";
    mixStr20.Append("(" + int2wxString(data[i].delayUp / 2));
    mixStr20.Append("," + int2wxString((data[i].delayUp % 2) * 5));
    mixStr20.Append("/" + int2wxString(data[i].delayDown / 2));
    mixStr20.Append("," + int2wxString((data[i].delayDown % 2) * 5) + ")");
    mixStr20.Append("(" + int2wxString(data[i].speedUp / 2));
    mixStr20.Append("," + int2wxString((data[i].speedUp % 2) * 5));
    mixStr20.Append("/" + int2wxString(data[i].speedDown / 2));
    mixStr20.Append("," + int2wxString((data[i].speedDown % 2) * 5) + ")");
    //-------------------------------------------------------------------------------------------------------
    //mixStr22 = "";
    //mixStr22 = mixStr22 + wxString::Format(wxT("%i"),(data[i].mixWarn));// IS THIS NECESSARY FOR THIS SCREEN ??
    //-------------------------------------------------------------------------------------------------------

    //wxString mixStr24 = "";
    //mixStr24 = " " + wxString::Format(wxT("%i"),(data[i].spare));

    mixerL = mixStr1 +mixStr2 +mixStr4 +mixStr6 +mixStr7 +mixStr8 +mixStr9 +mixStr10 +mixStr11 +mixStr14 +mixStr16
            +mixStr18 +mixStr19 +mixStr20;

    txt.Append(mixerL + "\n");
  }
  txt.Append("\0");
  Mixerline1->SetValue(txt);
}

void MixerFrame::OnTimerRefreshFrameTrigger(wxTimerEvent& event)
{
  FillMixerFrame();
}
