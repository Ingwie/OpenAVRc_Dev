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
#define TR_OFFON               "Vyp""Zap"
#define LEN_MMMINV             "\003"
#define TR_MMMINV              "---""Rev"
#define LEN_NCHANNELS          "\004"
#define TR_NCHANNELS           "\0014CH\0016CH\0018CH10CH12CH14CH16CH"
#define LEN_VBEEPMODE          "\005"
#define TR_VBEEPMODE           "Tich\226""Alarm""BezKl""V\216e\0"
#define LEN_VRENAVIG           "\003"
#define TR_VRENAVIG            "Ne REaREb"
#define LEN_VBLMODE            "\005"
#define TR_VBLMODE             "Vyp\0 ""Kl\201v.""P\201ky\0""V\216e\0 ""Zap\0 "
#define LEN_TRNMODE            "\003"
#define TR_TRNMODE             " x "" +="" :="
#define LEN_TRNCHN             "\003"
#define TR_TRNCHN              "CH1CH2CH3CH4"
#define LEN_VPERSISTENT        "\014"
#define TR_VPERSISTENT         "Ne\0         ""V r\201mci letu""Reset ru\202n\206\0"
#define LEN_DSM_PROTOCOLS      "\004"
#define TR_DSM_PROTOCOLS       "LP45""DSM2""DSMX"
#define LEN_VTRIMINC           "\007"
#define TR_VTRIMINC            "Expo\0  ""ExJemn\226""Jemn\226\0 ""St\215edn\210""Hrub\226\0 "
#define LEN_CURVE_TYPES        "\010"
#define TR_CURVE_TYPES         "Rastr-X\0""Voln\201-XY"
#define LEN_RETA123            "\001"
#define TR_RETA123             "SVPK123ab"

#if defined(PWM_BACKLIGHT)
#define TR_BLONBRIGHTNESS      INDENT"Jas Zap."
#define TR_BLOFFBRIGHTNESS     INDENT"Jas Vyp."
#endif
#if (SERIAL_PROTOCOL==DSM)
 #define TR_DSM2               "LP45\0 ""DSM2\0 ""DSMX\0 "
#else
 #define TR_DSM2
#endif
#if defined(SPIMODULES)
 #define TR_SPIM               "SPIRFmod"

 #define TR_RFPOWER INDENT     "RF v\226kon"
#endif
#if (SERIAL_PROTOCOL==MULTIMODULE)
 #define LEN_MULTIPROTOCOLS    "\006"
 #define TR_MULTIPROTOCOLS     "FlySky""Hubsan""FrSky\0""Hisky\0""V2x2\0 ""DSM\0  ""Devo\0 ""YD717\0""KN\0   ""SymaX\0""SLT\0  ""CX10\0 ""CG023\0""Bayang""ESky\0 ""MT99XX""MJXQ\0 ""Shenqi""FY326\0""SFHSS\0""J6 PRO""FQ777\0""Assan\0""Hontai""OLRS\0 ""FS 2A\0""Q2x2\0 ""Walk.\0""Q303\0 ""GW008\0""DM002\0"
 #define TR_MULTI_CUSTOM       "Custom"
#endif
#if ((SERIAL_PROTOCOL==SUMD) || (SERIAL_PROTOCOL==SBUS) || (SERIAL_PROTOCOL==CRSF))
#define TR_PERIOD                INDENT "Perioda"
#endif
#if (SERIAL_PROTOCOL==MULTIMODULE) || defined(SPIMODULES)
 #define TR_SUBTYPE                      "Sub typ"
 #define TR_RFTUNEFINE            INDENT "frek. lad."
 #define TR_TELEMETRY                    "telemetrie"
 #define TR_MULTI_VIDFREQ         INDENT "frek. lad."
 #define TR_MULTI_RFPOWER         INDENT "RF v\226kon"
 #define TR_MULTI_OPTION          INDENT "Mo\227nost"
 #define TR_AUTOBIND              INDENT "autobind"
 #define TR_MULTI_DSM_AUTODTECT   INDENT "autodetekt"
 #define TR_MULTI_LOWPOWER        INDENT "mal\226 v\226kon"
 #define TR_MULTI_SERVOFREQ       INDENT "v\226ch.serv"
