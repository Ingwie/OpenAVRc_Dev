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
Sheet 7 7
Title "Shield Mega2560 core Full"
Date "2018-02-15"
Rev "2.10"
Comp "© Team OpenAVRc"
Comment1 "Reproduction et commercialisation interdite"
Comment2 "By Antho.breizh / pierrotm777"
Comment3 ""
Comment4 "Shield pour RC OpenAVRc"
$EndDescr
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C31
U 1 1 59C53131
P 7500 3200
F 0 "C31" H 7525 3300 50  0000 L CNN
F 1 "100nF" V 7600 2900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 7538 3050 50  0001 C CNN
F 3 "" H 7500 3200 50  0000 C CNN
	1    7500 3200
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R16
U 1 1 59C53138
P 4250 4150
F 0 "R16" V 4330 4150 50  0000 C CNN
F 1 "2.7K" V 4250 4150 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 4180 4150 50  0001 C CNN
F 3 "" H 4250 4150 50  0000 C CNN
	1    4250 4150
	-1   0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0110
U 1 1 59C5313F
P 8200 4550
F 0 "#PWR0110" H 8200 4300 50  0001 C CNN
F 1 "GND" H 8200 4350 50  0000 C CNN
F 2 "" H 8200 4550 50  0000 C CNN
F 3 "" H 8200 4550 50  0000 C CNN
	1    8200 4550
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P26
U 1 1 59C53145
P 8300 3350
F 0 "P26" H 8300 3550 50  0000 C CNN
F 1 "LCD_Alim" H 8300 3150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 8300 3350 50  0001 C CNN
F 3 "" H 8300 3350 50  0000 C CNN
	1    8300 3350
	-1   0    0    -1  
$EndComp
Text Notes 8750 3800 0    60   ~ 0
+3.3V\n
Text Notes 8500 2900 0    60   ~ 0
+5V
$Comp
L VCC #PWR0111
U 1 1 59C5314F
P 8850 3000
F 0 "#PWR0111" H 8850 2850 50  0001 C CNN
F 1 "VCC" V 8850 3200 50  0000 C CNN
F 2 "" H 8850 3000 50  0000 C CNN
F 3 "" H 8850 3000 50  0000 C CNN
	1    8850 3000
	0    1    1    0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C38
U 1 1 59C53155
P 8700 4300
F 0 "C38" H 8550 4400 50  0000 L CNN
F 1 "100nF" H 8450 4200 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 8738 4150 50  0001 C CNN
F 3 "" H 8700 4300 50  0000 C CNN
	1    8700 4300
	1    0    0    -1  
$EndComp
Text Notes 2450 2350 0    60   ~ 0
Power Management
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0112
U 1 1 59C5315D
P 6700 3850
F 0 "#PWR0112" H 6700 3600 50  0001 C CNN
F 1 "GND" H 6700 3700 50  0000 C CNN
F 2 "" H 6700 3850 50  0000 C CNN
F 3 "" H 6700 3850 50  0000 C CNN
	1    6700 3850
	-1   0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0113
U 1 1 59C53163
P 4250 4350
F 0 "#PWR0113" H 4250 4100 50  0001 C CNN
F 1 "GND" H 4250 4200 50  0000 C CNN
F 2 "" H 4250 4350 50  0000 C CNN
F 3 "" H 4250 4350 50  0000 C CNN
	1    4250 4350
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P21
U 1 1 59C53169
P 3600 2800
F 0 "P21" H 3600 2950 50  0000 C CNN
F 1 "Sw_power" V 3300 2800 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 3600 2800 50  0001 C CNN
F 3 "" H 3600 2800 50  0000 C CNN
	1    3600 2800
	0    -1   -1   0   
$EndComp
NoConn ~ 8300 3350
NoConn ~ 8300 3250
NoConn ~ 8300 3450
$Comp
L LD1117S33TR U5
U 1 1 59C53173
P 8200 4050
F 0 "U5" H 8000 3800 50  0000 C CNN
F 1 "LD1117S33TR" H 8200 4250 50  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-223" H 8200 4150 50  0001 C CNN
F 3 "" H 8200 4050 50  0000 C CNN
	1    8200 4050
	1    0    0    -1  
$EndComp
Text Notes 7450 4950 0    60   Italic 0
Option si utilisation du 3.3V dans le montage
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R17
U 1 1 59C53182
P 4250 3600
F 0 "R17" V 4330 3600 50  0000 C CNN
F 1 "5.1K" V 4250 3600 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 4180 3600 50  0001 C CNN
F 3 "" H 4250 3600 50  0000 C CNN
	1    4250 3600
	1    0    0    1   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0114
