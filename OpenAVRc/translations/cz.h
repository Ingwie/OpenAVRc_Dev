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
#define TR_OFFON               "VYP""ZAP"

#define LEN_MMMINV             "\003"
#define TR_MMMINV              "---""INV"

#define LEN_NCHANNELS          "\004"
#define TR_NCHANNELS           "\0014CH\0016CH\0018CH10CH12CH14CH16CH"

#define LEN_VBEEPMODE          TR("\005","\012")
#define TR_VBEEPMODE           TR("Tich\212""Alarm""BezKl""V\207e\0", "Tich\212\0    ""Jen alarm\0""Bez kl\200ves""V\207e\0      ")

#define LEN_VBEEPLEN           "\005"
#define TR_VBEEPLEN            "O====""=O===""==O==""===O=""====O"

#define LEN_VRENAVIG           "\003"
#define TR_VRENAVIG            "Ne REaREb"

#define LEN_VBLMODE            TR("\005", "\007")
#define TR_VBLMODE             TR("Vyp\0 ""Kl\200v.""P\200ky\0""V\207e\0 ""Zap\0 ", "Vypnuto""Kl\200vesy""P\200ky\0  ""V\207e\0   ""Zapnuto")

#define LEN_TRNMODE            "\003"
#define TR_TRNMODE             " X "" +="" :="

#define LEN_TRNCHN             "\003"
#define TR_TRNCHN              "CH1CH2CH3CH4"

#define LEN_UART3MODES         "\015"
#define TR_UART3MODES          "VYP\0         ""S-Port Mirror""Telemetrie\0  ""SBUS Trainer\0""Debug\0"

#define LEN_SWTYPES            "\013"
#define TR_SWTYPES             "\217\200dn\212\0     ""Bez aretace""2-polohov\212\0""3-polohov\212\0"

#define LEN_POTTYPES           "\016"
#define TR_POTTYPES            "\217\200dn\212\0        ""Pot s aretac\204\0""V\204cepol. p\206ep.""Potenciometr\0 "

#define LEN_SLIDERTYPES        "\006"
#define TR_SLIDERTYPES         "\217\200dn\212\0""Slider"

#define LEN_DATETIME           "\006"
#define TR_DATETIME            "DATUM:""\201AS:  "

#define LEN_VLCD               "\006"
#define TR_VLCD                "NormalOptrex"

#define LEN_VPERSISTENT        "\014"
#define TR_VPERSISTENT         "Ne\0         ""V r\200mci letu""Reset ru\201n\203\0"

#define LEN_COUNTRYCODES       TR("\002", "\010")
#define TR_COUNTRYCODES        TR("US""JP""EU", "Amerika\0""Japonsko""Evropa\0 ")

#define LEN_TARANIS_PROTOCOLS  "\004"
#define TR_TARANIS_PROTOCOLS   "Vyp\0""PPM\0""XJT\0""DSM2""CRSF"

#define LEN_XJT_PROTOCOLS      "\004"
#define TR_XJT_PROTOCOLS       "Vyp\0""D16\0""D8\0 ""LR12"

#define LEN_DSM_PROTOCOLS      "\004"
#define TR_DSM_PROTOCOLS       "LP45""DSM2""DSMX"

#define LEN_VTRIMINC           "\007"
#define TR_VTRIMINC            "Expo\0  ""ExJemn\212""Jemn\212\0 ""St\206edn\204""Hrub\212\0 "

#define LEN_VDISPLAYTRIMS      "\006"
#define TR_VDISPLAYTRIMS       "Ne\0   ""Zm\203na\0""Ano\0  "

#define LEN_VBEEPCOUNTDOWN     "\007"
#define TR_VBEEPCOUNTDOWN      "Ne\0    ""Zvuk\0  ""Hlas\0  ""Vibrace"

#define LEN_VVARIOCENTER       "\005"
#define TR_VVARIOCENTER        "T\205n\0 ""Ticho"

#define LEN_CURVE_TYPES        "\010"
#define TR_CURVE_TYPES         "Rastr-X\0""Voln\200-XY"

#define LEN_RETA123            "\001"

#if defined(CPUM2560)
  #define TR_RETA123           "SVPK123ab"
#else
  #define TR_RETA123           "SVPK123"
#endif

#define LEN_VPROTOS            "\006"

#if defined(PXX)
  #define TR_PXX              "PXX\0  "
#elif defined(DSM2) 
  #define TR_PXX             "[PXX]\0"
#else
  #define TR_PXX
#endif

#if defined(DSM2)
  #define TR_DSM2            "LP45\0 ""DSM2\0 ""DSMX\0 "
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
#define TR_POSNEG              "POZ""NEG"


#define LEN_VCURVEFUNC         "\003"
#define TR_VCURVEFUNC          "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_VMLTPX             "\007"
#define TR_VMLTPX              "Se\201\204st\0""N\200sobit""Zam\203nit"

#define LEN_VMLTPX2            "\002"
#define TR_VMLTPX2             "+=""*="":="

#define LEN_VMIXTRIMS          "\004"
#define TR_VMIXTRIMS           "VYP\0""ZAP\0""Sm\203r""V\212\207k""Plyn""K\206id"

  #define TR_CSWTIMER          "Tim\0 "
  #define TR_CSWSTICKY         "Glue\0"
    #define TR_CSWRANGE
    #define TR_CSWSTAY

  #define TR_CSWEQUAL

#define LEN_VCSWFUNC           "\005"
#define TR_VCSWFUNC            "---\0 " TR_CSWEQUAL "a\173x\0 ""a>x\0 ""a<x\0 " TR_CSWRANGE "|a|>x""|a|<x""AND\0 ""OR\0  ""XOR\0 " TR_CSWSTAY "a=b\0 ""a>b\0 ""a<b\0 ""^}x\0 ""|^|}x" TR_CSWTIMER TR_CSWSTICKY

#define LEN_VFSWFUNC           "\013"

#if defined(VARIO)
  #define TR_VVARIO            "Vario\0     "
#else
  #define TR_VVARIO            "[Vario]\0   "
#endif

#if defined(AUDIO)
  #define TR_SOUND             "\221\222Zvuk\0    "
#else
  #define TR_SOUND             "P\204pnout\0   "
#endif

