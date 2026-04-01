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
*  Formatting octal codes available in TR_ strings:
*  \037\x       - sets LCD x-coord (x value in octal)
*  \036         - newline
*  \001 to \034 - extended spacing (value * FW / 2)
*  \0           - ends current string
*/

//*********************************************************************************************
// NON ZERO TERMINATED STRINGS
//*********************************************************************************************
// lcd, NASTAVEN\207 MODELU menu model setup
#define LEN_VSWITCHES          "\003"
#define TR_VSWITCHES           "---" TR_9X_3POS_SWITCHES TR_PHYS_SWITCHES TR_TRIMS_SWITCHES TR_ROTENC_SWITCHES TR_EXTRA_3POS_SWITCHES TR_LOGICALSW TR_ON_ONE_SWITCHES
#define TR_9X_3POS_SWITCHES    "ID0""ID1""ID2"
#define TR_PHYS_SWITCHES       "THR""RUD""ELE""AIL""GEA""TRN"
#define TR_TRIMS_SWITCHES      "tSl""tSp""tVd""tVn""tPd""tPn""tKl""tKp"
#define TR_ROTENC_SWITCHES     "REa""REb""REn"
#define TR_EXTRA_3POS_SWITCHES "XD0""XD1""XD2"
#define TR_LOGICALSW           "L1\0""L2\0""L3\0""L4\0""L5\0""L6\0""L7\0""L8\0""L9\0""L10""L11""L12""L13""L14""L15" //"L16""L17""L18""L19""L20"
#define TR_ON_ONE_SWITCHES     "ZAP""One"
// lcd, X-ANY menu model Xany
#define LEN_VSRCRAW        "\004"
#define TR_VSRCRAW         "---\0" TR_STICKS_VSRCRAW TR_POTS_VSRCRAW TR_ROTARY_ENCODERS "MAX\0" TR_CYC_VSRCRAW TR_TRIMS_VSRCRAW TR_SW_VSRCRAW TR_EXTRA_VSRCRAW
#define TR_STICKS_VSRCRAW  "Sm\206\0""V\226\216\0""Pln\0""K\215i\0"
#define TR_POTS_VSRCRAW    "POT1""POT2""POT3"
#define TR_ROTARY_ENCODERS "REa\0""REb\0"
#if defined(HELI)
  #define TR_CYC_VSRCRAW   "CYC1""CYC2""CYC3"
#else
  #define TR_CYC_VSRCRAW   "[C1]""[C2]""[C3]"
#endif
#define TR_TRIMS_VSRCRAW   "TrmS""TrmV""TrmP""TrmK"
#define TR_SW_VSRCRAW      "3POS"
#define TR_EXTRA_VSRCRAW
// lcd
#define LEN_VTMRMODES  "\004"
#define TR_VTMRMODES   "VYP\0""ZAP\0""Pln>""Pln%""Pln*"
#define LEN_VCURVEFUNC "\003"
#define TR_VCURVEFUNC  "---""x>0""x<0""|x|""f>0""f<0""|f|"
// lcd
#define LEN_OFFON "\003"
#define TR_OFFON  "VYP""ZAP"


// NASTAVEN\207 MODELU menu model setup
#define LEN_NCHANNELS   "\004"
#define TR_NCHANNELS    "\0014CH\0016CH\0018CH10CH12CH14CH16CH"
#define LEN_VPERSISTENT "\014"
#define TR_VPERSISTENT  "VYP\0        ""V r\201mci letu""Reset ru\202n\206\0"
#define LEN_VTRIMINC    "\007"
#define TR_VTRIMINC     "Expo\0  ""EXjemn\226""Jemn\226\0 ""St\215edn\210""Hrub\226\0 "
#define LEN_RETA123     "\001"
#define TR_RETA123      "SVPK123ab" // lcd
#if (SERIAL_PROTOCOL == DSM)
  #define TR_DSM2       "LP45\0 ""DSM2\0 ""DSMX\0 "
#else
  #define TR_DSM2
