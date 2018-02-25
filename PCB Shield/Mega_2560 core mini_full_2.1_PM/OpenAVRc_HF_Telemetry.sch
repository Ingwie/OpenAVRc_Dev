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
Date "2018-02-25"
Rev "2.10"
Comp "© Team OpenAVRc"
Comment1 "Reproduction et commercialisation interdite"
Comment2 "By Antho.breizh / pierrotm777"
Comment3 ""
Comment4 "Shield pour RC OpenAVRc"
$EndDescr
$Comp
L CONN_01X08 P19
U 1 1 59C6B42F
P 3850 1300
F 0 "P19" H 3850 1750 50  0000 C CNN
F 1 "Module_TX" V 3950 1300 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x04_Pitch2.54mm" H 3850 1300 50  0001 C CNN
F 3 "" H 3850 1300 50  0000 C CNN
	1    3850 1300
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR067
U 1 1 59C6B436
P 4000 1600
F 0 "#PWR067" H 4000 1350 50  0001 C CNN
F 1 "GND" V 4000 1400 50  0000 C CNN
F 2 "" H 4000 1600 50  0000 C CNN
F 3 "" H 4000 1600 50  0000 C CNN
	1    4000 1600
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR068
U 1 1 59C6B43D
P 3500 1600
F 0 "#PWR068" H 3500 1450 50  0001 C CNN
F 1 "VCC" V 3500 1800 50  0000 C CNN
F 2 "" H 3500 1600 50  0000 C CNN
F 3 "" H 3500 1600 50  0000 C CNN
	1    3500 1600
	-1   0    0    1   
$EndComp
Text Notes 3850 3600 2    60   ~ 0
Isolation Module TTL
$Comp
L +BATT #PWR069
U 1 1 59C6B445
P 3600 1600
F 0 "#PWR069" H 3600 1450 50  0001 C CNN
F 1 "+BATT" V 3600 1850 50  0000 C CNN
F 2 "" H 3600 1600 50  0000 C CNN
F 3 "" H 3600 1600 50  0000 C CNN
	1    3600 1600
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
Text Notes 2600 3750 0    60   Italic 0
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
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR070
U 1 1 59C6B464
P 9650 5100
F 0 "#PWR070" H 9650 4850 50  0001 C CNN
F 1 "GND" H 9650 4950 50  0000 C CNN
F 2 "" H 9650 5100 50  0000 C CNN
F 3 "" H 9650 5100 50  0000 C CNN
	1    9650 5100
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X08 P20
U 1 1 59C6B471
P 900 4950
F 0 "P20" H 900 5400 50  0000 C CNN
F 1 "TTL/RS232" V 1000 4950 50  0000 C CNN
F 2 "anthoperso:Con_RS232_TTL" H 900 4950 50  0001 C CNN
F 3 "" H 900 4950 50  0000 C CNN
	1    900  4950
	-1   0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR071
U 1 1 59C6B478
P 1150 5350
F 0 "#PWR071" H 1150 5100 50  0001 C CNN
F 1 "GND" H 1150 5200 50  0000 C CNN
F 2 "" H 1150 5350 50  0000 C CNN
F 3 "" H 1150 5350 50  0000 C CNN
	1    1150 5350
	-1   0    0    -1  
$EndComp
$Comp
L VCC #PWR072
U 1 1 59C6B47E
P 1200 4550
F 0 "#PWR072" H 1200 4400 50  0001 C CNN
F 1 "VCC" H 1200 4700 50  0000 C CNN
F 2 "" H 1200 4550 50  0000 C CNN
F 3 "" H 1200 4550 50  0000 C CNN
	1    1200 4550
	-1   0    0    -1  
$EndComp
Text Label 1700 4700 2    60   ~ 0
TX_module
Text Label 1700 4600 2    60   ~ 0
RX_module
Text Label 4100 2850 1    60   ~ 0
TX_module
Text Label 3900 2850 1    60   ~ 0
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
L VCC #PWR073
U 1 1 59C6B497
P 8800 4500
F 0 "#PWR073" H 8800 4350 50  0001 C CNN
F 1 "VCC" H 8800 4650 50  0000 C CNN
F 2 "" H 8800 4500 50  0000 C CNN
F 3 "" H 8800 4500 50  0000 C CNN
	1    8800 4500
	0    1    1    0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR074
