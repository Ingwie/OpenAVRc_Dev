/***************************************************************
 * Name:      NoNameRc_SimulatorApp.cpp
 * Purpose:   Code for Application Class
 * Author:    NoNameRc_TEAM ()
 * Created:   2016-09-12
 * Copyright: NoNameRc_TEAM ()
 * License:
 **************************************************************/

#include "NoNameRc_SimulatorApp.h"

//(*AppHeaders
#include <wx/image.h>
//*)

IMPLEMENT_APP(NoNameRc_SimulatorApp);


bool NoNameRc_SimulatorApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
        SimuFrame = new NoNameRc_SimulatorFrame(0);
        SimuFrame->Show();
        SetTopWindow(SimuFrame);
    }
    //*)
    return wxsOK;
}

int NoNameRc_SimulatorApp::FilterEvent(wxEvent& event)
{
    if ((event.GetEventType() == (wxEVT_KEY_DOWN)) || (event.GetEventType() == (wxEVT_KEY_UP)) && SimuFrame )
    {
        SimuFrame->OnKey((wxKeyEvent&)event);
        return true;
    }
   return Event_Skip;
}
