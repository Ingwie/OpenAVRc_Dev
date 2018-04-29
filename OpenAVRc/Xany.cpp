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
/*
Supported combination: /!\ 2 Devices supported max per X-Any: 1 X-Any is associated to one proportionnal channel /!\
=====================
.---------.-----.-----.-----.-------.-----------.
| Device  | A2  | A1  | A0  |Address| X-Any Idx |
+---------+-----+-----+-----+-------+-----------+
|         |  0  |  0  |  0  |  0x20 |     0     |
|         +-----+-----+-----+-------+-----------+
|         |  0  |  0  |  1  |  0x21 |     0     |
| PCF8574 +-----+-----+-----+-------+-----------+
|         |  0  |  1  |  0  |  0x22 |     1     |
|         +-----+-----+-----+-------+-----------+
|         |  0  |  1  |  1  |  0x23 |     1     |
+-------- +-----+-----+-----+-------+-----------+
|         |  1  |  0  |  0  |  0x24 |     0     |
| PCF8575 +-----+-----+-----+-------+-----------+
|         |  1  |  0  |  1  |  0x25 |     1     |
'---------'-----'-----'-----'-------'-----------'

.-------------.---------------------------.---------------------------.
| Device Type |         PCF8575           |         PCF8574           |
+-------------+------+------+------+------+------+------+------+------+
| I2C Address |  NU  |  NU  | 0x25 | 0x24 | 0x23 | 0x22 | 0x21 | 0x20 |
+-------------+------+------+------+------+------+------+------+------+
|IoExtMap Bit#|  b7  |  b6  |  b5  |  b4  |  b3  |  b2  |  b1  |  b0  |
+-------------+------+------+------+------+------+------+------+------+
|  X-Any Idx  |  NU  |  NU  |   1  |   0  |   1  |   1  |   0  |   0  |
+-------------+------+------+------+------+------+------+------+------+
|Payload field|  NU  |  NU  | Word | Word |Byte.h|Byte.l|Byte.h|Byte.l|
+-------------+------+------+------+------+------+------+------+------+
|     Each bit set in IoExtMap indicates presence of device at the    |
|     corresponding I2C address.                                      |
'---------------------------------------------------------------------'
*/
#include "Xany.h"
#include "i2c_master.h"

#define IO_EXP_BASE_ADDR              (0x20 << 1) /* Bit 0 is for R/W# */
#define IO_EXP_SUP_MAX_NB             6
/*
NIBBLE_WIDTH_US
  <--->
 996                                                                     2004
  |-+-|-+-|-+-|-+-|-+-|-+-|-+-|-+-|-+-|-+-|-+-|-+-|-+-|-+-|-+-|-+-|-+-|-+-|
    0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F   R   I
    <--->
    |   |                                                               |
  1024 1080                                                           1976
INTER_NIBBLE
*/
enum {NIBBLE_0=0, NIBBLE_1, NIBBLE_2, NIBBLE_3, NIBBLE_4, NIBBLE_5, NIBBLE_6, NIBBLE_7, NIBBLE_8, NIBBLE_9, NIBBLE_A, NIBBLE_B, NIBBLE_C, NIBBLE_D, NIBBLE_E, NIBBLE_F, NIBBLE_R, NIBBLE_I, NIBBLE_NB};

#define NEUTRAL_WIDTH_US                  1500
#define NIBBLE_WIDTH_US                   56
#define FULL_EXCURSION_US                 (NIBBLE_WIDTH_US * NIBBLE_NB)
#define PULSE_MIN_US                      (NEUTRAL_WIDTH_US - (FULL_EXCURSION_US / 2))
#define PULSE_WIDTH_US(NibbleIdx)         (PULSE_MIN_US + (NIBBLE_WIDTH_US / 2)+ ((NibbleIdx) * NIBBLE_WIDTH_US))
#define EXCURSION_HALF_US(NibbleIdx)      ((PULSE_WIDTH_US(NibbleIdx) - NEUTRAL_WIDTH_US) * 2)

#define GET_EXCURSION_HALF_US(NibbleIdx)  (int16_t)pgm_read_word_far(&ExcursionHalf_us[(NibbleIdx)])

#define X_ANY_MSG_LEN                     sizeof(X_OneAnyReadMsgSt_t)

