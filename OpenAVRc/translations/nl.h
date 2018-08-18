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
#define TR_OFFON               "UIT""AAN"

#define LEN_MMMINV             "\003"
#define TR_MMMINV              "---""INV"

#define LEN_NCHANNELS          "\004"
#define TR_NCHANNELS           "\0014CH\0016CH\0018CH10CH12CH14CH16CH"

#define LEN_VBEEPMODE          "\005"
#define TR_VBEEPMODE           "Stil\0""Alarm""NoKey""Alles"

#define LEN_VBEEPLEN           "\005"
#define TR_VBEEPLEN            "0====""=0===""==0==""===0=""====0"

#define LEN_VRENAVIG           "\003"
#define TR_VRENAVIG            "UITREaREb"

#define LEN_VBLMODE            TR("\005", "\010")
#define TR_VBLMODE             TR("UIT\0 ""Keys\0""Stick""Beide""AAN\0 ", "UIT\0    ""Toetsen\0""Sticks\0 ""Beide\0  ""AAN\0    ")

#define LEN_TRNMODE            "\003"
#define TR_TRNMODE             "UIT"" +="" :="

#define LEN_TRNCHN             "\003"
#define TR_TRNCHN              "CH1CH2CH3CH4"

#define LEN_UART3MODES         "\015"
#define TR_UART3MODES          "UIT\0         ""S-Port Mirror""Telemetrie\0  ""SBUS Leerling""Debug\0       "

#define LEN_SWTYPES            "\006"
#define TR_SWTYPES             "Geen\0 ""Wissel""2POS\0 ""3POS\0"

#define LEN_POTTYPES           "\021"
#define TR_POTTYPES            "Geen\0            ""Pot met Klik\0    ""Standenschakelaar""Pot zonder Klik\0 "


#define LEN_SLIDERTYPES        "\006"
#define TR_SLIDERTYPES         "Geen\0 ""Schuif"

#define LEN_DATETIME           "\006"
#define TR_DATETIME            "DATUM:""TIJD: "

#define LEN_VLCD               "\006"
#define TR_VLCD                "NormalOptrex"

#define LEN_VPERSISTENT        "\020"
#define TR_VPERSISTENT         "UIT\0            ""Vliegtijd\0      ""Handmatige Reset"

#define LEN_COUNTRYCODES       TR("\002", "\007")
#define TR_COUNTRYCODES        TR("US""JP""EU", "Amerika""Japan\0 ""Europa\0")

#define LEN_TARANIS_PROTOCOLS  "\004"
#define TR_TARANIS_PROTOCOLS   "UIT\0""PPM\0""XJT\0""DSM2""CRSF"

#define LEN_XJT_PROTOCOLS      "\004"
#define TR_XJT_PROTOCOLS       "OFF\0""D16\0""D8\0 ""LR12"

#define LEN_DSM_PROTOCOLS      "\004"
#define TR_DSM_PROTOCOLS       "LP45""DSM2""DSMX"

#define LEN_VTRIMINC           TR("\006", "\014")
#define TR_VTRIMINC            TR("Expo\0 ""ExFijn""Fijn\0 ""Medium""Grof\0 ", "Exponentieel""Extra Fijn\0 ""Fijn\0       ""Medium\0     ""Grof\0       ")

#define LEN_VDISPLAYTRIMS      "\006"
#define TR_VDISPLAYTRIMS       "Nee\0  ""Kort\0 ""Ja\0   "

#define LEN_VBEEPCOUNTDOWN     "\006"
#define TR_VBEEPCOUNTDOWN      "StilteBeeps\0SpraakTril\0 "

#define LEN_VVARIOCENTER       "\006"
#define TR_VVARIOCENTER        "Tonen\0""Stilte"

#define LEN_CURVE_TYPES        "\011"
#define TR_CURVE_TYPES         "Standaard""Custom\0  "

#define LEN_RETA123            "\001"

  #define TR_RETA123           "RETA123ab"

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

#if defined(SPIMODULES)
   #define TR_SPIM               "SPIRfMod"
   #define TR_RFTUNECOARSE       INDENT "Freq.coarse"
   #define TR_RFPOWER INDENT     "RF Power"
#endif

#if defined(MULTIMODULE)
  #define LEN_MULTIPROTOCOLS    "\006"
  #define TR_MULTIPROTOCOLS     "FlySky""Hubsan""FrSky\0""Hisky\0""V2x2\0 ""DSM\0  ""Devo\0 ""YD717\0""KN\0   ""SymaX\0""SLT\0  ""CX10\0 ""CG023\0""Bayang""ESky\0 ""MT99XX""MJXQ\0 ""Shenqi""FY326\0""SFHSS\0""J6 PRO""FQ777\0""Assan\0""Hontai""OLRS\0 ""FS 2A\0""Q2x2\0 ""Walk.\0""Q303\0 ""GW008\0""DM002\0"
  #define TR_MULTI_CUSTOM       "Custom"
#endif

