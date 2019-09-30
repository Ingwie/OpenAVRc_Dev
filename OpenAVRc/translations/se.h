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
#define TR_OFFON               "Av ""P\200\0"

#define LEN_MMMINV             "\003"
#define TR_MMMINV              "---""INV"

#define LEN_NCHANNELS          "\004"
#define TR_NCHANNELS           "\0014KN\0016KN\0018KN10KN12KN14KN16KN"

#define LEN_VBEEPMODE          "\005"
#define TR_VBEEPMODE           "Tyst\0""Alarm""EjKnp""Alla\0"

#define LEN_VBEEPLEN           "\005"
#define TR_VBEEPLEN            "0====""=0===""==0==""===0=""====0"

#define LEN_VRENAVIG           "\003"
#define TR_VRENAVIG            "Ingen REaREb"

#define LEN_VBLMODE            "\005"
#define TR_VBLMODE             "Av\0""Knapp""Spak\0""B\200da\0""P\200\0  "

#define LEN_TRNMODE            "\002"
#define TR_TRNMODE             "Av""+="":="

#define LEN_TRNCHN             "\003"
#define TR_TRNCHN              "KN1KN2KN3KN4"

#define LEN_UART3MODES         "\022"
#define TR_UART3MODES          "Av\0               ""Spegling av S-Port""Telemetri\0        ""SBUS Trainer\0     ""Debug\0"

#define LEN_SWTYPES            "\006"
#define TR_SWTYPES             "Ingen\0""Flipp\0""2Pos\0 ""3Pos\0"

#define LEN_POTTYPES           "\020"
#define TR_POTTYPES            "Ingen\0          ""Mittl\201ges-pot\0  ""Flerl\201gesv\201ljare""Potentiometer\0"

#define LEN_SLIDERTYPES        "\008"
#define TR_SLIDERTYPES         "Ingen\0""Skjutpot"

#define LEN_DATETIME           "\005"
#define TR_DATETIME            "DAG: ""TID: "

#define LEN_VLCD               "\006"
#define TR_VLCD                "NormalOptrex"

#define LEN_VPERSISTENT        "\014"
#define TR_VPERSISTENT         "AV\0         ""Flygning\0   ""Nolla Sj\201lv\0"

#define LEN_COUNTRYCODES       "\002"
#define TR_COUNTRYCODES        "US""JP""EU"

#define LEN_TARANIS_PROTOCOLS  "\004"
#define TR_TARANIS_PROTOCOLS   "Av\0 ""PPM\0""XJT\0""DSM2""CRSF"

#define LEN_XJT_PROTOCOLS      "\004"
#define TR_XJT_PROTOCOLS       "Av\0 ""D16\0""D8\0 ""LR12"

#define LEN_DSM_PROTOCOLS      "\004"
#define TR_DSM_PROTOCOLS       "LP45""DSM2""DSMX"

#define LEN_VTRIMINC           "\006"
#define TR_VTRIMINC            "Expo\0 ""xFin\0 ""Fin\0  ""Medium""Grov\0 "

#define LEN_VDISPLAYTRIMS      "\006"
#define TR_VDISPLAYTRIMS       "No\0   ""Change""Yes\0"

#define LEN_VBEEPCOUNTDOWN     "\007"
#define TR_VBEEPCOUNTDOWN      "Tyst\0  Pip\0   R\202st\0  Vibrera"

#define LEN_VVARIOCENTER       "\006"
#define TR_VVARIOCENTER        "Tone\0 ""Silent"

#define LEN_CURVE_TYPES        "\010"
#define TR_CURVE_TYPES         "Normal\0 ""Special\0"

#define LEN_RETA123            "\001"

  #define TR_RETA123           "RHGS123ab"

#define LEN_VPROTOS            "\006"

#if defined(PXX)
  #define TR_PXX               "PXX\0  "
#elif defined(DSM2_SERIAL)
  #define TR_PXX               "[PXX]\0"
#else
  #define TR_PXX
#endif

#if defined(DSM2_SERIAL)
  #define TR_DSM2              "LP45\0 ""DSM2\0 ""DSMX\0 "
#else
  #define TR_DSM2
#endif

#if defined(SPIMODULES)
   #define TR_SPIM              "SPIRfMod"
   #define TR_RFTUNECOARSE      INDENT "Freq.coarse"
   #define TR_RFPOWER INDENT    "RF Power"
#endif

#if defined(MULTIMODULE)
  #define LEN_MULTIPROTOCOLS    "\006"
  #define TR_MULTIPROTOCOLS     "FlySky""Hubsan""FrSky\0""Hisky\0""V2x2\0 ""DSM\0  ""Devo\0 ""YD717\0""KN\0   ""SymaX\0""SLT\0  ""CX10\0 ""CG023\0""Bayang""ESky\0 ""MT99XX""MJXQ\0 ""Shenqi""FY326\0""SFHSS\0""J6 PRO""FQ777\0""Assan\0""Hontai""OLRS\0 ""FS 2A\0""Q2x2\0 ""Walk.\0""Q303\0 ""GW008\0""DM002\0"
  #define TR_MULTI_CUSTOM       "Custom"
