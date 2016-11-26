/***************************************************************
 * Name:      NoNameRc_DesktopApp.cpp
 * Purpose:   Code for Application Class
 * Author:    NoNameRc_TEAM ()
 * Created:   2016-09-08
 * Copyright: NoNameRc_TEAM ()
 * License:
 **************************************************************/

#include "NoNameRc_DesktopApp.h"

//(*AppHeaders
#include "NoNameRc_DesktopMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(NoNameRc_DesktopApp);

bool NoNameRc_DesktopApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
        NoNameRc_DesktopFrame* Frame = new NoNameRc_DesktopFrame(0);
        Frame->Show();
        SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
