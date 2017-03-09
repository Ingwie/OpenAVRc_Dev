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


#ifndef keys_h
#define keys_h


enum EnumKeys {
  KEY_MENU,
  KEY_EXIT,
  KEY_DOWN,
  KEY_UP,
  KEY_RIGHT,
  KEY_LEFT,

  TRM_BASE,
  TRM_LH_DWN = TRM_BASE,
  TRM_LH_UP,
  TRM_LV_DWN,
  TRM_LV_UP,
  TRM_RV_DWN,
  TRM_RV_UP,
  TRM_RH_DWN,
  TRM_RH_UP,

#if ROTARY_ENCODERS > 0 || defined(ROTARY_ENCODER_NAVIGATION)
  BTN_REa,
#endif
#if ROTARY_ENCODERS > 0
  BTN_REb,
#endif

  NUM_KEYS,
  SW_BASE=NUM_KEYS,

#if defined(CPUM2560) && defined(REV_EVO_V1)
// This sequence should follow enum SwitchSources in myeeprom.h
  SW_Jup=SW_BASE,//ID0
  SW_Jmi,//ID1
  SW_Jdn,//ID2
  SW_N,//THR
  SW_Ldn,//RUD
  SW_Odn,//ELE
  SW_I,//AIL
  SW_Gdn,//GEA
  SW_M,//TRN
// Additional 2 and 3 position switches and states.
  SW_Lup,//
  SW_Lmi,//
  SW_Oup,//
  SW_Omi,//
  SW_Gup,//
  SW_Gmi,//
  SW_H,//H
  SW_P,//P optional - not fitted
  SW_K,//K optional - not fitted
#else
  SW_ID0=SW_BASE,
  SW_ID1,
  SW_ID2,
  SW_THR,
  SW_RUD,
  SW_ELE,
  SW_AIL,
  SW_GEA,
  SW_TRN,
#endif
  NUM_SWS
};
#define EVT_KEY_MASK(e)      ((e) & 0x1f)

#define _MSK_KEY_BREAK       0x20
#define _MSK_KEY_REPT        0x40
#define _MSK_KEY_FIRST       0x60
#define _MSK_KEY_LONG        0x80

#define EVT_KEY_BREAK(key)   ((key)|_MSK_KEY_BREAK)
#define EVT_KEY_FIRST(key)   ((key)|_MSK_KEY_FIRST)
#define EVT_KEY_REPT(key)    ((key)|_MSK_KEY_REPT)
#define EVT_KEY_LONG(key)    ((key)|_MSK_KEY_LONG)

#define IS_KEY_BREAK(evt)    (((evt)&0xe0) == _MSK_KEY_BREAK)
#define IS_KEY_FIRST(evt)    (((evt)&0xe0) == _MSK_KEY_FIRST)
#define IS_KEY_LONG(evt)     (((evt)&0xe0) == _MSK_KEY_LONG)
#define IS_KEY_REPT(evt)     (((evt)&0xe0) == _MSK_KEY_REPT)

#define EVT_ENTRY            0xbf
#define EVT_ENTRY_UP         0xbe
#define EVT_MENU_UP          0xbd

#define EVT_ROTARY_BREAK   0xcf
#define EVT_ROTARY_LONG    0xce
#define EVT_ROTARY_LEFT    0xdf
#define EVT_ROTARY_RIGHT   0xde

#if defined(ROTARY_ENCODER_NAVIGATION)
#define IS_ROTARY_LEFT(evt)   (evt == EVT_ROTARY_LEFT)
#define IS_ROTARY_RIGHT(evt)  (evt == EVT_ROTARY_RIGHT)
#define IS_ROTARY_UP(evt)     IS_ROTARY_LEFT(evt)
#define IS_ROTARY_DOWN(evt)   IS_ROTARY_RIGHT(evt)
#define IS_ROTARY_BREAK(evt)  (evt == EVT_ROTARY_BREAK)
#define IS_ROTARY_LONG(evt)   (evt == EVT_ROTARY_LONG)
#define IS_ROTARY_EVENT(evt)  (EVT_KEY_MASK(evt) >= 0x0e)
#define CASE_EVT_ROTARY_BREAK case EVT_ROTARY_BREAK:
#define CASE_EVT_ROTARY_LONG  case EVT_ROTARY_LONG:
#define CASE_EVT_ROTARY_LEFT  case EVT_ROTARY_LEFT:
#define CASE_EVT_ROTARY_RIGHT case EVT_ROTARY_RIGHT:
#else
#define IS_ROTARY_LEFT(evt)   (0)
#define IS_ROTARY_RIGHT(evt)  (0)
#define IS_ROTARY_UP(evt)     (0)
#define IS_ROTARY_DOWN(evt)   (0)
#define IS_ROTARY_BREAK(evt)  (0)
#define IS_ROTARY_LONG(evt)   (0)
#define IS_ROTARY_EVENT(evt)  (0)
#define CASE_EVT_ROTARY_BREAK
#define CASE_EVT_ROTARY_LONG
#define CASE_EVT_ROTARY_LEFT
#define CASE_EVT_ROTARY_RIGHT
#endif

class Key
{
#define FILTERBITS      4
#define FFVAL          ((1<<FILTERBITS)-1)
#define KSTATE_OFF      0
#define KSTATE_RPTDELAY 95 // gruvin: delay state before key repeating starts
#define KSTATE_START    97
#define KSTATE_PAUSE    98
#define KSTATE_KILLED   99

private:
  uint8_t m_vals;   // key debounce?  4 = 40ms
  uint8_t m_cnt;
  uint8_t m_state;
public:
  void input(bool val);
  bool state()
  {
    return m_vals > 0;
  }
  void pauseEvents()
  {
    m_state = KSTATE_PAUSE;
    m_cnt = 0;
  }
  void killEvents()
  {
    m_state = KSTATE_KILLED;
  }
  EnumKeys key() const;
};

extern Key keys[NUM_KEYS];

extern uint8_t s_evt;

#define putEvent(evt) s_evt = evt

void pauseEvents(uint8_t enuk);
void killEvents(uint8_t enuk);
void clearKeyEvents();
uint8_t getEvent();

uint8_t keyDown();

#endif
