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
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title "Shield Mega2560 core Full"
Date "2017-08-22"
Rev "2.00"
Comp ""
Comment1 "Reproduction et commercialisation interdite"
Comment2 "By Antho.breizh"
Comment3 ""
Comment4 "Shield pour RC OpenAVRc"
$EndDescr
$Comp
L CONN_02X03 P9
U 1 1 58A78BA9
P 4750 3150
F 0 "P9" H 4750 2900 50  0000 C CNN
F 1 "SPI" H 4750 3400 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03_Pitch2.54mm" H 4750 1950 50  0001 C CNN
F 3 "" H 4750 1950 50  0000 C CNN
	1    4750 3150
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR01
U 1 1 58A78BAA
P 1100 1400
F 0 "#PWR01" H 1100 1250 50  0001 C CNN
F 1 "VCC" H 1100 1550 50  0000 C CNN
F 2 "" H 1100 1400 50  0000 C CNN
F 3 "" H 1100 1400 50  0000 C CNN
	1    1100 1400
	-1   0    0    1   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR02
U 1 1 58A78BAB
P 1400 1400
F 0 "#PWR02" H 1400 1150 50  0001 C CNN
F 1 "GND" H 1400 1250 50  0000 C CNN
F 2 "" H 1400 1400 50  0000 C CNN
F 3 "" H 1400 1400 50  0000 C CNN
	1    1400 1400
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR03
U 1 1 58A78BAF
P 3650 7050
F 0 "#PWR03" H 3650 6800 50  0001 C CNN
F 1 "GND" H 3650 6900 50  0000 C CNN
F 2 "" H 3650 7050 50  0000 C CNN
F 3 "" H 3650 7050 50  0000 C CNN
	1    3650 7050
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR04
U 1 1 58A78BB0
P 3300 7300
F 0 "#PWR04" H 3300 7150 50  0001 C CNN
F 1 "VCC" V 3300 7500 50  0000 C CNN
F 2 "" H 3300 7300 50  0000 C CNN
F 3 "" H 3300 7300 50  0000 C CNN
	1    3300 7300
	0    1    1    0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR05
U 1 1 58A78BB1
P 5050 3400
F 0 "#PWR05" H 5050 3150 50  0001 C CNN
F 1 "GND" H 5050 3250 50  0000 C CNN
F 2 "" H 5050 3400 50  0000 C CNN
F 3 "" H 5050 3400 50  0000 C CNN
	1    5050 3400
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR06
U 1 1 58A78BB2
P 5350 3400
F 0 "#PWR06" H 5350 3250 50  0001 C CNN
F 1 "VCC" H 5350 3550 50  0000 C CNN
F 2 "" H 5350 3400 50  0000 C CNN
F 3 "" H 5350 3400 50  0000 C CNN
	1    5350 3400
	-1   0    0    1   
$EndComp
Text Label 2750 5050 0    60   ~ 0
Isp_rst
Text Label 4150 3250 0    60   ~ 0
Isp_rst
Text Label 4150 3150 0    60   ~ 0
Sck
Text Label 4150 3050 0    60   ~ 0
Miso
Text Label 5250 3150 2    60   ~ 0
Mosi
Text Label 2750 4950 0    60   ~ 0
Sck
Text Label 2750 4850 0    60   ~ 0
Miso
Text Label 4050 4950 2    60   ~ 0
Mosi
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR07
U 1 1 58A78BB3
P 4300 5050
F 0 "#PWR07" H 4300 4800 50  0001 C CNN
F 1 "GND" V 4300 4850 50  0000 C CNN
F 2 "" H 4300 5050 50  0000 C CNN
F 3 "" H 4300 5050 50  0000 C CNN
	1    4300 5050
	0    -1   -1   0   
$EndComp
$Comp
L VCC #PWR08
U 1 1 58A78BB4
P 4300 4850
F 0 "#PWR08" H 4300 4700 50  0001 C CNN
F 1 "VCC" V 4300 5050 50  0000 C CNN
F 2 "" H 4300 4850 50  0000 C CNN
F 3 "" H 4300 4850 50  0000 C CNN
	1    4300 4850
	0    1    1    0   
$EndComp
$Comp
L VCC #PWR09
U 1 1 58A78BB5
P 2000 4700
F 0 "#PWR09" H 2000 4550 50  0001 C CNN
F 1 "VCC" V 2000 4900 50  0000 C CNN
F 2 "" H 2000 4700 50  0000 C CNN
F 3 "" H 2000 4700 50  0000 C CNN
	1    2000 4700
	0    1    1    0   
$EndComp
Text Label 750  5650 0    60   ~ 0
Scl
Text Label 2250 5650 2    60   ~ 0
Sda
Text Label 900  1200 1    60   ~ 0
Scl
Text Label 1000 1200 1    60   ~ 0
Sda
$Comp
L CONN_01X06 P6
U 1 1 58A78BB6
P 2150 700
F 0 "P6" H 2150 1050 50  0000 C CNN
F 1 "Micro_sd" V 2250 700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06_Pitch2.54mm" H 2150 700 50  0001 C CNN
F 3 "" H 2150 700 50  0000 C CNN
	1    2150 700 
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR010
U 1 1 58A78BB7
P 2600 1400
F 0 "#PWR010" H 2600 1150 50  0001 C CNN
F 1 "GND" H 2600 1250 50  0000 C CNN
F 2 "" H 2600 1400 50  0000 C CNN
F 3 "" H 2600 1400 50  0000 C CNN
	1    2600 1400
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR011
U 1 1 58A78BB8
P 2300 1400
F 0 "#PWR011" H 2300 1250 50  0001 C CNN
F 1 "VCC" H 2300 1550 50  0000 C CNN
F 2 "" H 2300 1400 50  0000 C CNN
F 3 "" H 2300 1400 50  0000 C CNN
	1    2300 1400
	-1   0    0    1   
$EndComp
Text Label 1900 1350 1    60   ~ 0
Microsd_CS
Text Label 2000 1350 1    60   ~ 0
Sck
Text Label 2100 1350 1    60   ~ 0
Mosi
Text Label 2200 1350 1    60   ~ 0
Miso
Text Label 500  5750 0    60   ~ 0
audio_left_in
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR012
U 1 1 58A78BBC
P 5050 1250
F 0 "#PWR012" H 5050 1000 50  0001 C CNN
F 1 "GND" H 5050 1100 50  0000 C CNN
F 2 "" H 5050 1250 50  0000 C CNN
F 3 "" H 5050 1250 50  0000 C CNN
	1    5050 1250
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR013
U 1 1 58A78BBD
P 4750 1400
F 0 "#PWR013" H 4750 1250 50  0001 C CNN
F 1 "VCC" H 4750 1550 50  0000 C CNN
F 2 "" H 4750 1400 50  0000 C CNN
F 3 "" H 4750 1400 50  0000 C CNN
	1    4750 1400
	-1   0    0    1   
$EndComp
Text Label 4450 1950 1    60   ~ 0
audio_left_in
Text Label 4550 1950 1    60   ~ 0
audio_right_in
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR014
U 1 1 58A78BBE
P 9050 1050
F 0 "#PWR014" H 9050 800 50  0001 C CNN
F 1 "GND" H 9050 900 50  0000 C CNN
F 2 "" H 9050 1050 50  0000 C CNN
F 3 "" H 9050 1050 50  0000 C CNN
	1    9050 1050
	1    0    0    -1  
$EndComp
Text Label 9350 1300 1    60   ~ 0
Lcd_vo
Text Label 10550 1300 1    60   ~ 0
Lcd_A0
Text Label 10450 1300 1    60   ~ 0
Lcd_r/w
Text Label 10350 1300 1    60   ~ 0
Lcd_e
Text Label 9550 1300 1    60   ~ 0
Lcd_db0
Text Label 9650 1300 1    60   ~ 0
Lcd_db1
Text Label 9750 1300 1    60   ~ 0
Lcd_db2
Text Label 9850 1300 1    60   ~ 0
Lcd_db3
Text Label 9950 1300 1    60   ~ 0
Lcd_db4
Text Label 10150 1300 1    60   ~ 0
Lcd_db6
Text Label 10250 1300 1    60   ~ 0
Lcd_db7
Text Label 10650 1300 1    60   ~ 0
Lcd_rst
Text Label 10500 2200 1    60   ~ 0
Lcd_bla
Text Label 10850 1300 1    60   ~ 0
Lcd_blk
Text Label 4100 5150 2    60   ~ 0
Lcd_db0
Text Label 2750 5250 0    60   ~ 0
Lcd_db1
Text Label 4100 5250 2    60   ~ 0
Lcd_db2
Text Label 2750 5350 0    60   ~ 0
Lcd_db3
Text Label 4100 5350 2    60   ~ 0
Lcd_db4
Text Label 2750 5450 0    60   ~ 0
Lcd_db5
Text Label 4100 5450 2    60   ~ 0
Lcd_db6
Text Label 2750 5550 0    60   ~ 0
Lcd_db7
Text Label 2800 8250 1    60   ~ 0
Lcd_e
Text Label 2700 8250 1    60   ~ 0
Lcd_A0
Text Label 2600 8250 1    60   ~ 0
Lcd_cs
Text Label 2800 6850 3    60   ~ 0
Lcd_r/w
Text Label 2700 6850 3    60   ~ 0
Lcd_rst
Text Label 2600 6850 3    60   ~ 0
Lcd_bla
Text Label 4100 6150 2    60   ~ 0
Stick_LV
Text Label 2750 6250 0    60   ~ 0
Stick_RH
Text Label 2750 6150 0    60   ~ 0
Stick_RV
Text Label 4100 6050 2    60   ~ 0
Stick_LH
Text Label 6200 3950 1    60   ~ 0
Trim_RH->
Text Label 6300 3950 1    60   ~ 0
Trim_RH<-
Text Label 6500 3950 1    60   ~ 0
Trim_RV->
Text Label 6000 3950 1    60   ~ 0
Trim_RV<-
Text Label 6400 3950 1    60   ~ 0
Trim_LV->
Text Label 6100 3950 1    60   ~ 0
Trim_LV<-
Text Label 5800 3950 1    60   ~ 0
Trim_LH->
Text Label 5900 3950 1    60   ~ 0
Trim_LH<-
Text Label 2650 6650 0    60   ~ 0
Trim_RH->
Text Label 4150 6450 2    60   ~ 0
Trim_RV<-
Text Label 2650 6550 0    60   ~ 0
Trim_RV->
Text Label 2650 6450 0    60   ~ 0
Trim_LV->
Text Label 2650 6350 0    60   ~ 0
Trim_LH->
Text Label 4150 6550 2    60   ~ 0
Trim_RH<-
Text Label 4150 6350 2    60   ~ 0
Trim_LV<-
Text Label 4150 6250 2    60   ~ 0
Trim_LH<-
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C29
U 1 1 58A78BC4
P 12500 1600
F 0 "C29" H 12525 1700 50  0000 L CNN
F 1 "100nF" H 12525 1500 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 12538 1450 50  0001 C CNN
F 3 "" H 12500 1600 50  0000 C CNN
	1    12500 1600
	0    -1   -1   0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C27
U 1 1 58A78BC5
P 12400 1700
F 0 "C27" H 12425 1800 50  0000 L CNN
F 1 "100nF" H 12425 1600 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 12438 1550 50  0001 C CNN
F 3 "" H 12400 1700 50  0000 C CNN
	1    12400 1700
	0    -1   -1   0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C26
U 1 1 58A78BC6
P 12300 1800
F 0 "C26" H 12325 1900 50  0000 L CNN
F 1 "100nF" H 12325 1700 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 12338 1650 50  0001 C CNN
F 3 "" H 12300 1800 50  0000 C CNN
	1    12300 1800
	0    -1   -1   0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C34
U 1 1 58A78BC7
P 13750 1600
F 0 "C34" H 13775 1700 50  0000 L CNN
F 1 "100nF" H 13775 1500 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 13788 1450 50  0001 C CNN
F 3 "" H 13750 1600 50  0000 C CNN
	1    13750 1600
	0    -1   -1   0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C33
U 1 1 58A78BC8
P 13650 1700
F 0 "C33" H 13675 1800 50  0000 L CNN
F 1 "100nF" H 13675 1600 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 13688 1550 50  0001 C CNN
F 3 "" H 13650 1700 50  0000 C CNN
	1    13650 1700
	0    -1   -1   0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C32
U 1 1 58A78BC9
P 13550 1800
F 0 "C32" H 13575 1900 50  0000 L CNN
F 1 "100nF" H 13575 1700 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 13588 1650 50  0001 C CNN
F 3 "" H 13550 1800 50  0000 C CNN
	1    13550 1800
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR015
U 1 1 58A78BCA
P 13900 1950
F 0 "#PWR015" H 13900 1700 50  0001 C CNN
F 1 "GND" V 13750 1900 50  0000 C CNN
F 2 "" H 13900 1950 50  0000 C CNN
F 3 "" H 13900 1950 50  0000 C CNN
	1    13900 1950
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X04 P23
U 1 1 58A78BCB
P 12300 700
F 0 "P23" H 12300 950 50  0000 C CNN
F 1 "Rot_1" V 12400 700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch2.54mm" H 12300 700 50  0001 C CNN
F 3 "" H 12300 700 50  0000 C CNN
	1    12300 700 
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X04 P24
U 1 1 58A78BCC
P 13550 700
F 0 "P24" H 13550 950 50  0000 C CNN
F 1 "Rot_2" V 13650 700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch2.54mm" H 13550 700 50  0001 C CNN
F 3 "" H 13550 700 50  0000 C CNN
	1    13550 700 
	0    -1   -1   0   
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R19
U 1 1 58A78BCD
P 11950 1100
F 0 "R19" V 12030 1100 50  0000 C CNN
F 1 "5.1k" V 11950 1100 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 11880 1100 50  0001 C CNN
F 3 "" H 11950 1100 50  0000 C CNN
	1    11950 1100
	0    -1   -1   0   
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R18
U 1 1 58A78BCE
P 11950 950
F 0 "R18" V 12030 950 50  0000 C CNN
F 1 "5.1k" V 11950 950 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 11880 950 50  0001 C CNN
F 3 "" H 11950 950 50  0000 C CNN
	1    11950 950 
	0    -1   -1   0   
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R23
U 1 1 58A78BCF
P 13200 1100
F 0 "R23" V 13280 1100 50  0000 C CNN
F 1 "5.1k" V 13200 1100 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 13130 1100 50  0001 C CNN
F 3 "" H 13200 1100 50  0000 C CNN
	1    13200 1100
	0    -1   -1   0   
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R22
U 1 1 58A78BD0
P 13200 950
F 0 "R22" V 13280 950 50  0000 C CNN
F 1 "5.1k" V 13200 950 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 13130 950 50  0001 C CNN
F 3 "" H 13200 950 50  0000 C CNN
	1    13200 950 
	0    -1   -1   0   
$EndComp
$Comp
L VCC #PWR016
U 1 1 58A78BD1
P 11800 1400
F 0 "#PWR016" H 11800 1250 50  0001 C CNN
F 1 "VCC" V 11900 1500 50  0000 C CNN
F 2 "" H 11800 1400 50  0000 C CNN
F 3 "" H 11800 1400 50  0000 C CNN
	1    11800 1400
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR017
U 1 1 58A78BD2
P 13050 1400
F 0 "#PWR017" H 13050 1250 50  0001 C CNN
F 1 "VCC" V 12950 1500 50  0000 C CNN
F 2 "" H 13050 1400 50  0000 C CNN
F 3 "" H 13050 1400 50  0000 C CNN
	1    13050 1400
	-1   0    0    1   
$EndComp
Text Label 12350 2500 1    60   ~ 0
Rot_1A
Text Label 12250 2500 1    60   ~ 0
Rot_1B
Text Label 12150 2500 1    60   ~ 0
Rot_1push
Text Label 13600 2500 1    60   ~ 0
Rot_2A
Text Label 13500 2500 1    60   ~ 0
Rot_2B
Text Label 13400 2500 1    60   ~ 0
Rot_2push
Text Label 2350 5350 2    60   ~ 0
JQ6500Data
Text Label 2250 5550 2    60   ~ 0
Rot_2B
Text Label 750  5550 0    60   ~ 0
Rot_2A
Text Label 2250 5150 2    60   ~ 0
Rot_1A
Text Label 750  5150 0    60   ~ 0
Rot_1B
Text Label 15150 1500 1    60   ~ 0
Key_ESC
Text Label 15250 1500 1    60   ~ 0
Key_MENU
Text Label 750  6450 0    60   ~ 0
Key_ESC
Text Label 2250 6450 2    60   ~ 0
Key_MENU
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C3
U 1 1 58A78BDA
P 2450 1150
F 0 "C3" H 2475 1250 50  0000 L CNN
F 1 "100nF" H 2475 1050 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 2488 1000 50  0001 C CNN
F 3 "" H 2450 1150 50  0000 C CNN
	1    2450 1150
	0    1    1    0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C1
U 1 1 58A78BDB
P 1250 1200
F 0 "C1" H 1275 1300 50  0000 L CNN
F 1 "100nF" H 1275 1100 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 1288 1050 50  0001 C CNN
F 3 "" H 1250 1200 50  0000 C CNN
	1    1250 1200
	0    1    1    0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C5
U 1 1 58A78BDC
P 4900 1050
F 0 "C5" H 4925 1150 50  0000 L CNN
F 1 "100nF" H 4925 950 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 4938 900 50  0001 C CNN
F 3 "" H 4900 1050 50  0000 C CNN
	1    4900 1050
	0    1    1    0   