#endif

#if defined(MULTIMODULE) || defined(SPIMODULES)
  #define TR_SUBTYPE            "SubType"
  #define TR_RFTUNEFINE         INDENT "Freq.fine"
  #define TR_TELEMETRY             "Telemetry"
  #define TR_MULTI_VIDFREQ               INDENT "Vid. freq."
  #define TR_MULTI_RFPOWER               INDENT "RF Power"
  #define TR_MULTI_OPTION                INDENT "Option"
  #define TR_AUTOBIND              INDENT "Autobind"
  #define TR_MULTI_DSM_AUTODTECT         INDENT "Autodetect"
  #define TR_MULTI_LOWPOWER              INDENT "Low power"
  #define TR_MULTI_SERVOFREQ             INDENT "Servo rate"

#endif





#define LEN_POSNEG             "\003"
#define TR_POSNEG              "Pos""Neg"


#define LEN_VCURVEFUNC         "\003"
#define TR_VCURVEFUNC          "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_VMLTPX             "\010"
#define TR_VMLTPX              "Addera  ""F\202rst\201rk""Ers\201tt  "

#define LEN_VMLTPX2            "\002"
#define TR_VMLTPX2             "+=""*="":="

#define LEN_VMIXTRIMS          "\003"
#define TR_VMIXTRIMS           "Av\0""P\200\0""Rod""Hjd""Gas""Ske"

  #define TR_CSWTIMER          "Tim\0 "
  #define TR_CSWSTICKY         "Seg\0 "
    #define TR_CSWRANGE
    #define TR_CSWSTAY



#define LEN_VCSWFUNC           "\005"
#define TR_VCSWFUNC            "---\0 " "a=x\0 " "a\173x\0 ""a>x\0 ""a<x\0 " TR_CSWRANGE "|a|>x""|a|<x""AND\0 ""OR\0  ""XOR\0 " TR_CSWSTAY "a=b\0 ""a>b\0 ""a<b\0 ""^}x\0 ""|^|}x" TR_CSWTIMER TR_CSWSTICKY

#define LEN_VFSWFUNC           "\012"

#if defined(VARIO)
  #define TR_VVARIO            "Vario\0    "
#else
  #define TR_VVARIO            "[Vario]   "
#endif

#if defined(AUDIO)
  #define TR_SOUND             "Spela Ljud"
#else
  #define TR_SOUND             "Pip\0      "
#endif

#if defined(HAPTIC)
  #define TR_HAPTIC            "Vibrator\0 "
#else
  #define TR_HAPTIC            "[Vibrator]"
#endif

#if defined(VOICE)
    #define TR_PLAY_TRACK      "Spela upp\0"
  #define TR_PLAY_BOTH         "Spela B\200da"
  #define TR_PLAY_VALUE        "S\201g V\201rdet"
#else
  #define TR_PLAY_TRACK        "[Sp. Sp\200r]"
  #define TR_PLAY_BOTH         "[Sp. B\200da]"
  #define TR_PLAY_VALUE        "[Sp V\201rde]"
#endif

#define TR_SF_BG_MUSIC        "BgMusik\0  ""BgMusik ||"

#if defined(SDCARD)
  #define TR_SDCLOGS           "SD Loggar\0"
#else
  #define TR_SDCLOGS           "[SD Logg]\0"
#endif

#if defined(GVARS)
  #define TR_ADJUST_GVAR       "Justera\0  "
#else
  #define TR_ADJUST_GVAR       "[Just. GV]"
#endif

  #define TR_SF_PLAY_SCRIPT   "[Lua]\0    "

#if defined(DEBUG)
  #define TR_SF_TEST          "Test\0"
#else
  #define TR_SF_TEST
#endif

#if   defined(OVERRIDE_CHANNEL_FUNCTION)
  #define TR_SF_SAFETY        "S\201kra \0   "
#else
  #define TR_SF_SAFETY        "---\0      "
#endif

  #define TR_SF_SCREENSHOT

#define TR_SF_RESERVE         "[reserve]\0"

  #define TR_VFSWFUNC          TR_SF_SAFETY "Trainer \0 ""S\201tt Trim\0""Nollst\201ll\0" TR_ADJUST_GVAR TR_SOUND TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_VVARIO TR_HAPTIC TR_SDCLOGS "Belysning\0" TR_SF_TEST

#define LEN_VFSWRESET          "\004"

#if defined(FRSKY)
  #define TR_FSW_RESET_TELEM   "Telm"
  #define TR_MODELISON         INDENT "Modell is ON"
#else
  #define TR_FSW_RESET_TELEM
#endif

#if ROTARY_ENCODERS == 2
  #define TR_FSW_RESET_ROTENC  "REa\0""REb\0"
#elif ROTARY_ENCODERS == 1
  #define TR_FSW_RESET_ROTENC  "R.E."