U 1 1 59C53189
P 7500 3400
F 0 "#PWR0114" H 7500 3150 50  0001 C CNN
F 1 "GND" H 7500 3250 50  0000 C CNN
F 2 "" H 7500 3400 50  0000 C CNN
F 3 "" H 7500 3400 50  0000 C CNN
	1    7500 3400
	-1   0    0    -1  
$EndComp
$Comp
L CP1-RESCUE-Mega_2560_core_mini_full_2.0 C28
U 1 1 59C5318F
P 4700 3200
F 0 "C28" H 4725 3300 50  0000 L CNN
F 1 "100µF" V 4800 2900 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_6.3x5.3" H 4700 3200 50  0001 C CNN
F 3 "" H 4700 3200 50  0000 C CNN
	1    4700 3200
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0115
U 1 1 59C53196
P 4700 3400
F 0 "#PWR0115" H 4700 3150 50  0001 C CNN
F 1 "GND" H 4700 3250 50  0000 C CNN
F 2 "" H 4700 3400 50  0000 C CNN
F 3 "" H 4700 3400 50  0000 C CNN
	1    4700 3400
	-1   0    0    -1  
$EndComp
$Comp
L MBR0520 D6
U 1 1 59C5319C
P 3950 3000
F 0 "D6" H 3950 3100 50  0000 C CNN
F 1 "MBR0520" H 3950 2900 50  0000 C CNN
F 2 "Diodes_SMD:D_SOD-123" H 3950 2825 50  0001 C CNN
F 3 "" H 3950 3000 50  0001 C CNN
	1    3950 3000
	-1   0    0    1   
$EndComp
NoConn ~ 3650 3000
NoConn ~ 3550 3000
$Comp
L CP1-RESCUE-Mega_2560_core_mini_full_2.0 C7
U 1 1 59C531A5
P 7850 3200
F 0 "C7" H 7875 3300 50  0000 L CNN
F 1 "100µF" V 7750 2900 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_6.3x5.3" H 7850 3200 50  0001 C CNN
F 3 "" H 7850 3200 50  0000 C CNN
	1    7850 3200
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0116
U 1 1 59C531AC
P 7850 3450
F 0 "#PWR0116" H 7850 3200 50  0001 C CNN
F 1 "GND" H 7850 3300 50  0000 C CNN
F 2 "" H 7850 3450 50  0000 C CNN
F 3 "" H 7850 3450 50  0000 C CNN
	1    7850 3450
	-1   0    0    -1  
$EndComp
$Comp
L CP1-RESCUE-Mega_2560_core_mini_full_2.0 C8
U 1 1 59C531B2
P 8950 4300
F 0 "C8" H 8975 4400 50  0000 L CNN
F 1 "47µF" H 8975 4200 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_6.3x5.3" H 8950 4300 50  0001 C CNN
F 3 "" H 8950 4300 50  0000 C CNN
	1    8950 4300
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0117
U 1 1 59C531B9
P 8950 4550
F 0 "#PWR0117" H 8950 4300 50  0001 C CNN
F 1 "GND" H 8950 4350 50  0000 C CNN
F 2 "" H 8950 4550 50  0000 C CNN
F 3 "" H 8950 4550 50  0000 C CNN
	1    8950 4550
	1    0    0    -1  
$EndComp
Text Notes 3250 2700 0    60   ~ 0
Power Switch
$Comp
L CONN_01X08 P30
U 1 1 59C531C1
P 2600 4150
F 0 "P30" H 2600 4600 50  0000 C CNN
F 1 "MiniDCDC3A" V 2700 4150 50  0000 C CNN
F 2 "MiniDCDC3A" H 2600 4150 50  0001 C CNN
F 3 "" H 2600 4150 50  0000 C CNN
	1    2600 4150
	-1   0    0    1   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0118
U 1 1 59C531C8
P 2850 4550
F 0 "#PWR0118" H 2850 4300 50  0001 C CNN
F 1 "GND" H 2850 4400 50  0000 C CNN
F 2 "" H 2850 4550 50  0000 C CNN
F 3 "" H 2850 4550 50  0000 C CNN
	1    2850 4550
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR0119
U 1 1 59C531CE
P 3050 3900
F 0 "#PWR0119" H 3050 3750 50  0001 C CNN
F 1 "VCC" H 3050 4000 50  0000 C CNN
F 2 "" H 3050 3900 50  0000 C CNN
F 3 "" H 3050 3900 50  0000 C CNN
	1    3050 3900
	1    0    0    -1  