$EndComp
Text Label 7450 3950 1    60   ~ 0
Sw_id1
Text Label 7550 3950 1    60   ~ 0
Sw_id2
Text Label 7850 3950 1    60   ~ 0
Sw_ailDR
Text Label 7950 3950 1    60   ~ 0
Sw_thcut
Text Label 7750 3950 1    60   ~ 0
Sw_gear
Text Label 7650 3950 1    60   ~ 0
Sw_rudDR
Text Label 8150 3950 1    60   ~ 0
Sw_turn
Text Label 8050 3950 1    60   ~ 0
Sw_eleDR
$Comp
L CONN_01X08 P25
U 1 1 58A78BF0
P 14900 700
F 0 "P25" H 14900 1150 50  0000 C CNN
F 1 "Keys" V 15000 700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x04_Pitch2.54mm" H 14900 700 50  0001 C CNN
F 3 "" H 14900 700 50  0000 C CNN
	1    14900 700 
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR018
U 1 1 58A78BF6
P 12650 1950
F 0 "#PWR018" H 12650 1700 50  0001 C CNN
F 1 "GND" V 12500 1900 50  0000 C CNN
F 2 "" H 12650 1950 50  0000 C CNN
F 3 "" H 12650 1950 50  0000 C CNN
	1    12650 1950
	1    0    0    -1  
$EndComp
Text Label 14750 1500 1    60   ~ 0
Key_UP
Text Label 14850 1500 1    60   ~ 0
Key_DOWN
Text Label 14950 1500 1    60   ~ 0
Key_LEFT
Text Label 15050 1500 1    60   ~ 0
Key_RIGHT
Text Label 750  6250 0    60   ~ 0
Key_UP
Text Label 2250 6250 2    60   ~ 0
Key_DOWN
Text Label 750  6350 0    60   ~ 0
Key_LEFT
Text Label 2250 6350 2    60   ~ 0
Key_RIGHT
Text Label 2500 8250 1    60   ~ 0
Sw_id2
Text Label 2500 6850 3    60   ~ 0
Sw_id1
Text Label 2300 8250 1    60   ~ 0
Sw_ailDR
Text Label 4100 5550 2    60   ~ 0
Sw_thcut
Text Label 2400 8250 1    60   ~ 0
Sw_gear
Text Label 2400 6850 3    60   ~ 0
Sw_rudDR
Text Label 750  6550 0    60   ~ 0
Sw_turn
Text Label 2250 6550 2    60   ~ 0
Sw_eleDR
Text Label 2750 5950 0    60   ~ 0
Pot_3
Text Label 4100 5950 2    60   ~ 0
Pot_2
$Comp
L CONN_01X04 P15
U 1 1 58A78BFB
P 8300 700
F 0 "P15" H 8300 950 50  0000 C CNN
F 1 "LCD_I2C" V 8400 700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch2.54mm" H 8300 700 50  0001 C CNN
F 3 "" H 8300 700 50  0000 C CNN
	1    8300 700 
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X20 P16
U 1 1 58A78BFC
P 10000 700
F 0 "P16" H 10000 1750 50  0000 C CNN
F 1 "LCD" V 10100 700 50  0000 C CNN
F 2 "anthoperso:Pin_Header_Straight_2x10_Pitch2.54mm" H 10000 700 50  0001 C CNN
F 3 "" H 10000 700 50  0000 C CNN
	1    10000 700 
	0    -1   -1   0   
$EndComp
Text Label 10950 1300 1    60   ~ 0
Lcd_cs
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR019
U 1 1 58A78BFD
P 8150 1200
F 0 "#PWR019" H 8150 950 50  0001 C CNN
F 1 "GND" V 8150 1000 50  0000 C CNN
F 2 "" H 8150 1200 50  0000 C CNN
F 3 "" H 8150 1200 50  0000 C CNN
	1    8150 1200
	1    0    0    -1  
$EndComp
Text Label 8350 1150 1    60   ~ 0
Sda
Text Label 8450 1150 1    60   ~ 0
Scl
Text Label 2250 3900 1    60   ~ 0
Pot_3
Text Label 1600 3900 1    60   ~ 0
Pot_2
Text Label 1000 3900 1    60   ~ 0
Pot_1
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C31
U 1 1 58A78C01
P 13400 3800
F 0 "C31" H 13425 3900 50  0000 L CNN
F 1 "100nF" H 13150 3700 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 13438 3650 50  0001 C CNN
F 3 "" H 13400 3800 50  0000 C CNN
	1    13400 3800
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R16
U 1 1 58A78C03
P 11800 4750
F 0 "R16" V 11880 4750 50  0000 C CNN
F 1 "2.7K" V 11800 4750 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 11730 4750 50  0001 C CNN
F 3 "" H 11800 4750 50  0000 C CNN
	1    11800 4750
	-1   0    0    -1  
$EndComp
Text Label 4100 5850 2    60   ~ 0
U_batt
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR020
U 1 1 58A78C04
P 14100 5150
F 0 "#PWR020" H 14100 4900 50  0001 C CNN
F 1 "GND" H 14100 4950 50  0000 C CNN
F 2 "" H 14100 5150 50  0000 C CNN
F 3 "" H 14100 5150 50  0000 C CNN
	1    14100 5150
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P26
U 1 1 58A78C05
P 14300 3850
F 0 "P26" H 14300 4050 50  0000 C CNN
F 1 "LCD_Alim" H 14300 3650 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 14300 3850 50  0001 C CNN
F 3 "" H 14300 3850 50  0000 C CNN
	1    14300 3850
	-1   0    0    -1  
$EndComp
Text Label 14900 3850 2    60   ~ 0
LCD_VCC
Text Notes 14900 4400 0    60   ~ 0
+3.3V\n
Text Notes 14500 3500 0    60   ~ 0
+5V
$Comp
L VCC #PWR021
U 1 1 58A78C06
P 14750 3600
F 0 "#PWR021" H 14750 3450 50  0001 C CNN
F 1 "VCC" V 14750 3800 50  0000 C CNN
F 2 "" H 14750 3600 50  0000 C CNN
F 3 "" H 14750 3600 50  0000 C CNN
	1    14750 3600
	0    1    1    0   
$EndComp
Text Label 9250 1300 1    60   ~ 0
LCD_VCC
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C38
U 1 1 58A78C08
P 14600 4900
F 0 "C38" H 14625 5000 50  0000 L CNN
F 1 "100nF" H 14625 4800 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 14638 4750 50  0001 C CNN
F 3 "" H 14600 4900 50  0000 C CNN
	1    14600 4900
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R7
U 1 1 58A78C0A
P 4900 6600
F 0 "R7" V 4980 6600 50  0000 C CNN
F 1 "470" V 4900 6600 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 4830 6600 50  0001 C CNN
F 3 "" H 4900 6600 50  0000 C CNN
	1    4900 6600
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR022
U 1 1 58A78C0B
P 5150 7450
F 0 "#PWR022" H 5150 7200 50  0001 C CNN
F 1 "GND" H 5150 7300 50  0000 C CNN
F 2 "" H 5150 7450 50  0000 C CNN
F 3 "" H 5150 7450 50  0000 C CNN
	1    5150 7450
	1    0    0    -1  
$EndComp
Text Label 6050 6550 0    60   ~ 0
Tele_TX
Text Label 2250 5050 2    60   ~ 0
Tele_TX
Text Label 2450 6150 2    60   ~ 0
Master_PPMout
$Comp
L +BATT #PWR023
U 1 1 58A78C13
P 4900 6350
F 0 "#PWR023" H 4900 6200 50  0001 C CNN
F 1 "+BATT" H 4900 6490 50  0000 C CNN
F 2 "" H 4900 6350 50  0000 C CNN
F 3 "" H 4900 6350 50  0000 C CNN
	1    4900 6350
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X08 P19
U 1 1 58A78C14
P 8450 6450
F 0 "P19" H 8450 6900 50  0000 C CNN
F 1 "Module_TX" V 8550 6450 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x04_Pitch2.54mm" H 8450 6450 50  0001 C CNN
F 3 "" H 8450 6450 50  0000 C CNN
	1    8450 6450
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR024
U 1 1 58A78C15
P 8500 6750
F 0 "#PWR024" H 8500 6500 50  0001 C CNN
F 1 "GND" V 8500 6550 50  0000 C CNN
F 2 "" H 8500 6750 50  0000 C CNN
F 3 "" H 8500 6750 50  0000 C CNN
	1    8500 6750
	1    0    0    -1  
$EndComp
Text Label 8400 7300 1    60   ~ 0
Master_PPMout
$Comp
L VCC #PWR025
U 1 1 58A78C16
P 8600 6750
F 0 "#PWR025" H 8600 6600 50  0001 C CNN
F 1 "VCC" V 8600 6950 50  0000 C CNN
F 2 "" H 8600 6750 50  0000 C CNN
F 3 "" H 8600 6750 50  0000 C CNN
	1    8600 6750
	-1   0    0    1   
$EndComp
Text Label 750  4950 0    60   ~ 0
Tele_RX
Text Label 6050 7250 0    60   ~ 0
Tele_RX
Text Notes 6650 6050 2    60   ~ 0
Isolation Module TTL
Text Label 750  5950 0    60   ~ 0
Microsd_CS
$Comp
L +BATT #PWR026
U 1 1 58A78C18
P 8100 6750
F 0 "#PWR026" H 8100 6600 50  0001 C CNN
F 1 "+BATT" H 8100 6890 50  0000 C CNN
F 2 "" H 8100 6750 50  0000 C CNN
F 3 "" H 8100 6750 50  0000 C CNN
	1    8100 6750
	-1   0    0    1   
$EndComp
NoConn ~ 8100 6450
NoConn ~ 8200 6450
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R8
U 1 1 58A78C1C
P 14850 9100
F 0 "R8" V 14930 9100 50  0000 C CNN
F 1 "200" V 14850 9100 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 14780 9100 50  0001 C CNN
F 3 "" H 14850 9100 50  0000 C CNN
	1    14850 9100
	0    1    1    0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR027
U 1 1 58A78C22
P 1250 7050
F 0 "#PWR027" H 1250 6800 50  0001 C CNN
F 1 "GND" H 1250 6900 50  0000 C CNN
F 2 "" H 1250 7050 50  0000 C CNN
F 3 "" H 1250 7050 50  0000 C CNN
	1    1250 7050
	1    0    0    -1  
$EndComp
Text Label 500  6050 0    60   ~ 0
PPM16_SIMout
Text Notes 9700 2950 0    60   ~ 0
Power Management
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR028
U 1 1 58A78C35
P 12950 4150
F 0 "#PWR028" H 12950 3900 50  0001 C CNN
F 1 "GND" H 12950 4000 50  0000 C CNN
F 2 "" H 12950 4150 50  0000 C CNN
F 3 "" H 12950 4150 50  0000 C CNN
	1    12950 4150
	-1   0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR029
U 1 1 58A78C36
P 11800 4950
F 0 "#PWR029" H 11800 4700 50  0001 C CNN
F 1 "GND" H 11800 4800 50  0000 C CNN
F 2 "" H 11800 4950 50  0000 C CNN
F 3 "" H 11800 4950 50  0000 C CNN
	1    11800 4950
	1    0    0    -1  
$EndComp
Text Notes 12700 8150 2    60   ~ 0
PPM Management
Text Label 2750 6050 0    60   ~ 0
Pot_1
$Comp
L VCC #PWR030
U 1 1 58A78C63
P 1100 2550
F 0 "#PWR030" H 1100 2400 50  0001 C CNN
F 1 "VCC" H 1100 2700 50  0000 C CNN
F 2 "" H 1100 2550 50  0000 C CNN
F 3 "" H 1100 2550 50  0000 C CNN
	1    1100 2550
	-1   0    0    1   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR031
U 1 1 58A78C64
P 1400 2550
F 0 "#PWR031" H 1400 2300 50  0001 C CNN
F 1 "GND" H 1400 2400 50  0000 C CNN
F 2 "" H 1400 2550 50  0000 C CNN
F 3 "" H 1400 2550 50  0000 C CNN
	1    1400 2550
	1    0    0    -1  
$EndComp
Text Label 900  2350 1    60   ~ 0
Scl
Text Label 1000 2350 1    60   ~ 0
Sda
$Comp
L CONN_01X04 P3
U 1 1 58A78C66
P 1050 1850
F 0 "P3" H 1050 1600 50  0000 C CNN
F 1 "Option" V 1150 1850 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch2.54mm" H 1050 1850 50  0001 C CNN
F 3 "" H 1050 1850 50  0000 C CNN
	1    1050 1850
	0    1    -1   0   
$EndComp
Text Label 2300 2650 1    60   ~ 0
Xmitter_SCK
Text Label 2500 2650 1    60   ~ 0
Xmitter_MOSI
Text Label 2200 2650 1    60   ~ 0
Xmitter_MISO
$Comp
L CONN_01X02 P21
U 1 1 58A78C6B
P 11350 3400
F 0 "P21" H 11350 3550 50  0000 C CNN
F 1 "Sw_power" V 11050 3400 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 11350 3400 50  0001 C CNN
F 3 "" H 11350 3400 50  0000 C CNN
	1    11350 3400
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X02 P5
U 1 1 58A78C6D
P 3350 3000
F 0 "P5" H 3350 3150 50  0000 C CNN
F 1 "Vibreur" V 3450 3000 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 3350 3000 50  0001 C CNN
F 3 "" H 3350 3000 50  0000 C CNN
	1    3350 3000
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR032
U 1 1 58A78C6F
P 3600 3850
F 0 "#PWR032" H 3600 3600 50  0001 C CNN
F 1 "GND" H 3600 3700 50  0000 C CNN
F 2 "" H 3600 3850 50  0000 C CNN
F 3 "" H 3600 3850 50  0000 C CNN
	1    3600 3850
	1    0    0    -1  
$EndComp
Text Label 3300 4050 1    60   ~ 0
Vibreur
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R1
U 1 1 58A78C70
P 4450 1200
F 0 "R1" V 4350 1200 50  0000 C CNN
F 1 "22K" V 4450 1200 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 4380 1200 50  0001 C CNN
F 3 "" H 4450 1200 50  0000 C CNN
	1    4450 1200
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R2
U 1 1 58A78C71
P 4550 1200
F 0 "R2" V 4630 1200 50  0000 C CNN
F 1 "1K" V 4550 1200 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 4480 1200 50  0001 C CNN
F 3 "" H 4550 1200 50  0000 C CNN
	1    4550 1200
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR033
U 1 1 58A78C73
P 10850 1300
F 0 "#PWR033" H 10850 1050 50  0001 C CNN
F 1 "GND" H 10850 1150 50  0000 C CNN
F 2 "" H 10850 1300 50  0000 C CNN
F 3 "" H 10850 1300 50  0000 C CNN
	1    10850 1300
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR034
U 1 1 58A78C74
P 1750 7900
F 0 "#PWR034" H 1750 7650 50  0001 C CNN
F 1 "GND" H 1750 7750 50  0000 C CNN
F 2 "" H 1750 7900 50  0000 C CNN
F 3 "" H 1750 7900 50  0000 C CNN
	1    1750 7900
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR035
U 1 1 58A78C75
P 950 10600
F 0 "#PWR035" H 950 10350 50  0001 C CNN
F 1 "GND" H 950 10450 50  0000 C CNN
F 2 "" H 950 10600 50  0000 C CNN
F 3 "" H 950 10600 50  0000 C CNN
	1    950  10600
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG036
U 1 1 58A78C76
P 950 10600
F 0 "#FLG036" H 950 10695 50  0001 C CNN
F 1 "PWR_FLAG" H 950 10780 50  0000 C CNN
F 2 "" H 950 10600 50  0000 C CNN
F 3 "" H 950 10600 50  0000 C CNN
	1    950  10600
	1    0    0    -1  
$EndComp
Text Label 10050 1300 1    60   ~ 0
Lcd_db5
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR037
U 1 1 58A78C77
P 9450 1900
F 0 "#PWR037" H 9450 1650 50  0001 C CNN
F 1 "GND" H 9450 1750 50  0000 C CNN
F 2 "" H 9450 1900 50  0000 C CNN
F 3 "" H 9450 1900 50  0000 C CNN
	1    9450 1900
	1    0    0    -1  
$EndComp
$Comp
L +BATT #PWR038
U 1 1 58A78C78
P 950 10050
F 0 "#PWR038" H 950 9900 50  0001 C CNN
F 1 "+BATT" H 950 10190 50  0000 C CNN
F 2 "" H 950 10050 50  0000 C CNN
F 3 "" H 950 10050 50  0000 C CNN
	1    950  10050
	-1   0    0    1   
$EndComp
$Comp
L PWR_FLAG #FLG039
U 1 1 58A78C79
P 950 10050
F 0 "#FLG039" H 950 10145 50  0001 C CNN
F 1 "PWR_FLAG" H 950 10230 50  0000 C CNN
F 2 "" H 950 10050 50  0000 C CNN
F 3 "" H 950 10050 50  0000 C CNN
	1    950  10050
	1    0    0    -1  
$EndComp
NoConn ~ 14300 3850
NoConn ~ 14300 3750
NoConn ~ 14300 3950
$Comp
L LD1117S33TR U5
U 1 1 58A78C7A
P 14100 4650
F 0 "U5" H 13900 4400 50  0000 C CNN
F 1 "LD1117S33TR" H 14100 4850 50  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-223" H 14100 4750 50  0001 C CNN
F 3 "" H 14100 4650 50  0000 C CNN
	1    14100 4650
	1    0    0    -1  
$EndComp
$Comp
L MBR0520 D2
U 1 1 58A78C7B
P 12750 9350
F 0 "D2" H 12750 9250 50  0000 C CNN
F 1 "MBR0520" H 12750 9450 50  0000 C CNN
F 2 "Diodes_SMD:D_SOD-123" H 12750 9175 50  0001 C CNN
F 3 "" H 12750 9350 50  0001 C CNN
	1    12750 9350
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X04 P2
U 1 1 58A78C82
P 1050 700
F 0 "P2" H 1050 450 50  0000 C CNN
F 1 "Clock" V 1150 700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch2.54mm" H 1050 700 50  0001 C CNN
F 3 "" H 1050 700 50  0000 C CNN
	1    1050 700 
	0    1    -1   0   