#if defined(HAPTIC)
  #define TR_HAPTIC            "Vibrovat\0  "
#else
  #define TR_HAPTIC            "[Vibrovat]\0"
#endif

#if defined(VOICE)
    #define TR_PLAY_TRACK      "\221\222Stopa\0   "
  #define TR_PLAY_BOTH         "\221\222P\200r stop\0"
  #define TR_PLAY_VALUE        "\221\222Hl\200sit\0  "
#else
  #define TR_PLAY_TRACK        "[\221\222Stopa]\0 "
  #define TR_PLAY_BOTH         "[\221\222P\200r]\0   "
  #define TR_PLAY_VALUE        "[\221\222Hl\200sit]\0"
#endif

#define TR_SF_BG_MUSIC        "\221\222Hudba\0   ""\221\222Hudba ||\0"

#if defined(SDCARD)
  #define TR_SDCLOGS           "Loguj na SD"
#else
  #define TR_SDCLOGS           "[Logov\200n\204]\0"
#endif

#ifdef GVARS
  #define TR_ADJUST_GVAR       "Nastav \0   "
#else
  #define TR_ADJUST_GVAR       "[Nastav GP]"
#endif

  #define TR_SF_PLAY_SCRIPT   "[Lua]\0     "

#if defined(DEBUG)
  #define TR_SF_TEST          "Test\0      "
#else
  #define TR_SF_TEST
#endif

#if defined(OVERRIDE_CHANNEL_FUNCTION)
  #define TR_SF_SAFETY        "Z\200mek \0    "
#else
  #define TR_SF_SAFETY        "---\0       "
#endif

  #define TR_SF_SCREENSHOT

#define TR_SF_RESERVE         "[rezerva]\0 "

#if defined(PCBMEGA2560)
  #define TR_VFSWFUNC          TR_SF_SAFETY "Tren\202r \0   ""Insta-Trim\0""Reset\0     " TR_ADJUST_GVAR TR_SOUND TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_VVARIO TR_HAPTIC TR_SDCLOGS "Podsv\203tlen\204" TR_SF_TEST
#else
  #define TR_VFSWFUNC          TR_SF_SAFETY "Tren\202r \0   ""Insta-Trim\0""Reset\0     " TR_ADJUST_GVAR TR_SOUND TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_VVARIO TR_HAPTIC "Podsv\203tlen\204" TR_SF_TEST
#endif

#define LEN_VFSWRESET          TR("\004", "\012")

#if defined(FRSKY)
  #define TR_FSW_RESET_TELEM   TR("Telm", "Telemetrie")
#else
  #define TR_FSW_RESET_TELEM
#endif

#if ROTARY_ENCODERS == 2
  #define TR_FSW_RESET_ROTENC  TR("REa\0""REb\0", "RotEnc A\0 ""RotEnc B\0 ")
#elif ROTARY_ENCODERS == 1
  #define TR_FSW_RESET_ROTENC  TR("RoEn", "RotEnc\0   ")
#else
  #define TR_FSW_RESET_ROTENC
#endif

  #define TR_FSW_RESET_TIMERS  "Tmr1""Tmr2"

#define TR_VFSWRESET           TR(TR_FSW_RESET_TIMERS "V\207e\0" TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC, TR_FSW_RESET_TIMERS "V\207e\0      " TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC)

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

#define LEN_VALARM             "\004"
#define TR_VALARM              "----""\217lut""oran""\201erv"

#define LEN_VALARMFN           "\001"
#define TR_VALARMFN            "<>"

#define LEN_VTELPROTO          "\007"
#define TR_VTELPROTO           "---\0   ""Hub\0   ""WSHHigh"

  #define LEN_AMPSRC           TR("\003", "\005")
  #define TR_AMPSRC            TR("---""A1\0""A2\0""FAS""Cel", "---\0 ""A1\0  ""A2\0  ""FAS\0 ""Cells")

#define LEN_VARIOSRC           "\004"
#if defined(FRSKY_SPORT)
  #define TR_VARIOSRC          "VSpd""A1\0 ""A2\0 ""dTE\0"
#else
  #define TR_VARIOSRC          "Alt\0""Alt+""VSpd""A1\0 ""A2\0"
#endif

  #define LEN_VTELEMSCREENTYPE "\010"
  #define TR_VTELEMSCREENTYPE  "Hodnota ""Ukazatel"

#define LEN_GPSFORMAT          "\004"
#define TR_GPSFORMAT           "DMS\0""NMEA"

#define LEN2_VTEMPLATES        13
#define LEN_VTEMPLATES         "\015"
#define TR_VTEMPLATES          "Smazat mixy\0\0""Z\200kl. 4kan\200l\0""Sticky-T-Cut\0""V-Tail      \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"

#define LEN_VSWASHTYPE         "\004"
#define TR_VSWASHTYPE          "---\0""120\0""120X""140\0""90\0"

#define LEN_VKEYS              "\005"
#define TR_VKEYS               TR("Menu\0""Exit\0""Dol\211\0""Nhoru""Vprvo""Vlevo", "Menu\0""Exit\0""Enter""Page\0""Plus\0""M\204nus")

#define LEN_VRENCODERS         "\003"
#define TR_VRENCODERS          "REa""REb"

#define LEN_VSWITCHES          "\003"
#define LEN_VSRCRAW            TR("\004", "\005")

#define LEN_INPUTNAMES         "\004"
#define TR_INPUTNAMES          "Smer""Vysk""Plyn""Krid"

#define TR_STICKS_VSRCRAW      TR("Sm\203r""V\212\207k""Plyn""K\206id", "\307Sm\203r""\307V\212\207k""\307Plyn""\307K\206id")
#define TR_TRIMS_VSRCRAW       TR("TrmS""TrmV""TrmP""TrmK", "\313Sm\203r""\313V\212\207k""\313Plyn""\313K\206id")

  #define TR_POTS_VSRCRAW      "POT1""POT2""POT3"
  #define TR_SW_VSRCRAW        "3POS"
  #define TR_9X_3POS_SWITCHES  "ID0""ID1""ID2"

  #define TR_LOGICALSW         "L1\0""L2\0""L3\0""L4\0""L5\0""L6\0""L7\0""L8\0""L9\0""L10""L11""L12"