#else
  #define TR_FSW_RESET_ROTENC
#endif

  #define TR_FSW_RESET_TIMERS  "Tmr1""Tmr2"

#define TR_VFSWRESET           TR_FSW_RESET_TIMERS "Alla" TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC

#define LEN_FUNCSOUNDS         "\004"
#define TR_FUNCSOUNDS          "Bp1\0""Bp2\0""Bp3\0""Wrn1""Wrn2""Chee""Rata""Tick""Sirn""Ring""SciF""Robt""Chrp""Tada""Crck""Alrm"

#define LEN_VTELEMCHNS         "\004"

  #define TR_TELEM_RESERVE
  #define TR_TELEM_TIME
  #define TR_SWR
  #define TR_RX_BATT
  #define TR_A3_A4
  #define TR_A3_A4_MIN

#define TR_ASPD_MAX            "ASp+"

  #define TR_TELEM_RSSI_RX     "Rx\0  "

  #define TR_TELEM_TIMERS      "Tmr1""Tmr2"

  #define TR_VTELEMCHNS        "---\0""Batt" TR_TELEM_TIME TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_TIMERS TR_SWR "Tx\0 ""Rx\0 " TR_RX_BATT "A1\0 ""A2\0 " TR_A3_A4 "Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""Hdg\0""VSpd""ASpd""dTE\0" TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE "A1-\0""A2-\0" TR_A3_A4_MIN "Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""Dst+" TR_ASPD_MAX "Cel-""Cls-""Vfs-""Cur+""Pwr+" TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE "Acc\0""Tid\0"

#define LENGTH_UNIT_IMP        "fot"
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
#define TR_VALARM              "---""Gul""Ora""R\202d"

#define LEN_VALARMFN           "\001"
#define TR_VALARMFN            "<>"

#define LEN_VTELPROTO          "\007"
#define TR_VTELPROTO           "---\0   ""Hubb\0  ""WSHH\202g.""S.Port\0"

  #define LEN_AMPSRC           "\003"
  #define TR_AMPSRC            "---""A1\0""A2\0""FAS""Cel"

#define LEN_VARIOSRC           "\005"
#if defined(FRSKY_SPORT)
  #define TR_VARIOSRC          "Vario""A1\0  ""A2\0"
#else
  #define TR_VARIOSRC          "Alti\0""Alti+""Vario""A1\0  ""A2\0"
#endif

  #define LEN_VTELEMSCREENTYPE "\007"
  #define TR_VTELEMSCREENTYPE  "Siffror""Staplar"

#define LEN_GPSFORMAT          "\004"
#define TR_GPSFORMAT           "DMS NMEA"

#define LEN2_VTEMPLATES        13
#define LEN_VTEMPLATES         "\015"
#define TR_VTEMPLATES          "Nolla Mixar\0\0""Enkel 4kanal\0""Gasklippning\0""V-Stj\201rt    \0""Deltavinge  \0""eCCPM       \0""Helikopter  \0""Servotest   \0"

#define LEN_VSWASHTYPE         "\004"
#define TR_VSWASHTYPE          "--- ""120 ""120X""140 ""90\0"

#define LEN_VKEYS              "\005"
#define TR_VKEYS               "Menu\0""Exit\0""Ned\0 ""Upp\0 ""H\202ger""V\201nst"

#define LEN_VRENCODERS         "\003"
#define TR_VRENCODERS          "REa""REb"

#define LEN_VSWITCHES          "\003"
#define LEN_VSRCRAW            "\004"

#define TR_STICKS_VSRCRAW      "Rod\0""Hjd\0""Gas\0""Ske\0"
#define TR_TRIMS_VSRCRAW       "TrmR""TrmH""TrmG""TrmS"

  #define TR_POTS_VSRCRAW      "P1\0 ""P2\0 ""P3\0 "
  #define TR_SW_VSRCRAW        "3POS"
  #define TR_9X_3POS_SWITCHES  "ID0""ID1""ID2"

  #define TR_LOGICALSW          "L1\0""L2\0""L3\0""L4\0""L5\0""L6\0""L7\0""L8\0""L9\0""L10""L11""L12""L13""L14""L15""L16""L17""L18""L19""L20"

#define TR_TRIMS_SWITCHES      "tRv""tRh""tHn""tHu""tGn""tGu""tSv""tSh"

  #define TR_ROTARY_ENCODERS   "REa\0""REb\0"
  #define TR_ROTENC_SWITCHES   "REA""REB""REN"

  #define TR_PHYS_SWITCHES     "THR""RUD""ELE""AIL""GEA""TRN"

#define TR_ON_ONE_SWITCHES     "P\200\0""Ett"

  #define TR_EXTRA_3POS_SWITCHES  "XD0""XD1""XD2"

  #define TR_VSWITCHES         "---" TR_9X_3POS_SWITCHES TR_PHYS_SWITCHES TR_TRIMS_SWITCHES TR_ROTENC_SWITCHES TR_EXTRA_3POS_SWITCHES TR_LOGICALSW TR_ON_ONE_SWITCHES