#endif
#define LEN_VCURVEFUNC         "\003"
#define TR_VCURVEFUNC          "---""x>0""x<0""|x|""f>0""f<0""|f|"
#define LEN_VMLTPX             "\007"
#define TR_VMLTPX              "Se\202\210st\0""N\201sobit""Zam\206nit"
#define LEN_VMLTPX2            "\002"
#define TR_VMLTPX2             "+=""*="":="
#define LEN_VMIXTRIMS          "\004"
#define TR_VMIXTRIMS           "Vyp\0""Zap\0""Sm\206r""V\226\216k""Plyn""K\215id"
#define TR_CSWTIMER            "Tim\0 "
#define TR_CSWSTICKY           "Glue\0"
#define TR_CSWRANGE
#define TR_CSWSTAY
#define LEN_VCSWFUNC           "\005"
#define TR_VCSWFUNC            "---\0 " "a=x\0 " "a\173x\0 ""a>x\0 ""a<x\0 " TR_CSWRANGE "|a|>x""|a|<x""AND\0 ""OR\0  ""XOR\0 " TR_CSWSTAY "a=b\0 ""a>b\0 ""a<b\0 ""^}x\0 ""|^|}x" TR_CSWTIMER TR_CSWSTICKY
#define LEN_VFSWFUNC           "\013"
#if defined(VARIO)
 #define TR_VVARIO             "Vario\0     "
#else
 #define TR_VVARIO             "[vario]\0   "
#endif
#if defined(AUDIO)
 #define TR_SOUND              "\221\222Zvuk\0    "
#else
 #define TR_SOUND              "P\210pnout\0   "
#endif
#if defined(HAPTIC)
 #define TR_HAPTIC             "Vibrovat\0  "
#else
 #define TR_HAPTIC             "[vibrovat]\0"
#endif
#if defined(VOICE)
 #define TR_PLAY_TRACK         "\221\222Stopa\0   "
 #define TR_PLAY_BOTH          "\221\222P\201r stop\0"
 #define TR_PLAY_VALUE         "\221\222Hl\201sit\0  "
#else
 #define TR_PLAY_TRACK         "[\221\222Stopa]\0 "
 #define TR_PLAY_BOTH          "[\221\222P\201r]\0   "
 #define TR_PLAY_VALUE         "[\221\222Hl\201sit]\0"
#endif
#if defined(SDCARD)
 #define TR_SDCLOGS            "Loguj na SD"
#else
 #define TR_SDCLOGS            "[logov\201n\210]\0"
#endif
#ifdef GVARS
 #define TR_ADJUST_GVAR        "Nastav \0   "
#else
 #define TR_ADJUST_GVAR        "[nastav GP]"
#endif

#if defined(OVERRIDE_CHANNEL_FUNCTION)
 #define TR_SF_SAFETY          "Z\201mek \0    "
#else
 #define TR_SF_SAFETY          "---\0       "
#endif

#define TR_VFSWFUNC          TR_SF_SAFETY "Tren\204r \0   ""Insta-Trim\0""Reset\0     " TR_ADJUST_GVAR TR_SOUND TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_VVARIO TR_HAPTIC TR_SDCLOGS "Podsv\206tlen\210"
#define LEN_VFSWRESET                "\004"
#if defined(FRSKY)
 #define TR_FSW_RESET_TELEM          "Telm"
 #define TR_MODELISON         INDENT "Model je zapnut\226 !"
#else
 #define TR_FSW_RESET_TELEM