#define TR_TRIMS_SWITCHES      TR("tSl""tSp""tVd""tVn""tPd""tPn""tKl""tKp", "\313Sl""\313Sp""\313Vd""\313Vn""\313Pd""\313Pn""\313Kl""\313Kp")

#if defined(PCBMEGA2560)
  #define TR_ROTARY_ENCODERS   "REa\0""REb\0"
  #define TR_ROTENC_SWITCHES   "REa""REb"
#else
  #define TR_ROTARY_ENCODERS
  #define TR_ROTENC_SWITCHES
#endif

  #define TR_PHYS_SWITCHES     "THR""RUD""ELE""AIL""GEA""TRN"

#define TR_ON_ONE_SWITCHES     "ZAP""One"

  #define TR_VSWITCHES         "---" TR_9X_3POS_SWITCHES TR_PHYS_SWITCHES TR_TRIMS_SWITCHES TR_ROTENC_SWITCHES TR_LOGICALSW TR_ON_ONE_SWITCHES

#if defined(HELI)
  #define TR_CYC_VSRCRAW       TR("CYC1""CYC2""CYC3", "CYC1\0""CYC2\0""CYC3\0")
#else
  #define TR_CYC_VSRCRAW       TR("[C1]""[C2]""[C3]", "[C1]\0""[C2]\0""[C3]\0")
#endif

  #define TR_EXTRA_VSRCRAW

#define TR_VSRCRAW             TR("---\0" TR_STICKS_VSRCRAW TR_POTS_VSRCRAW TR_ROTARY_ENCODERS "MAX\0" TR_CYC_VSRCRAW TR_TRIMS_VSRCRAW TR_SW_VSRCRAW TR_EXTRA_VSRCRAW, "---\0 " TR_STICKS_VSRCRAW TR_POTS_VSRCRAW TR_ROTARY_ENCODERS "MAX\0 " TR_CYC_VSRCRAW TR_TRIMS_VSRCRAW TR_SW_VSRCRAW TR_EXTRA_VSRCRAW)

#define LEN_VTMRMODES          "\004"
#define TR_VTMRMODES           "VYP\0""ZAP\0""Pln>""Pln%""Pln*"

#define LEN_VTRAINERMODES      "\022"
#define TR_VTRAINERMODES       "U\201itel/Jack\0      ""\217\200k/Jack\0         ""U\201itel/SBUS Modul\0""U\201itel/CPPM Modul\0""U\201itel/Bat. slot\0"

#define LEN_VFAILSAFE          "\012"
#define TR_VFAILSAFE           "Nenastaven""Dr\217et\0    ""Vlastn\204\0  ""Bez pulz\211\0""P\206ij\204ma\201\0"

#if defined(MAVLINK)
  #define LEN_MAVLINK_BAUDS    "\006"
  #define TR_MAVLINK_BAUDS     "4800  ""9600  ""14400 ""19200 ""38400 ""57600 ""76800 ""115200"
  #define LEN_MAVLINK_AC_MODES "\011"
  #define TR_MAVLINK_AC_MODES  "Stabilize""Acro     ""Alt Hold ""Auto     ""Guided   ""Loiter   ""RTL      ""Circle   ""Pos Hold ""Land     ""OF Loiter""Toy A    ""Toy M    ""INVALID  "
  #define LEN_MAVLINK_AP_MODES "\015"
  #define TR_MAVLINK_AP_MODES  "Manual       ""Circle       ""Stabilize    ""Training     ""Fly by Wire A""Fly by Wire A""Auto         ""RTL          ""Loiter       ""Guided       ""Initialising ""INVALID      "
#endif

#define LEN_VSENSORTYPES        "\011"
#define TR_VSENSORTYPES        "Vlastn\204\0 ""Vypo\201ten\212"

#define LEN_VFORMULAS          "\012"
#define TR_VFORMULAS           "Sou\201et\0   ""Pr\211m\203r\0   ""Min\0      ""Max\0      ""N\200soben\204  ""Totalize  ""\201l\200nek\0   ""Spot\206eba  ""Vzd\200lenost"

#define LEN_VPREC              "\004"
#define TR_VPREC               "X   ""X.X ""X.XX"

#define LEN_VCELLINDEX         "\007"
#define TR_VCELLINDEX          "N\204zk\212\0 ""1\0     ""2\0     ""3\0     ""4\0     ""5\0     ""6\0     ""Nejv\204ce""Delta\0"

// ZERO TERMINATED STRINGS
#define INDENT                 "\001"
#define LEN_INDENT             1
#define INDENT_WIDTH           (FW/2)
#define BREAKSPACE             " "

  #define TR_ENTER             "[MENU]"

#define TR_EXIT                "[EXIT]"

  #define TR_POPUPS            TR_ENTER "\010" TR_EXIT
  #define OFS_EXIT             sizeof(TR_ENTER)

#define TR_MENUWHENDONE        CENTER "\011" TR_ENTER " > DAL\207\214"
#define TR_FREE                TR("voln\202:", "voln\212ch")
#define TR_DELETEMODEL         "SMAZAT" BREAKSPACE "MODEL"
#define TR_COPYINGMODEL        "Kop\204ruji model.."
#define TR_MOVINGMODEL         "P\206esouv\200m model."
#define TR_LOADINGMODEL        "Aktivuji model.."
#define TR_NAME                "N\200zev"
#define TR_MODELNAME           "N\200zev modelu"
#define TR_PHASENAME           "N\200zev"
#define TR_MIXNAME             "N\200zev"
#define TR_INPUTNAME           "N\200zev"
#define TR_EXPONAME            "N\200zev"
#define TR_BITMAP              "Obr\200zek"
#define TR_TIMER               "Stopky"
#define TR_ELIMITS             TR("Limit +25%", "Kan\200l +/- 125%")
#define TR_ETRIMS              TR("\207ir\207\204 Trim", "\207irok\212 trim")
#define TR_TRIMINC             TR("Krok Trimu", "Krok trimu")
#define TR_DISPLAY_TRIMS       "Zobr.hodnotu trimu"
#define TR_TTRACE              TR("StopaPlynu", INDENT "Stopa plynu")
#define TR_TTRIM               TR("TrimVolnob.", INDENT "Trim jen volnob\203h")
#define TR_BEEPCTR             TR("St\206edy \221\222", "P\204pat st\206edy \221\222")
#define TR_USE_GLOBAL_FUNCS    "Pou\217\204t Glob.Funkce"
#define TR_PROTO               INDENT "Protokol"
  #define TR_PPMFRAME          "PPM frame"