#if defined(MULTIMODULE) || defined(SPIMODULES)
  #define TR_RFTUNEFINE         INDENT "Freq.fine"
  #define TR_RFTUNECOARSE       INDENT "Freq.coarse"
  #define TR_TELEMETRY             "Telemetry"
  #define TR_MULTI_VIDFREQ               TR(INDENT "Vid. freq.", INDENT "Video frequency")
  #define TR_MULTI_RFPOWER               INDENT "RF Power"
  #define TR_MULTI_OPTION                TR(INDENT "Option", INDENT "Option value")
  #define TR_AUTOBIND              TR(INDENT "Autobind",INDENT "Bind on powerup")
  #define TR_MULTI_DSM_AUTODTECT         TR(INDENT "Autodetect", INDENT "Autodetect format")
  #define TR_MULTI_LOWPOWER              TR(INDENT "Low power", INDENT "Low power mode")
  #define TR_MULTI_SERVOFREQ             TR(INDENT "Servo rate", INDENT "Servo update rate")

#endif





#define LEN_POSNEG             "\003"
#define TR_POSNEG              "POS""NEG"


#define LEN_VCURVEFUNC         "\003"
#define TR_VCURVEFUNC          "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_VMLTPX             "\010"
#define TR_VMLTPX              "Add\0    ""Multiply""Replace\0"

#define LEN_VMLTPX2            "\002"
#define TR_VMLTPX2             "+=""*="":="

#define LEN_VMIXTRIMS          "\003"
#define TR_VMIXTRIMS           "OFF""ON\0""Rud""Ele""Thr""Ail"

  #define TR_CSWTIMER          "Tim\0 "
  #define TR_CSWSTICKY         "Glue\0"
    #define TR_CSWRANGE
    #define TR_CSWSTAY



#define LEN_VCSWFUNC           "\005"
#define TR_VCSWFUNC            "---\0 " "a=x\0 " "a\173x\0 ""a>x\0 ""a<x\0 " TR_CSWRANGE "|a|>x""|a|<x""AND\0 ""OR\0  ""XOR\0 " TR_CSWSTAY "a=b\0 ""a>b\0 ""a<b\0 ""^}x\0 ""|^|}x" TR_CSWTIMER TR_CSWSTICKY

#define LEN_VFSWFUNC           "\012"

#if defined(VARIO)
  #define TR_VVARIO            "Vario\0    "
#else
  #define TR_VVARIO            "[Vario]\0  "
#endif

#if defined(AUDIO)
  #define TR_SOUND             "Geluid    "
#else
  #define TR_SOUND             "Beep\0    "
#endif

#if defined(HAPTIC)
  #define TR_HAPTIC            "Haptic\0   "
#else
  #define TR_HAPTIC            "[Haptic]\0 "
#endif

#if defined(VOICE)
    #define TR_PLAY_TRACK      "Play Track"
  #define TR_PLAY_BOTH         "Play Both\0"
  #define TR_PLAY_VALUE        TR("Play Val\0 ", "Play Value")
#else
  #define TR_PLAY_TRACK        "[Play Trk]"
  #define TR_PLAY_BOTH         "[Play Bth]"
  #define TR_PLAY_VALUE        "[Play Val]"
#endif

#define TR_SF_BG_MUSIC         "BgMusic\0  ""BgMusic ||"

#if defined(SDCARD)
  #define TR_SDCLOGS           "SD Logs\0  "
#else
  #define TR_SDCLOGS           "[SD Logs]\0"
#endif

#if defined(GVARS)
  #define TR_ADJUST_GVAR       "Wijzig \0  "
#else
  #define TR_ADJUST_GVAR       "[WijzigGV]"
#endif

  #define TR_SF_PLAY_SCRIPT   "[Lua]\0    "

#if defined(DEBUG)
  #define TR_SF_TEST          "Test\0     "
#else
  #define TR_SF_TEST
#endif

#if   defined(OVERRIDE_CHANNEL_FUNCTION)
  #define TR_SF_SAFETY        "Overr.\0   "
#else
  #define TR_SF_SAFETY        "---\0      "
#endif

  #define TR_SF_SCREENSHOT

#define TR_SF_RESERVE         "[reserve]\0"

  #define TR_VFSWFUNC          TR_SF_SAFETY "Trainer\0  ""Inst. Trim""Reset\0    " TR_ADJUST_GVAR TR_SOUND TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_VVARIO TR_HAPTIC TR_SDCLOGS "Backlight\0" TR_SF_TEST

#define LEN_VFSWRESET          TR("\004", "\012")

#if defined(FRSKY)
  #define TR_FSW_RESET_TELEM   TR("Telm", "Telemetrie")
#else
  #define TR_FSW_RESET_TELEM
#endif

#if ROTARY_ENCODERS == 2
  #define TR_FSW_RESET_ROTENC  TR("REa\0""REb\0", "RotEnc A\0 ""RotEnc B\0 ")
#elif ROTARY_ENCODERS == 1
  #define TR_FSW_RESET_ROTENC  TR("R.E.", "RotEnc\0   ")
#else
  #define TR_FSW_RESET_ROTENC
#endif

  #define TR_FSW_RESET_TIMERS  "Tmr1""Tmr2"

