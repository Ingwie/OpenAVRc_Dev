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
#define TR_OFFON               "WY\214""ZA\214"

#define LEN_MMMINV             "\003"
#define TR_MMMINV              "---""REV"

#define LEN_NCHANNELS          "\004"
#define TR_NCHANNELS           "\0014KN\0016KN\0018KN10KN12KN14KN16KN"

#define LEN_VBEEPMODE          "\005"
#define TR_VBEEPMODE           "Cichy""Alarm""BezPr""Wsz.\0"

#define LEN_VBEEPLEN           "\005"
#define TR_VBEEPLEN            "0====""=0===""==0==""===0=""====0"

#define LEN_VRENAVIG           "\003"
#define TR_VRENAVIG            "No REaREb"

#define LEN_VBLMODE            "\004"
#define TR_VBLMODE             "Wy\203\0""Przy""Dr\200\207""Oba\0""Za\203\0"

#define LEN_TRNMODE            "\003"
#define TR_TRNMODE             "Wy\203"" +="" :="

#define LEN_TRNCHN             "\003"
#define TR_TRNCHN              "KN1KN2KN3KN4"


#define LEN_UART3MODES         "\015"  /*13 decimal*/
#define TR_UART3MODES          "Wy\203\200cz\0      ""S-Port Kopia ""Telemetria\0  ""Trener SBUS\0 ""Debug\0        "

#define LEN_SWTYPES            "\006"
#define TR_SWTYPES             "Brak\0 ""Chwil.""2POZ\0 ""3POZ\0"

#define LEN_POTTYPES           "\017" /*15 decimal*/
#define TR_POTTYPES            "Brak\0          ""Poten z zapadk\200""Prze\203.Wielopoz.""Potencjometr\0  "

#define LEN_SLIDERTYPES        "\006"
#define TR_SLIDERTYPES         "Brak\0 ""Suwak "

#define LEN_DATETIME           "\005"
#define TR_DATETIME            "DATA:""CZAS:"

#define LEN_VLCD               "\006"
#define TR_VLCD                "NormalOptrex"

#define LEN_VPERSISTENT        "\014"  /*12 decimal*/
#define TR_VPERSISTENT         "Wy\203\200cz\0     ""Lot\0        ""R\202czny Reset"

#define LEN_COUNTRYCODES       "\002"
#define TR_COUNTRYCODES        "US""JP""EU"

#define LEN_TARANIS_PROTOCOLS  "\004"
#define TR_TARANIS_PROTOCOLS   "OFF\0""PPM\0""XJT\0""DSM2""CRSF"

#define LEN_XJT_PROTOCOLS      "\004"
#define TR_XJT_PROTOCOLS       "OFF\0""D16\0""D8\0 ""LR12"

#define LEN_DSM_PROTOCOLS      "\004"
#define TR_DSM_PROTOCOLS       "LP45""DSM2""DSMX"

#define LEN_VTRIMINC           "\006"
#define TR_VTRIMINC            "Expo\0 ""B.Dok\203""Dok\203.\0""\217redni""Zgrubn"


#define LEN_VDISPLAYTRIMS      "\006"
#define TR_VDISPLAYTRIMS       "Nie\0  ""Zmie\204\0""Tak\0"

#define LEN_VBEEPCOUNTDOWN     "\006"
#define TR_VBEEPCOUNTDOWN      "Cichy Pik  \0D\210wi\202kWibrac"

#define LEN_VVARIOCENTER       "\006"
#define TR_VVARIOCENTER        "Ton\0  ""Cicho "

#define LEN_CURVE_TYPES        "\010" /*8 decimal*/
#define TR_CURVE_TYPES         "Standard""W\203asny\0 "

#define LEN_RETA123            "\001"

    #define TR_RETA123         "KWGL123ab"

#define LEN_VPROTOS            "\006"

#if defined(PXX)
  #define TR_PXX              "PXX\0  "
#elif defined(DSM2_SERIAL)
  #define TR_PXX             "[PXX]\0"
#else
  #define TR_PXX
#endif

#if defined(DSM2_SERIAL)
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
#define TR_POSNEG              "POZ""NEG"


#define LEN_VCURVEFUNC         "\003"
#define TR_VCURVEFUNC          "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_VMLTPX             "\010" /*8 decimal*/
#define TR_VMLTPX              "Dodaj\0  ""Mn\205\207    ""Zast\200p\0 "

#define LEN_VMLTPX2            "\002"
#define TR_VMLTPX2             "+=""*="":="