#define TR_MS                  "ms"
#define TR_SWITCH              "Sp\204na\201"
#define TR_TRIMS               "Trimy"
#define TR_FADEIN              "P\206echod Zap"
#define TR_FADEOUT             "P\206echod Vyp"
#define TR_DEFAULT             "(v\212choz\204)"
#define TR_CHECKTRIMS          "\011Kont.\010Trimy"
#define OFS_CHECKTRIMS         (9*FW)
#define TR_SWASHTYPE           "Typ cykliky"
#define TR_COLLECTIVE          "Kolektiv"
#define TR_AILERON             "Bo\201n\204 cyklika"
#define TR_ELEVATOR            "Pod\202ln\200 cykl."
#define TR_SWASHRING           "Cyklika"
#define TR_ELEDIRECTION        "Sm\203r:V\212\207kovka"
#define TR_AILDIRECTION        "\012K\206id\202lka"
#define TR_COLDIRECTION        "\012Kolektiv"
#define TR_MODE                INDENT"M\205d"
#define TR_NOFREEEXPO          "Nen\204 voln\202 expo!"
#define TR_NOFREEMIXER         "Nen\204 voln\212 mix!"
#define TR_INSERTMIX           "VLO\217IT MIX "
#define TR_EDITMIX             "UPRAVIT MIX "
#define TR_SOURCE              INDENT"Zdroj"
#define TR_WEIGHT              "V\200ha"
#define TR_EXPO                "Expo"
#define TR_SIDE                "Strana"
#define TR_DIFFERENTIAL        "Dif.v\212chylek"
#define TR_OFFSET              INDENT"Ofset"
#define TR_TRIM                "Trim"
#define TR_DREX                "DR/Expo"
#define DREX_CHBOX_OFFSET      45
#define TR_CURVE               "K\206ivka"
#define TR_FLMODE              "Re\217im"
#define TR_MIXWARNING          "Varov\200n\204"
#define TR_OFF                 "VYP"
#define TR_MULTPX              TR("Mat.operace", "Operace")
#define TR_DELAYDOWN           TR("Zpo\217d\203n\204 Vyp", "Zdr\217et(x)")
#define TR_DELAYUP             TR("Zpo\217d\203n\204 Zap", "Zdr\217et(\43)")
#define TR_SLOWDOWN            TR("Zpomalen\204(-)", "Zpomal(\177)")
#define TR_SLOWUP              TR("Zpomalen\204(+)", "Zpomal(\176)")
#define TR_MIXER               "MIXER"
#define TR_CV                  "K"
#define TR_GV                  "GP"
#define TR_ACHANNEL            "A\004Kan\200l"
#define TR_RANGE               INDENT"Rozsah"
#define TR_CENTER              INDENT "St\206ed"
#define TR_BAR                 "Ukazatel"
#define TR_ALARM               INDENT"Alarm"
#define TR_USRDATA             TR("U\217ivData", "U\217iv. data")
#define TR_BLADES              TR(INDENT"ListyVrt", INDENT"Listy vrtule")
#define TR_SCREEN              "Panel "
#define TR_SOUND_LABEL         "Zvuk"
#define TR_LENGTH              INDENT"D\202lka"
#define TR_BEEP_LENGTH         INDENT "D\202lka"
#define TR_SPKRPITCH           INDENT"T\205n"
#define TR_HAPTIC_LABEL        "Vibrace"
#define TR_HAPTICSTRENGTH      INDENT"S\204la"
#define TR_CONTRAST            "Kontrast LCD"
#define TR_ALARMS_LABEL        "Alarmy"
#define TR_BATTERY_RANGE       TR("Ukazatel bat.", "Ukazetel baterie")
#define TR_BATTERYWARNING      INDENT"Vybit\200 baterie"
#define TR_INACTIVITYALARM     TR(INDENT "Ne\201innost", INDENT "Ne\201innost r\200dia")
#define TR_MEMORYWARNING       INDENT"Pln\200 pam\203t'"
#define TR_ALARMWARNING        TR(INDENT "Vypnut\212 zvuk", INDENT "Upozornit na vypnut\212 zvuk")
#define TR_RENAVIG             "Navig. RotEnc"
#define TR_THROTTLE_LABEL      "Plyn"
#define TR_THROTTLEREVERSE     TR("ReversPlyn", INDENT"Revers plynu")
#define TR_TIMER_NAME          INDENT "N\200zev"
#define TR_MINUTEBEEP          TR(INDENT"Minuta", INDENT"Oznamovat minuty")
#define TR_BEEPCOUNTDOWN       INDENT"Odpo\201et"
#define TR_PERSISTENT          INDENT"Trval\202"
#define TR_BACKLIGHT_LABEL     "Podsv\203tlen\204"
#define TR_BLDELAY             INDENT"Zhasnout po"
#define TR_BLONBRIGHTNESS      INDENT"Jas Zap."
#define TR_BLOFFBRIGHTNESS     INDENT"Jas Vyp."
#define TR_BLCOLOR             INDENT "Barva"
#define TR_SPLASHSCREEN        TR("\210vodn\204Logo", "Zobrazit \210vodn\204 logo")
#define TR_THROTTLEWARNING     TR("* Plyn", INDENT "Kontrola plynu")
#define TR_SWITCHWARNING       TR("* Sp\204na\201e", INDENT "Polohy sp\204na\201\211")
#define TR_POTWARNING          TR("* Potenc.", INDENT "Nastaven\204 Pot.")
#define TR_TIMEZONE            "\201asov\202 p\200smo"
#define TR_ADJUST_RTC          INDENT "Upravit RTC"
#define TR_GPS                 "GPS"
#define TR_RXCHANNELORD        TR("Po\206ad\204 kan\200l\211", "V\212choz\204 po\206ad\204 kan\200l\211")
#define TR_STICKS              "P\200ky"
#define TR_POTS                "Potenciometry"
#define TR_SWITCHES            "Sp\204na\201e"
#define TR_SWITCHES_DELAY      "P\206echod st\206edu p\206ep\204na\201e"
#define TR_SLAVE               "Pod\206\204zen\212"
#define TR_MODESRC             " M\205d\006% Zdroj"
#define TR_MULTIPLIER          "N\200sobi\201"
#define TR_CAL                 "Kal."
#define TR_VTRIM               "Trim- +"
#define TR_BG                  "BG:"
 #define TR_MENUTOSTART         CENTER "\011" TR_ENTER " = START"
 #define TR_SETMIDPOINT         CENTER "\004Nastav p\200ky na st\206ed"
 #define TR_MOVESTICKSPOTS      TR(CENTER "\006H\212bej p\200kami/poty", "\014H\212bej p\200kami i potenciometry")