#define TR_VFSWRESET           TR(TR_FSW_RESET_TIMERS "All\0" TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC, TR_FSW_RESET_TIMERS "Vliegdata\0" TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC)

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

  #define TR_VTELEMCHNS        "---\0""Batt" TR_TELEM_TIME TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_TIMERS TR_SWR "Tx\0 " TR_TELEM_RSSI_RX TR_RX_BATT "A1\0 ""A2\0 " TR_A3_A4 "Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""Hdg\0""VSpd""ASpd""dTE\0" TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE "A1-\0""A2-\0" TR_A3_A4_MIN "Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""Dst+" TR_ASPD_MAX "Cel-""Cls-""Vfs-""Cur+""Pwr+" TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE "Acc\0""Tijd"

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
#define TR_VALARM              "---""Gel""Ora""Rod"

#define LEN_VALARMFN           "\001"
#define TR_VALARMFN            "<>"

#define LEN_VTELPROTO          "\007"
#define TR_VTELPROTO           "Geen\0  ""Hub\0   ""WSHHigh""S.Port\0"

  #define LEN_AMPSRC           TR("\003", "\005")
  #define TR_AMPSRC            TR("---""A1\0""A2\0""FAS""Cel", "---\0 ""A1\0  ""A2\0  ""FAS\0 ""Cells")

#define LEN_VARIOSRC           "\004"
#if defined(FRSKY_SPORT)
  #define TR_VARIOSRC          "VSpd""A1\0 ""A2\0 ""dTE\0"
#else
  #define TR_VARIOSRC          "Alt\0""Alt+""VSpd""A1\0 ""A2\0"
#endif

  #define LEN_VTELEMSCREENTYPE "\006"
  #define TR_VTELEMSCREENTYPE  "Nums\0 ""Balken"

#define LEN_GPSFORMAT          "\004"
#define TR_GPSFORMAT           "DMS\0""NMEA"

#define LEN2_VTEMPLATES        13
#define LEN_VTEMPLATES         "\015"
#define TR_VTEMPLATES          "Mix wissen\0\0\0""Simple 4-CH \0""Sticky-T-Cut\0""V-Tail      \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"

#define LEN_VSWASHTYPE         "\004"
#define TR_VSWASHTYPE          "---\0""120\0""120X""140\0""90\0"

#define LEN_VKEYS              "\005"
#define TR_VKEYS               TR("Menu\0""Exit\0""Down\0""Up\0  ""Right""Left\0", "Menu\0""Exit\0""Enter""Page\0""Plus\0""Minus")

#define LEN_VRENCODERS         "\003"
#define TR_VRENCODERS          "REa""REb"

#define LEN_VSWITCHES          "\003"
#define LEN_VSRCRAW            "\004"

#define TR_STICKS_VSRCRAW      TR("Rud\0""Ele\0""Thr\0""Ail\0", "\307Rud""\307Ele""\307Thr""\307Ail")
#define TR_TRIMS_VSRCRAW       TR("TrmR""TrmE""TrmT""TrmA", "\313Rud""\313Ele""\313Thr""\313Ail")

  #define TR_POTS_VSRCRAW      "P1\0 ""P2\0 ""P3\0 "
  #define TR_SW_VSRCRAW        "3POS"
  #define TR_9X_3POS_SWITCHES  "ID0""ID1""ID2"

  #define TR_LOGICALSW         "L1\0""L2\0""L3\0""L4\0""L5\0""L6\0""L7\0""L8\0""L9\0""L10""L11""L12""L13""L14""L15""L16""L17""L18""L19""L20"

#define TR_TRIMS_SWITCHES      TR("tRl""tRr""tEd""tEu""tTd""tTu""tAl""tAr", "\313Rl""\313Rr""\313Ed""\313Eu""\313Td""\313Tu""\313Al""\313Ar")

  #define TR_ROTARY_ENCODERS   "REa\0""REb\0"
  #define TR_ROTENC_SWITCHES   "REA""REB""REN"

  #define TR_PHYS_SWITCHES     "THR""RUD""ELE""AIL""GEA""TRN"

#define TR_ON_ONE_SWITCHES     "ON\0""One"

  #define TR_EXTRA_3POS_SWITCHES  "XD0""XD1""XD2"

  #define TR_VSWITCHES         "---" TR_9X_3POS_SWITCHES TR_PHYS_SWITCHES TR_TRIMS_SWITCHES TR_ROTENC_SWITCHES TR_EXTRA_3POS_SWITCHES TR_LOGICALSW TR_ON_ONE_SWITCHES

#if defined(HELI)
  #define TR_CYC_VSRCRAW       "CYC1""CYC2""CYC3"
#else
  #define TR_CYC_VSRCRAW       "[C1]""[C2]""[C3]"
#endif

  #define TR_EXTRA_VSRCRAW

#define TR_VSRCRAW             "---\0" TR_STICKS_VSRCRAW TR_POTS_VSRCRAW TR_ROTARY_ENCODERS "MAX\0" TR_CYC_VSRCRAW TR_TRIMS_VSRCRAW TR_SW_VSRCRAW TR_EXTRA_VSRCRAW

#define LEN_VTMRMODES          "\003"
#define TR_VTMRMODES           "UIT""AAN""THs""TH%""THt"

