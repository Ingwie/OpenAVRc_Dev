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
*  \001 to \034     -extended spacing (value * FW/2)
*  \0               -ends current string
*/

/* NON ZERO TERMINATED STRINGS */
#define LEN_OFFON              "\003"
#define TR_OFFON               "Av ""P\200\0"
#define LEN_MMMINV             "\003"
#define TR_MMMINV              "---""INV"
#define LEN_NCHANNELS          "\004"
#define TR_NCHANNELS           "\0014KN\0016KN\0018KN10KN12KN14KN16KN"
#define LEN_VBEEPMODE          "\005"
#define TR_VBEEPMODE           "Tyst\0""Alarm""EjKnp""Alla\0"
#define LEN_VRENAVIG           "\003"
#define TR_VRENAVIG            "Ingen REaREb"
#define LEN_VBLMODE            "\005"
#define TR_VBLMODE             "Av\0""Knapp""Spak\0""B\200da\0""P\200\0  "
#define LEN_TRNMODE            "\002"
#define TR_TRNMODE             "Av""+="":="
#define LEN_TRNCHN             "\003"
#define TR_TRNCHN              "KN1KN2KN3KN4"
#define LEN_VPERSISTENT        "\014"
#define TR_VPERSISTENT         "AV\0         ""Flygning\0   ""Nolla Sj\201lv\0"
#define LEN_DSM_PROTOCOLS      "\004"
#define TR_DSM_PROTOCOLS       "LP45""DSM2""DSMX"
#define LEN_VTRIMINC           "\006"
#define TR_VTRIMINC            "Expo\0 ""xFin\0 ""Fin\0  ""Medium""Grov\0 "
#define LEN_CURVE_TYPES        "\010"
#define TR_CURVE_TYPES         "Normal\0 ""Special\0"
#define LEN_RETA123            "\001"
#define TR_RETA123           "RHGS123ab"

#if defined(PWM_BACKLIGHT)
#define TR_BLONBRIGHTNESS      INDENT "P\200 Ljuss."
#define TR_BLOFFBRIGHTNESS     INDENT "Av Ljuss."
#endif
#if (SERIAL_PROTOCOL==DSM)
 #define TR_DSM2              "LP45\0 ""DSM2\0 ""DSMX\0 "
#else
 #define TR_DSM2
#endif
#if defined(SPIMODULES)
 #define TR_SPIM              "SPIRfMod"

 #define TR_RFPOWER INDENT    "RF Power"
#endif
#if (SERIAL_PROTOCOL==MULTIMODULE)
 #define LEN_MULTIPROTOCOLS    "\006"
 #define TR_MULTIPROTOCOLS     "FlySky""Hubsan""FrSky\0""Hisky\0""V2x2\0 ""DSM\0  ""Devo\0 ""YD717\0""KN\0   ""SymaX\0""SLT\0  ""CX10\0 ""CG023\0""Bayang""ESky\0 ""MT99XX""MJXQ\0 ""Shenqi""FY326\0""SFHSS\0""J6 PRO""FQ777\0""Assan\0""Hontai""OLRS\0 ""FS 2A\0""Q2x2\0 ""Walk.\0""Q303\0 ""GW008\0""DM002\0"
 #define TR_MULTI_CUSTOM       "Custom"
#endif
#if (SERIAL_PROTOCOL==MULTIMODULE) || defined(SPIMODULES)
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

#if   defined(OVERRIDE_CHANNEL_FUNCTION)
 #define TR_SF_SAFETY        "S\201kra \0   "
#else
 #define TR_SF_SAFETY        "---\0      "
#endif

#define TR_VFSWFUNC          TR_SF_SAFETY "Trainer \0 ""S\201tt Trim\0""Nollst\201ll\0" TR_ADJUST_GVAR TR_SOUND TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_VVARIO TR_HAPTIC TR_SDCLOGS "Belysning\0"
#define LEN_VFSWRESET          "\004"
#if defined(FRSKY)
 #define TR_FSW_RESET_TELEM   "Telm"
 #define TR_MODELISON         INDENT "Modell is ON"
#else
 #define TR_FSW_RESET_TELEM
#endif
#define TR_FSW_RESET_TIMERS  "Tmr1""Tmr2"
#define TR_FSW_RESET_ROTENC  "REa\0""REb\0"
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
#define LEN_VSRCRAW            "\004"
#define LEN_VSWITCHES          "\003"
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