#if defined(HELI)
  #define TR_CYC_VSRCRAW       "CYK1""CYK2""CYK3"
#else
  #define TR_CYC_VSRCRAW       "[C1]""[C2]""[C3]"
#endif

  #define TR_EXTRA_VSRCRAW

#define TR_VSRCRAW             "---\0""Rod\0""Hjd\0""Gas\0""Ske\0" TR_POTS_VSRCRAW TR_ROTARY_ENCODERS "MAX\0" TR_CYC_VSRCRAW "TrmR" "TrmH" "TrmG" "TrmS" TR_SW_VSRCRAW

#define LEN_VTMRMODES          "\003"
#define TR_VTMRMODES           "Av\0""P\200\0""GAs""GA%""GAt"

#define LEN_VTRAINERMODES      "\022"
#define TR_VTRAINERMODES       "L\201rare/Uttag\0     ""Elev/Uttag\0       ""L\201rare/SBUS-Modul\0""L\201rare/CPPM-Modul\0""L\201rare/Batteri\0"

#define LEN_VFAILSAFE          "\011"
#define TR_VFAILSAFE           "Fri\0     ""L\200s Servo""Anpassat\0""Pulsfritt""Mottagare"

#if defined(MAVLINK)
  #define LEN_MAVLINK_BAUDS    "\006"
  #define TR_MAVLINK_BAUDS     "4800  ""9600  ""14400 ""19200 ""38400 ""57600 ""76800 ""115200"
  #define LEN_MAVLINK_AC_MODES "\011"
  #define TR_MAVLINK_AC_MODES  "Stabil   ""Akrobat  ""L\200s H\202jd ""Automat  ""Guidad   ""Hovra    ""Flyg Hem ""Cirkla   ""H\200ll Pos.""Landa    ""OF Hovra ""Leksak A ""Leksak M ""OGILTIG  "
  #define LEN_MAVLINK_AP_MODES "\015"
  #define TR_MAVLINK_AP_MODES  "Manuell      ""Cirkla       ""Stabilisera  ""\205vning       ""Fly by Wire A""Fly by Wire A""Automatisk   ""Flyg Hem     ""Hovra        ""Guidad       ""Initialiserar""OGILTIG      "
#endif

#define LEN_VSENSORTYPES        "\010"
#define TR_VSENSORTYPES        "Egen\0   ""Ber\201knad"

#define LEN_VFORMULAS          "\010"
#define TR_VFORMULAS           "Addera\0 ""Medelv.\0""Min\0    ""Max\0    ""Multipl.""Tot.sum.\0""Cell\0   ""F\202rbrukn""Str\201cka\0"

#define LEN_VPREC              "\004"
#define TR_VPREC               "0.--""0.0 ""0.00"

#define LEN_VCELLINDEX         "\007"
#define TR_VCELLINDEX          "L\201gsta\0""1\0     ""2\0     ""3\0     ""4\0     ""5\0     ""6\0     ""H\202gsta\0""Diff.\0 "

// ZERO TERMINATED STRINGS
  #define INDENT               "\001"
  #define LEN_INDENT           1
  #define INDENT_WIDTH         (FW/2)
  #define BREAKSPACE           " "

  #define TR_ENTER             "[MENU]"

#define TR_EXIT                "[EXIT]"

  #define TR_POPUPS            TR_ENTER "\010" TR_EXIT
  #define OFS_EXIT             sizeof(TR_ENTER)

#define TR_MENUWHENDONE        CENTER "\011" TR_ENTER " Avslutar "
#define TR_FREE                "kvar"
#define TR_DELETEMODEL         "Radera Modell"
#define TR_COPYINGMODEL        "Kopierar Modell "
#define TR_MOVINGMODEL         "Flyttar Modell "
#define TR_LOADINGMODEL        "Laddar Modell   "
#define TR_NAME                "Namn"
#define TR_MODELNAME           "Modellnamn"
#define TR_PHASENAME           "L\201gesnamn "
#define TR_MIXNAME             "Mixnamn "
#define TR_INPUTNAME           "Inputnamn"
#define TR_EXPONAME          "Exponamn"
#define TR_BITMAP              "Modellikon"
#define TR_TIMER               "Timer"
#define TR_ELIMITS             "Gr\201nser++"
#define TR_ETRIMS              "Trimmar++"
#define TR_TRIMINC             "Trimning"
#define TR_DISPLAY_TRIMS       "Display Trims"
#define TR_TTRACE              "F\202lj Gas"
#define TR_TSWITCH             "G-Switch"
#define TR_TTRIM               "Gastrim"
#define TR_BEEPCTR             "Cent.pip"
#define TR_USE_GLOBAL_FUNCS    "Glob.Funkt"
#define TR_PROTO               INDENT "Proto"
  #define TR_PPMFRAME          "PPM paket"
