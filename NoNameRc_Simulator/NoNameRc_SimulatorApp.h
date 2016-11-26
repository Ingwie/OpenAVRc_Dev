/***************************************************************
 * Name:      NoNameRc_SimulatorApp.h
 * Purpose:   Defines Application Class
 * Author:    NoNameRc_TEAM ()
 * Created:   2016-09-12
 * Copyright: NoNameRc_TEAM ()
 * License:
 **************************************************************/

#ifndef NoNameRc_SIMULATORAPP_H
#define NoNameRc_SIMULATORAPP_H
#include "NoNameRc_SimulatorMain.h"

#include <wx/app.h>


class NoNameRc_SimulatorApp : public wxApp
{
    public:
      NoNameRc_SimulatorFrame* SimuFrame;
        virtual bool OnInit();
        int FilterEvent(wxEvent& event);

};

wxDECLARE_APP(NoNameRc_SimulatorApp);


#endif // NoNameRc_SIMULATORAPP_H
