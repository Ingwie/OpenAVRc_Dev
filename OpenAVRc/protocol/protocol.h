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
//definition : PROTODEF(proto, module, cmd, name, progmem_name)

PROTODEF(PROTOCOL_PPM, INTERNAL, PROTO_PPM_Cmds, STR_PPM ,const pm_char STR_PPM[] PROGMEM = "PPM")
PROTODEF(PROTOCOL_PPM16, INTERNAL, PROTO_PPM16_Cmds, STR_PPM16 ,const pm_char STR_PPM16[] PROGMEM = "PPM16")
#ifndef BLUETOOTH
PROTODEF(PROTOCOL_PPMSIM, INTERNAL, PROTO_PPMSIM_Cmds, STR_PPMSIM ,const pm_char STR_PPMSIM[] PROGMEM = "PPMSIM")
#else
PROTODEF(PROTOCOL_PPMSIM, INTERNAL, PROTO_PPMSIM_Cmds, STR_PPMSIMBT ,const pm_char STR_PPMSIMBT[] PROGMEM = "SIM/BT")
#endif
#if (PCM_PROTOCOL==FUTPCM1K)
PROTODEF(PROTOCOL_FUTPCM1K, INTERNAL, PROTO_FUTPCM1K_Cmds, STR_FUTPCM1K ,const pm_char STR_FUTPCM1K[] PROGMEM = "FUTPCM1K")
#endif
#if (SERIAL_PROTOCOL==DSM)
PROTODEF(PROTOCOL_DSM_SERIAL, INTERNAL, DSM_SERIAL_Cmds, STR_DSM_SERIAL, const pm_char STR_DSM_SERIAL[] PROGMEM = "DSM-SERI")
#elif (SERIAL_PROTOCOL==MULTIMODULE)
PROTODEF(PROTOCOL_MULTI, MULTIMOD, MULTI_Cmds, STR_MULTI ,const pm_char STR_MULTI[] PROGMEM = "MultiMod")
#elif (SERIAL_PROTOCOL==CRSF)
PROTODEF(PROTOCOL_CRSF, CRSF, CRSF_Cmds, STR_CRSF_SERIAL ,const pm_char STR_CRSF_SERIAL[] PROGMEM = "CRSF")
#elif (SERIAL_PROTOCOL==SBUS)
PROTODEF(PROTOCOL_SBUS, SBUS, SBUS_Cmds, STR_SBUS_SERIAL ,const pm_char STR_SBUS_SERIAL[] PROGMEM = "Sbus")
#elif (SERIAL_PROTOCOL==SUMD)
PROTODEF(PROTOCOL_SUMD, SUMD, SUMD_Cmds, STR_SUMD_SERIAL ,const pm_char STR_SUMD_SERIAL[] PROGMEM = "Sumd")
#endif

#if defined(SPIMODULES)

#ifdef PROTO_HAS_CC2500
PROTODEF(PROTOCOL_FRSKYD, CC2500, FRSKYD_Cmds, STR_FRSKYD, const pm_char STR_FRSKYD[] PROGMEM = "FrSky-D")
PROTODEF(PROTOCOL_FRSKYV, CC2500, FRSKYV_Cmds, STR_FRSKYV, const pm_char STR_FRSKYV[] PROGMEM = "FrSky-V8")
PROTODEF(PROTOCOL_FRSKYX, CC2500, FRSKYX_Cmds, STR_FRSKYX, const pm_char STR_FRSKYX[] PROGMEM = "FrSky-X")
PROTODEF(PROTOCOL_SKYARTEC, CC2500, SKYARTEC_Cmds, STR_SKYARTEC ,const pm_char STR_SKYARTEC[] PROGMEM = "Skyartec")
PROTODEF(PROTOCOL_CORONA, CC2500, CORONA_Cmds, STR_CORONA ,const pm_char STR_CORONA[] PROGMEM = "Corona")
PROTODEF(PROTOCOL_HITEC, CC2500, HITEC_Cmds, STR_HITEC ,const pm_char STR_HITEC[] PROGMEM = "Hitec")
#endif //PROTO_HAS_CC2500

#ifdef PROTO_HAS_CYRF6936
PROTODEF(PROTOCOL_DEVO, CYRF6936, DEVO_Cmds, STR_DEVO, const pm_char STR_DEVO[] PROGMEM = "Devo")
PROTODEF(PROTOCOL_DSM, CYRF6936, DSM_Cmds, STR_DSM, const pm_char STR_DSM[] PROGMEM = "DSM")
PROTODEF(PROTOCOL_J6PRO, CYRF6936, J6PRO_Cmds, STR_J6PRO, const pm_char STR_J6PRO[] PROGMEM = "J6Pro")
#endif //PROTO_HAS_CYRF6936

#ifdef PROTO_HAS_A7105
PROTODEF(PROTOCOL_FLYSKY, A7105, FLYSKY_Cmds, STR_FLYSKY ,const pm_char STR_FLYSKY[] PROGMEM = "Flysky")
PROTODEF(PROTOCOL_AFHDS2A, A7105, AFHDS2A_Cmds, STR_AFHDS2A ,const pm_char STR_AFHDS2A[] PROGMEM = "AFHDS2A")
PROTODEF(PROTOCOL_HUBSAN, A7105, HUBSAN_Cmds, STR_HUBSAN, const pm_char STR_HUBSAN[] PROGMEM = "HUBSAN")
#endif //PROTO_HAS_A7105

#ifdef PROTO_HAS_NRF24L01
PROTODEF(PROTOCOL_BAYANG, NRF24L01, BAYANG_Cmds, STR_BAYANG, const pm_char STR_BAYANG[] PROGMEM = "Bayang")
PROTODEF(PROTOCOL_CABELL, NRF24L01, CABELL_Cmds, STR_CABELL, const pm_char STR_CABELL[] PROGMEM = "Cabell V3")
PROTODEF(PROTOCOL_STANEK, NRF24L01, STANEK_Cmds, STR_STANEK, const pm_char STR_STANEK[] PROGMEM = "Stanek")
#endif //PROTO_HAS_NRF24L01


#endif
#endif //PROTODEF
