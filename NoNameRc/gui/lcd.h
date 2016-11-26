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

#ifndef _LCD_H
#define _LCD_H

#include "../NoNameRc.h"

#define LCD_W         128
#define LCD_H         64
#define BOX_WIDTH     23
#define coord_t       uint8_t
#define scoord_t      int8_t
#define CENTER
#define CENTER_OFS    0
#define CONTRAST_MIN  10
#define CONTRAST_MAX  45

#define lcdint_t      int16_t
#define lcduint_t     uint16_t

#define FW              6
#define FWNUM           5
#define FH              8

#define LCD_LINES       (LCD_H/FH)
#define LCD_COLS        (LCD_W/FW)

/* lcd common flags */
#define BLINK           0x01

/* lcd text flags */
#define INVERS          0x02
#if defined(BOLD_FONT)
#define BOLD          0x40
#else
#define BOLD          0x00
#endif

/* lcd putc flags */
#define CONDENSED       0x08
#define FIXEDWIDTH      0x10
/* lcd puts flags */
/* no 0x80 here because of "GV"1 which is aligned LEFT */
/* no 0x10 here because of "MODEL"01 which uses LEADING0 */
#define BSS           0x20
#define ZCHAR           0x80

/* lcd outdez flags */
#define UNSIGN          0x08
#define LEADING0        0x10
#define PREC1           0x20
#define PREC2           0x30
#define MODE(flags)     ((((int8_t)(flags) & 0x30) - 0x10) >> 4)
#define LEFT            0x80 /* align left */

/* line, rect, square flags */
#define FORCE           0x02
#define ERASE           0x04
#define ROUND           0x08

/* telemetry flags */
#define NO_UNIT         0x40

#define DBLSIZE       0x04
#define SMLSIZE       0x00
#define TINSIZE       0x00
#define XXLSIZE       0x00
#define ERASEBG       0x00

#define STREXPANDED   0x00

#define LcdFlags             uint8_t

#define display_t            uint8_t
#define DISPLAY_BUF_SIZE     (LCD_W*((LCD_H+7)/8))

extern display_t displayBuf[DISPLAY_BUF_SIZE];

#define lcdRefreshWait()

extern coord_t lcdLastPos;
extern coord_t lcdNextPos;

#define DISPLAY_BUFER_SIZE     (sizeof(display_t)*DISPLAY_BUF_SIZE)
#define DISPLAY_END            (displayBuf + DISPLAY_BUF_SIZE)
#define ASSERT_IN_DISPLAY(p)   assert((p) >= displayBuf && (p) < DISPLAY_END)

#if defined(PCBSTD) && defined(VOICE)
extern volatile uint8_t LcdLock ;
#endif

#if defined(DEBUG)
// TODO quick & dirty :(
typedef const unsigned char pm_uchar;
typedef const char pm_char;
#endif

void lcdDrawChar(coord_t x, coord_t y, const unsigned char c);
void lcdDrawCharAtt(coord_t x, coord_t y, const unsigned char c, LcdFlags mode);
void lcdDrawTextAtt(coord_t x, coord_t y, const pm_char * s, LcdFlags mode);
void lcdDrawTextAtIndex(coord_t x, coord_t y, const pm_char * s,uint8_t idx, LcdFlags mode);
void lcdDrawSizedTextAtt(coord_t x, coord_t y, const pm_char * s,unsigned char len, LcdFlags mode);
void lcdDrawText(coord_t x, coord_t y, const pm_char * s);
void lcdDrawSizedText(coord_t x, coord_t y, const pm_char * s, unsigned char len);
void lcdDrawTextLeft(coord_t y, const pm_char * s);

#define lcdDrawTextCenter(y, s) lcdDrawText((LCD_W-sizeof(TR_##s)*FW+FW+1)/2, y, STR_##s)

void lcd_outhex4(coord_t x, coord_t y, uint16_t val);

void lcdDrawNumberNAtt(coord_t x, coord_t y, lcdint_t val, LcdFlags mode=0, uint8_t len=0);
void lcdDrawNumberAttUnit(coord_t x, coord_t y, lcdint_t val, LcdFlags mode=0);
void lcd_outdez8(coord_t x, coord_t y, int8_t val);

