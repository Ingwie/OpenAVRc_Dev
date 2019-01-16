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
Supported combination: /!\ 2x I2C Devices supported max per X-Any: 1 X-Any is associated to one proportionnal channel /!\
=====================
*/
#include "Xany.h"
#include "i2c_master.h"

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

const int16_t ExcursionHalf_us[] PROGMEM = {EXCURSION_HALF_US(NIBBLE_0), EXCURSION_HALF_US(NIBBLE_1), EXCURSION_HALF_US(NIBBLE_2), EXCURSION_HALF_US(NIBBLE_3),
                                            EXCURSION_HALF_US(NIBBLE_4), EXCURSION_HALF_US(NIBBLE_5), EXCURSION_HALF_US(NIBBLE_6), EXCURSION_HALF_US(NIBBLE_7),
                                            EXCURSION_HALF_US(NIBBLE_8), EXCURSION_HALF_US(NIBBLE_9), EXCURSION_HALF_US(NIBBLE_A), EXCURSION_HALF_US(NIBBLE_B),
                                            EXCURSION_HALF_US(NIBBLE_C), EXCURSION_HALF_US(NIBBLE_D), EXCURSION_HALF_US(NIBBLE_E), EXCURSION_HALF_US(NIBBLE_F),
                                            EXCURSION_HALF_US(NIBBLE_R), EXCURSION_HALF_US(NIBBLE_I)};

/* Different supported Payload formats */
typedef struct{
  uint32_t
                  NibbleNbToTx:4,
                  NotUsed     :16,
                  Checksum    :8,
                  Sw          :4;
}Msg4SwSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  NibbleNbToTx:4,
                  NotUsed     :12,
                  Checksum    :8,
                  Sw          :8;
}Msg8SwSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  NibbleNbToTx:4,
                  NotUsed     :4,
                  Checksum    :8,
                  Sw          :16;
}Msg16SwSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  NibbleNbToTx:4,
                  NotUsed     :8,
                  Checksum    :8,
                  Angle       :12;
}MsgAngleSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  NibbleNbToTx:4,
                  NotUsed     :4,
                  Checksum    :8,
                  Sw          :4,
                  Angle       :12;
}MsgAngle4SwSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  NibbleNbToTx:4,
                  Checksum    :8,
                  Sw          :8,
                  Angle       :12;
}MsgAngle8SwSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  NibbleNbToTx:4,
                  Checksum    :8,
                  Pot         :8,
                  Angle       :12;
}MsgAnglePotSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  NibbleNbToTx:4,
                  NotUsed     :8,
                  Checksum    :8,
                  Sw          :4,
                  Pot         :8;
}MsgPot4SwSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  NibbleNbToTx:4,
                  NotUsed     :4,
                  Checksum    :8,
                  Sw          :8,
                  Pot         :8;
}MsgPot8SwSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  NibbleNbToTx      :4,
                  PayloadAndChecksum:28;
}MsgCommonSt_t; /* Size = 4 bytes */

typedef union{
  Msg4SwSt_t       Msg4Sw;
  Msg8SwSt_t       Msg8Sw;
  Msg16SwSt_t      Msg16Sw;
  MsgAngleSt_t     MsgAngle;
  MsgAngle4SwSt_t  MsgAngle4Sw;
  MsgAngle8SwSt_t  MsgAngle8Sw;
  MsgAnglePotSt_t  MsgAnglePot;
  MsgPot4SwSt_t    MsgPot4Sw;
  MsgPot8SwSt_t    MsgPot8Sw;
  MsgCommonSt_t    Common;
  uint32_t         Raw;
}XanyMsg_union;  /* Size = 4 bytes */

/* Declaration of supported combinations (See X_ANY_CFG() macro in myeeprom.h) */
#define XANY_MSG_4SW              X_ANY_CFG(1, 0, 0)
#define XANY_MSG_8SW              X_ANY_CFG(2, 0, 0)
#define XANY_MSG_16SW             X_ANY_CFG(3, 0, 0)
#define XANY_MSG_ANGLE            X_ANY_CFG(0, 1, 0)
#define XANY_MSG_ANGLE_4SW        X_ANY_CFG(1, 1, 0)
#define XANY_MSG_ANGLE_8SW        X_ANY_CFG(2, 1, 0)
#define XANY_MSG_ANGLE_POT        X_ANY_CFG(0, 1, 1)
#define XANY_MSG_POT_4SW          X_ANY_CFG(1, 0, 1)
#define XANY_MSG_POT_8SW          X_ANY_CFG(2, 0, 1)