$EndComp
Text Label 2950 3000 0    60   ~ 0
LIPO+
Text Notes 3000 4500 0    60   ~ 0
DCDC_IN-
$Comp
L CONN_01X02 P17
U 1 1 59C531D8
P 2600 3050
F 0 "P17" H 2600 3200 50  0000 C CNN
F 1 "BATT" V 2700 3050 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 2600 3050 50  0001 C CNN
F 3 "" H 2600 3050 50  0000 C CNN
	1    2600 3050
	-1   0    0    1   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0120
U 1 1 59C531DF
P 2900 3200
F 0 "#PWR0120" H 2900 2950 50  0001 C CNN
F 1 "GND" H 2900 3050 50  0000 C CNN
F 2 "" H 2900 3200 50  0000 C CNN
F 3 "" H 2900 3200 50  0000 C CNN
	1    2900 3200
	1    0    0    -1  
$EndComp
Text Notes 2400 4950 0    60   Italic 0
Option module DCDC 3A
$Comp
L +BATT #PWR0121
U 1 1 59C53200
P 4250 2800
F 0 "#PWR0121" H 4250 2650 50  0001 C CNN
F 1 "+BATT" H 4250 2940 50  0000 C CNN
F 2 "" H 4250 2800 50  0000 C CNN
F 3 "" H 4250 2800 50  0000 C CNN
	1    4250 2800
	1    0    0    -1  
$EndComp
Text GLabel 4150 3900 0    60   Output ~ 0
U_batt
Text GLabel 8900 3350 2    60   Output ~ 0
LCD_VCC
$Comp
L +3,3V #PWR0122
U 1 1 59CA5551
P 9000 4000
F 0 "#PWR0122" H 9000 3960 30  0001 C CNN
F 1 "+3,3V" V 9000 4200 39  0000 C CNN
F 2 "" H 9000 4000 60  0001 C CNN
F 3 "" H 9000 4000 60  0001 C CNN
	1    9000 4000
	0    1    1    0   
$EndComp
$Comp
L PM5033 U4
U 1 1 59E1D515
P 6700 3150
F 0 "U4" H 6900 2850 60  0000 C CNN
F 1 "PM5033" H 6700 3450 60  0000 C CNN
F 2 "PM5033_SIL5" H 6700 3150 60  0001 C CNN
F 3 "" H 6700 3150 60  0000 C CNN
	1    6700 3150
	1    0    0    -1  
$EndComp
$Comp
L +3,3V #PWR0123
U 1 1 59FED94A
P 7250 3300
F 0 "#PWR0123" H 7250 3260 30  0001 C CNN
F 1 "+3,3V" V 7300 3400 39  0000 C CNN
F 2 "" H 7250 3300 60  0001 C CNN
F 3 "" H 7250 3300 60  0001 C CNN
	1    7250 3300
	0    1    1    0   
$EndComp
Text GLabel 5600 3700 0    60   Output ~ 0
Hold_Power
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R43
U 1 1 5A086B2F
P 6100 3950
F 0 "R43" V 6180 3950 50  0000 C CNN
F 1 "10K" V 6100 3950 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 6030 3950 50  0001 C CNN
F 3 "" H 6100 3950 50  0000 C CNN
	1    6100 3950
	-1   0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R41
U 1 1 5A086C25
P 5400 3300
F 0 "R41" V 5480 3300 50  0000 C CNN
F 1 "33K" V 5400 3300 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 5330 3300 50  0001 C CNN
F 3 "" H 5400 3300 50  0000 C CNN
	1    5400 3300
	0    -1   1    0   
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R42
U 1 1 5A087144
P 5850 3700
F 0 "R42" V 5930 3700 50  0000 C CNN
F 1 "2.2K" V 5850 3700 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 5780 3700 50  0001 C CNN
F 3 "" H 5850 3700 50  0000 C CNN
	1    5850 3700
	0    1    -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0124
U 1 1 5A0873AD
P 6100 4200
F 0 "#PWR0124" H 6100 3950 50  0001 C CNN
F 1 "GND" H 6100 4050 50  0000 C CNN
F 2 "" H 6100 4200 50  0000 C CNN
F 3 "" H 6100 4200 50  0000 C CNN
	1    6100 4200
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P29
U 1 1 5A087968
P 5800 3100
F 0 "P29" H 5800 3250 50  0000 C CNN
F 1 "Sw_On" V 5500 3100 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 5800 3100 50  0001 C CNN
F 3 "" H 5800 3100 50  0000 C CNN
	1    5800 3100
	0    -1   -1   0   
