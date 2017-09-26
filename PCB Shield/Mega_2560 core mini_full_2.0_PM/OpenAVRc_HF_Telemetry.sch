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
Sheet 4 7
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R7
U 1 1 59C6B41B
P 1600 3750
F 0 "R7" V 1680 3750 50  0000 C CNN
F 1 "470" V 1600 3750 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 1530 3750 50  0001 C CNN
F 3 "" H 1600 3750 50  0000 C CNN
	1    1600 3750
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR067
U 1 1 59C6B422
P 1850 4600
F 0 "#PWR067" H 1850 4350 50  0001 C CNN
F 1 "GND" H 1850 4450 50  0000 C CNN
F 2 "" H 1850 4600 50  0000 C CNN
F 3 "" H 1850 4600 50  0000 C CNN
	1    1850 4600
	1    0    0    -1  
$EndComp
$Comp
L +BATT #PWR068
U 1 1 59C6B429
P 1600 3500
F 0 "#PWR068" H 1600 3350 50  0001 C CNN
F 1 "+BATT" H 1600 3640 50  0000 C CNN
F 2 "" H 1600 3500 50  0000 C CNN
F 3 "" H 1600 3500 50  0000 C CNN
	1    1600 3500
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X08 P19
U 1 1 59C6B42F
P 5150 3600
F 0 "P19" H 5150 4050 50  0000 C CNN
F 1 "Module_TX" V 5250 3600 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x04_Pitch2.54mm" H 5150 3600 50  0001 C CNN
F 3 "" H 5150 3600 50  0000 C CNN
	1    5150 3600
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR069
U 1 1 59C6B436
P 5200 3900
F 0 "#PWR069" H 5200 3650 50  0001 C CNN
F 1 "GND" V 5200 3700 50  0000 C CNN
F 2 "" H 5200 3900 50  0000 C CNN
F 3 "" H 5200 3900 50  0000 C CNN
	1    5200 3900
	1    0    0    -1  
$EndComp
Text Label 5100 4450 1    60   ~ 0
Master_PPMout
$Comp
L VCC #PWR070
U 1 1 59C6B43D
P 5300 3900
F 0 "#PWR070" H 5300 3750 50  0001 C CNN
F 1 "VCC" V 5300 4100 50  0000 C CNN
F 2 "" H 5300 3900 50  0000 C CNN
F 3 "" H 5300 3900 50  0000 C CNN
	1    5300 3900
	-1   0    0    1   
$EndComp
Text Notes 3350 3200 2    60   ~ 0
Isolation Module TTL
$Comp
L +BATT #PWR071
U 1 1 59C6B445
P 4800 3900
F 0 "#PWR071" H 4800 3750 50  0001 C CNN
F 1 "+BATT" H 4800 4040 50  0000 C CNN
F 2 "" H 4800 3900 50  0000 C CNN
F 3 "" H 4800 3900 50  0000 C CNN
	1    4800 3900
	-1   0    0    1   
$EndComp
NoConn ~ 4800 3600
NoConn ~ 4900 3600
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R8
U 1 1 59C6B44D
P 9300 4200
F 0 "R8" V 9380 4200 50  0000 C CNN
F 1 "200" V 9300 4200 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 9230 4200 50  0001 C CNN
F 3 "" H 9300 4200 50  0000 C CNN
	1    9300 4200
	0    1    1    0   
$EndComp
Text Notes 7150 3250 2    60   ~ 0
PPM Management
$Comp
L MBR0520 D2
U 1 1 59C6B455
P 7350 4450
F 0 "D2" H 7350 4350 50  0000 C CNN
F 1 "MBR0520" H 7350 4550 50  0000 C CNN
F 2 "Diodes_SMD:D_SOD-123" H 7350 4275 50  0001 C CNN
F 3 "" H 7350 4450 50  0001 C CNN
	1    7350 4450
	1    0    0    -1  