/* Message length (in nibbles) including the checksum for each supported combination */
#define XANY_MSG_4SW_NBL_NB       (1 + 2)
#define XANY_MSG_8SW_NBL_NB       (2 + 2)
#define XANY_MSG_16SW_NBL_NB      (4 + 2)
#define XANY_MSG_ANGLE_NBL_NB     (3 + 2)
#define XANY_MSG_ANGLE_4SW_NBL_NB (3 + 1 + 2)
#define XANY_MSG_ANGLE_8SW_NBL_NB (3 + 2 + 2)
#define XANY_MSG_ANGLE_POT_NBL_NB (3 + 2 + 2)
#define XANY_MSG_POT_4SW_NBL_NB   (2 + 1 + 2)
#define XANY_MSG_POT_8SW_NBL_NB   (2 + 2 + 2)

/* Array of Pots choosen */
#define CONV_XANY_POTS(x) (x<4)? 3-x : x
const uint8_t XANY_POT[] PROGMEM = {CONV_XANY_POTS(X_ANY_1_POT), CONV_XANY_POTS(X_ANY_2_POT), CONV_XANY_POTS(X_ANY_3_POT), CONV_XANY_POTS(X_ANY_4_POT)};
#define GET_XANY_POT(PotIdx)        (uint8_t)     pgm_read_byte_near(&XANY_POT[PotIdx])

/* Allegro A1335 absolute angle sensor 7 bit I2C address */
#define A1335_I2C_7B_ADDR         (0x0C) /* /!\ Even if the chip is powered under 5V, I2C SDA/SCL pull-ups SHALL be tied at 3.3V /!\ */

/*    <------------------8 I/O Expenders------------------><----------------16 I/O Expenders--------------->    */
enum {IO_EXP_PCF8574 = 0, IO_EXP_PCF8574A, IO_EXP_PCA9654E, IO_EXP_MCP23017, IO_EXP_PCF8575A, IO_EXP_PCA9671, IO_EXP_TYPE_NB};

/* PRIVATE FUNCTION PROTOYPES */
static uint8_t getMsgType     (uint8_t XanyIdx);
static uint8_t readIoExtender (uint8_t XanyIdx, uint8_t *RxBuf, uint8_t ByteToRead);
static uint8_t readAngleSensor(uint8_t XanyIdx, uint16_t *Angle);
static void    updateXanyMsgChecksum(XanyMsg_union *XanyMsg);
static uint8_t getBigEndianNibbleNbToTx(XanyMsg_union *XanyMsg);

/* The read functions for all the supported I/O expenders */

static const uint8_t readPcf8574(uint8_t I2cAddr, uint8_t *RxBuf)
{
  return(i2c_receive( I2cAddr, RxBuf, 1));
}

static const uint8_t readPcf8574A(uint8_t I2cAddr, uint8_t *RxBuf)
{
  return(i2c_receive( I2cAddr, RxBuf, 1));
}

static const uint8_t readMcp23017(uint8_t I2cAddr, uint8_t *RxBuf)
{
  return(i2c_readReg(I2cAddr, 0x12, RxBuf, 2));
}

static const uint8_t readPcf8575A(uint8_t I2cAddr, uint8_t *RxBuf)
{
  return(i2c_receive( I2cAddr, RxBuf, 2));
}

static const uint8_t readPca9671 (uint8_t I2cAddr, uint8_t *RxBuf)
{
  return(i2c_receive( I2cAddr, RxBuf, 2));
}

static const uint8_t readPca9654e(uint8_t I2cAddr, uint8_t *RxBuf)
{
  return(i2c_receive( I2cAddr, RxBuf, 1));
}

/* A little typedef to short the notation */
typedef const uint8_t (*ReadIoExpPtr)(uint8_t I2cAddr, uint8_t *RxBuf);

typedef struct{
  uint8_t      IoExtType;
  uint8_t      I2c7bAddr;
  ReadIoExpPtr ReadIoExp;
}I2cIoExpSt_t;