#define TR_RXBATT              "Rx Bat.:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              CENTER "NO DATA"
#define TR_TOTTM1TM2THRTHP     "\037\146SES\036TM1\037\146TM2\036THR\037\146TH%"
#define TR_TMR1LATMAXUS        "Tmr1Lat max\037\124us"
#define STR_US                 (STR_TMR1LATMAXUS+13)
#define TR_TMR1LATMINUS        "Tmr1Lat min\037\124us"
#define TR_TMR1JITTERUS        "Tmr1 Jitter\037\124us"

  #define TR_TMIXMAXMS         "Tmix max\037\124ms"
  #define TR_FREESTACKMINB     "Free Stack\037\124b"

#define TR_MENUTORESET         CENTER TR_ENTER" >> Reset"
#define TR_PPM_TRAINER         "TR"
#define TR_CH                  "CH"
#define TR_MODEL               "MODEL"
#define TR_FP                  "LR"
#define TR_MIX                 "MIX"
#define TR_EEPROMLOWMEM        "Doch\200z\204 EEPROM"
#define TR_ALERT               "\006   POZOR"
#define TR_PRESSANYKEYTOSKIP   TR("\003Kl\200vesa >>> p\206esko\201it", "Kl\200vesa >>> p\206esko\201it")
#define TR_THROTTLENOTIDLE     TR("\003P\200ka plynu je pohnut\200", "P\200ka plynu nen\204 na nule")
#define TR_ALARMSDISABLED      "Alarmy jsou zak\200z\200ny"
#define TR_PRESSANYKEY         TR("\006Stiskni kl\200vesu", "Stiskni kl\200vesu")
#define TR_BADEEPROMDATA       TR("\006Chyba dat EEPROM", "Chyba dat EEPROM")
#define TR_EEPROMFORMATTING    TR("\004Formatov\200n\204 EEPROM", "Formatov\200n\204 EEPROM")
#define TR_EEPROMOVERFLOW      "P\206etekla EEPROM"
#define TR_MENURADIOSETUP      "NASTAVEN\214 R\213DIA"
#define TR_MENUDATEANDTIME     "DATUM A \201AS"
#define TR_MENUTRAINER         "TREN\220R"
#define TR_MENUGLOBALFUNCS     "GLOB\213LN\214 FUNKCE"
#define TR_MENUVERSION         "VERZE"
#define TR_MENUDIAG            "DIAG"
#define TR_MENUANA             "ANALOGY"
#define TR_MENUCALIBRATION     "KALIBRACE"
#define TR_TRIMS2OFFSETS       "\006Trimy => Subtrimy"
#define TR_MENUMODELSEL        "MODEL"
#define TR_MENUSETUP           "NASTAVEN\214"
#define TR_MENUFLIGHTPHASE     "LETOV\216 RE\217IM"
#define TR_MENUFLIGHTPHASES    "LETOV\220 RE\217IMY"
#define TR_MENUHELISETUP       "HELI"

#define TR_MENULIMITS          "SERVA"

#define TR_MENUINPUTS        "DR/EXPO"


#define TR_MENUCURVES          "K\215IVKY"
#define TR_MENUCURVE           "\002K"
#define TR_MENULOGICALSWITCH   "LOG. SP\214NA\201"
#define TR_MENULOGICALSWITCHES "LOGICK\220 SP\214NA\201E"
#define TR_MENUCUSTOMFUNC      "SPECI\213LN\214 FUNKCE"
#define TR_MENUCUSTOMSCRIPTS   "SKRIPTY LUA"
#define TR_MENUCUSTOMSCRIPT    "SKRIPT"
#define TR_MENUTELEMETRY       "TELEMETRIE"
#define TR_MENUTEMPLATES       "\207ABLONY"
#define TR_MENUSTAT            "STATISTIKA"
#define TR_MENUDEBUG           "DIAG"
  #define TR_RXNUM             "RX \201\204slo"
