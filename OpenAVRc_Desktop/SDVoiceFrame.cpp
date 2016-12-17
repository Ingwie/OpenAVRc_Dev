#include "SDVoiceFrame.h"
#include <wx/dirctrl.h>
#include "wx/wx.h"

extern wxString SDslot = "f:";
extern wxString AppPath;
extern wxString postfix = "";
extern wxString fromPath;
wxString outPath ="";

//(*InternalHeaders(SDVoiceFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(SDVoiceFrame)
const long SDVoiceFrame::ID_STATICBOX2 = wxNewId();
const long SDVoiceFrame::ID_STATICBOX1 = wxNewId();
const long SDVoiceFrame::ID_BUTTON1 = wxNewId();
const long SDVoiceFrame::ID_CHOICE1 = wxNewId();
const long SDVoiceFrame::ID_COMBOBOX1 = wxNewId();
const long SDVoiceFrame::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(SDVoiceFrame,wxFrame)
	//(*EventTable(SDVoiceFrame)
	//*)
END_EVENT_TABLE()

SDVoiceFrame::SDVoiceFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(SDVoiceFrame)
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
	SetClientSize(wxSize(262,289));
	Move(wxDefaultPosition);
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(232,184), wxSize(322,296), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	StaticBox2 = new wxStaticBox(Panel1, ID_STATICBOX2, _("Localization de la carte SD"), wxPoint(64,64), wxSize(168,56), 0, _T("ID_STATICBOX2"));
	StaticBox1 = new wxStaticBox(Panel1, ID_STATICBOX1, _("Carte voix"), wxPoint(64,144), wxSize(168,64), 0, _T("ID_STATICBOX1"));
	EnrgisterlacarteSD = new wxButton(Panel1, ID_BUTTON1, _("OK"), wxPoint(160,248), wxSize(88,24), 0, wxDefaultValidator, _T("ID_BUTTON1"));
	SoundCard = new wxChoice(Panel1, ID_CHOICE1, wxPoint(88,176), wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	SoundCard->Append(_("JQ5600"));
	SoundCard->Append(_("WTV20SD"));
	Drive = new wxComboBox(Panel1, ID_COMBOBOX1, wxEmptyString, wxPoint(176,88), wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX1"));
	Drive->Append(_("D:"));
	Drive->Append(_("E:"));
	Drive->Append(_("F:"));
	Drive->Append(_("G:"));
	Drive->Append(_("H:"));
	Drive->Append(_("I:"));

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SDVoiceFrame::OnButton1Click);
	Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&SDVoiceFrame::OnSoundCardSelect);
	//*)

	//wxDirSelector(wxDirSelectorPromptStr, wxEmptyString, 0, wxDefaultPosition,	NULL);

  //wxDirSelector(wxDirDialogDefaultFolderStr, wxEmptyString, 0, wxDefaultPosition,	NULL);// No good.

  //wxDirDialog* drivers_list = new wxDirDialog(this, wxDirDialogDefaultFolderStr, wxEmptyString, wxDD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize,	wxDirDialogNameStr);

//  wxGenericDirCtrl* drivers_list = new wxGenericDirCtrl(this, -1, wxDirDialogDefaultFolderStr,
 //   wxDefaultPosition, wxDefaultSize, (wxDIRCTRL_DEFAULT_STYLE | wxDIRCTRL_SELECT_FIRST), wxEmptyString, 0, wxTreeCtrlNameStr);

   // this does not wait for a user choice. (?)

 // SDslot = drivers_list->GetPath();
  //wxMessageBox(SDslot);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //warning format SD !!!!




    //wxString inputFile = inputPath + (wxString::Format(wxT("000%d"),j) + ".mp3");////////////// AD4//Revisar formato.
//    inputFile2 = wxString::Truncate(4);

    //SDFile =            outputPath + (wxString::Format(wxT("000%d"),j) + ".wav");/////.mp3 ///////// AD4
    //wxCopyFile(inputFile , SDFile , true);


////////////////////////////////////////////////////////////////////////////////////////////////////////
}

SDVoiceFrame::~SDVoiceFrame()
{
  //(*Destroy(SDVoiceFrame)
	//*)
}

void SDVoiceFrame::OnwhereSDSelect(wxCommandEvent& event)
{
  //wxMessageBox(pepe);
}

void SDVoiceFrame::OnSoundCardSelect(wxCommandEvent& event)
{
}

void SDVoiceFrame::OnButton1Click(wxCommandEvent& event)//one day this will write an SD card.
{
  wxString outPath = Drive->GetString(Drive->GetSelection());
  wxString card = SoundCard->GetString(SoundCard->GetSelection());
  postfix = (".mp3");//default
  wxString fromPath = AppPath + ("\\VOICEMP3\\");//default
  if (card == "WTV20SD")
  {
    postfix = ".AD4";
    fromPath = AppPath + ("\\VOICEAD4\\");
  }

  for (int j = 0; j < 10; j++ )
  {
    wxString fromFile = fromPath + (wxString::Format(wxT("000%d"),j) + postfix);//TODO come back to this. ASAP.
    wxString toFile   =  outPath + (wxString::Format(wxT("000%d"),j) + postfix);
    //wxCopyFile(fromFile , toFile , true);
  }
  for (int j = 10; j < 100; j++ )
  {
    wxString fromFile = fromPath + (wxString::Format(wxT("00%d"),j) + postfix);
    wxString toFile   =  outPath + (wxString::Format(wxT("00%d"),j) + postfix);
  }
  for (int j = 100; j < 512; j++ )
  {
    wxString fromFile = fromPath + (wxString::Format(wxT("0%d"),j) + postfix);
    wxString toFile   =  outPath + (wxString::Format(wxT("0%d"),j) + postfix);
  }
}

