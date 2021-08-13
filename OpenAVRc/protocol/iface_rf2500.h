#ifndef _IFACE_RF2500_H_
#define _IFACE_RF2500_H_

#include "iface_cyrf6936.h"

//RF2500
void RF2500_Init(uint8_t, bool);
void RF2500_SetTXAddr(const uint8_t*);
void RF2500_BuildPayload(uint8_t*);
void RF2500_SendPayload();
#define RF2500_SetPower() CYRF_GFSK1M_SetPower()
#define CYRF_GFSK1M_SetPower() CYRF_SetPower(0x04)
#define RF2500_RFChannel(X) CYRF_ConfigRFChannel(X)

#endif
