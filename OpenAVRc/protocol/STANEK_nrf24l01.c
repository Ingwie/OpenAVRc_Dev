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
  **************************************************************************************************
  Support for DIY receivers from this repository:
  https://github.com/stanekTM/RX_nRF24_Stanek
  
  The "Stanek" protocol with the nRF24L01+ transceiver is included.
  Telemetry monitors receiver voltage A1(A2) and "fake" RSSI.
  The nRF24L01+ transceiver does not contain real RSSI and is only a rough counting of lost packets.
  
  - Setting the number of control channels 2 to 13ch.
  - Fail-Safe flag in bind option "bnd".
  **************************************************************************************************
*/

#include "../OpenAVRc.h"

// Jedinečná adresa (5 bajtové číslo nebo znak)
const uint8_t STANEK_TX_RX_Address[6] = "jirka";

// Výchozí RF kanál (odpovídá kmitočtu 2,476 GHz (0 až 125))
#define STANEK_RF_CHANNEL  76

// Délka jednoho vysílacího/přijímacího okna v mikrosekundách
#define STANEK_PACKET_PERIOD  3000

// Definice "pulses2MHz" znovupoužitelných 8/16 bitových hodnot (max. 13 bajtů).
// Zbývá ještě rezerva 2 bajty pro případné budoucí funkce (např. Frekvenční přeskakování)
#define STANEK_RF_CHANNEL_P2M     BYTE_P2M(1) // Ukládá aktuální frekvenční kanál rádia
#define STANEK_TSSI_COUNTER_P2M   BYTE_P2M(2) // Počítadlo úspěšně přijatých telemetrických paketů
#define STANEK_PACKET_SIZE_P2M    BYTE_P2M(3) // Vypočítaná velikost aktuálně odesílaného paketu
#define STANEK_TX_RX_TOGGLE_P2M   BYTE_P2M(4) // Přepínač fází vysílání a příjmu
#define STANEK_MIXER_COUNTER_P2M  BYTE_P2M(5) // Předdělička pro spouštění mixéru poloh serv v pevných intervalech

#define STANEK_TSSI_FIXED_COUNTER_16_P2M  WORD_P2M(1) // Pevný dělič cyklů pro pravidelný výpočet TSSI (250 cyklů)
#define STANEK_GET_ELAPSED_TIME_16_P2M    WORD_P2M(2) // Ukládá přesný systémový čas spotřebovaný výpočty procesoru
#define STANEK_FS_COUNTER_16_P2M          WORD_P2M(3) // Časový odpočet pro automatické ukončení zápisu Fail-Safe

// Konfigurace pro Menu
const static RfOptionSettingsvar_t RfOpt_STANEK_Ser[] PROGMEM =
{
  /* rfProtoNeed */ PROTO_NEED_SPI, // Protokol potřebuje SPI sběrnici (příklad dalších možností ... | BOOL1USED | BOOL2USED | BOOL3USED)
  /* rfSubTypeMax */             0, // Protokol nemá žádné další sub-protokoly
  /* rfOptionValue1Min */        2, // Minimální povolený počet RC kanálů
  /* rfOptionValue1Max */       13, // Maximální povolený počet RC kanálů
  /* rfOptionValue2Min */        0, // Nevyužito
  /* rfOptionValue2Max */        0, // Nevyužito
  /* rfOptionValue3Max */        3, // Volba RF výkonu (0 až 3): 0 = -18 dBm, 1 = -12 dBm, 2 = -6 dBm, 3 = 0 dBm
};

// Počet RC kanálů pro Menu (2 až 13 kanálů)
#define STANEK_NUM_RC_CHANNELS  g_model.rfOptionValue1

