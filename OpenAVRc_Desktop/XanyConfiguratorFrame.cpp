#include "XanyConfiguratorFrame.h"

//(*InternalHeaders(XanyConfiguratorFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(XanyConfiguratorFrame)
const long XanyConfiguratorFrame::ID_STATICBOX = wxNewId();
const long XanyConfiguratorFrame::ID_GRID1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(XanyConfiguratorFrame,wxFrame)
	//(*EventTable(XanyConfiguratorFrame)
	//*)
END_EVENT_TABLE()

XanyConfiguratorFrame::XanyConfiguratorFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(XanyConfiguratorFrame)
	Create(parent, id, _("XanyConfigurator"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
	SetClientSize(wxSize(906,450));
	Move(wxDefaultPosition);
	StaticBoxXanyConfig = new wxStaticBox(this, ID_STATICBOX, _("Xany Configurator"), wxPoint(8,24), wxSize(872,416), 0, _T("ID_STATICBOX"));
	Grid1 = new wxGrid(this, ID_GRID1, wxPoint(24,40), wxSize(640,352), 0, _T("ID_GRID1"));
	Grid1->CreateGrid(16,7);
	Grid1->EnableEditing(true);
	Grid1->EnableGridLines(true);
	Grid1->SetColLabelValue(0, _("N° Sortie"));
	Grid1->SetColLabelValue(1, _("Type de Sortie"));
	Grid1->SetColLabelValue(2, _("Mode de Cmde"));
	Grid1->SetColLabelValue(3, _("Pos0(µs)"));
	Grid1->SetColLabelValue(4, _("Pos1(µs)"));
	Grid1->SetColLabelValue(5, _("Durée(ms)"));
	Grid1->SetColLabelValue(6, _("Etat"));
	Grid1->SetDefaultCellFont( Grid1->GetFont() );
	Grid1->SetDefaultCellTextColour( Grid1->GetForegroundColour() );

	Connect(ID_GRID1,wxEVT_GRID_CELL_LEFT_CLICK,(wxObjectEventFunction)&XanyConfiguratorFrame::OnGrid1CellLeftClick);
	//*)
}

XanyConfiguratorFrame::~XanyConfiguratorFrame()
{
	//(*Destroy(XanyConfiguratorFrame)
	//*)
}


void XanyConfiguratorFrame::OnListBox1Select(wxCommandEvent& event)
{
}

void XanyConfiguratorFrame::OnClose(wxCloseEvent& event)
{
}

void XanyConfiguratorFrame::OnGrid1CellLeftClick(wxGridEvent& event)
{
}
