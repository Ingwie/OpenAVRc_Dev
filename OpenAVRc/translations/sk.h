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
#define TR_OFFON               "VYP""ZAP"
#define LEN_MMMINV             "\003"
#define TR_MMMINV              "---""INV"
#define LEN_NCHANNELS          "\004"
#define TR_NCHANNELS           "\0014CH\0016CH\0018CH10CH12CH14CH16CH"
#define LEN_VBEEPMODE          "\005"
#define TR_VBEEPMODE           "Tich\212""Alarm""BezKl""V\207e\0"
#define LEN_VRENAVIG           "\003"
#define TR_VRENAVIG            "Ne REaREb"
#define LEN_VBLMODE            "\005"
#define TR_VBLMODE             "Vyp\0 ""Kl\200v.""P\200ky\0""V\207e\0 ""Zap\0 "
#define LEN_TRNMODE            "\003"
#define TR_TRNMODE             " X "" +="" :="
#define LEN_TRNCHN             "\003"
#define TR_TRNCHN              "CH1CH2CH3CH4"
#define LEN_VPERSISTENT        "\014"
#define TR_VPERSISTENT         "Nie\0        ""V r\200mci letu""Reset ru\201ne\0"
#define LEN_DSM_PROTOCOLS      "\004"
#define TR_DSM_PROTOCOLS       "LP45""DSM2""DSMX"
#define LEN_VTRIMINC           "\007"
#define TR_VTRIMINC            "Expo\0  ""ExJemn\212""Jemn\212\0 ""Stredn\212""Hrub\212\0 "
#define LEN_CURVE_TYPES        "\010"
#define TR_CURVE_TYPES         "Rastr-X\0""Voln\200-XY"
#define LEN_RETA123            "\001"
#define TR_RETA123           "SVPK123ab"

#if defined(PWM_BACKLIGHT)
#define TR_BLONBRIGHTNESS      INDENT"Jas Zap."
#define TR_BLOFFBRIGHTNESS     INDENT"Jas Vyp."
#endif
#if (SERIAL_PROTOCOL==DSM2)
 #define TR_DSM2            "LP45\0 ""DSM2\0 ""DSMX\0 "
#else
 #define TR_DSM2
#endif
#if defined(SPIMODULES)
 #define TR_SPIM               "SPIRfMod"

 #define TR_RFPOWER INDENT     "Vf V\212kon"
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
#define LEN_VCURVEFUNC         "\003"
#define TR_VCURVEFUNC          "---""x>0""x<0""|x|""f>0""f<0""|f|"
#define LEN_VMLTPX             "\007"
#define TR_VMLTPX              "S\201\204ta\221\0""N\200sobi\221""Zameni\221"
#define LEN_VMLTPX2            "\002"
#define TR_VMLTPX2             "+=""*="":="
#define LEN_VMIXTRIMS          "\004"
#define TR_VMIXTRIMS           "VYP\0""ZAP\0""Smer""V\212\207k""Plyn""Kr\204d"
#define TR_CSWTIMER          "Tim\0 "
#define TR_CSWSTICKY         "Glue\0"
#define TR_CSWRANGE
#define TR_CSWSTAY
#define LEN_VCSWFUNC           "\005"
#define TR_VCSWFUNC            "---\0 " "a=x\0 " "a\173x\0 ""a>x\0 ""a<x\0 " TR_CSWRANGE "|a|>x""|a|<x""AND\0 ""OR\0  ""XOR\0 " TR_CSWSTAY "a=b\0 ""a>b\0 ""a<b\0 ""^}x\0 ""|^|}x" TR_CSWTIMER TR_CSWSTICKY
#define LEN_VFSWFUNC           "\013"
#if defined(VARIO)
 #define TR_VVARIO            "Vario\0     "
#else
 #define TR_VVARIO            "[Vario]\0   "
#endif
#if defined(AUDIO)
 #define TR_SOUND             "\221\222Zvuk\0    "