#define LEN_VMIXTRIMS          "\003"
#define TR_VMIXTRIMS           "WY\214""ZA\214""SK\0""SW\0""GAZ""Lot"

  #define TR_CSWTIMER          "Tim\0 "
  #define TR_CSWSTICKY         "Sta\203y"
    #define TR_CSWRANGE
    #define TR_CSWSTAY



#define LEN_VCSWFUNC           "\005"
#define TR_VCSWFUNC            "---\0 " "a=x\0 " "a\173x\0 ""a>x\0 ""a<x\0 " TR_CSWRANGE "|a|>x""|a|<x""AND\0 ""OR\0  ""XOR\0 " TR_CSWSTAY "a=b\0 ""a>b\0 ""a<b\0 ""^}x\0 ""|^|}x" TR_CSWTIMER TR_CSWSTICKY

#define LEN_VFSWFUNC           "\012" /*10 decimal*/

#if defined(VARIO)
  #define TR_VVARIO            "Wario\0    "
#else
  #define TR_VVARIO            "[Wario]\0  "
#endif

#if defined(AUDIO)
  #define TR_SOUND             "GrajD\210wi\202k"
#else
  #define TR_SOUND             "Pikanie\0  "
#endif

#if defined(HAPTIC)
  #define TR_HAPTIC            "Wibracje\0 "
#else
  #define TR_HAPTIC            "[Wibracje]"
#endif

#if defined(VOICE)
    #define TR_PLAY_TRACK      "Graj\217cie\207k"
  #define TR_PLAY_BOTH         "Graj Oba\0 "
  #define TR_PLAY_VALUE        "Graj Wart\0"
#else
  #define TR_PLAY_TRACK        "[Graj \217ci]"
  #define TR_PLAY_BOTH         "[Graj Oba]"
  #define TR_PLAY_VALUE        "[Graj War]"
#endif

#define TR_SF_BG_MUSIC        "Muz. t\203a\0 ""Muz.t\203a ||"

#if defined(SDCARD)
  #define TR_SDCLOGS           "Logi->SD\0 "
#else
  #define TR_SDCLOGS           "[Logowa\201]\0"
#endif

#if defined(GVARS)
  #define TR_ADJUST_GVAR       "Ustaw \0   "
#else
  #define TR_ADJUST_GVAR       "[UstawZG] "
#endif

  #define TR_SF_PLAY_SCRIPT   "[Lua]\0    "

#if defined(DEBUG)
  #define TR_SF_TEST          "Test\0"
#else
  #define TR_SF_TEST
#endif

#if   defined(OVERRIDE_CHANNEL_FUNCTION)
  #define TR_SF_SAFETY        "Bezp.\0    "
#else
  #define TR_SF_SAFETY        "---\0      "
#endif

  #define TR_SF_SCREENSHOT

#define TR_SF_RESERVE         "[rezerwa]\0"

  #define TR_VFSWFUNC          TR_SF_SAFETY "Trener \0  ""Inst-Trim ""Resetuj\0  " TR_ADJUST_GVAR TR_SOUND TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_VVARIO TR_HAPTIC TR_SDCLOGS "Pod\206wietl\0" TR_SF_TEST

#define LEN_VFSWRESET          "\004"

#if defined(FRSKY)
  #define TR_FSW_RESET_TELEM   "Telm"
  #define TR_MODELISON         INDENT "Modellu is ON"
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

#define TR_VFSWRESET           TR_FSW_RESET_TIMERS "All\0" TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC

#define LEN_FUNCSOUNDS         "\004"
#define TR_FUNCSOUNDS          "Bp1\0""Bp2\0""Bp3\0""Ost1""Ost2""Chee""Rata""Tik ""Syre""Dzwo""SciF""Robt""Chrp""Tada""Crck""Alrm"

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

  #define TR_VTELEMCHNS        "---\0""Bat\0" TR_TELEM_TIME TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_TIMERS TR_SWR "Tx\0 " TR_TELEM_RSSI_RX TR_RX_BATT "A1\0 ""A2\0 " TR_A3_A4 "Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""Hdg\0""VSpd""ASpd""dTE\0" TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE "A1-\0""A2-\0" TR_A3_A4_MIN "Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""Dst+" TR_ASPD_MAX "Cel-""Cls-""Vfs-""Cur+""Pwr+" TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE TR_TELEM_RESERVE "Acc\0""Time"

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
#define TR_VALARM              "---""\220\205\203""Pom""Cze"

#define LEN_VALARMFN           "\001"
#define TR_VALARMFN            "<>"

