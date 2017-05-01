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


#ifndef PROTODEF

#else
//definition : PROTODEF(proto, module, map, cmd, name, progmem_name) proto,

// PB6 out
PROTODEF(PROTOCOL_PPM_BB, MULTIMOD, NULL, PPM_BB_Cmds, STR_PPM_BB, const pm_char STR_PPM_BB[] PROGMEM = "BitBang")
PROTODEF(PROTOCOL_PPM_SWITCHING, MULTIMOD, NULL, PPM_SWITCHING_Cmds, STR_PPM_SWITCHING ,const pm_char STR_PPM_SWITCHING[] PROGMEM = " HW_PPM")

#ifdef PROTO_HAS_CYRF6936
PROTODEF(PROTOCOL_DEVO, CYRF6936, EATRG, DEVO_Cmds, STR_DEVO, const pm_char STR_DEVO[] PROGMEM = "Devo")
PROTODEF(PROTOCOL_DSM2, CYRF6936, EATRG, DSM_Cmds, STR_DSM2, const pm_char STR_DSM2[] PROGMEM = "DSM2")
PROTODEF(PROTOCOL_DSMX, CYRF6936, EATRG, DSM_Cmds, STR_DSMX, const pm_char STR_DSMX[] PROGMEM = "DSMX")
// PROTODEF(PROTOCOL_WK2801, CYRF6936, EATRG, WK2x01_Cmds, "WK2801")
#ifndef DEFINE_FUNCS
// PROTODEF(PROTOCOL_WK2601, CYRF6936, EATRG, WK2x01_Cmds, "WK2601")
// PROTODEF(PROTOCOL_WK2401, CYRF6936, EATRG, WK2x01_Cmds, "WK2401")
#endif
// PROTODEF(PROTOCOL_DSMX,   CYRF6936, TAERG, DSM2_Cmds,   "DSMX")
// PROTODEF(PROTOCOL_J6PRO,  CYRF6936, AETRG, J6PRO_Cmds,  "J6Pro")
#endif //PROTO_HAS_CYRF6936
#ifdef PROTO_HAS_A7105
//PROTODEF(PROTOCOL_FLYSKY, A7105, AETRG, FLYSKY_Cmds, "Flysky")
//PROTODEF(PROTOCOL_HUBSAN, A7105, AETRG, HUBSAN_Cmds, "Hubsan4")
#endif //PROTO_HAS_A7105
#ifdef PROTO_HAS_CC2500
PROTODEF(PROTOCOL_FRSKYD, CC2500, AETRG, FRSKYD_Cmds, STR_FRSKYD, const pm_char STR_FRSKYD[] PROGMEM = "FrSky-D")
PROTODEF(PROTOCOL_FRSKYV, CC2500, AETRG, FRSKYV_Cmds, STR_FRSKYV, const pm_char STR_FRSKYV[] PROGMEM = "FrSky-V8")
PROTODEF(PROTOCOL_SKYARTEC, CC2500, AETRG, SKYARTEC_Cmds, STR_SKYARTEC ,const pm_char STR_SKYARTEC[] PROGMEM = "Skyartec")
#endif //PROTO_HAS_CC2500
#ifdef PROTO_HAS_NRF24L01
// PROTODEF(PROTOCOL_V202, NRF24L01, AETRG, V202_Cmds, "V202")
// PROTODEF(PROTOCOL_SLT, NRF24L01, AETRG, SLT_Cmds, "SLT")
// PROTODEF(PROTOCOL_HiSky, NRF24L01, AETRG, HiSky_Cmds, "HiSky")
//PROTODEF(PROTOCOL_FBL100, NRF24L01, AETRG, FBL100_Cmds, "FBL100")
//PROTODEF(PROTOCOL_YD717, NRF24L01, AETRG, YD717_Cmds, "YD717")
#ifndef DEFINE_FUNCS
//PROTODEF(PROTOCOL_SymaX, NRF24L01, AETRG, YD717_Cmds, "SymaX")
#endif
// PROTODEF(PROTOCOL_CFlie, NRF24L01, AETRG, CFlie_Cmds, "CFlie")
// PROTODEF(PROTOCOL_H377, NRF24L01, AETRG, H377_Cmds, "H377")
// PROTODEF(PROTOCOL_HM830, NRF24L01, TAERG, HM830_Cmds, "HM830")
// PROTODEF(PROTOCOL_KN, NRF24L01, TAERG, KN_Cmds, "KN")
// PROTODEF(PROTOCOL_ESKY150, NRF24L01, TAERG, ESKY150_Cmds, "ESky150")
// PROTODEF(PROTOCOL_ESKY, NRF24L01, AETRG, ESKY_Cmds, "ESky")
// PROTODEF(PROTOCOL_NE260, NRF24L01, AETRG, NE260_Cmds, "NE260")
#endif //PROTO_HAS_NRF24L01

// PROTODEF(PROTOCOL_PPM_IC, TX_MODULE_LAST,  NULL, PPM_IC_Cmds, "PPM-in-Unimod") //unimod rick Added.
// PROTODEF(PROTOCOL_MPX_SERIAL, TX_MODULE_LAST,  NULL, MPX_SERIAL_Cmds, "MPX_Serial") //unimod rick Added.

#ifndef UNIMOD
//PROTODEF(PROTOCOL_USBHID, TX_MODULE_LAST,  NULL, USBHID_Cmds, "USBHID")
#endif //UNIMOD
#ifdef BUILDTYPE_DEV
//PROTODEF(PROTOCOL_TESTRF, TX_MODULE_LAST,  NULL, TESTRF_Cmds, "TESTRF")
#endif //BUILDTYPE_DEV
#endif //PROTODEF
