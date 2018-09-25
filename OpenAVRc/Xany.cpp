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
                  Sw          :4,
                  Checksum    :8,
                  NotUsed     :16,
                  NibbleNbToTx:4;
}Msg4SwSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  Sw          :8,
                  Checksum    :8,
                  NotUsed     :12,
                  NibbleNbToTx:4;
}Msg8SwSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  Sw          :16,
                  Checksum    :8,
                  NotUsed     :4,
                  NibbleNbToTx:4;
}Msg16SwSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  Angle       :12,
                  Checksum    :8,
                  NotUsed     :8,
                  NibbleNbToTx:4;
}MsgAngleSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  Angle       :12,
                  Sw          :4,
                  Checksum    :8,
                  NotUsed     :4,
                  NibbleNbToTx:4;
}MsgAngle4SwSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  Angle       :12,
                  Sw          :8,
                  Checksum    :8,
                  NibbleNbToTx:4;
}MsgAngle8SwSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  Angle       :12,
                  Pot         :8,
                  Checksum    :8,
                  NibbleNbToTx:4;
}MsgAnglePotSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  Pot         :8,
                  Sw          :4,
                  Checksum    :8,
                  NotUsed     :8,
                  NibbleNbToTx:4;
}MsgPot4SwSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  Pot         :8,
                  Sw          :8,
                  Checksum    :8,
                  NotUsed     :4,
                  NibbleNbToTx:4;
}MsgPot8SwSt_t; /* Size = 4 bytes */

typedef struct{
  uint32_t
                  PayloadAndChecksum:28,
                  NibbleNbToTx      :4;
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
}XanyMsg_union;  /* Size = 4 bytes */

/* Declaration of supported combinations (See X_ANY_CFG() macro in myeeprom.h) */
#define XANY_MSG_4SW              X_ANY_CFG(1, 0, 0)
#define XANY_MSG_8SW              X_ANY_CFG(2, 0, 0)
#define XANY_MSG_16_SW            X_ANY_CFG(3, 0, 0)
#define XANY_MSG_ANGLE            X_ANY_CFG(0, 1, 0)
#define XANY_MSG_ANGLE_4SW        X_ANY_CFG(1, 1, 0)
#define XANY_MSG_ANGLE_8SW        X_ANY_CFG(2, 1, 0)
#define XANY_MSG_ANGLE_POT        X_ANY_CFG(0, 1, 1)
#define XANY_MSG_POT_4SW          X_ANY_CFG(1, 0, 1)
#define XANY_MSG_POT_8SW          X_ANY_CFG(2, 0, 1)

/* Message length (in nibbles) including the checksum for each supported combination */
#define XANY_MSG_4SW_NBL_NB       (1 + 2)
#define XANY_MSG_8SW_NBL_NB       (2 + 2)
#define XANY_MSG_16_SW_NBL_NB     (4 + 2)
#define XANY_MSG_ANGLE_NBL_NB     (3 + 2)
#define XANY_MSG_ANGLE_4SW_NBL_NB (3 + 1 + 2)
#define XANY_MSG_ANGLE_8SW_NBL_NB (3 + 2 + 2)
#define XANY_MSG_ANGLE_POT_NBL_NB (3 + 2 + 2)
#define XANY_MSG_POT_4SW_NBL_NB   (2 + 1 + 2)
#define XANY_MSG_POT_8SW_NBL_NB   (2 + 2 + 2)

/* Array of Pots choosen */
const uint8_t XANY_POT[] PROGMEM = {X_ANY_1_POT, X_ANY_2_POT, X_ANY_3_POT, X_ANY_4_POT};
#define GET_XANY_POT(PotIdx)        (uint8_t)     pgm_read_byte_far(&XANY_POT[PotIdx])


/*    <------------------8 I/O Expenders------------------><----------------16 I/O Expenders--------------->    */
enum {IO_EXP_PCF8574 = 0, IO_EXP_PCF8574A, IO_EXP_PCA9654E, IO_EXP_MCP23017, IO_EXP_PCF8575A, IO_EXP_PCA9671, IO_EXP_TYPE_NB};