#define TR_SYNCMENU            "[Sync]"
#define TR_LIMIT               INDENT"Limit"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Zem. \207\204\206ka"
#define TR_LONGITUDE           "Zem. d\202lka"
#define TR_GPSCOORD            "GPS sou\206adnice"
#define TR_VARIO               "Vario"
#define TR_PITCH_AT_ZERO       INDENT "T\205n na nule"
#define TR_PITCH_AT_MAX        INDENT "T\205n na maximu"
#define TR_REPEAT_AT_ZERO      INDENT "Opakov\200n\204 na nule"
#define TR_POWEROFF            "\027Power OFF..."
#define TR_SHUTDOWN            "Vyp\204n\200n\204.."
#define TR_SAVEMODEL           "Ukl\200d\200m nastaven\204 modelu"
#define TR_BATT_CALIB          "Kalib. baterie"
#define TR_CURRENT_CALIB       " +=\006Proud"
#define TR_VOLTAGE             INDENT"Nap\203t\204"
#define TR_CURRENT             INDENT"Proud"
#define TR_SELECT_MODEL        "Vyber model"
#define TR_CREATE_MODEL        "Nov\212 model"
#define TR_COPY_MODEL          "Kop\204rovat"
#define TR_MOVE_MODEL          "P\206esunout"
#define TR_BACKUP_MODEL        "Z\200lohovat na SD"
#define TR_DELETE_MODEL        "Sma\217 model"
#define TR_RESTORE_MODEL       "Obnov model z SD"
#define TR_SDCARD_ERROR        "Chyba SD karty"
#define TR_NO_SDCARD           "Nen\204 SD karta"
#define TR_SDCARD_FULL         "Pln\200 SD karta"
#define TR_INCOMPATIBLE        "Nekompatibiln\204"
#define TR_WARNING             "KONTROLA"
#define TR_EEPROMWARN          "EEPROM"
#define TR_EEPROM_CONVERTING   "Aktualizuji EEPROM"
#define TR_THROTTLEWARN        "PLYNU"
#define TR_ALARMSWARN          "ALARMU"
#define TR_SWITCHWARN          "POZICE"
#define TR_FAILSAFEWARN        "FAILSAFE"
#define TR_NO_FAILSAFE         "Failsafe nen\204 nastaveno"
#define TR_KEYSTUCK            "Zasekl\200 kl\200vesa"
#define TR_INVERT_THR          "Invertovat plyn?"
#define TR_SPEAKER_VOLUME      INDENT "Hlasitost"
#define TR_LCD                 "LCD"
#define TR_BRIGHTNESS          INDENT "Jas"
#define TR_CPU_TEMP            "Tepl. CPU\016>"
#define TR_CPU_CURRENT         "Proud\030>"
#define TR_CPU_MAH             "Spot\206eba"
#define TR_COPROC              "CoProc."
#define TR_COPROC_TEMP         "Tepl. MB \016>"
#define TR_CAPAWARNING         INDENT "N\204zk\200 kapacita"
#define TR_TEMPWARNING         INDENT "P\206eh\206\200t\204"
#define TR_FUNC                "Fce."
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Trv\200n\204"
#define TR_DELAY               "Zdr\217et"
#define TR_SD_CARD             "SD"
#define TR_SDHC_CARD           "SD-HC"
#define TR_NO_SOUNDS_ON_SD     "\217\200dn\212 zvuk" BREAKSPACE "na SD"
#define TR_NO_MODELS_ON_SD     "\217\200dn\212 model" BREAKSPACE "na SD"
#define TR_NO_BITMAPS_ON_SD    "\217\200dn\202 obr\200zky" BREAKSPACE "na SD"
#define TR_NO_SCRIPTS_ON_SD    "\217\200dn\212 skript" BREAKSPACE "na SD"
#define TR_SCRIPT_SYNTAX_ERROR "Script syntax error"
#define TR_SCRIPT_PANIC        "Script panic"
#define TR_SCRIPT_KILLED       "Script killed"
#define TR_SCRIPT_ERROR        "Unknown error"
#define TR_PLAY_FILE           "P\206ehr\200t"
#define TR_DELETE_FILE         "Odstranit"
#define TR_COPY_FILE           "Kop\204rovat"
#define TR_RENAME_FILE         "P\206ejmenovat"
#define TR_ASSIGN_BITMAP       "Zvolit obr\200zek"
#define TR_EXECUTE_FILE        "Spustit"
#define TR_REMOVED             " odstran\203n"
#define TR_SD_INFO             "Informace"
#define TR_SD_FORMAT           "Form\200t"
#define TR_NA                  "[X]"
#define TR_HARDWARE            "HARDWARE"
#define TR_FORMATTING          "Form\200tovan\204.."
#define TR_TEMP_CALIB          " +=\006Teplota"
#define TR_TIME                "\201as"
#define TR_BAUDRATE            "BT Baudrate"
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Typ:"
#define TR_SD_SPEED            "Rychlost:"
#define TR_SD_SECTORS          "Sektor\211 :"
#define TR_SD_SIZE             "Velikost:"
#define TR_TYPE                INDENT TR_SD_TYPE
#define TR_GLOBAL_VARS         "Glob\200ln\204 prom\203nn\202"
#define TR_GLOBAL_V            "GLOB.PROM."
#define TR_GLOBAL_VAR          "Glob\200ln\204 prom\203nn\200"
#define TR_MENUGLOBALVARS      "GLOB\213LN\214 PROM\203NN\220"
#define TR_OWN                 " \043 "
#define TR_DATE                "Datum"
#define TR_ROTARY_ENCODER      "R.Encs"
#define TR_CHANNELS_MONITOR    "MONITOR KAN\213LU"
#define TR_MIXERS_MONITOR      "MONITOR MIXU"
#define TR_PATH_TOO_LONG       "Cesta je moc dlouh\200"
#define TR_VIEW_TEXT           "Zobrazit text"
#define TR_FLASH_BOOTLOADER    "Flash BootLoaderu"
#define TR_FLASH_EXTERNAL_DEVICE "Flash extern\204ho za\206\204zen\204"
#define TR_FLASH_INTERNAL_MODULE "Flash vnit\206n\204ho modulu"
#define TR_WRITING             "\032Zapisuji.."
#define TR_CONFIRM_FORMAT      "Prov\202st Form\200t?"
#define TR_INTERNALRF          "Vnit\206n\204 RF modul"
#define TR_EXTERNALRF          "Extern\204 RF modul"
#define TR_FAILSAFE            "M\205d Failsafe"
#define TR_FAILSAFESET         "NASTAVEN\214 FAILSAFE"
#define TR_MENUSENSOR          "SENZOR"
#define TR_COUNTRYCODE         "K\205d regionu"
#define TR_VOICELANG           "Jazyk hlasu"
#define TR_UNITSSYSTEM         "Jednotky"
#define TR_EDIT                "Upravit"
#define TR_INSERT_BEFORE       "Vlo\217it p\206ed"
#define TR_INSERT_AFTER        "Vlo\217it za"
#define TR_COPY                "Kop\204rovat"
#define TR_MOVE                "P\206esunout"
#define TR_PASTE               "Vlo\217it"
#define TR_DELETE              "Odstranit"
#define TR_INSERT              "P\206idat"
#define TR_RESET_FLIGHT        "Let"
#define TR_RESET_TIMER1        "Stopky1"
#define TR_RESET_TIMER2        "Stopky2"
#define TR_RESET_TIMER3        "Stopky3"
#define TR_RESET_TELEMETRY     "Telemetrii"
#define TR_STATISTICS          "Statistika"
#define TR_ABOUT_US            "O n\200s"
#define TR_AND_SWITCH          "AND Sp\204na\201"
#define TR_SF                  "SF"
#define TR_GF                  "GF"
#define TR_SPEAKER             INDENT"Repro"
#define TR_BUZZER              INDENT"P\204p\200k"
#define TR_BYTES               "[B]"
#define TR_MODULE_BIND         "[Bind]"
#define TR_MODULE_RANGE        "[Range]"
#define TR_RESET_BTN           "[Reset]"
#define TR_SET                 "[Volby]"
#define TR_TRAINER             "Tren\202r"
#define TR_ANTENNAPROBLEM      CENTER "Probl\202m s TX ant\202nou!"
#define TR_MODELIDUSED         TR("ID ji\217 pou\217ito","ID modelu je ji\217 pou\217ito")
#define TR_MODULE              INDENT "Modul"
#define TR_TELEMETRY_TYPE      TR("Typ", "Typ telemetrie")
#define TR_TELEMETRY_SENSORS   "Senzory"
#define TR_VALUE               "Hodnota"
#define TR_TOPLCDTIMER         "Stopky horn\204ho LCD"
#define TR_UNIT                "Jednotky"
#define TR_TELEMETRY_NEWSENSOR INDENT "P\206idat senzor ru\201n\203"
#define TR_CHANNELRANGE        INDENT "Rozsah kan\200l\211"
#define TR_LOWALARM            INDENT "N\204zk\212 Alarm"
#define TR_CRITICALALARM       INDENT "Kritick\212 Alarm"
#define TR_ENABLE_POPUP        "Povolit vyskakovac\204 okno"
#define TR_DISABLE_POPUP       "Zak\200zat vyskakovac\204 okno"
#define TR_CURVE_PRESET        "\207ablona"
#define TR_PRESET              "\207ablona"
#define TR_MIRROR              "Zrcadlit"
#define TR_CLEAR               "Smazat"
#define TR_RESET               "Reset"
#define TR_RESET_SUBMENU       "Inicializovat ..."
#define TR_COUNT               "Velikost"
#define TR_PT                  "\201."
#define TR_PTS                 " b."
#define TR_SMOOTH              "Hladk\200"
#define TR_COPY_STICKS_TO_OFS  "Kopie p\200k do subtrimu"
#define TR_COPY_TRIMS_TO_OFS   "Kop\204e trim\211 do subtrimu"
#define TR_INCDEC              "Zv\203\207it/Zmen\207it"
#define TR_GLOBALVAR           "Glob. prom\203nn\200"
#define TR_MIXSOURCE           "Zdroje mixeru"
#define TR_CONSTANT            "Konstanta"
#define TR_PERSISTENT_MAH      TR(INDENT "Ulo\217 mAh", INDENT "Ukl\200dat mAh")
#define TR_PREFLIGHT           "P\206edletov\200 kontrola"
#define TR_CHECKLIST           INDENT "Zobrazit pozn\200mky"
#define TR_FAS_OFFSET          TR(INDENT "FAS Ofs", INDENT "FAS Ofset")
#define TR_UART3MODE           "Seriov\212 port"
#define TR_SCRIPT              "Skript"
#define TR_INPUTS              "Vstupy"
#define TR_OUTPUTS             "V\212stupy"
#define TR_EEBACKUP            "\010[dlouze ENTER] z\200lohuje EEPROM"
#define TR_FACTORYRESET        "\012[dlouze MENU]: Factory reset"
#define TR_CONFIRMRESET        "Smazat modely a nastaven\204?"
#define TR_TO_MANY_LUA_SCRIPTS "P\206\204li\207 mnoho skript\211!"

