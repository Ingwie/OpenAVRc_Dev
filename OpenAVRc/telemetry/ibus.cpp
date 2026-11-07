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


#include "../OpenAVRc.h"

// Telemetry helpers implemented in frsky.cpp.
void manageBaroAltitude();

#if defined(GPS)
void manageGpsAltitude();
#endif

// AFHDS2A / FlySky iBUS telemetry decoder.
//
// This file contains only code moved from telemetry/frsky.cpp.
// The decoder logic, sensor mappings and P2M state are unchanged.

// AFHDS2A telemetry decoder persistent state uses already reserved P2M bytes.
// spirf.h unused2[0..2] map to pulses2MHz bytes 134..136.
// This avoids dedicated static SRAM variables in frsky.cpp.
#define AFHDS2A_CELL_INSTANCE_BASE_P2M  pulses2MHz.spi.afhds2a.unused2[0]
#define AFHDS2A_TEMP1_INSTANCE_P2M      pulses2MHz.spi.afhds2a.unused2[1]
#define AFHDS2A_TEMP2_INSTANCE_P2M      pulses2MHz.spi.afhds2a.unused2[2]

static void AFHDS2A_oavrc_set_cell(uint8_t instance, uint16_t centivolts)
{
  // -------------------------------------------------------------------------
  // AFHDS2A cell-voltage instance mapping.
  //
  // A normal FlySky / AFHDS2A chain can expose six 0x04 cell sensors with
  // instances 1..6.
  //
  // The current oXs grouped layout uses iBUS addresses:
  //
  //   1 = GPS_FULL
  //   2 = VOLT_FULL
  //   3 = ACC_FULL
  //   4..9 = C1..C6
  //
  // Some receivers keep the original iBUS address as the AFHDS2A "instance".
  // In that case OpenAVRc receives the six cells as instances 4..9.
  //
  // The old code accepted only instances 1..6:
  //   instance 4 -> C4
  //   instance 5 -> C5
  //   instance 6 -> C6
  //   instances 7..9 -> rejected
  //
  // That produces exactly the observed result:
  //   C1=C2=C3=0, while C4=C5=C6 are valid.
  //
  // Auto-detection:
  // - seeing cell instance 7..9 proves that the 4..9 layout is in use;
  // - seeing cell instance 1..3 switches back to the standard 1..6 layout.
  //
  // No change is made to oXs or to the AFHDS2A RF format.
  // -------------------------------------------------------------------------
  if (instance >= 7U && instance <= 9U)
    AFHDS2A_CELL_INSTANCE_BASE_P2M = 4U;  // oXs grouped layout: 4..9 -> C1..C6
  else if (instance >= 1U && instance <= 3U)
    AFHDS2A_CELL_INSTANCE_BASE_P2M = 1U;  // standard layout: 1..6 -> C1..C6

  uint8_t idx;

  if (AFHDS2A_CELL_INSTANCE_BASE_P2M == 4U)
  {
    if (instance < 4U || instance > 9U)
      return;
    idx = instance - 4U;
  }
  else
  {
    if (instance < 1U || instance > 6U)
      return;
    idx = instance - 1U;
  }

  uint16_t cell = (centivolts + 1U) / 2U; // 0.01 V -> OpenAVRc 0.02 V units
  if (cell > 255U)
    cell = 255U;

  telemetryData.value.cellVolts[idx] = (uint8_t)cell;

  // cellsCount is a count (1..6), not the raw AFHDS2A instance number.
  const uint8_t cellCount = idx + 1U;
  if (telemetryData.value.cellsCount < cellCount)
    telemetryData.value.cellsCount = cellCount;

  uint8_t currentMin = 0U;
  uint8_t currentMinIdx = 0U;
  for (uint8_t i = 0U; i < telemetryData.value.cellsCount; ++i)
  {
    const uint8_t v = telemetryData.value.cellVolts[i];
    if (!v)
      continue;
    if (!currentMin || v < currentMin)
    {
      currentMin = v;
      currentMinIdx = i;
    }
  }

  telemetryData.value.minCellVolts = currentMin;
  telemetryData.value.minCellIdx = currentMinIdx;
  if (currentMin && (!telemetryData.value.minCell || currentMin < telemetryData.value.minCell))
    telemetryData.value.minCell = currentMin;
}