U 1 1 59C6B49D
P 7350 4500
F 0 "#PWR074" H 7350 4250 50  0001 C CNN
F 1 "GND" H 7350 4350 50  0000 C CNN
F 2 "" H 7350 4500 50  0000 C CNN
F 3 "" H 7350 4500 50  0000 C CNN
	1    7350 4500
	0    1    1    0   
$EndComp
Text Notes 3400 950  0    60   ~ 0
Module HF
Wire Wire Line
	9650 4800 9450 4800
Wire Wire Line
	9650 4900 9650 5100
Wire Notes Line
	1950 6400 4850 6400
Wire Wire Line
	3900 4700 4700 4700
Wire Wire Line
	4100 1500 4100 2850
Wire Wire Line
	3900 1500 3900 2850
Wire Wire Line
	1150 4900 1100 4900
Connection ~ 1150 5300
Wire Wire Line
	1150 4900 1150 5350
Wire Wire Line
	1200 4550 1200 5200
Wire Wire Line
	1100 4800 1200 4800
Connection ~ 1200 4800
Wire Wire Line
	1100 4600 1700 4600
Wire Wire Line
	1100 4700 1700 4700
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
Wire Notes Line
	1950 3500 4850 3500
Wire Notes Line
	4400 800  3250 800 
Wire Wire Line
	1100 5200 1450 5200
Wire Wire Line
	1150 5300 1100 5300
Wire Wire Line
	3600 1600 3600 1500
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C6
U 1 1 59C6C67F
P 7600 1600
F 0 "C6" H 7650 1700 50  0000 L CNN
F 1 "100nF" H 7650 1500 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 7638 1450 50  0001 C CNN
F 3 "" H 7600 1600 50  0000 C CNN
	1    7600 1600
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR075
U 1 1 59C6C686
P 7600 1800
F 0 "#PWR075" H 7600 1550 50  0001 C CNN
F 1 "GND" H 7600 1650 50  0000 C CNN
F 2 "" H 7600 1800 50  0000 C CNN
F 3 "" H 7600 1800 50  0000 C CNN
	1    7600 1800
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR076
U 1 1 59C6C68C
P 7300 1450
F 0 "#PWR076" H 7300 1200 50  0001 C CNN
F 1 "GND" H 7300 1300 50  0000 C CNN
F 2 "" H 7300 1450 50  0000 C CNN
F 3 "" H 7300 1450 50  0000 C CNN
	1    7300 1450
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR077
U 1 1 59C6C694
P 7400 1600
F 0 "#PWR077" H 7400 1450 50  0001 C CNN
F 1 "VCC" V 7400 1750 50  0000 C CNN
F 2 "" H 7400 1600 50  0000 C CNN
F 3 "" H 7400 1600 50  0000 C CNN
	1    7400 1600
	-1   0    0    1   
$EndComp
Text Notes 6500 1100 0    60   ~ 0
(Module HF Interne\nMulti Protocoles)
$Comp
L CONN_01X10 P7
U 1 1 59C6C69D
P 6950 1150
F 0 "P7" H 6950 1700 50  0000 C CNN
F 1 "SPIX" V 6950 550 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x05_Pitch2.54mm" H 6950 1150 50  0001 C CNN
F 3 "" H 6950 1150 50  0000 C CNN
	1    6950 1150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7000 1350 7000 1950
Wire Wire Line
	7200 1350 7200 1950
Wire Wire Line
	6900 1350 6900 1950
Wire Wire Line
	7600 1800 7600 1750
Wire Wire Line
	6700 1350 6700 1950
Wire Wire Line
	7300 1450 7300 1350
Wire Wire Line
	6500 1350 6500 1950
Wire Wire Line
	7400 1350 7400 1600
Wire Wire Line
	6800 1350 6800 1950
Wire Wire Line
	6600 1350 6600 1950
Wire Wire Line
	7100 1350 7100 1550
Wire Wire Line
	7600 1450 7600 1400
Wire Wire Line
	7600 1400 7400 1400
Connection ~ 7400 1400
Wire Notes Line
	6250 5450 10250 5450
Wire Notes Line
	10250 5450 10250 3700
Wire Notes Line
	6250 5450 6250 3700
