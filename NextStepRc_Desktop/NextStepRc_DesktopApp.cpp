/***************************************************************
 * Name:      NextStepRc_DesktopApp.cpp
 * Purpose:   Code for Application Class
 * Author:    NextStepRc_TEAM ()
 * Created:   2016-09-08
 * Copyright: NextStepRc_TEAM ()
 * License:
 **************************************************************/

#include "NextStepRc_DesktopApp.h"

//(*AppHeaders
#include "NextStepRc_DesktopMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(NextStepRc_DesktopApp);

bool NextStepRc_DesktopApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
        NextStepRc_DesktopFrame* Frame = new NextStepRc_DesktopFrame(0);
        Frame->Show();
        SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
