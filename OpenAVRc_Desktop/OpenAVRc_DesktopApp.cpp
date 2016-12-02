/***************************************************************
 * Name:      OpenAVRc_DesktopApp.cpp
 * Purpose:   Code for Application Class
 * Author:    OpenAVRc_TEAM ()
 * Created:   2016-09-08
 * Copyright: OpenAVRc_TEAM ()
 * License:
 **************************************************************/

#include "OpenAVRc_DesktopApp.h"

//(*AppHeaders
#include "OpenAVRc_DesktopMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(OpenAVRc_DesktopApp);

bool OpenAVRc_DesktopApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
        OpenAVRc_DesktopFrame* Frame = new OpenAVRc_DesktopFrame(0);
        Frame->Show();
        SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