const I2cIoExpSt_t XanyI2cTypeAddr[] PROGMEM = {
          /*  IoExtType   I2c7bAddr<<1 ReadIoExp   Idx in I2cDevMap */
/*Xany0*/ {IO_EXP_PCF8574,  (0x20<<1), readPcf8574 }, /* Idx =  0 */
          {IO_EXP_PCF8574,  (0x22<<1), readPcf8574 }, /* Idx =  1 */
          {IO_EXP_PCF8574A, (0x38<<1), readPcf8574A}, /* Idx =  2 */
          {IO_EXP_PCF8574A, (0x3A<<1), readPcf8574A}, /* Idx =  3 */
          {IO_EXP_MCP23017, (0x24<<1), readMcp23017}, /* Idx =  4 */
          {IO_EXP_PCF8575A, (0x3C<<1), readPcf8575A}, /* Idx =  5 */
          {IO_EXP_PCA9671,  (0x28<<1), readPca9671 }, /* Idx =  6 */
/*Xany1*/ {IO_EXP_PCF8574,  (0x21<<1), readPcf8574 }, /* Idx =  7 */
          {IO_EXP_PCF8574,  (0x23<<1), readPcf8574 }, /* Idx =  8 */
          {IO_EXP_PCF8574A, (0x39<<1), readPcf8574A}, /* Idx =  9 */
          {IO_EXP_PCF8574A, (0x3B<<1), readPcf8574A}, /* Idx = 10 */
          {IO_EXP_MCP23017, (0x25<<1), readMcp23017}, /* Idx = 11 */
          {IO_EXP_PCF8575A, (0x3D<<1), readPcf8575A}, /* Idx = 12 */
          {IO_EXP_PCA9671,  (0x29<<1), readPca9671 }, /* Idx = 13 */
/*Xany2*/ {IO_EXP_PCA9654E, (0x10<<1), readPca9654e}, /* Idx = 14 */
          {IO_EXP_PCA9654E, (0x12<<1), readPca9654e}, /* Idx = 15 */
          {IO_EXP_MCP23017, (0x26<<1), readMcp23017}, /* Idx = 16 */
          {IO_EXP_PCF8575A, (0x3E<<1), readPcf8575A}, /* Idx = 17 */
          {IO_EXP_PCA9671,  (0x2A<<1), readPca9671 }, /* Idx = 18 */
/*Xany3*/ {IO_EXP_PCA9654E, (0x11<<1), readPca9654e}, /* Idx = 19 */
          {IO_EXP_PCA9654E, (0x13<<1), readPca9654e}, /* Idx = 20 */
          {IO_EXP_MCP23017, (0x27<<1), readMcp23017}, /* Idx = 21 */
          {IO_EXP_PCF8575A, (0x3F<<1), readPcf8575A}, /* Idx = 22 */
          {IO_EXP_PCA9671,  (0x2B<<1), readPca9671 }, /* Idx = 23 */
                            };

typedef struct{
    uint8_t FirstIdx;
    uint8_t LastIdx;
}XanyIdxRangeSt_t;

const XanyIdxRangeSt_t XanyIdxRange[] PROGMEM = {{0, 6}, {7, 13}, {14, 18}, {19, 23}};

#define SUPPORTED_I2C_IO_EXP_NB       DIM(XanyI2cTypeAddr)

#define GET_FIRST_IDX(XanyIdx)        (uint8_t)     (pgm_read_byte_far(pgm_get_far_address(XanyIdxRange) + XanyIdx*2))
#define GET_LAST_IDX(XanyIdx)         (uint8_t)     (pgm_read_byte_far(pgm_get_far_address(XanyIdxRange) + XanyIdx*2 + 1))

#define GET_I2C_IO_EXP_TYPE(Idx)      (uint8_t)     (pgm_read_byte_far(pgm_get_far_address(XanyI2cTypeAddr) + Idx*4))
#define GET_I2C_IO_EXP_7B_ADDR(Idx)   (uint8_t)     (pgm_read_byte_far(pgm_get_far_address(XanyI2cTypeAddr) + Idx*4 + 1))
#define GET_I2C_IO_EXP_READ(Idx)      (ReadIoExpPtr)(pgm_read_word_far(pgm_get_far_address(XanyI2cTypeAddr) + Idx*4 + 2))

#define htonl(x)                      __builtin_bswap32((uint32_t) (x))

typedef struct {
  uint16_t
                  TxInProgress:     1,
                  SentCnt:          3,
                  CurIdx:           5, /* Prev Nibble to compare to the following one */
                  PrevIdx:          5, /* Prev Nibble to compare to the following one */
                  Reserved:         2;
}TxNibbleSt_t;  /* Size = 2 bytes */

