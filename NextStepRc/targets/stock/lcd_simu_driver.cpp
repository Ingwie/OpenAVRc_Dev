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
#include <wx/app.h>



#define NUMITERATIONFULLREFRESH  1



void lcdInit()
{
  //TODO
}

void lcdSetRefVolt(uint8_t val)
{

}

void lcdRefreshFast()
{
    REFRESHDURATION1  //Debug function if defined LCDDURATIONSHOW in nextsteprc.h

wxDEFINE_EVENT(EVT_DRAW_LCD, wxCommandEvent);
wxCommandEvent event(EVT_DRAW_LCD);
event.SetString("This is the data");
wxTheApp->AddPendingEvent(event);


    REFRESHDURATION2  //Debug function if defined LCDDURATIONSHOW in nextsteprc.h
}

void lcdRefresh()
{
    for (uint8_t i=0; i < NUMITERATIONFULLREFRESH; i++)
    {
        lcdRefreshFast();
    }
}
