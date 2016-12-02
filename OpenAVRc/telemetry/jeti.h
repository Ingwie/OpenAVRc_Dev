/*
 *************************************************************
 *                      OpenAVRc                           *
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

#ifndef jeti_h
#define jeti_h

#define JETI_KEY_LEFT		0x70
#define JETI_KEY_RIGHT		0xe0
#define JETI_KEY_UP		0xd0
#define JETI_KEY_DOWN		0xb0
#define JETI_KEY_NOCHANGE	0xf0

void JETI_Init(void);
void JETI_EnableRXD (void);
void JETI_DisableRXD (void);
void JETI_EnableTXD (void);
void JETI_DisableTXD (void);

void telemetryInterrupt10ms();
void menuTelemetryJeti(uint8_t event);

#endif

