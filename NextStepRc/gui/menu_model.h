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

extern void menuModelSelect(uint8_t event);
extern void menuModelSetup(uint8_t event);
extern void menuModelHeli(uint8_t event);
extern void menuModelFlightModesAll(uint8_t event);
extern void menuModelExposAll(uint8_t event);
extern void menuModelMixAll(uint8_t event);
extern void menuModelLimits(uint8_t event);
extern void menuModelCurvesAll(uint8_t event);
extern void menuModelCurveOne(uint8_t event);
extern void menuModelGVars(uint8_t event);
extern void menuModelLogicalSwitches(uint8_t event);
extern void menuModelCustomFunctions(uint8_t event);
extern void menuModelTelemetry(uint8_t event);
extern void menuModelTemplates(uint8_t event);
extern void menuModelExpoOne(uint8_t event);
extern uint8_t editDelay(const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay);
extern uint8_t s_curveChan;


// TODO elsewhere!
#define WCHART (LCD_H/2)
#define X0     (LCD_W-WCHART-2)
#define Y0     (LCD_H/2)
#define MIXES_2ND_COLUMN  (12*FW)

#define FlightModesType uint8_t

#define EDIT_DELAY(x, y, event, attr, str, delay) editDelay(y, event, attr, str, delay)


#if   defined(CPUM64)
  #define INCDEC_DECLARE_VARS(f)
  #define INCDEC_SET_FLAG(f)
  #define INCDEC_ENABLE_CHECK(fn)
  #define CHECK_INCDEC_PARAM(event, var, min, max) checkIncDec(event, var, min, max, EE_MODEL)
#else
  #define INCDEC_DECLARE_VARS(f)  uint8_t incdecFlag = (f)
  #define INCDEC_SET_FLAG(f)      incdecFlag = (f)
  #define INCDEC_ENABLE_CHECK(fn)
  #define CHECK_INCDEC_PARAM(event, var, min, max) checkIncDec(event, var, min, max, incdecFlag)
#endif

static uint8_t s_currIdx;
#define COPY_MODE 1
#define MOVE_MODE 2
static uint8_t s_copyMode = 0;
static int8_t s_copySrcRow;
static int8_t s_copyTgtOfs;

#if defined(CPUM64)
  #define editNameCursorPos menuHorizontalPosition
#else
  static uint8_t editNameCursorPos = 0;
#endif


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

const MenuFuncP_PROGMEM menuTabModel[] PROGMEM = {
  menuModelSelect,
  menuModelSetup,
  CASE_HELI(menuModelHeli)
  CASE_FLIGHT_MODES(menuModelFlightModesAll)
  menuModelExposAll,
  menuModelMixAll,
  menuModelLimits,
  CASE_CURVES(menuModelCurvesAll)
  menuModelLogicalSwitches,
  menuModelCustomFunctions,
  CASE_FRSKY(menuModelTelemetry)
  CASE_MAVLINK(menuTelemetryMavlinkSetup)
  CASE_TEMPLATES(menuModelTemplates)
};

#if defined(CPUM64)
#define editSingleName(x, y, label, name, size, event, active) editName(x, y, name, size, event, active)
#else
extern void editSingleName(coord_t x, coord_t y, const pm_char *label, char *name, uint8_t size, uint8_t event, uint8_t active);
#endif


#endif