/* PRIVATE FUNCTION PROTOYPES */
static uint8_t getMsgType     (uint8_t XanyIdx);
static uint8_t readIoExtender (uint8_t XanyIdx, uint8_t *RxBuf, uint8_t ByteToRead);

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
  return(i2c_receive( I2cAddr, RxBuf, 2));
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
          /*  IoExtType   I2c7bAddr ReadIoExp   Idx in IoExpMap */
/*Xany0*/ {IO_EXP_PCF8574,  0x20, readPcf8574 }, /* Idx =  0 */
          {IO_EXP_PCF8574,  0x22, readPcf8574 }, /* Idx =  1 */
          {IO_EXP_PCF8574A, 0x38, readPcf8574A}, /* Idx =  2 */
          {IO_EXP_PCF8574A, 0x3A, readPcf8574A}, /* Idx =  3 */
          {IO_EXP_MCP23017, 0x24, readMcp23017}, /* Idx =  4 */
          {IO_EXP_PCF8575A, 0x3C, readPcf8575A}, /* Idx =  5 */
          {IO_EXP_PCA9671,  0xC0, readPca9671 }, /* Idx =  6 */
/*Xany1*/ {IO_EXP_PCF8574,  0x21, readPcf8574 }, /* Idx =  7 */
          {IO_EXP_PCF8574,  0x23, readPcf8574 }, /* Idx =  8 */
          {IO_EXP_PCF8574A, 0x39, readPcf8574A}, /* Idx =  9 */
          {IO_EXP_PCF8574A, 0x3B, readPcf8574A}, /* Idx = 10 */
          {IO_EXP_MCP23017, 0x25, readMcp23017}, /* Idx = 11 */
          {IO_EXP_PCF8575A, 0x3D, readPcf8575A}, /* Idx = 12 */
          {IO_EXP_PCA9671,  0x4A, readPca9671 }, /* Idx = 13 */
/*Xany2*/ {IO_EXP_PCA9654E, 0x40, readPca9654e}, /* Idx = 14 */
          {IO_EXP_PCA9654E, 0x44, readPca9654e}, /* Idx = 15 */
          {IO_EXP_MCP23017, 0x26, readMcp23017}, /* Idx = 16 */
          {IO_EXP_PCF8575A, 0x3E, readPcf8575A}, /* Idx = 17 */
          {IO_EXP_PCA9671,  0x4C, readPca9671 }, /* Idx = 18 */
/*Xany3*/ {IO_EXP_PCA9654E, 0x42, readPca9654e}, /* Idx = 19 */
          {IO_EXP_PCA9654E, 0x46, readPca9654e}, /* Idx = 20 */
          {IO_EXP_MCP23017, 0x27, readMcp23017}, /* Idx = 21 */
          {IO_EXP_PCF8575A, 0x3F, readPcf8575A}, /* Idx = 22 */
          {IO_EXP_PCA9671,  0x4E, readPca9671 }, /* Idx = 23 */
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

static uint32_t IoExpMap = 0L;//xFFFFFFFF; /* 4 bytes for the map of the 4 X-Any instances */

