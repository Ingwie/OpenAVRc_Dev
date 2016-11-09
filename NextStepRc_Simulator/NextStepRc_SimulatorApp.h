/***************************************************************
 * Name:      NextStepRc_SimulatorApp.h
 * Purpose:   Defines Application Class
 * Author:    NextStepRc_TEAM ()
 * Created:   2016-09-12
 * Copyright: NextStepRc_TEAM ()
 * License:
 **************************************************************/

#ifndef NEXTSTEPRC_SIMULATORAPP_H
#define NEXTSTEPRC_SIMULATORAPP_H
#include "NextStepRc_SimulatorMain.h"

#include <wx/app.h>


class NextStepRc_SimulatorApp : public wxApp
{
    public:
      NextStepRc_SimulatorFrame* SimuFrame;
        virtual bool OnInit();
        int FilterEvent(wxEvent& event);

};

wxDECLARE_APP(NextStepRc_SimulatorApp);


#endif // NEXTSTEPRC_SIMULATORAPP_H
