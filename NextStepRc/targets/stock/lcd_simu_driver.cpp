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

#include "../../nextsteprc.h"


#define NUMITERATIONFULLREFRESH  1



void lcdInit()
{

}

void lcdSetRefVolt(uint8_t val)
{
}

void lcdRefreshFast()
{
  REFRESHDURATION1  //Debug function if defined LCDDURATIONSHOW in nextsteprc.h


  REFRESHDURATION2  //Debug function if defined LCDDURATIONSHOW in nextsteprc.h
}

void lcdRefresh()
{
  for (uint8_t i=0; i < NUMITERATIONFULLREFRESH; i++) { lcdRefreshFast(); }
}
