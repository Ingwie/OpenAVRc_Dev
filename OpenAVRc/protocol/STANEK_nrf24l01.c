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
***********************************************************************************************
Simple RF protocol with frequency hopping (FHSS) and two-way telemetry (Auto-ACK Payload).
***********************************************************************************************
Rapid hopping between 4 strong RF channels (12, 40, 76, 88) outside the main WiFi frequencies.
Transmission rate of 1 Mbps (increased for full support of the newer nRF52 generation).
Constant latency / period of 4.5 ms (222 Hz).
The received telemetry displays the A1(A2) voltage and RSSI.
RC channel settings from 2 to 14.
By selecting the "bnd" bind option in the model settings menu, we save the failsafe servo
positions into the receiver.
Support for DIY receivers from this repository: https://github.com/stanekTM/RX_nRF24_FHSS
***********************************************************************************************
*/

#include "../OpenAVRc.h"

// Unikátní RF adresa max. 5 bajtů (číslo nebo znak). Musí být stejná i v přijímači
const uint8_t STANEK_RF_ADDRESS[] = "jirka";

// 4 silné RF kanály (mimo hlavní WiFi frekvence)
const uint8_t STANEK_RF_CHANNELS[] = {12, 40, 76, 88};

// Délka jednoho vysílacího / přijímacího okna v mikrosekundách
#define STANEK_PACKET_PERIOD  4500 // Pevná odesílací / přijímací perioda 4.5 ms (222 Hz)

// Definice "pulses2MHz" znovupoužitelných 8/16 bitových hodnot (max. 13 bajtů)
#define STANEK_FHSS_INDEX_P2M      BYTE_P2M(1) // Ukazatel na aktuálně používaný RF kanál z tabulky
#define STANEK_PACKET_COUNTER_P2M  BYTE_P2M(2) // Počítadlo úspěšně přijatých paketů
#define STANEK_PACKET_SIZE_P2M     BYTE_P2M(3) // Vypočítaná velikost aktuálně odesílaného paketu

#define STANEK_TSSI_COUNTER_16_P2M  WORD_P2M(1) // Předdělička cyklů pro pravidelný výpočet TSSI
#define STANEK_FS_COUNTER_16_P2M    WORD_P2M(2) // Počítadlo pro automatické ukončení zápisu Fail-Safe

// Konfigurace pro Menu RC vysílače
const static RfOptionSettingsvar_t RfOpt_STANEK_Ser[] PROGMEM =
{
  /* rfProtoNeed */ PROTO_NEED_SPI, // Protokol potřebuje SPI sběrnici (příklad dalších možností ... | BOOL1USED | BOOL2USED | BOOL3USED)
  /* rfSubTypeMax */             0, // Protokol nemá žádné další sub-protokoly
  /* rfOptionValue1Min */        2, // Minimální povolený počet RC kanálů
  /* rfOptionValue1Max */       14, // Maximální povolený počet RC kanálů
  /* rfOptionValue2Min */        0, // Nevyužito
  /* rfOptionValue2Max */        0, // Nevyužito
  /* rfOptionValue3Max */        3, // Volba RF výkonu (0 až 3): 0 = -18 dBm, 1 = -12 dBm, 2 = -6 dBm, 3 = 0 dBm
};

// Počet RC kanálů pro Menu (2 až 14 kanálů)
#define STANEK_NUM_RC_CHANNELS  g_model.rfOptionValue1

// Prototypy všech funkcí pro správné sestavení kompilátorem
static void STANEK_RF_init();
FORCEINLINE void STANEK_get_telemetry();
static void STANEK_send_packet();
static uint16_t STANEK_cb();
const void *STANEK_Cmds(enum ProtoCmds cmd);

