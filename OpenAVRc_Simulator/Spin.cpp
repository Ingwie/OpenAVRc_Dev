 /*
 **************************************************************************
 *                                                                        *
 *              This file is part of the OpenAVRc project.                *
 *                                                                        *
 *                         Based on code named                            *
 *             OpenTx - https://github.com/opentx/opentx                  *
 *                                                                        *
 *                Only AVR code here for lisibility ;-)                   *
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


#include <wx/msgdlg.h>
#include "Spin.h"

Spin::Spin(const char* id,uint8_t* pin,uint8_t* ddr,uint8_t* port)
{
    P_id = id;
    P_pin = pin;
    P_ddr = ddr;
    P_port = port;
}

Spin::~Spin()
{
    //dtor
}

void Spin::init()
{
    *P_pin = *P_port;
}

bool Spin::GetPin(uint8_t num)
{
    return (*P_pin & (1<<num));
}

void Spin::SetPin(uint8_t num)
{
    if (*P_ddr & (1<<num))
    {
        wxString mystring = wxString::Format(wxT("%i"),num);
        wxMessageBox( _("Ceci est une sortie"),P_id + mystring);
    }
    else
    {
        *P_pin |= (1<<num);
    }
}

void Spin::ResetPin(uint8_t num)
{
    if (*P_ddr & (1<<num))
    {
        wxString mystring = wxString::Format(wxT("%i"),num);
        wxMessageBox( _("Ceci est une sortie"),P_id + mystring);

    }
    else
    {
        *P_pin &= ~(1<<num);
    }
}

void Spin::TogglePin(uint8_t num)
{
    if (*P_ddr & (1<<num))
    {
        wxString mystring = wxString::Format(wxT("%i"),num);
        wxMessageBox( _("Ceci est une sortie"),P_id + mystring);

    }
    else
    {
        if (GetPin(num)) ResetPin(num);
        else SetPin(num);
    }
}