//**********************************************************************************************************************************
// STANEK_init Inicializace RF modulu
//**********************************************************************************************************************************
static void STANEK_init()
{
  STANEK_RF_CHANNEL_P2M = STANEK_RF_CHANNEL; // Inicializace RF kanálu
  
  // 1. HARDWAROVÝ RESET (uvede čip do předem známého výchozího stavu)
  NRF24L01_Initialize();
  
  // 2. VYČIŠTĚNÍ PAMĚTI (vymazat případné staré nebo poškozené pakety z FIFO front po resetu)
  NRF24L01_FlushTx(); // Vymazat vysílací vyrovnávací paměť
  NRF24L01_FlushRx(); // Vymazat přijímací vyrovnávací paměť
  
  // 3. ZÁPIS ADRES A AKTIVACE KANÁLŮ
  // Zápis 5bajtové adresy pro vysílání a pro příjem Pipe 0 (Pipe 0 je nutná pro zachycení zpětné telemetrie)
  NRF24L01_WriteRegisterMulti(NRF24L01_10_TX_ADDR,    (uint8_t*)(&STANEK_TX_RX_Address), 5);
  NRF24L01_WriteRegisterMulti(NRF24L01_0A_RX_ADDR_P0, (uint8_t*)(&STANEK_TX_RX_Address), 5);
  NRF24L01_WriteReg(NRF24L01_02_EN_RXADDR, 0x01); // Povolit pouze datový kanál Pipe 0 (pro příjem dat)
  NRF24L01_WriteReg(NRF24L01_03_SETUP_AW, 0x03);  // 5 bajtová šířka adresy
  
  // 4. PROTOKOLÁRNÍ KONFIGURACE (řízení přenosových vlastností)
  NRF24L01_WriteReg(NRF24L01_01_EN_AA, 0x00);      // Vypnuto hardwarové Auto-ACK (řízení času přebírá software)
  NRF24L01_WriteReg(NRF24L01_04_SETUP_RETR, 0x00); // Vypnuto automatické opakování paketů (zajišťuje nulovou latenci)
  NRF24L01_SetBitrate(NRF24L01_BR_250K);           // Nastavení nejnižší rychlosti 250 Kbps pro dosažení maximálního dosahu
  
  // 5. ODEMČENÍ A ZÁPIS POKROČILÝCH FUNKCÍ (dynamická délka dat)
  NRF24L01_Activate(0x73);                      // 1. Odemknout přístup k pokročilým registrům (DYNPD, FEATURE)
  NRF24L01_WriteReg(NRF24L01_1C_DYNPD, 0x01);   // Povolení příjmu paketů s proměnlivou (dynamickou) délkou pouze pro Pipe 0
  NRF24L01_WriteReg(NRF24L01_1D_FEATURE, 0x04); // Zapnout pouze dynamickou délku paketů
  NRF24L01_Activate(0x73);                      // 2. Zamknout registry pro zajištění stability (DYNPD, FEATURE). Zakomentovat v případě klonu
  
  // 6. VYČIŠTĚNÍ STAVOVÝCH HISTORIÍ
  NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70); // Vymazat všechny stavové flagy (RX_DR, TX_DS, MAX_RT)
  
  // 7. FINÁLNÍ SPUŠTĚNÍ MODULU
  NRF24L01_ManagePower();      // Zapnout napájení a zápis RF výkonu (nastaví RF_SETUP)
  NRF24L01_SetTxRxMode(TX_EN); // Zapnout vysílací režim
}