#define TR_MS                  "ms"
#define TR_SWITCH              "Brytare"
#define TR_TRIMS               "Trimmar"
#define TR_FADEIN              "Tona In"
#define TR_FADEOUT             "Tona Ut"
#define TR_DEFAULT             "Standard"
#define TR_CHECKTRIMS          CENTER "\006Kolla\012Trimmar"
#define OFS_CHECKTRIMS         CENTER_OFS+(9*FW)
#define TR_SWASHTYPE           "Swashtyp"
#define TR_COLLECTIVE          "Kollektiv"
#define TR_AILERON             "Skevroder-k\201lla"
#define TR_ELEVATOR            "H\202jdroder-k\201lla"
#define TR_SWASHRING           "Swashring"
#define TR_ELEDIRECTION        "H\202JD Riktning"
#define TR_AILDIRECTION        "SKEV Riktning"
#define TR_COLDIRECTION        "KOLL.Riktning"
#define TR_MODE                INDENT"L\201ge"
#define TR_NOFREEEXPO          "Expo saknas!"
#define TR_NOFREEMIXER         "Mixer saknas!"
#define TR_INSERTMIX           "Addera Mix"
#define TR_EDITMIX             "Redigera Mix"
#define TR_SOURCE              INDENT"K\201lla"
#define TR_WEIGHT              "Vikt"
#define TR_EXPO                "Expo"
#define TR_SIDE                "Sida"
#define TR_DIFFERENTIAL        "Diff."
#define TR_OFFSET              INDENT"Offset"
#define TR_TRIM                "Trim"
#define TR_DREX                "DRex"
#define DREX_CHBOX_OFFSET      30
#define TR_CURVE               "Kurva"
#define TR_FLMODE              "L\201ge"
#define TR_MIXWARNING          "Varning"
#define TR_OFF                 "Av "
#define TR_MULTPX              "Multpx"
#define TR_DELAYDOWN           "Dr\202j Ned"
#define TR_DELAYUP             "Dr\202j Upp"
#define TR_SLOWDOWN            "Tr\202g Ned"
#define TR_SLOWUP              "Tr\202g Upp"
#define TR_MIXER               "MIXAR"
#define TR_CV                  "KU"
#define TR_GV                  "GV"
#define TR_ACHANNEL            "A\004kanal  "
#define TR_RANGE               INDENT"MinMax"
#define TR_CENTER              INDENT "Center"
#define TR_BAR                 "Data"
#define TR_ALARM               INDENT"Alarm"
#define TR_USRDATA             "Anv\201ndardata"
#define TR_BLADES              INDENT"Blad"
#define TR_SCREEN              "Sk\201rm"
#define TR_SOUND_LABEL         "Ljud "
#define TR_LENGTH              INDENT"Tid"
#define TR_BEEP_LENGTH         INDENT "Pip-l\201ngd"
#define TR_SPKRPITCH           INDENT "Pip-ton"
#define TR_HAPTIC_LABEL        "Vibrator"
#define TR_HAPTICSTRENGTH      INDENT"Styrka"
#define TR_CONTRAST            "Kontrast"
#define TR_ALARMS_LABEL        "Alarm"
#define TR_BATTERY_RANGE       "Batteri-MinMax"
#define TR_BATTERYWARNING      INDENT"Batteri"
#define TR_INACTIVITYALARM     INDENT"Inaktivitet"
#define TR_MEMORYWARNING       INDENT"Lite Minne"
#define TR_ALARMWARNING        INDENT"Ljud Av"
#define TR_RENAVIG             "RotEnk Navig"
#define TR_THROTTLE_LABEL      "Gas"
#define TR_THROTTLEREVERSE     "Inv.Gas"
#define TR_TIMER_NAME          INDENT "Namn"
#define TR_MINUTEBEEP          INDENT "Minutpip"
#define TR_BEEPCOUNTDOWN       INDENT "Nedr\201kning"
#define TR_PERSISTENT          "J\201mt p\200 "
#define TR_BACKLIGHT_LABEL     "Belysning"
#define TR_BLDELAY             INDENT "Av efter"
#define TR_BLONBRIGHTNESS      INDENT "P\200 Ljusstyrka"
#define TR_BLOFFBRIGHTNESS     INDENT "Av Ljusstyrka"
#define TR_BLCOLOR             INDENT "F\201rg"
#define TR_SPLASHSCREEN        "Startbild"
#define TR_THROTTLEWARNING     "Gasvarning"
#define TR_SWITCHWARNING       "Bryt.varn."
#define TR_POTWARNING          "Rattvarn."
#define TR_TIMEZONE            "Tidszon"
#define TR_ADJUST_RTC          "Justera RTC"
#define TR_GPS                 "GPS"
#define TR_RXCHANNELORD        "Kanaler i RX"
#define TR_STICKS              "Spakar"
#define TR_POTS                "Rattar"
#define TR_SWITCHES_DELAY      "Brytarf\202rdr\202jning"
#define TR_SLAVE               "Elev"
#define TR_MODESRC             " L\201ge\004% K\201lla"
#define TR_MULTIPLIER          "Styrka"
#define TR_CAL                 "Kalib."
#define TR_VTRIM               "Trimm- +"
#define TR_BG                  "BG:"
  #define TR_MENUTOSTART         CENTER "\011" TR_ENTER " Startar "
  #define TR_SETMIDPOINT         CENTER "\012Centrera Allt"
  #define TR_MOVESTICKSPOTS      CENTER "\005R\202r Rattar/Spakar"
