EESchema Schematic File Version 2
LIBS:Mega_2560 core mini_full_2.0-rescue
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:OPEN_EVO
LIBS:Mega_2560 core mini_full_2.0-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 7
Title "Shield Mega2560 core Full"
Date "2017-11-05"
Rev "2.00"
Comp ""
Comment1 "Reproduction et commercialisation interdite"
Comment2 "By Antho.breizh / pierrotm777"
Comment3 ""
Comment4 "Shield pour RC OpenAVRc"
$EndDescr
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR01
U 1 1 58A78C75
P 6600 7400
F 0 "#PWR01" H 6600 7150 50  0001 C CNN
F 1 "GND" H 6600 7250 50  0000 C CNN
F 2 "" H 6600 7400 50  0000 C CNN
F 3 "" H 6600 7400 50  0000 C CNN
	1    6600 7400
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG02
U 1 1 58A78C76
P 6600 7400
F 0 "#FLG02" H 6600 7495 50  0001 C CNN
F 1 "PWR_FLAG" H 6600 7580 50  0000 C CNN
F 2 "" H 6600 7400 50  0000 C CNN
F 3 "" H 6600 7400 50  0000 C CNN
	1    6600 7400
	1    0    0    -1  
$EndComp
$Comp
L +BATT #PWR03
U 1 1 58A78C78
P 6600 6850
F 0 "#PWR03" H 6600 6700 50  0001 C CNN
F 1 "+BATT" H 6600 6990 50  0000 C CNN
F 2 "" H 6600 6850 50  0000 C CNN
F 3 "" H 6600 6850 50  0000 C CNN
	1    6600 6850
	-1   0    0    1   
$EndComp
$Comp
L PWR_FLAG #FLG04
U 1 1 58A78C79
P 6600 6850
F 0 "#FLG04" H 6600 6945 50  0001 C CNN
F 1 "PWR_FLAG" H 6600 7030 50  0000 C CNN
F 2 "" H 6600 6850 50  0000 C CNN
F 3 "" H 6600 6850 50  0000 C CNN
	1    6600 6850
	1    0    0    -1  
$EndComp
$Sheet
S 4650 1650 2000 1750
U 59C5A6CD
F0 "OpenAVRc_Sticks" 60
F1 "OpenAVRc_Sticks.sch" 60
$EndSheet
$Sheet
S 2500 3750 2000 1750
U 59C5D60D
F0 "OpenAVRc_CoreConnects" 60
F1 "OpenAVRc_CoreConnects.sch" 60
$EndSheet
$Sheet
S 4650 3800 2000 1700
U 59C6AC14
F0 "OpenAVRc_HF_Telemetry" 60
F1 "OpenAVRc_HF_Telemetry.sch" 60
$EndSheet
$Sheet
S 6800 1650 1950 1750
U 59C560A1
F0 "OpenAVRc_I2C" 60
F1 "OpenAVRc_I2C.sch" 60
$EndSheet
$Sheet
S 6800 3750 2000 1750
U 59C59DB3
F0 "OpenAVRc_Audio_Haptic" 60
F1 "OpenAVRc_Audio_Haptic.sch" 60
$EndSheet
$Sheet
S 2500 1650 2000 1750
U 59C5272D
F0 "OpenAVRc_Power" 60
F1 "OpenAVRc_Power.sch" 60
$EndSheet
$EndSCHEMATC