void lcdDrawStringWithIndex(coord_t x, coord_t y, const pm_char *str, uint8_t idx, LcdFlags att=0);
void putsModelName(coord_t x, coord_t y, char *name, uint8_t id, LcdFlags att);
void lcdPutsSwitches(coord_t x, coord_t y, int8_t swtch, LcdFlags att=0);
void putsMixerSource(coord_t x, coord_t y, uint8_t idx, LcdFlags att=0);
void putsFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att=0);
void lcdDrawCurveName(coord_t x, coord_t y, int8_t idx, LcdFlags att=0);
void lcdPutsTimerMode(coord_t x, coord_t y, int8_t mode, LcdFlags att=0);
void lcdPutsTrimMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att);
#if defined(ROTARY_ENCODERS)
void lcdPutsRotaryEncoderMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att);
#endif

#define putsChn(x, y, idx, att) putsMixerSource(x, y, MIXSRC_CH1+idx-1, att)
void lcdPutsChnLetter(coord_t x, coord_t y, uint8_t idx, LcdFlags attr);

void lcdPutsVolts(coord_t x, coord_t y, uint16_t volts, LcdFlags att);
void putsVBat(coord_t x, coord_t y, LcdFlags att);

#if !defined(BOOT)
void putsChannelValue(coord_t x, coord_t y, source_t channel, lcdint_t val, LcdFlags att=0);
void putsChannel(coord_t x, coord_t y, source_t channel, LcdFlags att=0);
void lcdPutsTelemetryChannelValue(coord_t x, coord_t y, uint8_t channel, lcdint_t val, LcdFlags att=0);
#endif

#define putstime_t int16_t

void putsRtcTime(coord_t x, coord_t y, LcdFlags att);
void putsTimer(coord_t x, coord_t y, putstime_t tme, LcdFlags att, LcdFlags att2);

#define SOLID  0xff
#define DOTTED 0x55

void lcdDrawPoint(coord_t x, coord_t y, LcdFlags att=0);
void lcdMaskPoint(uint8_t *p, uint8_t mask, LcdFlags att=0);
void lcdDrawSolidHorizontalLine(coord_t x, coord_t y, coord_t w, LcdFlags att=0);
void lcdDrawSolidHorizontalLineStip(coord_t x, coord_t y, coord_t w, uint8_t pat, LcdFlags att=0);
void lcdDrawSolidVerticalLine(coord_t x, scoord_t y, scoord_t h);
#if defined(CPUM64)
void lcdDrawSolidVerticalLineStip(coord_t x, scoord_t y, int8_t h, uint8_t pat);
#else
void lcdDrawSolidVerticalLineStip(coord_t x, scoord_t y, scoord_t h, uint8_t pat, LcdFlags att=0);
#endif


void lcdDrawFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, uint8_t pat=SOLID, LcdFlags att=0);
void lcdDrawRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat=SOLID, LcdFlags att=0);

void lcdInvertLine(int8_t line);
#define lcd_status_line() lcdInvertLine(LCD_LINES-1)
inline void lcd_square(coord_t x, coord_t y, coord_t w, LcdFlags att=0) { lcdDrawRect(x, y, w, w, SOLID, att); }

void lcdDrawTelemetryTopBar();

#define V_BAR(xx, yy, ll)    \
  lcdDrawSolidVerticalLine(xx-1,yy-ll,ll);  \
  lcdDrawSolidVerticalLine(xx  ,yy-ll,ll);  \
  lcdDrawSolidVerticalLine(xx+1,yy-ll,ll)

#if (defined(PCBMEGA2560) && !defined(SIMU))
void lcd_imgfar(coord_t x, coord_t y, uint_farptr_t img, uint8_t idx, LcdFlags att); // progmem "far"
#endif
void lcd_img(coord_t x, coord_t y, const pm_uchar * img, uint8_t idx, LcdFlags att=0);

void lcdSetRefVolt(unsigned char val);
void lcdClear();
void lcdSetContrast();
void lcdInit();
#define lcdOff()
#if defined (SIMU)
void simuTrace(const char * format, ...);
void SimuSleepMs(uint16_t x);
#endif // defined
void lcdRefresh();
void lcdRefreshFast();

#if defined(BOOT)
#define BLINK_ON_PHASE (0)
#else
#define BLINK_ON_PHASE (g_blinkTmr10ms & (1<<6))
#endif

char *strAppend(char * dest, const char * source, int len=0);
char *strSetCursor(char *dest, int position);
char *strAppendDate(char * str, bool time=false);
char *strAppendFilename(char * dest, const char * filename, const int size);

#endif
