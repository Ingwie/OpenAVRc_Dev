EESchema Schematic File Version 2
LIBS:Mega_2560 core mini_full_2.1-rescue
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
LIBS:Mega_2560 core mini_full_2.1
LIBS:Mega_2560 core mini_full_2.1-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 7
Title "Shield Mega2560 core Full"
Date "2018-02-14"
Rev "2.10"
Comp "© Team OpenAVRc"
Comment1 "Reproduction et commercialisation interdite"
Comment2 "By Antho.breizh / pierrotm777"
Comment3 ""
Comment4 "Shield pour RC OpenAVRc"
$EndDescr
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R7
U 1 1 59C6B41B
P 1600 4350
F 0 "R7" V 1680 4350 50  0000 C CNN
F 1 "470" V 1600 4350 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 1530 4350 50  0001 C CNN
F 3 "" H 1600 4350 50  0000 C CNN
	1    1600 4350
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR067
U 1 1 59C6B422
P 1850 5200
F 0 "#PWR067" H 1850 4950 50  0001 C CNN
F 1 "GND" H 1850 5050 50  0000 C CNN
F 2 "" H 1850 5200 50  0000 C CNN
F 3 "" H 1850 5200 50  0000 C CNN
	1    1850 5200
	1    0    0    -1  
$EndComp
$Comp
L +BATT #PWR068
U 1 1 59C6B429
P 1600 4100
F 0 "#PWR068" H 1600 3950 50  0001 C CNN
F 1 "+BATT" H 1600 4240 50  0000 C CNN
F 2 "" H 1600 4100 50  0000 C CNN
F 3 "" H 1600 4100 50  0000 C CNN
	1    1600 4100
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X08 P19
U 1 1 59C6B42F
P 3800 1700
F 0 "P19" H 3800 2150 50  0000 C CNN
F 1 "Module_TX" V 3900 1700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x04_Pitch2.54mm" H 3800 1700 50  0001 C CNN
F 3 "" H 3800 1700 50  0000 C CNN
	1    3800 1700
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR069
U 1 1 59C6B436
P 3850 2000
F 0 "#PWR069" H 3850 1750 50  0001 C CNN
F 1 "GND" V 3850 1800 50  0000 C CNN
F 2 "" H 3850 2000 50  0000 C CNN
F 3 "" H 3850 2000 50  0000 C CNN
	1    3850 2000
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR070
U 1 1 59C6B43D
P 3550 2000
F 0 "#PWR070" H 3550 1850 50  0001 C CNN
F 1 "VCC" V 3550 2200 50  0000 C CNN
F 2 "" H 3550 2000 50  0000 C CNN
F 3 "" H 3550 2000 50  0000 C CNN
	1    3550 2000
	-1   0    0    1   
$EndComp
Text Notes 3350 3800 2    60   ~ 0
Isolation Module TTL
$Comp
L +BATT #PWR071
U 1 1 59C6B445
P 3450 2000
F 0 "#PWR071" H 3450 1850 50  0001 C CNN
F 1 "+BATT" V 3450 2250 50  0000 C CNN
F 2 "" H 3450 2000 50  0000 C CNN
F 3 "" H 3450 2000 50  0000 C CNN
	1    3450 2000
	-1   0    0    1   
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R8
U 1 1 59C6B44D
P 9300 4800
F 0 "R8" V 9380 4800 50  0000 C CNN
F 1 "200" V 9300 4800 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 9230 4800 50  0001 C CNN
F 3 "" H 9300 4800 50  0000 C CNN
	1    9300 4800
	0    1    1    0   
$EndComp
Text Notes 7150 3850 2    60   ~ 0
PPM Management
Text Notes 2100 3950 0    60   Italic 0
( Option si utilisation de la télémètrie )
$Comp
L CONN_01X02 P18
U 1 1 59C6B45D
P 9850 4850
F 0 "P18" H 9850 5000 50  0000 C CNN
F 1 "Ecolage" V 10000 4850 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 9850 4850 50  0001 C CNN
F 3 "" H 9850 4850 50  0000 C CNN
	1    9850 4850
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR072
U 1 1 59C6B464
P 9650 5100
F 0 "#PWR072" H 9650 4850 50  0001 C CNN
F 1 "GND" H 9650 4950 50  0000 C CNN
F 2 "" H 9650 5100 50  0000 C CNN
F 3 "" H 9650 5100 50  0000 C CNN
	1    9650 5100
	1    0    0    -1  