#define LEN_VTELPROTO          "\007"
#define TR_VTELPROTO           "None\0  ""Hub\0   ""WSHHigh""S.Port\0"

  #define LEN_AMPSRC           "\003"
  #define TR_AMPSRC            "---""A1\0""A2\0""FAS""Cel"

#define LEN_VARIOSRC           "\004"
#if defined(FRSKY_SPORT)
  #define TR_VARIOSRC          "VSpd""A1\0 ""A2\0 ""dTE\0"
#else
  #define TR_VARIOSRC          "Wys\0""Wys+""VSpd""A1\0 ""A2\0"
#endif

  #define LEN_VTELEMSCREENTYPE "\004"
  #define TR_VTELEMSCREENTYPE    "Licz""Pask"

#define LEN_GPSFORMAT          "\004"
#define TR_GPSFORMAT           "DMS\0""NMEA"

#define LEN2_VTEMPLATES        13
#define LEN_VTEMPLATES         "\015" /*13 decimal*/
#define TR_VTEMPLATES          "Usu\204 Miksy ""Prosty. 4kn\0 ""Prze\203\200-T-Cut\0""V-Tail      \0""Elevon-Delta\0""eCCPM       \0""Ustaw Heli  \0""Test serwa  \0"

#define LEN_VSWASHTYPE         "\004"
#define TR_VSWASHTYPE          "---\0""120\0""120X""140\0""90\0"

#define LEN_VKEYS              "\005"
#define TR_VKEYS               "Menu\0""Exit\0""D\205\203 \0""G\205ra\0""Prawo""Lewo\0"

#define LEN_VRENCODERS         "\003"
#define TR_VRENCODERS          "REa""REb"

#define LEN_VSWITCHES          "\003"
#define LEN_VSRCRAW            "\004"

#define TR_STICKS_VSRCRAW      "SK \0""SW \0""Gaz\0""Lot\0"
#define TR_TRIMS_VSRCRAW       "TrSK""TrSW""TrGA""TrLO"

  #define TR_POTS_VSRCRAW      "P1\0 ""P2\0 ""P3\0 "
  #define TR_SW_VSRCRAW        "3POS"
  #define TR_9X_3POS_SWITCHES  "ID0""ID1""ID2"

  #define TR_LOGICALSW         "L1\0""L2\0""L3\0""L4\0""L5\0""L6\0""L7\0""L8\0""L9\0""L10""L11""L12""L13""L14""L15""L16""L17""L18""L19""L20"

#define TR_TRIMS_SWITCHES      "tRl""tRr""tEd""tEu""tTd""tTu""tAl""tAr"

  #define TR_ROTARY_ENCODERS   "REa\0""REb\0"
  #define TR_ROTENC_SWITCHES   "REA""REB""REN"

  #define TR_PHYS_SWITCHES     "GAZ""SK ""SW ""LOT""GEA""TRN"

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
#define TR_VTMRMODES           "WY\214""ABS""THs""TH%""THt"

#define LEN_VTRAINERMODES      "\022" /*18 decimal*/
#define TR_VTRAINERMODES       "Trener/Jack\0      ""Ucze\204/Jack\0       ""Trener/SBUS Modu\203 ""Trener/CPPM Modu\203 ""Trener/Bateria\0   "

#define LEN_VFAILSAFE          "\011" /*9 decimal*/
#define TR_VFAILSAFE           "Brak   \0 ""Utrzymuj\0""W\203asne  \0""0 sygna\203u""Odbiornik"

#if defined(MAVLINK)
  #define LEN_MAVLINK_BAUDS    "\006"
  #define TR_MAVLINK_BAUDS     "4800  ""9600  ""14400 ""19200 ""38400 ""57600 ""76800 ""115200"
  #define LEN_MAVLINK_AC_MODES "\011" /*9 decimal*/
  #define TR_MAVLINK_AC_MODES  "Stabiliz.""Akro     ""Alt Hold ""Auto     ""Sterowany""Loiter   ""RTL      ""Ko\203o     ""Utrzymuj ""L\200duj    ""OF Loiter""Toy A    ""Toy M    ""B\214\213DNY   "
  #define LEN_MAVLINK_AP_MODES "\015" /*13 decimal*/
  #define TR_MAVLINK_AP_MODES  "R\202czny      ""Ko\203o         ""Stabilizacja ""Trening      ""Fly by Wire A""Fly by Wire A""Auto         ""RTL          ""Loiter       ""Sterowany    ""Inicjalizacja""B\214\213DNY       "
#endif

#define LEN_VSENSORTYPES        "\012" /*10 decimal*/
#define TR_VSENSORTYPES        "U\207ytkownik""Obliczone "

