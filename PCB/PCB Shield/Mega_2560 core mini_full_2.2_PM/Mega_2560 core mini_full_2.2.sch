EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 7
Title "Shield Mega2560 core Full"
Date "2020-03-07"
Rev "2.2"
Comp "© Team OpenAVRc"
Comment1 "Reproduction et commercialisation interdite"
Comment2 "By Antho.breizh / pierrotm777"
Comment3 ""
Comment4 "Shield pour RC OpenAVRc"
$EndDescr
$Comp
L Mega_2560-core-mini_full_2.2-rescue:PWR_FLAG-power1 #FLG01
U 1 1 58A78C76
P 6600 7400
F 0 "#FLG01" H 6600 7495 50  0001 C CNN
F 1 "PWR_FLAG" H 6600 7580 50  0000 C CNN
F 2 "" H 6600 7400 50  0000 C CNN
F 3 "" H 6600 7400 50  0000 C CNN
	1    6600 7400
	1    0    0    -1  
$EndComp
$Comp
L Mega_2560-core-mini_full_2.2-rescue:+BATT-power1 #PWR02
U 1 1 58A78C78
P 6600 6850
F 0 "#PWR02" H 6600 6700 50  0001 C CNN
F 1 "+BATT" H 6600 6990 50  0000 C CNN
F 2 "" H 6600 6850 50  0000 C CNN
F 3 "" H 6600 6850 50  0000 C CNN
	1    6600 6850
	-1   0    0    1   
$EndComp
$Comp
L Mega_2560-core-mini_full_2.2-rescue:PWR_FLAG-power1 #FLG03
U 1 1 58A78C79
P 6600 6850
F 0 "#FLG03" H 6600 6945 50  0001 C CNN
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
$Comp
L power:GND #PWR0126
U 1 1 5C7039CD
P 6600 7400
F 0 "#PWR0126" H 6600 7150 50  0001 C CNN
F 1 "GND" H 6605 7227 50  0000 C CNN
F 2 "" H 6600 7400 50  0001 C CNN
F 3 "" H 6600 7400 50  0001 C CNN
	1    6600 7400
	1    0    0    -1  
$EndComp
$EndSCHEMATC
