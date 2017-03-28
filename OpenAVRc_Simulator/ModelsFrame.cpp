#include "ModelsFrame.h"
#include "OpenAVRc_SimulatorMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(ModelsFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

void PopulateModelsFrame();

//(*IdInit(ModelsFrame)
const long ModelsFrame::ID_MODELSGRID = wxNewId();
const long ModelsFrame::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ModelsFrame,wxFrame)
	//(*EventTable(ModelsFrame)
	//*)
END_EVENT_TABLE()

ModelsFrame::ModelsFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(ModelsFrame)
	Create(parent, id, _("Modèles"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxSUNKEN_BORDER, _T("id"));
	SetClientSize(wxSize(152,209));
	Move(wxDefaultPosition);
	Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(240,128), wxSize(152,450), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	ModelsGrid = new wxGrid(Panel1, ID_MODELSGRID, wxPoint(0,0), wxSize(150,202), 0, _T("ID_MODELSGRID"));
	ModelsGrid->CreateGrid(60,1);
	ModelsGrid->EnableEditing(true);
	ModelsGrid->EnableGridLines(true);
	ModelsGrid->SetColLabelSize(1);
	ModelsGrid->SetRowLabelSize(28);
	ModelsGrid->SetDefaultColSize(100, true);
	ModelsGrid->SetDefaultCellFont( ModelsGrid->GetFont() );
	ModelsGrid->SetDefaultCellTextColour( ModelsGrid->GetForegroundColour() );

	Connect(ID_MODELSGRID,wxEVT_GRID_CELL_LEFT_CLICK,(wxObjectEventFunction)&ModelsFrame::OnModelsGridCellLeftClick);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&ModelsFrame::OnClose);
	//*)

	PopulateModelsFrame();
}

ModelsFrame::~ModelsFrame()
{
	//(*Destroy(ModelsFrame)
	//*)
}

void ModelsFrame::OnClose(wxCloseEvent& event)
{
  OpenAVRc_SimulatorFrame *parent = wxDynamicCast(this->GetParent(), OpenAVRc_SimulatorFrame);
  if(parent)
    parent->EnableModelsMenu();
  Destroy();
}

void ModelsFrame::PopulateModelsFrame()
{
  wxString nameString;

  for (int8_t i = 0; i < MAX_MODELS; i++){
    nameString = "";
    char name[LEN_MODEL_NAME] = "";
    if (!eeModelExists(i)){
      continue;
    }
    theFile.openRlc(FILE_MODEL(i));
    theFile.readRlc((uint8_t*)name, sizeof(g_model.header.name));
    nameString = ConvCharFwToWxstr(name, LEN_MODEL_NAME); //Convert Firmware chars[] to wxString
    ModelsGrid->SetCellValue(i,0,nameString);
  }
}

void ModelsFrame::OnModelsGridCellLeftClick(wxGridEvent& event)
{
  wxString newNameString;
  for (int8_t i = 0; i < MAX_MODELS; i++){
    newNameString = "";
    char newName[LEN_MODEL_NAME] = "";
    newNameString = (ModelsGrid->GetCellValue(i,0));
    CheckEntryValues(newNameString);
    ConvWxstrToCharFw(newNameString, newName, LEN_MODEL_NAME); //Convert wxString to Firmware chars[]
    theFile.openRlc(FILE_MODEL(i));
    theFile.writeRlc(FILE_MODEL(i),FILE_TYP_MODEL, (uint8_t*)newName, LEN_MODEL_NAME, true);
  }
    PopulateModelsFrame();

    //TODO close files (?)
    //timer on and off
    //test if it works with new model (create file ?)
    //review the option "left click", clearly improvable
    //test if everyting is saved.
    //synchronize LCD and model screen (!!!!)
}

void ModelsFrame::CheckEntryValues(wxString inString)
{
  std::string  x(inString);
  if (x.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_-+:*=?") != std::string::npos){
      wxMessageBox(x +_(" :\nIl y a un nom avec des caractères spéciaux non autorisés.\nVeuillez le changer.\nCaractères spéciaux autorisés : _-+:*=?"));
    }
  if ((x.length() < 3) & (x.length() > 1)){
      wxMessageBox(x +_(" :\nIl y a un nom trop court, veuillez le compléter."));
    }
  return;
  }


