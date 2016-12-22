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


#ifndef SPIN_H
#define SPIN_H

#include "../OpenAVRc/OpenAVRc.h"

class Spin
{
public:
    Spin(const char* id, uint8_t* pin,uint8_t* ddr,uint8_t* port);
    virtual ~Spin();
    void init();
    bool GetPin(uint8_t num);
    void SetPin(uint8_t num);
    void ResetPin(uint8_t num);
    void TogglePin(uint8_t num);

protected:

private:
    const char* P_id;
    uint8_t* P_pin;
    uint8_t* P_ddr;
    uint8_t* P_port;
};

#endif // SPIN_H
