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
                  Prop        :8,
                  Angle       :12;
}MsgAnglePropSt_t; /* Size = 4 bytes */

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
  MsgAnglePropSt_t MsgAngleProp;
  MsgPot4SwSt_t    MsgPot4Sw;
  MsgPot8SwSt_t    MsgPot8Sw;
  MsgCommonSt_t    Common;
  uint32_t         Raw;
}XanyMsg_union;  /* Size = 4 bytes */

/* Declaration of supported combinations (See X_ANY_CFG() macro in myeeprom.h) */
/*                                             Sw,Agl,Pot,Lgis,Lthr,RGis,RThr  */
#define XANY_MSG_4SW                  X_ANY_CFG(1,  0,  0,  0,  0,  0,  0)
#define XANY_MSG_8SW                  X_ANY_CFG(2,  0,  0,  0,  0,  0,  0)
#define XANY_MSG_16SW                 X_ANY_CFG(3,  0,  0,  0,  0,  0,  0)
#define XANY_MSG_ANGLE                X_ANY_CFG(0,  1,  0,  0,  0,  0,  0)
#define XANY_MSG_ANGLE_4SW            X_ANY_CFG(1,  1,  0,  0,  0,  0,  0)
#define XANY_MSG_ANGLE_8SW            X_ANY_CFG(2,  1,  0,  0,  0,  0,  0)
#define XANY_MSG_ANGLE_PROP           X_ANY_CFG(0,  1,  1,  0,  0,  0,  0)
#define XANY_MSG_POT_4SW              X_ANY_CFG(1,  0,  1,  0,  0,  0,  0)
#define XANY_MSG_POT_8SW              X_ANY_CFG(2,  0,  1,  0,  0,  0,  0)

#define XANY_MSG_L_CS_GIS             X_ANY_CFG(0,  0,  0,  1,  0,  0,  0)
#define XANY_MSG_L_CS_GIS_THR         X_ANY_CFG(0,  0,  0,  1,  1,  0,  0)
#define XANY_MSG_L_CS_GIS_POT         X_ANY_CFG(0,  0,  1,  1,  0,  0,  0)

#define XANY_MSG_R_CS_GIS             X_ANY_CFG(0,  0,  0,  0,  0,  1,  0)
#define XANY_MSG_R_CS_GIS_THR         X_ANY_CFG(0,  0,  0,  0,  0,  1,  1)
#define XANY_MSG_R_CS_GIS_POT         X_ANY_CFG(0,  0,  1,  0,  0,  1,  0)

/* Message length (in nibbles) including the checksum for each supported combination */
#define XANY_MSG_4SW_NBL_NB           (1 + 2)
#define XANY_MSG_8SW_NBL_NB           (2 + 2)
#define XANY_MSG_16SW_NBL_NB          (4 + 2)
#define XANY_MSG_ANGLE_NBL_NB         (3 + 2)
#define XANY_MSG_ANGLE_4SW_NBL_NB     (3 + 1 + 2)
#define XANY_MSG_ANGLE_8SW_NBL_NB     (3 + 2 + 2)
#define XANY_MSG_ANGLE_PROP_NBL_NB    (3 + 2 + 2)
#define XANY_MSG_POT_4SW_NBL_NB       (2 + 1 + 2)
#define XANY_MSG_POT_8SW_NBL_NB       (2 + 2 + 2)

#define XANY_MSG_L_CS_GIS_NBL_NB      (3 + 2)
#define XANY_MSG_L_CS_GIS_THR_NBL_NB  (3 + 2 + 2)
#define XANY_MSG_L_CS_GIS_POT_NBL_NB  (3 + 2 + 2)

#define XANY_MSG_R_CS_GIS_NBL_NB      (3 + 2)
#define XANY_MSG_R_CS_GIS_THR_NBL_NB  (3 + 2 + 2)
#define XANY_MSG_R_CS_GIS_POT_NBL_NB  (3 + 2 + 2)