#define LEN_VTRAINERMODES      "\022"
#define TR_VTRAINERMODES       "Master/Jack\0      ""Slave/Jack\0       ""Master/SBUS Module""Master/CPPM Module""Master/Battery\0"

#define LEN_VFAILSAFE          TR("\013","\011")
#define TR_VFAILSAFE           TR("Niet Gezet\0""Vasthouden\0""Custom\0    ""Geen Pulsen""Ontvanger\0 ","Not Set\0 ""Hold\0    ""Custom\0  ""No Pulses""Receiver\0")

#if defined(MAVLINK)
  #define LEN_MAVLINK_BAUDS    "\006"
  #define TR_MAVLINK_BAUDS     "4800  ""9600  ""14400 ""19200 ""38400 ""57600 ""76800 ""115200"
  #define LEN_MAVLINK_AC_MODES "\011"
  #define TR_MAVLINK_AC_MODES  "Stabilize""Acro     ""Alt Hold ""Auto     ""Guided   ""Loiter   ""RTL      ""Circle   ""Pos Hold ""Land     ""OF Loiter""Toy A    ""Toy M    ""INVALID  "
  #define LEN_MAVLINK_AP_MODES "\015"
  #define TR_MAVLINK_AP_MODES  "Manueel      ""Circle       ""Stabiliseer  ""Training     ""Fly by Wire A""Fly by Wire A""Auto         ""RTL          ""Loiter       ""Begeleid     ""Initialiseren""ONGELDIG     "
#endif

#define LEN_VSENSORTYPES        "\010"
#define TR_VSENSORTYPES        "Custom\0 ""Berekend"

#define LEN_VFORMULAS          "\014"
#define TR_VFORMULAS           "Optellen\0   ""Gemiddeld\0  ""Min\0        ""Max\0        ""Vermenigvuld""Totaal\0     ""Cellen\0     ""Verbruik\0   ""Afstand\0    "



#define LEN_VPREC              "\004"  //  "\005"  Prec0 Prec1 Prec2
#define TR_VPREC               "0.--""0.0 ""0.00"

#define LEN_VCELLINDEX         "\007"
#define TR_VCELLINDEX          "Laagste""1e Cel\0""2e Cel\0""3e Cel\0""4e Cel\0""5e Cel\0""6e Cel\0""Hoogste""Delta\0 "

// ZERO TERMINATED STRINGS
  #define INDENT               "\001"
  #define LEN_INDENT           1
  #define INDENT_WIDTH         (FW/2)
  #define BREAKSPACE           " "

  #define TR_ENTER             "[MENU]"

#define TR_EXIT                "[EXIT]"

  #define TR_POPUPS            TR_ENTER "\010" TR_EXIT
  #define OFS_EXIT             sizeof(TR_ENTER)

#define TR_MENUWHENDONE        CENTER "\006" TR_ENTER " BIJ GEREED"
#define TR_FREE                "vrij"
#define TR_DELETEMODEL         "WIS" BREAKSPACE "MODEL"
#define TR_COPYINGMODEL        "Kopieer Model"
#define TR_MOVINGMODEL         "Verplaats Model"
#define TR_LOADINGMODEL        "Laad Model..."
#define TR_NAME                "Naam"
#define TR_MODELNAME           "Modelnaam"
#define TR_PHASENAME           "Modus"
#define TR_MIXNAME             "Mix Naam"
#define TR_INPUTNAME           "Input Naam"
#define TR_EXPONAME          "Expo Naam"
#define TR_BITMAP              "Modelafbeelding"
#define TR_TIMER               TR("Timer", "Timer ")
#define TR_ELIMITS             TR("E.Limits", "Extended Limits")
#define TR_ETRIMS              TR("E.Trims", "Extended Trims")
#define TR_TRIMINC             "Trim Step"
#define TR_DISPLAY_TRIMS       TR("Show Trims", "Display Trims")
#define TR_TTRACE              TR("T-Source", INDENT "Source")
#define TR_TSWITCH             "T-Switch"
#define TR_TTRIM               TR("T-Trim", INDENT "Trim Idle Only")
#define TR_BEEPCTR             TR("Ctr Beep", "Center Beep")
#define TR_USE_GLOBAL_FUNCS    TR("Glob.Funcs", "Use Global Funcs")
#define TR_PROTO               TR(INDENT "Proto", INDENT "Protocol")
  #define TR_PPMFRAME          "PPM frame"