//**********************************************************************************************************************************
// STANEK_get_telemetry - Asynchronní příjem zpětných dat z modelu a dorovnávání časového jitteru
//**********************************************************************************************************************************
FORCEINLINE void STANEK_get_telemetry()
{
  // Výpočet TSSI vysílače (úspěšnost doručení zpětných paketů) jednou za 250 cyklů
  if (++STANEK_TSSI_FIXED_COUNTER_16_P2M > 250)
  {
#if defined(FRSKY)
    uint16_t tssi_boosted = ((uint32_t)STANEK_TSSI_COUNTER_P2M * 69) / 10; // Koeficient navýšen na 6.9 (69 / 10), aby TSSI dosáhlo maxima 255
    
    if (tssi_boosted > 255) tssi_boosted = 255; // Upraveno na plné hardwarové maximum 8-bitového bajtu (0 až 255)
    
    telemetryData.rssi[1].set(tssi_boosted); // Zápis do telemetrického indexu 1 (TSSI vysílače)
#endif
    STANEK_TSSI_COUNTER_P2M = 0;          // Vynulování počítadla úspěšných paketů pro nové měření
    STANEK_TSSI_FIXED_COUNTER_16_P2M = 0; // Vynulování hlavního děliče cyklů
  }
  
  // Pokud dorazil telemetrický paket od přijímače
  if (NRF24L01_NOP() & _BV(NRF24L01_07_RX_DR))
  {
    NRF24L01_ReadPayload(telem_save_data_buff, 3); // Vyčteme 3 bajty telemetrie
#if defined(FRSKY)
    // Řízení stavu a resetování časovačů (timeoutů) telemetrického toku dat
    if (frskyStreaming) frskyStreaming = FRSKY_TIMEOUT10ms; // Pokud streamování už běželo, resetuj provozní timeout (10ms)
    else frskyStreaming = FRSKY_TIMEOUT_FIRST;              // Pokud streamování teprve začíná, nastav startovací timeout
    
    // 1.bajt: Síla signálu (RSSI) na straně modelu (0 až 255)
    telemetryData.rssi[0].set(telem_save_data_buff[0]);
    // 2.bajt: Analogový senzor A1 (např. napětí baterie) (0 až 255)
    telemetryData.analog[TELEM_ANA_A1].set(telem_save_data_buff[1], g_model.telemetry.channels[TELEM_ANA_A1].type);
    // 3.bajt: Analogový senzor A2 (např. druhé napětí nebo teplota) (0 až 255)
    telemetryData.analog[TELEM_ANA_A2].set(telem_save_data_buff[2], g_model.telemetry.channels[TELEM_ANA_A2].type);
#endif
    STANEK_TSSI_COUNTER_P2M++; // Navýšení počtu úspěšně přijatých datových balíčků pro budoucí výpočet TSSI
  }
  else
  {
    // Pokud nebyl přijat žádný telemetrický paket, je zpoždění odpovídající typické době zpracování telemetrického paketu.
    // To se provádí proto, aby se udrželo konzistentnější načasování procesu STANEK_send_packet, protože čtení dat SPI nějakou dobu trvá
    _delay_us(50);
  }
  
  NRF24L01_SetTxRxMode(TX_EN); // Okamžitě vypni příjem a zapni vysílání
  NRF24L01_FlushRx();          // Vymaž vyrovnávací paměť příjmu. Jelikož rádio již nenaslouchá, buffer zůstane čistý pro příští cyklus
}

//**********************************************************************************************************************************
// STANEK_send_packet - Sestavení datových struktur z kniplů vysílače, ořezání výchylek a odeslání balíčku pro serva
//**********************************************************************************************************************************
static void STANEK_send_packet()
{
  STANEK_get_telemetry(); // Zpracování telemetrie z minulého kola a bleskové přepnutí rádia do vysílacího režimu před výpočty
  
  int16_t hold_value;
  uint8_t payload_index = 0;
  
  // Plnění RC kanálů začíná přesně od indexu 1
  // Na indexu 0 (příznak Fail-Safe) zůstává hodnota (0 nebo 1) zapsaná ze STANEK_Cmds
  for (uint8_t i = 0; i < STANEK_NUM_RC_CHANNELS; i++)
  {
    hold_value = (FULL_CHANNEL_OUTPUTS(i)) / 2;          // Rozsah ±1024 / 2 a hodnota se zmenší na rozsah ±512
    hold_value += PPM_CENTER;                            // Přičte se středová konstanta (1500)
    hold_value = limit<int16_t>(1000, hold_value, 2000); // Přepočet výchylek páček (1000 až 2000 µs)
    
    packet_p2M[1 + payload_index] = hold_value & 0xFF; // Spodní (Low) bajt
    payload_index++;
    packet_p2M[1 + payload_index] = hold_value >> 8;   // Horní (High) bajt
    payload_index++;
  }
  
  // Velikost paketu: 2 bajty na kanál + 1 bajt (příznak Fail-Safe) na indexu 0
  STANEK_PACKET_SIZE_P2M = (STANEK_NUM_RC_CHANNELS * 2) + 1;
  
  NRF24L01_WritePayload(packet_p2M, STANEK_PACKET_SIZE_P2M);   // Zápis sestavených dat (poloh páček a příznaků)
  NRF24L01_WriteReg(NRF24L01_05_RF_CH, STANEK_RF_CHANNEL_P2M); // Nastavení frekvenčního kanálu
  NRF24L01_ManagePower(); // Zapnout napájení a zápis RF výkonu (nastaví RF_SETUP)
}

