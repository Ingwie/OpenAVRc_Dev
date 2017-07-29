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


 /* Formatting octal codes available in TR_ strings:
 *  \037\x           -sets LCD x-coord (x value in octal)
 *  \036             -newline
 *  \035             -horizontal tab (ARM only)
 *  \001 to \034     -extended spacing (value * FW/2)
 *  \0               -ends current string
 */

// NON ZERO TERMINATED STRINGS
#define LEN_OFFON              "\003"
#define TR_OFFON               "OFF""ON\0"

#define LEN_MMMINV             "\003"
#define TR_MMMINV              "---""INV"

#define LEN_NCHANNELS          "\004"
#define TR_NCHANNELS           "\0014CH\0016CH\0018CH10CH12CH14CH16CH"

#define LEN_VBEEPMODE          TR("\005", "\010")
#define TR_VBEEPMODE           TR("Silen""Avvis""Notst""Tutti","Silente ""Avvisi\0 ""No Tasti""Tutti\0  ")

#define LEN_VBEEPLEN           "\005"
#define TR_VBEEPLEN            "+Cort""Corta""Media""Lunga""+Lung"

#define LEN_VRENAVIG           "\003"
#define TR_VRENAVIG            "No REaREb"

#define LEN_VBLMODE            TR("\005", "\006")
#define TR_VBLMODE             TR("OFF\0 ""Tasti""Stks\0""Tutti""ON\0  ", "Spenta""Tasti\0""Sticks""Tutti\0""Accesa")

#define LEN_TRNMODE            "\003"
#define TR_TRNMODE             "off"" +="" :="

#define LEN_TRNCHN             "\003"
#define TR_TRNCHN              "ch1ch2ch3ch4"

#define LEN_UART3MODES         "\016"
#define TR_UART3MODES          "OFF\0          ""Replica S-Port""Telemetria\0   ""SBUS Trainer\0 ""Debug\0"

#define LEN_SWTYPES            "\006"
#define TR_SWTYPES             "None\0 ""Toggle""2POS\0 ""3POS\0"

#define LEN_POTTYPES           "\017"
#define TR_POTTYPES            "None\0          ""Pot. con centro""Inter. Multipos""Potenziometro\0 "

#define LEN_SLIDERTYPES        "\006"
#define TR_SLIDERTYPES         "Niente""Slider"

#define LEN_DATETIME           "\005"
#define TR_DATETIME            "DATA:""ORA :"

#define LEN_VLCD               "\006"
#define TR_VLCD                "NormalOptrex"

#define LEN_VPERSISTENT        "\015"
#define TR_VPERSISTENT         "NO\0          ""Volo\0        ""Reset Manuale"

#define LEN_COUNTRYCODES       TR("\002", "\007")
#define TR_COUNTRYCODES        TR("US""JP""EU", "America""Japan\0 ""Europa\0")

#define LEN_TARANIS_PROTOCOLS  "\004"
#define TR_TARANIS_PROTOCOLS   "OFF\0""PPM\0""XJT\0""DSM2""CRSF"

#define LEN_XJT_PROTOCOLS      "\004"
#define TR_XJT_PROTOCOLS       "OFF\0""D16\0""D8\0 ""LR12"

#define LEN_DSM_PROTOCOLS      "\004"
#define TR_DSM_PROTOCOLS       "LP45""DSM2""DSMX"

#define LEN_VTRIMINC           "\006"
#define TR_VTRIMINC            "Exp   ""ExFine""Fine  ""Medio ""Ampio "

#define LEN_VDISPLAYTRIMS      "\006"
#define TR_VDISPLAYTRIMS       "No\0   ""Cambio""Si\0 "

#define LEN_VBEEPCOUNTDOWN     "\006"
#define TR_VBEEPCOUNTDOWN      "NienteSuoni\0Voce\0 Vibra\0"

#define LEN_VVARIOCENTER       "\006"
#define TR_VVARIOCENTER        "Tono\0 ""Silent"

#define LEN_CURVE_TYPES        "\010"
#define TR_CURVE_TYPES         "Fisso\0  ""Modific."

#define LEN_RETA123            "\001"

#if defined(CPUM2560)
  #define TR_RETA123           "DEMA123ab"
#else
  #define TR_RETA123           "DEMA123"
#endif

#define LEN_VPROTOS            "\006"

#if defined(PXX)
  #define TR_PXX               "PXX\0  "
#elif defined(DSM2) 
  #define TR_PXX               "[PXX]\0"
#else
  #define TR_PXX
#endif

#if defined(DSM2)
  #define TR_DSM2              "LP45\0 ""DSM2\0 ""DSMX\0 "
#else
  #define TR_DSM2
#endif


#if defined(MULTIMODULE)
  #define TR_MMOD                "MultiM"
  #define LEN_MULTIPROTOCOLS    "\006"
  #define TR_MULTIPROTOCOLS     "FlySky""Hubsan""FrSky\0""Hisky\0""V2x2\0 ""DSM\0  ""Devo\0 ""YD717\0""KN\0   ""SymaX\0""SLT\0  ""CX10\0 ""CG023\0""Bayang""ESky\0 ""MT99XX""MJXQ\0 ""Shenqi""FY326\0""SFHSS\0""J6 PRO""FQ777\0""Assan\0""Hontai""OLRS\0 ""FS 2A\0""Q2x2\0 ""Walk.\0""Q303\0 ""GW008\0""DM002\0"
  #define TR_MULTI_CUSTOM       "Custom"
  #define TR_SUBTYPE            "SubType"
  #define TR_MULTI_RFTUNE                TR(INDENT "Freq tune",INDENT "RF Freq. fine tune")
  #define TR_MULTI_TELEMETRY             "Telemetry"
  #define TR_MULTI_VIDFREQ               TR(INDENT "Vid. freq.", INDENT "Video frequency")
  #define TR_MULTI_RFPOWER               INDENT "RF Power"
  #define TR_MULTI_OPTION                TR(INDENT "Option", INDENT "Option value")
  #define TR_MULTI_AUTOBIND              TR(INDENT "Autobind",INDENT "Bind on powerup")
  #define TR_MULTI_DSM_AUTODTECT         TR(INDENT "Autodetect", INDENT "Autodetect format")
  #define TR_MULTI_LOWPOWER              TR(INDENT "Low power", INDENT "Low power mode")
  #define TR_MULTI_SERVOFREQ             TR(INDENT "Servo rate", INDENT "Servo update rate")
  #define TR_RECEIVER_NUM        TR(INDENT "RxNum", INDENT "Receiver No.")
#else
  #define TR_MMOD
#endif

  #define TR_XPPM              "PPM16\0""PPMsim"

#define TR_VPROTOS             "PPM\0  " TR_XPPM TR_PXX TR_DSM2 "SPIMOD" TR_MMOD