$EndComp
Text Notes 2100 3350 0    60   Italic 0
( Option si utilisation de la télémètrie )
$Comp
L CONN_01X02 P18
U 1 1 59C6B45D
P 9850 4250
F 0 "P18" H 9850 4400 50  0000 C CNN
F 1 "Ecolage" V 10000 4250 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 9850 4250 50  0001 C CNN
F 3 "" H 9850 4250 50  0000 C CNN
	1    9850 4250
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR072
U 1 1 59C6B464
P 9650 4500
F 0 "#PWR072" H 9650 4250 50  0001 C CNN
F 1 "GND" H 9650 4350 50  0000 C CNN
F 2 "" H 9650 4500 50  0000 C CNN
F 3 "" H 9650 4500 50  0000 C CNN
	1    9650 4500
	1    0    0    -1  
$EndComp
$Comp
L TLP290-RESCUE-Mega_2560_core_mini_full_2.0 U3
U 1 1 59C6B46A
P 2250 4300
F 0 "U3" H 2050 4500 50  0000 L CNN
F 1 "TLP290" H 2250 4500 50  0000 L CNN
F 2 "anthoperso:SOP4" H 2050 4100 50  0000 L CIN
F 3 "" H 2275 4300 50  0000 L CNN
	1    2250 4300
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X08 P20
U 1 1 59C6B471
P 4000 4000
F 0 "P20" H 4000 4450 50  0000 C CNN
F 1 "TTL/RS232" V 4100 4000 50  0000 C CNN
F 2 "anthoperso:Con_RS232_TTL" H 4000 4000 50  0001 C CNN
F 3 "" H 4000 4000 50  0000 C CNN
	1    4000 4000
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR073
U 1 1 59C6B478
P 3750 4400
F 0 "#PWR073" H 3750 4150 50  0001 C CNN
F 1 "GND" H 3750 4250 50  0000 C CNN
F 2 "" H 3750 4400 50  0000 C CNN
F 3 "" H 3750 4400 50  0000 C CNN
	1    3750 4400
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR074
U 1 1 59C6B47E
P 3700 3600
F 0 "#PWR074" H 3700 3450 50  0001 C CNN
F 1 "VCC" H 3700 3750 50  0000 C CNN
F 2 "" H 3700 3600 50  0000 C CNN
F 3 "" H 3700 3600 50  0000 C CNN
	1    3700 3600
	1    0    0    -1  
$EndComp
$Comp
L TLP290-RESCUE-Mega_2560_core_mini_full_2.0 U2
U 1 1 59C6B484
P 2250 3800
F 0 "U2" H 2050 4000 50  0000 L CNN
F 1 "TLP290" H 2250 4000 50  0000 L CNN
F 2 "anthoperso:SOP4" H 2050 3600 50  0000 L CIN
F 3 "" H 2275 3800 50  0000 L CNN
	1    2250 3800
	1    0    0    -1  
$EndComp
Text Notes 4800 3400 0    60   ~ 0
Coupure Alim \n
Text Label 3200 3750 0    60   ~ 0
TX_module
Text Label 3200 3650 0    60   ~ 0
RX_module
Text Label 5500 4450 1    60   ~ 0
RX_module
Text Label 5400 4450 1    60   ~ 0
TX_module
$Comp
L 74LVC1G3157 U1
U 1 1 59C6B490
P 8100 3900
F 0 "U1" H 8100 4450 60  0000 C CNN
F 1 "74LVC1G3157GV" H 8100 3400 60  0000 C CNN
F 2 "SOT-457" H 8100 3900 60  0001 C CNN
F 3 "" H 8100 3900 60  0000 C CNN
	1    8100 3900
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR075
U 1 1 59C6B497
P 8800 3900
F 0 "#PWR075" H 8800 3750 50  0001 C CNN
F 1 "VCC" H 8800 4050 50  0000 C CNN
F 2 "" H 8800 3900 50  0000 C CNN
F 3 "" H 8800 3900 50  0000 C CNN
	1    8800 3900
	0    1    1    0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR076