#else
 #define TR_SOUND             "P\204pnu\221\0    "
#endif
#if defined(HAPTIC)
 #define TR_HAPTIC            "Vibrova\221\0  "
#else
 #define TR_HAPTIC            "[Vibrova\221]\0"
#endif
#if defined(VOICE)
 #define TR_PLAY_TRACK      "\221\222Stopa\0   "
 #define TR_PLAY_BOTH         "\221\222P\200r st\223p\0"
 #define TR_PLAY_VALUE        "\221\222Hl\200sit\0  "
#else
 #define TR_PLAY_TRACK        "[\221\222Stopa]\0 "
 #define TR_PLAY_BOTH         "[\221\222P\200r]\0   "
 #define TR_PLAY_VALUE        "[\221\222Hl\200sit]\0"
#endif
#if defined(SDCARD)
 #define TR_SDCLOGS           "Loguj na SD"
#else
 #define TR_SDCLOGS           "[Logovanie]\0"
#endif
#ifdef GVARS
 #define TR_ADJUST_GVAR       "Nastav \0   "
#else
 #define TR_ADJUST_GVAR       "[Nastav GP]"
#endif

#if defined(OVERRIDE_CHANNEL_FUNCTION)
 #define TR_SF_SAFETY        "Z\200mok \0    "
#else
 #define TR_SF_SAFETY        "---\0       "
#endif

#define TR_VFSWFUNC          TR_SF_SAFETY "Tr\202ner \0   ""Insta-Trim\0""Reset\0     " TR_ADJUST_GVAR TR_SOUND TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_VVARIO TR_HAPTIC TR_SDCLOGS "Podsv\203tlen\204"
#define LEN_VFSWRESET          "\004"
#if defined(FRSKY)
 #define TR_FSW_RESET_TELEM   "Telm"
 #define TR_MODELISON         INDENT "modelu is ON"
#else
 #define TR_FSW_RESET_TELEM
#endif
#define TR_FSW_RESET_TIMERS  "Tmr1""Tmr2"
#define TR_FSW_RESET_ROTENC  "REa\0""REb\0"
#define TR_VFSWRESET           TR_FSW_RESET_TIMERS "V\207e\0" TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC
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
#define LEN_AMPSRC           "\003"
#define TR_AMPSRC            "---""A1\0""A2\0""FAS""Cel"
#define LEN_VARIOSRC           "\004"
#if defined(FRSKY_SPORT)
 #define TR_VARIOSRC          "VSpd""A1\0 ""A2\0 ""dTE\0"
#else
 #define TR_VARIOSRC          "Alt\0""Alt+""VSpd""A1\0 ""A2\0"
