/*
*************************************************************
*                      NEXTSTEPRC                           *
*                                                           *
*             -> Build your DIY MEGA 2560 TX                *
*                                                           *
*      Based on code named                                  *
*      OpenTx - https://github.com/opentx/opentx            *
*                                                           *
*         Only avr code here for lisibility ;-)             *
*                                                           *
*  License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html  *
*                                                           *
*************************************************************
*/

#include "lcd_simu_driver.h"



#define NUMITERATIONFULLREFRESH  1



void lcdInit()
{
  //TODO
}

void lcdSetRefVolt(uint8_t val)
{
  //TODO
}

void lcdRefreshFast()
{
    REFRESHDURATION1  //Debug function if defined LCDDURATIONSHOW in nextsteprc.h
//const wxEventType wxEVT_MYTHREAD_STARTED = wxNewEventType();
   // &NextStepRc_SimulatorFrame::DrawLcd();//TODO

    REFRESHDURATION2  //Debug function if defined LCDDURATIONSHOW in nextsteprc.h
}

void lcdRefresh()
{
    for (uint8_t i=0; i < NUMITERATIONFULLREFRESH; i++)
    {
        lcdRefreshFast();
    }
}
