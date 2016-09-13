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

#include "../nextsteprc.h"


void menuModelSelect(uint8_t event);
void menuModelSetup(uint8_t event);
void menuModelHeli(uint8_t event);
void menuModelFlightModesAll(uint8_t event);
void menuModelExposAll(uint8_t event);
void menuModelMixAll(uint8_t event);
void menuModelLimits(uint8_t event);
void menuModelCurvesAll(uint8_t event);
void menuModelCurveOne(uint8_t event);
void menuModelGVars(uint8_t event);
void menuModelLogicalSwitches(uint8_t event);
void menuModelCustomFunctions(uint8_t event);
void menuModelTelemetry(uint8_t event);
void menuModelTemplates(uint8_t event);
void menuModelExpoOne(uint8_t event);

extern uint8_t s_curveChan;

  #define FlightModesType uint8_t

  #define MIXES_2ND_COLUMN  (12*FW)

uint8_t editDelay(const coord_t y, const uint8_t event, const uint8_t attr, const pm_char *str, uint8_t delay)
{
  lcdDrawTextLeft(y, str);
  lcdDrawNumberAttUnit(MIXES_2ND_COLUMN, y, (10/DELAY_STEP)*delay, attr|PREC1|LEFT);
  if (attr) CHECK_INCDEC_MODELVAR_ZERO(event, delay, DELAY_MAX);
  return delay;
}
#define EDIT_DELAY(x, y, event, attr, str, delay) editDelay(y, event, attr, str, delay)

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

void editName(coord_t x, coord_t y, char *name, uint8_t size, uint8_t event, uint8_t active)
{
#if defined(CPUM64)
  // in order to save flash
  lcdDrawTextLeft(y, STR_NAME);
#endif

  uint8_t mode = 0;
  if (active) {
    if (s_editMode <= 0)
      mode = INVERS + FIXEDWIDTH;
    else
      mode = FIXEDWIDTH;
  }

  lcdDrawSizedTextAtt(x, y, name, size, ZCHAR | mode);

  if (active) {
    uint8_t cur = editNameCursorPos;
    if (s_editMode > 0) {
      int8_t c = name[cur];
      int8_t v = c;

      if (p1valdiff || IS_ROTARY_RIGHT(event) || IS_ROTARY_LEFT(event) || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_FIRST(KEY_UP)
          || event==EVT_KEY_REPT(KEY_DOWN) || event==EVT_KEY_REPT(KEY_UP)) {
         v = checkIncDec(event, abs(v), 0, ZCHAR_MAX, 0);
         if (c <= 0) v = -v;
      }

      switch (event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
        case EVT_ROTARY_BREAK:
          if (s_editMode == EDIT_MODIFY_FIELD) {
            s_editMode = EDIT_MODIFY_STRING;
            cur = 0;
          }
          else if (cur<size-1)
            cur++;
          else
            s_editMode = 0;
          break;
#endif

        case EVT_KEY_BREAK(KEY_LEFT):
          if (cur>0) cur--;
          break;
        case EVT_KEY_BREAK(KEY_RIGHT):
          if (cur<size-1) cur++;
          break;

#if defined(ROTARY_ENCODER_NAVIGATION)
        case EVT_ROTARY_LONG:
          if (v==0) {
            s_editMode = 0;
            killEvents(event);
            break;
          }
          // no break
#endif

        case EVT_KEY_LONG(KEY_LEFT):
        case EVT_KEY_LONG(KEY_RIGHT):
          if (v>=-26 && v<=26) {
            v = -v; // toggle case
            if (event==EVT_KEY_LONG(KEY_LEFT))
              killEvents(KEY_LEFT);
          }
          break;
      }

      if (c != v) {
        name[cur] = v;
        eeDirty(EE_MODEL);
      }

      lcdDrawCharAtt(x+editNameCursorPos*FW, y, idx2char(v), ERASEBG|INVERS|FIXEDWIDTH);
    }
    else {
      cur = 0;
    }
    editNameCursorPos = cur;
  }
}

#if defined(CPUM64)
#define editSingleName(x, y, label, name, size, event, active) editName(x, y, name, size, event, active)
#else
void editSingleName(coord_t x, coord_t y, const pm_char *label, char *name, uint8_t size, uint8_t event, uint8_t active)
{
  lcdDrawTextLeft(y, label);
  editName(x, y, name, size, event, active);
}
#endif

static uint8_t s_currIdx;