$EndComp
Connection ~ 8200 4450
Wire Wire Line
	8200 4450 8700 4450
Wire Notes Line
	7600 3800 9400 3800
Wire Wire Line
	7200 3000 8850 3000
Wire Notes Line
	7600 4800 9400 4800
Wire Notes Line
	7600 3800 7600 4800
Connection ~ 8500 3000
Wire Wire Line
	8500 3250 8500 3000
Wire Wire Line
	8500 3350 8900 3350
Wire Wire Line
	8200 4300 8200 4550
Wire Wire Line
	6700 3600 6700 3850
Wire Wire Line
	4700 3400 4700 3350
Wire Wire Line
	7500 3350 7500 3400
Wire Wire Line
	7500 3050 7500 3000
Connection ~ 7500 3000
Wire Wire Line
	7850 3450 7850 3350
Wire Wire Line
	8950 4550 8950 4450
Wire Wire Line
	2800 4200 2950 4200
Wire Wire Line
	2850 4400 2800 4400
Wire Wire Line
	2850 3800 2850 4550
Wire Wire Line
	2800 3900 2850 3900
Connection ~ 2850 3900
Wire Wire Line
	2850 4500 2800 4500
Connection ~ 2850 4400
Connection ~ 2850 4500
Wire Wire Line
	3050 4100 2800 4100
Wire Wire Line
	3050 3900 3050 4100
Wire Wire Line
	3050 4000 2800 4000
Connection ~ 3050 4000
Wire Wire Line
	2800 3100 2900 3100
Wire Wire Line
	2900 3100 2900 3200
Wire Wire Line
	2950 4200 2950 4300
Connection ~ 2950 4300
Wire Wire Line
	2800 3000 3550 3000
Wire Wire Line
	2800 3800 2850 3800
Wire Notes Line
	2400 3700 3550 3700
Wire Notes Line
	3550 3700 3550 4800
Wire Notes Line
	3550 4800 2400 4800
Wire Notes Line
	2400 4800 2400 3700
Wire Notes Line
	2300 5050 2300 2200
Wire Notes Line
	2300 5050 9650 5050
Wire Notes Line
	2300 2200 9650 2200
Wire Wire Line
	4250 3750 4250 4000
Connection ~ 4250 3900
Wire Wire Line
	4250 4350 4250 4300
Wire Wire Line
	4250 2800 4250 3450
Connection ~ 4250 3000
Wire Notes Line
	9400 4800 9400 3800
Wire Wire Line
	8600 4000 9000 4000
Connection ~ 8700 4000
Wire Wire Line
	8700 4100 8600 4100
Connection ~ 8700 4100
Wire Wire Line
	8950 4150 8950 4000
Connection ~ 8950 4000
Connection ~ 8100 3000
Wire Wire Line
	7850 3050 7850 3000
Connection ~ 7850 3000
Wire Wire Line
	7250 3300 7200 3300
Wire Wire Line
	7700 4000 7800 4000
Wire Notes Line
	9650 2200 9650 5050
Wire Wire Line
	8700 3450 8700 4150
Wire Wire Line
	8700 3450 8500 3450
Wire Wire Line
	5700 3700 5600 3700
Wire Wire Line
	6100 4100 6100 4200
Wire Wire Line
	6000 3700 6100 3700
Wire Wire Line
	6100 3300 6100 3800
Wire Wire Line
	5850 3300 6200 3300
Connection ~ 6100 3700
Connection ~ 6100 3300
Wire Wire Line
	5250 3300 5150 3300
Wire Wire Line
	5150 3300 5150 3000
Connection ~ 5150 3000
Wire Wire Line
	5750 3300 5550 3300
Wire Wire Line
	8100 3000 8100 3700
Wire Wire Line
	8100 3700 7700 3700
Wire Wire Line
	7700 3700 7700 4000
Wire Wire Line
	4250 3900 4150 3900
Wire Wire Line
	4700 3000 4700 3050
Connection ~ 4700 3000
Wire Wire Line
	3800 3000 3650 3000
Wire Wire Line
	4100 3000 6200 3000
$Comp
L +BATT #PWR0125
U 1 1 5A702E5A
P 3200 4200
F 0 "#PWR0125" H 3200 4050 50  0001 C CNN
F 1 "+BATT" H 3200 4340 50  0000 C CNN
F 2 "" H 3200 4200 50  0000 C CNN
F 3 "" H 3200 4200 50  0000 C CNN
	1    3200 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 4300 3200 4300
Wire Wire Line
	3200 4300 3200 4200
$EndSCHEMATC
