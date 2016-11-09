/***************************************************************
 * Name:      NextStepRc_SimulatorApp.cpp
 * Purpose:   Code for Application Class
 * Author:    NextStepRc_TEAM ()
 * Created:   2016-09-12
 * Copyright: NextStepRc_TEAM ()
 * License:
 **************************************************************/

#include "NextStepRc_SimulatorApp.h"

//(*AppHeaders
#include <wx/image.h>
//*)

IMPLEMENT_APP(NextStepRc_SimulatorApp);


bool NextStepRc_SimulatorApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
        SimuFrame = new NextStepRc_SimulatorFrame(0);
        SimuFrame->Show();
        SetTopWindow(SimuFrame);
    }
    //*)
    return wxsOK;
}

int NextStepRc_SimulatorApp::FilterEvent(wxEvent& event)
{
    if ((event.GetEventType() == (wxEVT_KEY_DOWN)) || (event.GetEventType() == (wxEVT_KEY_UP)) && SimuFrame )
    {
        SimuFrame->OnKey((wxKeyEvent&)event);
        return true;
    }
   return Event_Skip;
}