$EndComp
NoConn ~ 1250 6650
NoConn ~ 1750 6650
NoConn ~ 2300 7300
NoConn ~ 2100 7800
NoConn ~ 2200 7800
NoConn ~ 1750 5250
NoConn ~ 3150 5850
Text Label 7550 800  2    60   ~ 0
audio_right_in
Text Label 7550 1000 2    60   ~ 0
JQ6500Data
Text Label 7550 1200 2    60   ~ 0
JQ6500Busy
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R3
U 1 1 58AB8D39
P 6900 1200
F 0 "R3" V 6980 1200 50  0000 C CNN
F 1 "18K" V 6900 1200 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 6830 1200 50  0001 C CNN
F 3 "" H 6900 1200 50  0000 C CNN
	1    6900 1200
	0    1    1    0   
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R4
U 1 1 58AB997D
P 6900 1000
F 0 "R4" V 6980 1000 50  0000 C CNN
F 1 "1K" V 6900 1000 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 6830 1000 50  0001 C CNN
F 3 "" H 6900 1000 50  0000 C CNN
	1    6900 1000
	0    1    1    0   
$EndComp
$Comp
L VCC #PWR040
U 1 1 58ABA4EE
P 6200 2250
F 0 "#PWR040" H 6200 2100 50  0001 C CNN
F 1 "VCC" H 6200 2400 50  0000 C CNN
F 2 "" H 6200 2250 50  0000 C CNN
F 3 "" H 6200 2250 50  0000 C CNN
	1    6200 2250
	-1   0    0    1   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR041
U 1 1 58ABA64C
P 5550 2150
F 0 "#PWR041" H 5550 1900 50  0001 C CNN
F 1 "GND" H 5550 2000 50  0000 C CNN
F 2 "" H 5550 2150 50  0000 C CNN
F 3 "" H 5550 2150 50  0000 C CNN
	1    5550 2150
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR042
U 1 1 58ABAF25
P 6850 2150
F 0 "#PWR042" H 6850 1900 50  0001 C CNN
F 1 "GND" H 6850 2000 50  0000 C CNN
F 2 "" H 6850 2150 50  0000 C CNN
F 3 "" H 6850 2150 50  0000 C CNN
	1    6850 2150
	1    0    0    -1  
$EndComp
Text Label 3200 10600 1    60   ~ 0
Stick_RHcon
Text Label 3300 10600 1    60   ~ 0
Stick_RVcon
Text Label 2150 10600 1    60   ~ 0
Stick_LVcon
Text Label 2250 10600 1    60   ~ 0
Stick_LHcon
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C9
U 1 1 58AF4E5F
P 2600 10000
F 0 "C9" H 2625 10100 50  0000 L CNN
F 1 "100nF" H 2625 9900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 2638 9850 50  0001 C CNN
F 3 "" H 2600 10000 50  0000 C CNN
	1    2600 10000
	0    -1   -1   0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C11
U 1 1 58AF4E60
P 2600 9700
F 0 "C11" H 2625 9800 50  0000 L CNN
F 1 "100nF" H 2625 9600 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 2638 9550 50  0001 C CNN
F 3 "" H 2600 9700 50  0000 C CNN
	1    2600 9700
	0    -1   -1   0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C16
U 1 1 58AF4E61
P 3650 10000
F 0 "C16" H 3675 10100 50  0000 L CNN
F 1 "100nF" H 3675 9900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 3688 9850 50  0001 C CNN
F 3 "" H 3650 10000 50  0000 C CNN
	1    3650 10000
	0    -1   -1   0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C17
U 1 1 58AF4E62
P 3650 9700
F 0 "C17" H 3675 9800 50  0000 L CNN
F 1 "100nF" H 3675 9600 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 3688 9550 50  0001 C CNN
F 3 "" H 3650 9700 50  0000 C CNN
	1    3650 9700
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR043
U 1 1 58AF4E64
P 2350 9450
F 0 "#PWR043" H 2350 9200 50  0001 C CNN
F 1 "GND" H 2350 9300 50  0000 C CNN
F 2 "" H 2350 9450 50  0000 C CNN
F 3 "" H 2350 9450 50  0000 C CNN
	1    2350 9450
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR044
U 1 1 58AF4E68
P 3400 9450
F 0 "#PWR044" H 3400 9200 50  0001 C CNN
F 1 "GND" H 3400 9300 50  0000 C CNN
F 2 "" H 3400 9450 50  0000 C CNN
F 3 "" H 3400 9450 50  0000 C CNN
	1    3400 9450
	1    0    0    -1  
$EndComp
$Comp
L MCP6004 U6
U 2 1 58AF4E69
P 6850 9300
F 0 "U6" H 6850 9500 50  0000 L CNN
F 1 "MCP604-I/SL" H 6850 9100 50  0000 L CNN
F 2 "Librairie master:SOP14" H 6800 9400 50  0001 C CNN
F 3 "" H 6900 9500 50  0000 C CNN
	2    6850 9300
	0    -1   -1   0   
$EndComp
$Comp
L MCP6004 U6
U 4 1 58AF4E6A
P 10250 9350
F 0 "U6" H 10250 9550 50  0000 L CNN
F 1 "MCP604-I/SL" H 10250 9150 50  0000 L CNN
F 2 "Librairie master:SOP14" H 10200 9450 50  0001 C CNN
F 3 "" H 10300 9550 50  0000 C CNN
	4    10250 9350
	0    -1   -1   0   
$EndComp
$Comp
L MCP6004 U6
U 3 1 58AF4E6B
P 8550 9300
F 0 "U6" H 8550 9500 50  0000 L CNN
F 1 "MCP604-I/SL" H 8550 9100 50  0000 L CNN
F 2 "Librairie master:SOP14" H 8500 9400 50  0001 C CNN
F 3 "" H 8600 9500 50  0000 C CNN
	3    8550 9300
	0    -1   -1   0   
$EndComp
$Comp
L MCP6004 U6
U 1 1 58AF4E6C
P 5150 9300
F 0 "U6" H 5150 9500 50  0000 L CNN
F 1 "MCP604-I/SL" H 5150 9100 50  0000 L CNN
F 2 "Librairie master:SOP14" H 5100 9400 50  0001 C CNN
F 3 "" H 5200 9500 50  0000 C CNN
	1    5150 9300
	0    -1   -1   0   
$EndComp
$Comp
L VCC #PWR045
U 1 1 58AF4E6D
P 9950 9450
F 0 "#PWR045" H 9950 9300 50  0001 C CNN
F 1 "VCC" H 9950 9600 50  0000 C CNN
F 2 "" H 9950 9450 50  0000 C CNN
F 3 "" H 9950 9450 50  0000 C CNN
	1    9950 9450
	0    -1   -1   0   
$EndComp
$Comp
L VCC #PWR046
U 1 1 58AF4E6E
P 4750 9000
F 0 "#PWR046" H 4750 8850 50  0001 C CNN
F 1 "VCC" H 4750 9150 50  0000 C CNN
F 2 "" H 4750 9000 50  0000 C CNN
F 3 "" H 4750 9000 50  0000 C CNN
	1    4750 9000
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R26
U 1 1 58AF4E6F
P 5700 10150
F 0 "R26" V 5780 10150 50  0000 C CNN
F 1 "47K" V 5700 10150 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 5630 10150 50  0001 C CNN
F 3 "" H 5700 10150 50  0000 C CNN
	1    5700 10150
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R28
U 1 1 58AF4E70
P 5900 10150
F 0 "R28" V 5980 10150 50  0000 C CNN
F 1 "120K" V 5900 10150 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 5830 10150 50  0001 C CNN
F 3 "" H 5900 10150 50  0000 C CNN
	1    5900 10150
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR047
U 1 1 58AF4E71
P 6100 10600
F 0 "#PWR047" H 6100 10350 50  0001 C CNN
F 1 "GND" H 6100 10400 50  0000 C CNN
F 2 "" H 6100 10600 50  0000 C CNN
F 3 "" H 6100 10600 50  0000 C CNN
	1    6100 10600
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR048
U 1 1 58AF4E72
P 5450 9400
F 0 "#PWR048" H 5450 9150 50  0001 C CNN
F 1 "GND" H 5450 9200 50  0000 C CNN
F 2 "" H 5450 9400 50  0000 C CNN
F 3 "" H 5450 9400 50  0000 C CNN
	1    5450 9400
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR049
U 1 1 58AF4E73
P 6550 9400
F 0 "#PWR049" H 6550 9250 50  0001 C CNN
F 1 "VCC" H 6550 9550 50  0000 C CNN
F 2 "" H 6550 9400 50  0000 C CNN
F 3 "" H 6550 9400 50  0000 C CNN
	1    6550 9400
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR050
U 1 1 58AF4E74
P 7150 9400
F 0 "#PWR050" H 7150 9150 50  0001 C CNN
F 1 "GND" H 7150 9200 50  0000 C CNN
F 2 "" H 7150 9400 50  0000 C CNN
F 3 "" H 7150 9400 50  0000 C CNN
	1    7150 9400
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR051
U 1 1 58AF4E75
P 8250 9400
F 0 "#PWR051" H 8250 9250 50  0001 C CNN
F 1 "VCC" H 8250 9550 50  0000 C CNN
F 2 "" H 8250 9400 50  0000 C CNN
F 3 "" H 8250 9400 50  0000 C CNN
	1    8250 9400
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR052
U 1 1 58AF4E76
P 8850 9400
F 0 "#PWR052" H 8850 9150 50  0001 C CNN
F 1 "GND" H 8850 9200 50  0000 C CNN
F 2 "" H 8850 9400 50  0000 C CNN
F 3 "" H 8850 9400 50  0000 C CNN
	1    8850 9400
	1    0    0    -1  
$EndComp
Text Label 5700 10900 1    60   ~ 0
Stick_RHcon
Text Label 5150 8350 3    60   ~ 0
Stick_RH
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C43
U 1 1 58AF4E77
P 6150 9400
F 0 "C43" H 6175 9500 50  0000 L CNN
F 1 "22nF" H 6175 9300 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 6188 9250 50  0001 C CNN
F 3 "" H 6150 9400 50  0000 C CNN
	1    6150 9400
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C44
U 1 1 58AF4E7A
P 7850 9400
F 0 "C44" H 7875 9500 50  0000 L CNN
F 1 "22nF" H 7875 9300 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 7888 9250 50  0001 C CNN
F 3 "" H 7850 9400 50  0000 C CNN
	1    7850 9400
	1    0    0    -1  
$EndComp
$Comp
L POT-RESCUE-Mega_2560_core_mini_full_2.0 RV2
U 1 1 58AF4E7B
P 6100 10350
F 0 "RV2" V 6000 10350 50  0000 C CNN
F 1 "100K" V 6100 10350 50  0000 C CNN
F 2 "Potentiometers:Potentiometer_Bourns_3296W_3-8Zoll_Inline_ScrewUp" H 6100 10350 50  0001 C CNN
F 3 "" H 6100 10350 50  0000 C CNN
	1    6100 10350
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR053
U 1 1 58AF4E7C
P 6100 10200
F 0 "#PWR053" H 6100 10050 50  0001 C CNN
F 1 "VCC" H 6100 10350 50  0000 C CNN
F 2 "" H 6100 10200 50  0000 C CNN
F 3 "" H 6100 10200 50  0000 C CNN
	1    6100 10200
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R29
U 1 1 58AF4E7D
P 7400 10150
F 0 "R29" V 7480 10150 50  0000 C CNN
F 1 "47K" V 7400 10150 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 7330 10150 50  0001 C CNN
F 3 "" H 7400 10150 50  0000 C CNN
	1    7400 10150
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R31
U 1 1 58AF4E7E
P 7600 10150
F 0 "R31" V 7680 10150 50  0000 C CNN
F 1 "120K" V 7600 10150 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 7530 10150 50  0001 C CNN
F 3 "" H 7600 10150 50  0000 C CNN
	1    7600 10150
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR054
U 1 1 58AF4E7F
P 7800 10600
F 0 "#PWR054" H 7800 10350 50  0001 C CNN
F 1 "GND" H 7800 10400 50  0000 C CNN
F 2 "" H 7800 10600 50  0000 C CNN
F 3 "" H 7800 10600 50  0000 C CNN
	1    7800 10600
	1    0    0    -1  
$EndComp
$Comp
L POT-RESCUE-Mega_2560_core_mini_full_2.0 RV3
U 1 1 58AF4E80
P 7800 10350
F 0 "RV3" V 7700 10350 50  0000 C CNN
F 1 "100K" V 7800 10350 50  0000 C CNN
F 2 "Potentiometers:Potentiometer_Bourns_3296W_3-8Zoll_Inline_ScrewUp" H 7800 10350 50  0001 C CNN
F 3 "" H 7800 10350 50  0000 C CNN
	1    7800 10350
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR055
U 1 1 58AF4E81
P 7800 10200
F 0 "#PWR055" H 7800 10050 50  0001 C CNN
F 1 "VCC" H 7800 10350 50  0000 C CNN
F 2 "" H 7800 10200 50  0000 C CNN
F 3 "" H 7800 10200 50  0000 C CNN
	1    7800 10200
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C45
U 1 1 58AF4E83
P 9550 9450
F 0 "C45" H 9575 9550 50  0000 L CNN
F 1 "22nF" H 9575 9350 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 9588 9300 50  0001 C CNN
F 3 "" H 9550 9450 50  0000 C CNN
	1    9550 9450
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R32
U 1 1 58AF4E84
P 9100 10150
F 0 "R32" V 9180 10150 50  0000 C CNN
F 1 "47K" V 9100 10150 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 9030 10150 50  0001 C CNN
F 3 "" H 9100 10150 50  0000 C CNN
	1    9100 10150
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R34
U 1 1 58AF4E85
P 9300 10150
F 0 "R34" V 9380 10150 50  0000 C CNN
F 1 "120K" V 9300 10150 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 9230 10150 50  0001 C CNN
F 3 "" H 9300 10150 50  0000 C CNN
	1    9300 10150
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR056
U 1 1 58AF4E86
P 9500 10600
F 0 "#PWR056" H 9500 10350 50  0001 C CNN
F 1 "GND" H 9500 10400 50  0000 C CNN
F 2 "" H 9500 10600 50  0000 C CNN
F 3 "" H 9500 10600 50  0000 C CNN
	1    9500 10600
	1    0    0    -1  
$EndComp
$Comp
L POT-RESCUE-Mega_2560_core_mini_full_2.0 RV4
U 1 1 58AF4E87
P 9500 10350
F 0 "RV4" V 9400 10350 50  0000 C CNN
F 1 "100K" V 9500 10350 50  0000 C CNN
F 2 "Potentiometers:Potentiometer_Bourns_3296W_3-8Zoll_Inline_ScrewUp" H 9500 10350 50  0001 C CNN
F 3 "" H 9500 10350 50  0000 C CNN
	1    9500 10350
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR057
U 1 1 58AF4E88
P 9500 10200
F 0 "#PWR057" H 9500 10050 50  0001 C CNN
F 1 "VCC" H 9500 10350 50  0000 C CNN
F 2 "" H 9500 10200 50  0000 C CNN
F 3 "" H 9500 10200 50  0000 C CNN
	1    9500 10200
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR058
U 1 1 58AF4E89
P 10550 9450
F 0 "#PWR058" H 10550 9200 50  0001 C CNN
F 1 "GND" H 10550 9250 50  0000 C CNN
F 2 "" H 10550 9450 50  0000 C CNN
F 3 "" H 10550 9450 50  0000 C CNN
	1    10550 9450
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C46
U 1 1 58AF4E8B
P 11250 9500
F 0 "C46" H 11275 9600 50  0000 L CNN
F 1 "22nF" H 11275 9400 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 11288 9350 50  0001 C CNN
F 3 "" H 11250 9500 50  0000 C CNN
	1    11250 9500
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R35
U 1 1 58AF4E8C
P 10800 10200
F 0 "R35" V 10880 10200 50  0000 C CNN
F 1 "47K" V 10800 10200 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 10730 10200 50  0001 C CNN
F 3 "" H 10800 10200 50  0000 C CNN
	1    10800 10200
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R37
U 1 1 58AF4E8D
P 11000 10200
F 0 "R37" V 11080 10200 50  0000 C CNN
F 1 "120K" V 11000 10200 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 10930 10200 50  0001 C CNN
F 3 "" H 11000 10200 50  0000 C CNN
	1    11000 10200
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR059
U 1 1 58AF4E8E
P 11200 10650
F 0 "#PWR059" H 11200 10400 50  0001 C CNN
F 1 "GND" H 11200 10450 50  0000 C CNN
F 2 "" H 11200 10650 50  0000 C CNN
F 3 "" H 11200 10650 50  0000 C CNN
	1    11200 10650
	1    0    0    -1  
$EndComp
$Comp
L POT-RESCUE-Mega_2560_core_mini_full_2.0 RV5
U 1 1 58AF4E8F
P 11200 10400
F 0 "RV5" V 11100 10400 50  0000 C CNN
F 1 "100K" V 11200 10400 50  0000 C CNN
F 2 "Potentiometers:Potentiometer_Bourns_3296W_3-8Zoll_Inline_ScrewUp" H 11200 10400 50  0001 C CNN
F 3 "" H 11200 10400 50  0000 C CNN
	1    11200 10400
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR060
U 1 1 58AF4E90
P 11200 10250
F 0 "#PWR060" H 11200 10100 50  0001 C CNN
F 1 "VCC" H 11200 10400 50  0000 C CNN
F 2 "" H 11200 10250 50  0000 C CNN
F 3 "" H 11200 10250 50  0000 C CNN
	1    11200 10250
	1    0    0    -1  