const int16_t ExcursionHalf_us[] PROGMEM = {EXCURSION_HALF_US(NIBBLE_0), EXCURSION_HALF_US(NIBBLE_1), EXCURSION_HALF_US(NIBBLE_2), EXCURSION_HALF_US(NIBBLE_3),
                                            EXCURSION_HALF_US(NIBBLE_4), EXCURSION_HALF_US(NIBBLE_5), EXCURSION_HALF_US(NIBBLE_6), EXCURSION_HALF_US(NIBBLE_7),
                                            EXCURSION_HALF_US(NIBBLE_8), EXCURSION_HALF_US(NIBBLE_9), EXCURSION_HALF_US(NIBBLE_A), EXCURSION_HALF_US(NIBBLE_B),
                                            EXCURSION_HALF_US(NIBBLE_C), EXCURSION_HALF_US(NIBBLE_D), EXCURSION_HALF_US(NIBBLE_E), EXCURSION_HALF_US(NIBBLE_F),
                                            EXCURSION_HALF_US(NIBBLE_R), EXCURSION_HALF_US(NIBBLE_I)};

typedef struct{
  uint8_t         Low;
  uint8_t         High;
}WordBytesSt_t; /* Size = 2 bytes */

/* Futur use? */
typedef struct{
  uint16_t
                  Angle    :10, /* Angle coded on 10 bits */
                  Switches :6;  /* 6 bits of a PCF8574 or Mcp (2 bits are lost) */
}WordAglSwSt_t; /* Size = 2 bytes */

typedef union{
  WordAglSwSt_t   AglSw;
  WordBytesSt_t   Byte;
  uint16_t        Word;
}WordBytes_union;  /* Size = 2 bytes */

typedef struct{
  WordBytes_union Payload;
  uint8_t         Chks;
}X_OneAnyReadMsgSt_t;  /* Size = 3 bytes */

typedef struct {
  uint8_t
                  TxInProgress:     1,
                  TxCharInProgress: 1;
  uint16_t
                  NbToSend:         3,
                  SentCnt:          3,
                  CurIdx:           5, /* Prev Nibble to compare to the following one */
                  PrevIdx:          5; /* Prev Nibble to compare to the following one */
}TxNibbleSt_t;  /* Size = 3 bytes */

typedef struct{
  char            Byte[sizeof(X_OneAnyReadMsgSt_t)]; /* Sending buffer (in interrupt) */
  uint8_t         ByteIdx;
  TxNibbleSt_t    Nibble;
}X_OneAnyWriteMsgSt_t;

static uint8_t IoExtMap = 0; /* 1 byte for the map of the 2 X-Any */

static          X_OneAnyReadMsgSt_t  X_AnyReadMsg[NUM_X_ANY]; /* 3 bytes per X-Any for storing read Msg */
static volatile X_OneAnyWriteMsgSt_t X_AnyWriteMsg[NUM_X_ANY];/* 4 bytes per X-Any for sending Msg in interrupt (-> Volatile) */

/* PRIVATE FUNCTION PROTOYPES */
static void Xany_readInputs(uint8_t XanyIdx);


/* PUBLIC FUNCTIONS */

/**
* \file   Xany.cpp
* \fn     void Xany_init(void)
* \brief  Probes the I2C bus to identify the present I/O expenders
* \param  Void
* \return Void
*/
void Xany_init(void)
{
  /* I2C drive SHALL be initilized before calling Xany_init() */
  uint8_t Idx;

  /* Clear the whole structure for the 2 groups */
  memset((void*)&X_AnyWriteMsg, 0, sizeof(X_AnyWriteMsg));

  /* Probe I2C bus to discover Io Expender chips */
  for(Idx = 0; Idx < IO_EXP_SUP_MAX_NB; Idx++)
  {
    if(!i2c_start((IO_EXP_BASE_ADDR  + Idx) | I2C_READ))
    {
      /* OK: device is present quit gracefully by sending a stop() */
      i2c_stop();
      IoExtMap |= (1 << Idx); /* Mark it as present */
    }
  }

}

/**
* \file   Xany.cpp
* \fn     uint8_t Xany_readInputsAndLoadMsg(uint8_t XanyIdx)
* \brief  Reads the I2C I/O expenders associated to the specified group and load it in the Tx buffer
* \param  XanyIdx: Index of the group
* \return 0: Not done, previous message not completely sent, 1: Done, new message in the Tx buffer
*/
uint8_t Xany_readInputsAndLoadMsg(uint8_t XanyIdx)
{
  uint8_t Done = 0;

  if(X_AnyWriteMsg[XanyIdx].ByteIdx >= (X_ANY_MSG_LEN + 1))
  {
    /* No need to mask interrupt since X_AnyWriteMsg[] is not use when X_AnyWriteMsg[XanyIdx].ByteIdx is >= (X_ANY_MSG_LEN + 1) */
    Xany_readInputs(XanyIdx); /* This reads the I2C bus */
    /* Load from Read structure to Write structure */
    memcpy((void*)&X_AnyWriteMsg[XanyIdx], (void*)&X_AnyReadMsg[XanyIdx], sizeof(X_OneAnyReadMsgSt_t)); /* Copy 3 bytes */
    X_AnyWriteMsg[XanyIdx].ByteIdx = 0; /* Go! */
    Done = 1;
  }
  return(Done);
}