typedef struct{
  XanyMsg_union   Msg; /* Sending buffer (in interrupt) */
  uint8_t         NibbleIdx;
  TxNibbleSt_t    Nibble;
}X_OneAnyWriteMsgSt_t;

static uint32_t I2cDevMap = 0UL; /* 4 bytes for the map of the 4 X-Any instances */

static          XanyMsg_union        X_AnyReadMsg[NUM_X_ANY]; /* 4 bytes per X-Any for storing read Msg */
static volatile X_OneAnyWriteMsgSt_t X_AnyWriteMsg[NUM_X_ANY];/* 7 bytes per X-Any for sending Msg in interrupt (-> Volatile) */


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
  uint8_t Idx, XanyIdx, I2c7bAddr, Data;

  /* Clear the whole structure for the n instances */
  memset((void*)&X_AnyWriteMsg, 0, sizeof(X_AnyWriteMsg));

  I2C_SPEED_400K();
  /* Probe I2C bus to discover Io Expender chips */
  for(Idx = 0; Idx < SUPPORTED_I2C_IO_EXP_NB; Idx++)
  {
    I2c7bAddr = GET_I2C_IO_EXP_7B_ADDR(Idx);
    if(!i2c_start(I2c7bAddr | I2C_WRITE))
    {
      /* OK: device is present quit gracefully by sending a stop() */
      i2c_stop();
      if(GET_I2C_IO_EXP_TYPE(Idx) == IO_EXP_MCP23017)
      {
        /* Enable internal Pull-up */
        Data = 0xFF;
        i2c_writeReg(I2c7bAddr , 0x0C, &Data, 1);
        i2c_writeReg(I2c7bAddr , 0x0D, &Data, 1);
      }
      I2cDevMap |= (1UL << Idx); /* Mark it as present */
    }
  }
  /* Probe I2C bus to discover Angle Sensor chips */
  for(XanyIdx = 0; XanyIdx < X_ANY; XanyIdx++)
  {
    I2c7bAddr = ((A1335_I2C_7B_ADDR + XanyIdx) << 1);
    if(!i2c_start(I2c7bAddr | I2C_WRITE))
    {
      /* OK: A1335 device is present quit gracefully by sending a stop() */
      i2c_stop();
      I2cDevMap |= (1UL << (SUPPORTED_I2C_IO_EXP_NB + XanyIdx)); /* Mark it as present */
    }
  }
  I2C_SPEED_888K();
}

/**
* \file   Xany.cpp
* \fn     uint8_t Xany_readInputsAndLoadMsg(uint8_t XanyIdx)
* \brief  Reads the I2C I/O expenders, Angle sensor and Rot POT associated to the specified X-Any instance and load it in the Tx buffer
* \param  XanyIdx: Index of the X-Any instance
* \return 0: Not done, previous message not completely sent, 1: Done, new message in the Tx buffer
*/
uint8_t Xany_readInputsAndLoadMsg(uint8_t XanyIdx)
{
  uint8_t      Done = 0;

#if defined(EXTERNALEEPROM) && !defined(SIMU)
#define CHECK_IIC_USED_IRQ_MODE_RETURN(x) if (TWCR & _BV(TWINT)) return(x)
#else
#define CHECK_IIC_USED_IRQ_MODE_RETURN(x) // I2C bus is free !
#endif

  if(g_model.Xany[XanyIdx].Active)
  {
    CHECK_IIC_USED_IRQ_MODE_RETURN(Done); /* Return if I2C is used */

    /* Read inputs and load the message in X_AnyReadMsg[] (will be loaded in X_AnyWriteMsg[] in Xany_scheduleTx()) */
    Xany_operation(XanyIdx, XANY_OP_BUILD_MSG, NULL); /* This reads the I2C bus */
    Done = 1;
  }
  return(Done);
}