#endif


// DSM SERIAL
#define LEN_DSM_PROTOCOLS "\004"
#define TR_DSM_PROTOCOLS  "LP45""DSM2""DSMX"


// MIXER menu model inputs mixes
#define LEN_VMIXTRIMS "\004"
#define TR_VMIXTRIMS  "VYP\0""ZAP\0""Sm\206r""V\226\216k""Plyn""K\215id"
#define LEN_VMLTPX    "\007"
#define TR_VMLTPX     "Se\202\210st\0""N\201sobit""Zam\206nit"
#define LEN_VMLTPX2   "\002"
#define TR_VMLTPX2    "+=""*="":="


// lcd, SPECI\200LN\207 FUNKCE menu model custom functions, HELI menu model heli, SERVA menu model limits
#define LEN_MMMINV "\003"
#define TR_MMMINV  "---""Rev"


// LOGICK\203 SP\207NA\202E menu model logical switches, view telemetry
#define LEN_VCSWFUNC "\005"
#define TR_VCSWFUNC  "---\0 ""a=x\0 ""a\173x\0 ""a>x\0 ""a<x\0 " TR_CSWRANGE "|a|>x""|a|<x""AND\0 ""OR\0  ""XOR\0 " TR_CSWSTAY "a=b\0 ""a>b\0 ""a<b\0 ""^}x\0 ""|^|}x" TR_CSWTIMER TR_CSWSTICKY
#define TR_CSWRANGE
#define TR_CSWSTAY
#define TR_CSWTIMER  "\202as\0 "
#define TR_CSWSTICKY "Lep\210\0"


// SPECI\200LN\207 FUNKCE menu model custom functions
#define LEN_VFSWFUNC     "\013"
#define TR_VFSWFUNC TR_SF_SAFETY "Tren\204r \0   ""Insta-trim\0""Reset\0     " TR_ADJUST_GVAR TR_SOUND TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_VVARIO TR_HAPTIC TR_SDCLOGS "Podsv\206tlen\210"
#if defined(OVERRIDE_CHANNEL_FUNCTION)
  #define TR_SF_SAFETY   "Z\201mek \0    "
#else
  #define TR_SF_SAFETY   "---\0       "
#endif
#ifdef GVARS
  #define TR_ADJUST_GVAR "Nastav GP\0 "
#else
  #define TR_ADJUST_GVAR "[Nastav GP]"
#endif
#if defined(AUDIO)
  #define TR_SOUND       "Zvuk\0      "
#else
  #define TR_SOUND       "P\210pnout\0   "
#endif
#if defined(VOICE)
  #define TR_PLAY_TRACK  "Stopa\0     "
  #define TR_PLAY_BOTH   "P\201r stop\0  "
  #define TR_PLAY_VALUE  "Hl\201sit\0    "
#else
  #define TR_PLAY_TRACK  "[Stopa]\0   "
  #define TR_PLAY_BOTH   "[P\201r stop]\0"
  #define TR_PLAY_VALUE  "[Hl\201sit]\0  "
#endif
#if defined(VARIO)
  #define TR_VVARIO      "Vario\0     "
#else
  #define TR_VVARIO      "[Vario]\0   "
#endif
#if defined(HAPTIC)
  #define TR_HAPTIC      "Vibrovat\0  "
#else
  #define TR_HAPTIC      "[Vibrovat]\0"
#endif
#if defined(SDCARD)
  #define TR_SDCLOGS     "Loguj na SD"
#else
  #define TR_SDCLOGS     "[LogujNaSD]"
#endif
//
#define LEN_VFSWRESET         "\004"
#define TR_VFSWRESET TR_FSW_RESET_TIMERS "V\216e\0" TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC
#define TR_FSW_RESET_TIMERS   "\202as1""\202as2"
#if defined(FRSKY)
  #define TR_FSW_RESET_TELEM  "Telm"
  #define TR_MODELISON INDENT "Model je zapnut\226!"
#else
  #define TR_FSW_RESET_TELEM