$EndComp
$Comp
L TLP290-RESCUE-Mega_2560_core_mini_full_2.0 U3
U 1 1 59C6B46A
P 2250 4900
F 0 "U3" H 2050 5100 50  0000 L CNN
F 1 "TLP290" H 2250 5100 50  0000 L CNN
F 2 "anthoperso:SOP4" H 2050 4700 50  0000 L CIN
F 3 "" H 2275 4900 50  0000 L CNN
	1    2250 4900
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X08 P20
U 1 1 59C6B471
P 4000 4600
F 0 "P20" H 4000 5050 50  0000 C CNN
F 1 "TTL/RS232" V 4100 4600 50  0000 C CNN
F 2 "anthoperso:Con_RS232_TTL" H 4000 4600 50  0001 C CNN
F 3 "" H 4000 4600 50  0000 C CNN
	1    4000 4600
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR073
U 1 1 59C6B478
P 3750 5000
F 0 "#PWR073" H 3750 4750 50  0001 C CNN
F 1 "GND" H 3750 4850 50  0000 C CNN
F 2 "" H 3750 5000 50  0000 C CNN
F 3 "" H 3750 5000 50  0000 C CNN
	1    3750 5000
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR074
U 1 1 59C6B47E
P 3700 4200
F 0 "#PWR074" H 3700 4050 50  0001 C CNN
F 1 "VCC" H 3700 4350 50  0000 C CNN
F 2 "" H 3700 4200 50  0000 C CNN
F 3 "" H 3700 4200 50  0000 C CNN
	1    3700 4200
	1    0    0    -1  
$EndComp
$Comp
L TLP290-RESCUE-Mega_2560_core_mini_full_2.0 U2
U 1 1 59C6B484
P 2250 4400
F 0 "U2" H 2050 4600 50  0000 L CNN
F 1 "TLP290" H 2250 4600 50  0000 L CNN
F 2 "anthoperso:SOP4" H 2050 4200 50  0000 L CIN
F 3 "" H 2275 4400 50  0000 L CNN
	1    2250 4400
	1    0    0    -1  
$EndComp
Text Label 3200 4350 0    60   ~ 0
TX_module
Text Label 3200 4250 0    60   ~ 0
RX_module
Text Label 4150 3250 1    60   ~ 0
TX_module
Text Label 3950 3250 1    60   ~ 0
RX_module
$Comp
L 74LVC1G3157 U1
U 1 1 59C6B490
P 8100 4500
F 0 "U1" H 8100 5050 60  0000 C CNN
F 1 "74LVC1G3157GV" H 8100 4000 60  0000 C CNN
F 2 "SOT-457" H 8100 4500 60  0001 C CNN
F 3 "" H 8100 4500 60  0000 C CNN
	1    8100 4500
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR075
U 1 1 59C6B497
P 8800 4500
F 0 "#PWR075" H 8800 4350 50  0001 C CNN
F 1 "VCC" H 8800 4650 50  0000 C CNN
F 2 "" H 8800 4500 50  0000 C CNN
F 3 "" H 8800 4500 50  0000 C CNN
	1    8800 4500
	0    1    1    0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR076
U 1 1 59C6B49D
P 7350 4500
F 0 "#PWR076" H 7350 4250 50  0001 C CNN
F 1 "GND" H 7350 4350 50  0000 C CNN
F 2 "" H 7350 4500 50  0000 C CNN
F 3 "" H 7350 4500 50  0000 C CNN
	1    7350 4500
	0    1    1    0   
$EndComp
Text Notes 3350 1350 0    60   ~ 0
Module HF
Wire Wire Line
	9650 4800 9450 4800