$EndComp
Text Label 6850 8350 3    60   ~ 0
Stick_RV
Text Label 7400 10900 1    60   ~ 0
Stick_RVcon
Text Label 9100 10900 1    60   ~ 0
Stick_LVcon
Text Label 8550 8350 3    60   ~ 0
Stick_LV
Text Label 10250 8400 3    60   ~ 0
Stick_LH
Text Label 10800 10950 1    60   ~ 0
Stick_LHcon
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R24
U 1 1 58AF4E91
P 4750 9650
F 0 "R24" V 4830 9650 50  0000 C CNN
F 1 "82K" V 4750 9650 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 4680 9650 50  0001 C CNN
F 3 "" H 4750 9650 50  0000 C CNN
	1    4750 9650
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C42
U 1 1 58AF4E92
P 4500 10050
F 0 "C42" H 4525 10150 50  0000 L CNN
F 1 "6.8nF" H 4250 9900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 4538 9900 50  0001 C CNN
F 3 "" H 4500 10050 50  0000 C CNN
	1    4500 10050
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R25
U 1 1 58AF4E93
P 4750 10050
F 0 "R25" V 4830 10050 50  0000 C CNN
F 1 "39K" V 4750 10050 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 4680 10050 50  0001 C CNN
F 3 "" H 4750 10050 50  0000 C CNN
	1    4750 10050
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR061
U 1 1 58AF4E94
P 4750 10600
F 0 "#PWR061" H 4750 10350 50  0001 C CNN
F 1 "GND" H 4750 10400 50  0000 C CNN
F 2 "" H 4750 10600 50  0000 C CNN
F 3 "" H 4750 10600 50  0000 C CNN
	1    4750 10600
	1    0    0    -1  
$EndComp
Text Notes 3050 8750 2    60   ~ 0
Stick control
Text Notes 13600 5550 0    60   Italic 0
Option si utilisation du 3.3V dans le montage
Text Notes 6650 8150 0    60   Italic 0
Option Amplification des sticks (Fonctionne sans)
Text Notes 5400 6200 0    60   Italic 0
( Option si utilisation de la télémètrie )
$Comp
L L4941 U4
U 1 1 58D88F29
P 12950 3650
F 0 "U4" H 12800 3400 50  0000 C CNN
F 1 "L4941" H 12800 3850 50  0000 L CNN
F 2 "Power_Integrations:TO-220" H 13100 3950 50  0001 C CIN
F 3 "" H 12950 3650 50  0000 C CNN
	1    12950 3650
	1    0    0    -1  
$EndComp
Text Label 2300 5750 2    60   ~ 0
Xmitter_SCK
Text Label 2350 5450 2    60   ~ 0
Xmitter_MOSI
Text Label 650  5450 0    60   ~ 0
Xmitter_MISO
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R17
U 1 1 58A78C02
P 11800 4200
F 0 "R17" V 11880 4200 50  0000 C CNN
F 1 "5.1K" V 11800 4200 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 11730 4200 50  0001 C CNN
F 3 "" H 11800 4200 50  0000 C CNN
	1    11800 4200
	1    0    0    1   
$EndComp
$Comp
L CONN_01X02 P18
U 1 1 58EC21AF
P 15400 9150
F 0 "P18" H 15400 9300 50  0000 C CNN
F 1 "Ecolage" V 15550 9150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 15400 9150 50  0001 C CNN
F 3 "" H 15400 9150 50  0000 C CNN
	1    15400 9150
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR062
U 1 1 58EC22CA
P 15200 9550
F 0 "#PWR062" H 15200 9300 50  0001 C CNN
F 1 "GND" H 15200 9400 50  0000 C CNN
F 2 "" H 15200 9550 50  0000 C CNN
F 3 "" H 15200 9550 50  0000 C CNN
	1    15200 9550
	1    0    0    -1  
$EndComp
$Comp
L TLP290-RESCUE-Mega_2560_core_mini_full_2.0 U3
U 1 1 58EA0FF0
P 5550 7150
F 0 "U3" H 5350 7350 50  0000 L CNN
F 1 "TLP290" H 5550 7350 50  0000 L CNN
F 2 "anthoperso:SOP4" H 5350 6950 50  0000 L CIN
F 3 "" H 5575 7150 50  0000 L CNN
	1    5550 7150
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X08 P20
U 1 1 58EB2071
P 7300 6850
F 0 "P20" H 7300 7300 50  0000 C CNN
F 1 "TTL/RS232" V 7400 6850 50  0000 C CNN
F 2 "anthoperso:Con_RS232_TTL" H 7300 6850 50  0001 C CNN
F 3 "" H 7300 6850 50  0000 C CNN
	1    7300 6850
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR063
U 1 1 58EB259D
P 7050 7250
F 0 "#PWR063" H 7050 7000 50  0001 C CNN
F 1 "GND" H 7050 7100 50  0000 C CNN
F 2 "" H 7050 7250 50  0000 C CNN
F 3 "" H 7050 7250 50  0000 C CNN
	1    7050 7250
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR064
U 1 1 58EB26A2
P 7000 6450
F 0 "#PWR064" H 7000 6300 50  0001 C CNN
F 1 "VCC" H 7000 6600 50  0000 C CNN
F 2 "" H 7000 6450 50  0000 C CNN
F 3 "" H 7000 6450 50  0000 C CNN
	1    7000 6450
	1    0    0    -1  
$EndComp
$Comp
L TLP290-RESCUE-Mega_2560_core_mini_full_2.0 U2
U 1 1 58EA0B44
P 5550 6650
F 0 "U2" H 5350 6850 50  0000 L CNN
F 1 "TLP290" H 5550 6850 50  0000 L CNN
F 2 "anthoperso:SOP4" H 5350 6450 50  0000 L CIN
F 3 "" H 5575 6650 50  0000 L CNN
	1    5550 6650
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR065
U 1 1 58EE2BDA
P 13400 4000
F 0 "#PWR065" H 13400 3750 50  0001 C CNN
F 1 "GND" H 13400 3850 50  0000 C CNN
F 2 "" H 13400 4000 50  0000 C CNN
F 3 "" H 13400 4000 50  0000 C CNN
	1    13400 4000
	-1   0    0    -1  
$EndComp
$Comp
L CP1-RESCUE-Mega_2560_core_mini_full_2.0 C28
U 1 1 58EF0A99
P 12450 3800
F 0 "C28" H 12475 3900 50  0000 L CNN
F 1 "100µF" H 12475 3700 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_6.3x5.3" H 12450 3800 50  0001 C CNN
F 3 "" H 12450 3800 50  0000 C CNN
	1    12450 3800
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR066
U 1 1 58EF1316
P 12450 4000
F 0 "#PWR066" H 12450 3750 50  0001 C CNN
F 1 "GND" H 12450 3850 50  0000 C CNN
F 2 "" H 12450 4000 50  0000 C CNN
F 3 "" H 12450 4000 50  0000 C CNN
	1    12450 4000
	-1   0    0    -1  
$EndComp
Text Notes 8100 6250 0    60   ~ 0
Coupure Alim \n
$Comp
L CONN_JQ6500_28P_mini-RESCUE-Mega_2560_core_mini_full_2.0 P22
U 1 1 58EF95DD
P 5900 1350
F 0 "P22" H 6200 1400 50  0000 C CNN
F 1 "JQ6500_28P" V 5750 1350 50  0000 C CNN
F 2 "anthoperso:connector_mini-JQ6500-28P" H 5900 1350 50  0001 C CNN
F 3 "" H 5900 1350 50  0000 C CNN
	1    5900 1350
	1    0    0    -1  
$EndComp
$Comp
L MBR0520 D6
U 1 1 58F1A0C4
P 12150 3600
F 0 "D6" H 12150 3700 50  0000 C CNN
F 1 "MBR0520" H 12150 3500 50  0000 C CNN
F 2 "Diodes_SMD:D_SOD-123" H 12150 3425 50  0001 C CNN
F 3 "" H 12150 3600 50  0001 C CNN
	1    12150 3600
	-1   0    0    1   
$EndComp
$Comp
L MBR0520 D1
U 1 1 58F1A570
P 3450 3400
F 0 "D1" H 3450 3500 50  0000 C CNN
F 1 "MBR0520" H 3800 3500 50  0000 C CNN
F 2 "Diodes_SMD:D_SOD-123" H 3450 3225 50  0001 C CNN
F 3 "" H 3450 3400 50  0001 C CNN
	1    3450 3400
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C2
U 1 1 58A78C65
P 1250 2350
F 0 "C2" H 1275 2450 50  0000 L CNN
F 1 "100nF" H 1275 2250 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 1288 2200 50  0001 C CNN
F 3 "" H 1250 2350 50  0000 C CNN
	1    1250 2350
	0    1    1    0   
$EndComp
$Comp
L TEST_2P W1
U 1 1 58FAE3C5
P 5650 9400
F 0 "W1" H 5650 9460 50  0000 C CNN
F 1 "TEST_2P" H 5650 9330 50  0000 C CNN
F 2 "anthoperso:TESTPT" H 5650 9400 50  0001 C CNN
F 3 "" H 5650 9400 50  0000 C CNN
	1    5650 9400
	0    1    1    0   
$EndComp
$Comp
L TEST_2P W2
U 1 1 58FB1CAE
P 7350 9400
F 0 "W2" H 7350 9460 50  0000 C CNN
F 1 "TEST_2P" H 7350 9330 50  0000 C CNN
F 2 "anthoperso:TESTPT" H 7350 9400 50  0001 C CNN
F 3 "" H 7350 9400 50  0000 C CNN
	1    7350 9400
	0    1    1    0   
$EndComp
$Comp
L TEST_2P W3
U 1 1 58FB2B81
P 9050 9400
F 0 "W3" H 9050 9460 50  0000 C CNN
F 1 "TEST_2P" H 9050 9330 50  0000 C CNN
F 2 "anthoperso:TESTPT" H 9050 9400 50  0001 C CNN
F 3 "" H 9050 9400 50  0000 C CNN
	1    9050 9400
	0    1    1    0   
$EndComp
$Comp
L TEST_2P W4
U 1 1 58FB3674
P 10750 9450
F 0 "W4" H 10750 9510 50  0000 C CNN
F 1 "TEST_2P" H 10750 9380 50  0000 C CNN
F 2 "anthoperso:TESTPT" H 10750 9450 50  0001 C CNN
F 3 "" H 10750 9450 50  0000 C CNN
	1    10750 9450
	0    1    1    0   
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R9
U 1 1 58F91D67
P 3300 3600
F 0 "R9" V 3400 3600 50  0000 C CNN
F 1 "220" V 3300 3600 50  0000 C CNN
F 2 "anthoperso:R_1206" V 3230 3600 50  0001 C CNN
F 3 "" H 3300 3600 50  0000 C CNN
	1    3300 3600
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR067
U 1 1 5930E4B5
P 6700 3400
F 0 "#PWR067" H 6700 3150 50  0001 C CNN
F 1 "GND" H 6700 3250 50  0000 C CNN
F 2 "" H 6700 3400 50  0000 C CNN
F 3 "" H 6700 3400 50  0000 C CNN
	1    6700 3400
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR068
U 1 1 5932156A
P 8350 3400
F 0 "#PWR068" H 8350 3150 50  0001 C CNN
F 1 "GND" H 8350 3250 50  0000 C CNN
F 2 "" H 8350 3400 50  0000 C CNN
F 3 "" H 8350 3400 50  0000 C CNN
	1    8350 3400
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X10 P14
U 1 1 5932AA20
P 7900 3000
F 0 "P14" H 7900 3550 50  0000 C CNN
F 1 "Switch" V 8000 3000 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x05_Pitch2.54mm" H 7900 3000 50  0001 C CNN
F 3 "" H 7900 3000 50  0000 C CNN
	1    7900 3000
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X10 P11
U 1 1 5932C935
P 6250 3000
F 0 "P11" H 6250 3550 50  0000 C CNN
F 1 "Trim" V 6350 3000 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x05_Pitch2.54mm" H 6250 3000 50  0001 C CNN
F 3 "" H 6250 3000 50  0000 C CNN
	1    6250 3000
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR069
U 1 1 5932E166
P 14550 1200
F 0 "#PWR069" H 14550 950 50  0001 C CNN
F 1 "GND" H 14550 1050 50  0000 C CNN
F 2 "" H 14550 1200 50  0000 C CNN
F 3 "" H 14550 1200 50  0000 C CNN
	1    14550 1200
	1    0    0    -1  
$EndComp
$Comp
L POT-RESCUE-Mega_2560_core_mini_full_2.0 R27
U 1 1 5936EC81
P 5900 9150
F 0 "R27" V 5800 9150 50  0000 C CNN
F 1 "100K" V 5900 9150 50  0000 C CNN
F 2 "Potentiometers:Potentiometer_Trimmer_Bourns_3296W" H 5900 9150 50  0001 C CNN
F 3 "" H 5900 9150 50  0000 C CNN
	1    5900 9150
	-1   0    0    1   
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R11
U 1 1 5936EE51
P 5900 9600
F 0 "R11" V 5980 9600 50  0000 C CNN
F 1 "120K" V 5900 9600 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 5830 9600 50  0001 C CNN
F 3 "" H 5900 9600 50  0000 C CNN
	1    5900 9600
	1    0    0    -1  
$EndComp
$Comp
L POT-RESCUE-Mega_2560_core_mini_full_2.0 R30
U 1 1 593700CE
P 7600 9150
F 0 "R30" V 7500 9150 50  0000 C CNN
F 1 "100K" V 7600 9150 50  0000 C CNN
F 2 "Potentiometers:Potentiometer_Trimmer_Bourns_3296W" H 7600 9150 50  0001 C CNN
F 3 "" H 7600 9150 50  0000 C CNN
	1    7600 9150
	-1   0    0    1   
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R12
U 1 1 59370292
P 7600 9600
F 0 "R12" V 7680 9600 50  0000 C CNN
F 1 "120K" V 7600 9600 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 7530 9600 50  0001 C CNN
F 3 "" H 7600 9600 50  0000 C CNN
	1    7600 9600
	1    0    0    -1  
$EndComp
$Comp
L POT-RESCUE-Mega_2560_core_mini_full_2.0 R33
U 1 1 5937059E
P 9300 9150
F 0 "R33" V 9200 9150 50  0000 C CNN
F 1 "100K" V 9300 9150 50  0000 C CNN
F 2 "Potentiometers:Potentiometer_Trimmer_Bourns_3296W" H 9300 9150 50  0001 C CNN
F 3 "" H 9300 9150 50  0000 C CNN
	1    9300 9150
	-1   0    0    1   
$EndComp
$Comp
L POT-RESCUE-Mega_2560_core_mini_full_2.0 R36
U 1 1 5937076F
P 11000 9200
F 0 "R36" V 10900 9200 50  0000 C CNN
F 1 "100K" V 11000 9200 50  0000 C CNN
F 2 "Potentiometers:Potentiometer_Trimmer_Bourns_3296W" H 11000 9200 50  0001 C CNN
F 3 "" H 11000 9200 50  0000 C CNN
	1    11000 9200
	-1   0    0    1   
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R13
U 1 1 593709A3
P 9300 9600
F 0 "R13" V 9380 9600 50  0000 C CNN
F 1 "120K" V 9300 9600 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 9230 9600 50  0001 C CNN
F 3 "" H 9300 9600 50  0000 C CNN
	1    9300 9600
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R14
U 1 1 59370B0E
P 11000 9700
F 0 "R14" V 11080 9700 50  0000 C CNN
F 1 "120K" V 11000 9700 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 10930 9700 50  0001 C CNN
F 3 "" H 11000 9700 50  0000 C CNN
	1    11000 9700
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C4
U 1 1 593C2136
P 4450 9250
F 0 "C4" H 4475 9350 50  0000 L CNN
F 1 "100nF" H 4475 9150 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 4488 9100 50  0001 C CNN
F 3 "" H 4450 9250 50  0000 C CNN
	1    4450 9250
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR070
U 1 1 593C3872
P 4450 9500
F 0 "#PWR070" H 4450 9250 50  0001 C CNN
F 1 "GND" H 4450 9300 50  0000 C CNN
F 2 "" H 4450 9500 50  0000 C CNN
F 3 "" H 4450 9500 50  0000 C CNN
	1    4450 9500
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C6
U 1 1 593DDC8A
P 2900 2300
F 0 "C6" H 2950 2400 50  0000 L CNN
F 1 "100nF" H 2950 2200 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 2938 2150 50  0001 C CNN
F 3 "" H 2900 2300 50  0000 C CNN
	1    2900 2300
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR071
U 1 1 593E1AF9
P 2900 2500
F 0 "#PWR071" H 2900 2250 50  0001 C CNN
F 1 "GND" H 2900 2350 50  0000 C CNN
F 2 "" H 2900 2500 50  0000 C CNN
F 3 "" H 2900 2500 50  0000 C CNN
	1    2900 2500
	1    0    0    -1  
$EndComp
NoConn ~ 11400 3600
NoConn ~ 11300 3600
$Comp
L CP1-RESCUE-Mega_2560_core_mini_full_2.0 C7
U 1 1 593F794B
P 13800 3800
F 0 "C7" H 13825 3900 50  0000 L CNN
F 1 "100µF" H 13825 3700 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_6.3x5.3" H 13800 3800 50  0001 C CNN
F 3 "" H 13800 3800 50  0000 C CNN
	1    13800 3800
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR072
U 1 1 593F876D
P 13800 4050
F 0 "#PWR072" H 13800 3800 50  0001 C CNN
F 1 "GND" H 13800 3900 50  0000 C CNN
F 2 "" H 13800 4050 50  0000 C CNN
F 3 "" H 13800 4050 50  0000 C CNN
	1    13800 4050
	-1   0    0    -1  
$EndComp
$Comp
L CP1-RESCUE-Mega_2560_core_mini_full_2.0 C8
U 1 1 593F8B53
P 15000 4900
F 0 "C8" H 15025 5000 50  0000 L CNN
F 1 "47µF" H 15025 4800 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_6.3x5.3" H 15000 4900 50  0001 C CNN
F 3 "" H 15000 4900 50  0000 C CNN
	1    15000 4900
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR073
U 1 1 593F91DA
P 15000 5150
F 0 "#PWR073" H 15000 4900 50  0001 C CNN
F 1 "GND" H 15000 4950 50  0000 C CNN
F 2 "" H 15000 5150 50  0000 C CNN
F 3 "" H 15000 5150 50  0000 C CNN
	1    15000 5150
	1    0    0    -1  