/* ZERO TERMINATED STRINGS */
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
#define TR_TIMER               "Timer"
#define TR_ELIMITS             "Gr\201nser++"
#define TR_ETRIMS              "Trimmar++"
#define TR_TRIMINC             "Trimning"
#define TR_TTRACE              "F\202lj Gas"
#define TR_TSWITCH             "G-Switch"
#define TR_TTRIM               "Gastrim"
#define TR_BEEPCTR             "Cent.pip"
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
#define TR_MINUTEBEEP          INDENT "Minutpip"
#define TR_BEEPCOUNTDOWN       INDENT "Nedr\201kning"
#define TR_PERSISTENT          "J\201mt p\200 "
#define TR_BACKLIGHT_LABEL     "Belysning"
#define TR_BLDELAY             INDENT "Av efter"
#define TR_SPLASHSCREEN        "Startbild"
#define TR_THROTTLEWARNING     "Gasvarning"
#define TR_SWITCHWARNING       "Bryt.varn."
#define TR_TIMEZONE            "Tidszon"
#define TR_ADJUST_RTC          "Justera RTC"
#define TR_GPS                 "GPS"
#define TR_RXCHANNELORD        "Kanaler i RX"
#define TR_STICKS              "Spakar"
#define TR_POTS                "Rattar"
#define TR_SLAVE               "Elev"
#define TR_MODESRC             " L\201ge\004% K\201lla"
#define TR_MULTIPLIER          "Styrka"
#define TR_CAL                 "Kalib."
#define TR_VTRIM               "Trimm- +"
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
#define TR_MENUTRAINER         "TRAINER (PPM IN)"
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
#define TR_MENULOGICALSWITCHES "LOGISKA BRYTARE"
#define TR_MENUCUSTOMFUNC      "BRYTARFUNKTIONER"
#define TR_MENUTELEMETRY       "TELEMETRI"
#define TR_MENUTEMPLATES       "MALLAR"
#define TR_MENUSTAT            "STATISTIK"
#define TR_MENUDEBUG           "DEBUG"
#define TR_RXNUM             "RxNum"
#define TR_LIMIT               INDENT"Niv\200"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Breddgrad"
#define TR_LONGITUDE           "L\201ngdgrad"
#define TR_GPSCOORD            "GPS-Koordinater"
#define TR_VARIO               "Vario"
#define TR_POWEROFF            "\006SHUT DOWN ?"
#define TR_SHUTDOWN            "ST\204NGER AV"
#define TR_SAVEMODEL           "Spara modellinst\201lln."
#define TR_BATT_CALIB          "Kalib. Batteri"
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
#define TR_INCOMPATIBLE        "Inkompatibel"
#define TR_WARNING             "VARNING"
#define TR_EEPROMWARN          "EEPROM"
#define TR_THROTTLEWARN        "GAS"
#define TR_ALARMSWARN          "ALARM"
#define TR_SWITCHWARN          "BRYTARE"
#define TR_INVERT_THR          "Invert.Gas?"
#define TR_SPEAKER_VOLUME      INDENT "Volym"
#define TR_FUNC                "Funk"
#define TR_DELAY               "F\202rdr\202j"
#define TR_SD_CARD             "SD-kort"
#define TR_NO_MODELS_ON_SD     "Ingen modell i SD"
#define TR_PLAY_FILE           "Spela"
#define TR_DELETE_FILE         "Radera"
#define TR_COPY_FILE           "Kopia"
#define TR_RENAME_FILE         "Byt namn"
#define TR_REMOVED             " raderad"
#define TR_SD_INFO             "Information"
#define TR_NA                  "N/A"
#define TR_TIME                "Tid "
#if defined(IMPERIAL_UNITS)
 #define TR_TXTEMP              "Temp. TX\037\164@F"
#else
 #define TR_TXTEMP              "Temp. TX\037\164@C"
#endif
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Typ: "
#define TR_SD_SIZE             "Strl:"
#define TR_TYPE                INDENT "Typ "
#define TR_GLOBAL_VARS         "Globala Variabler"
#define TR_GLOBAL_V            "GLOBAL V."
#define TR_GLOBAL_VAR          "Global Variabel"
#define TR_OWN                 "Egen"
#define TR_DATE                "Datum"
#define TR_ROTARY_ENCODER      "R.Enks"
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
#define TR_RESET_TELEMETRY     "Nollst\201ll Telemetri"
#define TR_STATISTICS          "Statistik"
#define TR_SAVE_TIMERS         "Save Timers"
#define TR_SPEAKER             INDENT"H\202gtalare"
#define TR_BUZZER              INDENT"Summer"
#define TR_MODULE_BIND         "[Bnd]"
#define TR_MODULE_RANGE        "[Prt]"
#define TR_RESET_BTN           "[Nollst\201ll]"
#define TR_SET                 "[Spara]"
#define TR_RESET               "Nollst\201ll"
#define TR_CONSTANT            "Konstant"
#define TR_FAS_OFFSET          INDENT "FAS Ofs"
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
#if defined(BLUETOOTH)
 #define TR_BLUETOOTH                     "BLUETOOTH"
 #define TR_NOBLUETOOTH                   "\3NO BLUETOOTH"
 #define TR_CONNECTED                     "\036\5CONECTEd"
 #define TR_BT_ROLE                       "Role"
 #define TR_BT_PIN                        "Pin"
 #define TR_BT_M_S                        "\006""Slave\0""Master"
 #define TR_BT_PAIR                       "Pair"
 #define TR_AUTOCON                       "Auto-con."
 #define TR_RESCANN                       "Re-Scan"
#endif