Wire Wire Line
	9650 4900 9650 5100
Wire Notes Line
	1450 5450 4350 5450
Connection ~ 1850 5000
Wire Wire Line
	1950 5000 1850 5000
Wire Wire Line
	1850 4500 1850 5200
Wire Wire Line
	1950 4500 1850 4500
Wire Wire Line
	1600 4200 1600 4100
Wire Wire Line
	2600 4300 2550 4300
Wire Wire Line
	4150 1900 4150 3250
Wire Wire Line
	3950 1900 3950 3250
Wire Wire Line
	3750 4550 3800 4550
Wire Wire Line
	3100 4750 3800 4750
Wire Wire Line
	2550 4800 2750 4800
Wire Wire Line
	2750 4800 2900 4650
Wire Wire Line
	2900 4650 3800 4650
Connection ~ 3750 4950
Wire Wire Line
	3750 4550 3750 5000
Wire Wire Line
	3700 4200 3700 4850
Wire Wire Line
	3800 4450 3700 4450
Connection ~ 3700 4450
Wire Wire Line
	3100 4750 2850 4500
Wire Wire Line
	2850 4500 2550 4500
Wire Wire Line
	3800 4250 3200 4250
Wire Wire Line
	3800 4350 3200 4350
Wire Wire Line
	8600 4500 8800 4500
Wire Wire Line
	9150 4800 8600 4800
Wire Wire Line
	7600 4500 7350 4500
Wire Wire Line
	8600 4200 9150 4200
Wire Wire Line
	6800 4200 7600 4200
Wire Notes Line
	6250 3700 10250 3700
Wire Wire Line
	1750 4800 1950 4800
Wire Wire Line
	1750 4300 1750 4800
Wire Wire Line
	1750 4300 1950 4300
Wire Wire Line
	1750 4550 1600 4550
Wire Wire Line
	1600 4550 1600 4500
Connection ~ 1750 4550
Wire Notes Line
	1450 3700 4350 3700
Wire Notes Line
	4350 3700 4350 5450
Wire Notes Line
	1450 5450 1450 3700
Wire Notes Line
	4350 1200 3200 1200
Wire Wire Line
	3700 4850 3800 4850
Wire Wire Line
	3750 4950 3800 4950
Wire Wire Line
	3850 2000 3850 1900
Wire Wire Line
	3450 2000 3450 1900
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C6
U 1 1 59C6C67F
P 7550 2000
F 0 "C6" H 7600 2100 50  0000 L CNN
F 1 "100nF" H 7600 1900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 7588 1850 50  0001 C CNN
F 3 "" H 7550 2000 50  0000 C CNN
	1    7550 2000
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR077
U 1 1 59C6C686
P 7550 2200
F 0 "#PWR077" H 7550 1950 50  0001 C CNN
F 1 "GND" H 7550 2050 50  0000 C CNN
F 2 "" H 7550 2200 50  0000 C CNN
F 3 "" H 7550 2200 50  0000 C CNN
	1    7550 2200
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR078
U 1 1 59C6C68C
P 7250 1850
F 0 "#PWR078" H 7250 1600 50  0001 C CNN
F 1 "GND" H 7250 1700 50  0000 C CNN
F 2 "" H 7250 1850 50  0000 C CNN
F 3 "" H 7250 1850 50  0000 C CNN
	1    7250 1850
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR079
U 1 1 59C6C694
P 7350 2000
F 0 "#PWR079" H 7350 1850 50  0001 C CNN
F 1 "VCC" V 7350 2150 50  0000 C CNN
F 2 "" H 7350 2000 50  0000 C CNN
F 3 "" H 7350 2000 50  0000 C CNN
	1    7350 2000
	-1   0    0    1   
$EndComp
Text Notes 6450 1500 0    60   ~ 0
(Module HF Interne\nMulti Protocoles)
$Comp
L CONN_01X10 P7
U 1 1 59C6C69D
P 6900 1550
F 0 "P7" H 6900 2100 50  0000 C CNN
F 1 "SPIX" V 6900 950 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x05_Pitch2.54mm" H 6900 1550 50  0001 C CNN
F 3 "" H 6900 1550 50  0000 C CNN
	1    6900 1550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6950 1750 6950 2350