#if defined(MAVLINK)
  #define TR_MAVLINK_RC_RSSI_SCALE_LABEL        "Max RSSI"
  #define TR_MAVLINK_PC_RSSI_EN_LABEL           "PC RSSI EN"
  #define TR_MAVMENUSETUP_TITLE                 "Mavlink Setup"
  #define TR_MAVLINK_BAUD_LABEL                 "Baudrate"
  #define TR_MAVLINK_INFOS                      "INFOS"
  #define TR_MAVLINK_MODE                       "MODE"
  #define TR_MAVLINK_CUR_MODE                   "Current Mode"
  #define TR_MAVLINK_ARMED                      "Armed"
  #define TR_MAVLINK_BAT_MENU_TITLE             "BAT RSSI"
  #define TR_MAVLINK_BATTERY_LABEL              "Flight Battery status"
  #define TR_MAVLINK_RC_RSSI_LABEL              "RC RSSI"
  #define TR_MAVLINK_PC_RSSI_LABEL              "PC RSSI"
  #define TR_MAVLINK_NAV_MENU_TITLE             "NAV"
  #define TR_MAVLINK_COURSE                     "Course"
  #define TR_MAVLINK_HEADING                    "Heading"
  #define TR_MAVLINK_BEARING                    "Bearing"
  #define TR_MAVLINK_ALTITUDE                   "Altitude"
  #define TR_MAVLINK_GPS                        "GPS"
  #define TR_MAVLINK_NO_FIX                     "NO Fix"
  #define TR_MAVLINK_SAT                        "SAT"
  #define TR_MAVLINK_HDOP                       "HDOP"
  #define TR_MAVLINK_LAT                        "LAT"
  #define TR_MAVLINK_LON                        "LON"
#endif

// Taranis column headers
#define TR_PHASES_HEADERS      { " N\200zev ", " Sp\204na\201 ", " Trim sm\203rovky ", " Trim v\212\207kovky ", " Trim plynu ", " Trim k\206id\202lek ", " P\206echod n\200b\203hu ", " P\206echod konce " }
#define TR_LIMITS_HEADERS      { " N\200zev ", " Subtrim ", " Min ", " Max ", " Invertovat ", " K\206ivka ", " St\206ed PPM ", " Symetrick\202 " }
#define TR_CSW_HEADERS         { " Funkce ", " Hodnota 1 ", " Hodnota 2 ", " Hodnota 2 ", " AND Sp\204na\201 ", " Trv\200n\204 ", " Zpo\217d\203n\204 " }

//Taranis About screen
#define TR_ABOUTUS             TR("  O n\200s   ", "O n\200s")

