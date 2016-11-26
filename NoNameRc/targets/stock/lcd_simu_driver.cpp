/*
*************************************************************
*                      NoNameRc                           *
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
#include "../../../NoNameRc_Simulator/NoNameRc_SimulatorApp.h"


#define NUMITERATIONFULLREFRESH  1

void simuTrace(const char * format, ...)
{
    va_list arglist;
    va_start(arglist, format);
    char tmp[50];
    vsnprintf(tmp, 50, format, arglist);
    wxLogStatus(tmp);
    va_end(arglist);
}


void SimuSleepMs(uint16_t x)
{
    wxTheApp->Yield(FALSE); //This function tell the app to process events
    wxMilliSleep(x);
}

void lcdInit()
{
	BACKLIGHT_ON(); //LCD back light on
}

void lcdSetRefVolt(uint8_t val)
{

}

void lcdRefreshFast()
{
    REFRESHDURATION1  //Debug function if defined LCDDURATIONSHOW in NoNameRc.h

    wxGetApp().SimuFrame->DrawWxSimuLcd(); // ca marche  !!

    REFRESHDURATION2  //Debug function if defined LCDDURATIONSHOW in NoNameRc.h
}

void lcdRefresh()
{
    for (uint8_t i=0; i < NUMITERATIONFULLREFRESH; i++)
    {
        lcdRefreshFast();
    }
}