/**
* \file   Xany.cpp
* \fn     void Xany_scheduleTx(uint8_t XanyIdx)
* \brief  Schedule the transmisson of the X-Any message (One nibble per CPPM frame with PPM proto)
* \param  XanyIdx: Index of the X-Any instance
* \return Void
*/
void Xany_scheduleTx(uint8_t XanyIdx)
{
  X_OneAnyWriteMsgSt_t *t;
  char                 *CharPtr, TxChar;
  uint8_t               NibbleNbToTx;

  if(g_model.Xany[XanyIdx].Active)
  {
    t = (X_OneAnyWriteMsgSt_t *)&X_AnyWriteMsg[XanyIdx]; /* XanyIdx SHALL be < NUM_X_ANY */
    /* Here, take care that X_AnyWriteMsg is in big endian, then t->Msg.Common.NibbleNbToTx CANNOT be used! */
    NibbleNbToTx = getBigEndianNibbleNbToTx((XanyMsg_union *)&t->Msg);
    if(!t->Nibble.TxInProgress)
    {
      t->Nibble.TxInProgress = 1;
      /* Get next char to send */
      if(t->NibbleIdx < NibbleNbToTx)
      {
        CharPtr = (char *)t;
        TxChar = CharPtr[t->NibbleIdx / 2];
        if(!(t->NibbleIdx & 1)) t->Nibble.CurIdx = ((TxChar & 0xF0) >> 4); /* MSN first */
        else                    t->Nibble.CurIdx =   TxChar & 0x0F;        /* LSN */
      }
      else
      {
        t->Nibble.CurIdx = NIBBLE_I; /* Nothing to transmit */
      }
      if(t->Nibble.CurIdx == t->Nibble.PrevIdx) t->Nibble.CurIdx = NIBBLE_R; /* Repeat symbol */
      t->Nibble.PrevIdx = t->Nibble.CurIdx;
    }
    /* Send the Nibble or the Repeat or the Idle symbol */
    uint_farptr_t ExcursionHalf_us_Far_Adress = pgm_get_far_address(ExcursionHalf_us); /* Get 32 bits adress */
    ExcursionHalf_us_Far_Adress += (2 * t->Nibble.CurIdx); /* Compute offset */
    int16_t valueTemp = (int16_t)pgm_read_word_far(ExcursionHalf_us_Far_Adress);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    channelOutputs[g_model.Xany[XanyIdx].ChId] = valueTemp; /* overwrite in int-level */
    }
    t->Nibble.SentCnt++;
    if(t->Nibble.SentCnt >= (g_model.Xany[XanyIdx].RepeatNb + 1))
    {
      /* Symbol sent taking into account the repetition(s) */
      if(t->NibbleIdx < NibbleNbToTx)
      {
        /* Next Nibble */
        t->NibbleIdx++;
      }
      else
      {
        /* The full message is sent: load the pending X_AnyReadMsg[XanyIdx] message in X_AnyWriteMsg[XanyIdx] message */
        t->Msg.Raw   = X_AnyReadMsg[XanyIdx].Raw;
        t->NibbleIdx = 0;
      }
      t->Nibble.SentCnt = 0;
      t->Nibble.TxInProgress = 0;
    }
  }
}