#define TR_MS                  "ms"
#define TR_SWITCH              TR("Schak.", "Schakelaar")
#define TR_TRIMS               "Trims"
#define TR_FADEIN              "Fade In"
#define TR_FADEOUT             "Fade Out"
#define TR_DEFAULT             "(default)"
#define TR_CHECKTRIMS          CENTER "\006Check\012Trims"
#define OFS_CHECKTRIMS         CENTER_OFS+(9*FW)
#define TR_SWASHTYPE           "Swash Type"
#define TR_COLLECTIVE          TR("Collective", "Coll. pitch source")
#define TR_AILERON             "Lateral cyc. source"
#define TR_ELEVATOR            "Long. cyc. source"
#define TR_SWASHRING           "Swash Ring"
#define TR_ELEDIRECTION        "ELE Direction"
#define TR_AILDIRECTION        "AIL Direction"
#define TR_COLDIRECTION        "PIT Direction"
#define TR_MODE                INDENT "Mode"
#define TR_NOFREEEXPO          "Geen vrije expo!"
#define TR_NOFREEMIXER         "Geen vrije mixer!"
#define TR_INSERTMIX           "INSERT MIX "
#define TR_EDITMIX             "EDIT MIX "
#define TR_SOURCE              INDENT "Source"
#define TR_WEIGHT              "Weight"
#define TR_EXPO                TR("Expo", "Exponentieel")
#define TR_SIDE                "Side"
#define TR_DIFFERENTIAL        "Differ"
#define TR_OFFSET              INDENT "Offset"
#define TR_TRIM                "Trim"
#define TR_DREX                "DRex"
#define DREX_CHBOX_OFFSET      30
#define TR_CURVE               "Curve"
#define TR_FLMODE              TR("Mode", "Modes")
#define TR_MIXWARNING          "Waarsc."
#define TR_OFF                 "UIT"
#define TR_MULTPX              "Multpx"
#define TR_DELAYDOWN           "Vertr.Dn"
#define TR_DELAYUP             "Vertr.Up"
#define TR_SLOWDOWN            "Langz.Dn"
#define TR_SLOWUP              "Langz.Up"
#define TR_MIXER               "MIXER"
#define TR_CV                  "CV"
#define TR_GV                  "GV"
#define TR_ACHANNEL            "A\004kanaal"
#define TR_RANGE               INDENT "Bereik"
#define TR_CENTER              INDENT "Centreer"
#define TR_BAR                 "Balk"
#define TR_ALARM               INDENT "Alarm"
#define TR_USRDATA             "Data berekenen uit"
#define TR_BLADES              "Bladen"

#define TR_SCREEN              "Scherm\001"
#define TR_SOUND_LABEL         "----Geluid-------"
#define TR_LENGTH              INDENT "Duur"
#define TR_BEEP_LENGTH         INDENT "Piep-Lengte"
#define TR_SPKRPITCH           INDENT "Piep-Freq. +/-"
#define TR_HAPTIC_LABEL        "----Haptic-------" //9XR-Pro
#define TR_HAPTICSTRENGTH      INDENT "Sterkte"
#define TR_CONTRAST            "LCD-Kontrast"
#define TR_ALARMS_LABEL        "----Alarm---"
#define TR_BATTERY_RANGE       TR("Accu Bereik", "Accu Spngs-Bereik") // Symbol Akku Ladezustand

#define TR_BATTERYWARNING      INDENT "Accu laag"
#define TR_INACTIVITYALARM     TR(INDENT "Inactiviteit", INDENT "Inactiviteit na") //9XR-Pro
#define TR_MEMORYWARNING       INDENT "Geheugen laag"
#define TR_ALARMWARNING        TR(INDENT "Al Geluid uit?", INDENT "Al het geluid uit?")
#define TR_RENAVIG             "Stappenschakelaar"
#define TR_THROTTLE_LABEL      "----Gas-------------"
#define TR_THROTTLEREVERSE     TR("Reverse", INDENT "Omgekeerd")
#define TR_TIMER_NAME          INDENT "Naam"
#define TR_MINUTEBEEP          TR(INDENT "Min-Alarm", INDENT "Minuten-Alarm")
#define TR_BEEPCOUNTDOWN       INDENT "Countdown"
#define TR_PERSISTENT          TR(INDENT "Vasth.", INDENT "Vasthouden")
#define TR_BACKLIGHT_LABEL     "----LCD-Verlichting--"
#define TR_BLDELAY             INDENT "Duur"
#define TR_BLONBRIGHTNESS      INDENT "Aan-Helderheid"
#define TR_BLOFFBRIGHTNESS     INDENT "Uit-Helderheid"
#define TR_BLCOLOR             INDENT "Kleur"
#define TR_SPLASHSCREEN        TR("Startscherm Aan", "Startscherm Aan voor")

#define TR_THROTTLEWARNING     TR("T-Warning", INDENT "Throttle Status")
#define TR_SWITCHWARNING       TR("S-Warning", INDENT "Switch Posities")
#define TR_POTWARNING          TR("Pot Warn.", INDENT "Pot Posities")
#define TR_TIMEZONE            TR("Tijdzone", "GPS-Tijdzone +/-Std")
#define TR_ADJUST_RTC          "Klok middels GPS instellen"
#define TR_GPS                 "----GPS--------------"

#define TR_RXCHANNELORD        TR("Kan.Volgorde", "Kanaalvolgorde")
#define TR_STICKS              "Sticks"
#define TR_POTS                "Pots"
#define TR_SWITCHES_DELAY      "Vertraging"
#define TR_SLAVE               CENTER "Leerling"
#define TR_MODESRC             "Mode\006% Source"
#define TR_MULTIPLIER          "Multiplier"
#define TR_CAL                 "Cal"
#define TR_VTRIM               "Trim - +"
#define TR_BG                  "BG:"
  #define TR_MENUTOSTART       CENTER "\010" TR_ENTER " VOOR START"
  #define TR_SETMIDPOINT       TR(CENTER "\004ZET STICKS NAAR HET MIDDEN", CENTER "\004CENTREER STICKS/SLIDERS")
  #define TR_MOVESTICKSPOTS    CENTER "\006BEWEEG STICKS/POTS"