#define LEN_POSNEG             "\003"
#define TR_POSNEG              "POS""NEG"


#define LEN_VCURVEFUNC         "\003"
#define TR_VCURVEFUNC          "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_VMLTPX             "\005"
#define TR_VMLTPX              "Add. ""Molt.""Sost."

#define LEN_VMLTPX2            "\002"
#define TR_VMLTPX2             "+=""*="":="

#define LEN_VMIXTRIMS          "\003"
#define TR_VMIXTRIMS           "OFF""ON ""Dir""Ele""Mot""Ale"

  #define TR_CSWTIMER          "Tim\0 "
  #define TR_CSWSTICKY         "Glue\0"
    #define TR_CSWRANGE
    #define TR_CSWSTAY

  #define TR_CSWEQUAL

#define LEN_VCSWFUNC           "\005"
#define TR_VCSWFUNC            "---\0 " TR_CSWEQUAL "a\173x\0 ""a>x\0 ""a<x\0 " TR_CSWRANGE "|a|>x""|a|<x""AND\0 ""OR\0  ""XOR\0 " TR_CSWSTAY "a=b\0 ""a>b\0 ""a<b\0 ""^}x\0 ""|^|}x" TR_CSWTIMER TR_CSWSTICKY

#define LEN_VFSWFUNC           "\015"

#if defined(VARIO)
  #define TR_VVARIO            "Vario\0       "
#else
  #define TR_VVARIO            "[Vario]\0     "
#endif

#if defined(AUDIO)
  #define TR_SOUND             "Suona\0       "
#else
  #define TR_SOUND             "Beep\0        "
#endif

#if defined(HAPTIC)
  #define TR_HAPTIC            "Vibrazione\0  "
#else
  #define TR_HAPTIC            "[Vibrazione]\0"
#endif

#if defined(VOICE)
    #define TR_PLAY_TRACK      "SuonaTraccia\0"
  #define TR_PLAY_BOTH         "Play Both\0   "
  #define TR_PLAY_VALUE        TR("LeggiVal\0 ", "LeggiValore\0 ")
#else
  #define TR_PLAY_TRACK        "[Brano]\0     "
  #define TR_PLAY_BOTH         "[Play Both]\0 "
  #define TR_PLAY_VALUE        "[LeggiValore]"
#endif

#define TR_SF_BG_MUSIC        "Musica Sf\0   ""Musica Sf ||\0"

#if defined(SDCARD)
  #define TR_SDCLOGS           "SDCARD Logs\0 "
#else
  #define TR_SDCLOGS           "[SDCARD Logs]\0"
#endif

#if defined(GVARS)
  #define TR_ADJUST_GVAR       "Regola \0     "
#else
  #define TR_ADJUST_GVAR       "[RegolaVG]\0  "
#endif

  #define TR_SF_PLAY_SCRIPT   "[Lua]\0       "

#if defined(DEBUG)
  #define TR_SF_TEST          "Test\0        "
#else
  #define TR_SF_TEST
#endif

#if   defined(OVERRIDE_CHANNEL_FUNCTION)
  #define TR_SF_SAFETY        "Blocco\0      "
#else
  #define TR_SF_SAFETY        "---\0         "
#endif

  #define TR_SF_SCREENSHOT

#define TR_SF_RESERVE         "[riserva]   \0"

#if defined(PCBMEGA2560)
  #define TR_VFSWFUNC          TR_SF_SAFETY "Maestro \0    ""Trim Instant.""Azzera\0      " TR_ADJUST_GVAR TR_SOUND TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_VVARIO TR_HAPTIC TR_SDCLOGS "Retroillum.\0 " TR_SF_TEST
#else
  #define TR_VFSWFUNC          TR_SF_SAFETY "Maestro \0    ""Trim Instant.""Azzera\0      " TR_ADJUST_GVAR TR_SOUND TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_VVARIO TR_HAPTIC "Retroillum.\0 " TR_SF_TEST
#endif

#define LEN_VFSWRESET          TR("\004", "\011")

#if defined(FRSKY)
  #define TR_FSW_RESET_TELEM   TR("Telm", "Telemetr.")
#else
  #define TR_FSW_RESET_TELEM
#endif

#if ROTARY_ENCODERS == 2
  #define TR_FSW_RESET_ROTENC  TR("REa\0""REb\0", "EncRot A\0""EncRot B\0")
#elif ROTARY_ENCODERS == 1
  #define TR_FSW_RESET_ROTENC  TR("R.E.", "EncRot\0  ")
#else
  #define TR_FSW_RESET_ROTENC
#endif

  #define TR_FSW_RESET_TIMERS  "Tmr1""Tmr2"

#define TR_VFSWRESET           TR(TR_FSW_RESET_TIMERS "All\0" TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC, TR_FSW_RESET_TIMERS "Tutto\0    " TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC)

#define LEN_FUNCSOUNDS         TR("\004", "\006")
#define TR_FUNCSOUNDS          TR("Bp1\0""Bp2\0""Bp3\0""Wrn1""Wrn2""Chee""Rata""Tick""Sirn""Ring""SciF""Robt""Chrp""Tada""Crck""Alrm", "Beep1 ""Beep2 ""Beep3 ""Warn1 ""Warn2 ""Cheep ""Ratata""Tick  ""Siren ""Ring  ""SciFi ""Robot ""Chirp ""Tada  ""Crickt""AlmClk")

#define LEN_VTELEMCHNS         TR("\004", "\005")

  #define TR_TELEM_RESERVE
  #define TR_TELEM_TIME
  #define TR_SWR
  #define TR_RX_BATT
  #define TR_A3_A4
  #define TR_A3_A4_MIN

#define TR_ASPD_MAX            TR("ASp+", "ASpd+")

  #define TR_TELEM_RSSI_RX     TR("Rx\0 ", "Rx\0  ")

  #define TR_TELEM_TIMERS      TR("Tmr1""Tmr2", "Tmr1\0""Tmr2\0")

  #define TR_VTELEMCHNS        "---\0""Batt" TR_TELEM_TIME TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_TIMERS TR_SWR "Tx\0 " TR_TELEM_RSSI_RX TR_RX_BATT "A1\0 ""A2\0 " TR_A3_A4 "Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""Hdg\0""VSpd""ASpd""dTE\0" TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE "A1-\0""A2-\0" TR_A3_A4_MIN "Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""Dst+" TR_ASPD_MAX "Cel-""Cls-""Vfs-""Cur+""Pwr+" TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE "Acc\0""Ora\0"