#endif
#define TR_FSW_RESET_ROTENC   "REa\0""REb\0"
//
#define LEN_FUNCSOUNDS "\004"
#define TR_FUNCSOUNDS  "Bp1\0""Bp2\0""Bp3\0""Wrn1""Wrn2""Chee""Rata""Tick""Sirn""Ring""SciF""Robt""Chrp""Tada""Crck""Alrm"


// lcd, TELEMETRIE menu model telemetry, view telemetry
#define LEN_VTELEMCHNS   "\004"
#define TR_VTELEMCHNS    "---\0""Bat." TR_TELEM_TIME TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_TIMERS TR_SWR "TX\0 " TR_TELEM_RSSI_RX TR_RX_BATT "A1\0 ""A2\0 " TR_A3_A4 "Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""Hdg\0""VSpd""ASpd""dTE\0" TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE "A1-\0""A2-\0" TR_A3_A4_MIN "Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""Dst+" TR_ASPD_MAX "Cel-""Cls-""Vfs-""Cur+""Pwr+" TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE "Acc\0""\202as\0"
#define TR_TELEM_TIME
#define TR_TELEM_RESERVE
#define TR_TELEM_TIMERS  "\202as1""\202as2"
#define TR_SWR
#define TR_TELEM_RSSI_RX "RX\0 "
#define TR_RX_BATT
#define TR_A3_A4
#define TR_A3_A4_MIN
#define TR_ASPD_MAX      "ASp+"


// TELEMETRIE menu model telemetry, view telemetry
#define LEN_AMPSRC "\003"
#define TR_AMPSRC  "---""A1\0""A2\0""FAS""Cel"


// TELEMETRIE menu model telemetry
#define LEN_VTELPROTO        "\007"
#define TR_VTELPROTO         "---\0   ""Hub\0   ""WSHHigh""S.Port\0""iBus\0  "
#define LEN_VTELEMSCREENTYPE "\010"
#define TR_VTELEMSCREENTYPE  "Hodnota ""Ukazatel"
#define LEN_VARIOSRC         "\004"
#if defined(FRSKY_SPORT)
  #define TR_VARIOSRC        "VSpd""A1\0 ""A2\0 ""dTE\0"
#else
  #define TR_VARIOSRC        "Alt\0""Alt+""VSpd""A1\0 ""A2\0 "
#endif


// lcd, TELEMETRIE menu model telemetry
#define STR_V (STR_VTELEMUNIT + 1)
#define STR_A (STR_VTELEMUNIT + 4)
#define LEN_VTELEMUNIT  "\003"
#define TR_VTELEMUNIT   "V\0 ""A\0 ""m/s""-\0 " SPEED_UNIT LENGTH_UNIT "@\0 ""%\0 ""mA\0""mAh""W\0 "
#if defined(IMPERIAL_UNITS)
  #define LENGTH_UNIT LENGTH_UNIT_IMP
  #define SPEED_UNIT  SPEED_UNIT_IMP
#else
  #define LENGTH_UNIT LENGTH_UNIT_METR
  #define SPEED_UNIT  SPEED_UNIT_METR
#endif
#define LENGTH_UNIT_IMP  "ft\0"
#define SPEED_UNIT_IMP   "mph"
#define LENGTH_UNIT_METR "m\0 "
#define SPEED_UNIT_METR  "kmh"


// NASTAVEN\207 R\200DIA menu general setup
#define LEN_VBEEPMODE "\005"
#define TR_VBEEPMODE  "Tich\226""Alarm""BezKl""V\216e\0 "
#define LEN_VRENAVIG  "\003"
#define TR_VRENAVIG   "Ne REaREb"
#define LEN_VBLMODE   "\004"
#define TR_VBLMODE    "VYP\0""Kl\201v""P\201ky""V\216e\0""ZAP\0"
#define LEN_GPSFORMAT "\004"
#define TR_GPSFORMAT  "DMS\0""NMEA"
#if defined(PWM_BACKLIGHT)
  #define TR_BLONBRIGHTNESS  INDENT "Jas ZAP"
  #define TR_BLOFFBRIGHTNESS INDENT "Jas VYP"