//**********************************************************************************************************************************
// Kompletní inicializace registrů nRF24L01+
//**********************************************************************************************************************************
static void STANEK_RF_init()
{
  // 1. Hardwarový reset (uvede čip do předem známého výchozího stavu)
  NRF24L01_Initialize();
  
  // 2. Vyčistit případné staré nebo poškozené pakety z FIFO front po resetu
  NRF24L01_FlushTx(); // Vyčistit vysílací vyrovnávací paměť (TX FIFO)
  NRF24L01_FlushRx(); // Vyčistit přijímací vyrovnávací paměť (RX FIFO)
  
  // 3. Zápis adres a aktivace datových kanálů (Pipe 0 je nutná pro zachycení zpětné telemetrie)
  NRF24L01_WriteRegisterMulti(NRF24L01_10_TX_ADDR,    (uint8_t*)(&STANEK_RF_ADDRESS), 5);
  NRF24L01_WriteRegisterMulti(NRF24L01_0A_RX_ADDR_P0, (uint8_t*)(&STANEK_RF_ADDRESS), 5);
  NRF24L01_WriteReg(NRF24L01_02_EN_RXADDR, 0x01); // Povolit pouze datový kanál Pipe 0 (pro příjem dat)
  NRF24L01_WriteReg(NRF24L01_03_SETUP_AW, 0x03); // Šířka adresy 5 bajtů
  
  // 4. Protokolární konfigurace (řízení přenosových vlastností)
  NRF24L01_WriteReg(NRF24L01_01_EN_AA, 0x01); // Zapnuto hardwarové Auto-ACK na Pipe 0 pro příjem potvrzení a telemetrie
  NRF24L01_WriteReg(NRF24L01_04_SETUP_RETR, 0x00); // Vypnuto automatické opakování paketů (zajišťuje nulovou latenci)
  NRF24L01_SetBitrate(NRF24L01_BR_1M); // Nastavení střední rychlosti 1 Mbps (250 kbps nRF52832 nepodporuje)
  NRF24L01_WriteReg(NRF24L01_05_RF_CH, STANEK_RF_CHANNELS[STANEK_FHSS_INDEX_P2M]); // Nastavení startovního RF kanálu z FHSS tabulky
  
  // 5. Odemčení a zápis pokročilých funkcí (dynamická délka dat a ACK payload)
  NRF24L01_Activate(0x73); // 1. Odemknout přístup k pokročilým registrům (DYNPD, FEATURE)
  NRF24L01_WriteReg(NRF24L01_1C_DYNPD, 0x01); // Povolení příjmu paketů s proměnlivou (dynamickou) délkou pouze pro Pipe 0
  NRF24L01_WriteReg(NRF24L01_1D_FEATURE, 0x06); // Bit 0x04 zapíná Dynamic Payload, bit 0x02 zapíná funkci ACK Payload (přibalování telemetrie)
  NRF24L01_Activate(0x73); // 2. Zamknout registry pro zajištění stability (DYNPD, FEATURE). Zakomentovat v případě klonu
  
  // 6. Vyčistit všechny stavové flagy (RX_DR, TX_DS, MAX_RT)
  NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);
  
  // 7. Finální spuštění modulu
  NRF24L01_ManagePower(); // Zapnout napájení a zápis RF výkonu (nastaví RF_SETUP)
  NRF24L01_SetTxRxMode(TX_EN); // Zapnout vysílací režim
}

//**********************************************************************************************************************************
// Asynchronní příjem zpětných dat z modelu a vyhodnocování kvality signálu linky
//**********************************************************************************************************************************
FORCEINLINE void STANEK_get_telemetry()
{
  // Měření TSSI: Okno pro 100 očekávaných paketů jednou za 300 cyklů
  if (++STANEK_TSSI_COUNTER_16_P2M > 300)
  {
   #if defined(FRSKY)
    // Pojistka proti přetečení
    if (STANEK_PACKET_COUNTER_P2M > 100) STANEK_PACKET_COUNTER_P2M = 100;
    
    // Počet paketů (0-100) je přímo výsledné procento
    telemetryData.rssi[1].set(STANEK_PACKET_COUNTER_P2M); // Zápis TSSI (kvality odesílání) do telemetrie zpětné vazby OpenAVRc
   #endif
    
    STANEK_TSSI_COUNTER_16_P2M = 0; // Vynulování děliče cyklů
    STANEK_PACKET_COUNTER_P2M = 0; // Vynulování počítadla úspěšně přijatých paketů
  }
  
  // Vyčteme aktuální stav registru STATUS
  uint8_t status_reg = NRF24L01_ReadReg(NRF24L01_07_STATUS);
  
  // Pokud je nastaven bit 6 (RX_DR), znamená, že v rámci Auto-ACK dorazil telemetrický paket od přijímače
  if (status_reg & (1 << 6))
  {
    NRF24L01_ReadPayload(telem_save_data_buff, 3); // Vyčteme 3 bajty telemetrie z RX FIFO paměti čipu
    
   #if defined(FRSKY)
    // Řízení stavu a resetování časovačů (timeoutů) telemetrického toku dat
    if (frskyStreaming) frskyStreaming = FRSKY_TIMEOUT10ms; // Pokud streamování už běželo, resetuj provozní timeout (10 ms)
    else frskyStreaming = FRSKY_TIMEOUT_FIRST; // Pokud streamování teprve začíná, nastav startovací timeout
    
    // 1.bajt: Síla signálu (RSSI) na straně přijímače (0 až 255)
    telemetryData.rssi[0].set(telem_save_data_buff[0]);
    // 2.bajt: Analogový senzor A1 napětí baterie (0 až 255)
    telemetryData.analog[TELEM_ANA_A1].set(telem_save_data_buff[1], g_model.telemetry.channels[TELEM_ANA_A1].type);
    // 3.bajt: Analogový senzor A2 napětí baterie (0 až 255)
    telemetryData.analog[TELEM_ANA_A2].set(telem_save_data_buff[2], g_model.telemetry.channels[TELEM_ANA_A2].type);
   #endif
    
    STANEK_PACKET_COUNTER_P2M++; // Navýšení počtu přijatých paketů do statistiky TSSI
  }
  
  NRF24L01_SetTxRxMode(TX_EN); // Okamžitě vypni příjem a zapni vysílání
  NRF24L01_FlushRx(); // Vyčistit přijímací vyrovnávací paměť (RX FIFO) pro čistý start příštího kola
}