/**
* \file   Xany.cpp
* \fn     void Xany_operation(uint8_t XanyIdx, uint8_t XanyOp, XanyInfoSt_t *XanyInfo)
* \brief  Reads the I2C I/O expenders and sensors or information associated to the specified X-Any instance
* \param  XanyIdx:  Index of the X-Any instance
* \param  XanyOp:   The X-Any operation
* \param  XanyInfo: Pointer on an X-Any information structure to fill
* \return 0: not supported message, 1: supported message
*/
uint8_t Xany_operation(uint8_t XanyIdx, uint8_t XanyOp, XanyInfoSt_t *XanyInfo)
{
  XanyMsg_union    Built, Read;
  uint8_t          MsgType, One8bitPort = 0, ValidMsg = 1;
  uint16_t         Two8bitPorts = 0, Angle = 0;

  Built.Raw = 0; /* Clear temporary message */
  MsgType = getMsgType(XanyIdx);
  if(XanyOp & XANY_OP_READ_INFO)
  {
    Read.Raw = htonl(X_AnyReadMsg[XanyIdx].Raw);
  }
  switch(MsgType)
  {
    case XANY_MSG_4SW:
    Built.Msg4Sw.NibbleNbToTx = XANY_MSG_4SW_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
      Built.Msg4Sw.Sw = One8bitPort & 0x0F; /* Keep 4 bits */
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Read.Msg4Sw.NibbleNbToTx;
      XanyInfo->SwNb         = 4;
      XanyInfo->SwValue      = (uint16_t)Read.Msg4Sw.Sw;
      XanyInfo->Angle        = 0;
      XanyInfo->PotValue     = 0;
    }
    break;

    case XANY_MSG_8SW:
    Built.Msg8Sw.NibbleNbToTx = XANY_MSG_8SW_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
      Built.Msg8Sw.Sw = One8bitPort;
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Read.Msg8Sw.NibbleNbToTx;
      XanyInfo->SwNb         = 8;
      XanyInfo->SwValue      = (uint16_t)Read.Msg8Sw.Sw;
      XanyInfo->Angle        = 0;
      XanyInfo->PotValue     = 0;
    }
    break;

    case XANY_MSG_16SW:
    Built.Msg16Sw.NibbleNbToTx = XANY_MSG_16SW_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&Two8bitPorts, 2);
      Built.Msg16Sw.Sw = Two8bitPorts;
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Read.Msg16Sw.NibbleNbToTx;
      XanyInfo->SwNb         = 16;
      XanyInfo->SwValue      = (uint16_t)Read.Msg16Sw.Sw;
      XanyInfo->Angle        = 0;
      XanyInfo->PotValue     = 0;
    }
    break;

    case XANY_MSG_ANGLE:
    Built.MsgAngle.NibbleNbToTx = XANY_MSG_ANGLE_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readAngleSensor(XanyIdx, &Angle);
      Built.MsgAngle.Angle = Angle;
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Read.MsgAngle.NibbleNbToTx;
      XanyInfo->SwNb         = 0;
      XanyInfo->SwValue      = 0;
      XanyInfo->Angle        = Read.MsgAngle.Angle;
      XanyInfo->PotValue     = 0;
    }
    break;

    case XANY_MSG_ANGLE_4SW:
    Built.MsgAngle4Sw.NibbleNbToTx = XANY_MSG_ANGLE_4SW_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
      Built.MsgAngle4Sw.Sw    = One8bitPort & 0x0F; /* Keep 4 bits */
      readAngleSensor(XanyIdx, &Angle);
      Built.MsgAngle4Sw.Angle = Angle;
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Read.MsgAngle4Sw.NibbleNbToTx;
      XanyInfo->SwNb         = 4;
      XanyInfo->SwValue      = (uint16_t)Read.MsgAngle4Sw.Sw;
      XanyInfo->Angle        = Read.MsgAngle4Sw.Angle;
      XanyInfo->PotValue     = 0;
    }
    break;

    case XANY_MSG_ANGLE_8SW:
    Built.MsgAngle8Sw.NibbleNbToTx = XANY_MSG_ANGLE_8SW_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
      Built.MsgAngle8Sw.Sw    = One8bitPort;
      readAngleSensor(XanyIdx, &Angle);
      Built.MsgAngle8Sw.Angle = Angle;
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Read.MsgAngle8Sw.NibbleNbToTx;
      XanyInfo->SwNb         = 8;
      XanyInfo->SwValue      = Read.MsgAngle8Sw.Sw;
      XanyInfo->Angle        = Read.MsgAngle8Sw.Angle;
      XanyInfo->PotValue     = 0;
    }
    break;

    case XANY_MSG_ANGLE_POT:
    Built.MsgAnglePot.NibbleNbToTx = XANY_MSG_ANGLE_POT_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readAngleSensor(XanyIdx, &Angle);
      Built.MsgAnglePot.Angle = Angle;
      Built.MsgAnglePot.Pot   = ((calibratedStick[GET_XANY_POT(XanyIdx)] + RESX - 1) / 8); /* 8 bits value */
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Read.MsgAnglePot.NibbleNbToTx;
      XanyInfo->SwNb         = 0;
      XanyInfo->SwValue      = 0;
      XanyInfo->Angle        = Read.MsgAnglePot.Angle;
      XanyInfo->PotValue     = Read.MsgAnglePot.Pot;
    }
    break;

    case XANY_MSG_POT_4SW:
    Built.MsgPot4Sw.NibbleNbToTx = XANY_MSG_POT_4SW_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
      Built.MsgPot4Sw.Sw    = One8bitPort & 0x0F; /* Keep 4 bits */
      Built.MsgPot4Sw.Pot   = ((calibratedStick[GET_XANY_POT(XanyIdx)] + RESX - 1) / 8); /* 8 bits value */
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Read.MsgPot4Sw.NibbleNbToTx;
      XanyInfo->SwNb         = 4;
      XanyInfo->SwValue      = Read.MsgPot4Sw.Sw;
      XanyInfo->Angle        = 0;
      XanyInfo->PotValue     = Read.MsgPot4Sw.Pot;
    }
    break;

    case XANY_MSG_POT_8SW:
    Built.MsgPot8Sw.NibbleNbToTx = XANY_MSG_POT_8SW_NBL_NB;
    if (XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
      Built.MsgPot8Sw.Sw    = One8bitPort;
      Built.MsgPot8Sw.Pot   = ((calibratedStick[GET_XANY_POT(XanyIdx)] + RESX - 1) / 8); /* 8 bits value */
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Read.MsgPot8Sw.NibbleNbToTx;
      XanyInfo->SwNb         = 8;
      XanyInfo->SwValue      = Read.MsgPot8Sw.Sw;
      XanyInfo->Angle        = 0;
      XanyInfo->PotValue     = Read.MsgPot8Sw.Pot;
    }
    break;

    default: /* We arrive here if the composed X-Any message from the LCD menu is not supported */
    ValidMsg = 0; /* Invalid or not yet supported message */
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      Built.Common.PayloadAndChecksum = 0; /* Detroy the checksum to be sure the message won't be interpreted at receiver side (1st stage) */
      Built.Common.NibbleNbToTx       = 0; /* Prevent transmission (2nd stage) */
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = 0;
      XanyInfo->SwNb         = 0;
      XanyInfo->SwValue      = 0;
      XanyInfo->Angle        = 0;
      XanyInfo->PotValue     = 0;
    }
    break;
  }
  /* Can be common */
  if(XanyOp & XANY_OP_BUILD_MSG)
  {
    /* Update Checksum */
    updateXanyMsgChecksum((XanyMsg_union *)&Built); /* /!\ Now, Built XanyMsg is in Big endian /!\ */
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    X_AnyReadMsg[XanyIdx].Raw = Built.Raw; /* Load the temporary built message in X_AnyReadMsg */
    }
  }
  if(XanyOp & XANY_OP_READ_INFO)
  {
    XanyInfo->TxPeriodMs = ((g_model.Xany[XanyIdx].RepeatNb + 1) * (XanyInfo->MsgNibbleLen + 1) * 225) / 10; /* + 1 for IDLE symbol */
  }
  return(ValidMsg);
}