#endif


// TREN\203R menu general trainer
#define LEN_TRNCHN  "\003"
#define TR_TRNCHN   "CH1CH2CH3CH4"
#define LEN_TRNMODE "\003"
#define TR_TRNMODE  "VYP"" +="" :="


// DIAG menu general diagkeys
#define LEN_VKEYS      "\005"
#define TR_VKEYS       "Menu\0""Exit\0""Dolu\0""Nhoru""Vprvo""Vlevo"
#define LEN_VRENCODERS "\003"
#define TR_VRENCODERS  "REa""REb"


// HELI menu model heli
#define LEN_VSWASHTYPE "\004"
#define TR_VSWASHTYPE  "---\0""120\0""120X""140\0""90\0"


// \216ABLONY menu model templates
#define LEN2_VTEMPLATES 13
#define LEN_VTEMPLATES  "\016"
#define TR_VTEMPLATES   "Smazat mixy\0  ""Z\201kl. 4kan\201l\0 ""P\201ky-T-Cut\0   ""V-Tail\0       ""Elevon\\Delta\0 ""eCCPM\0        ""Heli setup\0   ""Servo test\0   "


//*********************************************************************************************
// ZERO TERMINATED STRINGS
//*********************************************************************************************
#define INDENT       "\001"
#define LEN_INDENT   1
#define INDENT_WIDTH (FW / 2)
#define OFS_RX       4 // NASTAVEN\207 R\200DIA menu general setup, NASTAVEN\207 MODELU menu model setup, view telemetry


// MODEL menu model select
#define BREAKSPACE         " "
#define TR_DELETEMODEL     "Smazat" BREAKSPACE "model?"
#define TR_NO_MODELS_ON_SD "\227\201dn\226 model" BREAKSPACE "na SD"
#define TR_ENTER           "[MENU]"
#define TR_EXIT            "    [EXIT]"
#define TR_POPUPS TR_ENTER "\010" TR_EXIT
#define OFS_EXIT  sizeof(TR_ENTER)

#define TR_MENUMODELSEL  "MODEL"
#define TR_FREE          "Voln\204:"
#define TR_LOADINGMODEL  "Aktivuji model.."
#define TR_COPYINGMODEL  "Kop\210ruji model.."
#define TR_MOVINGMODEL   "P\215esouv\201m model.."
#define TR_SELECT_MODEL  "Vybrat model"
#define TR_COPY_MODEL    "Kop\210rovat"
#define TR_MOVE_MODEL    "P\215esunout"
#define TR_DELETE_MODEL  "Smazat model"
#define TR_CREATE_MODEL  "Nov\226 model"
#define TR_BACKUP_MODEL  "Z\201lohovat na SD"
#define TR_RESTORE_MODEL "Obnov model z SD"
#define TR_SDCARD_ERROR  "Chyba SD karty"


