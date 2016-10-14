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
#include "../../../NextStepRc_Simulator/NextStepRc_SimulatorApp.h"


#define NUMITERATIONFULLREFRESH  1



void lcdInit()
{
wxTheApp->Yield(); //This function tell the app to process events
}

void lcdSetRefVolt(uint8_t val)
{

}

void lcdRefreshFast()
{
    REFRESHDURATION1  //Debug function if defined LCDDURATIONSHOW in nextsteprc.h

    wxGetApp().SimuFrame->DrawWxSimuLcd(); // ca marche  !!

    REFRESHDURATION2  //Debug function if defined LCDDURATIONSHOW in nextsteprc.h
}

void lcdRefresh()
{
    for (uint8_t i=0; i < NUMITERATIONFULLREFRESH; i++)
    {
        lcdRefreshFast();
    }
}
