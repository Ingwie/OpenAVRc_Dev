/*
 **************************************************************************
 *                                                                        *
 *              This file is part of the OpenAVRc project.                *
 *                                                                        *
 *       GPS last-position QR view for 128x64 telemetry displays.         *
 *                                                                        *
 **************************************************************************
 */

#ifndef _VIEW_QR_H_
#define _VIEW_QR_H_

#include "../OpenAVRc.h"

#if defined(GPS)
void gpsQrUpdateLastPosition();
uint8_t gpsQrHasValidPosition();
void menuGpsQr(uint8_t event);
#endif

#endif
