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

#define NEUTRAL_WIDTH_US                    1500
#define NIBBLE_WIDTH_US                     56
#define FULL_EXCURSION_US                   (NIBBLE_WIDTH_US * NIBBLE_NB)
#define PULSE_MIN_US                        (NEUTRAL_WIDTH_US - (FULL_EXCURSION_US / 2))
#define PULSE_WIDTH_US(NibbleIdx)           (PULSE_MIN_US + (NIBBLE_WIDTH_US / 2)+ ((NibbleIdx) * NIBBLE_WIDTH_US))
#define EXCURSION_HALF_US(NibbleIdx)        ((PULSE_WIDTH_US(NibbleIdx) - NEUTRAL_WIDTH_US) * 2)

#define GET_EXCURSION_HALF_US(NibbleIdx)    (int16_t)pgm_read_word_near(&ExcursionHalf_us[(NibbleIdx)])

#define EIGHT_BIT_PER_CENT(Value, PerCent)  (((Value) * (uint16_t)(PerCent)) / 100U)

#define DEAD_ANGLE_RADIUS_PER_CENT          5
#define DEAD_THROTTLE_RADIUS_PER_CENT       15

#define DEAD_ANGLE_RADIUS                   EIGHT_BIT_PER_CENT(255, DEAD_ANGLE_RADIUS_PER_CENT)
#define DEAD_THROTTLE_RADIUS                EIGHT_BIT_PER_CENT(255, DEAD_THROTTLE_RADIUS_PER_CENT)

const int16_t ExcursionHalf_us[] PROGMEM = {
        EXCURSION_HALF_US(NIBBLE_0), EXCURSION_HALF_US(NIBBLE_1),
        EXCURSION_HALF_US(NIBBLE_2), EXCURSION_HALF_US(NIBBLE_3),
        EXCURSION_HALF_US(NIBBLE_4), EXCURSION_HALF_US(NIBBLE_5),
        EXCURSION_HALF_US(NIBBLE_6), EXCURSION_HALF_US(NIBBLE_7),
        EXCURSION_HALF_US(NIBBLE_8), EXCURSION_HALF_US(NIBBLE_9),
        EXCURSION_HALF_US(NIBBLE_A), EXCURSION_HALF_US(NIBBLE_B),
        EXCURSION_HALF_US(NIBBLE_C), EXCURSION_HALF_US(NIBBLE_D),
        EXCURSION_HALF_US(NIBBLE_E), EXCURSION_HALF_US(NIBBLE_F),
        EXCURSION_HALF_US(NIBBLE_R), EXCURSION_HALF_US(NIBBLE_I)
        };

const uint8_t Atan45Tbl[] PROGMEM =  {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
        1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
        5, 5, 6, 6, 6, 7, 7, 8, 8, 8, 9, 9, 10, 10, 10, 11,
        11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 17, 17, 18, 18, 19, 19,
        20, 21, 21, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28, 29, 29, 30,
        31, 32, 32, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 41, 42, 43,
        44, 45, 46, 47, 48, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
        59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 73, 74, 75,
        76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 87, 88, 89, 91, 92, 93,
        94, 95, 97, 98, 99, 100, 102, 103, 104, 105, 107, 108, 109, 110, 112, 113,
        114, 115, 117, 118, 119, 121, 122, 123, 125, 126, 127, 129, 130, 131, 133, 134,
        135, 137, 138, 139, 141, 142, 144, 145, 146, 148, 149, 151, 152, 153, 155, 156,
        158, 159, 161, 162, 164, 165, 166, 168, 169, 171, 172, 174, 175, 177, 178, 180,
        181, 183, 184, 186, 187, 189, 190, 192, 193, 195, 196, 198, 199, 201, 202, 204,
        206, 207, 209, 210, 212, 213, 215, 216, 218, 220, 221, 223, 224, 226, 228, 229,
        231, 232, 234, 236, 237, 239, 240, 242, 244, 245, 247, 248, 250, 252, 253, 255
        };

#define XANY_PAYLOAD_MAX_SIZE_BITS     (12 + 8 + 16) /* Max X-Any payload in bits: Angle + Pot + 16 Sw */
#define XANY_PAYLOAD_MAX_SIZE_NIBBLES  ((XANY_PAYLOAD_MAX_SIZE_BITS + 3) / 4)
#define XANY_PAYLOAD_MAX_SIZE_BYTES    ((XANY_PAYLOAD_MAX_SIZE_NIBBLES + 1) / 2)