$EndComp
Text Notes 11000 3300 0    60   ~ 0
Power Switch
$Comp
L CONN_01X02 P27
U 1 1 595556C6
P 5400 750
F 0 "P27" H 5400 900 50  0000 C CNN
F 1 "HP" V 5550 750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 5400 750 50  0001 C CNN
F 3 "" H 5400 750 50  0000 C CNN
	1    5400 750 
	-1   0    0    1   
$EndComp
$Comp
L POT-RESCUE-Mega_2560_core_mini_full_2.0 RV6
U 1 1 59568790
P 10500 1650
F 0 "RV6" V 10325 1650 50  0000 C CNN
F 1 "22K" V 10400 1650 50  0000 C CNN
F 2 "Potentiometers:Potentiometer_Trimmer_Bourns_3296W" H 10500 1650 50  0001 C CNN
F 3 "" H 10500 1650 50  0000 C CNN
	1    10500 1650
	1    0    0    -1  
$EndComp
$Comp
L POT-RESCUE-Mega_2560_core_mini_full_2.0 RV1
U 1 1 5956AF2E
P 9450 1650
F 0 "RV1" V 9275 1650 50  0000 C CNN
F 1 "220K" V 9350 1650 50  0000 C CNN
F 2 "Potentiometers:Potentiometer_Trimmer_Bourns_3296W" H 9450 1650 50  0001 C CNN
F 3 "" H 9450 1650 50  0000 C CNN
	1    9450 1650
	-1   0    0    1   
$EndComp
Text Label 6500 6600 0    60   ~ 0
TX_module
Text Label 6500 6500 0    60   ~ 0
RX_module
Text Label 8800 7300 1    60   ~ 0
RX_module
Text Label 8700 7300 1    60   ~ 0
TX_module
$Comp
L CONN_01X03 P28
U 1 1 595AE5EC
P 3700 10350
F 0 "P28" H 3700 10550 50  0000 C CNN
F 1 "5v/3,3v manches" V 3800 10400 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 3700 10350 50  0001 C CNN
F 3 "" H 3700 10350 50  0000 C CNN
	1    3700 10350
	0    -1   -1   0   
$EndComp
Text Label 14700 4600 0    60   ~ 0
P3_3V
$Comp
L VCC #PWR074
U 1 1 595B6C62
P 3600 10650
F 0 "#PWR074" H 3600 10500 50  0001 C CNN
F 1 "VCC" H 3600 10800 50  0000 C CNN
F 2 "" H 3600 10650 50  0000 C CNN
F 3 "" H 3600 10650 50  0000 C CNN
	1    3600 10650
	-1   0    0    1   
$EndComp
Text Label 1950 10850 1    60   ~ 0
Power_manches
Text Label 3800 10950 1    60   ~ 0
P3_3V
Text Label 3700 6650 0    60   ~ 0
VREF
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C10
U 1 1 595CDA49
P 3950 6850
F 0 "C10" H 3975 6950 50  0000 L CNN
F 1 "100nF" H 3975 6750 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 3988 6700 50  0001 C CNN
F 3 "" H 3950 6850 50  0000 C CNN
	1    3950 6850
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR075
U 1 1 595CF4F9
P 3950 7050
F 0 "#PWR075" H 3950 6800 50  0001 C CNN
F 1 "GND" H 3950 6900 50  0000 C CNN
F 2 "" H 3950 7050 50  0000 C CNN
F 3 "" H 3950 7050 50  0000 C CNN
	1    3950 7050
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR076
U 1 1 595FDAAF
P 2600 2150
F 0 "#PWR076" H 2600 1900 50  0001 C CNN
F 1 "GND" H 2600 2000 50  0000 C CNN
F 2 "" H 2600 2150 50  0000 C CNN
F 3 "" H 2600 2150 50  0000 C CNN
	1    2600 2150
	1    0    0    -1  
$EndComp
Text Label 2000 2650 1    60   ~ 0
CS_CYRF6936
Text Label 1800 2650 1    60   ~ 0
CS_CC2500
NoConn ~ 2100 7300
$Comp
L CONN_01X06 P13
U 1 1 5969F6BA
P 3250 9150
F 0 "P13" H 3250 9500 50  0000 C CNN
F 1 "Stick_right" V 3350 9150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06_Pitch2.54mm" H 3250 9150 50  0001 C CNN
F 3 "" H 3250 9150 50  0000 C CNN
	1    3250 9150
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X06 P12
U 1 1 596A22F2
P 2200 9150
F 0 "P12" H 2200 9500 50  0000 C CNN
F 1 "Stick_left" V 2300 9150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03_Pitch2.54mm" H 2200 9150 50  0001 C CNN
F 3 "" H 2200 9150 50  0000 C CNN
	1    2200 9150
	0    -1   -1   0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C12
U 1 1 596A0908
P 2350 4000
F 0 "C12" H 2375 4100 50  0000 L CNN
F 1 "100nF" H 2375 3900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 2388 3850 50  0001 C CNN
F 3 "" H 2350 4000 50  0000 C CNN
	1    2350 4000
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C13
U 1 1 596A4430
P 1700 4000
F 0 "C13" H 1725 4100 50  0000 L CNN
F 1 "100nF" H 1725 3900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 1738 3850 50  0001 C CNN
F 3 "" H 1700 4000 50  0000 C CNN
	1    1700 4000
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C14
U 1 1 596A45A1
P 1100 4000
F 0 "C14" H 1125 4100 50  0000 L CNN
F 1 "100nF" H 1125 3900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 1138 3850 50  0001 C CNN
F 3 "" H 1100 4000 50  0000 C CNN
	1    1100 4000
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR077
U 1 1 596A575F
P 2350 4200
F 0 "#PWR077" H 2350 3950 50  0001 C CNN
F 1 "GND" H 2350 4050 50  0000 C CNN
F 2 "" H 2350 4200 50  0000 C CNN
F 3 "" H 2350 4200 50  0000 C CNN
	1    2350 4200
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR078
U 1 1 596A58C6
P 1700 4200
F 0 "#PWR078" H 1700 3950 50  0001 C CNN
F 1 "GND" H 1700 4050 50  0000 C CNN
F 2 "" H 1700 4200 50  0000 C CNN
F 3 "" H 1700 4200 50  0000 C CNN
	1    1700 4200
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR079
U 1 1 596A5A2D
P 1100 4200
F 0 "#PWR079" H 1100 3950 50  0001 C CNN
F 1 "GND" H 1100 4050 50  0000 C CNN
F 2 "" H 1100 4200 50  0000 C CNN
F 3 "" H 1100 4200 50  0000 C CNN
	1    1100 4200
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR080
U 1 1 596ACF82
P 3850 9700
F 0 "#PWR080" H 3850 9450 50  0001 C CNN
F 1 "GND" H 3850 9550 50  0000 C CNN
F 2 "" H 3850 9700 50  0000 C CNN
F 3 "" H 3850 9700 50  0000 C CNN
	1    3850 9700
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR081
U 1 1 596AD0E9
P 3850 10000
F 0 "#PWR081" H 3850 9750 50  0001 C CNN
F 1 "GND" H 3850 9850 50  0000 C CNN
F 2 "" H 3850 10000 50  0000 C CNN
F 3 "" H 3850 10000 50  0000 C CNN
	1    3850 10000
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR082
U 1 1 596AEB21
P 2800 9700
F 0 "#PWR082" H 2800 9450 50  0001 C CNN
F 1 "GND" H 2800 9550 50  0000 C CNN
F 2 "" H 2800 9700 50  0000 C CNN
F 3 "" H 2800 9700 50  0000 C CNN
	1    2800 9700
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR083
U 1 1 596AEC88
P 2800 10000
F 0 "#PWR083" H 2800 9750 50  0001 C CNN
F 1 "GND" H 2800 9850 50  0000 C CNN
F 2 "" H 2800 10000 50  0000 C CNN
F 3 "" H 2800 10000 50  0000 C CNN
	1    2800 10000
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X08 P30
U 1 1 59708367
P 9900 4750
F 0 "P30" H 9900 5200 50  0000 C CNN
F 1 "MiniDCDC3A" V 10000 4750 50  0000 C CNN
F 2 "MiniDCDC3A" H 9900 4750 50  0001 C CNN
F 3 "" H 9900 4750 50  0000 C CNN
	1    9900 4750
	-1   0    0    1   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR084
U 1 1 5970AC32
P 10150 5150
F 0 "#PWR084" H 10150 4900 50  0001 C CNN
F 1 "GND" H 10150 5000 50  0000 C CNN
F 2 "" H 10150 5150 50  0000 C CNN
F 3 "" H 10150 5150 50  0000 C CNN
	1    10150 5150
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR085
U 1 1 5970FC7E
P 10350 4500
F 0 "#PWR085" H 10350 4350 50  0001 C CNN
F 1 "VCC" H 10350 4600 50  0000 C CNN
F 2 "" H 10350 4500 50  0000 C CNN
F 3 "" H 10350 4500 50  0000 C CNN
	1    10350 4500
	1    0    0    -1  
$EndComp
Text Label 10700 3600 0    60   ~ 0
LIPO+
$Comp
L CONN_01X04 P8
U 1 1 59715B3E
P 4700 700
F 0 "P8" H 4700 950 50  0000 C CNN
F 1 "Audio" V 4800 700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch2.54mm" H 4700 700 50  0001 C CNN
F 3 "" H 4700 700 50  0000 C CNN
	1    4700 700 
	0    -1   -1   0   
$EndComp
Text Notes 10300 5100 0    60   ~ 0
DCDC_IN-
Text Label 12450 3550 1    60   ~ 0
DCDC_IN+
Text Label 10300 4900 0    60   ~ 0
DCDC_IN+
$Comp
L CONN_01X02 P17
U 1 1 597235EC
P 10350 3650
F 0 "P17" H 10350 3800 50  0000 C CNN
F 1 "BATT" V 10450 3650 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 10350 3650 50  0001 C CNN
F 3 "" H 10350 3650 50  0000 C CNN
	1    10350 3650
	-1   0    0    1   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR086
U 1 1 597240B5
P 10650 3800
F 0 "#PWR086" H 10650 3550 50  0001 C CNN
F 1 "GND" H 10650 3650 50  0000 C CNN
F 2 "" H 10650 3800 50  0000 C CNN
F 3 "" H 10650 3800 50  0000 C CNN
	1    10650 3800
	1    0    0    -1  
$EndComp
Text Notes 9700 5550 0    60   Italic 0
Option module DCDC 3A
Text Label 2350 4950 2    60   ~ 0
Rot_1push
$Comp
L CONN_01X03 P32
U 1 1 5988242B
P 1000 3000
F 0 "P32" H 1000 3200 50  0000 C CNN
F 1 "Pot1" V 1100 3000 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 1000 3000 50  0001 C CNN
F 3 "" H 1000 3000 50  0000 C CNN
	1    1000 3000
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X03 P31
U 1 1 5988261A
P 1600 3000
F 0 "P31" H 1600 3200 50  0000 C CNN
F 1 "Pot2" V 1700 3000 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 1600 3000 50  0001 C CNN
F 3 "" H 1600 3000 50  0000 C CNN
	1    1600 3000
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X03 P4
U 1 1 598827BA
P 2250 3000
F 0 "P4" H 2250 3200 50  0000 C CNN
F 1 "Pot3" V 2350 3000 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03_Pitch2.54mm" H 2250 3000 50  0001 C CNN
F 3 "" H 2250 3000 50  0000 C CNN
	1    2250 3000
	0    -1   -1   0   
$EndComp
$Comp
L VCC #PWR087
U 1 1 598829FE
P 2150 3300
F 0 "#PWR087" H 2150 3150 50  0001 C CNN
F 1 "VCC" H 2150 3450 50  0000 C CNN
F 2 "" H 2150 3300 50  0000 C CNN
F 3 "" H 2150 3300 50  0000 C CNN
	1    2150 3300
	-1   0    0    1   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR088
U 1 1 59882B74
P 2350 3300
F 0 "#PWR088" H 2350 3050 50  0001 C CNN
F 1 "GND" H 2350 3150 50  0000 C CNN
F 2 "" H 2350 3300 50  0000 C CNN
F 3 "" H 2350 3300 50  0000 C CNN
	1    2350 3300
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR089
U 1 1 59885008
P 1500 3300
F 0 "#PWR089" H 1500 3150 50  0001 C CNN
F 1 "VCC" H 1500 3450 50  0000 C CNN
F 2 "" H 1500 3300 50  0000 C CNN
F 3 "" H 1500 3300 50  0000 C CNN
	1    1500 3300
	-1   0    0    1   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR090
U 1 1 5988517E
P 1700 3300
F 0 "#PWR090" H 1700 3050 50  0001 C CNN
F 1 "GND" H 1700 3150 50  0000 C CNN
F 2 "" H 1700 3300 50  0000 C CNN
F 3 "" H 1700 3300 50  0000 C CNN
	1    1700 3300
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR091
U 1 1 59885F41
P 900 3300
F 0 "#PWR091" H 900 3150 50  0001 C CNN
F 1 "VCC" H 900 3450 50  0000 C CNN
F 2 "" H 900 3300 50  0000 C CNN
F 3 "" H 900 3300 50  0000 C CNN
	1    900  3300
	-1   0    0    1   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR092
U 1 1 598860B7
P 1100 3300
F 0 "#PWR092" H 1100 3050 50  0001 C CNN
F 1 "GND" H 1100 3150 50  0000 C CNN
F 2 "" H 1100 3300 50  0000 C CNN
F 3 "" H 1100 3300 50  0000 C CNN
	1    1100 3300
	1    0    0    -1  
$EndComp
Text Notes 4650 2200 0    60   ~ 0
Buzzer
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R10
U 1 1 598C359B
P 7300 1500
F 0 "R10" V 7380 1500 50  0000 C CNN
F 1 "22K" V 7300 1500 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 7230 1500 50  0001 C CNN
F 3 "" H 7300 1500 50  0000 C CNN
	1    7300 1500
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR093
U 1 1 598C3F24
P 7300 1750
F 0 "#PWR093" H 7300 1500 50  0001 C CNN
F 1 "GND" H 7300 1600 50  0000 C CNN
F 2 "" H 7300 1750 50  0000 C CNN
F 3 "" H 7300 1750 50  0000 C CNN
	1    7300 1750
	1    0    0    -1  
$EndComp
$Comp
L DIODE D3
U 1 1 598C396B
P 12150 4250
F 0 "D3" H 12150 4350 40  0000 C CNN
F 1 "LL4148" H 12150 4150 40  0000 C CNN
F 2 "D_SCD80_HandSoldering" H 12150 4250 60  0001 C CNN
F 3 "" H 12150 4250 60  0000 C CNN
	1    12150 4250
	0    -1   -1   0   
$EndComp
$Comp
L DIODE D4
U 1 1 598C4010
P 12150 4800
F 0 "D4" H 12150 4900 40  0000 C CNN
F 1 "LL4148" H 12150 4700 40  0000 C CNN
F 2 "D_SCD80_HandSoldering" H 12150 4800 60  0001 C CNN
F 3 "" H 12150 4800 60  0000 C CNN
	1    12150 4800
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR094
U 1 1 598C41CD
P 12150 5050
F 0 "#PWR094" H 12150 4800 50  0001 C CNN
F 1 "GND" H 12150 4900 50  0000 C CNN
F 2 "" H 12150 5050 50  0000 C CNN
F 3 "" H 12150 5050 50  0000 C CNN
	1    12150 5050
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR095
U 1 1 598C59DE
P 12150 4000
F 0 "#PWR095" H 12150 3850 50  0001 C CNN
F 1 "VCC" H 12150 4150 50  0000 C CNN
F 2 "" H 12150 4000 50  0000 C CNN
F 3 "" H 12150 4000 50  0000 C CNN
	1    12150 4000
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C18
U 1 1 599094ED
P 3300 7550
F 0 "C18" H 3325 7650 50  0000 L CNN
F 1 "100nF" H 3325 7450 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 3338 7400 50  0001 C CNN
F 3 "" H 3300 7550 50  0000 C CNN
	1    3300 7550
	0    1    1    0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR096
U 1 1 59909EED
P 3550 7550
F 0 "#PWR096" H 3550 7300 50  0001 C CNN
F 1 "GND" H 3550 7400 50  0000 C CNN
F 2 "" H 3550 7550 50  0000 C CNN
F 3 "" H 3550 7550 50  0000 C CNN
	1    3550 7550
	0    -1   -1   0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C19
U 1 1 5990ADA5
P 4000 4700
F 0 "C19" V 3950 4800 50  0000 L CNN
F 1 "100nF" V 3950 4400 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 4038 4550 50  0001 C CNN
F 3 "" H 4000 4700 50  0000 C CNN
	1    4000 4700
	0    1    1    0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C15
U 1 1 5990AF96
P 1000 4700
F 0 "C15" V 1050 4800 50  0000 L CNN
F 1 "100nF" V 1050 4400 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 1038 4550 50  0001 C CNN
F 3 "" H 1000 4700 50  0000 C CNN
	1    1000 4700
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR097
U 1 1 5990C006
P 800 4700
F 0 "#PWR097" H 800 4450 50  0001 C CNN
F 1 "GND" H 800 4550 50  0000 C CNN
F 2 "" H 800 4700 50  0000 C CNN
F 3 "" H 800 4700 50  0000 C CNN
	1    800  4700
	0    1    1    0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR098
U 1 1 5990D282
P 4250 4700
F 0 "#PWR098" H 4250 4450 50  0001 C CNN
F 1 "GND" V 4250 4500 50  0000 C CNN
F 2 "" H 4250 4700 50  0000 C CNN
F 3 "" H 4250 4700 50  0000 C CNN
	1    4250 4700
	0    -1   -1   0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C20