#endif
#define TR_FSW_RESET_TIMERS          "Tmr1""Tmr2"
#define TR_FSW_RESET_ROTENC          "REa\0""REb\0"
#define TR_VFSWRESET       TR_FSW_RESET_TIMERS "v\216e\0" TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC
#define LEN_FUNCSOUNDS               "\004"
#define TR_FUNCSOUNDS                "Bp1\0""Bp2\0""Bp3\0""Wrn1""Wrn2""Chee""Rata""Tick""Sirn""Ring""SciF""Robt""Chrp""Tada""Crck""Alrm"
#define LEN_VTELEMCHNS               "\004"
#define TR_TELEM_RESERVE
#define TR_TELEM_TIME
#define TR_SWR
#define TR_RX_BATT
#define TR_A3_A4
#define TR_A3_A4_MIN
#define TR_ASPD_MAX          "ASp+"
#define TR_TELEM_RSSI_RX     "Rx\0 "
#define TR_TELEM_TIMERS      "Tmr1""Tmr2"
#define TR_VTELEMCHNS        "---\0""Bat." TR_TELEM_TIME TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_TIMERS TR_SWR "Tx\0 " TR_TELEM_RSSI_RX TR_RX_BATT "A1\0 ""A2\0 " TR_A3_A4 "Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""Hdg\0""VSpd""ASpd""dTE\0" TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE "A1-\0""A2-\0" TR_A3_A4_MIN "Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""Dst+" TR_ASPD_MAX "Cel-""Cls-""Vfs-""Cur+""Pwr+" TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE "Acc\0""Time"
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
#define LEN_VTELPROTO          "\007"
#define TR_VTELPROTO           "---\0   ""Hub\0   ""WSHHigh""S.Port\0"
#define LEN_AMPSRC             "\003"
#define TR_AMPSRC              "---""A1\0""A2\0""FAS""Cel"
#define LEN_VARIOSRC           "\004"
#if defined(FRSKY_SPORT)
 #define TR_VARIOSRC           "VSpd""A1\0 ""A2\0 ""dTE\0"
#else
 #define TR_VARIOSRC           "Alt\0""Alt+""VSpd""A1\0 ""A2\0"
#endif
#define LEN_VTELEMSCREENTYPE    "\010"
#define TR_VTELEMSCREENTYPE     "Hodnota ""Ukazatel"
#define LEN_GPSFORMAT           "\004"
#define TR_GPSFORMAT            "DMS\0""NMEA"
#define LEN2_VTEMPLATES         13
#define LEN_VTEMPLATES          "\015"
#define TR_VTEMPLATES           "Smazat mixy\0\0""Z\201kl. 4kan\201l\0""Sticky-T-Cut\0""V-Tail      \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"
#define LEN_VSWASHTYPE          "\004"
#define TR_VSWASHTYPE           "---\0""120\0""120X""140\0""90\0"
#define LEN_VKEYS               "\005"
#define TR_VKEYS                "Menu\0""Exit\0""Dolu\0""Nhoru""Vprvo""Vlevo"
#define LEN_VRENCODERS          "\003"
#define TR_VRENCODERS           "REa""REb"
#define LEN_INPUTNAMES          "\004"
#define TR_INPUTNAMES           "Smer""Vysk""Plyn""Krid"
#define TR_STICKS_VSRCRAW       "Sm\206r""V\226\216k""Plyn""K\215id"
#define TR_TRIMS_VSRCRAW        "TrmS""TrmV""TrmP""TrmK"
#define TR_POTS_VSRCRAW         "POT1""POT2""POT3"
#define TR_SW_VSRCRAW           "3POS"
#define TR_9X_3POS_SWITCHES     "ID0""ID1""ID2"
#define TR_LOGICALSW            "L1\0""L2\0""L3\0""L4\0""L5\0""L6\0""L7\0""L8\0""L9\0""L10""L11""L12""L13""L14""L15""L16""L17""L18""L19""L20"
#define TR_TRIMS_SWITCHES       "tSl""tSp""tVd""tVn""tPd""tPn""tKl""tKp"
#define TR_ROTARY_ENCODERS      "REa\0""REb\0"
#define TR_ROTENC_SWITCHES      "REa""REb""REn"
#define TR_PHYS_SWITCHES        "THR""RUD""ELE""AIL""GEA""TRN"
#define TR_ON_ONE_SWITCHES      "Zap""One"
#define TR_EXTRA_3POS_SWITCHES  "XD0""XD1""XD2"
#define LEN_VSRCRAW             "\004"
#define LEN_VSWITCHES           "\003"
#define TR_VSWITCHES            "---" TR_9X_3POS_SWITCHES TR_PHYS_SWITCHES TR_TRIMS_SWITCHES TR_ROTENC_SWITCHES TR_EXTRA_3POS_SWITCHES TR_LOGICALSW TR_ON_ONE_SWITCHES
#if defined(HELI)
 #define TR_CYC_VSRCRAW         "CYC1""CYC2""CYC3"