/*
 Xany Message format:
 ===================
 1) Xany Fields are nibble-aligned (4 bits) to optimize transmission time :-)
 2) Fields are always filled in the following order:            [Angle][Prop][Sw]
 3) The Xany message is build dynamically. If a field is not present the other one(s) is/are left justified
    Example#1: If Angle is not present,          the message is [Prop][Sw]
    Example#2: If Prop  is not present,          the message is [Angle][Sw]
    Example#3: If Angle & Prop  are not present, the message is [Sw]
 4) The 8 bit checksum is added just after the last Nibble. Ex: [Prop][Sw][Chks]
*/

typedef struct{
  uint8_t NibbleNbToTx;
  uint8_t PayloadAndChecksum[XANY_PAYLOAD_MAX_SIZE_BYTES + 1]; /* + 1 for Checksum */
}MsgCommonSt_t; /* Size = 4 bytes */

typedef union{
  MsgCommonSt_t    Common;
  uint8_t          Raw[1 + XANY_PAYLOAD_MAX_SIZE_BYTES + 1]; /* + 1 for NibbleNbToTx and + 1 for Checksum */
}XanyMsg_union;  /* Size = 4 bytes */

typedef struct{
  uint16_t
        AngleNblNb:4, /* 3 max (12 Bits Max) */
        PropNblNb :4, /* 2 max (8  Bits Max) */
        SwNblNb   :4, /* 4 max (16 Bits Max) */
        NblNb     :4; /* Max nibble nb is currently 9 */
}PayloadMapSt_t;

/* Allegro A1335 absolute angle sensor 7 bit I2C address */
#define A1335_I2C_7B_ADDR             (0x0C) /* /!\ Even if the chip is powered under 5V, I2C SDA/SCL pull-ups SHALL be tied at 3.3V /!\ */

/*    <------------------8 I/O Expenders------------------><----------------16 I/O Expenders--------------->    */
enum {IO_EXP_PCF8574 = 0, IO_EXP_PCF8574A, IO_EXP_PCA9654E, IO_EXP_MCP23017, IO_EXP_PCF8575A, IO_EXP_PCA9671, IO_EXP_TYPE_NB};