Text GLabel 4750 6000 2    60   Output ~ 0
Tele_RX
Text GLabel 4700 4700 2    60   Input ~ 0
Tele_TX
Text GLabel 6500 1950 3    60   Input ~ 0
CS_CC2500
Text GLabel 6600 1950 3    60   Input ~ 0
CS_NRF24L01
Text GLabel 6700 1950 3    60   Input ~ 0
CS_CYRF6936
Text GLabel 6800 1950 3    60   Input ~ 0
CS_A7105
Text GLabel 6900 1950 3    60   Input ~ 0
Xmitter_MISO
Text GLabel 7000 1950 3    60   Input ~ 0
Xmitter_SCK
Text GLabel 7200 1950 3    60   Output ~ 0
Xmitter_MOSI
Text GLabel 6800 4200 0    60   Output ~ 0
PPM_in
Text GLabel 7050 4800 0    60   Input ~ 0
PPM16_SIMout
Text GLabel 9150 4200 2    60   Input ~ 0
Sim_control
$Comp
L +3,3V #PWR078
U 1 1 59CA7757
P 7100 1550
F 0 "#PWR078" H 7100 1510 30  0001 C CNN
F 1 "+3,3V" V 7100 1750 39  0000 C CNN
F 2 "" H 7100 1550 60  0001 C CNN
F 3 "" H 7100 1550 60  0001 C CNN
	1    7100 1550
	-1   0    0    1   
$EndComp
Wire Notes Line
	6300 850  7900 850 
Wire Notes Line
	7900 850  7900 2700
Wire Notes Line
	7900 2700 6300 2700
Wire Notes Line
	6300 2700 6300 850 
Wire Wire Line
	7050 4800 7600 4800
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R18
U 1 1 5A718C98
P 3800 1850
F 0 "R18" V 3700 1850 50  0000 C CNN
F 1 "200" V 3800 1850 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3730 1850 50  0001 C CNN
F 3 "" H 3800 1850 50  0000 C CNN
	1    3800 1850
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R19
U 1 1 5A757DEE
P 4450 6000
F 0 "R19" V 4530 6000 50  0000 C CNN
F 1 "220" V 4450 6000 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 4380 6000 50  0001 C CNN
F 3 "" H 4450 6000 50  0000 C CNN
	1    4450 6000
	0    1    1    0   
$EndComp
Wire Wire Line
	4750 6000 4600 6000
Wire Wire Line
	3900 6000 4300 6000
Text GLabel 3650 2100 3    60   Input ~ 0
Master_PPMout
Wire Wire Line
	3650 2100 3800 2000
Wire Notes Line
	4400 2900 3250 2900
Wire Notes Line
	4400 2900 4400 800 
Wire Notes Line
	3250 2900 3250 800 
Wire Wire Line
	3800 1700 3800 1500
Wire Wire Line
	3500 1600 3500 1500
Wire Wire Line
	3700 1550 4000 1550
Wire Wire Line
	4000 1500 4000 1600
Connection ~ 4000 1550
Wire Wire Line
	3700 1550 3700 1500
$Comp
L 74LVC1G3157 U2
U 1 1 5A926F9C
P 3400 4400
F 0 "U2" H 3400 4950 60  0000 C CNN
F 1 "74LVC1G3157GV" H 3400 3900 60  0000 C CNN
F 2 "SOT-457" H 3400 4400 60  0001 C CNN
F 3 "" H 3400 4400 60  0000 C CNN
	1    3400 4400
	1    0    0    -1  
$EndComp
$Comp
L 74LVC1G3157 U3
U 1 1 5A927419
P 3400 5700
F 0 "U3" H 3400 6250 60  0000 C CNN
F 1 "74LVC1G3157GV" H 3400 5200 60  0000 C CNN
F 2 "SOT-457" H 3400 5700 60  0001 C CNN
F 3 "" H 3400 5700 60  0000 C CNN
	1    3400 5700
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR079
U 1 1 5A92799A
P 4100 4400
F 0 "#PWR079" H 4100 4250 50  0001 C CNN
F 1 "VCC" H 4100 4550 50  0000 C CNN
F 2 "" H 4100 4400 50  0000 C CNN
F 3 "" H 4100 4400 50  0000 C CNN
	1    4100 4400
	0    1    1    0   
$EndComp
Wire Wire Line
	3900 4400 4100 4400