//**************************************************************************************************************** ******************
// STANEK_manage_time - Výpočet a dynamické vyvažování délky časových oken pro stabilní šíření signálu bez jitteru
//**************************************************************************************************************** ******************
static uint16_t STANEK_manage_time()
{
  uint16_t packet_period;
  
  // Vypočítá se čas, jak dlouho poletí paket vzduchem na základě jeho velikosti
  // Matematika nRF24L01 při rychlosti 250 Kbps: 1 bajt trvá 32 µs (4 * 8). Pevná režie čipu (preambule, adresa, CRC) trvá 432 µs
  uint16_t rx_delay = /* Čas přenosu dat vzduchem */ (4 * 8 * STANEK_PACKET_SIZE_P2M) + /* Pevná režie transceiveru */ 432; // 592 až 1296 us
  
  // Vysílání: Rádio odešle paket a okamžitě se přepne do režimu příjmu, kde čeká na telemetrickou odpověď z modelu
  if (!STANEK_TX_RX_TOGGLE_P2M)
  {
    STANEK_GET_ELAPSED_TIME_16_P2M = PROTOCOL_GetElapsedTime(); // Získat uplynulý čas
    packet_period = rx_delay + STANEK_GET_ELAPSED_TIME_16_P2M;  // Výsledná délka časového okna vyhrazená pro vysílací fázi
    STANEK_TX_RX_TOGGLE_P2M = 1; // Nastavení příznaku pro přepnutí do fáze příjmu telemetrie v příštím kroku
  }
  else
  {
    packet_period = 100 + ((STANEK_NUM_RC_CHANNELS - 2) * 50); // Začínáme výchozí hodnoty 100 µs (pro 2 kanály) a za každý kanál přičteme 50 µs
    packet_period += STANEK_PACKET_PERIOD; // Přičtení k základní časové periodě (3 ms)
    packet_period -= rx_delay + STANEK_GET_ELAPSED_TIME_16_P2M; // Odečtení času, který byl již spotřebován ve fázi vysílání
    
    STANEK_TX_RX_TOGGLE_P2M = 0; // Následně se příznak shodí na 0 a v příštím cyklu se začne znovu od fáze vysílání
  }
  
  return packet_period; // Vrácení vypočítané periody
}