//**********************************************************************************************************************************
// Sestavení dat z kniplů RC vysílače, ořezání výchylek a odeslání paketu řízení
//**********************************************************************************************************************************
static void STANEK_send_packet()
{
  // Okamžitě na začátku cyklu stáhneme telemetrická data z minulého kola
  STANEK_get_telemetry();
  
  // Pokud je aktivní příznak Fail-Safe na indexu 0, odpočítáváme 3 sekundy
  if (packet_p2M[0] == 1)
  {
    // 667 průchodů při 4.5ms periodě odpovídá délce trvání 3 sekund
    if (++STANEK_FS_COUNTER_16_P2M >= 667)
    {
      packet_p2M[0] = 0; // Vypnutí příznaku Fail-Safe na indexu 0 po 3 sekundách
      STANEK_FS_COUNTER_16_P2M = 0; // Vynulování počítadla
      s_editMode = 0; // Resetovat volbu párování "bnd"
    }
  }
  else STANEK_FS_COUNTER_16_P2M = 0; // Pokud Fail-Safe zápis neběží, čítač se trvale nuluje
  
  // Sestavení payloadu (2 bajty na jeden RC kanál). Na indexu 0 je příznak Fail-Safe
  uint8_t payload_index = 1; // Plnění RC kanálů začíná přesně od indexu 1
  
  for (uint8_t i = 0; i < STANEK_NUM_RC_CHANNELS; i++)
  {
    int16_t hold_value = (FULL_CHANNEL_OUTPUTS(i)) / 2; // Rozsah ±1024 / 2 a hodnota se zmenší na rozsah ±512
    hold_value += PPM_CENTER; // Přičte se středová konstanta (1500)
    hold_value = limit<int16_t>(1000, hold_value, 2000); // Konverze výchylek páček na limitovaný rozsah (1000 až 2000 us)
    
    packet_p2M[payload_index++] = hold_value & 0xFF; // Spodní (Low) bajt
    packet_p2M[payload_index++] = hold_value >> 8;   // Horní (High) bajt
  }
  
  // Velikost paketu: 1 bajt (příznak Fail-Safe) na indexu 0 + 2 bajty na RC kanál
  STANEK_PACKET_SIZE_P2M = 1 + (STANEK_NUM_RC_CHANNELS * 2);
  
  // 1. Synchronní skok na další RF kanál
  STANEK_FHSS_INDEX_P2M = (STANEK_FHSS_INDEX_P2M + 1) % 4; // Posun na další index v tabulce
  NRF24L01_WriteReg(NRF24L01_05_RF_CH, STANEK_RF_CHANNELS[STANEK_FHSS_INDEX_P2M]); // Nastavení RF kanálu na novou frekvenci
  
  // 2. Vyčistit stavové flagy a vyprázdnit paměť před novým zápisem
  NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70); // Vyčistit všechny předchozí stavové flagy (TX_DS, MAX_RT, RX_DR)
  NRF24L01_FlushTx(); // Vyčistit vysílací vyrovnávací paměť (TX FIFO)
  
  // 3. Zapnout napájení a zápis RF výkonu (nastaví RF_SETUP)
  NRF24L01_ManagePower();
  
  // 4. Zápis sestavených dat
  NRF24L01_WritePayload(packet_p2M, STANEK_PACKET_SIZE_P2M);
  
  // 5. Trvalé vynucení vysílacího režimu (PWR_UP, PRIM_TX, 2B CRC)
  NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x7E);
}

