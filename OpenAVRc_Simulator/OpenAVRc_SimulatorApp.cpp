 /*
 **************************************************************************
 *                                                                        *
 *                 ____                ___ _   _____                      *
 *                / __ \___  ___ ___  / _ | | / / _ \____                 *
 *               / /_/ / _ \/ -_) _ \/ __ | |/ / , _/ __/                 *
 *               \____/ .__/\__/_//_/_/ |_|___/_/|_|\__/                  *
 *                   /_/                                                  *
 *                                                                        *
 *              This file is part of the OpenAVRc project.                *
 *                                                                        *
 *                         Based on code(s) named :                       *
 *             OpenTx - https://github.com/opentx/opentx                  *
 *             Deviation - https://www.deviationtx.com/                   *
 *                                                                        *
 *                Only AVR code here for visibility ;-)                   *
 *                                                                        *
 *   OpenAVRc is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation, either version 2 of the License, or    *
 *   (at your option) any later version.                                  *
 *                                                                        *
 *   OpenAVRc is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *   GNU General Public License for more details.                         *
 *                                                                        *
 *       License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html          *
 *                                                                        *
 **************************************************************************
*/


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
  if ( wxsOK ) {
    SimuFrame = new OpenAVRc_SimulatorFrame(0);
    SimuFrame->Show();
    SetTopWindow(SimuFrame);
  }
  //*)
  return wxsOK;
}

int OpenAVRc_SimulatorApp::FilterEvent(wxEvent& event)
{
  if ((event.GetEventType() == (wxEVT_KEY_DOWN)) || (event.GetEventType() == (wxEVT_KEY_UP)) && SimuFrame ) {
    SimuFrame->OnKey((wxKeyEvent&)event);
    return Event_Skip;
  }
  return Event_Skip;
}
