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

#define CROSSFIRE_START_BYTE        0x0F
#define CROSSFIRE_CH_CENTER         0x3E0
#define CROSSFIRE_CH_BITS           11

// Range for pulses (channels output) is [-1024:+1024]
void createCrossfireFrame(uint8_t * frame, int16_t * pulses)
{
  uint8_t * buf = frame;
  *buf++ = CROSSFIRE_START_BYTE;

  uint32_t bits = 0;
  uint8_t bitsavailable = 0;
  for (int i=0; i<CROSSFIRE_CHANNELS_COUNT; i++) {
    bits |= (CROSSFIRE_CH_CENTER + (((pulses[i]) * 4) / 5)) << bitsavailable;
    bitsavailable += CROSSFIRE_CH_BITS;
    while (bitsavailable >= 8) {
      *buf++ = bits;
      bits >>= 8;
      bitsavailable -= 8;
    }
  }
  *buf++ = 0;
  *buf++ = crc16(frame, 24);
}