#define LEN_VFORMULAS          "\010" /*8 decimal*/
#define TR_VFORMULAS           "Dodaj\0  ""\217rednie\0""Min\0    ""Max\0    ""Mn\205\207\0   ""Zliczani""Kom\205rka\0""Zu\207ycie\0""Zasi\202g\0 "

#define LEN_VPREC              "\004"
#define TR_VPREC               "0.--""0.0 ""0.00"

#define LEN_VCELLINDEX         "\007"
#define TR_VCELLINDEX          "Niskie\0""1\0     ""2\0     ""3\0     ""4\0     ""5\0     ""6\0     ""Wysokie""Delta\0"

// ZERO TERMINATED STRINGS
  #define INDENT               "\001"
  #define LEN_INDENT           1
  #define INDENT_WIDTH         (FW/2)
  #define BREAKSPACE           " "

  #define TR_ENTER             "[MENU]"

#define TR_EXIT                "[WYJ\217]"

  #define TR_POPUPS            TR_ENTER "\010" TR_EXIT
  #define OFS_EXIT             sizeof(TR_ENTER)

#define TR_MENUWHENDONE        CENTER "\006"TR_ENTER " > DALEJ  "
#define TR_FREE                "woln"
#define TR_DELETEMODEL         "SKASUJ MODEL"
#define TR_COPYINGMODEL        "Kopiuj model.."
#define TR_MOVINGMODEL         "Przenie\206 model."
#define TR_LOADINGMODEL        "Za\203aduj model.."
#define TR_NAME                "Nazwa"
#define TR_MODELNAME           "Nazwa modelu"
#define TR_PHASENAME           "Nazwa Fazy"
#define TR_MIXNAME             "Nazwa Mix"
#define TR_INPUTNAME           "Wpisz Nazw"
#define TR_EXPONAME            "Nazwa Exp"
#define TR_BITMAP              "Obrazek    "
#define TR_TIMER               "Timer"
#define TR_ELIMITS             "Limi+25%"
#define TR_ETRIMS              "E.Trym "
#define TR_TRIMINC             "Krok Trym"
#define TR_DISPLAY_TRIMS       "Pok.Trym."
#define TR_TTRACE              "\221r\205d gaz"
#define TR_TSWITCH             "T-Switch"
#define TR_TTRIM               "TryGaz"
#define TR_BEEPCTR             "\217rodBeep"
#define TR_USE_GLOBAL_FUNCS    "Funk.Glb."
#define TR_PROTO               INDENT "Proto"
  #define TR_PPMFRAME          INDENT "Ramka PPM"