Wire Wire Line
	7150 1750 7150 2350
Wire Wire Line
	6850 1750 6850 2350
Wire Wire Line
	7550 2200 7550 2150
Wire Wire Line
	6650 1750 6650 2350
Wire Wire Line
	7250 1850 7250 1750
Wire Wire Line
	6450 1750 6450 2350
Wire Wire Line
	7350 1750 7350 2000
Wire Wire Line
	6750 1750 6750 2350
Wire Wire Line
	6550 1750 6550 2350
Wire Wire Line
	7050 1750 7050 1950
Wire Wire Line
	7550 1850 7550 1800
Wire Wire Line
	7550 1800 7350 1800
Connection ~ 7350 1800
Wire Notes Line
	6250 5450 10250 5450
Wire Notes Line
	10250 5450 10250 3700
Wire Notes Line
	6250 5450 6250 3700
Text GLabel 3100 5000 2    60   Output ~ 0
Tele_RX
Text GLabel 2600 4300 2    60   Input ~ 0
Tele_TX
Text GLabel 6450 2350 3    60   Input ~ 0
CS_CC2500
Text GLabel 6550 2350 3    60   Input ~ 0
CS_NRF24L01
Text GLabel 6650 2350 3    60   Input ~ 0
CS_CYRF6936
Text GLabel 6750 2350 3    60   Input ~ 0
CS_A7105
Text GLabel 6850 2350 3    60   Input ~ 0
Xmitter_MISO
Text GLabel 6950 2350 3    60   Input ~ 0
Xmitter_SCK
Text GLabel 7150 2350 3    60   Output ~ 0
Xmitter_MOSI
Text GLabel 6800 4200 0    60   Output ~ 0
PPM_in
Text GLabel 7050 4800 0    60   Input ~ 0
PPM16_SIMout
Text GLabel 9150 4200 2    60   Input ~ 0
Sim_control
$Comp
L +3,3V #PWR080
U 1 1 59CA7757
P 7050 1950
F 0 "#PWR080" H 7050 1910 30  0001 C CNN
F 1 "+3,3V" V 7050 2150 39  0000 C CNN
F 2 "" H 7050 1950 60  0001 C CNN
F 3 "" H 7050 1950 60  0001 C CNN
	1    7050 1950
	-1   0    0    1   
$EndComp
Wire Notes Line
	6250 1250 7850 1250
Wire Notes Line
	7850 1250 7850 3100
Wire Notes Line
	7850 3100 6250 3100
Wire Notes Line
	6250 3100 6250 1250
Wire Wire Line
	7050 4800 7600 4800
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R18
U 1 1 5A718C98
P 3650 2250
F 0 "R18" V 3750 2250 50  0000 C CNN
F 1 "200" V 3650 2250 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3580 2250 50  0001 C CNN
F 3 "" H 3650 2250 50  0000 C CNN
	1    3650 2250
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R19
U 1 1 5A757DEE
P 2800 5000
F 0 "R19" V 2880 5000 50  0000 C CNN
F 1 "220" V 2800 5000 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 2730 5000 50  0001 C CNN
F 3 "" H 2800 5000 50  0000 C CNN
	1    2800 5000
	0    1    1    0   
$EndComp
Wire Wire Line
	3100 5000 2950 5000
Wire Wire Line
	2650 5000 2550 5000
Text GLabel 3650 2500 3    60   Input ~ 0
Master_PPMout
Wire Wire Line
	3650 2500 3650 2400
Wire Notes Line
	4350 3300 3200 3300
Wire Notes Line
	4350 3300 4350 1200
Wire Notes Line
	3200 3300 3200 1200
Wire Wire Line
	3650 2100 3650 1900
Connection ~ 3850 1950
Wire Wire Line
	3550 2000 3550 1900
Wire Wire Line
	3750 1900 3750 1950
Wire Wire Line
	3750 1950 3850 1950
$EndSCHEMATC