/* PRIVATE FUNCTIONS */
/**
* \file   Xany.cpp
* \fn     uint8_t getMsgType(uint8_t XanyIdx)
* \brief  Returns the type of X-Any message
* \param  XanyIdx: Index of the X-Any
* \return The type of X-Any message among XANY_MSG_4SW - XANY_MSG_POT_8SW
*/
static uint8_t getMsgType(uint8_t XanyIdx)
{
  return(g_model.Xany[XanyIdx].PayloadCfg.Raw & X_ANY_CFG_MSK);
}

/**
* \file   Xany.cpp
* \fn     uint8_t readIoExtender(uint8_t XanyIdx, uint8_t *RxBuf, uint8_t ByteToRead)
* \brief  Reads the I2C I/O expenders associated to the specified X-Any instance
* \param  XanyIdx:    Index of the X-Any
* \param  RxBuf:      Pointer on Rx Buffer
* \param  ByteToRead: The number of bytes to read
* \return The number of read byte(s)
*/
static uint8_t readIoExtender(uint8_t XanyIdx, uint8_t *RxBuf, uint8_t ByteToRead)
{
  ReadIoExpPtr ReadIoExp;
  uint8_t      I2c7bAddr, ExpType, One8bitPort = 0, ByteRead = 0;
  uint16_t     Two8bitPorts = 0;
  uint8_t     *BytePtr  = (uint8_t  *)RxBuf;
  uint16_t    *WordPtr  = (uint16_t *)RxBuf;

  for(uint8_t BitIdx = GET_FIRST_IDX(XanyIdx); BitIdx <= GET_LAST_IDX(XanyIdx); BitIdx++)
  {
    if(I2cDevMap & (1UL << BitIdx))
    {
      /* Chip is present in the I/O Expender map */
      I2c7bAddr = GET_I2C_IO_EXP_7B_ADDR(BitIdx);
      ExpType   = GET_I2C_IO_EXP_TYPE(BitIdx);
      ReadIoExp = GET_I2C_IO_EXP_READ(BitIdx);
      if((ExpType == IO_EXP_PCF8574) || (ExpType == IO_EXP_PCF8574A) || (ExpType == IO_EXP_PCF8575A))
      {
        I2C_SPEED_400K();
      }
      else
      {
        I2C_SPEED_888K();
      }
      if(ExpType < IO_EXP_MCP23017)
      {
        /* Read one 8 bit port */
        if(!ReadIoExp(I2c7bAddr , (uint8_t*)&One8bitPort)) One8bitPort ^= 0xFF; /* Apply polarity: close contact = 1 */
        if(BitIdx & 1) BytePtr[1] = One8bitPort; /* Odd  */
        else           BytePtr[0] = One8bitPort; /* Even */
        ByteRead++;
      }
      else
      {
        /* Read two 8 bit ports */
        if(!ReadIoExp(I2c7bAddr, (uint8_t*)&Two8bitPorts)) Two8bitPorts ^= 0xFFFF; /* Apply polarity: close contact = 1 */
        *WordPtr = Two8bitPorts;
        ByteRead += 2;
      }
      if(ByteRead >= ByteToRead) break;
    }
  }
  I2C_SPEED_888K();

 return(ByteRead);
}