#define LENGTH_UNIT_IMP        "ft\0"
#define SPEED_UNIT_IMP         "mph"
#define LENGTH_UNIT_METR       "m\0 "
#define SPEED_UNIT_METR        "kmh"

  #if defined(IMPERIAL_UNITS)
    #define LENGTH_UNIT        LENGTH_UNIT_IMP
    #define SPEED_UNIT         SPEED_UNIT_IMP
  #else
    #define LENGTH_UNIT        LENGTH_UNIT_METR
    #define SPEED_UNIT         SPEED_UNIT_METR
  #endif
  #define LEN_VTELEMUNIT       "\003"
  #define TR_VTELEMUNIT        "V\0 ""A\0 ""m/s""-\0 " SPEED_UNIT LENGTH_UNIT "@\0 ""%\0 ""mA\0""mAh""W\0 "

#define STR_V                  (STR_VTELEMUNIT+1)
#define STR_A                  (STR_VTELEMUNIT+4)

#define LEN_VALARM             "\003"
#define TR_VALARM              "---""Gia""Ara""Ros"

#define LEN_VALARMFN           "\001"
#define TR_VALARMFN            "<>"

#define LEN_VTELPROTO          "\007"
#define TR_VTELPROTO           "---\0   ""Hub\0   ""WSHHigh"

  #define LEN_AMPSRC           TR("\003", "\007")
  #define TR_AMPSRC            TR("---""A1\0""A2\0""FAS""Cel", "---\0   ""A1\0    ""A2\0    ""FAS\0   ""Celle\0 ")

#define LEN_VARIOSRC           "\005"
#if defined(FRSKY_SPORT)
  #define TR_VARIOSRC          "Vario""A1\0  ""A2\0"
#else
  #define TR_VARIOSRC          "Alti\0""Alti+""Vario""A1\0  ""A2\0"
#endif

  #define LEN_VTELEMSCREENTYPE   "\006"
  #define TR_VTELEMSCREENTYPE    "Valori""Barre\0"

#define LEN_GPSFORMAT          "\004"
#define TR_GPSFORMAT           "HMS NMEA"

#define LEN2_VTEMPLATES        13
#define LEN_VTEMPLATES         "\015"
#define TR_VTEMPLATES          "Canc. Mixer\0 ""Semplice 4CH\0""Sticky-T-Cut\0""Coda-V      \0""Elevoni\0     ""Delta\0       ""eCCPM\0       ""Setup Heli\0  ""Test Servo\0  "

#define LEN_VSWASHTYPE         "\004"
#define TR_VSWASHTYPE          "---\0""120\0""120X""140\0""90\0"

#define LEN_VKEYS              "\005"
#define TR_VKEYS               TR("Men\201\0""EXIT\0""DN\0  ""UP\0  ""SX\0  ""DX\0  ", "Menu\0""Esci\0""Enter""Pag.\0""Pi\201\0 ""Meno")

#define LEN_VRENCODERS         "\003"
#define TR_VRENCODERS          "REa""REb"

#define LEN_VSWITCHES          "\003"
#define LEN_VSRCRAW            "\004"

#define TR_STICKS_VSRCRAW      TR("Dir\0""Ele\0""Mot\0""Ale\0", "\307Dir""\307Ele""\307Mot""\307Ale")
#define TR_TRIMS_VSRCRAW       TR("TrmR""TrmE""TrmT""TrmA", "\313Dir""\313Ele""\313Mot""\313Ale")
 
  #define TR_POTS_VSRCRAW      "P1\0 ""P2\0 ""P3\0 "
  #define TR_SW_VSRCRAW        "3POS"
  #define TR_9X_3POS_SWITCHES  "ID0""ID1""ID2"

  #define TR_LOGICALSW         "L1\0""L2\0""L3\0""L4\0""L5\0""L6\0""L7\0""L8\0""L9\0""L10""L11""L12"

#define TR_TRIMS_SWITCHES      TR("tRl""tRr""tEd""tEu""tTd""tTu""tAl""tAr", "\313Rl""\313Rr""\313Ed""\313Eu""\313Td""\313Tu""\313Al""\313Ar")

#if defined(PCBMEGA2560)
  #define TR_ROTARY_ENCODERS   "REa\0""REb\0"
  #define TR_ROTENC_SWITCHES   "REa""REb"
#else
  #define TR_ROTARY_ENCODERS
  #define TR_ROTENC_SWITCHES
#endif

  #define TR_PHYS_SWITCHES     "THR""RUD""ELE""AIL""GEA""TRN"

#define TR_ON_ONE_SWITCHES     "ON\0""One"

  #define TR_VSWITCHES         "---" TR_9X_3POS_SWITCHES TR_PHYS_SWITCHES TR_TRIMS_SWITCHES TR_ROTENC_SWITCHES TR_LOGICALSW TR_ON_ONE_SWITCHES

#if defined(HELI)
#define TR_CYC_VSRCRAW         "CYC1""CYC2""CYC3"
#else
#define TR_CYC_VSRCRAW         "[C1]""[C2]""[C3]"
#endif

  #define TR_EXTRA_VSRCRAW

#define TR_VSRCRAW             "---\0" TR_STICKS_VSRCRAW TR_POTS_VSRCRAW TR_ROTARY_ENCODERS "MAX\0" TR_CYC_VSRCRAW TR_TRIMS_VSRCRAW TR_SW_VSRCRAW TR_EXTRA_VSRCRAW

#define LEN_VTMRMODES          "\003"
#define TR_VTMRMODES           "OFF""ABS""MOs""MO%""MOt"

#define LEN_VTRAINERMODES      "\022"
#define TR_VTRAINERMODES       "Maestro/Jack\0     ""Allievo/Jack\0     ""Master/Modulo SBUS""Master/Modulo CPPM""Master/Batteria\0  "

#define LEN_VFAILSAFE          "\013"
#define TR_VFAILSAFE           "Non settato""Mantieni\0  ""Personali\0 ""No impulsi\0""Ricevente\0 "

#if defined(MAVLINK)
  #define LEN_MAVLINK_BAUDS    "\006"
  #define TR_MAVLINK_BAUDS     "4800  ""9600  ""14400 ""19200 ""38400 ""57600 ""76800 ""115200"
  #define LEN_MAVLINK_AC_MODES "\011"
  #define TR_MAVLINK_AC_MODES  "Stabilize""Acro     ""Alt Hold ""Auto     ""Guided   ""Loiter   ""RTL      ""Circle   ""Pos Hold ""Land     ""OF Loiter""Toy A    ""Toy M    ""INVALID  "
  #define LEN_MAVLINK_AP_MODES "\015"
  #define TR_MAVLINK_AP_MODES  "Manual       ""Circle       ""Stabilize    ""Training     ""Fly by Wire A""Fly by Wire A""Auto         ""RTL          ""Loiter       ""Guided       ""Inizializza ""INVALID      "
#endif