U 1 1 5990F9CC
P 5850 2300
F 0 "C20" H 5875 2400 50  0000 L CNN
F 1 "100nF" H 5875 2200 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 5888 2150 50  0001 C CNN
F 3 "" H 5850 2300 50  0000 C CNN
	1    5850 2300
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C21
U 1 1 599102FD
P 6500 2300
F 0 "C21" H 6525 2400 50  0000 L CNN
F 1 "100nF" H 6525 2200 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 6538 2150 50  0001 C CNN
F 3 "" H 6500 2300 50  0000 C CNN
	1    6500 2300
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR099
U 1 1 599110A4
P 5850 2500
F 0 "#PWR099" H 5850 2250 50  0001 C CNN
F 1 "GND" H 5850 2350 50  0000 C CNN
F 2 "" H 5850 2500 50  0000 C CNN
F 3 "" H 5850 2500 50  0000 C CNN
	1    5850 2500
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0100
U 1 1 5991122F
P 6500 2500
F 0 "#PWR0100" H 6500 2250 50  0001 C CNN
F 1 "GND" H 6500 2350 50  0000 C CNN
F 2 "" H 6500 2500 50  0000 C CNN
F 3 "" H 6500 2500 50  0000 C CNN
	1    6500 2500
	1    0    0    -1  
$EndComp
$Comp
L 24C512 U7
U 1 1 5992C41F
P 13900 6800
F 0 "U7" H 14050 7150 60  0000 C CNN
F 1 "FM24W256" H 14150 6450 60  0000 C CNN
F 2 "SOIC-8-FRAM_DI" H 13900 6800 60  0001 C CNN
F 3 "" H 13900 6800 60  0001 C CNN
	1    13900 6800
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0101
U 1 1 5992FA64
P 13900 7400
F 0 "#PWR0101" H 13900 7150 50  0001 C CNN
F 1 "GND" H 13900 7250 50  0000 C CNN
F 2 "" H 13900 7400 50  0000 C CNN
F 3 "" H 13900 7400 50  0000 C CNN
	1    13900 7400
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR0102
U 1 1 5992FBF2
P 13900 6100
F 0 "#PWR0102" H 13900 5950 50  0001 C CNN
F 1 "VCC" H 13900 6250 50  0000 C CNN
F 2 "" H 13900 6100 50  0000 C CNN
F 3 "" H 13900 6100 50  0000 C CNN
	1    13900 6100
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C22
U 1 1 59931093
P 14150 6200
F 0 "C22" H 14175 6300 50  0000 L CNN
F 1 "100nF" H 14175 6100 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 14188 6050 50  0001 C CNN
F 3 "" H 14150 6200 50  0000 C CNN
	1    14150 6200
	0    -1   -1   0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0103
U 1 1 5993109F
P 14350 6200
F 0 "#PWR0103" H 14350 5950 50  0001 C CNN
F 1 "GND" H 14350 6050 50  0000 C CNN
F 2 "" H 14350 6200 50  0000 C CNN
F 3 "" H 14350 6200 50  0000 C CNN
	1    14350 6200
	0    -1   -1   0   
$EndComp
Text Label 14850 6900 2    60   ~ 0
Scl
Text Label 14850 7000 2    60   ~ 0
Sda
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0104
U 1 1 59936A0A
P 14700 6700
F 0 "#PWR0104" H 14700 6450 50  0001 C CNN
F 1 "GND" H 14700 6550 50  0000 C CNN
F 2 "" H 14700 6700 50  0000 C CNN
F 3 "" H 14700 6700 50  0000 C CNN
	1    14700 6700
	0    -1   -1   0   
$EndComp
Text Label 9750 6600 0    60   ~ 0
SCL
Text Label 9750 6700 0    60   ~ 0
SDA
$Comp
L DS3231 U8
U 1 1 599350DA
P 10800 6900
F 0 "U8" H 10800 7400 60  0000 C CNN
F 1 "DS3231SN" H 10800 6400 60  0000 C CNN
F 2 "SO-16-DS3231" H 10750 6850 60  0001 C CNN
F 3 "" H 10750 6850 60  0001 C CNN
	1    10800 6900
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0105
U 1 1 59935ABE
P 10200 7350
F 0 "#PWR0105" H 10200 7100 50  0001 C CNN
F 1 "GND" H 10200 7200 50  0000 C CNN
F 2 "" H 10200 7350 50  0000 C CNN
F 3 "" H 10200 7350 50  0000 C CNN
	1    10200 7350
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0106
U 1 1 5993715D
P 11400 7350
F 0 "#PWR0106" H 11400 7100 50  0001 C CNN
F 1 "GND" H 11400 7200 50  0000 C CNN
F 2 "" H 11400 7350 50  0000 C CNN
F 3 "" H 11400 7350 50  0000 C CNN
	1    11400 7350
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0107
U 1 1 5993A60D
P 11750 7550
F 0 "#PWR0107" H 11750 7300 50  0001 C CNN
F 1 "GND" H 11750 7400 50  0000 C CNN
F 2 "" H 11750 7550 50  0000 C CNN
F 3 "" H 11750 7550 50  0000 C CNN
	1    11750 7550
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C23
U 1 1 5993B608
P 12000 6200
F 0 "C23" H 12025 6300 50  0000 L CNN
F 1 "100nF" H 12025 6100 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 12038 6050 50  0001 C CNN
F 3 "" H 12000 6200 50  0000 C CNN
	1    12000 6200
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR0108
U 1 1 5993B60E
P 12000 6000
F 0 "#PWR0108" H 12000 5850 50  0001 C CNN
F 1 "VCC" H 12000 6150 50  0000 C CNN
F 2 "" H 12000 6000 50  0000 C CNN
F 3 "" H 12000 6000 50  0000 C CNN
	1    12000 6000
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0109
U 1 1 5993B614
P 12000 6400
F 0 "#PWR0109" H 12000 6150 50  0001 C CNN
F 1 "GND" H 12000 6250 50  0000 C CNN
F 2 "" H 12000 6400 50  0000 C CNN
F 3 "" H 12000 6400 50  0000 C CNN
	1    12000 6400
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR0110
U 1 1 5993C20F
P 11700 6500
F 0 "#PWR0110" H 11700 6350 50  0001 C CNN
F 1 "VCC" H 11700 6650 50  0000 C CNN
F 2 "" H 11700 6500 50  0000 C CNN
F 3 "" H 11700 6500 50  0000 C CNN
	1    11700 6500
	0    1    1    0   
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R15
U 1 1 5993A10C
P 11350 6200
F 0 "R15" V 11430 6200 50  0000 C CNN
F 1 "10K" V 11350 6200 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 11280 6200 50  0001 C CNN
F 3 "" H 11350 6200 50  0000 C CNN
	1    11350 6200
	-1   0    0    1   
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R20
U 1 1 5993AEBC
P 11550 6200
F 0 "R20" V 11630 6200 50  0000 C CNN
F 1 "10K" V 11550 6200 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 11480 6200 50  0001 C CNN
F 3 "" H 11550 6200 50  0000 C CNN
	1    11550 6200
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR0111
U 1 1 5993BDA3
P 11550 6000
F 0 "#PWR0111" H 11550 5850 50  0001 C CNN
F 1 "VCC" H 11550 6150 50  0000 C CNN
F 2 "" H 11550 6000 50  0000 C CNN
F 3 "" H 11550 6000 50  0000 C CNN
	1    11550 6000
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR0112
U 1 1 5993BF43
P 11350 6000
F 0 "#PWR0112" H 11350 5850 50  0001 C CNN
F 1 "VCC" H 11350 6150 50  0000 C CNN
F 2 "" H 11350 6000 50  0000 C CNN
F 3 "" H 11350 6000 50  0000 C CNN
	1    11350 6000
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0113
U 1 1 59948315
P 13100 6900
F 0 "#PWR0113" H 13100 6650 50  0001 C CNN
F 1 "GND" H 13100 6750 50  0000 C CNN
F 2 "" H 13100 6900 50  0000 C CNN
F 3 "" H 13100 6900 50  0000 C CNN
	1    13100 6900
	1    0    0    -1  
$EndComp
$Comp
L BATTERY BT1
U 1 1 5995BECE
P 11750 7150
F 0 "BT1" H 11750 6950 50  0000 C CNN
F 1 "CR2032" H 11750 7350 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 11750 7150 60  0001 C CNN
F 3 "" H 11750 7150 60  0000 C CNN
	1    11750 7150
	0    1    1    0   
$EndComp
Text Notes 9700 5900 0    60   Italic 0
Clock
Text Notes 12850 5900 0    60   Italic 0
Fram
$Comp
L 74LVC1G3157 U1
U 1 1 59968D98
P 13650 8800
F 0 "U1" H 13650 8400 60  0000 C CNN
F 1 "74LVC1G3157GV" H 13650 8300 60  0000 C CNN
F 2 "SOT-457" H 13650 8800 60  0001 C CNN
F 3 "" H 13650 8800 60  0000 C CNN
	1    13650 8800
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR0114
U 1 1 59971A86
P 14350 8800
F 0 "#PWR0114" H 14350 8650 50  0001 C CNN
F 1 "VCC" H 14350 8950 50  0000 C CNN
F 2 "" H 14350 8800 50  0000 C CNN
F 3 "" H 14350 8800 50  0000 C CNN
	1    14350 8800
	0    1    1    0   
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0115
U 1 1 599753B8
P 12900 8800
F 0 "#PWR0115" H 12900 8550 50  0001 C CNN
F 1 "GND" H 12900 8650 50  0000 C CNN
F 2 "" H 12900 8800 50  0000 C CNN
F 3 "" H 12900 8800 50  0000 C CNN
	1    12900 8800
	0    1    1    0   
$EndComp
Text Label 14700 8500 2    60   ~ 0
Sim_control
Text Label 12350 8500 0    60   ~ 0
PPM_in
Text Label 12350 9100 0    60   ~ 0
PPM16_SIMout
Text Label 11900 9350 0    60   ~ 0
Master_PPMout
$Comp
L +BATT #PWR0116
U 1 1 59987638
P 11800 3400
F 0 "#PWR0116" H 11800 3250 50  0001 C CNN
F 1 "+BATT" H 11800 3540 50  0000 C CNN
F 2 "" H 11800 3400 50  0000 C CNN
F 3 "" H 11800 3400 50  0000 C CNN
	1    11800 3400
	1    0    0    -1  
$EndComp
$Comp
L 4050-RESCUE-Mega_2560_core_mini_full_2.0 U9
U 6 1 599860D1
P 7600 5400
F 0 "U9" H 7795 5515 60  0000 C CNN
F 1 "4050" H 7790 5275 60  0000 C CNN
F 2 "SO-16-N" H 7600 5400 60  0001 C CNN
F 3 "" H 7600 5400 60  0001 C CNN
	6    7600 5400
	1    0    0    -1  
$EndComp
$Comp
L 4050-RESCUE-Mega_2560_core_mini_full_2.0 U9
U 2 1 59986657
P 5750 5000
F 0 "U9" H 5945 5115 60  0000 C CNN
F 1 "4050" H 5940 4875 60  0000 C CNN
F 2 "SO-16-N" H 5750 5000 60  0001 C CNN
F 3 "" H 5750 5000 60  0001 C CNN
	2    5750 5000
	1    0    0    -1  
$EndComp
$Comp
L 4050-RESCUE-Mega_2560_core_mini_full_2.0 U9
U 3 1 599868D6
P 5750 5400
F 0 "U9" H 5945 5515 60  0000 C CNN
F 1 "4050" H 5940 5275 60  0000 C CNN
F 2 "SO-16-N" H 5750 5400 60  0001 C CNN
F 3 "" H 5750 5400 60  0001 C CNN
	3    5750 5400
	1    0    0    -1  
$EndComp
$Comp
L 4050-RESCUE-Mega_2560_core_mini_full_2.0 U9
U 4 1 59986A55
P 7600 4600
F 0 "U9" H 7795 4715 60  0000 C CNN
F 1 "4050" H 7790 4475 60  0000 C CNN
F 2 "SO-16-N" H 7600 4600 60  0001 C CNN
F 3 "" H 7600 4600 60  0001 C CNN
	4    7600 4600
	1    0    0    -1  
$EndComp
$Comp
L 4050-RESCUE-Mega_2560_core_mini_full_2.0 U9
U 5 1 59986BBC
P 7600 5000
F 0 "U9" H 7795 5115 60  0000 C CNN
F 1 "4050" H 7790 4875 60  0000 C CNN
F 2 "SO-16-N" H 7600 5000 60  0001 C CNN
F 3 "" H 7600 5000 60  0001 C CNN
	5    7600 5000
	1    0    0    -1  
$EndComp
$Comp
L 4050-RESCUE-Mega_2560_core_mini_full_2.0 U9
U 1 1 59986D0D
P 5750 4650
F 0 "U9" H 5945 4765 60  0000 C CNN
F 1 "4050" H 5940 4525 60  0000 C CNN
F 2 "SO-16-N" H 5750 4650 60  0001 C CNN
F 3 "" H 5750 4650 60  0001 C CNN
	1    5750 4650
	1    0    0    -1  
$EndComp
Text Notes 8000 6100 0    60   ~ 0
Module HF
Text Label 750  5050 0    60   ~ 0
Rot_2push
Text Label 2550 5750 0    60   ~ 0
CS_CYRF6936
Text Label 4300 5750 2    60   ~ 0
CS_CC2500
Text Label 2550 5650 0    60   ~ 0
CS_A7105
Text Label 4300 5650 2    60   ~ 0
CS_NRF24L01
Text Label 2200 6850 3    60   ~ 0
PPM_in
Text Label 650  5350 0    60   ~ 0
JQ6500Busy
Text Label 2300 5850 2    60   ~ 0
Hold_Power
$Comp
L 2560-core P1
U 1 1 58A78C17
P -550 6000
F 0 "P1" H 1500 7250 50  0000 C CNN
F 1 "2560-core" V 1500 6200 50  0000 C CNN
F 2 "anthoperso:Connectors-2560-core" H 1500 5250 50  0001 C CNN
F 3 "" H 1500 5250 50  0000 C CNN
	1    -550 6000
	1    0    0    -1  
$EndComp
Text Label 750  5850 0    60   ~ 0
Vibreur
Text Label 2350 5950 2    60   ~ 0
Sim_Control
$Comp
L VCC #PWR0117
U 1 1 599D95CE
P 2700 2300
F 0 "#PWR0117" H 2700 2150 50  0001 C CNN
F 1 "VCC" V 2700 2450 50  0000 C CNN
F 2 "" H 2700 2300 50  0000 C CNN
F 3 "" H 2700 2300 50  0000 C CNN
	1    2700 2300
	-1   0    0    1   
$EndComp
Text Label 2100 2650 1    60   ~ 0
CS_A7105
Text Label 1900 2650 1    60   ~ 0
CS_NRF24L01
Text Notes 1800 1800 0    60   ~ 0
(Module Interne\nMulti Protocoles)
$Comp
L CONN_01X10 P7
U 1 1 599D5917
P 2250 1850
F 0 "P7" H 2250 2400 50  0000 C CNN
F 1 "SPIX" V 2250 1250 50  0000 C CNN
F 2 "Pin_Header_Straight_2x05_Pitch2.54mm" H 2250 1850 50  0001 C CNN
F 3 "" H 2250 1850 50  0000 C CNN
	1    2250 1850
	0    -1   -1   0   
$EndComp
Text Label 2400 2300 1    60   ~ 0
P3_3V
$Comp
L CONN_01X04 P10
U 1 1 596E2CEC
P 3800 750
F 0 "P10" H 3800 1000 50  0000 C CNN
F 1 "RX1TX1" V 3900 750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch2.54mm" H 3800 750 50  0001 C CNN
F 3 "" H 3800 750 50  0000 C CNN
	1    3800 750 
	0    -1   -1   0   
$EndComp
$Comp
L C-RESCUE-Mega_2560_core_mini_full_2.0 C24
U 1 1 59A0065C
P 3600 1200
F 0 "C24" H 3450 1300 50  0000 L CNN
F 1 "100nF" H 3350 1100 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 3638 1050 50  0001 C CNN
F 3 "" H 3600 1200 50  0000 C CNN
	1    3600 1200
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0118
U 1 1 59A02577
P 3950 1050
F 0 "#PWR0118" H 3950 800 50  0001 C CNN
F 1 "GND" H 3950 900 50  0000 C CNN
F 2 "" H 3950 1050 50  0000 C CNN
F 3 "" H 3950 1050 50  0000 C CNN
	1    3950 1050
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0119
U 1 1 59A02D9C
P 3600 1450
F 0 "#PWR0119" H 3600 1200 50  0001 C CNN
F 1 "GND" H 3600 1300 50  0000 C CNN
F 2 "" H 3600 1450 50  0000 C CNN
F 3 "" H 3600 1450 50  0000 C CNN
	1    3600 1450
	1    0    0    -1  
$EndComp
Text Label 3750 2100 1    60   ~ 0
Txd1
Text Label 3850 2100 1    60   ~ 0
Rxd1
Text Notes 3400 600  0    60   ~ 0
Bluetooth HC06
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R5
U 1 1 59A1104E
P 3850 1500
F 0 "R5" V 3930 1500 50  0000 C CNN
F 1 "1K" V 3850 1500 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3780 1500 50  0001 C CNN
F 3 "" H 3850 1500 50  0000 C CNN
	1    3850 1500
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-Mega_2560_core_mini_full_2.0 R6
U 1 1 59A12075
P 4050 1500
F 0 "R6" V 4150 1500 50  0000 C CNN
F 1 "2,2K" V 4050 1500 50  0000 C CNN
F 2 "Resistors_SMD:R_1206" V 3980 1500 50  0001 C CNN
F 3 "" H 4050 1500 50  0000 C CNN
	1    4050 1500
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Mega_2560_core_mini_full_2.0 #PWR0120
U 1 1 59A13130
P 4050 1750
F 0 "#PWR0120" H 4050 1500 50  0001 C CNN
F 1 "GND" H 4050 1600 50  0000 C CNN
F 2 "" H 4050 1750 50  0000 C CNN
F 3 "" H 4050 1750 50  0000 C CNN
	1    4050 1750
	1    0    0    -1  