U 1 1 59C6B49D
P 7350 3900
F 0 "#PWR076" H 7350 3650 50  0001 C CNN
F 1 "GND" H 7350 3750 50  0000 C CNN
F 2 "" H 7350 3900 50  0000 C CNN
F 3 "" H 7350 3900 50  0000 C CNN
	1    7350 3900
	0    1    1    0   
$EndComp
Text Notes 4700 3250 0    60   ~ 0
Module HF
Wire Wire Line
	9650 4200 9450 4200
Wire Wire Line
	9650 4300 9650 4500
Wire Notes Line
	1450 4850 4350 4850
Wire Wire Line
	5000 3800 4900 3800
Connection ~ 1850 4400
Wire Wire Line
	1950 4400 1850 4400
Wire Wire Line
	1850 3900 1850 4600
Wire Wire Line
	1950 3900 1850 3900
Wire Wire Line
	5100 3800 5100 4450
Wire Wire Line
	1600 3600 1600 3500
Wire Wire Line
	2600 3700 2550 3700
Wire Wire Line
	2550 4400 2600 4400
Wire Wire Line
	5500 3800 5500 4450
Wire Wire Line
	5400 3800 5400 4450
Wire Wire Line
	3750 3950 3800 3950
Wire Wire Line
	3100 4150 3800 4150
Wire Wire Line
	2550 4200 2750 4200
Wire Wire Line
	2750 4200 2900 4050
Wire Wire Line
	2900 4050 3800 4050
Connection ~ 3750 4350
Wire Wire Line
	3750 3950 3750 4400
Wire Wire Line
	3700 3600 3700 4250
Wire Wire Line
	3800 3850 3700 3850
Connection ~ 3700 3850
Wire Wire Line
	3100 4150 2850 3900
Wire Wire Line
	2850 3900 2550 3900
Wire Wire Line
	3800 3650 3200 3650
Wire Wire Line
	3800 3750 3200 3750
Wire Wire Line
	8600 3900 8800 3900
Wire Wire Line
	9150 4200 8600 4200
Wire Wire Line
	7600 3900 7350 3900
Wire Wire Line
	8600 3600 9150 3600
Wire Wire Line
	6800 3600 7600 3600
Wire Wire Line
	7050 4200 7600 4200
Wire Wire Line
	7050 4450 7200 4450
Wire Wire Line
	7550 4450 7550 4200
Connection ~ 7550 4200
Wire Notes Line
	6250 3100 10250 3100
Wire Wire Line
	1750 4200 1950 4200
Wire Wire Line
	1750 3700 1750 4200
Wire Wire Line
	1750 3700 1950 3700
Wire Wire Line
	1750 3950 1600 3950
Wire Wire Line
	1600 3950 1600 3900
Connection ~ 1750 3950
Wire Notes Line
	1450 3100 4350 3100
Wire Notes Line
	4350 3100 4350 4850
Wire Notes Line
	1450 4850 1450 3100
Wire Notes Line
	4800 3600 4800 3400
Wire Notes Line
	4800 3400 4900 3400
Wire Notes Line
	4900 3400 4900 3600
Wire Notes Line
	4550 4850 5700 4850
Wire Notes Line
	5700 4850 5700 3100
Wire Notes Line
	5700 3100 4550 3100
Wire Notes Line
	4550 3100 4550 4850
Wire Wire Line
	3700 4250 3800 4250
Wire Wire Line
	3750 4350 3800 4350
Wire Wire Line
	7550 4450 7500 4450
Wire Wire Line
	5300 3900 5300 3800
Wire Wire Line
	5200 3900 5200 3800
