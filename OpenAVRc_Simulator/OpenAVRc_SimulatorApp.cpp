/***************************************************************
 * Name:      OpenAVRc_SimulatorApp.cpp
 * Purpose:   Code for Application Class
 * Author:    OpenAVRc_TEAM ()
 * Created:   2016-09-12
 * Copyright: OpenAVRc_TEAM ()
 * License:
 **************************************************************/

#include "OpenAVRc_SimulatorApp.h"

//(*AppHeaders
#include <wx/image.h>
//*)

IMPLEMENT_APP(OpenAVRc_SimulatorApp);


bool OpenAVRc_SimulatorApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
        SimuFrame = new OpenAVRc_SimulatorFrame(0);
        SimuFrame->Show();
        SetTopWindow(SimuFrame);
    }
    //*)
    return wxsOK;
}

int OpenAVRc_SimulatorApp::FilterEvent(wxEvent& event)
{
    if ((event.GetEventType() == (wxEVT_KEY_DOWN)) || (event.GetEventType() == (wxEVT_KEY_UP)) && SimuFrame )
    {
        SimuFrame->OnKey((wxKeyEvent&)event);
        return true;
    }
   return Event_Skip;
}