#define TR_MS                  "ms"
#define TR_SWITCH              "Prze\203\200"
#define TR_TRIMS               "Trymy"
#define TR_FADEIN              "Pojawia"
#define TR_FADEOUT             "Zanik   "
#define TR_DEFAULT             "(Bazowa) "
#define TR_CHECKTRIMS          CENTER "\006Spr  \012Trymy"
#define OFS_CHECKTRIMS         CENTER_OFS+(9*FW)
#define TR_SWASHTYPE           "Typ tarczy"
#define TR_COLLECTIVE          "Kolektyw"
#define TR_AILERON             "Lateral cyc. source"
#define TR_ELEVATOR            "Long. cyc. source"
#define TR_SWASHRING           "Tarcza    "
#define TR_ELEDIRECTION        "Ster Wysoko\206c"
#define TR_AILDIRECTION        "Kierunek Lotk"
#define TR_COLDIRECTION        "PIT Direction"
#define TR_MODE                INDENT "Tryb"
#define TR_NOFREEEXPO          "BrakWoln.Expo!"
#define TR_NOFREEMIXER         "BrakWoln.Mix\205w!"
#define TR_INSERTMIX           "WSTAW MIX"
#define TR_EDITMIX             "EDYTUJ MIX"
#define TR_SOURCE              INDENT "\221r\205d\203o"
#define TR_WEIGHT              "Waga "
#define TR_EXPO                "Expo"
#define TR_SIDE                "Strona"
#define TR_DIFFERENTIAL        "R\205\207nic"
#define TR_OFFSET              INDENT"Ofset "
#define TR_TRIM                "Trym"
#define TR_DREX                "DRex"
#define DREX_CHBOX_OFFSET      30
#define TR_CURVE               "Krzywa"
#define TR_FLMODE              "Tryb"
#define TR_MIXWARNING          "UWAGA"
#define TR_OFF                 "Wy\203."
#define TR_MULTPX              "Z\203\200cz."
#define TR_DELAYDOWN           "Op\205zn.(-)"
#define TR_DELAYUP             "Op\205zn.(+)"
#define TR_SLOWDOWN            "Spowoln.(-)"
#define TR_SLOWUP              "Spowoln.(+)"
#define TR_MIXER               "MIKSERY"
#define TR_CV                  "Kr"
#define TR_GV                  "ZG"
#define TR_ACHANNEL            "A\004Kana\203"
#define TR_RANGE               INDENT "Zakres"
#define TR_CENTER              INDENT "\217rodek"
#define TR_BAR                 "Pas"
#define TR_ALARM               INDENT "Alarm"
#define TR_USRDATA             "Dane U\207ytk."
#define TR_BLADES              " \214opaty \206migla"
#define TR_SCREEN              "Ekran "
#define TR_SOUND_LABEL         "D\210wi\202k"
#define TR_LENGTH              INDENT"D\203ugo\206\201"
#define TR_BEEP_LENGTH         INDENT "D\203ug. Beepa"
#define TR_SPKRPITCH           INDENT"D\210wi\202k"
#define TR_HAPTIC_LABEL        "Wibracja"
#define TR_HAPTICSTRENGTH      INDENT"Si\203a"
#define TR_CONTRAST            "Kontrast"
#define TR_ALARMS_LABEL        "Alarmy"
#define TR_BATTERY_RANGE       "Poka\207PasBat."
#define TR_BATTERYWARNING      INDENT"Bateria roz\203adowana"
#define TR_INACTIVITYALARM     INDENT"Alarm bezczynno\206ci"
#define TR_MEMORYWARNING       INDENT"Pe\203na pami\202\201"
#define TR_ALARMWARNING        INDENT"Wy\203\200cz d\210wi\202k"
#define TR_RENAVIG             "Nawigacja potencjometrem"
#define TR_THROTTLE_LABEL      "Gaz    "
#define TR_THROTTLEREVERSE     "OdwrGaz"
#define TR_TIMER_NAME          INDENT "Nazwa"
#define TR_MINUTEBEEP          INDENT "Minuta"
#define TR_BEEPCOUNTDOWN       INDENT "Odliczanie"
#define TR_PERSISTENT          INDENT "Dok\203."
#define TR_BACKLIGHT_LABEL     "Pod\206wietl"
#define TR_BLDELAY             INDENT"Czas trwania"
#define TR_BLONBRIGHTNESS      INDENT"Jasno\206c w\203."
#define TR_BLOFFBRIGHTNESS     INDENT"Jasno\206\201 wy\203."
#define TR_BLCOLOR             INDENT "Color"
#define TR_SPLASHSCREEN        "Logo ekranu"
#define TR_THROTTLEWARNING     IF_CPUARM(INDENT)"OstrzGaz"
#define TR_SWITCHWARNING       "OstrzPrze\203"
#define TR_POTWARNING          "Ostrz.Poten."
#define TR_TIMEZONE            "Strefa czasowa" 
#define TR_ADJUST_RTC          "RegulujRTC"
#define TR_GPS                 "GPS"
#define TR_RXCHANNELORD        "KolejnKan.RX"
#define TR_STICKS              "Dr\200\207ki"
#define TR_POTS                "Pots"
#define TR_SWITCHES_DELAY      "Op\205\210n.Prze\203."
#define TR_SLAVE               "Ucze\204"
#define TR_MODESRC             " Tryb\006% \221r\205d\203o"
#define TR_MULTIPLIER          "Mno\207nik"
#define TR_CAL                 "Kal."
#define TR_VTRIM               "Trym- +"
#define TR_BG                  "BG:"
  #define TR_MENUTOSTART       CENTER "\010" TR_ENTER " KALIBRUJ"
  #define TR_SETMIDPOINT         CENTER "\002Cenruj dr\200\207ki/potencj"
  #define TR_MOVESTICKSPOTS      "Poruszaj dr\200\207kami/pot"