static          XanyMsg_union        X_AnyReadMsg[NUM_X_ANY]; /* 4 bytes per X-Any for storing read Msg */
static volatile X_OneAnyWriteMsgSt_t X_AnyWriteMsg[NUM_X_ANY];/* 8 bytes per X-Any for sending Msg in interrupt (-> Volatile) */


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
  uint8_t Idx, I2c7bAddr, Data;

  /* Clear the whole structure for the n instances */
  memset((void*)&X_AnyWriteMsg, 0, sizeof(X_AnyWriteMsg));

  I2C_SPEED_400K();
  /* Probe I2C bus to discover Io Expender chips */
  for(Idx = 0; Idx < SUPPORTED_I2C_IO_EXP_NB; Idx++)
  {
    I2c7bAddr = GET_I2C_IO_EXP_7B_ADDR(Idx);;
    if(!i2c_start((I2c7bAddr << 1) | I2C_WRITE))
    {
      /* OK: device is present quit gracefully by sending a stop() */
      i2c_stop();
      if(GET_I2C_IO_EXP_TYPE(Idx) == IO_EXP_MCP23017)
      {
        /* Enable internal Pull-up */
        Data = 0xFF;
        i2c_writeReg((I2c7bAddr << 1), 0x06, &Data, 1);
        i2c_writeReg((I2c7bAddr << 1), 0x16, &Data, 1);
      }
      IoExpMap |= (1L << Idx); /* Mark it as present */
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
  XanyInfoSt_t XanyInfo;

#if defined(EXTERNALEEPROM) && !defined(SIMU)
#define CHECK_IIC_USED_IRQ_MODE_RETURN(x) if (TWCR & _BV(TWINT)) return(x)
#else
#define CHECK_IIC_USED_IRQ_MODE_RETURN(x) // I2C bus is free !
#endif

  CHECK_IIC_USED_IRQ_MODE_RETURN(Done); /* Return if I2C is used */

  if(X_AnyWriteMsg[XanyIdx].NibbleIdx >= (X_AnyWriteMsg[XanyIdx].Msg.Common.NibbleNbToTx + 1))
  {
    /* No need to mask interrupt since X_AnyWriteMsg[] is not use when X_AnyWriteMsg[XanyIdx].NibbleIdx is >= (X_AnyWriteMsg[XanyIdx].Msg.NibbleNbToTx + 1) */
    Xany_operation(XanyIdx, XANY_OP_BUILD_MSG, &XanyInfo); /* This reads the I2C bus */
    /* Load from Read structure to Write structure */
    memcpy((void*)&X_AnyWriteMsg[XanyIdx].Msg, (void*)&X_AnyReadMsg[XanyIdx], sizeof(XanyMsg_union)); /* Copy 4 bytes */
    X_AnyWriteMsg[XanyIdx].NibbleIdx = 0; /* Go! */
    Done = 1;
  }
  return(Done);
}

/**
* \file   Xany.cpp
* \fn     void Xany_scheduleTx(uint8_t XanyIdx)
* \brief  Schedule the transmisson of the X-Any message
* \param  XanyIdx: Index of the X-Any instance
* \return Void
*/
void Xany_scheduleTx(uint8_t XanyIdx)
{
  X_OneAnyWriteMsgSt_t *t;
  char                 *CharPtr, TxChar;

  if(g_model.Xany[XanyIdx].Active)
  {
    t = (X_OneAnyWriteMsgSt_t *)&X_AnyWriteMsg[XanyIdx]; /* XanyIdx SHALL be < NUM_X_ANY */

    if(!t->Nibble.TxInProgress)
    {
      t->Nibble.TxInProgress = 1;
      /* Get next char to send */
      if(t->NibbleIdx < t->Msg.Common.NibbleNbToTx)
      {
        CharPtr = (char *)t;
        TxChar = CharPtr[t->NibbleIdx / 2];
        if(!(t->NibbleIdx & 1)) t->Nibble.CurIdx = ((TxChar & 0xF0) >> 4); /* MSN first */
        else                    t->Nibble.CurIdx =   TxChar & 0x0F;        /* LSN */
      }
      else
      {
        t->Nibble.CurIdx = NIBBLE_I; /* Nothing to transmit */
        if(t->NibbleIdx < (t->Msg.Common.NibbleNbToTx + 1)) t->NibbleIdx++; /* Bounded to NibbleNbToTx + 1: meanst synchro to allow reload of the new message */
      }
      if(t->Nibble.CurIdx == t->Nibble.PrevIdx) t->Nibble.CurIdx = NIBBLE_R; /* Repeat symbol */
      t->Nibble.PrevIdx = t->Nibble.CurIdx;
    }
    /* Send the Nibble or the Repeat or the Idle symbol */
    uint_farptr_t ExcursionHalf_us_Far_Adress = pgm_get_far_address(ExcursionHalf_us); /* Get 32 bits adress */
    ExcursionHalf_us_Far_Adress += (2 * t->Nibble.CurIdx); /* Compute offset */
    int16_t valueTemp = (int16_t)pgm_read_word_far(ExcursionHalf_us_Far_Adress);
    cli();
    channelOutputs[g_model.Xany[XanyIdx].ChId] = valueTemp; /* overwrite in int-level */
    sei();
    t->Nibble.SentCnt++;
    if(t->Nibble.SentCnt >= (g_model.Xany[XanyIdx].RepeatNb + 1))
    {
      t->NibbleIdx++;
      t->Nibble.SentCnt = 0;
      t->Nibble.TxInProgress = 0;
    }
  }
}

#define COMPUTE_X_ANY_MSG_CHECKSUM(Msg, BytePtr, Checksum) do{                                    \
    Msg->Checksum = 0;/* Clear Checksum which can share a nibble with previous byte */            \
    for(uint8_t ByteIdx = 0; ByteIdx < ((Msg->NibbleNbToTx + 1) / 2); ByteIdx++)                  \
    {                                                                                             \
      Checksum ^= BytePtr[ByteIdx];                                                               \
    }                                                                                             \
    Checksum ^= 0x55;                                                                             \
    Msg->Checksum = Checksum;                                                                     \
  }while(0)

