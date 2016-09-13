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

#ifndef menu_model_h
#define menu_model_h

#if defined(MAVLINK)
#include "gui/view_mavlink.h"
#endif

// TODO elsewhere!
#define WCHART (LCD_H/2)
#define X0     (LCD_W-WCHART-2)
#define Y0     (LCD_H/2)

enum EnumTabModel {
  e_ModelSelect,
  e_ModelSetup,
  CASE_HELI(e_Heli)
  CASE_FLIGHT_MODES(e_FlightModesAll)
  e_InputsAll,
  e_MixAll,
  e_Limits,
  CASE_CURVES(e_CurvesAll)
  e_LogicalSwitches,
  e_CustomFunctions,
  CASE_FRSKY(e_Telemetry)
  CASE_MAVLINK(e_MavSetup)
  CASE_TEMPLATES(e_Templates)
};


#endif