static uint16_t AFHDS2A_get_u16(const uint8_t *p)
{
  return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static int16_t AFHDS2A_get_s16(const uint8_t *p)
{
  return (int16_t)AFHDS2A_get_u16(p);
}

static int32_t AFHDS2A_get_s32(const uint8_t *p)
{
  return (int32_t)((uint32_t)p[0] |
                   ((uint32_t)p[1] << 8) |
                   ((uint32_t)p[2] << 16) |
                   ((uint32_t)p[3] << 24));
}

static void AFHDS2A_update_power(void)
{
  const uint16_t voltage = telemetryData.value.vfas;   // 0.1 V
  const uint16_t current = telemetryData.value.current; // 0.1 A
  telemetryData.value.power = ((current >> 1) * (voltage >> 1)) / 25;
  if (telemetryData.value.power > telemetryData.value.maxPower)
    telemetryData.value.maxPower = telemetryData.value.power;
}

static void AFHDS2A_set_gps_coord(int32_t value, bool latitude)
{
  uint32_t decum = (value < 0) ? (uint32_t)(-(int64_t)value) : (uint32_t)value;
  uint32_t temp = decum % 10000000UL;
  decum -= temp;
  uint16_t bp = (uint16_t)((decum / 10000000UL) * 100UL);
  temp *= 60UL;
  bp += (uint16_t)(temp / 10000000UL);
  temp %= 10000000UL;
  uint16_t ap = (uint16_t)(temp / 1000UL);

  if (latitude)
  {
    telemetryData.value.gpsLatitudeNS = (value >= 0) ? 'N' : 'S';
    telemetryData.value.gpsLatitude_bp = bp;
    telemetryData.value.gpsLatitude_ap = ap;
  }
  else
  {
    telemetryData.value.gpsLongitudeEW = (value >= 0) ? 'E' : 'W';
    telemetryData.value.gpsLongitude_bp = bp;
    telemetryData.value.gpsLongitude_ap = ap;
  }
}

static void AFHDS2A_set_accel(uint8_t id, int16_t value)
{
  // AFHDS2A/EdgeTX: m/s2 * 100. OpenAVRc: 0.01 g.
  int16_t accel = (int16_t)(((int32_t)value * 100L) / 981L);
  if (id == AFHDS2A_SENSOR_ACC_X) telemetryData.value.accelX = accel;
  if (id == AFHDS2A_SENSOR_ACC_Y) telemetryData.value.accelY = accel;
  if (id == AFHDS2A_SENSOR_ACC_Z) telemetryData.value.accelZ = accel;
}

static void AFHDS2A_set_temperature(uint8_t instance, uint16_t data16)
{
  const int16_t temperature = ((int16_t)data16 - 400) / 10;

  if (AFHDS2A_TEMP1_INSTANCE_P2M == 0xFFU || AFHDS2A_TEMP1_INSTANCE_P2M == instance)
  {
    AFHDS2A_TEMP1_INSTANCE_P2M = instance;
    telemetryData.value.temperature1 = temperature;
    checkMaxTemperature1();
  }
  else if (AFHDS2A_TEMP2_INSTANCE_P2M == 0xFFU || AFHDS2A_TEMP2_INSTANCE_P2M == instance)
  {
    AFHDS2A_TEMP2_INSTANCE_P2M = instance;
    telemetryData.value.temperature2 = temperature;
    checkMaxTemperature2();
  }
}

void process_ibus_telem(void)
{
  // Any packet reaching this decoder has already been accepted by the
  // AFHDS2A RF layer (packet type, transmitter ID and telemetry option).
  // Refresh the telemetry timeout on every valid normal or extended iBUS
  // packet.  Previously only selected 0xAA sensor records refreshed the
  // timeout, while 0xAC extended packets never did, which could make the
  // OpenAVRc "NO DATA" indication blink even though telemetry was present.
  if (ibus_telem_buffer[0] == 0xAA || ibus_telem_buffer[0] == 0xAC)
    frskyStreaming = frskyStreaming ? FRSKY_TIMEOUT10ms : FRSKY_TIMEOUT_FIRST;

  // Over The Air packet format.
  // 0    1234   5678   9           10         11       12                 ......36
  // AC | TXID | rx_id | sensor id | sensor # | length | bytes | sensor id ......

  // AA | TXID | rx_id | sensor id | sensor # | value 16 bit   | sensor id ......
  // Max 7 sensors per packet, but first packet has 5 Receiver internal sensors.


  if (ibus_telem_buffer[0] == 0xAA) // 0xAA Normal telemetry.
  {
    for (uint8_t sensor = 0; sensor < 7; sensor++)
    {
      uint8_t index = 1 + (4 * sensor); // Skip header.
      uint16_t data16 = ibus_telem_buffer[index + 3] << 8 | (ibus_telem_buffer[index + 2]);

      switch (ibus_telem_buffer[index])
      {
        case AFHDS2A_SENSOR_RX_VOLTAGE:
          if (ibus_telem_buffer[index + 1] == 0)
          { // Voltage is sent in two bytes as 0.01 Volt value.
            telemetryData.analog[TELEM_ANA_A1].set(
                ibus_telem_buffer[index + 3] << 6 | (ibus_telem_buffer[index + 2] >> 2), UNIT_VOLTS);
          }
          break;

        case AFHDS2A_SENSOR_RX_RSSI:
        {
          // EdgeTX AFHDS2A scaling for sensor 0xFC:
          // converted RSSI = 135 - raw value.
          int16_t rxRssi = 135 - ibus_telem_buffer[index + 2];
          limit<int16_t>(0, rxRssi, 99);
          telemetryData.rssi[0].set((uint8_t)rxRssi);
          break;
        }

        case AFHDS2A_SENSOR_RX_ERR_RATE:
          // 0xFE is the receiver error rate / link quality indication.
          // Keep it separate from the main OpenAVRc Rx RSSI display.
          break;

        case AFHDS2A_SENSOR_TEMPERATURE:
          AFHDS2A_set_temperature(ibus_telem_buffer[index + 1], data16);
          break;

        case AFHDS2A_SENSOR_RPM:
        case AFHDS2A_SENSOR_RPM2:
          telemetryData.value.rpm = data16;
          break;

        case AFHDS2A_SENSOR_CELL_VOLTAGE:
          AFHDS2A_oavrc_set_cell(ibus_telem_buffer[index + 1], data16);
          break;

        case AFHDS2A_SENSOR_EXT_VOLTAGE:
//        if (ibus_telem_buffer[index + 1] == 1) // Sensor #1. Is this the first in the Daisy Chain of Sensors ?.
//        { // FS-CVT01 Voltage is sent as int16_t 0.01 Volt value.
          // oXs sends MVOLT as 0.01 V. OpenAVRc VFAS uses 0.1 V.
          telemetryData.value.vfas = data16 / 10;
          if (!telemetryData.value.minVfas || telemetryData.value.vfas < telemetryData.value.minVfas)
            telemetryData.value.minVfas = telemetryData.value.vfas;
          AFHDS2A_update_power();
//        }
          break;

        case AFHDS2A_SENSOR_BAT_CURRENT:
          // iBUS current is A * 100. OpenAVRc stores current in 0.1 A units.
          telemetryData.value.current = data16 / 10;
          checkOffsetAndMaxCurrent();
          AFHDS2A_update_power();
          break;

        case AFHDS2A_SENSOR_FUEL:
          // ID 0x06 is used as fuel percentage by some FlySky sensors and as
          // consumed capacity by others. OpenAVRc maps it to Conso (mAh),
          // matching the oXs / battery telemetry use case.
          telemetryData.value.currentConsumption = data16;
          telemetryData.value.openXsensor = 1;
          break;

        case AFHDS2A_SENSOR_HEADING:
          // Native heading uses whole degrees. Reuse the existing GPS course
          // display fields without inventing an extra telemetry structure.
          telemetryData.value.gpsCourse_bp = data16;
          telemetryData.value.gpsCourse_ap = 0;
          break;

        case AFHDS2A_SENSOR_ACC_X:
        case AFHDS2A_SENSOR_ACC_Y:
        case AFHDS2A_SENSOR_ACC_Z:
          AFHDS2A_set_accel(ibus_telem_buffer[index], (int16_t)data16);
          break;

        case AFHDS2A_SENSOR_CLIMB_RATE:
          telemetryData.value.varioSpeed = (int16_t) data16;
          break;

        case AFHDS2A_SENSOR_COG:
          telemetryData.value.gpsCourse_bp = data16 / 100;
          telemetryData.value.gpsCourse_ap = data16 % 100;
          break;

        case AFHDS2A_SENSOR_VERTICAL_SPEED:
          telemetryData.value.varioSpeed = (int16_t)data16;
          break;

        case AFHDS2A_SENSOR_GROUND_SPEED:
          // AFHDS2A: m/s * 100. OpenAVRc GPS speed is stored in 0.1 knot.
          telemetryData.value.gpsSpeed_bp =
              (uint16_t)(((uint32_t)data16 * 1944UL + 5000UL) / 10000UL);
          telemetryData.value.gpsSpeed_ap = 0;
          checkMaxGpsSpeed();
          break;

        case AFHDS2A_SENSOR_GPS_DISTANCE:
          telemetryData.value.gpsDistance = data16;
          if (telemetryData.value.gpsDistance > telemetryData.value.maxGpsDistance)
            telemetryData.value.maxGpsDistance = telemetryData.value.gpsDistance;
          break;

        case AFHDS2A_SENSOR_ALT_FLYSKY:
          telemetryData.value.baroAltitude = (int16_t)data16;
          manageBaroAltitude();
          break;

        // AFHDS2A_SENSOR_GPS_STATUS, RX_SNR, RX_NOISE and RX_ERR_RATE have no
        // dedicated OpenAVRc TelemetryData fields in this code base, therefore
        // they are intentionally left untouched here.

        case 0xff: // end of sensor data
          break;
      }

    }
    // The stream timeout is refreshed once per accepted iBUS packet above.
  }
  else if (ibus_telem_buffer[0] == 0xAC) // 0xAC Extended telemetry.
  {
    uint8_t *sensor = ibus_telem_buffer + 1; // Skip packet type byte.

    while (sensor <= ibus_telem_buffer + 25 && sensor[0] != 0xFF)
    {
      const uint8_t id = sensor[0];
      const uint8_t size = sensor[2];
      uint8_t *data = sensor + 3;

      if ((uint16_t)(sensor - ibus_telem_buffer) + 3U + size > 29U)
        break;

      if (id == AFHDS2A_SENSOR_GPS_FULL && size >= 14U)
      {
        const uint8_t fix = data[0];
        const uint8_t sats = data[1];
        const int32_t lat = AFHDS2A_get_s32(data + 2);
        const int32_t lon = AFHDS2A_get_s32(data + 6);
        const int32_t alt = AFHDS2A_get_s32(data + 10);

        AFHDS2A_set_gps_coord(lat, true);
        AFHDS2A_set_gps_coord(lon, false);
        telemetryData.value.gpsAltitude = alt / 100;
        manageGpsAltitude();
        if (fix || sats || lat || lon)
        {
          telemetryData.value.gpsFix = 1;
          telemetryData.value.gpsGetFirstData = 0;
        }
      }
      else if (id == AFHDS2A_SENSOR_VOLT_FULL && size >= 10U)
      {
        // EdgeTX expands this group into AFHDS2A IDs 0x03..0x07.
        const uint16_t extV = AFHDS2A_get_u16(data + 0);
        // data + 2 is ID 0x04 average cell voltage. Individual C1..C6 are
        // received separately, so do not overwrite them with the average.
        const uint16_t curr = AFHDS2A_get_u16(data + 4);
        const uint16_t capa = AFHDS2A_get_u16(data + 6);
        const uint16_t rpm  = AFHDS2A_get_u16(data + 8);

        telemetryData.value.vfas = extV / 10;
        if (!telemetryData.value.minVfas || telemetryData.value.vfas < telemetryData.value.minVfas)
          telemetryData.value.minVfas = telemetryData.value.vfas;

        telemetryData.value.current = curr / 10;
        checkOffsetAndMaxCurrent();
        telemetryData.value.currentConsumption = capa;
        telemetryData.value.openXsensor = 1;
        telemetryData.value.rpm = rpm;
        checkMaxRpm();
        AFHDS2A_update_power();
      }
      else if (id == AFHDS2A_SENSOR_ACC_FULL && size >= 12U)
      {
        // -------------------------------------------------------------------
        // AFHDS2A ACC_FULL (0xEF), native 12-byte payload.
        //
        // AccX/Y/Z always remain real accelerometer values and are decoded
        // exactly as in the validated V4 implementation.
        // -------------------------------------------------------------------
        AFHDS2A_set_accel(AFHDS2A_SENSOR_ACC_X, AFHDS2A_get_s16(data + 0));
        AFHDS2A_set_accel(AFHDS2A_SENSOR_ACC_Y, AFHDS2A_get_s16(data + 2));
        AFHDS2A_set_accel(AFHDS2A_SENSOR_ACC_Z, AFHDS2A_get_s16(data + 4));

        // -------------------------------------------------------------------
        // oXs -> OpenAVRc private GPS-time extension.
        //
        // oXs reuses the existing Roll/Pitch/Yaw words ONLY when GPS_TIME is
        // available. ACC_FULL remains exactly 12 bytes, so no extra iBUS
        // sensor and no extra AFHDS2A RF bytes are introduced.
        //
        // Tagged format:
        //
        //   Roll  = 0x48HH   high byte 'H', low byte = UTC hour
        //   Pitch = 0x4DMM   high byte 'M', low byte = UTC minute
        //   Yaw   = 0x53SS   high byte 'S', low byte = UTC second
        //
        // Example:
        //   0x4816 / 0x4D0D / 0x531F = 22:13:31 UTC
        //
        // All three tags and all three ranges must be valid. If not, bytes
        // 6..11 are simply normal Roll/Pitch/Yaw and are ignored here.
        // -------------------------------------------------------------------
        const uint16_t rollWord  = AFHDS2A_get_u16(data + 6);
        const uint16_t pitchWord = AFHDS2A_get_u16(data + 8);
        const uint16_t yawWord   = AFHDS2A_get_u16(data + 10);

        if ((rollWord  & 0xFF00U) == 0x4800U &&   // 'H'
            (pitchWord & 0xFF00U) == 0x4D00U &&   // 'M'
            (yawWord   & 0xFF00U) == 0x5300U)     // 'S'
        {
          const uint8_t gpsHour = (uint8_t)rollWord;
          const uint8_t gpsMin  = (uint8_t)pitchWord;
          const uint8_t gpsSec  = (uint8_t)yawWord;

          if (gpsHour < 24U && gpsMin < 60U && gpsSec < 60U)
          {
            // process_ibus_telem() is called from telemetryInterrupt10ms().
            // Do not call processHubPacket(GPS_HOUR_MIN_ID, ...) here:
            // that path can reach adjustRTChour()/rtcSetTime() from the timer
            // interrupt and previously caused the telemetry screen to crash.
            //
            // This version updates only the OpenAVRc telemetry clock display.
            // Hardware RTC synchronization can be added later from a safe
            // non-interrupt context if required.
            telemetryData.value.sec = gpsSec;
            telemetryData.value.min = gpsMin;

            int16_t localHour = (int16_t)gpsHour + (int16_t)g_eeGeneral.timezone;
            while (localHour < 0)   localHour += 24;
            while (localHour >= 24) localHour -= 24;
            telemetryData.value.hour = (uint8_t)localHour;
          }
        }

        // Normal Roll/Pitch/Yaw data are intentionally not copied anywhere:
        // this legacy OpenAVRc telemetry structure has no fixed fields for
        // these three AFHDS2A attitude values.
      }
      else if (size == 4U)
      {
        const int32_t dataS32 = AFHDS2A_get_s32(data);
        switch (id)
        {
          case AFHDS2A_SENSOR_GPS_LAT:
            AFHDS2A_set_gps_coord(dataS32, true);
            telemetryData.value.gpsFix = 1;
            telemetryData.value.gpsGetFirstData = 0;
            break;

          case AFHDS2A_SENSOR_GPS_LON:
            AFHDS2A_set_gps_coord(dataS32, false);
            telemetryData.value.gpsFix = 1;
            telemetryData.value.gpsGetFirstData = 0;
            break;

          case AFHDS2A_SENSOR_GPS_ALT:
            telemetryData.value.gpsAltitude = dataS32 / 100;
            manageGpsAltitude();
            break;

          case AFHDS2A_SENSOR_ALT:
            telemetryData.value.baroAltitude = dataS32 / 100;
            break;
        }
      }

      sensor += size + 3U;
    }
  }
}

void LoadAFHDS2ATelemBuffer(uint8_t *data)
{
  if (!ibus_telem_buffer[0])
  {
    memcpy(ibus_telem_buffer, data, IBUS_TLM_PACKET_SIZE - IBUS_TLM_HEADER +1);
    return;
  }
}