// NASTAVEN\207 MODELU menu model setup
#define TR_MENUSETUP             "NASTAVEN\207 MODELU"
#define TR_NAME                  "N\201zev"
#define TR_TIMER                 "Stopky "
#define TR_PERSISTENT     INDENT "Trval\204"
#define TR_MINUTEBEEP     INDENT "Minuta"
#define TR_BEEPCOUNTDOWN  INDENT "Odpo\202et"
#define TR_ELIMITS               "Limit +25%"
#define TR_ETRIMS                "\216ir\216\210 trim"
#define TR_TRIMINC               "Krok trimu"
#define TR_TTRACE                "StopaPlynu"
#define TR_TSWITCH               "Sp\210na\202Plyn"
#define TR_TTRIM                 "TrimVolnob."
#define TR_THROTTLEWARNING       "Var. plynu"
#define TR_SWITCHWARNING         "Var. sp\210na\202"
#define TR_BEEPCTR               "P\210p. st\215edu"
#define TR_MODULE                "Modul"
#define TR_NODATA         CENTER "\227\201dn\201 data!"
#define TR_CH                    "CH"
#define TR_PPMFRAME              "PPM r\201mec"
#define TR_MS                    "ms"
#define TR_US                    "us"
#define TR_SPIM                  "SPI RF"
#define TR_PROTO          INDENT "Protokol"
#define TR_SUBPROTO              "Subproto"
#define TR_RXNUM                 "P\215ij\210ma\202"
#define TR_MODULE_BIND           "[bnd]"
#define TR_MODULE_RANGE          "[tst]"
#define TR_NUMCH          INDENT "Po\202et kan\201l\224"
#define TR_RFTUNEFINE     INDENT "Ladit RF"
#define TR_MULTI_LOWPOWER INDENT "N\210zk\226 v\226kon"
#define TR_RFPOWER        INDENT "RF v\226kon"
#define TR_AUTOBIND       INDENT "Autobind"
#define TR_TELEMETRY             "Telemetrie"
#define TR_UBIND          INDENT "Rozv\201zat"
#define TR_PERIOD         INDENT "Latence"
#define TR_RESET_BTN             "[reset]"


// MULTI SERIAL
#define TR_MULTI_SERVOFREQ     INDENT "Frekvence serv"
#define TR_MULTI_VIDFREQ       INDENT "Ladit RF video"
#define TR_MULTI_OPTION        INDENT "Mo\227nost"
#define TR_MULTI_RFPOWER       INDENT "Rf v\226kon"
#define TR_MULTI_FIXEDID              "Pevn\204 ID"
#define TR_MULTI_MAX_THROW            "Max. dojezd"
#define TR_MULTI_RFCHAN               "RF kan\201l"
#define TR_MULTI_OUTPUT               "V\226stup"
#define TR_MULTI_DSM_AUTODTECT INDENT "Autodetekt"


// LETOV\203 F\200ZE monu model flight modes
#define TR_MENUFLIGHTPHASES "LETOV\203 F\200ZE"
#define TR_MENUFLIGHTPHASE  "LETOV\200 F\200ZE"
#define TR_FP               "LF"
#define TR_DEFAULT          "(v\226choz\210)"
#define OFS_CHECKTRIMS      (9 * FW)
#define TR_CHECKTRIMS       "\001Kontrola\013Trimy"
#define TR_SWITCH           "Sp\210na\202"
#define TR_TRIM             "Trim"
#define TR_TRIMS            "Trimy"
#define TR_ROTARY_ENCODER   "Rota\202n\210 enk."
#define TR_FADEIN           "P\215echod ZAP"
#define TR_FADEOUT          "P\215echod VYP"
#define TR_GLOBAL_VARS      "Glob\201ln\210 prom\206nn\204"
#define TR_OWN              "Majitel"


// DR/EXPO menu model inputs mixes
#define TR_MENUINPUTS    "DR/EXPO"
#define TR_EDIT          "Upravit"
#define TR_INSERT_BEFORE "Vlo\227it p\215ed"
#define TR_INSERT_AFTER  "Vlo\227it za"
#define TR_COPY          "Kop\210rovat"
#define TR_MOVE          "P\215esunout"
#define TR_PASTE         "Vlo\227it"
#define TR_DELETE        "Odstranit"
#define TR_WEIGHT        "V\201ha"
#define TR_EXPO          "Expo"
#define TR_CURVE         "K\215ivka"
#define TR_FLMODE        "Re\227im"
#define TR_SIDE          "Strana"
#define TR_NA            "[x]"
#define TR_CV            "K"


