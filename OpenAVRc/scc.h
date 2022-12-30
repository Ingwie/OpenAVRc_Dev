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
#ifndef SCC_H
#define SCC_H

#ifdef INPUT
#undef INPUT
#endif
#define INPUT    0

#ifdef OUTPUT
#undef OUTPUT
#endif
#define OUTPUT   1

#ifdef LOW
#undef LOW
#endif
#define LOW     0

#ifdef HIGH
#undef HIGH
#endif
#define HIGH    1

// Some macros to easily manipulate I/O pins
#define PIN_MODE2(Port,  BitInPort, Dir)     (Dir)? ((DDR##Port)  |= _BV(BitInPort)): ((DDR##Port)  &= ~_BV(BitInPort))
#define PIN_MODE(Port_BitInPort, Dir)         PIN_MODE2(Port_BitInPort, Dir)

#define DIGITAL_READ2(Port, BitInPort)        !!((PIN##Port) & _BV(BitInPort))
#define DIGITAL_READ(Port_BitInPort)          DIGITAL_READ2(Port_BitInPort)

#define DIGITAL_WRITE2(Port, BitInPort, Val)  (Val)? ((PORT##Port) |= _BV(BitInPort)): ((PORT##Port) &= ~_BV(BitInPort))
#define DIGITAL_WRITE(Port_BitInPort, Val)    DIGITAL_WRITE2(Port_BitInPort, Val)

#define DIGITAL_TOGGLE2(Port, BitInPort)      (PIN##Port) |= _BV(BitInPort)
#define DIGITAL_TOGGLE(Port_BitInPort)        DIGITAL_TOGGLE2(Port_BitInPort)

#define PCIMSK_REG2(Port, BitInPort)          ((#Port[0] == 'B')? PCMSK0: ((#Port[0] == 'K')? PCMSK2: PCMSK1))
#define PCIMSK_REG(Port_BitInPort)            PCIMSK_REG2(Port_BitInPort)
#define PCIMSK_BIT2(Port, BitInPort)          ((#Port[0] == 'J')? (BitInPort + 1): (BitInPort))
#define PCIMSK_BIT(Port_BitInPort)            PCIMSK_BIT2(Port_BitInPort)

#define PCINT_ID2(Port, BitInPort)            (#Port[0] == 'B')? 0: ((#Port[0] == 'K')? 2: 1)
#define PCINT_ID(Port_BitInPort)              PCINT_ID2(Port_BitInPort)

#define ENABLE_PCINT_PORT(Port_BitInPort)     PCICR |=  _BV(PCINT_ID2(Port_BitInPort))
#define DISABLE_PCINT_PORT(Port_BitInPort)    PCICR &= ~_BV(PCINT_ID2(Port_BitInPort))

#define ENABLE_PCINT_PIN(Port_BitInPort)      PCIMSK_REG2(Port_BitInPort) |=  _BV(PCIMSK_BIT2(Port_BitInPort))
#define DISABLE_PCINT_PIN(Port_BitInPort)     PCIMSK_REG2(Port_BitInPort) &= ~_BV(PCIMSK_BIT2(Port_BitInPort))

#define SBUS_CHANNEL_NB                       16

typedef struct{
  uint8_t HeaderByte;
  uint8_t ChIdx;
  uint8_t FldFwd[3];
  uint8_t DoNotTouch[10];
  uint8_t CmdCode;
  uint8_t Checksum;
}SbusMsgSt_t;

#define SCC_SBUS_MSG_LEN  sizeof(SbusMsgSt_t)


typedef struct{
  union SbusMsg_union {
    uint8_t     ByteTbl[SCC_SBUS_MSG_LEN];
    SbusMsgSt_t Content;
  }Msg;
  uint8_t       CurChId;
  uint8_t       NewChId;
  uint8_t       MsgIdx      :7,
                SweepDirUp  :1;
  uint8_t       TxDelay;
  int16_t       SweepValue;
}SbusSt_t;

typedef struct{
  volatile SbusSt_t Sbus;
}SccSt_t;

/* PUBLIC FUNCTIONS */
void    scc_init(void);
void    scc_end(void);
uint8_t scc_sendCmd(uint8_t ChId);
void    scc_sbusSweepEnable(uint8_t On);
void    scc_sbusSweepTestProcess(void);
#define scc_getCurChId()                      scc_sendCmd(0)
#define scc_setChId(ChId)                     scc_sendCmd(ChId)

#endif // SCC_H