/* Array of Pots choosen */
#define CONV_XANY_POTS(x)             (x<4)? 3-x : x
const uint8_t XANY_POT[] PROGMEM = {CONV_XANY_POTS(X_ANY_1_POT), CONV_XANY_POTS(X_ANY_2_POT), CONV_XANY_POTS(X_ANY_3_POT), CONV_XANY_POTS(X_ANY_4_POT)};
#define GET_XANY_POT(PotIdx)          (uint8_t)     pgm_read_byte_near(&XANY_POT[PotIdx])

/* Allegro A1335 absolute angle sensor 7 bit I2C address */
#define A1335_I2C_7B_ADDR             (0x0C) /* /!\ Even if the chip is powered under 5V, I2C SDA/SCL pull-ups SHALL be tied at 3.3V /!\ */

/*    <------------------8 I/O Expenders------------------><----------------16 I/O Expenders--------------->    */
enum {IO_EXP_PCF8574 = 0, IO_EXP_PCF8574A, IO_EXP_PCA9654E, IO_EXP_MCP23017, IO_EXP_PCF8575A, IO_EXP_PCA9671, IO_EXP_TYPE_NB};

/* PRIVATE FUNCTION PROTOYPES */
static uint8_t  getMsgType     (uint8_t XanyIdx);
static uint8_t  readIoExtender (uint8_t XanyIdx, uint8_t *RxBuf, uint8_t ByteToRead);
static uint8_t  readAngleSensor(uint8_t XanyIdx, uint16_t *Angle);
static uint16_t sinCosToGis(int16_t Sinus, int16_t Cosinus);
static uint8_t  throttle(int16_t ExcSinus, int16_t ExcCosinus, uint8_t ExpoLevel);
static uint8_t  tinySqrt(uint16_t Square); /* Max exec time: 4 to 60us */
static uint8_t  tinyExpo(uint8_t InValue, uint8_t ExpoPerCentIdx);
static void     updateXanyMsgChecksum(XanyMsg_union *XanyMsg);
static uint8_t  getBigEndianNibbleNbToTx(XanyMsg_union *XanyMsg);
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
#define POINTS_FOR_45_DEG             (uint32_t)sizeof(Atan45Tbl) /* 32 bits to avoid trunction with macro */
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
  uint8_t          MsgType, One8bitPort = 0, Prop = 0, ExpoPerCentIdx = 0, Radius, ValidMsg = 1;
  uint16_t         Two8bitPorts = 0, Angle = 0;
  int16_t          ExcSin, ExcCos;

  Built.Raw = 0; /* Clear temporary message */
  MsgType = getMsgType(XanyIdx);
  if(XanyOp & XANY_OP_READ_INFO)
  {
    Read.Raw = htonl(X_AnyReadMsg[XanyIdx].Raw);
  }
  else
  {
    ExpoPerCentIdx = g_model.Xany[XanyIdx].Param.Expo.PerCentIdx;
    ExpoPerCentIdx = (ExpoPerCentIdx < X_ANY_EXPO_PER_CENT_NB)? ExpoPerCentIdx: 0;
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
      XanyInfo->PropValue    = 0;
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
      XanyInfo->PropValue    = 0;
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
      XanyInfo->PropValue    = 0;
    }
    break;

    case XANY_MSG_ANGLE:
    case XANY_MSG_L_CS_GIS:
    case XANY_MSG_R_CS_GIS:
    Built.MsgAngle.NibbleNbToTx = XANY_MSG_ANGLE_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      switch(MsgType)
      {
        case XANY_MSG_ANGLE:
        readAngleSensor(XanyIdx, &Angle);
        break;

        case XANY_MSG_L_CS_GIS:
        ExcSin = (calibratedStick[ELE_STICK] / 2);
        ExcCos = (calibratedStick[RUD_STICK] / 2);
        Radius = throttle(ExcSin / 2, ExcCos / 2, 0);
        if(Radius >= DEAD_ANGLE_RADIUS)
        {
          Angle  = sinCosToGis(ExcSin, ExcCos);
        }
        else
        {
          Read.Raw = htonl(X_AnyReadMsg[XanyIdx].Raw);
          Angle    = Read.MsgAngle.Angle; /* Keep current angle */
        }
        break;

        case XANY_MSG_R_CS_GIS:
        ExcSin = (calibratedStick[THR_STICK] / 2);
        ExcCos = (calibratedStick[AIL_STICK] / 2);
        Radius = throttle(ExcSin / 2, ExcCos / 2, 0);
        if(Radius >= DEAD_ANGLE_RADIUS)
        {
          Angle  = sinCosToGis(ExcSin, ExcCos);
        }
        else
        {
          Read.Raw = htonl(X_AnyReadMsg[XanyIdx].Raw);
          Angle    = Read.MsgAngle.Angle; /* Keep current angle */
        }
        break;
      }
      Built.MsgAngle.Angle = Angle;
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Read.MsgAngle.NibbleNbToTx;
      XanyInfo->SwNb         = 0;
      XanyInfo->SwValue      = 0;
      XanyInfo->Angle        = Read.MsgAngle.Angle;
      XanyInfo->PropValue    = 0;
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
      XanyInfo->PropValue    = 0;
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
      XanyInfo->PropValue    = 0;
    }
    break;

    case XANY_MSG_ANGLE_PROP:
    case XANY_MSG_L_CS_GIS_THR:
    case XANY_MSG_L_CS_GIS_POT:
    case XANY_MSG_R_CS_GIS_THR:
    case XANY_MSG_R_CS_GIS_POT:
    Built.MsgAngleProp.NibbleNbToTx = XANY_MSG_ANGLE_PROP_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      switch(MsgType)
      {
        case XANY_MSG_ANGLE_PROP:
        readAngleSensor(XanyIdx, &Angle);
        Prop = ((calibratedStick[GET_XANY_POT(XanyIdx)] + RESX - 1) / 8);
        break;

        case XANY_MSG_L_CS_GIS_THR:
        case XANY_MSG_L_CS_GIS_POT:
        ExcSin = (calibratedStick[CONVERT_MODE(ELE_STICK)] / 2);
        ExcCos = (calibratedStick[CONVERT_MODE(RUD_STICK)] / 2);
        /* Here, ExcSin and ExcCos are between -512 and +511 (-> better angle accuracy) */
        Radius = throttle(ExcSin / 2, ExcCos / 2, 0); /* throttle() expects arguments between -255 to 255 */
        if(Radius >= DEAD_ANGLE_RADIUS)
        {
          Angle  = sinCosToGis(ExcSin, ExcCos);
        }
        else
        {
          Read.Raw = htonl(X_AnyReadMsg[XanyIdx].Raw);
          Angle    = Read.MsgAngle.Angle; /* Keep current angle */
        }
        if(MsgType == XANY_MSG_L_CS_GIS_THR)
        {
          if(Radius >= DEAD_THROTTLE_RADIUS)
          {
            Prop = map32(Radius, DEAD_THROTTLE_RADIUS, 255, 0, 255);
          }
        }
        else
        {
          Prop = ((calibratedStick[GET_XANY_POT(XanyIdx)] + RESX - 1) / 8);
        }
        break;

        case XANY_MSG_R_CS_GIS_THR:
        case XANY_MSG_R_CS_GIS_POT:
        ExcSin = (calibratedStick[CONVERT_MODE(THR_STICK)] / 2);
        ExcCos = (calibratedStick[CONVERT_MODE(AIL_STICK)] / 2);
        /* Here, ExcSin and ExcCos are between -512 and +511 (-> better angle accuracy) */
        Radius = throttle(ExcSin / 2, ExcCos / 2, 0); /* throttle() expects arguments between -255 to 255 */
        if(Radius >= DEAD_ANGLE_RADIUS)
        {
          Angle  = sinCosToGis(ExcSin, ExcCos);
        }
        else
        {
          Read.Raw = htonl(X_AnyReadMsg[XanyIdx].Raw);
          Angle    = Read.MsgAngle.Angle; /* Keep current angle */
        }
        if(MsgType == XANY_MSG_R_CS_GIS_THR)
        {
          if(Radius >= DEAD_THROTTLE_RADIUS)
          {
            Prop = map32(Radius, DEAD_THROTTLE_RADIUS, 255, 0, 255);
          }
        }
        else
        {
          Prop = ((calibratedStick[GET_XANY_POT(XanyIdx)] + RESX - 1) / 8);
        }
        break;
      }
      Built.MsgAngleProp.Angle = Angle;
      Built.MsgAngleProp.Prop  = tinyExpo(Prop, ExpoPerCentIdx); /* 8 bits expo value at global level */
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Read.MsgAngleProp.NibbleNbToTx;
      XanyInfo->SwNb         = 0;
      XanyInfo->SwValue      = 0;
      XanyInfo->Angle        = Read.MsgAngleProp.Angle;
      XanyInfo->PropValue    = Read.MsgAngleProp.Prop;
    }
    break;

    case XANY_MSG_POT_4SW:
    Built.MsgPot4Sw.NibbleNbToTx = XANY_MSG_POT_4SW_NBL_NB;
    if(XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
      Built.MsgPot4Sw.Sw    = One8bitPort & 0x0F; /* Keep 4 bits */
      Built.MsgPot4Sw.Pot   = tinyExpo(((calibratedStick[GET_XANY_POT(XanyIdx)] + RESX - 1) / 8), ExpoPerCentIdx); /* 8 bits value */
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Read.MsgPot4Sw.NibbleNbToTx;
      XanyInfo->SwNb         = 4;
      XanyInfo->SwValue      = Read.MsgPot4Sw.Sw;
      XanyInfo->Angle        = 0;
      XanyInfo->PropValue    = Read.MsgPot4Sw.Pot;
    }
    break;

    case XANY_MSG_POT_8SW:
    Built.MsgPot8Sw.NibbleNbToTx = XANY_MSG_POT_8SW_NBL_NB;
    if (XanyOp & XANY_OP_BUILD_MSG)
    {
      readIoExtender(XanyIdx, (uint8_t *)&One8bitPort, 1);
      Built.MsgPot8Sw.Sw    = One8bitPort;
      Built.MsgPot8Sw.Pot   = tinyExpo(((calibratedStick[GET_XANY_POT(XanyIdx)] + RESX - 1) / 8), ExpoPerCentIdx); /* 8 bits value */
    }
    if(XanyOp & XANY_OP_READ_INFO)
    {
      XanyInfo->MsgNibbleLen = Read.MsgPot8Sw.NibbleNbToTx;
      XanyInfo->SwNb         = 8;
      XanyInfo->SwValue      = Read.MsgPot8Sw.Sw;
      XanyInfo->Angle        = 0;
      XanyInfo->PropValue    = Read.MsgPot8Sw.Pot;
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
      XanyInfo->PropValue    = 0;
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

/**
* \file   Xany.cpp
* \fn     uint8_t Xany_getAngleSrcIdx(uint8_t XanyIdx)
* \brief  Returns the angle source of the specified Xany instance in non volatile memory
* \param  XanyIdx: Index of the X-Any instance
* \return The index of the angle source
*/
uint8_t Xany_getAngleSrcIdx(uint8_t XanyIdx)
{
  if(g_model.Xany[XanyIdx].PayloadCfg.Item.AbsAngleSensor)     return(X_ANY_ANGLE_SRC_ABS_SENSOR);
  if(g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickLeftGis)  return(X_ANY_ANGLE_SRC_LEFT_CROSS_STICK);
  if(g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickRightGis) return(X_ANY_ANGLE_SRC_RIGHT_CROSS_STICK);
  return(X_ANY_ANGLE_SRC_NONE);
}

/**
* \file   Xany.cpp
* \fn     void Xany_updateAngleSrcIdx(uint8_t XanyIdx, uint8_t AngleSrcIdx)
* \brief  Returns update the proportional source of the specified Xany instance in non volatile memory
* \param  XanyIdx:     Index of the X-Any instance
* \param  AngleSrcIdx: Index of the angle source
* \return Void
*/
void Xany_updateAngleSrcIdx(uint8_t XanyIdx, uint8_t AngleSrcIdx)
{
  switch(AngleSrcIdx)
  {

    case X_ANY_ANGLE_SRC_ABS_SENSOR:
    g_model.Xany[XanyIdx].PayloadCfg.Item.AbsAngleSensor     = 1;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickLeftGis  = 0;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickRightGis = 0;
    break;

    case X_ANY_ANGLE_SRC_LEFT_CROSS_STICK:
    g_model.Xany[XanyIdx].PayloadCfg.Item.AbsAngleSensor     = 0;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickLeftGis  = 1;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickRightGis = 0;
    break;

    case X_ANY_ANGLE_SRC_RIGHT_CROSS_STICK:
    g_model.Xany[XanyIdx].PayloadCfg.Item.AbsAngleSensor     = 0;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickLeftGis  = 0;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickRightGis = 1;
    break;

    case X_ANY_ANGLE_SRC_NONE:
    default:
    g_model.Xany[XanyIdx].PayloadCfg.Item.AbsAngleSensor     = 0;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickLeftGis  = 0;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickRightGis = 0;
    break;
  }
}

/**
* \file   Xany.cpp
* \fn     uint8_t Xany_getPropSrcIdx(uint8_t XanyIdx)
* \brief  Returns the proportional source of the specified Xany instance in non volatile memory
* \param  XanyIdx: Index of the X-Any instance
* \return The index of the proportionals source
*/
uint8_t Xany_getPropSrcIdx(uint8_t XanyIdx)
{
  if(g_model.Xany[XanyIdx].PayloadCfg.Item.RotPot)             return(X_ANY_PROP_SRC_POT);
  if(g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickLeftThr)  return(X_ANY_PROP_SRC_LEFT_CROSS_STICK);
  if(g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickRightThr) return(X_ANY_PROP_SRC_RIGHT_CROSS_STICK);
  return(X_ANY_PROP_SRC_NONE);
}

/**
* \file   Xany.cpp
* \fn     void Xany_updatePropSrcIdx(uint8_t XanyIdx, uint8_t PropSrcIdx)
* \brief  Returns update the proportional source of the specified Xany instance in non volatile memory
* \param  XanyIdx:    Index of the X-Any
* \param  PropSrcIdx: Index of the proportional source
* \return Void
*/
void Xany_updatePropSrcIdx(uint8_t XanyIdx, uint8_t PropSrcIdx)
{
  switch(PropSrcIdx)
  {

    case X_ANY_PROP_SRC_POT:
    g_model.Xany[XanyIdx].PayloadCfg.Item.RotPot             = 1;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickLeftThr  = 0;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickRightThr = 0;
    break;

    case X_ANY_PROP_SRC_LEFT_CROSS_STICK:
    g_model.Xany[XanyIdx].PayloadCfg.Item.RotPot             = 0;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickLeftThr  = 1;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickRightThr = 0;
    break;

    case X_ANY_PROP_SRC_RIGHT_CROSS_STICK:
    g_model.Xany[XanyIdx].PayloadCfg.Item.RotPot             = 0;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickLeftThr  = 0;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickRightThr = 1;
    break;

    case X_ANY_PROP_SRC_NONE:
    default:
    g_model.Xany[XanyIdx].PayloadCfg.Item.RotPot             = 0;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickLeftThr  = 0;
    g_model.Xany[XanyIdx].PayloadCfg.Item.CrossStickRightThr = 0;
    break;
  }
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