// MIXER menu model inputs mixes
#define TR_MIXER          "MIXER"
#define TR_INSERTMIX      "Vlo\227it mix"
#define TR_EDITMIX        "Upravit mix"
#define TR_SOURCE  INDENT "Zdroj"
#define TR_OFFSET  INDENT "Ofset"
#define TR_DREX           "DRex"
#define TR_MIXWARNING     "Varov\201n\210"
#define TR_MULTPX         "Mat. operace"
#define TR_DELAYDOWN      "Zpo\227d\206n\210 VYP"
#define TR_DELAYUP        "Zpo\227d\206n\210 ZAP"
#define TR_SLOWDOWN       "Zpomalen\210 (-)"
#define TR_SLOWUP         "Zpomalen\210 (+)"
#define TR_PPM_TRAINER    "TR"
#define TR_OFF            "VYP"
#define TR_NOFREEEXPO     "Nen\210 voln\204 expo!"
#define TR_NOFREEMIXER    "Nen\210 voln\226 mix!"
#define TR_MIX            "MIX"
#define TR_DELAY          "Zdr\227et"
#define DREX_CHBOX_OFFSET 30


// SERVA menu model limits
#define TR_MENULIMITS    "SERVA"
#define TR_TRIMS2OFFSETS "\007Trimy => Subtrimy"
#define TR_INVERT_THR    "Invertovat plyn?"


// K\214IVKY menu model curves
#define TR_MENUCURVES "K\214IVKY"
#define TR_MENUCURVE  "K\214IVKA"
#define TR_TYPE INDENT TR_SD_TYPE
#define TR_SD_TYPE    "Typ:"


// LOGICK\203 SP\207NA\202E menu model logical svitches
#define TR_MENULOGICALSWITCHES "LOGICK\203 SP\207NA\202E"
#define TR_GV                  "GP"


// SPECI\200LN\207 FUNKCE menu model custom functions
#define TR_MENUCUSTOMFUNC "SPECI\200LN\207 FUNKCE"
#define TR_FUNC           "Funkce"


// TELEMETRIE menu model telemetry
#define TR_MENUTELEMETRY     "TELEMETRIE"
#define TR_ACHANNEL          "A\003kan\201l"
#define TR_RANGE      INDENT "Rozsah"
#define TR_ALARM      INDENT "Alarm"
#define TR_USRDATA           "U\227ivData"
#define TR_BLADES     INDENT "ListyVrt"
#define TR_VOLTAGE    INDENT "Nap\206t\210"
#define TR_CURRENT    INDENT "Proud"
#define TR_FAS_OFFSET INDENT "FAS Ofs"
#define TR_VARIO             "Vario"
#define TR_LIMIT      INDENT "Limit"
#define TR_SCREEN            "Panel"


// NASTAVEN\207 R\200DIA menu general setup
#define TR_MENURADIOSETUP "NASTAVEN\207 R\200DIA"
#define TR_DATE           "Datum"
#define TR_TIME           "\202as"
#if defined(IMPERIAL_UNITS)
  #define TR_TXTEMP       "Teplota TX\037\164@F"
#else
  #define TR_TXTEMP       "Teplota TX\037\164@C"
#endif
#define TR_SOUND_LABEL            "Zvuk"
#define TR_MODE            INDENT "Re\227im"
#define TR_BEEP_LENGTH     INDENT "D\204lka"
#define TR_CONTRAST               "Kontrast LCD"
#define TR_ALARMS_LABEL           "Alarmy"
#define TR_BATTERYWARNING  INDENT "Vybit\201 baterie"
#define TR_INACTIVITYALARM INDENT "Ne\202innost"
#define TR_MEMORYWARNING   INDENT "Pln\201 pam\206\220"
#define TR_ALARMWARNING    INDENT "Vypnut\226 zvuk"
#define TR_RENAVIG                "Navigace rot.enk."
#define TR_BACKLIGHT_LABEL        "Podsv\206tlen\210"
#define TR_BLDELAY         INDENT "Zhasnout po"
#define TR_SPLASHSCREEN           "\221vodn\210 logo"
#define TR_TIMEZONE               "\202asov\204 p\201smo"
#define TR_ADJUST_RTC             "Upravit RTC"
#define TR_GPSCOORD               "GPS sou\215adnice"
#define TR_RXCHANNELORD           "Po\215ad\210 kan\201l\224"
#define TR_LENGTH          INDENT "D\204lka"
#define TR_SPKRPITCH       INDENT "T\213n"
#define TR_HAPTIC_LABEL           "Vibrace"
#define TR_HAPTICSTRENGTH  INDENT "S\210la"
#define TR_BATTERY_RANGE          "Ukazatel bat."
#define TR_GPS                    "GPS"
#define TR_SPEAKER_VOLUME  INDENT "Hlasitost"
#define TR_SPEAKER         INDENT "Repro"
#define TR_BUZZER          INDENT "P\210p\201k"


