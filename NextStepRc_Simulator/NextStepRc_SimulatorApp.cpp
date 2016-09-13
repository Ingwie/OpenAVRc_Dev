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
#include "NextStepRc_SimulatorMain.h"
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
    	NextStepRc_SimulatorFrame* Frame = new NextStepRc_SimulatorFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