#define TR_RXBATT              "Rx Batt:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              CENTER "BrakDAN"
#define TR_TOTTM1TM2THRTHP     "\037\146SES\036TM1\037\146TM2\036THR\037\146TH%"
#define TR_US                  "us"
#define TR_MENUTORESET         CENTER TR_ENTER " >> Reset"
#define TR_PPM_TRAINER         "TR"
#define TR_CH                  "KN"
#define TR_MODEL               "MODEL"
#define TR_FP                  "FL"
#define TR_MIX                 "MIX"
#define TR_EEPROMLOWMEM        "Niska pami\202\201 EEPROM"
#define TR_ALERT               "\006UWAGA"
#define TR_PRESSANYKEYTOSKIP   "Wci\206nij guzik->ignoruj"
#define TR_THROTTLENOTIDLE     "Gaz nie na zerze"
#define TR_ALARMSDISABLED      "Alarmy wy\203\200czone"
#define TR_PRESSANYKEY         "\010Wci\206nij jaki\206 klawisz"
#define TR_BADEEPROMDATA       "\006B\203\200d danych EEPROM"
#define TR_EEPROMFORMATTING    "\004Formatowanie EEPROM"
#define TR_EEPROMOVERFLOW      "Prze\203adowany EEPROM"
#define TR_MENURADIOSETUP      "USTAWIENIA RADIA"
#define TR_MENUDATEANDTIME     "DATA I CZAS"
#define TR_MENUTRAINER         "TRENER"
#define TR_MENUGLOBALFUNCS     "FUNKCJE GLOBALNE"
#define TR_MENUVERSION         "WERSJA"
#define TR_MENUDIAG            "PRZE\214"
#define TR_MENUANA             "WE-ANA"
#define TR_MENUCALIBRATION     "KALIBRACJA"
  #define TR_TRIMS2OFFSETS     "\006Trims => Subtrims"
#define TR_MENUMODELSEL        "WYB.MODEL"
#define TR_MENUSETUP           "USTAW"
#define TR_MENUFLIGHTPHASE     "FAZA LOTU"
#define TR_MENUFLIGHTPHASES    "FAZY LOTU"
#define TR_MENUHELISETUP       "USTAW HELI"

#define TR_MENUINPUTS        "DR\211\220KI"
#define TR_MENULIMITS        "SERWA "

#define TR_MENUCURVES          "KRZYWE"
#define TR_MENUCURVE           "KRZYWA"
#define TR_MENULOGICALSWITCH    "PRZE\214. LOGICZNY"
#define TR_MENULOGICALSWITCHES   "PRZE\214. LOG."
#define TR_MENUCUSTOMFUNC      "FUNKCJE SPECJALNE"
#define TR_MENUCUSTOMSCRIPTS   "W\214ASNE SKRYPTY"
#define TR_MENUCUSTOMSCRIPT    "W\214ASNY SKRYPT"
#define TR_MENUTELEMETRY       "TELEMETRIA"
#define TR_MENUTEMPLATES       "SZABLONY"
#define TR_MENUSTAT            "STAT"
#define TR_MENUDEBUG           "DEBUG"
  #define TR_RXNUM             "NumOdb"