#define TR_RXBATT              "Rx Accu:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              CENTER"Geen Data"
#define TR_TOTTM1TM2THRTHP     "\037\146SES\036TM1\037\146TM2\036THR\037\146TH%"
#define TR_US                  "us"
#define TR_MENUTORESET         CENTER TR_ENTER" voor Reset"
#define TR_PPM_TRAINER         "TR"
#define TR_CH                  "CH"
#define TR_MODEL               "MODEL"
#define TR_FP                  "FM"
#define TR_MIX                 "MIX"
#define TR_EEPROMLOWMEM        "EEPROM vol"
#define TR_ALERT               "ALARM"
#define TR_PRESSANYKEYTOSKIP   "Druk een toets.."
#define TR_THROTTLENOTIDLE     "Gas niet Dicht!"
#define TR_ALARMSDISABLED      "Alarm uitgeschakeld"
#define TR_PRESSANYKEY         TR("\010Druk een Toets", "Druk een Toets")
#define TR_BADEEPROMDATA       "EEPROM Ongeldig"
#define TR_EEPROMFORMATTING    "EEPROM Initialiseren"
#define TR_EEPROMOVERFLOW      "EEPROM Overflow"
#define TR_MENURADIOSETUP      TR("ZENDER-INSTELLEN", "ZENDER-BASISINSTELLENGEN")


#define TR_MENUDATEANDTIME     "DATUM EN TIJD"
#define TR_MENUTRAINER         "LERAAR/LEERLING"
#define TR_MENUGLOBALFUNCS     "GLOBALE FUNKTIES"
#define TR_MENUVERSION         "VERSIE"
#define TR_MENUDIAG            TR("Schak.", "Schakelaar-Test")
#define TR_MENUANA             "Analoog-Test"
#define TR_MENUCALIBRATION     TR("CALIB. ANALOOG", "CALIBRERING Sticks+Pots")

  #define TR_TRIMS2OFFSETS     "\006Trims => Subtrims"
#define TR_MENUMODELSEL        TR("MODELLEN", "MODEL KIEZEN")
#define TR_MENUSETUP           TR("MODEL-INSTELLING", "MODEL-INSTELLINGEN")
#define TR_MENUFLIGHTPHASES    "VLIEGFASES"
#define TR_MENUFLIGHTPHASE     "VLIEGFASE"
#define TR_MENUHELISETUP       "HELI TS-Mixer"

#define TR_MENUINPUTS        "STICKS"
#define TR_MENULIMITS        "SERVOS"

#define TR_MENUCURVES          "CURVEN"
#define TR_MENUCURVE           "CURVE"
#define TR_MENULOGICALSWITCH   "LOGISCHE SCHAKELAAR"
#define TR_MENULOGICALSWITCHES "LOGISCHE SCHAKELAARS"
#define TR_MENUCUSTOMFUNC      TR("SPEC.-FUNKTIES", "SPECIALE-FUNKTIES")
#define TR_MENUCUSTOMSCRIPTS   "LUA-SCRIPTS"
#define TR_MENUCUSTOMSCRIPT    "LUA-SCRIPT"
#define TR_MENUTELEMETRY       "TELEMETRIE"
#define TR_MENUTEMPLATES       "SJABLONEN"
#define TR_MENUSTAT            "STAT"
#define TR_MENUDEBUG           "DEBUG"
  #define TR_RXNUM             "RxNum"