#define LEN_VSENSORTYPES        "\012"
#define TR_VSENSORTYPES        "Custom\0   ""Calcolato\0"

#define LEN_VFORMULAS          "\011"
#define TR_VFORMULAS           "Somma\0   ""Media\0   ""Min\0     ""Max\0     ""Moltipl\0 ""Totalizza""Cella\0   ""Consumo\0 ""Distanza\0"

#define LEN_VPREC              "\004"
#define TR_VPREC               "0.--""0.0 ""0.00"

#define LEN_VCELLINDEX         "\010"
#define TR_VCELLINDEX          "Minore\0 ""1\0      ""2\0      ""3\0      ""4\0      ""5\0      ""6\0      ""Maggiore""Delta\0"

// ZERO TERMINATED STRINGS
  #define INDENT               "\001"
  #define LEN_INDENT           1
  #define INDENT_WIDTH         (FW/2)
  #define BREAKSPACE           " "

  #define TR_ENTER             "[Men\201]"

#define TR_EXIT                "[EXIT]"

  #define TR_POPUPS            TR_ENTER "\010" TR_EXIT
  #define OFS_EXIT             sizeof(TR_ENTER)

#define TR_MENUWHENDONE        CENTER "\007" TR_ENTER " Conferma"
#define TR_FREE                "disp"
#define TR_DELETEMODEL         "Elimina modello?"
#define TR_COPYINGMODEL        "Copia in corso.."
#define TR_MOVINGMODEL         "Spostamento..."
#define TR_LOADINGMODEL        "Caricamento..."
#define TR_NAME                "Nome"
#define TR_MODELNAME           "Nome modello"
#define TR_PHASENAME           "Nome fase"
#define TR_MIXNAME             "Nome mix"
#define TR_INPUTNAME           "Nome Ingr."
#define TR_EXPONAME          "Nome Expo"
#define TR_BITMAP              "Immagine"
#define TR_TIMER               TR("Timer","Timer ")
#define TR_ELIMITS             TR("Limiti.E","Limiti Estesi")
#define TR_ETRIMS              TR("Trim Ext","Trim Estesi")
#define TR_TRIMINC             "Passo Trim"
#define TR_DISPLAY_TRIMS       "Mostra Trims"
#define TR_TTRACE              TR("T-Source", INDENT "Sorgente Motore")
#define TR_TTRIM               TR("T-Trim", INDENT "Trim Motore")
#define TR_BEEPCTR             TR("Ctr Beep","Beep al centro")
#define TR_USE_GLOBAL_FUNCS    "Usa Funz Globali"
#define TR_PROTO               TR(INDENT "Proto", INDENT "Protocollo")
  #define TR_PPMFRAME          "PPM frame"
#define TR_MS                  "ms"
#define TR_SWITCH              "Inter."
#define TR_TRIMS               "Trims"
#define TR_FADEIN              "Diss.In"
#define TR_FADEOUT             "Diss.Out"
#define TR_DEFAULT             "(Predefinita)"
#define TR_CHECKTRIMS          CENTER "\006Check\012Trims"
#define OFS_CHECKTRIMS         CENTER_OFS+(9*FW)
#define TR_SWASHTYPE           "Tipo Ciclico"
#define TR_COLLECTIVE          TR("Collettivo","Origine Collettivo")
#define TR_AILERON             "Lateral cyc. source"
#define TR_ELEVATOR            "Long. cyc. source"
#define TR_SWASHRING           "Anello Ciclico"
#define TR_ELEDIRECTION        TR("Direzione ELE","Direzione Long. cyc.")
#define TR_AILDIRECTION        TR("Direzione AIL","Direzione Lateral cyc.")
#define TR_COLDIRECTION        TR("Direzione PIT","Direzione Coll. Passo")
#define TR_MODE                INDENT"Modo"
#define TR_NOFREEEXPO          "Expo pieni!"
#define TR_NOFREEMIXER         "Mixer pieni!"
#define TR_INSERTMIX           "INSER. MIX"
#define TR_EDITMIX             "MOD. MIX"
#define TR_SOURCE              INDENT"Sorg."
#define TR_WEIGHT              "Peso"
#define TR_EXPO                "Espo"
#define TR_SIDE                "Lato"
#define TR_DIFFERENTIAL        "Differ"
#define TR_OFFSET              INDENT"Offset"
#define TR_TRIM                "Trim"
#define TR_DREX                "DRex"
#define DREX_CHBOX_OFFSET      30
#define TR_CURVE               "Curva"
#define TR_FLMODE              "Fase"
#define TR_MIXWARNING          "Avviso"
#define TR_OFF                 "OFF"
#define TR_MULTPX              "MultPx"
#define TR_DELAYDOWN           "Post.Gi\201 "
#define TR_DELAYUP             "Post.S\201"
#define TR_SLOWDOWN            "Rall.Gi\201 "
#define TR_SLOWUP              "Rall.S\201"
#define TR_MIXER               "MIXER"
#define TR_CV                  "CV"
#define TR_GV                  "GV"
#define TR_ACHANNEL            "A\002ingresso"
#define TR_RANGE               TR(INDENT"Inter.", INDENT"Intervallo")
#define TR_CENTER              INDENT "Centro"
#define TR_BAR                 "Barra"
#define TR_ALARM               TR(INDENT"Allar.", INDENT"Allarme")
#define TR_USRDATA             "Dati"
#define TR_BLADES              INDENT"Pale"
#define TR_SCREEN              "Schermo "
#define TR_SOUND_LABEL         "Suono"
#define TR_LENGTH              INDENT"Durata"
#define TR_BEEP_LENGTH         INDENT "Lung. Beep"
#define TR_SPKRPITCH           INDENT"Tono"
#define TR_HAPTIC_LABEL        "Vibraz"
#define TR_HAPTICSTRENGTH      INDENT"Forza"
#define TR_CONTRAST            "Contrasto"
#define TR_ALARMS_LABEL        "Allarmi"
#define TR_BATTERY_RANGE       TR("Interv. Batt", "Intervallo batteria")
#define TR_BATTERYWARNING      INDENT"Avviso batteria"
#define TR_INACTIVITYALARM     INDENT"Inattivita'"
#define TR_MEMORYWARNING       INDENT"Avviso memoria"
#define TR_ALARMWARNING        INDENT"Spegni suono"
#define TR_RENAVIG             "Navig EncRot "
#define TR_THROTTLE_LABEL      "Motore"
#define TR_THROTTLEREVERSE     TR("Mot inv.", INDENT "Motore Inverso")
#define TR_TIMER_NAME          INDENT "Nome"
#define TR_MINUTEBEEP          INDENT"Minuto"
#define TR_BEEPCOUNTDOWN       TR(INDENT"Conto rov", INDENT"Conto rovescia")
#define TR_PERSISTENT          TR(INDENT"Persist.", INDENT"Persistente")
#define TR_BACKLIGHT_LABEL     TR("Retroillum.", "Retroilluminazione")
#define TR_BLDELAY             INDENT"Durata"
#define TR_BLONBRIGHTNESS      TR(INDENT"Lumin. ON",INDENT"Luminosit\200 ON")
#define TR_BLOFFBRIGHTNESS     TR(INDENT"Lumin. OFF",INDENT"Luminosit\200 OFF")
#define TR_BLCOLOR             INDENT "Colore"
#define TR_SPLASHSCREEN        TR("Schermo avvio", "Schermata di avvio")
#define TR_THROTTLEWARNING     TR("All. Mot.", INDENT "Allarme Motore")
#define TR_SWITCHWARNING       TR("Avv. Int.", INDENT "Avviso Interr.")
#define TR_POTWARNING          TR("Avv. Pot.", INDENT "Avviso Pot.")
#define TR_TIMEZONE            "Ora locale"
#define TR_ADJUST_RTC          INDENT "Aggiusta RTC"
#define TR_GPS                 "GPS"
#define TR_RXCHANNELORD        "Ordine ch RX"
#define TR_STICKS              "Sticks"
#define TR_POTS                "Pot."
#define TR_SWITCHES            "Interutt."
#define TR_SWITCHES_DELAY      "Ritardo voce FV"
#define TR_SLAVE               "Allievo"
#define TR_MODESRC             "Modo\006% Origine"
#define TR_MULTIPLIER          "Moltiplica"
#define TR_CAL                 "Cal"
#define TR_VTRIM               "Trim- +"
#define TR_BG                  "BG:"
  #define TR_MENUTOSTART         CENTER"\011" TR_ENTER " per Cal."
  #define TR_SETMIDPOINT         CENTER"\012SETTA CENTRO"
  #define TR_MOVESTICKSPOTS      CENTER"\010MUOVI STICK/POT"