#define TR_SYNCMENU            "[Synch]"
#define TR_LIMIT               INDENT "Limit"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Szer. geo."
#define TR_LONGITUDE           "D\203.   geo."
#define TR_GPSCOORD            "Koord.GPS"
#define TR_VARIO               "Wario"
#define TR_PITCH_AT_ZERO       INDENT "Ton przy Zerze"
#define TR_PITCH_AT_MAX        INDENT "Ton przy Max"
#define TR_REPEAT_AT_ZERO      INDENT "Powt\205rz przy Zerze"
#define TR_POWEROFF            "\027Wy\203\200czanie.."
#define TR_SHUTDOWN            "Wy\203\200czanie.."
#define TR_SAVEMODEL           "Zapisz ustwienia modelu"
#define TR_BATT_CALIB          "Kalib:Baterii"
#define TR_CURRENT_CALIB       " +=\006Kalibracja pr\200du"
#define TR_VOLTAGE             TR (INDENT"Napi\202cie"
#define TR_CURRENT             TR (INDENT"Pr\200d"
#define TR_SELECT_MODEL        "Wyb\205r modelu"
#define TR_CREATE_MODEL        "Nowy model"
#define TR_COPY_MODEL          "Skopiuj model"
#define TR_MOVE_MODEL          "Przenie\206 model"
#define TR_BACKUP_MODEL        "Zbackupuj model"
#define TR_DELETE_MODEL        "Skasuj model"
#define TR_RESTORE_MODEL       "Odtw\205rz model"
#define TR_SDCARD_ERROR        "B\203\200d karty SD"
#define TR_NO_SDCARD           "Brak karty SD"
#define TR_SDCARD_FULL         "Karta Pe\203na "
#define TR_INCOMPATIBLE        "Niekompatybilne"
#define TR_WARNING             "UWAGA"
#define TR_EEPROMWARN          "EEPROM"
#define TR_EEPROM_CONVERTING   "Konwersja EEPROM"
#define TR_THROTTLEWARN        "GAZ"
#define TR_ALARMSWARN          "ALARM"
#define TR_SWITCHWARN          "Prze\203\200"
#define TR_FAILSAFEWARN        "FAILSAFE"
#define TR_NO_FAILSAFE         "Brak Failsafe"
#define TR_KEYSTUCK            "Blokada klucza"
#define TR_INVERT_THR          "Odw.Gaz?"
#define TR_SPEAKER_VOLUME      INDENT "G\203o\206no\206\201"
#define TR_LCD                 "LCD"
#define TR_BRIGHTNESS          INDENT "Jasno\206\201"
#define TR_CPU_TEMP            "Temp. CPU\016>"
#define TR_CPU_CURRENT         "Pr\200d"
#define TR_CPU_MAH             "Zu\207ycie"
#define TR_COPROC              "CoProc."
#define TR_COPROC_TEMP         "Temp. MB"
#define TR_CAPAWARNING         INDENT "Ma\203a pojemno\206\201"
#define TR_TEMPWARNING         INDENT "Przegrzanie"
#define TR_FUNC                "Funkc"
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Czas trwania"
#define TR_DELAY               "Op\205\210nienie"
#define TR_SD_CARD             "SD"
#define TR_SDHC_CARD           "SD-HC"
#define TR_NO_SOUNDS_ON_SD     "Brak d\210wi\202k\205w na SD"
#define TR_NO_MODELS_ON_SD     "Brak modelu na SD"
#define TR_NO_BITMAPS_ON_SD    "Brak obrazk\205w na SD"
#define TR_NO_SCRIPTS_ON_SD    "Brak skrypt\205w na SD"
#define TR_SCRIPT_SYNTAX_ERROR "Skrypt:syntax error"
#define TR_SCRIPT_PANIC        "Skrypt:panic"
#define TR_SCRIPT_KILLED       "Skrypt:killed"
#define TR_SCRIPT_ERROR        "Nieznany b\203\200d"
#define TR_PLAY_FILE           "Odtw\205rz"
#define TR_DELETE_FILE         "Skasuj"
#define TR_COPY_FILE           "Kopiuj"
#define TR_RENAME_FILE         "Zmie\204 nazw\202"
#define TR_ASSIGN_BITMAP       "Wybierz obrazek"
#define TR_EXECUTE_FILE        "Wykonaj"
#define TR_REMOVED             " skasowane"
#define TR_SD_INFO             "Informacje"
#define TR_SD_FORMAT           "Formatuj"
#define TR_NA                  "[X]"
#define TR_HARDWARE            "HARDWARE"
#define TR_FORMATTING          "Formatowanie.."
#define TR_TEMP_CALIB          "Temp. kalibracji"
#define TR_TIME                "Czas"
#if defined(IMPERIAL_UNITS)
#define TR_TXTEMP              "Temp. TX\037\164@F"
#else
#define TR_TXTEMP              "Temp. TX\037\164@C"
#endif
#define TR_BAUDRATE            "Pr\202dko\206\201 BT"
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Typ:"
#define TR_SD_SPEED            "Pr\202dko\206\201:"
#define TR_SD_SECTORS          "Sektory :"
#define TR_SD_SIZE             "Wielko\206\201:"
#define TR_TYPE                INDENT TR_SD_TYPE
#define TR_GLOBAL_VARS         "Zmienne Globalne"
#define TR_GLOBAL_V            "Zmien.Glob."
#define TR_GLOBAL_VAR          "Zmienna Globalna"
#define TR_MENUGLOBALVARS      "ZMIENNE GLOBALNE"
#define TR_OWN                 "W\203asno\206\201"
#define TR_DATE                "Data"
#define TR_ROTARY_ENCODER      "R.Encs"
#define TR_CHANNELS_MONITOR    "MONITOR KANA\214\216W"
#define TR_MIXERS_MONITOR      "MONITOR MIKSER"
#define TR_PATH_TOO_LONG       "\217cie\207ka za d\203uga"
#define TR_VIEW_TEXT           "Poka\207 tekst"
#define TR_FLASH_BOOTLOADER    "Flash BootLoader"
#define TR_FLASH_EXTERNAL_DEVICE "Sflashuj Modu\203 Zewn\202trzny"
#define TR_FLASH_INTERNAL_MODULE "Sflashuj Modu\203 Wewn\202trzny"
#define TR_WRITING             "\032Zapis...  "
#define TR_CONFIRM_FORMAT      "Zatwierd\210 Format?"
#define TR_INTERNALRF          "Wewn.Modu\203 RF"
#define TR_EXTERNALRF          "Zewn.Modu\203 RF"
#define TR_FAILSAFE            INDENT"Failsafe"
#define TR_FAILSAFESET         "USTAWIENIE FAILSAFE"
#define TR_MENUSENSOR          "CZUJNIK"
#define TR_COUNTRYCODE         "Kod regionu"
#define TR_VOICELANG           "J\202zyk g\203osu"
#define TR_UNITSSYSTEM         "Jednostki"
#define TR_EDIT                "Edytuj"
#define TR_INSERT_BEFORE       "Wstaw przed"
#define TR_INSERT_AFTER        "Wstaw za"
#define TR_COPY                "Kopiuj"
#define TR_MOVE                "Przenie\206"
#define TR_PASTE               "Wklej"
#define TR_DELETE              "Kasuj"
#define TR_INSERT              "Wstaw"
#define TR_RESET_FLIGHT        "Zeruj lot"
#define TR_RESET_TIMER1        "Zeruj Timer1"
#define TR_RESET_TIMER2        "Zeruj Timer2"
#define TR_RESET_TIMER3        "Zeruj Timer3"
#define TR_RESET_TELEMETRY     "Wyczy\206\201 telemetri\202"
#define TR_STATISTICS          "Statystyki"
#define TR_SAVE_TIMERS         "Save Timers"
#define TR_ABOUT_US            "O nas"
#define TR_AND_SWITCH          "Prze\203\200cznik AND"
#define TR_SF                  "FS"
#define TR_GF                  "FG"
#define TR_SPEAKER             INDENT"G\203o\206nik"
#define TR_BUZZER              INDENT"Brz\202czyk"
#define TR_BYTES               "bajty"
#define TR_MODULE_BIND         "[BND]"
#define TR_MODULE_RANGE        "[Zas]"
#define TR_RESET_BTN           "[Reset]"
#define TR_SET                 "[Set]"
#define TR_TRAINER             "Port Trenera"
#define TR_ANTENNAPROBLEM      CENTER "Problem z anten\200 TX"
#define TR_MODELIDUSED         "ID zaj\202te"
#define TR_MODULE              INDENT "Modu\203 "
#define TR_TELEMETRY_TYPE      "Typ "
#define TR_TELEMETRY_SENSORS   "Czujniki"
#define TR_VALUE               "Warto\206\201"
#define TR_TOPLCDTIMER         "Top LCD Timer"
#define TR_UNIT                "Jedn"
#define TR_TELEMETRY_NEWSENSOR INDENT "Dodaj nowy czujnik..."
#define TR_CHANNELRANGE        INDENT "ZakrKn"
#define TR_LOWALARM            INDENT "Alarm niski"
#define TR_CRITICALALARM       INDENT "Alarm krytyczny"
#define TR_ENABLE_POPUP        "Aktywuj Popup"
#define TR_DISABLE_POPUP       "Wy\203\200cz Popup"
#define TR_CURVE_PRESET        "Predef..."
#define TR_PRESET              "Predef."
#define TR_MIRROR              "Lustro"
#define TR_CLEAR               "Wyczy\206\201"
#define TR_RESET               "Resetuj"
#define TR_RESET_SUBMENU       "Reset..."
#define TR_COUNT               "Punkty"
#define TR_PT                  "pkt"
#define TR_PTS                 "pkty"
#define TR_SMOOTH              "G\203adka"
#define TR_COPY_STICKS_TO_OFS  "Skopiuj Dr\200\207ki Do Offsetu"
#define TR_COPY_TRIMS_TO_OFS   "Skopiuj Trymery Do subtrymer\205w"
#define TR_INCDEC              "Zwi\202ksz/Zmnie"
#define TR_GLOBALVAR           "Zm.Global."
#define TR_MIXSOURCE           "\221r\205d\203o Miks."
#define TR_CONSTANT            "Stala   "
#define TR_PERSISTENT_MAH      INDENT "Zap. mAh"
#define TR_PREFLIGHT           "Lista Ostrze\207e\204"
#define TR_CHECKLIST           INDENT "Czeklista"
#define TR_FAS_OFFSET          INDENT "FAS Ofs"
#define TR_UART3MODE           "Port szreg."
#define TR_SCRIPT              "Skrypt"
#define TR_INPUTS              "Wej\206cia"
#define TR_OUTPUTS             "Wyj\206cia"
#define TR_EEBACKUP            "\012[ENTER D\203ugo] Backup EEPROMu"
#define TR_FACTORYRESET        "\012[MENU Long]: Ustaw.Fabrycz"

#if defined(X_ANY)
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