$Comp
L VCC #PWR080
U 1 1 5A9279BB
P 4100 5700
F 0 "#PWR080" H 4100 5550 50  0001 C CNN
F 1 "VCC" H 4100 5850 50  0000 C CNN
F 2 "" H 4100 5700 50  0000 C CNN
F 3 "" H 4100 5700 50  0000 C CNN
	1    4100 5700
	0    1    1    0   
$EndComp
Wire Wire Line
	3900 5700 4100 5700
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR081
U 1 1 5A9279DC
P 2650 4400
F 0 "#PWR081" H 2650 4150 50  0001 C CNN
F 1 "GND" H 2650 4250 50  0000 C CNN
F 2 "" H 2650 4400 50  0000 C CNN
F 3 "" H 2650 4400 50  0000 C CNN
	1    2650 4400
	0    1    1    0   
$EndComp
Wire Wire Line
	2900 4400 2650 4400
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR082
U 1 1 5A9279FD
P 2750 5700
F 0 "#PWR082" H 2750 5450 50  0001 C CNN
F 1 "GND" H 2750 5550 50  0000 C CNN
F 2 "" H 2750 5700 50  0000 C CNN
F 3 "" H 2750 5700 50  0000 C CNN
	1    2750 5700
	0    1    1    0   
$EndComp
Wire Wire Line
	3000 5700 2750 5700
$Comp
L CONN_01X05 P3
U 1 1 5A927E08
P 2100 5000
F 0 "P3" H 2100 5300 50  0000 C CNN
F 1 "FTDI" V 2200 5000 50  0000 C CNN
F 2 "" H 2100 5000 50  0000 C CNN
F 3 "" H 2100 5000 50  0000 C CNN
	1    2100 5000
	-1   0    0    -1  
$EndComp
$Comp
L VCC #PWR083
U 1 1 5A92837D
P 2350 4900
F 0 "#PWR083" H 2350 4750 50  0001 C CNN
F 1 "VCC" H 2350 5050 50  0000 C CNN
F 2 "" H 2350 4900 50  0000 C CNN
F 3 "" H 2350 4900 50  0000 C CNN
	1    2350 4900
	0    1    1    0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR084
U 1 1 5A9283AC
P 2350 5200
F 0 "#PWR084" H 2350 4950 50  0001 C CNN
F 1 "GND" H 2350 5050 50  0000 C CNN
F 2 "" H 2350 5200 50  0000 C CNN
F 3 "" H 2350 5200 50  0000 C CNN
	1    2350 5200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2350 5200 2300 5200
Wire Wire Line
	2900 4700 2550 4700
Wire Wire Line
	2550 5100 2550 6000
Wire Wire Line
	2550 5100 2300 5100
Wire Wire Line
	2300 5000 2650 5000
Text GLabel 2650 5000 2    60   Input ~ 0
lsp_rst
Wire Wire Line
	2550 6000 2900 6000
Wire Wire Line
	4050 5400 3900 5400
Wire Wire Line
	4050 4100 4050 5400
Wire Wire Line
	4050 4100 3900 4100
Wire Wire Line
	4050 5000 4550 5000
Connection ~ 4050 5000
Text GLabel 4550 5000 2    60   Input ~ 0
Hold_Power
Wire Wire Line
	1800 5400 2900 5400
Wire Wire Line
	1800 4100 2900 4100
Wire Wire Line
	1100 5000 1800 5000
Wire Wire Line
	1800 5000 1800 4100
Wire Wire Line
	1800 5400 1800 5100
Wire Wire Line
	1800 5100 1100 5100
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C2
U 1 1 5A92C79D
P 1450 5400
F 0 "C2" H 1500 5500 50  0000 L CNN
F 1 "100nF" H 1500 5300 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 1488 5250 50  0001 C CNN
F 3 "" H 1450 5400 50  0000 C CNN
	1    1450 5400
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR085
U 1 1 5A92C7A3
P 1450 5600
F 0 "#PWR085" H 1450 5350 50  0001 C CNN
F 1 "GND" H 1450 5450 50  0000 C CNN
F 2 "" H 1450 5600 50  0000 C CNN
F 3 "" H 1450 5600 50  0000 C CNN
	1    1450 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 5600 1450 5550
Wire Wire Line
	1450 5200 1450 5250
Connection ~ 1200 5200
Wire Wire Line
	2550 4700 2550 4800
Wire Wire Line
	2550 4800 2300 4800
Wire Wire Line
	2350 4900 2300 4900
$EndSCHEMATC