#define TR_ABOUT_OpenAVRc_1      TR("OpenAVRc je nekomer\201n\204,", "OpenAVRc\001je\001open-source,")
#define TR_ABOUT_OpenAVRc_2      TR("opensource bez z\200ruky.", "nekomer\201n\204 a bez\001z\200ruky.")
#define TR_ABOUT_OpenAVRc_3      TR("Vytvo\206en ve voln\202m \201ase.", "Byl\001vytvo\206en\001nad\207en\212mi\001model\200\206i")
#define TR_ABOUT_OpenAVRc_4      TR("Podpora v\212voje pomoc\204", "Podpora\001v\212voje\001pomoc\204")
#define TR_ABOUT_OpenAVRc_5      TR("finan\201n\204ho daru je v\204t\200na!", "drobn\202ho\001daru\001je\001v\204t\200na!")

#define TR_ABOUT_BERTRAND_1    "Bertrand Songis"
#define TR_ABOUT_BERTRAND_2    "Hlavn\204 v\212voj\200\206 OpenAVRc"
#define TR_ABOUT_BERTRAND_3    "Spoluautor Companion"

#define TR_ABOUT_MIKE_1        "Mike Blandford"
#define TR_ABOUT_MIKE_2        "Specialista na k\205d,"
#define TR_ABOUT_MIKE_3        " a ovlada\201e hardwaru."
#define TR_ABOUT_MIKE_4        ""

#define TR_ABOUT_ROMOLO_1      "Romolo Manfredini"
#define TR_ABOUT_ROMOLO_2      "Hlavn\204 v\212voj\200\206"
#define TR_ABOUT_ROMOLO_3      "Companion"

#define TR_ABOUT_ANDRE_1       "Andre Bernet"
#define TR_ABOUT_ANDRE_2       "Funk\201nost, pou\217itelnost,"
#define TR_ABOUT_ANDRE_3       "lad\203n\204 a dokumentace"

#define TR_ABOUT_ROB_1         "Rob Thomson"
#define TR_ABOUT_ROB_2         "openrcforums webmaster"

#define TR_ABOUT_KJELL_1       "Kjell Kernen"
#define TR_ABOUT_KJELL_2       "www.open-tx.org main author"
#define TR_ABOUT_KJELL_3       "autor OpenAVRc Recorder"
#define TR_ABOUT_KJELL_4       "Spoluautor Companion"

#define TR_ABOUT_MARTIN_1      "Martin Hota\206"
#define TR_ABOUT_MARTIN_2      "Grafick\212 design, \201e\207tina"

  #define TR_ABOUT_HARDWARE_1  "Brent Nelson"
  #define TR_ABOUT_HARDWARE_2  "Sky9x designer/v\212robce"
  #define TR_ABOUT_HARDWARE_3  ""

#define TR_ABOUT_PARENTS_1     "Mate\206sk\202 projekty"
#define TR_ABOUT_PARENTS_2     "ersky9x (Mike Blandford)"
#define TR_ABOUT_PARENTS_3     "ER9X (Erez Raviv)"
#define TR_ABOUT_PARENTS_4     "TH9X (Thomas Husterer)"

#define TR_CHR_SHORT  's'
#define TR_CHR_LONG   'l'
#define TR_CHR_TOGGLE 't'
#define TR_CHR_HOUR   'h'
#define TR_CHR_INPUT  'I'   // Values between A-I will work

#define TR_BEEP_VOLUME         "Upozorn\203n\204"
#define TR_WAV_VOLUME          "Zvuky WAV"
#define TR_VARIO_VOLUME        "Vario"
#define TR_BG_VOLUME           "WAV na pozad\204"

#define TR_TOP_BAR             "Horn\204 li\207ta"
#define TR_ALTITUDE            INDENT "V\212\207ka"
#define TR_SCALE               "M\203\206\204tko"
#define TR_VIEW_CHANNELS       "Zobrazit kan\200ly"
#define TR_VIEW_NOTES          "Zobrazit pozn\200mky"
#define TR_MODS_FORBIDDEN      "Zm\203ny nejsou povoleny!"
#define TR_UNLOCKED            "Odem\201eno"
#define TR_ID                  "ID"
#define TR_PRECISION           "P\206esnost"
#define TR_RATIO               "Koeficient"
#define TR_FORMULA             "Operace"
#define TR_CELLINDEX           "\201l\200nek"
#define TR_LOGS                "Logovat"
#define TR_OPTIONS             "Options"

#define TR_ALTSENSOR           "Senzor v\212\207ky"
#define TR_CELLSENSOR          "Senzor \201l\200nk\211"
#define TR_GPSSENSOR           "GPS senzor"
#define TR_CURRENTSENSOR       "Senzor"
#define TR_AUTOOFFSET          "Auto ofset"
#define TR_ONLYPOSITIVE        "Jen kladn\202"
#define TR_FILTER              "Filtr"
#define TR_TELEMETRYFULL       "V\207echny sloty jsou pln\202!"
//TODO: translation
#define TR_INVERTED_SERIAL     INDENT "Invert"
#define TR_IGNORE_INSTANCE     INDENT "Ignoruj chyby ID"
#define TR_DISCOVER_SENSORS    INDENT "Detekovat nov\202 senzory"
#define TR_STOP_DISCOVER_SENSORS INDENT "Zastavit autodetekci"
#define TR_DELETE_ALL_SENSORS  INDENT "Odstranit v\207echny senzory"
#define TR_CONFIRMDELETE       "Opravdu odstranit v\207e?"

#define TR_MENU_INPUTS         "\314Vstupy"
#define TR_MENU_LUA            "\322Lua skripty"
#define TR_MENU_STICKS         "\307P\200ky"
#define TR_MENU_POTS           "\310Potenciometry"
#define TR_MENU_MAX            "\315MAX"
#define TR_MENU_HELI           "\316Cyklika"
#define TR_MENU_TRIMS          "\313Trimy"
#define TR_MENU_SWITCHES       "\312Sp\204na\201e"
#define TR_MENU_LOGICAL_SWITCHES "\312Logick\202 sp\204na\201e"
#define TR_MENU_TRAINER        "\317Tren\202r"
#define TR_MENU_CHANNELS       "\320Kan\200ly"
#define TR_MENU_GVARS          "\311Glob.prom\203nn\202"
#define TR_MENU_TELEMETRY      "\321Telemetrie"
#define TR_MENU_OTHER          "Ostatn\204"
#define TR_MENU_INVERT         "Invertovat"

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
#define ZSTR_GPSDATETIME       "\004\377\354\373"
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