#define TR_RXBATT              "Batt Rx:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              CENTER"DATI ASSENTI"
#define TR_TOTTM1TM2THRTHP     "\037\146SES\036TM1\037\146TM2\036THR\037\146TH%"
#define TR_TMR1LATMAXUS        "Tmr1Lat max\037\124us"
#define STR_US (STR_TMR1LATMAXUS+13)
#define TR_TMR1LATMINUS        "Tmr1Lat min\037\124us"
#define TR_TMR1JITTERUS        "Tmr1 Jitter\037\124us"

  #define TR_TMIXMAXMS         "Tmix max\037\124ms"
  #define TR_FREESTACKMINB     "Free Stack\037\124b"

#define TR_MENUTORESET         CENTER TR_ENTER" x Azzerare"
#define TR_PPM_TRAINER         "TR"
#define TR_CH                  "CH"
#define TR_MODEL               "MODELLO"
#define TR_FP                  "FV"
#define TR_MIX                 "MIX"
#define TR_EEPROMLOWMEM        "EEPROM quasi piena!"
#define TR_ALERT               "\016ALERT"
#define TR_PRESSANYKEYTOSKIP   "\010Premi un tasto"
#define TR_THROTTLENOTIDLE     TR("Motore non in posiz.","Motore non in posizione")
#define TR_ALARMSDISABLED      "Avvisi Disattivati!"
#define TR_PRESSANYKEY         "\010Premi un tasto"
#define TR_BADEEPROMDATA       "Dati corrotti!"
#define TR_EEPROMFORMATTING    "Formatto EEPROM..."
#define TR_EEPROMOVERFLOW      "EEPROM Piena"
#define TR_MENURADIOSETUP      "CONFIGURATX"
#define TR_MENUDATEANDTIME     "DATA E ORA"
#define TR_MENUTRAINER         "MAESTRO/ALLIEVO"
#define TR_MENUGLOBALFUNCS     "FUNZIONI GLOBALI"
#define TR_MENUVERSION         "VERSIONE"
#define TR_MENUDIAG            "DIAG"
#define TR_MENUANA             "ANAS"
#define TR_MENUCALIBRATION     "CALIBRAZIONE"
  #define TR_TRIMS2OFFSETS     "\006Trim  => Offset "
#define TR_MENUMODELSEL        "MODELLI"
#define TR_MENUSETUP           "CONFIGURA"
#define TR_MENUFLIGHTPHASE     "FASE DI VOLO"
#define TR_MENUFLIGHTPHASES    "FASI DI VOLO"
#define TR_MENUHELISETUP       "CONFIGURA ELI"

#define TR_MENUINPUTS        "DR/ESPO"
#define TR_MENULIMITS        "LIMITI"

#define TR_MENUCURVES          "CURVE"
#define TR_MENUCURVE           "CURVA"
#define TR_MENULOGICALSWITCH   "INTER. PERSON."
#define TR_MENULOGICALSWITCHES TR("INTER. LOGICI","INTERRUTTORI LOGICI")
#define TR_MENUCUSTOMFUNC      TR("FUNZ. SPECIALI","FUNZIONI SPECIALI")
#define TR_MENUCUSTOMSCRIPTS   "SCRIPTS UTENTE"
#define TR_MENUCUSTOMSCRIPT    "SCRIPT UTENTE"
#define TR_MENUTELEMETRY       "TELEMETRIA"
#define TR_MENUTEMPLATES       "ESEMPI GUIDA"
#define TR_MENUSTAT            "STATO"
#define TR_MENUDEBUG           "DEBUG"
  #define TR_RXNUM             "RxNum"