/**
* \file   Xany.cpp
* \fn     void Xany_operation(uint8_t XanyIdx, uint8_t XanyOp, XanyInfoSt_t *XanyInfo)
* \brief  Reads the I2C I/O expenders and sensors or information associated to the specified X-Any instance
* \param  XanyIdx:  Index of the X-Any instance
* \param  XanyOp:   The X-Any operation
* \param  XanyInfo: Pointer on an X-Any information structure to fill
* \return 0: Message non supporte, 1: Message supporte
*/
uint8_t Xany_operation(uint8_t XanyIdx, uint8_t XanyOp, XanyInfoSt_t *XanyInfo)
{
  Msg4SwSt_t      *Msg4Sw;
  Msg8SwSt_t      *Msg8Sw;
  Msg16SwSt_t     *Msg16Sw;
  MsgAngleSt_t    *MsgAngle;
  MsgAngle4SwSt_t *MsgAngle4Sw;
  MsgAngle8SwSt_t *MsgAngle8Sw;
  MsgAnglePotSt_t *MsgAnglePot;
  MsgPot4SwSt_t   *MsgPot4Sw;
  MsgPot8SwSt_t   *MsgPot8Sw;
  MsgCommonSt_t   *MsgCommon;
  uint8_t          MsgType, One8bitPort = 0, Checksum, *BytePtr, ValidMsg = 1;
  uint16_t         Two8bitPorts;

  Checksum = 0;
  BytePtr = (uint8_t *)&X_AnyReadMsg[XanyIdx];

  MsgType = getMsgType(XanyIdx);

  switch(MsgType)
  {
    case XANY_MSG_4SW:
    Msg4Sw = (Msg4SwSt_t *)&X_AnyReadMsg[XanyIdx];
    Msg4Sw->NibbleNbToTx = XANY_MSG_4SW_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
      Msg4Sw->Sw = One8bitPort & 0x0F; /* Keep 4 bits */
      /* Update Checksum */
      COMPUTE_X_ANY_MSG_CHECKSUM(Msg4Sw, BytePtr, Checksum);
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Msg4Sw->NibbleNbToTx;
      XanyInfo->SwNb         = 4;
      XanyInfo->SwValue      = (uint16_t)Msg4Sw->Sw;
      XanyInfo->Angle        = 0;
      XanyInfo->RotPotValue  = 0;
    }
    break;

    case XANY_MSG_8SW:
    Msg8Sw = (Msg8SwSt_t *)&X_AnyReadMsg[XanyIdx];
    Msg8Sw->NibbleNbToTx = XANY_MSG_8SW_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
      Msg8Sw->Sw = One8bitPort;
      /* Update Checksum */
      COMPUTE_X_ANY_MSG_CHECKSUM(Msg8Sw, BytePtr, Checksum);
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Msg8Sw->NibbleNbToTx;
      XanyInfo->SwNb         = 8;
      XanyInfo->SwValue      = (uint16_t)Msg8Sw->Sw;
      XanyInfo->Angle        = 0;
      XanyInfo->RotPotValue  = 0;
    }
    break;

    case XANY_MSG_16_SW:
    Msg16Sw = (Msg16SwSt_t *)&X_AnyReadMsg[XanyIdx];
    Msg16Sw->NibbleNbToTx = XANY_MSG_16_SW_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&Two8bitPorts, 2);
      Msg16Sw->Sw = Two8bitPorts;
      /* Update Checksum */
      COMPUTE_X_ANY_MSG_CHECKSUM(Msg16Sw, BytePtr, Checksum);
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Msg16Sw->NibbleNbToTx;
      XanyInfo->SwNb         = 16;
      XanyInfo->SwValue      = (uint16_t)Msg16Sw->Sw;
      XanyInfo->Angle        = 0;
      XanyInfo->RotPotValue  = 0;
    }
    break;

    case XANY_MSG_ANGLE:
    MsgAngle = (MsgAngleSt_t *)&X_AnyReadMsg[XanyIdx];
    MsgAngle->NibbleNbToTx = XANY_MSG_ANGLE_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      MsgAngle->Angle = 0; /* TODO: read ADS1015 CAN Channel */
      /* Update Checksum */
      COMPUTE_X_ANY_MSG_CHECKSUM(MsgAngle, BytePtr, Checksum);
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = MsgAngle->NibbleNbToTx;
      XanyInfo->SwNb         = 0;
      XanyInfo->SwValue      = 0;
      XanyInfo->Angle        = MsgAngle->Angle;
      XanyInfo->RotPotValue  = 0;
    }
    break;

    case XANY_MSG_ANGLE_4SW:
    MsgAngle4Sw = (MsgAngle4SwSt_t *)&X_AnyReadMsg[XanyIdx];
    MsgAngle4Sw->NibbleNbToTx = XANY_MSG_ANGLE_4SW_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
      MsgAngle4Sw->Sw    = One8bitPort & 0x0F; /* Keep 4 bits */
      MsgAngle4Sw->Angle = 0; /* TODO: read ADS1015 CAN Channel */
      /* Update Checksum */
      COMPUTE_X_ANY_MSG_CHECKSUM(MsgAngle4Sw, BytePtr, Checksum);
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = MsgAngle4Sw->NibbleNbToTx;
      XanyInfo->SwNb         = 4;
      XanyInfo->SwValue      = (uint16_t)MsgAngle4Sw->Sw;
      XanyInfo->Angle        = MsgAngle4Sw->Angle;
      XanyInfo->RotPotValue  = 0;
    }
    break;

    case XANY_MSG_ANGLE_8SW:
    MsgAngle8Sw = (MsgAngle8SwSt_t *)&X_AnyReadMsg[XanyIdx];
    MsgAngle8Sw->NibbleNbToTx = XANY_MSG_ANGLE_8SW_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
      MsgAngle8Sw->Sw    = One8bitPort;
      MsgAngle8Sw->Angle = 0; /* TODO: read ADS1015 CAN Channel */
      /* Update Checksum */
      COMPUTE_X_ANY_MSG_CHECKSUM(MsgAngle8Sw, BytePtr, Checksum);
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = MsgAngle8Sw->NibbleNbToTx;
      XanyInfo->SwNb         = 8;
      XanyInfo->SwValue      = (uint16_t)MsgAngle8Sw->Sw;
      XanyInfo->Angle        = MsgAngle8Sw->Angle;
      XanyInfo->RotPotValue  = 0;
    }
    break;

    case XANY_MSG_ANGLE_POT:
    MsgAnglePot = (MsgAnglePotSt_t *)&X_AnyReadMsg[XanyIdx];
    MsgAnglePot->NibbleNbToTx = XANY_MSG_ANGLE_POT_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      MsgAnglePot->Angle = 0;              /* TODO: read ADS1015 CAN Channel */
      MsgAnglePot->Pot   = ((calibratedStick[GET_XANY_POT(XanyIdx)] + RESX-1) /8); /* 8 bits value */
      /* Update Checksum */
      COMPUTE_X_ANY_MSG_CHECKSUM(MsgAnglePot, BytePtr, Checksum);
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = MsgAnglePot->NibbleNbToTx;
      XanyInfo->SwNb         = 0;
      XanyInfo->SwValue      = 0;
      XanyInfo->Angle        = MsgAnglePot->Angle;
      XanyInfo->RotPotValue  = 0;
    }
    break;

    case XANY_MSG_POT_4SW:
    MsgPot4Sw = (MsgPot4SwSt_t *)&X_AnyReadMsg[XanyIdx];
    MsgPot4Sw->NibbleNbToTx = XANY_MSG_POT_4SW_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
      MsgPot4Sw->Sw    = One8bitPort & 0x0F; /* Keep 4 bits */
      MsgPot4Sw->Pot   = ((calibratedStick[GET_XANY_POT(XanyIdx)] + RESX-1) /8); /* 8 bits value */
      /* Update Checksum */
      COMPUTE_X_ANY_MSG_CHECKSUM(MsgPot4Sw, BytePtr, Checksum);
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = MsgPot4Sw->NibbleNbToTx;
      XanyInfo->SwNb         = 4;
      XanyInfo->SwValue      = MsgPot4Sw->Sw;
      XanyInfo->Angle        = 0;
      XanyInfo->RotPotValue  = MsgPot4Sw->Pot;
    }
    break;

    case XANY_MSG_POT_8SW:
    MsgPot8Sw = (MsgPot8SwSt_t *)&X_AnyReadMsg[XanyIdx];
    MsgPot8Sw->NibbleNbToTx = XANY_MSG_POT_8SW_NBL_NB;
    if (XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
      MsgPot8Sw->Sw    = One8bitPort;
      MsgPot8Sw->Pot   = ((calibratedStick[GET_XANY_POT(XanyIdx)] + RESX-1) /8); /* 8 bits value */
      /* Update Checksum */
      COMPUTE_X_ANY_MSG_CHECKSUM(MsgPot8Sw, BytePtr, Checksum);
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = MsgPot8Sw->NibbleNbToTx;
      XanyInfo->SwNb         = 8;
      XanyInfo->SwValue      = MsgPot8Sw->Sw;
      XanyInfo->Angle        = 0;
      XanyInfo->RotPotValue  = MsgPot8Sw->Pot;
    }
    break;

    default: /* We arrive here if the composed X-Any message from the LCD menu is not supported */
    ValidMsg = 0; /* Invalid or not yet supported message */
    MsgCommon = (MsgCommonSt_t *)&X_AnyReadMsg[XanyIdx];
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      MsgCommon->PayloadAndChecksum = 0; /* Detroy the checksum to be sure the message won't be interpreted at receiver side (1st stage) */
      MsgCommon->NibbleNbToTx       = 0; /* Prevent transmission (2nd stage) */
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = 0;
      XanyInfo->SwNb         = 0;
      XanyInfo->SwValue      = 0;
      XanyInfo->Angle        = 0;
      XanyInfo->RotPotValue  = 0;
    }
    break;
  }
  /* Can be common */
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
    if(IoExpMap & (1 << BitIdx))
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
        if(!ReadIoExp((I2c7bAddr << 1), (uint8_t*)&One8bitPort)) One8bitPort ^= 0xFF; /* Apply polarity: close contact = 1 */
        if(BitIdx & 1) BytePtr[1] = One8bitPort; /* Odd  */
        else           BytePtr[0] = One8bitPort; /* Even */
        ByteRead++;
      }
      else
      {
        /* Read two 8 bit ports */
        if(!ReadIoExp((I2c7bAddr << 1), (uint8_t*)&Two8bitPorts)) Two8bitPorts ^= 0xFFFF; /* Apply polarity: close contact = 1 */
        *WordPtr = Two8bitPorts;
        ByteRead += 2;
      }
      if(ByteRead >= ByteToRead) break;
    }
  }
  I2C_SPEED_888K();

 return(ByteRead);
}