#endif
#define LEN_VTELEMSCREENTYPE "\010"
#define TR_VTELEMSCREENTYPE  "Hodnota ""Ukazate\224"
#define LEN_GPSFORMAT          "\004"
#define TR_GPSFORMAT           "DMS\0""NMEA"
#define LEN2_VTEMPLATES        13
#define LEN_VTEMPLATES         "\015"
#define TR_VTEMPLATES          "Zmaza\221 mixy\0\0""Z\200kl. 4kan\200l\0""Sticky-T-Cut\0""V-Tail      \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"
#define LEN_VSWASHTYPE         "\004"
#define TR_VSWASHTYPE          "---\0""120\0""120X""140\0""90\0"
#define LEN_VKEYS              "\005"
#define TR_VKEYS               "Menu\0""Exit\0""Dole\0""Hore ""Vprvo""V\224avo"
#define LEN_VRENCODERS         "\003"
#define TR_VRENCODERS          "REa""REb"
#define LEN_INPUTNAMES         "\004"
#define TR_INPUTNAMES          "Smer""V\212\207k""Plyn""Krid"
#define TR_STICKS_VSRCRAW      "Smer""V\212\207k""Plyn""Krid"
#define TR_TRIMS_VSRCRAW       "TrmS""TrmV""TrmP""TrmK"
#define TR_POTS_VSRCRAW      "POT1""POT2""POT3"
#define TR_SW_VSRCRAW        "3POS"
#define TR_9X_3POS_SWITCHES  "ID0""ID1""ID2"
#define TR_LOGICALSW         "L1\0""L2\0""L3\0""L4\0""L5\0""L6\0""L7\0""L8\0""L9\0""L10""L11""L12""L13""L14""L15""L16""L17""L18""L19""L20"
#define TR_TRIMS_SWITCHES      "tSl""tSp""tVd""tVn""tPd""tPn""tKl""tKp"
#define TR_ROTARY_ENCODERS   "REa\0""REb\0"
#define TR_ROTENC_SWITCHES   "REA""REB""REN"
#define TR_PHYS_SWITCHES     "THR""RUD""ELE""AIL""GEA""TRN"
#define TR_ON_ONE_SWITCHES     "ZAP""One"
#define TR_EXTRA_3POS_SWITCHES  "XD0""XD1""XD2"
#define LEN_VSRCRAW            "\004"
#define LEN_VSWITCHES          "\003"
#define TR_VSWITCHES         "---" TR_9X_3POS_SWITCHES TR_PHYS_SWITCHES TR_TRIMS_SWITCHES TR_ROTENC_SWITCHES TR_EXTRA_3POS_SWITCHES TR_LOGICALSW TR_ON_ONE_SWITCHES
#if defined(HELI)
 #define TR_CYC_VSRCRAW       "CYC1""CYC2""CYC3"
#else
 #define TR_CYC_VSRCRAW       "[C1]""[C2]""[C3]"
#endif
#define TR_EXTRA_VSRCRAW
#define TR_VSRCRAW             "---\0" TR_STICKS_VSRCRAW TR_POTS_VSRCRAW TR_ROTARY_ENCODERS "MAX\0" TR_CYC_VSRCRAW TR_TRIMS_VSRCRAW TR_SW_VSRCRAW TR_EXTRA_VSRCRAW
#define LEN_VTMRMODES          "\004"
#define TR_VTMRMODES           "VYP\0""ZAP\0""Pln>""Pln%""Pln*"