// TREN\203R menu general trainer
#define TR_MENUTRAINER         "TREN\203R"
#define TR_SLAVE               "\227\201k"
#define TR_MODESRC             "Re\227im     %  Zdroj"
#define TR_MULTIPLIER          "N\201sobi\202"
#define TR_CAL                 "Kalib."


// VERZE FIRMWARE menu general version
#define TR_MENUVERSION "VERZE FIRMWARE"


// DIAG menu general diagkeys
#define TR_MENUDIAG "DIAG"
#define TR_VTRIM    "Trim - +"


// ANALOGOV\203 VSTUPY menu general diaganas
#define TR_MENUANA    "ANALOGOV\203 VSTUPY"
#define TR_BATT_CALIB "Kalibrace bat."


// KALIBRACE P\200K/POT\223 menu general calib
#define TR_MENUCALIBRATION     "KALIBRACE P\200K/POT\223"
#define TR_MENUTOSTART  CENTER "\011" TR_ENTER " = START"
#define TR_MENUWHENDONE CENTER "\011" TR_ENTER " > d\201le"
#define TR_SETMIDPOINT         "\003Nastav p\201ky na st\215ed"
#define TR_MOVESTICKSPOTS      "\005H\226bej p\201kami/poty"
#define TR_SET                 "[volby]"


// HELI menu model heli
#define TR_MENUHELISETUP "HELI"
#define TR_SWASHTYPE     "Typ cykliky"
#define TR_COLLECTIVE    "Kolektiv"
#define TR_SWASHRING     "Cyklika"
#define TR_ELEDIRECTION  "V\226\216kovka"
#define TR_AILDIRECTION  "K\215id\204lka"
#define TR_COLDIRECTION  "Kolektiv"


// \216ABLONY menu model templates
#define TR_MENUTEMPLATES "\216ABLONY"


// SD KARTA menu general sdmanager
#define TR_SD_CARD       "SD KARTA"
#define TR_SD_SIZE       "Velikost:"
#define TR_SD_INFO_TITLE "SD INFO"
#define TR_SD_INFO       "Informace"
#define TR_DELETE_FILE   "Odstranit"
#define TR_REMOVED       "Odstran\206n"
#define TR_NO_SDCARD     "Nen\210 SD karta"


// X-ANY menu model Xany
#if defined(X_ANY) || defined(BLUETOOTH)
  #define TR_X_ANY            "X-ANY"
  #define TR_ACTIVED          "ZAP"
  #define TR_NUMBER           "\202\210slo"
  #define TR_CHANNEL          "Kan\201l"
  #define TR_NB_REPEAT        "Nb opakovat"
  #define LEN_SWITCHES_VALUES "\005"
  #define TR_SWITCHES_VALUES  "Sp.--""L1-L4""L1-L8""Sp.4\0""Sp.8\0""Sp.16"
  #define LEN_XANY_EXPO       "\005"
  #define TR_XANY_EXPO        "0%\0  ""25%\0 ""37.5%""50%\0 "
  #define TR_PROP             "Moje"
  #define TR_ANGLE_SENSOR     "0:360"
#endif


