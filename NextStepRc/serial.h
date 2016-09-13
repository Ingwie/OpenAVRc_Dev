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

#ifndef _SERIAL_H_
#define _SERIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

void serialPutc(char c);
void serialPrintf(const char *format, ...);
void serialCrlf();

#ifdef __cplusplus
}
#endif

#define serialPrint(...) do { serialPrintf(__VA_ARGS__); serialCrlf(); } while(0)

#endif // _SERIAL_H_

