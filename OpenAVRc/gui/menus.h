 /*
 **************************************************************************
 *                                                                        *
 *                 ____                ___ _   _____                      *
 *                / __ \___  ___ ___  / _ | | / / _ \____                 *
 *               / /_/ / _ \/ -_) _ \/ __ | |/ / , _/ __/                 *
 *               \____/ .__/\__/_//_/_/ |_|___/_/|_|\__/                  *
 *                   /_/                                                  *
 *                                                                        *
 *              This file is part of the OpenAVRc project.                *
 *                                                                        *
 *                         Based on code(s) named :                       *
 *             OpenTx - https://github.com/opentx/opentx                  *
 *             Deviation - https://www.deviationtx.com/                   *
 *                                                                        *
 *                Only AVR code here for visibility ;-)                   *
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


#ifndef _MENUS_H_
#define _MENUS_H_

#include "../myeeprom.h"
#include "lcd.h"


#define MENUS_SCROLLBAR_WIDTH  0
#define MENU_COLUMNS           1
#define COLUMN_X               0
#define lcdDrawTextColumnLeft(x, y, str) lcdDrawTextLeft(y, str)

// Menus related stuff ...
#if defined(SDCARD)
typedef uint16_t vertpos_t;
#else
typedef uint8_t vertpos_t;
#endif

typedef uint8_t check_event_t;
#define horzpos_t uint8_t


extern vertpos_t menuVerticalPosition;
extern horzpos_t menuHorizontalPosition;
extern vertpos_t menuVerticalOffset;
extern uint8_t calibrationState;

// Temporary no highlight
extern uint8_t noHighlightCounter;
#define NO_HIGHLIGHT()        (noHighlightCounter > 0)
#define START_NO_HIGHLIGHT()  do { noHighlightCounter = 25; } while(0)


typedef void (*MenuHandlerFunc)(uint8_t event);
typedef void (*MenuFuncP_PROGMEM)(uint8_t event);
extern const MenuFuncP_PROGMEM menuTabModel[];
extern const MenuFuncP_PROGMEM menuTabGeneral[];
extern const MenuFuncP_PROGMEM menuTabFPV[];
extern const MenuFuncP_PROGMEM menuTabTelemetry[];

extern MenuHandlerFunc menuHandlers[5];
extern uint8_t menuVerticalPositions[4];
extern uint8_t menuLevel;
extern uint8_t menuEvent;
extern uint8_t shared_u8;


/// goto given Menu, but substitute current menu in menuStack
extern void chainMenu(MenuHandlerFunc newMenu);
/// goto given Menu, store current menu in menuStack
extern void pushMenu(MenuHandlerFunc newMenu);
/// return to last menu in menustack
extern void popMenu();
///deliver address of last menu which was popped from
inline MenuHandlerFunc lastPopMenu()
{
  return menuHandlers[menuLevel+1];
}

extern void drawPotsBars();
extern void doMainScreenGraphics();
extern void menuFirstCalib(uint8_t event);
extern void menuGeneralBluetooth(uint8_t event);
extern void menuGeneralScc(uint8_t event);


extern void onMainViewMenu(const char *result);
extern void menuMainView(uint8_t event);
extern void menuGeneralDiagAna(uint8_t event);
#if defined(FRSKY)
extern void menuTelemetryFrsky(uint8_t event);
#endif
extern void menuGeneralSetup(uint8_t event);
extern void menuModelSetup(uint8_t event);
extern void menuGeneralCalib(uint8_t event);
//void menuCustomFunctions(uint8_t event, CustomFunctionData * functions, CustomFunctionsContext * functionsContext);

extern void menuModelSelect(uint8_t event);
extern void menuModelCustomFunctions(uint8_t event);
extern void menuStatisticsView(uint8_t event);
extern void menuStatisticsDebug(uint8_t event);

extern void displaySlider(coord_t x, coord_t y, uint8_t value, uint8_t max, uint8_t attr);

extern uint8_t editNameCursorPos;

#if defined(NAVIGATION_POT1)
extern int16_t p1valdiff;
#else
#define p1valdiff 0
#endif

#if defined(NAVIGATION_POT2)
extern int8_t p2valdiff;
#else
#define p2valdiff 0
#endif

extern int8_t checkIncDec_Ret;  // global helper vars

#define EDIT_SELECT_MENU   -1
#define EDIT_SELECT_FIELD  0
#define EDIT_MODIFY_FIELD  1
#define EDIT_MODIFY_STRING 2
extern int8_t s_editMode;       // global editmode

// checkIncDec flags
#define EE_NO           0x00
#define EE_GENERAL      0x01
#define EE_MODEL        0x02
#define NO_INCDEC_MARKS 0x04
#define INCDEC_SWITCH   0x08
#define INCDEC_SOURCE   0x10
#define INCDEC_REP10    0x40
#define NO_DBLKEYS      0x80

// mawrow special values
#define TITLE_ROW      ((uint8_t)-1)
#define HIDDEN_ROW     ((uint8_t)-2)

int16_t checkIncDec(uint8_t event, int16_t i_pval, int16_t i_min, int16_t i_max, uint8_t i_flags=0);

int8_t checkIncDecMovedSwitch(int8_t val);

#define checkIncDecModel(event, i_val, i_min, i_max) checkIncDec(event, i_val, i_min, i_max, EE_MODEL)
#define checkIncDecModelZero(event, i_val, i_max) checkIncDec(event, i_val, 0, i_max, EE_MODEL)
#define checkIncDecGen(event, i_val, i_min, i_max) checkIncDec(event, i_val, i_min, i_max, EE_GENERAL)

#define CHECK_INCDEC_MODELVAR(event, var, min, max) \
  var = checkIncDecModel(event,var,min,max)

#define CHECK_INCDEC_MODELVAR_STARTPULSES_IF_CHANGE(event, var, min, max) \
  tmp = checkIncDecModel(event,var,min,max);                              \
  if (tmp != var) { startPulses(PROTOCMD_INIT); };                        \
  var = tmp

#define CHECK_INCDEC_MODELVAR_ZERO(event, var, max) \
  var = checkIncDecModelZero(event,var,max)

#define CHECK_INCDEC_MODELVAR_ZERO_STARTPULSES_IF_CHANGE(event, var, max) \
  tmp = checkIncDecModelZero(event,var,max);                              \
  if (tmp != var) { startPulses(PROTOCMD_INIT); };                        \
  var = tmp

#define CHECK_INCDEC_MODELVAR_CHECK(event, var, min, max, check) \
    var = checkIncDec(event, var, min, max, EE_MODEL)

#define CHECK_INCDEC_MODELVAR_ZERO_CHECK(event, var, max, check) \
    CHECK_INCDEC_MODELVAR_ZERO(event, var, max)

#if   defined(AUTOSWITCH)
#define AUTOSWITCH_ENTER_LONG() (attr && event==EVT_KEY_LONG(KEY_ENTER))
#define CHECK_INCDEC_SWITCH(event, var, min, max, flags, available) \
    var = checkIncDec(event, var, min, max, (flags)|INCDEC_SWITCH)
#define CHECK_INCDEC_MODELSWITCH(event, var, min, max, available) \
    CHECK_INCDEC_SWITCH(event, var, min, max, EE_MODEL, available)
#else
#define AUTOSWITCH_ENTER_LONG() (0)
#define CHECK_INCDEC_SWITCH(event, var, min, max, flags, available) \
    CHECK_INCDEC_MODELVAR(event, var, min, max)
#define CHECK_INCDEC_MODELSWITCH(event, var, min, max, available) \
    CHECK_INCDEC_MODELVAR(event, var, min, max)
#endif

#if   defined(AUTOSOURCE)
#define CHECK_INCDEC_MODELSOURCE(event, var, min, max) \
    var = checkIncDec(event,var,min,max,EE_MODEL|INCDEC_SOURCE|NO_INCDEC_MARKS)
#else
#define CHECK_INCDEC_MODELSOURCE CHECK_INCDEC_MODELVAR
#endif

#define CHECK_INCDEC_GENVAR(event, var, min, max) \
  var = checkIncDecGen(event, var, min, max)

#define NAVIGATION_LINE_BY_LINE  0
#define CURSOR_ON_LINE()         (0)

void check(check_event_t event, uint8_t curr, const MenuHandlerFunc *menuTab, uint8_t menuTabSize, const pm_uint8_t *horTab, uint8_t horTabMax, vertpos_t maxrow);
void check_simple(check_event_t event, uint8_t curr, const MenuHandlerFunc *menuTab, uint8_t menuTabSize, vertpos_t maxrow);
void check_submenu_simple(check_event_t event, uint8_t maxrow);

void title(const pm_char * s);
#define TITLE(str) title(str)

#define MENU_TAB(...) static const pm_uint8_t mstate_tab[] PROGMEM = __VA_ARGS__

#define MENU_CHECK(tab, menu, lines_count) \
  check(event, menu, tab, DIM(tab), mstate_tab, DIM(mstate_tab)-1, (lines_count)-1)

#define MENU_CHECK_FLAGS(tab, menu, flags, lines_count) \
  check(event, menu, tab, DIM(tab), mstate_tab, DIM(mstate_tab)-1, (lines_count)-1, flags)

#define MENU(title, tab, menu, lines_count, ...) \
  MENU_TAB(__VA_ARGS__); \
  MENU_CHECK(tab, menu, lines_count); \
  TITLE(title)

#define MENU_FLAGS(title, tab, menu, flags, lines_count, ...) \
  MENU_TAB(__VA_ARGS__); \
  MENU_CHECK_FLAGS(tab, menu, flags, lines_count); \
  TITLE(title)

#define SIMPLE_MENU_NOTITLE(tab, menu, lines_count) \
  check_simple(event, menu, tab, DIM(tab), (lines_count)-1);

#define SIMPLE_MENU(title, tab, menu, lines_count) \
  SIMPLE_MENU_NOTITLE(tab, menu, lines_count); \
  TITLE(title)

#define SUBMENU_NOTITLE(lines_count, ...) { \
  MENU_TAB(__VA_ARGS__); \
  check(event, 0, NULL, 0, mstate_tab, DIM(mstate_tab)-1, (lines_count)-1); \
  }

#define SUBMENU(title, lines_count, ...) \
  MENU_TAB(__VA_ARGS__); \
  check(event, 0, NULL, 0, mstate_tab, DIM(mstate_tab)-1, (lines_count)-1); \
  TITLE(title)

#define SIMPLE_SUBMENU_NOTITLE(lines_count) \
  check_submenu_simple(event, (lines_count)-1);

#define SIMPLE_SUBMENU(title, lines_count) \
  SIMPLE_SUBMENU_NOTITLE(lines_count); \
  TITLE(title)

typedef int8_t select_menu_value_t;

select_menu_value_t selectMenuItem(coord_t x, coord_t y, const pm_char *label, const pm_char *values, select_menu_value_t value, select_menu_value_t min, select_menu_value_t max, LcdFlags attr, uint8_t event);
select_menu_value_t selectMenuSubImg(coord_t x, coord_t y, select_menu_value_t value, select_menu_value_t minSubImgIdx, select_menu_value_t maxSubImgIdx, uint_farptr_t img, uint8_t subImgIdx, LcdFlags attr, uint8_t event);
uint8_t onoffMenuItem(uint8_t value, coord_t x, coord_t y, const pm_char *label, LcdFlags attr, uint8_t event);
int8_t switchMenuItem(coord_t x, coord_t y, int8_t value, LcdFlags attr, uint8_t event);

#define ON_OFF_MENU_ITEM(value, x, y, label, attr, event) value = onoffMenuItem(value, x, y, label, attr, event)

#define GVAR_MENU_ITEM(x, y, v, min, max, lcdattr, editflags, event) gvarMenuItem(x, y, v, min, max, lcdattr, event)

#if defined(GVARS)
int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t event); // @@@ open.20.fsguruh
#define displayGVar(x, y, v, min, max) GVAR_MENU_ITEM(x, y, v, min, max, 0, 0, 0)
#else
int16_t gvarMenuItem(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t event);
#define displayGVar(x, y, v, min, max) lcdDrawNumberNAtt(x, y, v)
#endif

#define WARNING_TYPE_ASTERISK  0
#define WARNING_TYPE_CONFIRM   1
#define WARNING_TYPE_INPUT     2

extern const pm_char * warningText;
extern const pm_char * warningInfoText;

PACK(typedef struct {
  uint8_t warningInfoLength:5;
  uint8_t warningType:2;
  uint8_t warningResult:1;
}) warning_t;

extern warning_t warning;

#define MENU_X   10
#define MENU_Y   16
#define MENU_W   LCD_W-(2*MENU_X)
#define WARNING_LINE_LEN 20
#define WARNING_LINE_X 16
#define WARNING_LINE_Y 3*FH

void displayBox();
void displayPopup(const pm_char * pstr);
void displayWarning(uint8_t event);

#define POPUP_WARNING(s)             warningText = s
#define POPUP_CONFIRMATION(s)        (warningText = s, warning.warningType = WARNING_TYPE_CONFIRM)
#define WARNING_INFO_FLAGS           ZCHAR
#define SET_WARNING_INFO(info, len, flags) (warningInfoText = info, warning.warningInfoLength = len)

#define POPUP_MENU_ADD_ITEM(s) popupMenuItems[popupMenuNoItems++] = s
#define POPUP_MENU_MAX_LINES               6
#define MENU_MAX_DISPLAY_LINES       POPUP_MENU_MAX_LINES
#if defined(SDCARD)
#define POPUP_MENU_ADD_SD_ITEM(s)        POPUP_MENU_ADD_ITEM(s)
#else
#define POPUP_MENU_ADD_SD_ITEM(s)
#endif
#define MENU_LINE_LENGTH             (LEN_MODEL_NAME+1)
#define POPUP_MENU_ITEMS_FROM_BSS()  (popupMenuFlags = BSS)
extern const char *popupMenuItems[POPUP_MENU_MAX_LINES];
extern uint8_t s_menu_item;
extern uint8_t popupMenuNoItems;
extern uint8_t popupMenuFlags;
extern uint8_t popupMenuOffset;
const char * displayPopupMenu(uint8_t event);
extern void (*popupMenuHandler)(const char *result);

#if defined(SDCARD)
#define STATUS_LINE_LENGTH           32
extern char statusLineMsg[STATUS_LINE_LENGTH];
void showStatusLine();
void drawStatusLine();
#else
#define drawStatusLine()
#endif


#define LABEL(...)                     (uint8_t)-1

#define KEY_MOVE_UP    KEY_UP
#define KEY_MOVE_DOWN  KEY_DOWN
#define CURSOR_MOVED_LEFT(event)       (IS_ROTARY_LEFT(event) || EVT_KEY_MASK(event) == KEY_LEFT)
#define CURSOR_MOVED_RIGHT(event)      (IS_ROTARY_RIGHT(event) || EVT_KEY_MASK(event) == KEY_RIGHT)

void repeatLastCursorMove(uint8_t event);
#define REPEAT_LAST_CURSOR_MOVE()    { if (EVT_KEY_MASK(event) >= 0x0e) putEvent(event); else repeatLastCursorMove(event); }
#define MOVE_CURSOR_FROM_HERE()      if (menuHorizontalPosition > 0) REPEAT_LAST_CURSOR_MOVE()

#define POS_VERT_INIT                  0
#define POS_HORZ_INIT(posVert)         0
#define EDIT_MODE_INIT                 -1

typedef int16_t (*FnFuncP) (int16_t x);
void DrawFunction(FnFuncP fn, uint8_t offset=0);

//Range type
#define RANGE_NONE   0
#define RANGE_UPPER  1
#define RANGE_NUMBER 2

void editSingleName(coord_t x, coord_t y, const pm_char *label, char *name, uint8_t size, uint8_t event, uint8_t active, uint8_t msk, uint8_t range);
void editName(coord_t x, coord_t y, char *name, uint8_t size, uint8_t event, uint8_t active, uint8_t msk, uint8_t range);

#endif // _MENUS_H_