#define TR_SYNCMENU            "[Sync]"
#define TR_LIMIT               INDENT "Limiti"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Latitud."
#define TR_LONGITUDE           "Longitud."
#define TR_GPSCOORD            TR("GPS Coords", INDENT "Formato Coordinate")
#define TR_VARIO               TR("Vario", "Variometro")
#define TR_PITCH_AT_ZERO       INDENT "Tono a Zero"
#define TR_PITCH_AT_MAX        INDENT "Tono al Max"
#define TR_REPEAT_AT_ZERO      INDENT "Ripeti a Zero"
#define TR_POWEROFF            "\027Power OFF..."
#define TR_SHUTDOWN            "ARRESTO.."
#define TR_SAVEMODEL           "Salvataggio dati modello"
#define TR_BATT_CALIB          "Calibra batt."
#define TR_CURRENT_CALIB       "Calibra corr."
#define TR_VOLTAGE             TR(INDENT "Voltagg.",INDENT "Voltaggio")
#define TR_CURRENT             TR(INDENT "Corrente",INDENT "Corrente")
#define TR_SELECT_MODEL        "Scegli Memo."
#define TR_CREATE_MODEL        "Crea Modello"
#define TR_COPY_MODEL          "Copia Modello"
#define TR_MOVE_MODEL          "Sposta Modello"
#define TR_BACKUP_MODEL        "Salva Modello"
#define TR_DELETE_MODEL        TR("Elim. Modello","Elimina Modello") // TODO merged into DELETEMODEL?
#define TR_RESTORE_MODEL       TR("Ripr. Modello","Ripristina Modello")
#define TR_SDCARD_ERROR        "Errore SD"
#define TR_NO_SDCARD           "No SDCARD"
#define TR_SDCARD_FULL         "SD Card Piena"
#define TR_INCOMPATIBLE        "Incompatibile"
#define TR_WARNING             "AVVISO"
#define TR_EEPROMWARN          "EEPROM"
#define TR_EEPROM_CONVERTING   "Conversione EEPROM"
#define TR_THROTTLEWARN        "MOTORE"
#define TR_ALARMSWARN          "ALLARMI"
#define TR_SWITCHWARN          "CONTROLLI"
#define TR_FAILSAFEWARN        "FAILSAFE"
#define TR_NO_FAILSAFE         "Failsafe non settato"
#define TR_KEYSTUCK            "Tasto bloccato"
#define TR_INVERT_THR          "Inverti Mot?"
#define TR_SPEAKER_VOLUME      "Volume Audio"
#define TR_LCD                 "LCD"
#define TR_BRIGHTNESS          INDENT "Luminosit\200"
#define TR_CPU_TEMP            "Temp CPU \016>"
#define TR_CPU_CURRENT         "Corrente\022>"
#define TR_CPU_MAH             "Consumo"
#define TR_COPROC              "CoProc."
#define TR_COPROC_TEMP         "Temp. MB \016>"
#define TR_CAPAWARNING         INDENT "Capacit\200Low"
#define TR_TEMPWARNING         INDENT "Temp. Alta"
#define TR_FUNC                "Funz"
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Durata"
#define TR_DELAY               "Ritardo"
#define TR_SD_CARD             "SD Card"
#define TR_SDHC_CARD           "SD-HC Card"
#define TR_NO_SOUNDS_ON_SD     "No Suoni su SD"
#define TR_NO_MODELS_ON_SD     "No Model. su SD"
#define TR_NO_BITMAPS_ON_SD    "No Immag. su SD"
#define TR_NO_SCRIPTS_ON_SD    "No Scripts su SD"
#define TR_SCRIPT_SYNTAX_ERROR "Script errore sintassi"
#define TR_SCRIPT_PANIC        "Script panic"
#define TR_SCRIPT_KILLED       "Script fermato"
#define TR_SCRIPT_ERROR        "Errore sconosciuto"
#define TR_PLAY_FILE           "Suona"
#define TR_DELETE_FILE         "Elimina"
#define TR_COPY_FILE           "Copia"
#define TR_RENAME_FILE         "Rinomomina"
#define TR_ASSIGN_BITMAP       "Assegna Immagine"
#define TR_EXECUTE_FILE        "Esegui"
#define TR_REMOVED             " rimosso"
#define TR_SD_INFO             "Informazioni"
#define TR_SD_FORMAT           "Format"
#define TR_NA                  "N/A"
#define TR_HARDWARE            "HARDWARE"
#define TR_FORMATTING          "Formattazione."
#define TR_TEMP_CALIB          "Temp. Calib"
#define TR_TIME                "Ora"
#define TR_BAUDRATE            "BT Baudrate"
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Tipo:"
#define TR_SD_SPEED            "Veloc.:"
#define TR_SD_SECTORS          "Settori:"
#define TR_SD_SIZE             "Dimens:"
#define TR_TYPE                INDENT "Tipo"
#define TR_GLOBAL_VARS         "Variabili Globali"
#define TR_GLOBAL_V            "V.GLOBALI"
#define TR_GLOBAL_VAR          "Variabile globale"
#define TR_MENUGLOBALVARS      "VARIABILI GLOBALI"
#define TR_OWN                 "Fase"
#define TR_DATE                "Data"
#define TR_ROTARY_ENCODER      "R.Encs"
#define TR_CHANNELS_MONITOR    "MONITOR CANALI"
#define TR_MIXERS_MONITOR      "MONITOR MIXER"
#define TR_PATH_TOO_LONG       "Path troppo lungo"
#define TR_VIEW_TEXT           "Vedi testo"
#define TR_FLASH_BOOTLOADER    "Flash BootLoader"
#define TR_FLASH_EXTERNAL_DEVICE "Progr. Dispositivo Esterno"
#define TR_FLASH_INTERNAL_MODULE "Progr. Modulo Interno"
#define TR_WRITING             "\032Scrivendo..."
#define TR_CONFIRM_FORMAT      "Confermi Format?"
#define TR_INTERNALRF          "Modulo Interno"
#define TR_EXTERNALRF          "Modulo esterno"
#define TR_FAILSAFE            "Modo failsafe"
#define TR_FAILSAFESET         TR("FAILSAFE","IMPOSTAZIONI FAILSAFE")
#define TR_MENUSENSOR          "SENSOR"
#define TR_COUNTRYCODE         TR("Codice Paese","Standard 2.4Ghz")
#define TR_VOICELANG           "Lingua Voce"
#define TR_UNITSSYSTEM         "Unit\200"
#define TR_EDIT                "Modifica"
#define TR_INSERT_BEFORE       "Inserisci prima"
#define TR_INSERT_AFTER        "Inserisci dopo"
#define TR_COPY                "Copia"
#define TR_MOVE                "Sposta"
#define TR_PASTE               "Incolla"
#define TR_DELETE              "Elimina"
#define TR_INSERT              TR("Inser.","Inserisci")
#define TR_RESET_FLIGHT        "Azzera volo"
#define TR_RESET_TIMER1        "Azzera Timer1"
#define TR_RESET_TIMER2        "Azzera Timer2"
#define TR_RESET_TIMER3        "Azzera Timer3"
#define TR_RESET_TELEMETRY     "Azzera Telemetria"
#define TR_STATISTICS          "Statistiche"
#define TR_ABOUT_US            "Info su"
#define TR_AND_SWITCH          "Inter. AND"
#define TR_SF                  "CF"
#define TR_GF                  "GF"
#define TR_SPEAKER             INDENT"Speaker"
#define TR_BUZZER              INDENT"Buzzer"
#define TR_BYTES               "Bytes"
#define TR_MODULE_BIND         "[Bind]"
#define TR_MODULE_RANGE        "[Range]"
#define TR_RESET_BTN           "[Reset]"
#define TR_SET                 "[Set]"
#define TR_TRAINER             TR("Trainer","Maestro/Allievo")
#define TR_ANTENNAPROBLEM      CENTER "Problemi antenna TX!"
#define TR_MODELIDUSED         TR("ID gi\200 usato","ID Modello gi\200 usato")
#define TR_MODULE              INDENT "Modulo"
#define TR_TELEMETRY_TYPE      "Tipo Telemetria"
#define TR_TELEMETRY_SENSORS   "Sensori"
#define TR_VALUE               "Valore"
#define TR_TOPLCDTIMER         "Timer LCD Sup."
#define TR_UNIT                "Unita"
#define TR_TELEMETRY_NEWSENSOR INDENT "Aggiungi nuovo sensore..."
#define TR_CHANNELRANGE        INDENT "Numero Canali"
#define TR_LOWALARM            INDENT "Allarme Basso"
#define TR_CRITICALALARM       INDENT "Allarme Critico"
#define TR_ENABLE_POPUP        "Abilita Popup"
#define TR_DISABLE_POPUP       "Disabilita Popup"
#define TR_CURVE_PRESET        "Preimpostate..."
#define TR_PRESET              "Preimpostate"
#define TR_MIRROR              "Mirror"
#define TR_CLEAR               "Cancella"
#define TR_RESET               "Azzera"
#define TR_RESET_SUBMENU       "Azzera..."
#define TR_COUNT               "Punti"
#define TR_PT                  "pt"
#define TR_PTS                 "pti"
#define TR_SMOOTH              "Smussa"
#define TR_COPY_STICKS_TO_OFS  "Copia Stick su Offset"
#define TR_COPY_TRIMS_TO_OFS   "Copia Trim in Subtrim"
#define TR_INCDEC              "Inc/Decrementa"
#define TR_GLOBALVAR           "Var Globale"
#define TR_MIXSOURCE           "Sorgente Mixer"
#define TR_CONSTANT            "Constante"
#define TR_PERSISTENT_MAH      INDENT "Memo mAh"
#define TR_PREFLIGHT           "Controlli Prevolo"
#define TR_CHECKLIST           INDENT "Mostra Checklist"
#define TR_FAS_OFFSET          TR(INDENT "FAS Ofs", INDENT "FAS Offset")
#define TR_UART3MODE           "Porta Seriale"
#define TR_SCRIPT              "Script"
#define TR_INPUTS              "Ingresso"
#define TR_OUTPUTS             "Uscita"
#define TR_EEBACKUP            "\004[ENTER Lungo] per copiare la EEPROM"
#define TR_FACTORYRESET        "\012[MENU Lungo]: Inizializza dati"
#define TR_CONFIRMRESET        "Resettare TUTTI i dati?"
#define TR_TO_MANY_LUA_SCRIPTS "Troppi Scripts Lua!"