/* ZERO TERMINATED STRINGS */
#define INDENT                 "\001"
#define LEN_INDENT             1
#define INDENT_WIDTH           (FW/2)
#define BREAKSPACE             " "
#define TR_ENTER             "[MENU]"
#define TR_EXIT                "[EXIT]"
#define TR_POPUPS            TR_ENTER "\010" TR_EXIT
#define OFS_EXIT             sizeof(TR_ENTER)
#define TR_MENUWHENDONE        CENTER "\011" TR_ENTER " > ĎAL\207\214"
#define TR_FREE                "voln\202:"
#define TR_DELETEMODEL         "ZMAZAŤ" BREAKSPACE "MODEL"
#define TR_COPYINGMODEL        "Kop\204rujem model.."
#define TR_MOVINGMODEL         "Pres\210vam model.."
#define TR_LOADINGMODEL        "Aktivujem model.."
#define TR_NAME                "N\200zov"
#define TR_TIMER               "Stopky"
#define TR_ELIMITS             "Limit +25%"
#define TR_ETRIMS              "\207ir\207\204 Trim"
#define TR_TRIMINC             "Krok Trimu"
#define TR_TTRACE              "StopaPlynu"
#define TR_TSWITCH             "T-Switch"
#define TR_TTRIM               "TrimVolnob."
#define TR_BEEPCTR             "Stredy \221\222"
#define TR_PROTO               INDENT "Protokol"
#define TR_PPMFRAME          "PPM frame"
#define TR_MS                  "ms"
#define TR_SWITCH              "Sp\204na\201"
#define TR_TRIMS               "Trimy"
#define TR_FADEIN              "Prechod Zap"
#define TR_FADEOUT             "Prechod Vyp"
#define TR_DEFAULT             "(v\212chodzie)"
#define TR_CHECKTRIMS          "\011Kont.\010Trimy"
#define OFS_CHECKTRIMS         (9*FW)
#define TR_SWASHTYPE           "Typ cykliky"
#define TR_COLLECTIVE          "Kolekt\204v"
#define TR_SWASHRING           "Cyklika"
#define TR_ELEDIRECTION        "Smer:V\212\207kovka"
#define TR_AILDIRECTION        "\012Kr\204dielka"
#define TR_COLDIRECTION        "\012Kolekt\204v"
#define TR_MODE                INDENT"M\205d"
#define TR_NOFREEEXPO          "Neje vo\224n\202 expo!"
#define TR_NOFREEMIXER         "Neje vo\224n\212 mix!"
#define TR_INSERTMIX           "VLO\217IT MIX "
#define TR_EDITMIX             "UPRAVIT MIX "
#define TR_SOURCE              INDENT"Zdroj"
#define TR_WEIGHT              "V\200ha"
#define TR_EXPO                "Expo"
#define TR_SIDE                "Strana"
#define TR_DIFFERENTIAL        "Dif.v\212chyliek"
#define TR_OFFSET              INDENT"Ofset"
#define TR_TRIM                "Trim"
#define TR_DREX                "DR/Expo"
#define DREX_CHBOX_OFFSET      45
#define TR_CURVE               "Krivka"
#define TR_FLMODE              "Re\217im"
#define TR_MIXWARNING          "Varovanie"
#define TR_OFF                 "VYP"
#define TR_MULTPX              "Mat.oper\200cie"
#define TR_DELAYDOWN           "Spozdenie Vyp"
#define TR_DELAYUP             "Spozdenie Zap"
#define TR_SLOWDOWN            "Spomalenie(-)"
#define TR_SLOWUP              "Spomalenie(+)"
#define TR_MIXER               "MIXER"
#define TR_CV                  "K"
#define TR_GV                  "GP"
#define TR_ACHANNEL            "A\004Kan\200l"
#define TR_RANGE               INDENT"Rozsah"
#define TR_ALARM               INDENT"Alarm"
#define TR_USRDATA             "U\217ivData"
#define TR_BLADES              INDENT"ListyVrt"
#define TR_SCREEN              "Panel "
#define TR_SOUND_LABEL         "Zvuk"
#define TR_LENGTH              INDENT "Dl\217ka"
#define TR_BEEP_LENGTH         INDENT "Dl\217ka"
#define TR_SPKRPITCH           INDENT "T\205n"
#define TR_HAPTIC_LABEL        "Vibr\200cie"
#define TR_HAPTICSTRENGTH      INDENT"Sila"
#define TR_CONTRAST            "Kontrast LCD"
#define TR_ALARMS_LABEL        "Alarmy"
#define TR_BATTERY_RANGE       "Ukazatel bat."
#define TR_BATTERYWARNING      INDENT"Vybit\200 bat\202ria"
#define TR_INACTIVITYALARM     INDENT "Ne\201innos\221"
#define TR_MEMORYWARNING       INDENT"Pln\200 pam\222\221'"
#define TR_ALARMWARNING        INDENT "Vypnut\212 zvuk"
#define TR_RENAVIG             "Navig. RotEnc"
#define TR_MINUTEBEEP          INDENT"Min\210ta"
#define TR_BEEPCOUNTDOWN       INDENT"Odpo\201et"
#define TR_PERSISTENT          INDENT"Trval\202"
#define TR_BACKLIGHT_LABEL     "Podsvietenie"
#define TR_BLDELAY             INDENT"Zhasn\210\221 po"
#define TR_SPLASHSCREEN        "\210vodn\202Logo"
#define TR_THROTTLEWARNING     "* Plyn"
#define TR_SWITCHWARNING       "* Sp\204na\201e"
#define TR_TIMEZONE            "\201asov\202 p\200smo"
#define TR_ADJUST_RTC          "Upravi\221 RTC"
#define TR_GPS                 "GPS"
#define TR_RXCHANNELORD        "Poradie kan\200lov"
#define TR_STICKS              "P\200ky"
#define TR_POTS                "Potenciometre"
#define TR_SLAVE               "Podriaden\212"
#define TR_MODESRC             " M\205d\006% Zdroj"
#define TR_MULTIPLIER          "N\200sobi\201"
#define TR_CAL                 "Kal."
#define TR_VTRIM               "Trim- +"
#define TR_MENUTOSTART         CENTER "\011" TR_ENTER " = START"
#define TR_SETMIDPOINT         CENTER "\004Nastav p\200ky na stred"
#define TR_MOVESTICKSPOTS      CENTER "\006H\212b p\200kami/poty"
#define TR_RXBATT              "Rx Bat.:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              CENTER "NO DATA"
#define TR_TOTTM1TM2THRTHP     "\037\146SES\036TM1\037\146TM2\036THR\037\146TH%"
#define TR_US                 "us"
#define TR_MENUTORESET         CENTER TR_ENTER" >> Reset"
#define TR_PPM_TRAINER         "TR"
#define TR_CH                  "CH"
#define TR_MODEL               "MODELU"
#define TR_FP                  "LR"
#define TR_MIX                 "MIX"
#define TR_EEPROMLOWMEM        "M\200lo EEPROM"
#define TR_ALERT               "\006   POZOR"
#define TR_PRESSANYKEYTOSKIP   "\003Kl\200vesa >>> presko\201i\221"
#define TR_THROTTLENOTIDLE     "\003P\200ka plynu je pohnut\200"
#define TR_ALARMSDISABLED      "Alarmy s\210 zak\200zan\202"
#define TR_PRESSANYKEY         "\006Stla\201 kl\200vesu"
#define TR_BADEEPROMDATA       "\006Chyba d\200t EEPROM"
#define TR_EEPROMFORMATTING    "\004Formatovanie EEPROM"
#define TR_EEPROMOVERFLOW      "Pretiekla EEPROM"
#define TR_MENURADIOSETUP      "NASTAVENIE R\213DIA"
#define TR_MENUTRAINER         "TREN\220R"
#define TR_MENUVERSION         "VERZIA"
#define TR_MENUDIAG            "DIAG"
#define TR_MENUANA             "ANALOGY"
#define TR_MENUCALIBRATION     "KALIBR\213CIA"
#define TR_TRIMS2OFFSETS       "\006Trimy => Subtrimy"
#define TR_MENUMODELSEL        "MODEL"
#define TR_MENUSETUP           "NASTAVENIE"
#define TR_MENUFLIGHTPHASE     "LETOV\216 RE\217IM"
#define TR_MENUFLIGHTPHASES    "LETOV\220 RE\217IMY"
#define TR_MENUHELISETUP       "HELI"
#define TR_MENULIMITS          "SERVA"
#define TR_MENUINPUTS        "DR/EXPO"
#define TR_MENUCURVES          "KRIVKY"
#define TR_MENUCURVE           "KRIVKA"
#define TR_MENULOGICALSWITCHES "LOGICK\220 SP\214NA\201E"
#define TR_MENUCUSTOMFUNC      "\207PECI\213LNE FUNKCIE"
#define TR_MENUTELEMETRY       "TELEMETRIA"
#define TR_MENUTEMPLATES       "\207ABLÓNY"
#define TR_MENUSTAT            "\207TATISTIKA"
#define TR_MENUDEBUG           "DIAG"
#define TR_RXNUM             "RX \201\204slo"
#define TR_LIMIT               INDENT"Limit"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Zem. \207\204rka"
#define TR_LONGITUDE           "Zem. dĺ\217ka"
#define TR_GPSCOORD            "GPS s\210radnice"
#define TR_VARIO               "Vario"
#define TR_POWEROFF            "\006SHUT DOWN ?"
#define TR_SHUTDOWN            "Vyp\204nanie."
#define TR_SAVEMODEL           "Ukl\200d\200m nastavenie modelu"
#define TR_BATT_CALIB          "Kalib. bat\202rie"
#define TR_VOLTAGE             INDENT"Nap\222tia"
#define TR_CURRENT             INDENT"Pr\210d"
#define TR_SELECT_MODEL        "Vyber model"
#define TR_CREATE_MODEL        "Nov\212 model"
#define TR_COPY_MODEL          "Kop\204rova\221"
#define TR_MOVE_MODEL          "Presun\210\221"
#define TR_BACKUP_MODEL        "Z\200lohova\221 na SD"
#define TR_DELETE_MODEL        "Zma\217 model"
#define TR_RESTORE_MODEL       "Obnov model z SD"
#define TR_SDCARD_ERROR        "Chyba SD karty"
#define TR_NO_SDCARD           "Neje SD karta"
#define TR_INCOMPATIBLE        "Nekompatibiln\200"
#define TR_WARNING             "KONTROLA"
#define TR_EEPROMWARN          "EEPROM"
#define TR_THROTTLEWARN        "PLYNU"
#define TR_ALARMSWARN          "ALARMU"
#define TR_SWITCHWARN          "POZICIA"
#define TR_INVERT_THR          "Invertovat plyn?"
#define TR_SPEAKER_VOLUME      INDENT "Hlasitos\221"
#define TR_FUNC                "Fce."
#define TR_DELAY               "Zdr\217at"
#define TR_SD_CARD             "SD"
#define TR_NO_MODELS_ON_SD     "\217iadn\212 model" BREAKSPACE "na SD"
#define TR_PLAY_FILE           "Prehra\221"
#define TR_DELETE_FILE         "Odstr\200ni\221"
#define TR_COPY_FILE           "Kop\204rova\221"
#define TR_RENAME_FILE         "Prejmenova\221"
#define TR_REMOVED             " odstr\200nen\212"
#define TR_SD_INFO             "Inform\200cie"
#define TR_NA                  "[X]"
#define TR_TIME                "\201as"
#if defined(IMPERIAL_UNITS)
 #define TR_TXTEMP              "Temp. TX\037\164@F"