//**********************************************************************************************************************************
// Pravidelný hardwarový callback volaný přerušením Timeru 1
//**********************************************************************************************************************************
static uint16_t STANEK_cb()
{
  SCHEDULE_MIXER_END_IN_US(STANEK_PACKET_PERIOD); // Synchronizace hodin mixéru s rádiem před odesláním dat

  STANEK_send_packet(); // Odeslání aktuálního paketu k modelu
  
  heartbeat |= HEART_TIMER_PULSES; // Potvrzení aktivity vysílače pro systémový hlídač (Watchdog)
  
  CALCULATE_LAT_JIT(); // Měření latence (zpoždění) a kolísání signálu (jitter) pro diagnostické zobrazení na displeji
  
  return STANEK_PACKET_PERIOD * 2; // Plánování příštího přerušení časovače: V OpenAVRc trvá 1 tik přesně 0.5 us, proto násobíme dvěma
}

//**********************************************************************************************************************************
// Rozhraní pro Menu RC vysílače
//**********************************************************************************************************************************
const void *STANEK_Cmds(enum ProtoCmds cmd)
{
  switch(cmd)
  {
    // Spouští se při zapnutí RC vysílače nebo výběru modelu
    case PROTOCMD_INIT:
      // Vynulujeme indexy v RAM unii pro čistý start
      STANEK_FHSS_INDEX_P2M = 0;
      STANEK_TSSI_COUNTER_16_P2M = 0;
      STANEK_PACKET_COUNTER_P2M = 0;
      STANEK_FS_COUNTER_16_P2M = 0;
      
      packet_p2M[0] = 0; // Nastavení (vypnutí) výchozího příznaku Fail-Safe na indexu 0
      
      STANEK_RF_init(); // Kompletní inicializace registrů nRF24L01+
      PROTO_Start_Callback(STANEK_cb); // Spuštění pravidelného hardwarového přerušení (callbacku)
      return 0;
    
    // Bezpečně zastaví rádio a vypne časovač při vypínání nebo změně modelu
    case PROTOCMD_RESET:
      PROTO_Stop_Callback(); // Okamžité zastavení pravidelného hardwarového přerušení (callbacku)
      NRF24L01_Reset(); // Uvedení nRF24L01+ modulu do hlubokého spánku (Power Down režim)
      return 0;
    
    // Vyvolá uložení Fail-Safe hodnot v RC modelu. Aktivuje se volbou párování "bnd" v Menu RC vysílače
    case PROTOCMD_BIND:
      STANEK_FS_COUNTER_16_P2M = 0; // Vynulování počítadla pro nový odpočet
      
      packet_p2M[0] = 1; // Jednorázový příznak pro zápis Fail-Safe na indexu 0
      
      STANEK_RF_init(); // Kompletní inicializace registrů nRF24L01+
      PROTO_Start_Callback(STANEK_cb); // Spuštění pravidelného hardwarového přerušení (callbacku)
      return 0;
    
    // Funkce pro grafické Menu, jaké položky má zobrazit v nastavení protokolu
    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_STANEK_Ser),
      STR_DUMMY,   // Sub-protokol:    Skryt (tento protokol nemá žádné sub-ptotokoly)
      STR_NUMCH,   // Option 1  (int): Volba počtu RC kanálů (2 až 14)
      STR_DUMMY,   // Option 2  (int): Skryta (nevyužito)
      STR_RFPOWER, // Option 3 (uint): Volba RF výkonu (0 až 3): 0 = -18 dBm, 1 = -12 dBm, 2 = -6 dBm, 3 = 0 dBm
      STR_DUMMY,   // OptionBool 1:    Skryta (nevyužito)
      STR_DUMMY,   // OptionBool 2:    Skryta (nevyužito)
      STR_DUMMY);  // OptionBool 3:    Skryta (nevyužito)
      return 0;
    
    default:
    break;
  }
  return 0;
}