/**
* \file   Xany.cpp
* \fn     uint8_t readAngleSensor(uint8_t XanyIdx, uint16_t *Angle)
* \brief  Reads the I2C angle sensor associated to the specified X-Any instance
* \param  XanyIdx:    Index of the X-Any
* \param  Angle:      Pointer on the destination 16 bits buffer
* \return 0: OK,   1: Error
*/
static uint8_t readAngleSensor(uint8_t XanyIdx, uint16_t *Angle)
{
  #define A1335_ANG_REG_ADDR  (0x20)
  uint32_t PresenceMask;
  uint8_t  AngleSensor7BitAddr, Ret = 0;

  PresenceMask = (1UL << (SUPPORTED_I2C_IO_EXP_NB + XanyIdx));
  if(I2cDevMap & PresenceMask)
  {
    /* Try to read angle only f chip is present */
    AngleSensor7BitAddr = A1335_I2C_7B_ADDR + XanyIdx;
    I2C_SPEED_400K();
    Ret = i2c_readReg((AngleSensor7BitAddr << 1), A1335_ANG_REG_ADDR, (uint8_t *)Angle, 2);
    I2C_SPEED_888K();
    *Angle &= 0x0FFF; /* Keep only angle value */
  }
  return(Ret);
}

/**
* \file   Xany.cpp
* \fn     void updateXanyMsgChecksum(XanyMsg_union *XanyMsg)
* \brief  Computes the checksum for the X-Any message AND converts it in big endian
* \param  XanyMsg:    pointer in an X-Any union
* \return Void
*/
static void updateXanyMsgChecksum(XanyMsg_union *XanyMsg)
{
  uint8_t PayloadNibbleNb, Checksum = 0;
  uint8_t *BytePtr;

  /* Checksum of passed XanyMsg structure SHALL be 0 */
  PayloadNibbleNb = XanyMsg -> Common.NibbleNbToTx - 2;
  BytePtr = (uint8_t *)XanyMsg;
  XanyMsg->Raw = htonl(XanyMsg->Raw); /* Now: Big endian to have Nibbles from most to least significant order */
  for(uint8_t ByteIdx = 0; ByteIdx < (PayloadNibbleNb + 1) / 2; ByteIdx++)
  {
    Checksum ^= BytePtr[ByteIdx];
  }
  Checksum ^= 0x55;
  if(PayloadNibbleNb & 1)
  {
    /* Odd number of nibble -> Checksum between 2 contiguous bytes */
    BytePtr    = (uint8_t *)XanyMsg + (PayloadNibbleNb / 2);
    *BytePtr = *BytePtr | ((Checksum & 0xF0) >> 4); /* Most  Significant Nibble of Checksum */
    BytePtr++;
    *BytePtr |= ((Checksum & 0x0F) << 4); /* Least Significant Nibble of Checksum */
  }
  else
  {
    /* Even number of nibble -> Checksum in a single byte */
    BytePtr    = (uint8_t *)XanyMsg + (PayloadNibbleNb / 2);
    *BytePtr   = Checksum;
  }
}

/**
* \file   Xany.cpp
* \fn     uint8_t getBigEndianNibbleNbToTx(XanyMsg_union *XanyMsg)
* \brief  Get the number of nibbles to transmit from a big endian X-Any message
* \param  XanyMsg:    pointer in an X-Any union
* \return The number of nibbles to transmit (including the checksum)
*/
static uint8_t getBigEndianNibbleNbToTx(XanyMsg_union *XanyMsg)
{
  uint8_t *BytePtr;
  BytePtr = (uint8_t *)XanyMsg;
  return(BytePtr[3] & 0x0F);
}