$EndComp
Text Notes 12150 7300 1    47   ~ 0
LIR2032\n
Text Notes 12050 7200 1    60   ~ 0
ou
Text Label 11350 4500 0    60   ~ 0
U_batt
Wire Wire Line
	13600 4600 13700 4600
Connection ~ 14100 5050
Wire Wire Line
	14100 5050 14600 5050
Wire Wire Line
	15200 9100 15000 9100
Wire Wire Line
	15200 9200 15200 9550
Wire Notes Line
	15300 4400 13750 4400
Wire Wire Line
	13350 3600 14750 3600
Wire Notes Line
	4750 7700 7650 7700
Wire Notes Line
	15300 5400 15300 4400
Wire Notes Line
	13750 5400 15300 5400
Wire Notes Line
	13750 4400 13750 5400
Wire Wire Line
	1600 3200 1600 3900
Wire Wire Line
	2250 3200 2250 3900
Wire Wire Line
	3100 9450 3100 9350
Wire Wire Line
	2050 9450 2050 9350
Wire Notes Line
	4100 8000 11600 8000
Wire Notes Line
	4100 8000 4100 11050
Wire Notes Line
	4100 8900 1700 8900
Wire Notes Line
	1700 11050 11600 11050
Wire Notes Line
	1700 8900 1700 11050
Wire Notes Line
	11600 11050 11600 8000
Connection ~ 4750 10300
Wire Wire Line
	4750 10200 4750 10600
Wire Wire Line
	4500 10300 4750 10300
Wire Wire Line
	4500 10200 4500 10300
Connection ~ 4750 9800
Wire Wire Line
	4500 9900 4500 9800
Wire Wire Line
	4750 9800 4750 9900
Wire Wire Line
	4750 9400 4850 9400
Connection ~ 4750 9400
Wire Wire Line
	4750 9000 4750 9500
Wire Wire Line
	10150 9800 10150 9650
Wire Wire Line
	4500 9800 10150 9800
Connection ~ 8450 9800
Wire Wire Line
	8450 9800 8450 9600
Connection ~ 6750 9800
Wire Wire Line
	6750 9800 6750 9600
Connection ~ 5050 9800
Wire Wire Line
	5050 9800 5050 9600
Connection ~ 6850 8900
Connection ~ 5150 8900
Connection ~ 8550 8900
Connection ~ 10250 8950
Wire Wire Line
	10350 9950 10350 9650
Wire Wire Line
	10350 9950 11250 9950
Connection ~ 11000 8950
Wire Wire Line
	11250 8950 11250 9350
Connection ~ 10800 9950
Connection ~ 11000 9950
Wire Wire Line
	10800 10350 10800 10950
Wire Wire Line
	10800 10050 10800 9950
Wire Wire Line
	11200 10650 11200 10550
Wire Wire Line
	11000 10400 11000 10350
Wire Wire Line
	11050 10400 11000 10400
Wire Wire Line
	11250 9950 11250 9650
Wire Wire Line
	10250 8950 11250 8950
Wire Wire Line
	8650 9900 8650 9600
Wire Wire Line
	8650 9900 9550 9900
Connection ~ 9300 8900
Wire Wire Line
	9550 8900 9550 9300
Connection ~ 9100 9900
Connection ~ 9300 9900
Wire Wire Line
	9100 10300 9100 10900
Wire Wire Line
	9100 10000 9100 9900
Wire Wire Line
	9500 10600 9500 10500
Wire Wire Line
	9300 10350 9300 10300
Wire Wire Line
	9350 10350 9300 10350
Wire Wire Line
	9550 9900 9550 9600
Connection ~ 7400 9900
Connection ~ 7600 9900
Wire Wire Line
	7400 10300 7400 10900
Wire Wire Line
	7400 10000 7400 9900
Wire Wire Line
	7800 10600 7800 10500
Wire Wire Line
	7600 10350 7600 10300
Wire Wire Line
	7650 10350 7600 10350
Wire Wire Line
	5700 10300 5700 10900
Connection ~ 5700 9900
Wire Wire Line
	5700 10000 5700 9900
Wire Wire Line
	6100 10600 6100 10500
Wire Wire Line
	5900 10350 5900 10300
Wire Wire Line
	5950 10350 5900 10350
Connection ~ 5900 9900
Connection ~ 5900 8900
Wire Wire Line
	6150 8900 6150 9250
Wire Wire Line
	6150 9900 6150 9550
Connection ~ 7600 8900
Wire Wire Line
	7850 8900 7850 9250
Wire Wire Line
	7850 9900 7850 9550
Wire Wire Line
	8550 8350 8550 9000
Wire Wire Line
	8550 8900 9550 8900
Wire Wire Line
	6850 8350 6850 9000
Wire Wire Line
	6850 8900 7850 8900
Wire Wire Line
	6950 9900 7850 9900
Wire Wire Line
	6950 9600 6950 9900
Wire Wire Line
	5150 8350 5150 9000
Wire Wire Line
	5150 8900 6150 8900
Wire Wire Line
	5250 9900 6150 9900
Wire Wire Line
	5250 9600 5250 9900
Wire Wire Line
	10250 8400 10250 9050
Wire Wire Line
	2250 9350 2250 10600
Wire Wire Line
	2150 9350 2150 10600
Wire Wire Line
	3300 9350 3300 10600
Wire Wire Line
	3200 9350 3200 10600
Connection ~ 6700 1900
Connection ~ 5700 1900
Wire Wire Line
	5700 1800 5700 1900
Wire Wire Line
	6850 1900 6850 2150
Wire Wire Line
	6700 1900 6850 1900
Wire Wire Line
	6700 1800 6700 1900
Wire Wire Line
	7050 1200 7550 1200
Wire Wire Line
	7050 1000 7550 1000
Wire Wire Line
	6700 1200 6750 1200
Wire Wire Line
	6700 1000 6750 1000
Connection ~ 6200 2100
Wire Wire Line
	6200 2100 6200 2250
Wire Wire Line
	6700 2100 6700 2000
Wire Wire Line
	5700 2100 6700 2100
Wire Wire Line
	5700 2000 5700 2100
Wire Wire Line
	5550 1900 5550 2150
Wire Wire Line
	5700 1900 5550 1900
Wire Wire Line
	4550 1350 4550 1950
Wire Wire Line
	9450 1500 9450 900 
Wire Wire Line
	10550 900  10550 1300
Wire Wire Line
	10250 900  10250 1300
Wire Wire Line
	10850 1300 10850 900 
Connection ~ 3600 3400
Wire Wire Line
	3600 3200 3600 3850
Wire Wire Line
	3400 3200 3600 3200
Wire Notes Line
	15900 5650 15900 5550
Wire Wire Line
	2300 2050 2300 2650
Wire Wire Line
	2500 2050 2500 2650
Wire Wire Line
	2200 2050 2200 2650
Connection ~ 1400 2350
Wire Wire Line
	1400 2150 1400 2550
Wire Wire Line
	1200 2150 1400 2150
Wire Wire Line
	1200 2050 1200 2150
Connection ~ 1100 2350
Wire Wire Line
	1100 2050 1100 2550
Wire Wire Line
	900  2050 900  2350
Wire Wire Line
	1000 2050 1000 2350
Connection ~ 2300 1150
Connection ~ 4750 1050
Connection ~ 1400 1200
Wire Wire Line
	1400 1000 1400 1400
Wire Wire Line
	1200 1000 1400 1000
Wire Wire Line
	1200 900  1200 1000
Connection ~ 1100 1200
Wire Wire Line
	1100 900  1100 1400
Wire Notes Line
	15900 5600 15900 2800
Wire Wire Line
	13900 900  13900 1950
Wire Wire Line
	12650 900  12650 1950
Wire Wire Line
	12250 900  12250 2500
Wire Wire Line
	12350 900  12350 2500
Wire Wire Line
	500  6050 1250 6050
Wire Wire Line
	2800 8250 2800 7800
Wire Wire Line
	2700 8250 2700 7800
Wire Wire Line
	2600 8250 2600 7800
Wire Wire Line
	2500 8250 2500 7800
Wire Wire Line
	2400 8250 2400 7800
Wire Wire Line
	2300 8250 2300 7800
Connection ~ 1250 7000
Wire Wire Line
	1750 7000 1750 6750
Wire Wire Line
	1250 7000 1750 7000
Connection ~ 1750 7800
Wire Wire Line
	2000 7800 1750 7800
Wire Wire Line
	1750 7300 1750 7900
Wire Wire Line
	2000 7300 1750 7300
Connection ~ 3050 7300
Wire Wire Line
	3050 7300 3050 7800
Wire Wire Line
	3050 7800 2900 7800
Wire Wire Line
	2800 6850 2800 7300
Wire Wire Line
	2700 6850 2700 7300
Wire Wire Line
	2600 6850 2600 7300
Wire Wire Line
	2500 6850 2500 7300
Wire Wire Line
	2400 7300 2400 6850
Wire Wire Line
	8300 6650 8200 6650
Wire Wire Line
	1250 5950 750  5950
Connection ~ 5150 7250
Wire Wire Line
	5250 7250 5150 7250
Wire Wire Line
	5150 6750 5150 7450
Wire Wire Line
	5250 6750 5150 6750
Wire Wire Line
	1250 4950 750  4950
Wire Wire Line
	8400 6650 8400 7300
Wire Wire Line
	4900 6450 4900 6350
Wire Wire Line
	1750 6150 2450 6150
Wire Wire Line
	1750 5050 2250 5050
Connection ~ 14500 3600
Wire Wire Line
	14500 3750 14500 3600
Wire Wire Line
	14500 3850 14900 3850
Wire Wire Line
	14100 4900 14100 5150
Wire Wire Line
	12950 3900 12950 4150
Wire Wire Line
	3650 5850 4100 5850
Wire Wire Line
	1000 3200 1000 3900
Wire Wire Line
	8450 900  8450 1150
Wire Wire Line
	8350 900  8350 1150
Wire Wire Line
	10950 900  10950 1300
Wire Wire Line
	3650 5950 4100 5950
Wire Wire Line
	3150 5950 2750 5950
Wire Wire Line
	3150 6050 2750 6050
Wire Wire Line
	1250 6550 750  6550
Wire Wire Line
	1750 6550 2250 6550
Wire Wire Line
	3650 5550 4100 5550
Wire Wire Line
	1250 6350 750  6350
Wire Wire Line
	1750 6350 2250 6350
Wire Wire Line
	1750 6250 2250 6250
Wire Wire Line
	1250 6250 750  6250
Connection ~ 11800 1100
Wire Wire Line
	11800 950  11800 1400
Connection ~ 13050 1100
Wire Wire Line
	13050 950  13050 1400
Connection ~ 13400 1100
Wire Wire Line
	13400 1100 13350 1100
Connection ~ 13500 950 
Wire Wire Line
	13350 950  13500 950 
Wire Wire Line
	13900 1800 13700 1800
Wire Wire Line
	13900 1700 13800 1700
Wire Wire Line
	13700 900  13900 900 
Wire Wire Line
	12100 1100 12150 1100
Wire Wire Line
	12250 950  12100 950 
Connection ~ 13900 1600
Connection ~ 13600 1600
Connection ~ 13900 1800
Connection ~ 13900 1700
Connection ~ 13400 1800
Wire Wire Line
	12650 1800 12450 1800
Wire Wire Line
	12650 1700 12550 1700
Connection ~ 5050 1050
Wire Wire Line
	5050 900  5050 1250
Wire Wire Line
	4850 900  5050 900 
Connection ~ 2600 1150
Wire Wire Line
	2400 900  2600 900 
Wire Wire Line
	2600 900  2600 1400
Wire Wire Line
	1250 4850 1250 4700
Wire Wire Line
	1150 4700 2000 4700
Wire Wire Line
	1750 4700 1750 4850
Wire Wire Line
	3150 6750 3150 7000
Wire Wire Line
	3650 6750 3650 7050
Connection ~ 3650 7000
Wire Wire Line
	5000 3250 5050 3250
Wire Wire Line
	5050 3250 5050 3400
Wire Wire Line
	5000 3050 5350 3050
Wire Wire Line
	5350 3050 5350 3400
Wire Wire Line
	5000 3150 5250 3150
Wire Wire Line
	4150 3050 4500 3050
Wire Wire Line
	900  900  900  1200
Wire Wire Line
	1000 900  1000 1200
Wire Wire Line
	3150 5050 2750 5050
Wire Wire Line
	3150 4950 2750 4950
Wire Wire Line
	3150 4850 2750 4850
Wire Wire Line
	3650 5050 4300 5050
Wire Wire Line
	3650 4950 4050 4950
Wire Wire Line
	3650 4850 4300 4850
Wire Wire Line
	4500 3150 4150 3150
Wire Wire Line
	4500 3250 4150 3250
Connection ~ 1750 4700
Wire Wire Line
	1250 5650 750  5650
Wire Wire Line
	1750 5650 2250 5650
Wire Wire Line
	1900 900  1900 1350
Wire Wire Line
	2000 900  2000 1350
Wire Wire Line
	2100 900  2100 1350
Wire Wire Line
	2200 900  2200 1350
Wire Wire Line
	500  5750 1250 5750
Wire Wire Line
	4750 900  4750 1400
Wire Wire Line
	4450 1350 4450 1950
Wire Wire Line
	9550 900  9550 1300
Wire Wire Line
	9650 900  9650 1300
Wire Wire Line
	9750 900  9750 1300
Wire Wire Line
	9850 900  9850 1300
Wire Wire Line
	9950 900  9950 1300
Wire Wire Line
	10050 900  10050 1300
Wire Wire Line
	10150 900  10150 1300
Wire Wire Line
	10350 900  10350 1300
Wire Wire Line
	10450 900  10450 1300
Wire Wire Line
	10650 900  10650 1300
Wire Wire Line
	3650 5150 4100 5150
Wire Wire Line
	3150 5250 2750 5250
Wire Wire Line
	3650 5250 4100 5250
Wire Wire Line
	3650 5350 4100 5350
Wire Wire Line
	3150 5350 2750 5350
Wire Wire Line
	3150 5450 2750 5450
Wire Wire Line
	3650 5450 4100 5450
Wire Wire Line
	3150 5550 2750 5550
Wire Wire Line
	10500 1800 10500 2200
Wire Wire Line
	3650 6150 4100 6150
Wire Wire Line
	3150 6250 2750 6250
Wire Wire Line
	3150 6150 2750 6150
Wire Wire Line
	3650 6050 4100 6050
Wire Wire Line
	2650 6650 3150 6650
Wire Wire Line
	2650 6550 3150 6550
Wire Wire Line
	2650 6450 3150 6450
Wire Wire Line
	2650 6350 3150 6350
Wire Wire Line
	3650 6550 4150 6550
Wire Wire Line
	3650 6450 4150 6450
Wire Wire Line
	3650 6350 4150 6350
Wire Wire Line
	3650 6250 4150 6250
Connection ~ 12650 1800
Connection ~ 12650 1700
Wire Wire Line
	13500 900  13500 2500
Wire Wire Line
	13600 900  13600 2500
Connection ~ 12650 1600
Wire Wire Line
	12150 900  12150 2500
Wire Wire Line
	13400 900  13400 2500
Connection ~ 12150 1800
Connection ~ 12250 1700
Connection ~ 12350 1600
Connection ~ 13500 1700
Connection ~ 12150 1100
Connection ~ 12250 950 
Wire Wire Line
	12650 900  12450 900 
Wire Wire Line
	1250 5550 750  5550
Wire Wire Line
	1750 5550 2250 5550
Wire Wire Line
	1750 5350 2350 5350
Wire Wire Line
	1250 5150 750  5150
Wire Wire Line
	1750 5150 2250 5150
Wire Wire Line
	1250 6450 750  6450
Wire Wire Line
	1750 6450 2250 6450
Wire Wire Line
	2300 900  2300 1400
Wire Wire Line
	6050 6550 5850 6550
Wire Wire Line
	5850 7250 6050 7250
Wire Wire Line
	13600 4600 13600 3600
Connection ~ 13600 3600
Wire Wire Line
	6700 800  7550 800 
Wire Wire Line
	5650 8900 5650 9200
Connection ~ 5650 8900
Wire Wire Line
	5600 10300 5700 10300
Wire Wire Line
	5650 9600 5600 9700
Wire Wire Line
	5600 9700 5600 10300
Connection ~ 5700 10300
Connection ~ 7350 8900
Wire Wire Line
	7350 8900 7350 9200
Wire Wire Line
	7350 9600 7300 9700
Wire Wire Line
	7300 9700 7300 10300
Wire Wire Line
	7300 10300 7400 10300
Connection ~ 7400 10300
Connection ~ 9050 8900
Wire Wire Line
	9050 8900 9050 9200
Wire Wire Line
	10750 8950 10750 9250
Connection ~ 10750 8950
Wire Wire Line
	9050 9600 9000 9700
Wire Wire Line
	9000 9700 9000 10300
Wire Wire Line
	9000 10300 9100 10300
Wire Wire Line
	10750 9650 10700 9750
Wire Wire Line
	10700 9750 10700 10350
Wire Wire Line
	10700 10350 10800 10350
Connection ~ 10800 10350
Connection ~ 9100 10300
Wire Wire Line
	3300 3200 3300 3450
Wire Wire Line
	3300 3750 3300 4050
Connection ~ 3300 3400
Wire Wire Line
	5900 8900 5900 9000
Wire Wire Line
	5750 9150 5750 8900
Connection ~ 5750 8900
Wire Wire Line
	5900 9300 5900 9450