Wire Wire Line
	4800 3900 4800 3800
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C6
U 1 1 59C6C67F
P 5700 1950
F 0 "C6" H 5750 2050 50  0000 L CNN
F 1 "100nF" H 5750 1850 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 5738 1800 50  0001 C CNN
F 3 "" H 5700 1950 50  0000 C CNN
	1    5700 1950
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR077
U 1 1 59C6C686
P 5700 2150
F 0 "#PWR077" H 5700 1900 50  0001 C CNN
F 1 "GND" H 5700 2000 50  0000 C CNN
F 2 "" H 5700 2150 50  0000 C CNN
F 3 "" H 5700 2150 50  0000 C CNN
	1    5700 2150
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR078
U 1 1 59C6C68C
P 5400 1800
F 0 "#PWR078" H 5400 1550 50  0001 C CNN
F 1 "GND" H 5400 1650 50  0000 C CNN
F 2 "" H 5400 1800 50  0000 C CNN
F 3 "" H 5400 1800 50  0000 C CNN
	1    5400 1800
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR079
U 1 1 59C6C694
P 5500 1950
F 0 "#PWR079" H 5500 1800 50  0001 C CNN
F 1 "VCC" V 5500 2100 50  0000 C CNN
F 2 "" H 5500 1950 50  0000 C CNN
F 3 "" H 5500 1950 50  0000 C CNN
	1    5500 1950
	-1   0    0    1   
$EndComp
Text Notes 4600 1450 0    60   ~ 0
(Module Interne\nMulti Protocoles)
$Comp
L CONN_01X10 P7
U 1 1 59C6C69D
P 5050 1500
F 0 "P7" H 5050 2050 50  0000 C CNN
F 1 "SPIX" V 5050 900 50  0000 C CNN
F 2 "Pin_Header_Straight_2x05_Pitch2.54mm" H 5050 1500 50  0001 C CNN
F 3 "" H 5050 1500 50  0000 C CNN
	1    5050 1500
	0    -1   -1   0   
$EndComp
Text Label 5200 1950 1    60   ~ 0
P3_3V
Wire Wire Line
	5100 1700 5100 2300
Wire Wire Line
	5300 1700 5300 2300
Wire Wire Line
	5000 1700 5000 2300
Wire Wire Line
	5700 2150 5700 2100
Wire Wire Line
	4800 1700 4800 2300
Wire Wire Line
	5400 1800 5400 1700
Wire Wire Line
	4600 1700 4600 2300
Wire Wire Line
	5500 1700 5500 1950
Wire Wire Line
	4900 1700 4900 2300
Wire Wire Line
	4700 1700 4700 2300
Wire Wire Line
	5200 1700 5200 1950
Wire Wire Line
	5700 1800 5700 1750
Wire Wire Line
	5700 1750 5500 1750
Connection ~ 5500 1750
Wire Notes Line
	6250 4850 10250 4850
Wire Notes Line
	10250 4850 10250 3100
Wire Notes Line
	6250 4850 6250 3100
Text GLabel 2600 4400 2    60   Output ~ 0
Tele_RX
Text GLabel 2600 3700 2    60   Input ~ 0
Tele_TX
Text GLabel 4600 2300 3    60   Input ~ 0
CS_CC2500
Text GLabel 4700 2300 3    60   Input ~ 0
CS_NRF24L01
Text GLabel 4800 2300 3    60   Input ~ 0
CS_CYRF6936
Text GLabel 4900 2300 3    60   Input ~ 0
CS_A7105
Text GLabel 5000 2300 3    60   Input ~ 0
Xmitter_MISO
Text GLabel 5100 2300 3    60   Input ~ 0
Xmitter_SCK
Text GLabel 5300 2300 3    60   Output ~ 0
Xmitter_MOSI
Text GLabel 6800 3600 0    60   Output ~ 0
PPM_in
Text GLabel 7050 4200 0    60   Input ~ 0
PPM16_SIMout
Text GLabel 7050 4450 0    60   Input ~ 0
Master_PPMout
Text GLabel 9150 3600 2    60   Input ~ 0
Sim_control
$EndSCHEMATC
