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


//


void mpx_start();
void mpx_telem_send();
void uart_putc(char);
void uart_puts(char *);

enum
{
	TELEM_MPX_CHANNEL_0 = 0,
	TELEM_MPX_CHANNEL_1,
	TELEM_MPX_CHANNEL_2,
	TELEM_MPX_CHANNEL_3,
	TELEM_MPX_CHANNEL_4,
	TELEM_MPX_CHANNEL_5,
	TELEM_MPX_CHANNEL_6,
	TELEM_MPX_CHANNEL_7,
	TELEM_MPX_CHANNEL_8,
	TELEM_MPX_CHANNEL_9,
	TELEM_MPX_CHANNEL_10,
	TELEM_MPX_CHANNEL_11,
	TELEM_MPX_CHANNEL_12,
	TELEM_MPX_CHANNEL_13,
	TELEM_MPX_CHANNEL_14,
	TELEM_MPX_CHANNEL_LAST
};

enum MPX_TELEM_MPX_Units
{
	TELEM_MPX_NOT_USED = 0, // Channel not used.
	TELEM_MPX_VOLTS_TENTHS,
	TELEM_MPX_AMPS_TENTHS,
	TELEM_MPX_MTRS_P_SEC_TENTHS,
	TELEM_MPX_KMTRS_P_HOUR_TENTHS,
	TELEM_MPX_RPM_HUNDREDS,
	TELEM_MPX_DEGREES_C_TENTHS,
	TELEM_MPX_DEGREES_TENTHS,
	TELEM_MPX_METRES,
	TELEM_MPX_P_CENT_FUEL,
	TELEM_MPX_P_CENT_LQI, // Link Quality Indicator use as RSSI ... misnomer ?.
	TELEM_MPX_MILLI_AMPERE_HOURS,
	TELEM_MPX_MILLI_LITRES,
	TELEM_MPX_KMTRS_TENTHS,
	TELEM_MPX_14_NO_UNITS,
	TELEM_MPX_15_NO_UNITS,
};


struct MPX_Telemetry
{
unsigned alarm :1;
signed value :15;
unsigned units :4;
};