#else
 #define TR_CYC_VSRCRAW         "[C1]""[C2]""[C3]"
#endif
#define TR_EXTRA_VSRCRAW
#define TR_VSRCRAW              "---\0" TR_STICKS_VSRCRAW TR_POTS_VSRCRAW TR_ROTARY_ENCODERS "MAX\0" TR_CYC_VSRCRAW TR_TRIMS_VSRCRAW TR_SW_VSRCRAW TR_EXTRA_VSRCRAW
#define LEN_VTMRMODES           "\004"
#define TR_VTMRMODES            "Vyp\0""Zap\0""Pln>""Pln%""Pln*"

/* ZERO TERMINATED STRINGS */
#define INDENT                  "\001"
#define LEN_INDENT              1
#define INDENT_WIDTH            (FW/2)
#define BREAKSPACE              " "
#define TR_ENTER                "[MENU]"
#define TR_EXIT                 "   [EXIT]"
#define TR_POPUPS      TR_ENTER "\010" TR_EXIT
#define OFS_EXIT                sizeof(TR_ENTER)
#define TR_MENUWHENDONE         CENTER "\011" TR_ENTER " > d\201le"
#define TR_FREE                 "voln\204:"
#define TR_DELETEMODEL          "SMAZAT" BREAKSPACE "MODEL"
#define TR_COPYINGMODEL         "Kop\210ruji model.."
#define TR_MOVINGMODEL          "P\215esouv\201m model."
#define TR_LOADINGMODEL         "Aktivuji model.."
#define TR_NAME                 "N\201zev"
#define TR_TIMER                "Stopky "
#define TR_ELIMITS              "Limit +25%"
#define TR_ETRIMS              "\216ir\216\210 trim"
#define TR_TRIMINC             "Krok trimu"
#define TR_TTRACE              "StopaPlynu"
#define TR_TSWITCH             "Sp\210na\202Plyn"
#define TR_TTRIM               "TrimVolnob."
#define TR_BEEPCTR             "P\210p. st\215edu"
#define TR_PROTO               INDENT "Protokol"
#define TR_PPMFRAME            "PPM r\201mec"
#define TR_MS                  "ms"
#define TR_SWITCH              "Sp\210na\202"
#define TR_TRIMS               "Trimy"
#define TR_FADEIN              "P\215echod Zap"
#define TR_FADEOUT             "P\215echod Vyp"
#define TR_DEFAULT             "(v\226choz\210)"
#define TR_CHECKTRIMS          "\011Kont.\010Trimy"
#define OFS_CHECKTRIMS         (9*FW)
#define TR_SWASHTYPE           "Typ cykliky"
#define TR_COLLECTIVE          "Kolektiv"
#define TR_SWASHRING           "Cyklika"
#define TR_ELEDIRECTION        "Sm\206r:V\226\216kovka"
#define TR_AILDIRECTION        "\012K\215id\204lka"
#define TR_COLDIRECTION        "\012Kolektiv"
#define TR_MODE                INDENT "re\227im"
#define TR_NOFREEEXPO          "Nen\210 voln\204 expo!"
#define TR_NOFREEMIXER         "Nen\210 voln\226 mix!"
#define TR_INSERTMIX           "VLO\227IT MIX "
#define TR_EDITMIX             "UPRAVIT MIX "
#define TR_SOURCE              INDENT "Zdroj"
#define TR_WEIGHT              "V\201ha"
#define TR_EXPO                "Expo"
#define TR_SIDE                "Strana"
#define TR_DIFFERENTIAL        "Dif.v\226chylek"
#define TR_OFFSET              INDENT "Ofset"
#define TR_TRIM                "Trim"
#define TR_DREX                "DR/Expo"
#define DREX_CHBOX_OFFSET      45
#define TR_CURVE               "K\215ivka"
#define TR_FLMODE              "Re\227im"
#define TR_MIXWARNING          "Varov\201n\210"
#define TR_OFF                 "Vyp"
#define TR_MULTPX              "Mat.operace"
#define TR_DELAYDOWN           "Zpo\227d\206n\210 Vyp"
#define TR_DELAYUP             "Zpo\227d\206n\210 Zap"
#define TR_SLOWDOWN            "Zpomalen\210(-)"
#define TR_SLOWUP              "Zpomalen\210(+)"
#define TR_MIXER               "MIXER"
#define TR_CV                  "K"
#define TR_GV                  "GP"
#define TR_ACHANNEL            "A\004Kan\201l"
#define TR_RANGE               INDENT "Rozsah"
#define TR_ALARM               INDENT "alarm"
#define TR_USRDATA             "U\227ivData"
#define TR_BLADES              INDENT "ListyVrt"
#define TR_SCREEN              "Panel "
#define TR_SOUND_LABEL         "Zvuk"
#define TR_LENGTH              INDENT "d\204lka"
#define TR_BEEP_LENGTH         INDENT "d\204lka"
#define TR_SPKRPITCH           INDENT "t\213n"
#define TR_HAPTIC_LABEL        "Vibrace"
#define TR_HAPTICSTRENGTH      INDENT "S\210la"
#define TR_CONTRAST            "Kontrast LCD"
#define TR_ALARMS_LABEL        "Alarmy"
#define TR_BATTERY_RANGE       "Ukazatel bat."
#define TR_BATTERYWARNING      INDENT "vybit\201 baterie"
#define TR_INACTIVITYALARM     INDENT "ne\202innost"
#define TR_MEMORYWARNING       INDENT "pln\201 pam\206\220"
#define TR_ALARMWARNING        INDENT "vypnut\226 zvuk"
#define TR_RENAVIG             "Navig. rot.enk."
#define TR_MINUTEBEEP          INDENT "minuta"
#define TR_BEEPCOUNTDOWN       INDENT "odpo\202et"
#define TR_PERSISTENT          INDENT "trval\204"
#define TR_BACKLIGHT_LABEL     "Podsv\206tlen\210"
#define TR_BLDELAY             INDENT "zhasnout po"
#define TR_SPLASHSCREEN        "\221vodn\210 logo"
#define TR_THROTTLEWARNING     "* Plyn"
#define TR_SWITCHWARNING       "* Sp\210na\202e"
#define TR_TIMEZONE            "\202asov\204 p\201smo"
#define TR_ADJUST_RTC          "Upravit RTC"
#define TR_GPS                 "GPS"
#define TR_RXCHANNELORD        "Po\215ad\210 kan\201l\224"
#define TR_STICKS              "P\201ky"
#define TR_POTS                "Potenciometry"
#define TR_SLAVE               "Pod\215\210zen\226"
#define TR_MODESRC             "Re\227im     %  Zdroj"
#define TR_MULTIPLIER          "N\201sobi\202"
#define TR_CAL                 "Kalib."
#define TR_VTRIM               "Trim- +"
#define TR_MENUTOSTART         CENTER "\011" TR_ENTER " = START"
#define TR_SETMIDPOINT         "\003Nastav p\201ky na st\215ed"
#define TR_MOVESTICKSPOTS      "\005H\226bej p\201kami/poty"
#define TR_RXBATT              "Rx Bat.:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              CENTER "\227\201dn\201 data !"
#define TR_TOTTM1TM2THRTHP     "\037\146SES\036TM1\037\146TM2\036THR\037\146TH%"
#define TR_US                  "us"
#define TR_MENUTORESET         CENTER TR_ENTER " >> reset"
#define TR_PPM_TRAINER         "TR"
#define TR_CH                  "CH"
#define TR_MODEL               "MODEL"
#define TR_FP                  "LR"
#define TR_MIX                 "MIX"
#define TR_EEPROMLOWMEM        "Doch\201z\210 EEPROM"
#define TR_ALERT               "\006   POZOR"
#define TR_PRESSANYKEYTOSKIP   "\002Kl\201vesa >>> p\215esko\202it"
#define TR_THROTTLENOTIDLE     "\002P\201ka plynu je pohnut\201 !"
#define TR_ALARMSDISABLED      "\002Alarmy jsou zak\201z\201ny !"
#define TR_PRESSANYKEY         "\002Stiskni kl\201vesu"
#define TR_BADEEPROMDATA       "\006Chyba dat EEPROM"
#define TR_EEPROMFORMATTING    "\004Formatov\201n\210 EEPROM"
#define TR_EEPROMOVERFLOW      "P\215etekla EEPROM"
#define TR_MENURADIOSETUP      "NASTAVEN\207 R\200DIA"
#define TR_MENUTRAINER         "TREN\203R"
#define TR_MENUVERSION         "VERZE FIRMWARE"
#define TR_MENUDIAG            "DIAG."
#define TR_MENUANA             "ANALOGY"
#define TR_MENUCALIBRATION     "KALIBRACE P\200K/POT\223"
#define TR_TRIMS2OFFSETS       "\006Trimy => Subtrimy"
#define TR_MENUMODELSEL        "MODEL"
#define TR_MENUSETUP           "NASTAVEN\207"
#define TR_MENUFLIGHTPHASE     "LETOV\225 RE\227IM"
#define TR_MENUFLIGHTPHASES    "LETOV\203 RE\227IMY"
#define TR_MENUHELISETUP       "HELI"
#define TR_MENULIMITS          "SERVA"
#define TR_MENUINPUTS          "DR/EXPO"
#define TR_MENUCURVES          "K\214IVKY"
#define TR_MENUCURVE           "\002K"
#define TR_MENULOGICALSWITCHES "LOGICK\203 SP\207NA\202E"
#define TR_MENUCUSTOMFUNC      "SPECI\200LN\207 FUNKCE"
#define TR_MENUTELEMETRY       "TELEMETRIE"
#define TR_MENUTEMPLATES       "\216ABLONY"
#define TR_MENUSTAT            "STATISTIKA"
#define TR_MENUDEBUG           "DIAGNOSTIKA"
#define TR_RXNUM               "RX \202\210slo"
#define TR_LIMIT       INDENT  "Limit"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Zem. \216\210\215ka"
#define TR_LONGITUDE           "Zem. d\204lka"
#define TR_GPSCOORD            "GPS sou\215adnice"
#define TR_VARIO               "Vario"
#define TR_POWEROFF            "Opravdu vypnout ?"
#define TR_SHUTDOWN            "\007Vyp\210n\201n\210..."
#define TR_SAVEMODEL           "\005Ukl\201d\201m model..."
#define TR_BATT_CALIB          "Kalib. baterie"
#define TR_VOLTAGE     INDENT  "Nap\206t\210"
#define TR_CURRENT     INDENT  "Proud"
#define TR_SELECT_MODEL        "Vyber model"
#define TR_CREATE_MODEL        "Nov\226 model"
#define TR_COPY_MODEL          "Kop\210rovat"
#define TR_MOVE_MODEL          "P\215esunout"
#define TR_BACKUP_MODEL        "Z\201lohovat na SD"
#define TR_DELETE_MODEL        "Sma\227 model"
#define TR_RESTORE_MODEL       "Obnov model z SD"
#define TR_SDCARD_ERROR        "Chyba SD karty"
#define TR_NO_SDCARD           "Nen\210 SD karta"
#define TR_INCOMPATIBLE        "Nekompatibiln\210"
#define TR_WARNING             "KONTROLA"
#define TR_EEPROMWARN          "EEPROM"
#define TR_THROTTLEWARN        "PLYNU"
#define TR_ALARMSWARN          "ALARMU"
#define TR_SWITCHWARN          "POZICE"
#define TR_INVERT_THR          "Invertovat plyn?"
#define TR_SPEAKER_VOLUME      INDENT "Hlasitost"
#define TR_FUNC                "Fce."
#define TR_DELAY               "Zdr\227et"
#define TR_SD_CARD             "SD KARTA"
#define TR_NO_MODELS_ON_SD     "\227\201dn\226 model" BREAKSPACE "na SD"
#define TR_PLAY_FILE           "P\215ehr\201t"
#define TR_DELETE_FILE         "Odstranit"
#define TR_COPY_FILE           "Kop\210rovat"
#define TR_RENAME_FILE         "P\215ejmenovat"
#define TR_REMOVED             " odstran\206n"
#define TR_SD_INFO             "Informace"
#define TR_NA                  "[X]"
#define TR_TIME                "\202as"
#if defined(IMPERIAL_UNITS)
 #define TR_TXTEMP             "Teplota TX\037\164@F"