Wire Wire Line
	5900 9750 5900 10000
Wire Wire Line
	7600 9300 7600 9450
Wire Wire Line
	7600 9750 7600 10000
Wire Wire Line
	7600 9000 7600 8900
Wire Wire Line
	7450 9150 7450 8900
Connection ~ 7450 8900
Wire Wire Line
	9300 9450 9300 9300
Wire Wire Line
	9300 9000 9300 8900
Wire Wire Line
	9150 9150 9150 8900
Connection ~ 9150 8900
Wire Wire Line
	9300 9750 9300 10000
Wire Wire Line
	11000 9850 11000 10050
Wire Wire Line
	11000 9550 11000 9350
Wire Wire Line
	11000 9050 11000 8950
Wire Wire Line
	10850 9200 10850 8950
Connection ~ 10850 8950
Wire Wire Line
	10750 900  10750 1650
Wire Wire Line
	4450 9100 4450 9050
Wire Wire Line
	4450 9050 4750 9050
Connection ~ 4750 9050
Wire Wire Line
	4450 9500 4450 9400
Wire Wire Line
	2900 2500 2900 2450
Wire Wire Line
	12450 3150 12450 3650
Connection ~ 12450 3600
Wire Wire Line
	12450 4000 12450 3950
Wire Wire Line
	13400 3950 13400 4000
Wire Wire Line
	13400 3650 13400 3600
Connection ~ 13400 3600
Wire Wire Line
	13800 3650 13800 3600
Connection ~ 13800 3600
Wire Wire Line
	13800 4050 13800 3950
Wire Wire Line
	15000 4600 15000 4750
Wire Wire Line
	14500 4600 15000 4600
Wire Wire Line
	15000 5150 15000 5050
Wire Wire Line
	5600 800  5700 800 
Wire Wire Line
	5700 700  5600 700 
Wire Wire Line
	8800 6650 8800 7300
Wire Wire Line
	8700 6650 8700 7300
Wire Wire Line
	3800 10550 3800 10950
Wire Wire Line
	3700 10850 3700 10550
Wire Wire Line
	3600 10550 3600 10650
Wire Wire Line
	3150 7000 3650 7000
Wire Wire Line
	3950 6700 3950 6650
Wire Wire Line
	3950 7000 3950 7050
Wire Wire Line
	2900 7300 3300 7300
Wire Wire Line
	3950 6650 3650 6650
Wire Wire Line
	2000 2050 2000 2650
Wire Wire Line
	2600 2150 2600 2050
Wire Wire Line
	1800 2050 1800 2650
Wire Wire Line
	9250 900  9250 1300
Wire Wire Line
	9350 900  9350 1500
Wire Wire Line
	3000 9350 3000 10850
Connection ~ 3000 9450
Wire Wire Line
	1950 9350 1950 10850
Connection ~ 1950 9450
Wire Wire Line
	9450 1900 9450 1800
Wire Wire Line
	9350 1500 9300 1500
Wire Wire Line
	9300 1500 9300 1650
Wire Wire Line
	2350 3850 2350 3600
Wire Wire Line
	2350 3600 2250 3600
Connection ~ 2250 3600
Wire Wire Line
	1700 3850 1700 3600
Wire Wire Line
	1700 3600 1600 3600
Connection ~ 1600 3600
Wire Wire Line
	1100 3850 1100 3600
Wire Wire Line
	2350 4150 2350 4200
Wire Wire Line
	1700 4150 1700 4200
Wire Wire Line
	1100 4150 1100 4200
Wire Wire Line
	7050 6800 7100 6800
Wire Wire Line
	6400 7000 7100 7000
Wire Wire Line
	5850 7050 6050 7050
Wire Wire Line
	6050 7050 6200 6900
Wire Wire Line
	6200 6900 7100 6900
Connection ~ 7050 7200
Wire Wire Line
	7050 6800 7050 7250
Wire Wire Line
	7000 6450 7000 7100
Wire Wire Line
	7100 6700 7000 6700
Connection ~ 7000 6700
Wire Wire Line
	6400 7000 6150 6750
Wire Wire Line
	6150 6750 5850 6750
Wire Wire Line
	7100 6500 6500 6500
Wire Wire Line
	7100 6600 6500 6600
Wire Wire Line
	3500 9350 3500 9400
Wire Wire Line
	3500 9400 3400 9400
Wire Wire Line
	3400 9350 3400 9450
Connection ~ 3400 9400
Wire Wire Line
	3500 9700 3300 9700
Connection ~ 3300 9700
Wire Wire Line
	3500 10000 3200 10000
Connection ~ 3200 10000
Connection ~ 3000 10850
Wire Wire Line
	3100 9450 3000 9450
Wire Wire Line
	3800 9700 3850 9700
Wire Wire Line
	3800 10000 3850 10000
Wire Wire Line
	2450 9350 2450 9400
Wire Wire Line
	2450 9400 2350 9400
Wire Wire Line
	2350 9350 2350 9450
Connection ~ 2350 9400
Wire Wire Line
	2450 9700 2250 9700
Connection ~ 2250 9700
Wire Wire Line
	2450 10000 2150 10000
Connection ~ 2150 10000
Wire Wire Line
	2750 9700 2800 9700
Wire Wire Line
	2750 10000 2800 10000
Wire Wire Line
	1950 10850 3700 10850
Wire Wire Line
	2050 9450 1950 9450
Wire Wire Line
	10100 4900 10750 4900
Wire Wire Line
	10100 4800 10250 4800
Wire Wire Line
	10150 5000 10100 5000
Wire Wire Line
	10150 4400 10150 5150
Wire Wire Line
	10100 4500 10150 4500
Connection ~ 10150 4500
Wire Wire Line
	10150 5100 10100 5100
Connection ~ 10150 5000
Connection ~ 10150 5100
Wire Wire Line
	10350 4700 10100 4700
Wire Wire Line
	10350 4500 10350 4700
Wire Wire Line
	10350 4600 10100 4600
Connection ~ 10350 4600
Wire Wire Line
	4550 1050 4550 1000
Wire Wire Line
	4550 1000 4650 950 
Wire Wire Line
	4650 950  4650 900 
Wire Wire Line
	4450 1050 4450 1000
Wire Wire Line
	4450 1000 4550 950 
Wire Wire Line
	4550 950  4550 900 
Wire Wire Line
	10550 3700 10650 3700
Wire Wire Line
	10650 3700 10650 3800
Wire Wire Line
	10250 4800 10250 4900
Connection ~ 10250 4900
Wire Wire Line
	10550 3600 11300 3600
Wire Wire Line
	10100 4400 10150 4400
Wire Notes Line
	9700 4300 10850 4300
Wire Notes Line
	10850 4300 10850 5400
Wire Notes Line
	10850 5400 9700 5400
Wire Notes Line
	9700 5400 9700 4300
Wire Wire Line
	1750 4950 2350 4950
Wire Wire Line
	14600 3950 14600 4750
Connection ~ 14600 4600
Wire Wire Line
	14500 4700 14600 4700
Connection ~ 14600 4700
Wire Wire Line
	14500 3950 14600 3950
Wire Wire Line
	2150 3200 2150 3300
Wire Wire Line
	2350 3200 2350 3300
Wire Wire Line
	1500 3200 1500 3300
Wire Wire Line
	1700 3200 1700 3300
Wire Wire Line
	900  3200 900  3300
Wire Wire Line
	1100 3200 1100 3300
Wire Wire Line
	1100 3600 1000 3600
Connection ~ 1000 3600
Wire Notes Line
	5050 1500 5050 2150
Wire Notes Line
	5050 2150 5000 2150
Wire Notes Line
	4600 2150 4450 2150
Wire Notes Line
	4450 2150 4450 2000
Wire Wire Line
	7300 1350 7300 800 
Connection ~ 7300 800 
Wire Wire Line
	7300 1650 7300 1750
Wire Wire Line
	10750 1650 10650 1650
Wire Wire Line
	12150 4450 12150 4600
Wire Wire Line
	12150 4000 12150 4050
Wire Wire Line
	11350 4500 12150 4500
Connection ~ 12150 4500
Wire Wire Line
	12150 5050 12150 5000
Wire Wire Line
	3150 7550 3050 7550
Connection ~ 3050 7550
Wire Wire Line
	3450 7550 3550 7550
Connection ~ 1250 4700
Wire Wire Line
	850  4700 800  4700
Wire Wire Line
	4150 4700 4250 4700
Wire Wire Line
	3850 4700 3800 4700
Wire Wire Line
	3800 4700 3800 4850
Connection ~ 3800 4850
Wire Wire Line
	5850 2150 5850 2100
Connection ~ 5850 2100
Wire Wire Line
	6500 2150 6500 2100
Connection ~ 6500 2100
Wire Wire Line
	6500 2450 6500 2500
Wire Wire Line
	5850 2450 5850 2500
Wire Wire Line
	13900 6100 13900 6300
Wire Wire Line
	14350 6200 14300 6200
Wire Wire Line
	14000 6200 13900 6200
Connection ~ 13900 6200
Wire Wire Line
	13900 7400 13900 7300
Wire Wire Line
	14600 6900 14850 6900
Wire Wire Line
	14600 7000 14850 7000
Wire Wire Line
	14600 6700 14700 6700
Wire Wire Line
	10300 6600 9750 6600
Wire Wire Line
	10300 6700 9750 6700
Wire Wire Line
	10300 6900 10200 6900
Wire Wire Line
	10200 6900 10200 7350
Wire Wire Line
	10300 7300 10200 7300
Connection ~ 10200 7300
Wire Wire Line
	10300 7200 10200 7200
Connection ~ 10200 7200
Wire Wire Line
	10300 7100 10200 7100
Connection ~ 10200 7100
Wire Wire Line
	10300 7000 10200 7000
Connection ~ 10200 7000
Wire Wire Line
	11300 6900 11400 6900
Wire Wire Line
	11400 6900 11400 7350
Wire Wire Line
	11300 7200 11400 7200
Connection ~ 11400 7200
Wire Wire Line
	11300 7100 11400 7100
Connection ~ 11400 7100
Wire Wire Line
	11300 7000 11400 7000
Connection ~ 11400 7000
Wire Wire Line
	11300 6800 11750 6800
Wire Wire Line
	11750 7450 11750 7550
Wire Wire Line
	12000 6050 12000 6000
Wire Wire Line
	12000 6400 12000 6350
Wire Wire Line
	11700 6500 11300 6500
Wire Wire Line
	11350 6350 11350 6600
Wire Wire Line
	11350 6600 11300 6600
Wire Wire Line
	11550 6350 11550 6700
Wire Wire Line
	11550 6700 11300 6700
Wire Wire Line
	11350 6050 11350 6000
Wire Wire Line
	11550 6050 11550 6000
Wire Wire Line
	13200 6600 13100 6600
Wire Wire Line
	13100 6600 13100 6900
Wire Wire Line
	13200 6800 13100 6800
Connection ~ 13100 6800
Wire Wire Line
	13200 6700 13100 6700
Connection ~ 13100 6700
Wire Wire Line
	11750 6800 11750 6850
Wire Notes Line
	9550 5750 12300 5750
Wire Notes Line
	12300 5750 12300 7750
Wire Notes Line
	12300 7750 9550 7750
Wire Notes Line
	9550 7750 9550 5750
Wire Notes Line
	12700 5750 15000 5750
Wire Notes Line
	15000 5750 15000 7750
Wire Notes Line
	15000 7750 12700 7750
Wire Notes Line
	12700 7750 12700 5750
Wire Wire Line
	14150 8800 14350 8800
Wire Wire Line
	14700 9100 14150 9100
Wire Wire Line
	13150 8800 12900 8800
Wire Wire Line
	14150 8500 14700 8500
Wire Wire Line
	12350 8500 13150 8500
Wire Wire Line
	12350 9100 13150 9100
Wire Wire Line
	11900 9350 12600 9350
Wire Wire Line
	13000 9350 13000 9100
Connection ~ 13000 9100
Wire Notes Line
	15800 9850 11800 9850
Wire Notes Line
	11800 9850 11800 8000
Wire Notes Line
	11800 8000 15800 8000
Wire Notes Line
	15800 8000 15800 9850
Wire Wire Line
	1250 6750 1250 7050
Wire Notes Line
	9550 5650 9550 2800
Wire Notes Line
	9550 5650 15900 5650
Wire Notes Line
	9550 2800 15900 2800
Wire Wire Line
	5050 7050 5250 7050
Wire Wire Line
	5050 6550 5050 7050
Wire Wire Line
	5050 6550 5250 6550
Wire Wire Line
	5050 6800 4900 6800
Wire Wire Line
	4900 6800 4900 6750
Connection ~ 5050 6800
Wire Notes Line
	4750 5950 7650 5950
Wire Notes Line
	7650 5950 7650 7700
Wire Notes Line
	4750 7700 4750 5950
Wire Wire Line
	5800 3950 5800 3200
Wire Wire Line
	5900 3200 5900 3950
Wire Wire Line
	6000 3950 6000 3200
Wire Wire Line
	6100 3950 6100 3200
Wire Wire Line
	6200 3950 6200 3200
Wire Wire Line
	6300 3950 6300 3200
Wire Wire Line
	6400 3950 6400 3200
Wire Wire Line
	6500 3950 6500 3200
Wire Wire Line
	6700 3200 6700 3400
Wire Wire Line
	6600 3200 6600 3300
Wire Wire Line
	6600 3300 6700 3300
Connection ~ 6700 3300
Wire Wire Line
	7450 3950 7450 3200
Wire Wire Line
	7550 3950 7550 3200
Wire Wire Line
	7650 3950 7650 3200
Wire Wire Line
	7750 3950 7750 3200
Wire Wire Line
	7850 3950 7850 3200
Wire Wire Line
	7950 3950 7950 3200
Wire Wire Line
	8050 3950 8050 3200
Wire Wire Line
	8150 3950 8150 3200
Wire Wire Line
	8350 3200 8350 3400
Wire Wire Line
	8250 3200 8250 3300
Wire Wire Line
	8250 3300 8350 3300
Connection ~ 8350 3300
Wire Notes Line
	8100 6450 8100 6250
Wire Notes Line
	8100 6250 8200 6250
Wire Notes Line
	8200 6250 8200 6450
Wire Notes Line
	7850 7700 9000 7700
Wire Notes Line
	9000 7700 9000 5950
Wire Notes Line
	9000 5950 7850 5950
Wire Notes Line
	7850 5950 7850 7700
Wire Wire Line
	1250 5050 750  5050
Wire Wire Line
	1250 5450 650  5450
Wire Wire Line
	1750 5450 2350 5450
Wire Wire Line
	1750 5750 2300 5750
Wire Wire Line
	2550 5650 3150 5650
Wire Wire Line
	2550 5750 3150 5750
Wire Wire Line
	3650 5650 4300 5650
Wire Wire Line
	3650 5750 4300 5750
Wire Wire Line
	2200 7300 2200 6850
Wire Wire Line
	1250 5350 650  5350
Wire Wire Line
	1750 5850 2300 5850
Wire Wire Line
	1250 5850 750  5850
Wire Wire Line
	2350 5950 1750 5950
Wire Wire Line
	2700 2050 2700 2300
Wire Wire Line
	2100 2050 2100 2650
Wire Wire Line
	1900 2050 1900 2650
Wire Wire Line
	2400 2050 2400 2300
Wire Wire Line
	2900 2150 2900 2100
Wire Wire Line
	2900 2100 2700 2100
Connection ~ 2700 2100
Wire Wire Line
	7000 7100 7100 7100
Wire Wire Line
	7050 7200 7100 7200
Wire Wire Line
	3950 950  3950 1050
Wire Wire Line
	3600 1350 3600 1450
Wire Wire Line
	3650 1000 3650 950 
Wire Wire Line
	3750 950  3750 2100
Wire Wire Line
	3850 1650 3850 2100
Wire Wire Line
	3650 1000 3600 1000
Wire Wire Line
	3600 1000 3600 1050
Wire Wire Line
	3850 950  3850 1350
Wire Wire Line
	4050 1350 4050 1300
Wire Wire Line
	4050 1300 3850 1300
Connection ~ 3850 1300
Wire Wire Line
	4050 1650 4050 1750
Wire Wire Line
	13000 9350 12900 9350
Wire Wire Line
	8600 6750 8600 6650
Wire Wire Line
	8500 6750 8500 6650
Wire Wire Line
	8100 6750 8100 6650
Wire Wire Line
	11800 4350 11800 4600
Connection ~ 11800 4500
Wire Wire Line
	11800 4950 11800 4900
Wire Wire Line
	11800 3400 11800 4050
Connection ~ 11800 3600
Wire Wire Line
	12300 3600 12550 3600
Wire Wire Line
	11400 3600 12000 3600
Wire Wire Line
	9050 900  9050 1050
Wire Wire Line
	9150 900  9150 950 
Wire Wire Line
	9150 950  9050 950 
Connection ~ 9050 950 
Wire Wire Line
	14650 900  14650 1100
Wire Wire Line
	14650 1100 14550 1100
Wire Wire Line
	14550 900  14550 1200
Connection ~ 14550 1100
Wire Wire Line
	14750 1500 14750 900 
Wire Wire Line
	14850 1500 14850 900 
Wire Wire Line
	14950 1500 14950 900 
Wire Wire Line
	15050 1500 15050 900 
Wire Wire Line
	15150 1500 15150 900 
Wire Wire Line
	15250 1500 15250 900 
Wire Wire Line
	8150 900  8150 1200
$Comp
L VCC #PWR?
U 1 1 59B17663
P 8250 1200
F 0 "#PWR?" H 8250 1050 50  0001 C CNN
F 1 "VCC" V 8250 1400 50  0000 C CNN
F 2 "" H 8250 1200 50  0000 C CNN
F 3 "" H 8250 1200 50  0000 C CNN
	1    8250 1200
	-1   0    0    1   
$EndComp
Wire Wire Line
	8250 1200 8250 900 
$EndSCHEMATC