#define TR_RXBATT              "Rx Batt:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              CENTER "DATA SAKNAS"
#define TR_TOTTM1TM2THRTHP     "\037\146SES\036TM1\037\146TM2\036THR\037\146TH%"
#define TR_US                 "us"
#define TR_MENUTORESET         CENTER TR_ENTER " Nollar"
#define TR_PPM_TRAINER         "TR"
#define TR_CH                  "KN"
#define TR_MODEL               "Modell"
#define TR_FP                  "FL"
#define TR_MIX                 "MIX"
#define TR_EEPROMLOWMEM        "Minnesbrist"
#define TR_ALERT               "\016OBS"
#define TR_PRESSANYKEYTOSKIP   "Tryck ned en knapp"
#define TR_THROTTLENOTIDLE     "Gasen \201r p\200slagen!"
#define TR_ALARMSDISABLED      "Alarmen Avst\201ngda!"
#define TR_PRESSANYKEY         "Tryck ned en knapp"
#define TR_BADEEPROMDATA       "Minnet kan inte tolkas"
#define TR_EEPROMFORMATTING    "Minnet Nollst\201lls"
#define TR_EEPROMOVERFLOW      "Minnesfel"
#define TR_MENURADIOSETUP      "INST\204LLNINGAR"
#define TR_MENUDATEANDTIME     "DAG OCH TID"
#define TR_MENUTRAINER         "TRAINER (PPM IN)"
#define TR_MENUGLOBALFUNCS     "GLOBALA FUNKTIONER"
#define TR_MENUVERSION         "VERSION"
#define TR_MENUDIAG            "BRYTARE"
#define TR_MENUANA             "ANALOGA V\204RDEN"
#define TR_MENUCALIBRATION     "KALIBRERING"
  #define TR_TRIMS2OFFSETS       "\004[Spara Trimv\201rden]"
#define TR_MENUMODELSEL        "MODELL"
#define TR_MENUSETUP           "V\204RDEN"
#define TR_MENUFLIGHTPHASE     "FLYGL\204GE"
#define TR_MENUFLIGHTPHASES    "FLYGL\204GEN"
#define TR_MENUHELISETUP       "HELIKOPTER"
#define TR_MENUINPUTS        "SPAKAR"
#define TR_MENULIMITS        "SERVON"
#define TR_MENUCURVES          "KURVOR"
#define TR_MENUCURVE           "KURVA"
#define TR_MENULOGICALSWITCH   "LOGISK BRYTARE"
#define TR_MENULOGICALSWITCHES "LOGISKA BRYTARE"
#define TR_MENUCUSTOMFUNC      "BRYTARFUNKTIONER"
#define TR_MENUCUSTOMSCRIPTS   "SPECIALKOD"
#define TR_MENUCUSTOMSCRIPT    "SPECIALKOD"
#define TR_MENUTELEMETRY       "TELEMETRI"
#define TR_MENUTEMPLATES       "MALLAR"
#define TR_MENUSTAT            "STATISTIK"
#define TR_MENUDEBUG           "DEBUG"
  #define TR_RXNUM             "RxNum"
