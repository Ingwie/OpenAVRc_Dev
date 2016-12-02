/***************************************************************
 * Name:      OpenAVRc_SimulatorApp.h
 * Purpose:   Defines Application Class
 * Author:    OpenAVRc_TEAM ()
 * Created:   2016-09-12
 * Copyright: OpenAVRc_TEAM ()
 * License:
 **************************************************************/

#ifndef OpenAVRc_SIMULATORAPP_H
#define OpenAVRc_SIMULATORAPP_H
#include "OpenAVRc_SimulatorMain.h"

#include <wx/app.h>


class OpenAVRc_SimulatorApp : public wxApp
{
    public:
      OpenAVRc_SimulatorFrame* SimuFrame;
        virtual bool OnInit();
        int FilterEvent(wxEvent& event);

};

wxDECLARE_APP(OpenAVRc_SimulatorApp);


#endif // OpenAVRc_SIMULATORAPP_H