#define TR_SYNCMENU            "Sync [MENU]"
#define TR_LIMIT               INDENT "Grenzen"
#define TR_MINRSSI             "Min. RSSI"
#define TR_LATITUDE            "Latitude"
#define TR_LONGITUDE           "Longitude"
#define TR_GPSCOORD            TR("GPS-coord.", "GPS-coordinaten format")
#define TR_VARIO               TR("----Vario--------", "----Variometer----") //9XR-Pro
#define TR_PITCH_AT_ZERO       INDENT "Laagste Toon"
#define TR_PITCH_AT_MAX        INDENT "Hoogste Toon"
#define TR_REPEAT_AT_ZERO      INDENT "Herhalen bij 0"
#define TR_POWEROFF            "\027Uitschakelen..."
#define TR_SHUTDOWN            "Afsluiten"
#define TR_SAVEMODEL           "Bewaar Model-instellingen"
#define TR_BATT_CALIB          "Accu Calib"
#define TR_CURRENT_CALIB       "Stroom Calib"
#define TR_VOLTAGE             TR(INDENT "Spg", INDENT "Spanningsbron")  //9XR-Pro
#define TR_CURRENT             TR(INDENT "Stroom", INDENT "Stroombron")
#define TR_SELECT_MODEL        "Kies Model"
#define TR_CREATE_MODEL        "Nieuw Model"
#define TR_COPY_MODEL          "Kopieer Model"
#define TR_MOVE_MODEL          "Verplaats Model"
#define TR_BACKUP_MODEL        "Backup Model"
#define TR_DELETE_MODEL        "Wis Model"
#define TR_RESTORE_MODEL       "Model Terugzetten"
#define TR_SDCARD_ERROR        "SD-Kaart fout"
#define TR_NO_SDCARD           "Geen SD-Kaart"
#define TR_SDCARD_FULL         "SD-Kaart vol"
#define TR_INCOMPATIBLE        "Niet compatibel"
#define TR_WARNING             TR("WARNING","WAARSCHUWING")
#define TR_EEPROMWARN          "EEPROM"
#define TR_EEPROM_CONVERTING   "EEPROM Converteren"
#define TR_THROTTLEWARN        "GAS"
#define TR_ALARMSWARN          "ALARM"
#define TR_SWITCHWARN          "SCHAKELAAR"
#define TR_FAILSAFEWARN        "FAILSAFE"
#define TR_NO_FAILSAFE         TR("Failsafe niet ing.", "Failsafe niet ingesteld")
#define TR_KEYSTUCK            "Toets klemt"
#define TR_INVERT_THR          TR("Gas omdraaien?", "Volgas achter?")
#define TR_SPEAKER_VOLUME      INDENT "Volume"
#define TR_LCD                 "LCD"
#define TR_BRIGHTNESS          INDENT "Helderheid"
#define TR_CPU_TEMP            "CPU-Temp.\016>"
#define TR_CPU_CURRENT         "Stroom\022>"
#define TR_CPU_MAH             "Verbruik"
#define TR_COPROC              "CoProc."
#define TR_COPROC_TEMP         "MB Temp. \016>"
#define TR_CAPAWARNING         INDENT "Capaciteit laag" // wg 9XR-Pro
#define TR_TEMPWARNING         INDENT "Overhitting"  //wg 9XR-Pro
#define TR_FUNC                "Funktie"
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Duur"
#define TR_DELAY               "Vertrag."
#define TR_SD_CARD             "SD-Card"
#define TR_SDHC_CARD           "SD-HC CARD"
#define TR_NO_SOUNDS_ON_SD     "Geen Geluiden" BREAKSPACE "op SD"
#define TR_NO_MODELS_ON_SD     "Geen Modellen" BREAKSPACE "op SD"
#define TR_NO_BITMAPS_ON_SD    "Geen Bitmaps" BREAKSPACE "op SD"
#define TR_NO_SCRIPTS_ON_SD    "Geen Scripts" BREAKSPACE "op SD"
#define TR_SCRIPT_SYNTAX_ERROR "Script syntax error"
#define TR_SCRIPT_PANIC        "Script panic"
#define TR_SCRIPT_KILLED       "Script killed"
#define TR_SCRIPT_ERROR        "Unknown error"
#define TR_PLAY_FILE           "Speel af"
#define TR_DELETE_FILE         "Verwijderen"
#define TR_COPY_FILE           "Kopieer"
#define TR_RENAME_FILE         "Hernoemen"
#define TR_ASSIGN_BITMAP       "Bitmap toekennen"
#define TR_EXECUTE_FILE        "Uitvoeren"
#define TR_REMOVED             " verwijderd"
#define TR_SD_INFO             "Informatie"
#define TR_SD_FORMAT           "Format"
#define TR_NA                  "N/A"
#define TR_HARDWARE            "HARDWARE"
#define TR_FORMATTING          "Formatteren..."
#define TR_TEMP_CALIB          "Temp.  Calib."
#define TR_TIME                "Tijd:"
#if defined(IMPERIAL_UNITS)
#define TR_TXTEMP              "Temp. TX\037\164@F"
#else
#define TR_TXTEMP              "Temp. TX\037\164@C"
#endif
#define TR_BAUDRATE            "BT Baudrate"
#define TR_SD_INFO_TITLE       "SD-INFO"
#define TR_SD_TYPE             "Type:"
#define TR_SD_SPEED            "Snelheid:"
#define TR_SD_SECTORS          "Sectoren:"
#define TR_SD_SIZE             "Grootte:"
#define TR_TYPE                INDENT "Type"
#define TR_GLOBAL_VARS         "Globale Variabelen"
#define TR_GLOBAL_V            "GLOBALE V."
#define TR_GLOBAL_VAR          "Globale Variabele"
#define TR_MENUGLOBALVARS      "GLOBALE VARIABELEN"
#define TR_OWN                 "Eigen"
#define TR_DATE                "Datum:"
#define TR_ROTARY_ENCODER      "Draaischakelaar"
#define TR_CHANNELS_MONITOR    "Kanaal-Monitor==>"
#define TR_MIXERS_MONITOR      "==>Mixer Monitor"
#define TR_PATH_TOO_LONG       "Pad te Lang"
#define TR_VIEW_TEXT           "Lees Tekst"
#define TR_FLASH_BOOTLOADER      "Flash BootLoader"
#define TR_FLASH_EXTERNAL_DEVICE "Flash extern Apparaat"
#define TR_FLASH_INTERNAL_MODULE "Flash interne XJT-Module"
#define TR_WRITING             "\032Schrijven..."
#define TR_CONFIRM_FORMAT      "Formatteren bevestigen?"
#define TR_INTERNALRF          "Interne RF"
#define TR_EXTERNALRF          "Externe RF"
#define TR_FAILSAFE            TR(INDENT "Failsafe", INDENT "Failsafe Modus")
#define TR_FAILSAFESET         "Failsafe instellen"
#define TR_MENUSENSOR          "SENSOR"
#define TR_COUNTRYCODE         "Landcode"
#define TR_VOICELANG           "Taal"
#define TR_UNITSSYSTEM         "Eenheden"
#define TR_EDIT                "Wijzigen"
#define TR_INSERT_BEFORE       "Invoegen ervoor"
#define TR_INSERT_AFTER        "Invoegen erna"
#define TR_COPY                "Kopieren"
#define TR_MOVE                "Verplaatsen"
#define TR_PASTE               "Plakken"
#define TR_DELETE              "Verwijderen"
#define TR_INSERT              "Invoegen"
#define TR_RESET_FLIGHT        "Reset Vliegdata"
#define TR_RESET_TIMER1        "Reset Timer1"
#define TR_RESET_TIMER2        "Reset Timer2"
#define TR_RESET_TIMER3        "Reset Timer3"
#define TR_RESET_TELEMETRY     "Reset Telemetrie"
#define TR_STATISTICS          "Statistieken"
#define TR_ABOUT_US            "De Programmeurs"
#define TR_AND_SWITCH          "AND Switch"
#define TR_SF                  "SF"
#define TR_GF                  "GF"
#define TR_SPEAKER             INDENT "Speaker"
#define TR_BUZZER              INDENT "Zoemer"
#define TR_BYTES               "bytes"
#define TR_MODULE_BIND         TR("[Bnd]", "[Bind]")
#define TR_MODULE_RANGE        TR("[Rng]", "[Range]")
#define TR_RESET_BTN           "[Reset]"
#define TR_SET                 "[Set]"
#define TR_TRAINER             "Trainer Poort"
#define TR_ANTENNAPROBLEM      CENTER "TX-Antenneprobleem!"
#define TR_MODELIDUSED         TR("ID al gebruikt", "Model-ID al gebruikt")
#define TR_MODULE              INDENT "Module-Type"
#define TR_TELEMETRY_TYPE      TR("Type", "Telemetrietype")
#define TR_TELEMETRY_SENSORS   "----Sensoren----"
#define TR_VALUE               "Waarde"
#define TR_TOPLCDTIMER         "Top LCD Timer"
#define TR_UNIT                "Eenheid"
#define TR_TELEMETRY_NEWSENSOR INDENT "Sensor toevoegen ..."
#define TR_CHANNELRANGE        TR(INDENT "Kanalen", INDENT "Uitgangs Kanalen")  //wg 9XR-Pro
#define TR_LOWALARM            INDENT "Waarschuwing"
#define TR_CRITICALALARM       INDENT "Kritiek Alarm"
#define TR_ENABLE_POPUP        "Inschakelen Popups"
#define TR_DISABLE_POPUP       "Uitschakelen Popups"
#define TR_CURVE_PRESET        "Preset..."
#define TR_PRESET              "Preset"
#define TR_MIRROR              "Spiegelen"
#define TR_CLEAR               "Wissen"
#define TR_RESET               "Reset Servowaardes"
#define TR_RESET_SUBMENU       "Reset..."
#define TR_COUNT               "Punten"
#define TR_PT                  "Pt"
#define TR_PTS                 "Ptn"
#define TR_SMOOTH              "Zacht"
#define TR_COPY_STICKS_TO_OFS  "Kopieer Sticks naar Subtrim"
#define TR_COPY_TRIMS_TO_OFS   "Kopieer Trim naar Subtrim"
#define TR_INCDEC              "Inc/Decrement"
#define TR_GLOBALVAR           "Globale Var"
#define TR_MIXSOURCE           "Mixer Bron"
#define TR_CONSTANT            "Constant"
#define TR_PERSISTENT_MAH      TR(INDENT "Str mAh", INDENT "Vasthouden mAh")
#define TR_PREFLIGHT           "Preflight Checks"
#define TR_CHECKLIST           TR(INDENT "Checklist", INDENT "Display Checklist")
#define TR_FAS_OFFSET          TR(INDENT "FAS Ofs", INDENT "FAS Offset")
#define TR_UART3MODE           "Seriele poort"
#define TR_SCRIPT              "Script"
#define TR_INPUTS              "Inputs"
#define TR_OUTPUTS             "Outputs"
#define TR_EEBACKUP            "\012[ENTER Lang] voor een EEPROM backup"
#define TR_FACTORYRESET        "\012[MENU Lang]: Factory reset"

#if defined(X_ANY)
  #define TR_X_ANY                        "X ANY"
  #define TR_ACTIVED                      "Active"
  #define TR_NUMBER                       "Number"
  #define TR_CHANNEL                      "Channel "
  #define TR_NB_REPEAT                    "Nb repeat "
  #define TR_SWITCHES                     "Sw. "
  #define TR_ANGLE_SENSOR                 "0:360"
#endif