// BLUETOOTH menu general bluetooth
#if defined(BLUETOOTH)
  #define TR_BLUETOOTH   "BLUETOOTH"
  #define TR_NOBLUETOOTH "\227\201dn\204 bluetooth!"
  #define TR_CONNECTED   "\036\005P\215ipojeno"
  #define TR_BT_ROLE     "Role"
  #define TR_BT_PIN      "Pin"
  #define TR_BT_M_S      "\006""\227\201k\0  ""U\202itel"
  #define TR_BT_PAIR     "P\201rovat"
  #define TR_AUTOCON     "Auto p\215ipojen\210"
  #define TR_RESCANN     "Znovu skenovat"
#endif


// M\205NI\202 KAN\200L\223 menu general scc
#if defined(SCC)
  #define TR_SCC                  "M\205NI\202 KAN\200L\223"
  #define TR_SCC_CURCHANNELLBL    "Aktu\201ln\210 kan\201l"
  #define TR_SCC_NEWCHANNELLBL    "Vybrat kan\201l"
  #define TR_SCC_SBUSSWEEPTESTLBL "Test Sbus"
#endif


// STATISTIKA view statistics
#define TR_MENUSTAT        "STATISTIKA"
#define TR_MENUDEBUG       "DIAGNOSTIKA"
#define TR_TOTTM1TM2THRTHP "\037\146SES\036TM1\037\146TM2\036Pln\037\146Pl%"
#define TR_MENUTORESET CENTER TR_ENTER " >> Reset"


// view telemetry
#define TR_MINRSSI   "Min. RSSI"
#define TR_ACCEL     "Acc:"
#define TR_LATITUDE  "Zem. \216\210\215ka"
#define TR_LONGITUDE "Zem. d\204lka"


// view main
#define TR_RESET_FLIGHT    "Resetovat let"
#define TR_RESET_TIMER1    "Reset stopek 1"
#define TR_RESET_TIMER2    "Reset stopek 2"
#define TR_RESET_TELEMETRY "Telemetrie"
#define TR_STATISTICS      "Statistika"
#define TR_SAVE_TIMERS     "Ulo\227it stopky"
#define TR_POWEROFF        "Opravdu vypnout?"


// OpenAVRc, eeprom rlc, popups
#define TR_THROTTLENOTIDLE   "P\201ka plynu je pohnut\201!"
#define TR_ALARMSDISABLED    "Alarmy jsou zak\201z\201ny!"
#define TR_MODEL             "MODEL"
#define TR_WARNING           "KONTROLA"
#define TR_ALARMSWARN        "ALARMU"
#define TR_SWITCHWARN        "POZICE"
#define TR_THROTTLEWARN      "PLYNU"
#define TR_EEPROMWARN        "EEPROM"
#define TR_EEPROMLOWMEM      "Doch\201z\210 EEPROM!"
#define TR_EEPROMFORMATTING  "Form\201tov\201n\210 EEPROM"
#define TR_EEPROMOVERFLOW    "P\215etekla EEPROM!"
#define TR_BADEEPROMDATA     "Chyba dat EEPROM!"
#define TR_INCOMPATIBLE      "Nekompatibiln\210"
#define TR_PRESSANYKEY       "Stiskni kl\201vesu        >>>>>"
#define TR_PRESSANYKEYTOSKIP "Kl\201vesou p\215esko\202it >>>>>"
#define TR_SHUTDOWN          "Vyp\210n\201n\210.."
#define TR_SAVEMODEL         "Ukl\201d\201m model.."


// Unused
#define TR_DIFFERENTIAL "Dif.v\226chylek"
#define TR_STICKS       "P\201ky"
#define TR_POTS         "Potenciometry"
#define TR_RXBATT       "RX Bat.:"
#define TR_TXnRX        "TX:\0RX:"
#define TR_ALERT        "\006POZOR!"
#define TR_PLAY_FILE    "P\215ehr\201t"
#define TR_COPY_FILE    "Kop\210rovat"
#define TR_RENAME_FILE  "P\215ejmenovat"
#define TR_GLOBAL_VAR   "Glob\201ln\210 prom\206nn\201"
#define TR_GLOBAL_V     "GLOB\200LN\207"
#define TR_INSERT       "P\215idat"