/* PRIVATE FUNCTION PROTOYPES */
static void     cfg2PayloadMap(XanyPayloadCfgSt_t *PayloadCfg, PayloadMapSt_t *PayloadMap);
static void     setPayloadAngle(XanyMsg_union *XanyPl, uint16_t Angle, PayloadMapSt_t *PayloadMap);
static void     setPayloadProp(XanyMsg_union *XanyPl, uint8_t Prop, PayloadMapSt_t *PayloadMap);
static void     setPayloadSw(XanyMsg_union *XanyPl, uint16_t Sw, PayloadMapSt_t *PayloadMap);
static uint16_t getPayloadAngle(XanyMsg_union *XanyPl, PayloadMapSt_t *PayloadMap);
static uint8_t  getPayloadProp(XanyMsg_union *XanyPl, PayloadMapSt_t *PayloadMap);
static uint16_t getPayloadSw(XanyMsg_union *XanyPl, PayloadMapSt_t *PayloadMap);
static uint8_t  readIoExtender (uint8_t XanyIdx, uint8_t *RxBuf, uint8_t ByteToRead);
static uint8_t  readI2cAngleSensor(uint8_t XanyIdx, uint16_t *Angle);
static uint16_t sinCosToGis(int16_t Sinus, int16_t Cosinus);
static uint8_t  throttle(int16_t ExcSinus, int16_t ExcCosinus, uint8_t ExpoLevel);
static uint8_t  tinySqrt(uint16_t Square); /* Max exec time: 4 to 60us */
static uint8_t  tinyExpo(uint8_t InValue, uint8_t ExpoPerCentIdx);
static void     updateXanyMsgChecksum(XanyMsg_union *XanyMsg);
static int32_t  map32(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);

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
#define POINTS_FOR_45_DEG             (uint32_t)sizeof(Atan45Tbl) /* 32 bits to avoid truncation with macro */
#define UNIT_PER_DEG(Deg)             (((Deg) * POINTS_FOR_45_DEG) / 45)
#define ATAN_45_BY_IDX(Idx)           (((Idx) < POINTS_FOR_45_DEG)? (pgm_read_byte_near(&Atan45Tbl[(Idx)])): POINTS_FOR_45_DEG)

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
  char                  TxChar;
  uint8_t               NibbleNbToTx;

  if(g_model.Xany[XanyIdx].Active)
  {
    t = (X_OneAnyWriteMsgSt_t *)&X_AnyWriteMsg[XanyIdx]; /* XanyIdx SHALL be < NUM_X_ANY */
    NibbleNbToTx = t->Msg.Common.NibbleNbToTx;
    if(!t->Nibble.TxInProgress)
    {
      t->Nibble.TxInProgress = 1;
      /* Get next char to send */
      if(t->NibbleIdx < NibbleNbToTx)
      {
        TxChar = t->Msg.Common.PayloadAndChecksum[t->NibbleIdx / 2];
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
    channelOutputs[g_model.Xany[XanyIdx].ChId] = GET_EXCURSION_HALF_US(t->Nibble.CurIdx); /* overwrite (we already are in int-level). Use progmem "near" for speed */
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
        memcpy((void*)t->Msg.Raw, (void*)X_AnyReadMsg[XanyIdx].Raw, 1 + XANY_PAYLOAD_MAX_SIZE_BYTES + 1); /* + 1 for NibbleNbToTx and + 1 for Checksum */
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
  XanyPayloadCfgSt_t PayloadCfg;
  PayloadMapSt_t     PayloadMap;
  XanyMsg_union      Built, Read;
  uint8_t            One8bitPort = 0, Prop = 0, ExpoPerCentIdx = 0, Radius, ValidMsg;
  uint16_t           Two8bitPorts = 0, Angle = 0, Sw = 0;
  int16_t            ExcSin, ExcCos;

  PayloadCfg = g_model.Xany[XanyIdx].PayloadCfg;
  cfg2PayloadMap(&PayloadCfg, &PayloadMap); /* Convert PayloadCfg to Payload Map */
  ValidMsg = !!PayloadMap.NblNb;
  if(XanyOp & XANY_OP_BUILD_MSG)
  {
    memset((void*)Built.Common.PayloadAndChecksum, 0, XANY_PAYLOAD_MAX_SIZE_BYTES + 1); /* + 1 for Checksum */
    Built.Common.NibbleNbToTx = PayloadMap.NblNb + 2; /* + 2 for Checksum */

    if(PayloadMap.AngleNblNb)
    {
      switch(PayloadCfg.AngleSrcIdx)
      {
        case X_ANY_ANGLE_SRC_I2C_SENSOR:
        readI2cAngleSensor(XanyIdx, &Angle);
        break;

        case X_ANY_ANGLE_SRC_LEFT_CROSS_STICK:
        ExcSin = (calibratedStick[CONVERT_MODE(ELE_STICK)] / 2);
        ExcCos = (calibratedStick[CONVERT_MODE(RUD_STICK)] / 2);
        Radius = throttle(ExcSin / 2, ExcCos / 2, 0);
        if(Radius >= DEAD_ANGLE_RADIUS)
        {
          Angle = sinCosToGis(ExcSin, ExcCos);
        }
        else
        {
          memcpy((void*)Read.Raw, (void*)X_AnyReadMsg[XanyIdx].Raw, XANY_PAYLOAD_MAX_SIZE_BYTES + 1); /* + 1 for Checksum */
          Angle = getPayloadAngle(&Read, &PayloadMap); /* Keep current angle */
        }
        break;

        case X_ANY_ANGLE_SRC_RIGHT_CROSS_STICK:
        ExcSin = (calibratedStick[CONVERT_MODE(THR_STICK)] / 2);
        ExcCos = (calibratedStick[CONVERT_MODE(AIL_STICK)] / 2);
        Radius = throttle(ExcSin / 2, ExcCos / 2, 0);
        if(Radius >= DEAD_ANGLE_RADIUS)
        {
          Angle = sinCosToGis(ExcSin, ExcCos);
        }
        else
        {
          memcpy((void*)Read.Common.PayloadAndChecksum, (void*)X_AnyReadMsg[XanyIdx].Common.PayloadAndChecksum, XANY_PAYLOAD_MAX_SIZE_BYTES + 1); /* + 1 for Checksum */
          Angle = getPayloadAngle(&Read, &PayloadMap); /* Keep current angle */
        }
        break;

        default:
        break;
      }
      setPayloadAngle(&Built, Angle, &PayloadMap);
    }

    if(PayloadMap.PropNblNb)
    {
      if(g_model.Xany[XanyIdx].PayloadCfg.PropSrcIdx < X_ANY_PROP_SRC_LEFT_CROSS_STICK)
      {
        Prop = ((calibratedStick[CONVERT_MODE(g_model.Xany[XanyIdx].PayloadCfg.PropSrcIdx - 1)] + RESX - 1) / 8);
      }
      else
      {
        if(g_model.Xany[XanyIdx].PayloadCfg.PropSrcIdx == X_ANY_PROP_SRC_LEFT_CROSS_STICK)
        {
          ExcSin = (calibratedStick[CONVERT_MODE(ELE_STICK)] / 2);
          ExcCos = (calibratedStick[CONVERT_MODE(RUD_STICK)] / 2);
          /* Here, ExcSin and ExcCos are between -512 and +511 (-> better angle accuracy) */
          Radius = throttle(ExcSin / 2, ExcCos / 2, 0); /* throttle() expects arguments between -255 to 255 */
          if(Radius >= DEAD_THROTTLE_RADIUS)
          {
            Prop = map32(Radius, DEAD_THROTTLE_RADIUS, 255, 0, 255);
          }
        }
        else
        {
          ExcSin = (calibratedStick[CONVERT_MODE(THR_STICK)] / 2);
          ExcCos = (calibratedStick[CONVERT_MODE(AIL_STICK)] / 2);
          /* Here, ExcSin and ExcCos are between -512 and +511 (-> better angle accuracy) */
          Radius = throttle(ExcSin / 2, ExcCos / 2, 0); /* throttle() expects arguments between -255 to 255 */
          if(Radius >= DEAD_THROTTLE_RADIUS)
          {
            Prop = map32(Radius, DEAD_THROTTLE_RADIUS, 255, 0, 255);
          }
        }
      }
      ExpoPerCentIdx = g_model.Xany[XanyIdx].Param.Expo.PerCentIdx;
      Prop = tinyExpo(Prop, ExpoPerCentIdx);
      setPayloadProp(&Built, Prop, &PayloadMap);
    }

    if(PayloadMap.SwNblNb)
    {
      switch(PayloadCfg.SwitchSrcIdx)
      {
        case X_ANY_SW_SRC_I2C_SW4:
        readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
        Sw = (uint16_t)(One8bitPort & 0x0F); /* Keep 4 bits */
        break;

        case X_ANY_SW_SRC_I2C_SW8:
        readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
        Sw = (uint16_t)One8bitPort;
        break;

        case X_ANY_SW_SRC_I2C_SW16:
        readIoExtender(XanyIdx, (uint8_t *)&Two8bitPorts, 2);
        Sw = Two8bitPorts;
        break;

        default:
        break;
      }
      setPayloadSw(&Built, Sw, &PayloadMap);
    }
    /* Update Checksum */
    updateXanyMsgChecksum((XanyMsg_union *)&Built);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    /* Load the temporary built message in X_AnyReadMsg */
    memcpy((void*)X_AnyReadMsg[XanyIdx].Raw, (void*)Built.Raw, 1 + XANY_PAYLOAD_MAX_SIZE_BYTES + 1); /* + 1 for NblToTx and +1 for Checksum */
    }
  }
  else
  {
    /* XANY_OP_READ_INFO */
    memcpy((void*)Read.Common.PayloadAndChecksum, (void*)X_AnyReadMsg[XanyIdx].Common.PayloadAndChecksum, XANY_PAYLOAD_MAX_SIZE_BYTES + 1);
    XanyInfo->MsgNibbleLen = PayloadMap.NblNb + 2; /* + 2 for Checksum */
    XanyInfo->SwNb         = PayloadMap.SwNblNb << 2;
    XanyInfo->AngleValue   = getPayloadAngle(&Read, &PayloadMap);
    XanyInfo->PropValue    = getPayloadProp(&Read, &PayloadMap);
    XanyInfo->SwValue      = getPayloadSw(&Read, &PayloadMap);
    XanyInfo->TxPeriodMs = ((g_model.Xany[XanyIdx].RepeatNb + 1) * (XanyInfo->MsgNibbleLen + 1) * 225) / 10; /* + 1 for IDLE symbol */
  }
  return(ValidMsg);
}

/* PRIVATE FUNCTIONS */
/**
* \file   Xany.cpp
* \fn     void cfg2PayloadMap(XanyPayloadCfgSt_t *PayloadCfg, PayloadMapSt_t *PayloadMap)
* \brief  Returns the PayloadMap structure filled from the PayloadCfg structure content
* \param  XanyIdx: Index of the X-Any
* \return Void
*/
static void cfg2PayloadMap(XanyPayloadCfgSt_t *PayloadCfg, PayloadMapSt_t *PayloadMap)
{
  PayloadMap->AngleNblNb = PayloadCfg->AngleSrcIdx? 3: 0; /* Always 12 bits */
  PayloadMap->PropNblNb = PayloadCfg->PropSrcIdx? 2: 0;   /* Always 8 bits */
  PayloadMap->SwNblNb = (PayloadCfg->SwitchSrcIdx < X_ANY_SW_SRC_I2C_SW16)? (PayloadCfg->SwitchSrcIdx): 4;
  PayloadMap->NblNb = PayloadMap->AngleNblNb + PayloadMap->PropNblNb + PayloadMap->SwNblNb;
  if(PayloadMap->NblNb > XANY_PAYLOAD_MAX_SIZE_NIBBLES)
  {
    PayloadMap->NblNb = 0; /* Too long! */
  }
}

/**
* \file   Xany.cpp
* \fn     void setPayloadAngle(XanyMsg_union *XanyPl, uint16_t Angle, PayloadMapSt_t *PayloadMap)
* \brief  Sets the Angle value in the payload structure
* \param  XanyPl:     Pointer on the payload structure
* \param  Angle:      The angle value to load
* \param  PayloadMap: Pointer on the payload map
* \return Void
*/
static void setPayloadAngle(XanyMsg_union *XanyPl, uint16_t Angle, PayloadMapSt_t *PayloadMap)
{
  PayloadMap = PayloadMap; /* To avoid a compilation warning */
  XanyPl->Common.PayloadAndChecksum[0] = (Angle & 0x0FF0) >> 4;
  XanyPl->Common.PayloadAndChecksum[1] = (Angle & 0x000F) << 4;
}

/**
* \file   Xany.cpp
* \fn     void setPayloadProp(XanyMsg_union *XanyPl, uint8_t Prop, PayloadMapSt_t *PayloadMap)
* \brief  Sets the Prop value in the payload structure
* \param  XanyPl:     Pointer on the payload structure
* \param  Prop:       The Prop value to load
* \param  PayloadMap: Pointer on the payload map
* \return Void
*/
static void setPayloadProp(XanyMsg_union *XanyPl, uint8_t Prop, PayloadMapSt_t *PayloadMap)
{
  if(!PayloadMap->AngleNblNb)
  {
    /* Prop is the first field */
    XanyPl->Common.PayloadAndChecksum[0] = Prop;
  }
  else
  {
    /* Prop is after the Angle field */
    XanyPl->Common.PayloadAndChecksum[1] |= (Prop & 0xF0) >> 4;
    XanyPl->Common.PayloadAndChecksum[2]  = (Prop & 0x0F) << 4;
  }
}

/**
* \file   Xany.cpp
* \fn     void setPayloadSw(XanyMsg_union *XanyPl, uint16_t Sw, PayloadMapSt_t *PayloadMap)
* \brief  Sets the Switch value in the payload structure
* \param  XanyPl:     Pointer on the payload structure
* \param  Sw:         The Switch value to load
* \param  PayloadMap: Pointer on the payload map
* \return Void
*/
static void setPayloadSw(XanyMsg_union *XanyPl, uint16_t Sw, PayloadMapSt_t *PayloadMap)
{
  if(PayloadMap->SwNblNb < 4)
  {
    /* Sw.4 and Sw.8 */
    if(PayloadMap->AngleNblNb)
    {
      if(PayloadMap->SwNblNb == 1)
      {
        XanyPl->Common.PayloadAndChecksum[1 + !!PayloadMap->PropNblNb] |= Sw & 0x0F;
      }
      else
      {
        XanyPl->Common.PayloadAndChecksum[1 + !!PayloadMap->PropNblNb] |= (Sw & 0xF0) >> 4;
        XanyPl->Common.PayloadAndChecksum[2 + !!PayloadMap->PropNblNb] |= (Sw & 0x0F) << 4;
      }
    }
    else
    {
      if(PayloadMap->SwNblNb == 1)
      {
        XanyPl->Common.PayloadAndChecksum[0 + !!PayloadMap->PropNblNb] |= (Sw & 0x000F) << 4;
      }
      else
      {
        XanyPl->Common.PayloadAndChecksum[0 + !!PayloadMap->PropNblNb] |= (Sw & 0x00FF);
      }
    }
  }
  else
  {
    /* Sw.16 */
    if(PayloadMap->AngleNblNb)
    {
      XanyPl->Common.PayloadAndChecksum[1 + !!PayloadMap->PropNblNb] |= (Sw & 0xF000) >> 12;
      XanyPl->Common.PayloadAndChecksum[2 + !!PayloadMap->PropNblNb] |= (Sw & 0x0FF0) >> 4;
      XanyPl->Common.PayloadAndChecksum[3 + !!PayloadMap->PropNblNb] |= (Sw & 0x000F) << 4;
    }
    else
    {
      XanyPl->Common.PayloadAndChecksum[0 + !!PayloadMap->PropNblNb] = (Sw & 0xFF00) >> 8;
      XanyPl->Common.PayloadAndChecksum[1 + !!PayloadMap->PropNblNb] = (Sw & 0x00FF);
    }
  }
}

/**
* \file   Xany.cpp
* \fn     uint16_t getPayloadAngle(XanyMsg_union *XanyPl, PayloadMapSt_t *PayloadMap)
* \brief  Gets the Angle value from the payload structure
* \param  XanyPl:     Pointer on the payload structure
* \param  PayloadMap: Pointer on the payload map
* \return The Angle value
*/
static uint16_t getPayloadAngle(XanyMsg_union *XanyPl, PayloadMapSt_t *PayloadMap)
{
  PayloadMap = PayloadMap; /* To avoid a compilation warning */
  return((((XanyPl->Common.PayloadAndChecksum[0] << 8) | (XanyPl->Common.PayloadAndChecksum[1] & 0xF0)) >> 4));
}

/**
* \file   Xany.cpp
* \fn     uint8_t getPayloadProp(XanyMsg_union *XanyPl, PayloadMapSt_t *PayloadMap)
* \brief  Gets the Prop value from the payload structure
* \param  XanyPl:     Pointer on the payload structure
* \param  PayloadMap: Pointer on the payload map
* \return The Prop value
*/
static uint8_t getPayloadProp(XanyMsg_union *XanyPl, PayloadMapSt_t *PayloadMap)
{
  uint8_t Prop;

  if(!PayloadMap->AngleNblNb)
  {
    /* Prop is the first field */
    Prop = XanyPl->Common.PayloadAndChecksum[0];
  }
  else
  {
    /* Prop is after the Angle field */
    Prop  = (XanyPl->Common.PayloadAndChecksum[1] & 0x0F) << 4;
    Prop |= (XanyPl->Common.PayloadAndChecksum[2] & 0xF0) >> 4;
  }

  return(Prop);
}

/**
* \file   Xany.cpp
* \fn     uint16_t getPayloadSw(XanyMsg_union *XanyPl, PayloadMapSt_t *PayloadMap)
* \brief  Gets the Switch value from the payload structure
* \param  XanyPl:     Pointer on the payload structure
* \param  PayloadMap: Pointer on the payload map
* \return The Switch value
*/
static uint16_t getPayloadSw(XanyMsg_union *XanyPl, PayloadMapSt_t *PayloadMap)
{
  uint16_t Sw = 0;

  if(PayloadMap->SwNblNb < 4)
  {
    /* Sw.4 and Sw.8 */
    if(PayloadMap->AngleNblNb)
    {
      if(PayloadMap->SwNblNb == 1)
      {
        Sw |= XanyPl->Common.PayloadAndChecksum[1 + !!PayloadMap->PropNblNb] & 0x0F;
      }
      else
      {
        Sw |= (XanyPl->Common.PayloadAndChecksum[1 + !!PayloadMap->PropNblNb] & 0x0F) << 4;
        Sw |= (XanyPl->Common.PayloadAndChecksum[2 + !!PayloadMap->PropNblNb] & 0xF0) >> 4;
      }
    }
    else
    {
      if(PayloadMap->SwNblNb == 1)
      {
        Sw |= (XanyPl->Common.PayloadAndChecksum[0 + !!PayloadMap->PropNblNb] & 0xF0) >> 4;
      }
      else
      {
        Sw |= XanyPl->Common.PayloadAndChecksum[0 + !!PayloadMap->PropNblNb];
      }
    }
  }
  else
  {
    /* Sw.16 */
    if(PayloadMap->AngleNblNb)
    {
      Sw |= (XanyPl->Common.PayloadAndChecksum[1 + !!PayloadMap->PropNblNb] & 0x0F) << 12;
      Sw |= (XanyPl->Common.PayloadAndChecksum[2 + !!PayloadMap->PropNblNb] << 4);
      Sw |= (XanyPl->Common.PayloadAndChecksum[3 + !!PayloadMap->PropNblNb] & 0xF0) >> 4;
    }
    else
    {
      Sw |= XanyPl->Common.PayloadAndChecksum[0 + !!PayloadMap->PropNblNb] << 8;
      Sw |= XanyPl->Common.PayloadAndChecksum[1 + !!PayloadMap->PropNblNb];
    }
  }
  return(Sw);
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
* \fn     uint8_t readI2cAngleSensor(uint8_t XanyIdx, uint16_t *Angle)
* \brief  Reads the I2C angle sensor associated to the specified X-Any instance
* \param  XanyIdx:    Index of the X-Any
* \param  Angle:      Pointer on the destination 16 bits buffer
* \return 0: OK,   1: Error
*/
static uint8_t readI2cAngleSensor(uint8_t XanyIdx, uint16_t *Angle)
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
* \fn     uint16_t sinCosToGis(int16_t Sinus, int16_t Cosinus)
* \brief  Computes the gisement angle from sinus and cosinus values
* \param  Sinus:    Sinus value
* \param  Cosinus:  Sinus value
* \return The gisement angle (from 0 to 2047)
*/
static uint16_t sinCosToGis(int16_t Sinus, int16_t Cosinus) /* Max exec time: 44us */
{
  int16_t   AbsSinus, AbsCosinus;
  uint32_t  AtanIdx;
  uint16_t  Gis;
  uint16_t  Ret;

  if(!Sinus && !Cosinus) return(0); /* To avoid div by 0 */
  /* First, compute 0° < Atan2(Sinus,Cosinus) < 90° */
  AbsSinus = Sinus;
  AbsCosinus = Cosinus;
  if(Sinus   < 0) AbsSinus   = -Sinus;
  if(Cosinus < 0) AbsCosinus = -Cosinus;

  if(AbsCosinus > AbsSinus)
  {
    /* Use Tangent */
    AtanIdx = POINTS_FOR_45_DEG * (uint16_t)AbsSinus;
    /* If both variables are even: divide by 2 to reduce the last division duration */
    while(!(AtanIdx & 1) && !(AbsCosinus & 1))
    {
      /* Div by 2 */
      AtanIdx    >>= 1;
      AbsCosinus >>= 1;
    }
    AtanIdx /= AbsCosinus;
    Gis = ATAN_45_BY_IDX(AtanIdx);
  }
  else
  {
    /* Use Cotangent */
    AtanIdx = POINTS_FOR_45_DEG * (uint16_t)AbsCosinus;
    /* If both variables are even: divide by 2 to reduce the last division duration */
    while(!(AtanIdx & 1) && !(AbsSinus & 1))
    {
      /* Div by 2 */
      AtanIdx  >>= 1;
      AbsSinus >>= 1;
    }
    AtanIdx /= AbsSinus;
    Gis = (uint16_t)UNIT_PER_DEG(90) - ATAN_45_BY_IDX(AtanIdx);
  }
  /* Now, we must find the good quadrant to add the rigth offset */
  if(Cosinus >= 0)
  {
    if(Sinus >= 0)
    {
      Ret = UNIT_PER_DEG(90) - Gis;
    }
    else
    {
      Ret = UNIT_PER_DEG(90) + Gis;
    }
  }
  else
  {
    if(Sinus >= 0)
    {
      Gis += UNIT_PER_DEG(270);
      if(Gis == UNIT_PER_DEG(360))
      {
        /* Perticular case of 360° -> 0° */
        Gis = 0;
      }
      Ret = Gis;
    }
    else
    {
      Ret = UNIT_PER_DEG(270) - Gis;
    }
  }
  return(Ret);
}

/**
* \file   Xany.cpp
* \fn     uint8_t throttle(int16_t ExcSinus, int16_t ExcCosinus, uint8_t ExpoPerCentIdx)
* \brief  Computes the throttle from sinus and cosinus excursion values with expo
* \param  ExcSinus:       Sinus excursion value (-255 to 255)
* \param  ExcCosinus:     Sinus excursion value (-255 to 255)
* \param  ExpoPerCentIdx: Expo per cent index: 0 -> 0%, 1 -> 25%, 2 -> 37.5%, 3 -> 50%
* \return The throttle value (from 0 to 255)
*/
static uint8_t throttle(int16_t ExcSinus, int16_t ExcCosinus, uint8_t ExpoPerCentIdx)
{
  uint32_t ThrottleSquare;
  int16_t  ExcSin, ExcCos;
  uint16_t SqrExcSin, SqrExcCos;
  uint8_t  RawThrottle, OutThrottle;

  ExcSin = ExcSinus;
  ExcCos = ExcCosinus;

  /* Take absolute value */
  if(ExcSin < 0)   ExcSin = -ExcSin;
  if(ExcCos < 0)   ExcCos = -ExcCos;
  /* Limit range to 8 bit */
  if(ExcSin > 255) ExcSin = 255;
  if(ExcCos > 255) ExcCos = 255;

  SqrExcSin      = (uint16_t)ExcSin  * (uint16_t)ExcSin; /* cast to unsigned because 181 x 181 = 32 761, so 182 x 182 is negative */
  SqrExcCos      = (uint16_t)ExcCos  * (uint16_t)ExcCos; /* cast to unsigned because 181 x 181 = 32 761, so 182 x 182 is negative */
  ThrottleSquare = (uint32_t)((uint32_t)SqrExcSin + (uint32_t)SqrExcCos); /* /!\ The sum may be higher than 255 x 255 /!\ */
//  ThrottleSquare = (ThrottleSquare <= (255U * 255U))? ThrottleSquare: (255U * 255U);
  ThrottleSquare = (ThrottleSquare <= (65025))? ThrottleSquare: (65025);
  RawThrottle    = tinySqrt((uint16_t)ThrottleSquare);
  if(ExpoPerCentIdx)
  {
    OutThrottle  = tinyExpo(RawThrottle, ExpoPerCentIdx);
  }
  else
  {
    OutThrottle  = RawThrottle;
  }
  return(OutThrottle);
}

/**
* \file   Xany.cpp
* \fn     uint8_t tinySqrt(uint16_t Square)
* \brief  Computes 8 bit square root from a 16 bit value
* \param  Square: The input 16 bit value (0 to (255 * 255))
* \return The 8 bit square root value (from 0 to 255)
*/
static uint8_t tinySqrt(uint16_t Square) /* Max exec time: 4 to 60us */
{
  uint8_t Sqrt;
  uint8_t Start, End;

  if(Square < (128U * 128U))
  {
    if(Square < (64U * 64U))
    {
      Start = 0;
      End   = 63;
    }
    else
    {
      Start = 64;
      End   = 127;
    }
  }
  else
  {
    if(Square < (192U * 192U))
    {
      Start = 128;
      End   = 191;
    }
    else
    {
      Start = 192;
      End   = 255;
    }
  }
  for(Sqrt = Start; Sqrt <= End; Sqrt++)
  {
    if((uint16_t)(Sqrt * Sqrt) >= Square) break;
    if(Sqrt == End) break; /* For 255 to exit the loop */
  }
  return(Sqrt);
}

/**
* \file   Xany.cpp
* \fn     uint8_t tinyExpos(uint8_t InValue, uint8_t ExpoPerCentIdx)
* \brief  Computes 8 bit expo
* \param  InValue: The input 8 bit value (0 to 255)
* \param  ExpoPerCentIdx: Expo per cent index: 0 -> 0%, 1 -> 25%, 2 -> 37.5%, 3 -> 50%
* \return The output value (from 0 to 255)
*/
static uint8_t tinyExpo(uint8_t InValue, uint8_t ExpoPerCentIdx)
{
  uint8_t HalfValue, Delta = 0, OutValue;

  HalfValue   = (InValue < 128)? InValue: (255 - InValue);
  /* Apply Expo:
  OutVal 255^
            |                        /
            |                      //
            |                    /
            |                  /   /
            |                /
            |              x     /    <-- Expo=0% (linear response)
            |            / x          <-- Expo=12.5% (not implemented)
            |          /   x   /      <-- Expo=25%
            |        /     x          <-- Expo=37.5%
            |      /       x          <-- Expo=50%
            |    /    /
            |  /  /
            |//
            +--------------|----------->
            0              50%         255 InVal
  */
  switch(ExpoPerCentIdx)
  {
    case X_ANY_EXPO_0_PER_CENT:
    Delta = 0;
    break;

    case X_ANY_EXPO_25_PER_CENT:
    Delta = (HalfValue / 4);
    break;

    case X_ANY_EXPO_37_5_PER_CENT:
    Delta = ((HalfValue / 8) + (HalfValue / 4));
    break;

    case X_ANY_EXPO_50_PER_CENT:
    Delta = (HalfValue / 2);
    break;

    default:
    break;
  }
  OutValue = (InValue < 128)? HalfValue - Delta: (255 - (HalfValue + Delta));

  return(OutValue);
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
  BytePtr = XanyMsg->Common.PayloadAndChecksum;
  for(uint8_t ByteIdx = 0; ByteIdx < (PayloadNibbleNb + 1) / 2; ByteIdx++)
  {
    Checksum ^= BytePtr[ByteIdx];
  }
  Checksum ^= 0x55;
  if(PayloadNibbleNb & 1)
  {
    /* Odd number of nibble -> Checksum between 2 contiguous bytes */
    BytePtr   = XanyMsg->Common.PayloadAndChecksum + (PayloadNibbleNb / 2);
    *BytePtr = *BytePtr | ((Checksum & 0xF0) >> 4); /* Most  Significant Nibble of Checksum */
    BytePtr++;
    *BytePtr |= ((Checksum & 0x0F) << 4); /* Least Significant Nibble of Checksum */
  }
  else
  {
    /* Even number of nibble -> Checksum in a single byte */
    BytePtr   = XanyMsg->Common.PayloadAndChecksum + (PayloadNibbleNb / 2);
    *BytePtr  = Checksum;
  }
}

/**
* \file   Xany.cpp
* \fn     int32_t map32(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
* \brief  Map an input range to an output range
* \param  x:       value to map
* \param  in_min:  min value of input  range
* \param  in_max:  max value of input  range
* \param  out_min: min value of output range
* \param  out_max: max value of output range
* \return The mapped value
*/
static int32_t map32(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