#if defined(MAVLINK)
  #define TR_MAVLINK_RC_RSSI_SCALE_LABEL  "Max RSSI"
  #define TR_MAVLINK_PC_RSSI_EN_LABEL     "PC RSSI EN"
  #define TR_MAVMENUSETUP_TITLE           "Setta Mavlink"
  #define TR_MAVLINK_BAUD_LABEL           "Baudrate"
  #define TR_MAVLINK_INFOS                "INFOS"
  #define TR_MAVLINK_MODE                 "MODO"
  #define TR_MAVLINK_CUR_MODE             "Modo Corrente"
  #define TR_MAVLINK_ARMED                "Armato"
  #define TR_MAVLINK_BAT_MENU_TITLE       "BAT RSSI"
  #define TR_MAVLINK_BATTERY_LABEL        "Stato batteria di volo"
  #define TR_MAVLINK_RC_RSSI_LABEL        "RC RSSI"
  #define TR_MAVLINK_PC_RSSI_LABEL        "PC RSSI"
  #define TR_MAVLINK_NAV_MENU_TITLE       "NAV"
  #define TR_MAVLINK_COURSE               "Course"
  #define TR_MAVLINK_HEADING              "Heading"
  #define TR_MAVLINK_BEARING              "Bearing"
  #define TR_MAVLINK_ALTITUDE             "Altitudine"
  #define TR_MAVLINK_GPS                  "GPS"
  #define TR_MAVLINK_NO_FIX               "NO Fix"
  #define TR_MAVLINK_SAT                  "SAT"
  #define TR_MAVLINK_HDOP                 "HDOP"
  #define TR_MAVLINK_LAT                  "LAT"
  #define TR_MAVLINK_LON                  "LON"
#endif

// Taranis column headers
#define TR_PHASES_HEADERS      { " Nome ", " Inter. ", " Trim Deriva ", " Trim Elevator ", " Trim Motore ", " Trim Alettoni ", " Disolv. In ", " Dissolv. Out " }
#define TR_LIMITS_HEADERS      { " Nome ", " Offset ", " Min ", " Max ", " Direzione ", " Curve ", " Centro PPM ", " Simmetria " }
#define TR_CSW_HEADERS         { "   Funzione ", "   V1 ", "   V2 ", "   V2 ", "   Inter. AND ", "   Durata ", "   Ritardo " }

// About screen
#define TR_ABOUTUS             TR(" INFO ", "INFO SU")

#define TR_ABOUT_OpenAVRc_1      TR("OpenAVRc\001e'\001open\001source,\001non", "OpenAVRc e' open source, non-")
#define TR_ABOUT_OpenAVRc_2      TR("commercial,\001wo\001warranties.", "commerciale, fornito senza")
#define TR_ABOUT_OpenAVRc_3      TR("It\001was\001developed\001for\001free.", "garanzie. E' stato sviluppato")
#define TR_ABOUT_OpenAVRc_4      TR("Support through donations", "gratuitamente. Il supporto")
#define TR_ABOUT_OpenAVRc_5      TR("is welcome!", "mediante donazioni e' gradito!")

#define TR_ABOUT_BERTRAND_1    "Bertrand Songis"
#define TR_ABOUT_BERTRAND_2    "Autore principale OpenAVRc"
#define TR_ABOUT_BERTRAND_3    "Co-autore Companion9x"

#define TR_ABOUT_MIKE_1        "Mike Blandford"
#define TR_ABOUT_MIKE_2        "Guru codice e driver"
#define TR_ABOUT_MIKE_3        "esperto a basso livello"
#define TR_ABOUT_MIKE_4        "Ispiratore"

#define TR_ABOUT_ROMOLO_1      "Romolo Manfredini"
#define TR_ABOUT_ROMOLO_2      "Autore Companion9x"
#define TR_ABOUT_ROMOLO_3      ""

#define TR_ABOUT_ANDRE_1       "Andre Bernet"
#define TR_ABOUT_ANDRE_2       "Funzionalit\200, usabilit\200,"
#define TR_ABOUT_ANDRE_3       "debugging, documentazione"