//**********************************************************************************************************************************
// STANEK_cb - Pravidelný hardwarový callback volaný operačním systémem vysílače
//**********************************************************************************************************************************
static uint16_t STANEK_cb()
{
  // Pro čtení telemetrie musíme přepnout do režimu příjmu
  if (STANEK_TX_RX_TOGGLE_P2M)
  {
    NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x7F); // Přepnutí do režimu příjmu telemetrie s vypnutým IRQ a zapnutým 16-bit CRC
  }
  else
  {
    // Spuštění mixéru v přesně definovaných intervalech 12 ms (každý 4. průchod při 3 ms periodě)
    if (++STANEK_MIXER_COUNTER_P2M >= 4)
    {
      STANEK_MIXER_COUNTER_P2M = 0;    // Reset předděličky
      SCHEDULE_MIXER_END_IN_US(12000); // Naplánujte další výpočty mixéru
    }
    
    // Pokud je aktivní Fail-Safe (1), odpočítáváme 3 sekundy. Jelikož vysíláme ob cyklus na 3 ms periodě (každých 6 ms jeden paket)
    if (packet_p2M[0] == 1)
    {
      // Hodnota 500 průchodů odpovídá přesně délce trvání 3 sekund
      if (++STANEK_FS_COUNTER_16_P2M >= 500) 
      {
        packet_p2M[0] = 0;            // Vypnutí příznaku po 3 sekundách
        STANEK_FS_COUNTER_16_P2M = 0; // Vynulování počítadla
        s_editMode = 0;               // Resetovat tlačítko vazby
      }
    }
    else
    {
      STANEK_FS_COUNTER_16_P2M = 0; // Pokud Fail-Safe zápis neběží, čítač se trvale nuluje
    }
    
    STANEK_send_packet(); // Odeslání aktuálního paketu k modelu
  }
  
  uint16_t protocol_period = STANEK_manage_time(); // Výpočet bezpečné periody
  
  heartbeat |= HEART_TIMER_PULSES; // Potvrzení aktivity vysílače pro systémový hlídač (Watchdog)
  
  CALCULATE_LAT_JIT(); // Měření latence (zpoždění) a kolísání signálu (jitter) pro diagnostické zobrazení na displeji
  
  return protocol_period * 2; // Násobení dvěma a vrácení vypočítané periody pro stoprocentní kompatibilitu s OpenAVRc schedulerem
}

//**********************************************************************************************************************************
// STANEK_Cmds - Rozhraní pro Menu a RF modul
//**********************************************************************************************************************************
const void *STANEK_Cmds(enum ProtoCmds cmd)
{
  switch(cmd)
  {
    // Spouští se při zapnutí vysílače nebo výběru modelu
    case PROTOCMD_INIT:
      packet_p2M[0] = 0; // Nastavení výchozího stavu paketu (index 0 = 0, běžný přenos řízení)
      STANEK_init();     // Spuštění kompletní inicializace nRF24L01 registrů
      PROTO_Start_Callback(STANEK_cb); // Pravidelný hardwarový callback
      return 0;
    
    // Bezpečně zastaví rádio a vypne časovač při vypínání nebo změně modelu
    case PROTOCMD_RESET:
      PROTO_Stop_Callback(); // Okamžité zastavení periodického callbacku
      NRF24L01_Reset();      // Uvedení nRF24L01 modulu do hlubokého spánku (Power Down režim)
      return 0;
    
    // Vyvolá uložení Fail-Safe hodnot v modelu. Aktivuje se stiskem tlačítka BIND (bnd) v Menu vysílače
    case PROTOCMD_BIND:
      STANEK_init();                   // Spuštění kompletní inicializace nRF24L01 registrů
      STANEK_FS_COUNTER_16_P2M = 0;    // Vynulování počítadla při startu
      packet_p2M[0] = 1;               // Jednorázový příznak pro zápis Fail-Safe vyvolaný z menu na 3 sekundy
      PROTO_Start_Callback(STANEK_cb); // Pravidelný hardwarový callback
      return 0;
    
    // Funkce pro grafické Menu, jaké položky má zobrazit v nastavení protokolu
    case PROTOCMD_GETOPTIONS:
      SetRfOptionSettings(pgm_get_far_address(RfOpt_STANEK_Ser),
      STR_DUMMY,   // Sub-protokol:    Skryt (tento protokol nemá žádné sub-ptotokoly)
      STR_NUMCH,   // Option 1  (int): Volba počtu RC kanálů (2 až 13)
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