#else
 #define TR_TXTEMP             "Teplota TX\037\164@C"
#endif
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "typ:"
#define TR_SD_SIZE             "Velikost:"
#define TR_TYPE                INDENT TR_SD_TYPE
#define TR_GLOBAL_VARS         "Glob\201ln\210 prom\206nn\204"
#define TR_GLOBAL_V            "GLOB.PROM."
#define TR_GLOBAL_VAR          "Glob\201ln\210 prom\206nn\201"
#define TR_OWN                 " \043 "
#define TR_DATE                "Datum"
#define TR_ROTARY_ENCODER      "Rot.enk"
#define TR_EDIT                "Upravit"
#define TR_INSERT_BEFORE       "Vlo\227it p\215ed"
#define TR_INSERT_AFTER        "Vlo\227it za"
#define TR_COPY                "Kop\210rovat"
#define TR_MOVE                "P\215esunout"
#define TR_PASTE               "Vlo\227it"
#define TR_DELETE              "Odstranit"
#define TR_INSERT              "P\215idat"
#define TR_RESET_FLIGHT        "            Let"
#define TR_RESET_TIMER1        "Reset: Stopky 1"
#define TR_RESET_TIMER2        "            Stopky 2"
#define TR_RESET_TELEMETRY     "            Telemetrie"
#define TR_STATISTICS          "Statistika"
#define TR_SAVE_TIMERS         "Ulo\227it stopky"
#define TR_SPEAKER             INDENT "Repro"
#define TR_MODULE_BIND         "[bnd]"
#define TR_MODULE_RANGE        "[tst]"
#define TR_BUZZER              INDENT "P\210p\201k"
#define TR_RESET_BTN           "[reset]"
#define TR_SET                 "[volby]"
#define TR_RESET               "Reset"
#define TR_FAS_OFFSET          INDENT "FAS Ofs"
#if defined(X_ANY) || defined(BLUETOOTH)
 #define TR_X_ANY                        "X ANY"
 #define TR_ACTIVED                      "Aktivovat"
 #define TR_NUMBER                       "\202\210slo"
 #define TR_CHANNEL                      "Kan\201l "
 #define TR_NB_REPEAT                    "Nb opakovat "
 #define LEN_SWITCHES_VALUES             "\005"
 #define TR_SWITCHES_VALUES              "Sp.--""L1-L4""L1-L8""Sp.4 ""Sp.8 ""Sp.16"
 #define TR_ANGLE_SENSOR                 "0:360"
 #define TR_PROP                         "Vlst."
 #define LEN_XANY_EXPO                   "\005"
 #define TR_XANY_EXPO                    "0%\0  ""25%\0 ""37.5%""50%\0 "
#endif
#if defined(BLUETOOTH)
 #define TR_BLUETOOTH                    "BLUETOOTH"
 #define TR_NOBLUETOOTH                  "\3NO BLUETOOTH"
 #define TR_CONNECTED                    "\036\5CONECTEd"
 #define TR_BT_ROLE                      "Role"
 #define TR_BT_PIN                       "Pin"
 #define TR_BT_M_S                       "\006""slave\0""master"
 #define TR_BT_PAIR                      "P\201rovat"
 #define TR_AUTOCON                      "Auto-kon."
 #define TR_RESCANN                      "Znovu skenovat"
#endif