#define TR_ABOUT_ROB_1         "Rob Thomson"
#define TR_ABOUT_ROB_2         "Webmaster openrcforums"

#define TR_ABOUT_KJELL_1       "Kjell Kernen"
#define TR_ABOUT_KJELL_2       "www.open-tx.org main author"
#define TR_ABOUT_KJELL_3       "OpenAVRc Recorder author"
#define TR_ABOUT_KJELL_4       "Companion contributor"

#define TR_ABOUT_MARTIN_1      "Martin Hotar"
#define TR_ABOUT_MARTIN_2      "Graphic Designer"

  #define TR_ABOUT_HARDWARE_1  "Brent Nelson"
  #define TR_ABOUT_HARDWARE_2  "Designer/Produttore SKY9X"
  #define TR_ABOUT_HARDWARE_3  ""

#define TR_ABOUT_PARENTS_1     "Progetti correlati"
#define TR_ABOUT_PARENTS_2     "ersky9x (Mike Blandford)"
#define TR_ABOUT_PARENTS_3     "ER9X (Erez Raviv)"
#define TR_ABOUT_PARENTS_4     "TH9X (Thomas Husterer)"

#define TR_CHR_SHORT  's'
#define TR_CHR_LONG   'l'
#define TR_CHR_TOGGLE 't'
#define TR_CHR_HOUR   'h'
#define TR_CHR_INPUT  'I'   // Values between A-I will work

#define TR_BEEP_VOLUME         "Volume Beep"
#define TR_WAV_VOLUME          "Volume Wav"
#define TR_VARIO_VOLUME        "Volume Vario"
#define TR_BG_VOLUME           "Volume Sf"

#define TR_TOP_BAR             "Barra sup."
#define TR_ALTITUDE            INDENT "Altitudine"
#define TR_SCALE               "Scala"
#define TR_VIEW_CHANNELS       "Vedi Canali"
#define TR_VIEW_NOTES          "Vedi Note"
#define TR_MODS_FORBIDDEN      "Modifica proibita!"
#define TR_UNLOCKED            "Sbloccato"
#define TR_ID                  "ID"
#define TR_PRECISION           "Precisione"
#define TR_RATIO               "Ratio"
#define TR_FORMULA             "Formula"
#define TR_CELLINDEX           "Indice cella"
#define TR_LOGS                "Logs"
#define TR_OPTIONS             "Opzioni"

#define TR_ALTSENSOR           "Sensore Alt"
#define TR_CELLSENSOR          "Sensore Cell"
#define TR_GPSSENSOR           "Sensore GPS"
#define TR_CURRENTSENSOR       "Sensore"
#define TR_AUTOOFFSET          "Auto Offset"
#define TR_ONLYPOSITIVE        "Positivo"
#define TR_FILTER              "Filtro"
#define TR_TELEMETRYFULL       "Tutti gli slot sono pieni!"
//TODO: translation
#define TR_INVERTED_SERIAL     INDENT "Invert"
#define TR_IGNORE_INSTANCE     TR(INDENT "No inst.","Ignora instanza")
#define TR_DISCOVER_SENSORS    INDENT "Cerca nuovi sensori"
#define TR_STOP_DISCOVER_SENSORS INDENT "Ferma ricerca.."
#define TR_DELETE_ALL_SENSORS  INDENT "Elimina tutti i sensori"
#define TR_CONFIRMDELETE       "Confermi eliminazione?"

#define TR_MENU_INPUTS         "\314Ingressi"
#define TR_MENU_LUA            "\322Lua scripts"
#define TR_MENU_STICKS         "\307Sticks"
#define TR_MENU_POTS           "\310Pots"
#define TR_MENU_MAX            "\315MAX"
#define TR_MENU_HELI           "\316Ciclico"
#define TR_MENU_TRIMS          "\313Trims"
#define TR_MENU_SWITCHES       "\312Interrut."
#define TR_MENU_LOGICAL_SWITCHES "\312Interrut. Logici"
#define TR_MENU_TRAINER        "\317Trainer"
#define TR_MENU_CHANNELS       "\320Canali"
#define TR_MENU_GVARS          "\311GVars"
#define TR_MENU_TELEMETRY      "\321Telemetry"
#define TR_MENU_OTHER          "Altro"
#define TR_MENU_INVERT         "Inverti"

#define ZSTR_RSSI              "\022\023\023\011"
#define ZSTR_SWR               "\023\027\022"
#define ZSTR_A1                "\001\034"
#define ZSTR_A2                "\001\035"
#define ZSTR_A3                "\001\036"
#define ZSTR_A4                "\001\037"
#define ZSTR_BATT              "\022\350\002\354"
#define ZSTR_ALT               "\001\364\354"
#define ZSTR_TEMP1             "\024\363\360\034"
#define ZSTR_TEMP2             "\024\363\360\035"
#define ZSTR_RPM               "\022\020\015"
#define ZSTR_FUEL              "\006\353\373\364"
#define ZSTR_VSPD              "\026\023\360\374"
#define ZSTR_ACCX              "\001\375\375\030"
#define ZSTR_ACCY              "\001\375\375\031"
#define ZSTR_ACCZ              "\001\375\375\032"
#define ZSTR_CURR              "\003\353\356\356"
#define ZSTR_VFAS              "\026\006\001\023"
#define ZSTR_ASPD              "\001\023\360\374"
#define ZSTR_GSPD              "\007\023\360\374"
#define ZSTR_HDG               "\010\374\371"
#define ZSTR_CELLS             "\003\373\364\355"
#define ZSTR_GPSALT            "\007\001\364\354"
#define ZSTR_GPSDATETIME       "\004\377\354\377"
#define ZSTR_GPS               "\007\020\023"
#define ZSTR_BATT1_VOLTAGE     "\022\002\034\026"
#define ZSTR_BATT2_VOLTAGE     "\022\002\035\026"
#define ZSTR_BATT1_CURRENT     "\022\002\034\001"
#define ZSTR_BATT2_CURRENT     "\022\002\035\001"
#define ZSTR_BATT1_CONSUMPTION "\022\002\034\003"
#define ZSTR_BATT2_CONSUMPTION "\022\002\035\003"
#define ZSTR_RX1_FAILSAFE      "\022\350\034\006"
#define ZSTR_RX1_LOSTFRAME     "\022\350\034\014"
#define ZSTR_RX1_CONN_LOST     "\022\350\034\003"
#define ZSTR_RX1_NO_SIGNAL     "\022\350\034\023"
#define ZSTR_RX2_FAILSAFE      "\022\350\035\006"
#define ZSTR_RX2_LOSTFRAME     "\022\350\035\014"
#define ZSTR_RX2_CONN_LOST     "\022\350\035\003"
#define ZSTR_RX2_NO_SIGNAL     "\022\350\035\023"