#define TR_SYNCMENU            "Synk [MENU]"
#define TR_LIMIT               INDENT"Niv\200"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Breddgrad"
#define TR_LONGITUDE           "L\201ngdgrad"
#define TR_GPSCOORD            "GPS-Koordinater"
#define TR_VARIO               "Vario"
#define TR_PITCH_AT_ZERO       INDENT "Ton vid Noll"
#define TR_PITCH_AT_MAX        INDENT "Ton vid Max"
#define TR_REPEAT_AT_ZERO      INDENT "Repetera vid Noll"
#define TR_POWEROFF            "\006SHUT DOWN ?"
#define TR_SHUTDOWN            "ST\204NGER AV"
#define TR_SAVEMODEL           "Spara modellinst\201lln."
#define TR_BATT_CALIB          "Kalib. Batteri"
#define TR_CURRENT_CALIB       "Kalib. Str\202m"
#define TR_VOLTAGE             INDENT"Volt"
#define TR_CURRENT             INDENT"Ampere"
#define TR_SELECT_MODEL        "V\201lj Modell"
#define TR_CREATE_MODEL        "Skapa Modell"
#define TR_COPY_MODEL          "Kopiera Modell"
#define TR_MOVE_MODEL          "Flytta Modell"
#define TR_BACKUP_MODEL        "Modell-backup"
#define TR_DELETE_MODEL        "Ta Bort Modell"
#define TR_RESTORE_MODEL       "\200terst\201ll Modell"
#define TR_SDCARD_ERROR        "SDCARD-fel"
#define TR_NO_SDCARD           "SDCARD saknas"
#define TR_SDCARD_FULL         "SD-kort Fullt"
#define TR_INCOMPATIBLE        "Inkompatibel"
#define TR_WARNING             "VARNING"
#define TR_EEPROMWARN          "EEPROM"
#define TR_EEPROM_CONVERTING   "EEPROM Konvertering"
#define TR_THROTTLEWARN        "GAS"
#define TR_ALARMSWARN          "ALARM"
#define TR_SWITCHWARN          "BRYTARE"
#define TR_FAILSAFEWARN        "FAILSAFE"
#define TR_NO_FAILSAFE         "Failsafe fri"
#define TR_KEYSTUCK            "Knapp-fel"
#define TR_INVERT_THR          "Invert.Gas?"
#define TR_SPEAKER_VOLUME      INDENT "Volym"
#define TR_LCD                 "LCD"
#define TR_BRIGHTNESS          INDENT "Ljusstyrka"
#define TR_CPU_TEMP            "CPU temp.\016>"
#define TR_CPU_CURRENT         "Str\202m\022>"
#define TR_CPU_MAH             "F\202rbrukn."
#define TR_COPROC              "CoProc."
#define TR_COPROC_TEMP         "MB temp. \016>"
#define TR_CAPAWARNING         INDENT "L\200g Kapacitet"
#define TR_TEMPWARNING         INDENT "H\202g Temp "
#define TR_FUNC                "Funk"
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Tidl\201ngd"
#define TR_DELAY               "F\202rdr\202j"
#define TR_SD_CARD             "SD-kort"
#define TR_SDHC_CARD           "SD/HC-kort"
#define TR_NO_SOUNDS_ON_SD     "Inga ljud i SD"
#define TR_NO_MODELS_ON_SD     "Ingen modell i SD"
#define TR_NO_BITMAPS_ON_SD    "Ikoner saknas p\200 SD"
#define TR_NO_SCRIPTS_ON_SD    "Programkod saknas p\200 SD"
#define TR_SCRIPT_SYNTAX_ERROR "Skript syntaxfel"
#define TR_SCRIPT_PANIC        "Skript panik"
#define TR_SCRIPT_KILLED       "Skript avbruten"
#define TR_SCRIPT_ERROR        "Ok\201nt skriptfel"
#define TR_PLAY_FILE           "Spela"
#define TR_DELETE_FILE         "Radera"
#define TR_COPY_FILE           "Kopia"
#define TR_RENAME_FILE         "Byt namn"
#define TR_ASSIGN_BITMAP       "Tilldela ikon"
#define TR_EXECUTE_FILE        "Utf\202r"
#define TR_REMOVED             " raderad"
#define TR_SD_INFO             "Information"
#define TR_SD_FORMAT           "Format"
#define TR_NA                  "N/A"
#define TR_HARDWARE            "H\200rdvara"
#define TR_FORMATTING          "Formaterar..."
#define TR_TEMP_CALIB          "Temp.kalib."
#define TR_TIME                "Tid "
#if defined(IMPERIAL_UNITS)
#define TR_TXTEMP              "Temp. TX\037\164@F"
#else
#define TR_TXTEMP              "Temp. TX\037\164@C"
#endif
#define TR_BAUDRATE            "BT Baudrate"
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Typ: "
#define TR_SD_SPEED            "Hastighet:"
#define TR_SD_SECTORS          "Sektorer:"
#define TR_SD_SIZE             "Strl:"
#define TR_TYPE                INDENT "Typ "
#define TR_GLOBAL_VARS         "Globala Variabler"
#define TR_GLOBAL_V            "GLOBAL V."
#define TR_GLOBAL_VAR          "Global Variabel"
#define TR_MENUGLOBALVARS      "GLOBALA VARIABLER"
#define TR_OWN                 "Egen"
#define TR_DATE                "Datum"
#define TR_ROTARY_ENCODER      "R.Enks"
#define TR_CHANNELS_MONITOR    "Kanal\202versikt"
#define TR_MIXERS_MONITOR      "MIXER-SK\204RM"
#define TR_PATH_TOO_LONG       "F\202r l\200ng s\202kv\201g"
#define TR_VIEW_TEXT           "Visa Text"
#define TR_FLASH_BOOTLOADER    "Skriv BootLoader"
#define TR_FLASH_EXTERNAL_DEVICE "Flash Extern Enhet"
#define TR_FLASH_INTERNAL_MODULE "Flash Intern Modul"
#define TR_WRITING             "\032Skriver..."
#define TR_CONFIRM_FORMAT      "Formatera Minnet?"
#define TR_INTERNALRF          "Intern Radio"
#define TR_EXTERNALRF          "Extern Radiomodul"
#define TR_FAILSAFE            INDENT "Failsafe"
#define TR_FAILSAFESET         "FailsafeInst\201llning"
#define TR_MENUSENSOR          "SENSOR"
#define TR_COUNTRYCODE         "Landskod"
#define TR_VOICELANG           "R\202stspr\200k"
#define TR_UNITSSYSTEM         "Enheter"
#define TR_EDIT                "Redigera"
#define TR_INSERT_BEFORE       "Addera F\202re"
#define TR_INSERT_AFTER        "Addera Efter"
#define TR_COPY                "Kopiera"
#define TR_MOVE                "Flytta"
#define TR_PASTE               "Infoga"
#define TR_DELETE              "Radera"
#define TR_INSERT              "Addera"
#define TR_RESET_FLIGHT        "Nollst\201ll Flygning"
#define TR_RESET_TIMER1        "Nollst\201ll Timer1"
#define TR_RESET_TIMER2        "Nollst\201ll Timer2"
#define TR_RESET_TIMER3        "Nollst\201ll Timer3"
#define TR_RESET_TELEMETRY     "Nollst\201ll Telemetri"
#define TR_STATISTICS          "Statistik"
#define TR_SAVE_TIMERS         "Save Timers"
#define TR_ABOUT_US            "Om Oss"
#define TR_AND_SWITCH          "OCH Brytare"
#define TR_SF                  "BF"
#define TR_GF                  "GF"
#define TR_SPEAKER             INDENT"H\202gtalare"
#define TR_BUZZER              INDENT"Summer"
#define TR_BYTES               "byte"
#define TR_MODULE_BIND         "[Bnd]"
#define TR_MODULE_RANGE        "[Tst]"
#define TR_RESET_BTN           "[Nollst\201ll]"
#define TR_SET                 "[Spara]"
#define TR_TRAINER             "Trainer"
#define TR_ANTENNAPROBLEM      CENTER "Fel p\200 TX-antennen"
#define TR_MODELIDUSED         "ID finns redan"
#define TR_MODULE              INDENT "Modul"
#define TR_TELEMETRY_TYPE      "Telemetrityp"
#define TR_TELEMETRY_SENSORS   "Sensorer"
#define TR_VALUE               "V\201rde"
#define TR_TOPLCDTIMER         "\205vre LCD Timer"
#define TR_UNIT                "Enhet"
#define TR_TELEMETRY_NEWSENSOR INDENT "L\201gg till sensor..."
#define TR_CHANNELRANGE        INDENT "Kanalomr\200de"
#define TR_LOWALARM            INDENT "L\200g-alarm"
#define TR_CRITICALALARM       INDENT "Kritiskt alarm"
#define TR_ENABLE_POPUP        "Sl\200 p\200 Dialog"
#define TR_DISABLE_POPUP       "Sl\200 av Dialog"
#define TR_CURVE_PRESET        "Lutning..."
#define TR_PRESET              "Lutning"
#define TR_MIRROR              "Spegla"
#define TR_CLEAR               "T\202m"
#define TR_RESET               "Nollst\201ll"
#define TR_RESET_SUBMENU       "Nollst\201ll..."
#define TR_COUNT               "Antal"
#define TR_PT                  "pt"
#define TR_PTS                 "pkt"
#define TR_SMOOTH              "Mjuk"
#define TR_COPY_STICKS_TO_OFS  "Spara spakar som offset"
#define TR_COPY_TRIMS_TO_OFS   "Spara trimmar som offset"
#define TR_INCDEC              "Inc/Decrement"
#define TR_GLOBALVAR           "Global Var"
#define TR_MIXSOURCE           "Mixer K\201lla"
#define TR_CONSTANT            "Konstant"
#define TR_PERSISTENT_MAH      INDENT "Lagra mAh"
#define TR_PREFLIGHT           "Startkontroller"
#define TR_CHECKLIST           INDENT "Checklista"
#define TR_FAS_OFFSET          INDENT "FAS Ofs"
#define TR_UART3MODE           "Serieporten"
#define TR_SCRIPT              "Programkod"
#define TR_INPUTS              "Input"
#define TR_OUTPUTS             "Outputs"
#define TR_EEBACKUP            "\004[ENTER L\200ng] s\201kerhetskopiera EEPROM"
#define TR_FACTORYRESET        "\012[MENU Long]: Factory reset"

#if defined(X_ANY) || defined(BLUETOOTH)
  #define TR_X_ANY                        "X ANY"
  #define TR_ACTIVED                      "Active"
  #define TR_NUMBER                       "Number"
  #define TR_CHANNEL                      "Channel "
  #define TR_NB_REPEAT                    "Nb repeat "
  #define LEN_SWITCHES_VALUES             "\005"
  #define TR_SWITCHES_VALUES              "Sw.--""L1-L4""L1-L8""Sw.4 ""Sw.8 ""Sw.16"
  #define TR_ANGLE_SENSOR                 "0:360"
  #define TR_PROP                         "Prop."
  #define LEN_XANY_EXPO                   "\005"
  #define TR_XANY_EXPO                    "0%\0  ""25%\0 ""37.5%""50%\0 "
#endif