/**
* \file   Xany.cpp
* \fn     void Xany_scheduleTx(uint8_t XanyIdx)
* \brief  Schedule the transmisson of the X-Any message
* \param  XanyIdx: Index of X-Any
* \return Void
*/
void Xany_scheduleTx(uint8_t XanyIdx)
{
  X_OneAnyWriteMsgSt_t *t;
  char                 TxChar;

  if(g_model.Xany[XanyIdx].Active)
  {
    t = (X_OneAnyWriteMsgSt_t *)&X_AnyWriteMsg[XanyIdx]; /* XanyIdx SHALL be < NUM_X_ANY */

    if(!t->Nibble.TxInProgress)
    {
      t->Nibble.TxInProgress = 1;
      if(!t->Nibble.TxCharInProgress)
      {
        /* Get next char to send */
        if(t->ByteIdx < X_ANY_MSG_LEN)
        {
          TxChar = t->Byte[t->ByteIdx];
          t->Nibble.CurIdx = ((TxChar & 0xF0) >> 4); /* MSN first */
          t->Nibble.TxCharInProgress = 1;
        }
        else
        {
          t->Nibble.CurIdx = NIBBLE_I; /* Nothing to transmit */
        }
        if(t->ByteIdx < (X_ANY_MSG_LEN + 1)) t->ByteIdx++; /* Bounded to 4: meanst synchro to allow reload of the new message */
      }
      else
      {
        /* Tx Char in progress: send least significant nibble */
        TxChar = t->Byte[t->ByteIdx];
        t->Nibble.CurIdx = TxChar & 0x0F; /* LSN */
        t->Nibble.TxCharInProgress = 0;
      }
      if(t->Nibble.CurIdx == t->Nibble.PrevIdx) t->Nibble.CurIdx = NIBBLE_R; /* Repeat symbol */
      t->Nibble.PrevIdx = t->Nibble.CurIdx;
    }
    /* Send the Nibble or the Repeat or the Idle symbol */
    channelOutputs[g_model.Xany[XanyIdx].ChId] = GET_EXCURSION_HALF_US(t->Nibble.CurIdx);
    t->Nibble.SentCnt++;
    t->Nibble.NbToSend = g_model.Xany[XanyIdx].RepeatNb + 1;
    if(t->Nibble.SentCnt >= t->Nibble.NbToSend)
    {
      t->Nibble.SentCnt = 0;
      t->Nibble.TxInProgress = 0;
    }
  }
}

/* PRIVATE FUNCTIONS */

/**
* \file   Xany.cpp
* \fn     void Xany_readInputs(uint8_t XanyIdx)
* \brief  Reads the I2C I/O expenders associated to the specified X-Any
* \param  XanyIdx: Index of the X-Any
* \return Void
*/
static void Xany_readInputs(uint8_t XanyIdx)
{
  uint8_t  BitIdx, One8bitPort;
  uint16_t Two8bitPorts;

  X_AnyReadMsg[XanyIdx].Payload.Word  = 0;
  for(BitIdx = 0; BitIdx < IO_EXP_SUP_MAX_NB; BitIdx++)
  {
    if(IoExtMap | (1<<BitIdx))
    {
      /* Chip is present in the map */
      if(BitIdx < 4)
      {
        /* Read one 8 bit port */
        i2c_receive((IO_EXP_BASE_ADDR  + BitIdx), (uint8_t*)&One8bitPort, 1); /* This function expects a nack for last byte: will work? */
        if(BitIdx & 1) X_AnyReadMsg[XanyIdx].Payload.Byte.High = One8bitPort; /* Odd  */
        else           X_AnyReadMsg[XanyIdx].Payload.Byte.Low  = One8bitPort; /* Even */
      }
      else
      {
        /* Read two 8 bit ports */
        i2c_receive((IO_EXP_BASE_ADDR  + BitIdx), (uint8_t*)&Two8bitPorts, 2); /* This function expects a nack for last byte: will work? */
        X_AnyReadMsg[XanyIdx].Payload.Word  = Two8bitPorts;
      }
    }
  }
  if(g_model.Xany[XanyIdx].AbsAglSensor)
  {
    //read the Absolute Angular Sensor:
    /* TODO */
    //X_AnyReadMsg[XanyIdx].Payload.Angle = Angle; //This will overwrite some digital inputs!
  }
  /* Update Checksum */
  X_AnyReadMsg[XanyIdx].Chks = X_AnyReadMsg[XanyIdx].Payload.Byte.High ^ X_AnyReadMsg[XanyIdx].Payload.Byte.Low ^ 0x55;
}