#else
 #define TR_TXTEMP              "Temp. TX\037\164@C"
#endif
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Typ:"
#define TR_SD_SIZE             "Ve\224kost:"
#define TR_TYPE                INDENT TR_SD_TYPE
#define TR_GLOBAL_VARS         "Glob\200lne premenn\202"
#define TR_GLOBAL_V            "GLOB.PROM."
#define TR_GLOBAL_VAR          "Glob\200lna premenn\200"
#define TR_OWN                 " \043 "
#define TR_DATE                "D\200tum"
#define TR_ROTARY_ENCODER      "R.Encs"
#define TR_EDIT                "Upravi\221"
#define TR_INSERT_BEFORE       "Vlo\217i\221 pred"
#define TR_INSERT_AFTER        "Vlo\217i\221 za"
#define TR_COPY                "Kop\204rova\221"
#define TR_MOVE                "Presuno\210\221"
#define TR_PASTE               "Vlo\217i\221"
#define TR_DELETE              "Odstr\200ni\221"
#define TR_INSERT              "Prida\221"
#define TR_RESET_FLIGHT        "Let"
#define TR_RESET_TIMER1        "Stopky1"
#define TR_RESET_TIMER2        "Stopky2"
#define TR_RESET_TELEMETRY     "Telemetrii"
#define TR_STATISTICS          "\207tatistika"
#define TR_SAVE_TIMERS         "Save Timers"
#define TR_SPEAKER             INDENT"Repro"
#define TR_MODULE_BIND         "[Bnd]"
#define TR_MODULE_RANGE        "[Prt]"
#define TR_BUZZER              INDENT"P\204pak"
#define TR_RESET_BTN           "[Reset]"
#define TR_SET                 "[Vo\224by]"
#define TR_RESET               "Reset"
#define TR_CONSTANT            "Kon\207tanta"
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
