EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "RCTX Decoder Channels Expander"
Date "2020-08-16"
Rev ""
Comp ""
Comment1 "Using a single TLC5957 chip to drive 48 optoisolators LEDs for the ON/OFF channels"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L atmel:ATMEGA328P-AU U3
U 1 1 5F3940B9
P 4400 4950
F 0 "U3" H 4450 6317 50  0000 C CNN
F 1 "ATMEGA328P-AU" H 4450 6226 50  0000 C CNN
F 2 "Housings_QFP:TQFP-32_7x7mm_Pitch0.8mm" H 4400 4950 50  0001 C CIN
F 3 "http://www.atmel.com/images/atmel-8271-8-bit-avr-microcontroller-atmega48a-48pa-88a-88pa-168a-168pa-328-328p_datasheet.pdf" H 4400 4950 50  0001 C CNN
	1    4400 4950
	1    0    0    -1  
$EndComp
$Comp
L conn:Conn_01x03_Male J5
U 1 1 5F39DCC3
P 6650 7450
F 0 "J5" V 6600 7500 50  0000 R CNN
F 1 "SERIAL IN" V 6500 7450 50  0000 C CNN
F 2 "" H 6650 7450 50  0001 C CNN
F 3 "~" H 6650 7450 50  0001 C CNN
	1    6650 7450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5400 5450 6550 5450
Wire Wire Line
	3500 5950 3400 5950
Wire Wire Line
	3400 5950 3400 6050
Wire Wire Line
	3400 6050 3500 6050
Wire Wire Line
	3400 6050 3400 6150
Wire Wire Line
	3400 6150 3500 6150
Connection ~ 3400 6050
Wire Wire Line
	3400 6150 3400 6850
Connection ~ 3400 6150
Wire Wire Line
	3400 6850 3400 7350
Connection ~ 3400 6850
$Comp
L power:Earth #PWR09
U 1 1 5F3ABFE3
P 3400 7350
F 0 "#PWR09" H 3400 7100 50  0001 C CNN
F 1 "Earth" H 3400 7200 50  0001 C CNN
F 2 "" H 3400 7350 50  0001 C CNN
F 3 "~" H 3400 7350 50  0001 C CNN
	1    3400 7350
	1    0    0    -1  
$EndComp
$Comp
L device:R R4
U 1 1 5F3BB341
P 6550 6350
F 0 "R4" H 6620 6396 50  0000 L CNN
F 1 "1k" H 6620 6305 50  0000 L CNN
F 2 "" V 6480 6350 50  0001 C CNN
F 3 "" H 6550 6350 50  0001 C CNN
	1    6550 6350
	1    0    0    -1  
$EndComp
Wire Wire Line
	6550 6200 6550 5450
$Comp
L regul:LM1117-5.0 U1
U 1 1 5F3C6736
P 1300 3850
F 0 "U1" H 1300 4092 50  0000 C CNN
F 1 "LM1117-5.0" H 1300 4001 50  0000 C CNN
F 2 "" H 1300 3850 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm1117.pdf" H 1300 3850 50  0001 C CNN
	1    1300 3850
	1    0    0    -1  
$EndComp
$Comp
L device:C C1
U 1 1 5F3C7ECE
P 1700 4100
F 0 "C1" H 1650 4450 50  0000 L CNN
F 1 "1uF" V 1850 4000 50  0000 L CNN
F 2 "" H 1738 3950 50  0001 C CNN
F 3 "" H 1700 4100 50  0001 C CNN
	1    1700 4100
	1    0    0    -1  
$EndComp
$Comp
L device:R R1
U 1 1 5F3C8851
P 2050 4100
F 0 "R1" H 2120 4146 50  0000 L CNN
F 1 "1k" H 2120 4055 50  0000 L CNN
F 2 "" V 1980 4100 50  0001 C CNN
F 3 "" H 2050 4100 50  0001 C CNN
	1    2050 4100
	1    0    0    -1  
$EndComp
$Comp
L device:LED D1
U 1 1 5F3C9564
P 2050 4500
F 0 "D1" V 2089 4383 50  0000 R CNN
F 1 "GREEN" H 2150 4650 50  0000 R CNN
F 2 "" H 2050 4500 50  0001 C CNN
F 3 "" H 2050 4500 50  0001 C CNN
	1    2050 4500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1600 3850 1700 3850
Wire Wire Line
	1700 3850 1700 3950
Wire Wire Line
	1700 3850 2050 3850
Wire Wire Line
	2050 3850 2050 3950
Connection ~ 1700 3850
Wire Wire Line
	2050 4250 2050 4350
Wire Wire Line
	2050 3850 2750 3850
Connection ~ 2050 3850
$Comp
L power:Earth #PWR04
U 1 1 5F3CF6F3
P 2050 4750
F 0 "#PWR04" H 2050 4500 50  0001 C CNN
F 1 "Earth" H 2050 4600 50  0001 C CNN
F 2 "" H 2050 4750 50  0001 C CNN
F 3 "~" H 2050 4750 50  0001 C CNN
	1    2050 4750
	1    0    0    -1  
$EndComp
$Comp
L power:Earth #PWR02
U 1 1 5F3D0097
P 1700 4750
F 0 "#PWR02" H 1700 4500 50  0001 C CNN
F 1 "Earth" H 1700 4600 50  0001 C CNN
F 2 "" H 1700 4750 50  0001 C CNN
F 3 "~" H 1700 4750 50  0001 C CNN
	1    1700 4750
	1    0    0    -1  
$EndComp
$Comp
L power:Earth #PWR01
U 1 1 5F3D07CB
P 1300 4750
F 0 "#PWR01" H 1300 4500 50  0001 C CNN
F 1 "Earth" H 1300 4600 50  0001 C CNN
F 2 "" H 1300 4750 50  0001 C CNN
F 3 "~" H 1300 4750 50  0001 C CNN
	1    1300 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1300 4150 1300 4750
Wire Wire Line
	1700 4250 1700 4750
Wire Wire Line
	2050 4650 2050 4750
$Comp
L power:+5V #PWR03
U 1 1 5F3D354A
P 2050 3750
F 0 "#PWR03" H 2050 3600 50  0001 C CNN
F 1 "+5V" H 2065 3923 50  0000 C CNN
F 2 "" H 2050 3750 50  0001 C CNN
F 3 "" H 2050 3750 50  0001 C CNN
	1    2050 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 3750 2050 3850
Wire Wire Line
	3500 3950 3400 3950
Wire Wire Line
	3400 3850 3500 3850
Wire Wire Line
	900  3850 1000 3850
Wire Wire Line
	3500 4150 3400 4150
$Comp
L device:C C2
U 1 1 5F3DE7EC
P 2450 4350
F 0 "C2" H 2500 4200 50  0000 L CNN
F 1 "1uF" V 2600 4300 50  0000 L CNN
F 2 "" H 2488 4200 50  0001 C CNN
F 3 "" H 2450 4350 50  0001 C CNN
	1    2450 4350
	1    0    0    -1  
$EndComp
Connection ~ 3400 3850
$Comp
L device:CP C4
U 1 1 5F3FA7F3
P 2750 4350
F 0 "C4" H 2800 4200 50  0000 L CNN
F 1 "7uF 16V" V 2900 4300 50  0000 L CNN
F 2 "" H 2788 4200 50  0001 C CNN
F 3 "" H 2750 4350 50  0001 C CNN
	1    2750 4350
	1    0    0    -1  
$EndComp
$Comp
L device:C C5
U 1 1 5F3FB1D8
P 3050 4350
F 0 "C5" H 3100 4200 50  0000 L CNN
F 1 "1uF" V 3200 4300 50  0000 L CNN
F 2 "" H 3088 4200 50  0001 C CNN
F 3 "" H 3050 4350 50  0001 C CNN
	1    3050 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 4200 2450 4000
Wire Wire Line
	2450 4000 2750 4000
Wire Wire Line
	2750 4000 2750 4200
Wire Wire Line
	2750 4000 3050 4000
Wire Wire Line
	3050 4000 3050 4200
Connection ~ 2750 4000
Wire Wire Line
	2750 4000 2750 3850
Connection ~ 2750 3850
Wire Wire Line
	2750 3850 3400 3850
Wire Wire Line
	2450 4500 2450 4650
Wire Wire Line
	2450 4650 2750 4650
Wire Wire Line
	2750 4650 2750 4500
Wire Wire Line
	2750 4650 3050 4650
Wire Wire Line
	3050 4650 3050 4500
Connection ~ 2750 4650
$Comp
L power:Earth #PWR07
U 1 1 5F407E27
P 2750 4750
F 0 "#PWR07" H 2750 4500 50  0001 C CNN
F 1 "Earth" H 2750 4600 50  0001 C CNN
F 2 "" H 2750 4750 50  0001 C CNN
F 3 "~" H 2750 4750 50  0001 C CNN
	1    2750 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 4750 2750 4650
$Comp
L motors:Motor_Servo M12
U 1 1 5F433BF3
P 9750 6250
F 0 "M12" H 10082 6315 50  0000 L CNN
F 1 "Motor_Servo" H 10082 6224 50  0000 L CNN
F 2 "" H 9750 6060 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 9750 6060 50  0001 C CNN
	1    9750 6250
	1    0    0    -1  
$EndComp
$Comp
L motors:Motor_Servo M11
U 1 1 5F434EC5
P 9750 5950
F 0 "M11" H 10082 6015 50  0000 L CNN
F 1 "Motor_Servo" H 10082 5924 50  0000 L CNN
F 2 "" H 9750 5760 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 9750 5760 50  0001 C CNN
	1    9750 5950
	1    0    0    -1  
$EndComp
$Comp
L motors:Motor_Servo M10
U 1 1 5F435E18
P 9750 5650
F 0 "M10" H 10082 5715 50  0000 L CNN
F 1 "Motor_Servo" H 10082 5624 50  0000 L CNN
F 2 "" H 9750 5460 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 9750 5460 50  0001 C CNN
	1    9750 5650
	1    0    0    -1  
$EndComp
$Comp
L motors:Motor_Servo M9
U 1 1 5F436AAD
P 9750 5350
F 0 "M9" H 10082 5415 50  0000 L CNN
F 1 "Motor_Servo" H 10082 5324 50  0000 L CNN
F 2 "" H 9750 5160 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 9750 5160 50  0001 C CNN
	1    9750 5350
	1    0    0    -1  
$EndComp
$Comp
L motors:Motor_Servo M8
U 1 1 5F43777F
P 9750 5050
F 0 "M8" H 10082 5115 50  0000 L CNN
F 1 "Motor_Servo" H 10082 5024 50  0000 L CNN
F 2 "" H 9750 4860 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 9750 4860 50  0001 C CNN
	1    9750 5050
	1    0    0    -1  
$EndComp
$Comp
L motors:Motor_Servo M7
U 1 1 5F438568
P 9750 4750
F 0 "M7" H 10082 4815 50  0000 L CNN
F 1 "Motor_Servo" H 10082 4724 50  0000 L CNN
F 2 "" H 9750 4560 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 9750 4560 50  0001 C CNN
	1    9750 4750
	1    0    0    -1  
$EndComp
$Comp
L motors:Motor_Servo M6
U 1 1 5F43903E
P 9750 4450
F 0 "M6" H 10082 4515 50  0000 L CNN
F 1 "Motor_Servo" H 10082 4424 50  0000 L CNN
F 2 "" H 9750 4260 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 9750 4260 50  0001 C CNN
	1    9750 4450
	1    0    0    -1  
$EndComp
$Comp
L motors:Motor_Servo M5
U 1 1 5F439ED4
P 9750 4150
F 0 "M5" H 10082 4215 50  0000 L CNN
F 1 "Motor_Servo" H 10082 4124 50  0000 L CNN
F 2 "" H 9750 3960 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 9750 3960 50  0001 C CNN
	1    9750 4150
	1    0    0    -1  
$EndComp
$Comp
L motors:Motor_Servo M4
U 1 1 5F43AC62
P 9750 3850
F 0 "M4" H 10082 3915 50  0000 L CNN
F 1 "Motor_Servo" H 10082 3824 50  0000 L CNN
F 2 "" H 9750 3660 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 9750 3660 50  0001 C CNN
	1    9750 3850
	1    0    0    -1  
$EndComp
$Comp
L motors:Motor_Servo M3
U 1 1 5F43B9C2
P 9750 3550
F 0 "M3" H 10082 3615 50  0000 L CNN
F 1 "Motor_Servo" H 10082 3524 50  0000 L CNN
F 2 "" H 9750 3360 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 9750 3360 50  0001 C CNN
	1    9750 3550
	1    0    0    -1  
$EndComp
$Comp
L motors:Motor_Servo M2
U 1 1 5F43C919
P 9750 3250
F 0 "M2" H 10082 3315 50  0000 L CNN
F 1 "Motor_Servo" H 10082 3224 50  0000 L CNN
F 2 "" H 9750 3060 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 9750 3060 50  0001 C CNN
	1    9750 3250
	1    0    0    -1  
$EndComp
$Comp
L motors:Motor_Servo M1
U 1 1 5F43D539
P 9750 2950
F 0 "M1" H 10082 3015 50  0000 L CNN
F 1 "Motor_Servo" H 10082 2924 50  0000 L CNN
F 2 "" H 9750 2760 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 9750 2760 50  0001 C CNN
	1    9750 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	9250 6250 9450 6250
Wire Wire Line
	9250 6250 9250 5950
Wire Wire Line
	9250 5950 9450 5950
Wire Wire Line
	9250 5950 9250 5650
Wire Wire Line
	9250 5650 9450 5650
Connection ~ 9250 5950
Wire Wire Line
	9250 5650 9250 5350
Wire Wire Line
	9250 5350 9450 5350
Connection ~ 9250 5650
Wire Wire Line
	9250 5350 9250 5050
Wire Wire Line
	9250 5050 9450 5050
Connection ~ 9250 5350
Wire Wire Line
	9250 5050 9250 4750
Wire Wire Line
	9250 4750 9450 4750
Connection ~ 9250 5050
Wire Wire Line
	9250 4750 9250 4450
Wire Wire Line
	9250 4450 9450 4450
Connection ~ 9250 4750
Wire Wire Line
	9250 4450 9250 4150
Wire Wire Line
	9250 4150 9450 4150
Connection ~ 9250 4450
Wire Wire Line
	9250 4150 9250 3850
Wire Wire Line
	9250 3850 9450 3850
Connection ~ 9250 4150
Wire Wire Line
	9250 3850 9250 3550
Wire Wire Line
	9250 3550 9450 3550
Connection ~ 9250 3850
Wire Wire Line
	9250 3550 9250 3250
Wire Wire Line
	9250 3250 9450 3250
Connection ~ 9250 3550
Wire Wire Line
	9250 3250 9250 2950
Wire Wire Line
	9250 2950 9450 2950
Connection ~ 9250 3250
Wire Wire Line
	9350 6350 9450 6350
Wire Wire Line
	9350 6350 9350 6050
Wire Wire Line
	9350 6050 9450 6050
Wire Wire Line
	9350 6050 9350 5750
Wire Wire Line
	9350 5750 9450 5750
Connection ~ 9350 6050
Wire Wire Line
	9350 5750 9350 5450
Wire Wire Line
	9350 5450 9450 5450
Connection ~ 9350 5750
Wire Wire Line
	9350 5450 9350 5150
Wire Wire Line
	9350 5150 9450 5150
Connection ~ 9350 5450
Wire Wire Line
	9350 5150 9350 4850
Wire Wire Line
	9350 4850 9450 4850
Connection ~ 9350 5150
Wire Wire Line
	9350 4850 9350 4550
Wire Wire Line
	9350 4550 9450 4550
Connection ~ 9350 4850
Wire Wire Line
	9350 4550 9350 4250
Wire Wire Line
	9350 4250 9450 4250
Connection ~ 9350 4550
Wire Wire Line
	9350 4250 9350 3950
Wire Wire Line
	9350 3950 9450 3950
Connection ~ 9350 4250
Wire Wire Line
	9350 3950 9350 3650
Wire Wire Line
	9350 3650 9450 3650
Connection ~ 9350 3950
Wire Wire Line
	9350 3650 9350 3350
Wire Wire Line
	9350 3350 9450 3350
Connection ~ 9350 3650
Wire Wire Line
	9350 3350 9350 3050
Wire Wire Line
	9350 3050 9450 3050
Connection ~ 9350 3350
Wire Wire Line
	5400 3850 5500 3850
Wire Wire Line
	5500 3850 5500 2850
Wire Wire Line
	5500 2850 9450 2850
Wire Wire Line
	5400 3950 5600 3950
Wire Wire Line
	5600 3950 5600 2950
Wire Wire Line
	5600 2950 9150 2950
Wire Wire Line
	9150 2950 9150 3150
Wire Wire Line
	9150 3150 9450 3150
Wire Wire Line
	5400 4050 5700 4050
Wire Wire Line
	5700 4050 5700 3050
Wire Wire Line
	5700 3050 9050 3050
Wire Wire Line
	9050 3050 9050 3450
Wire Wire Line
	9050 3450 9450 3450
Wire Wire Line
	5400 4150 5800 4150
Wire Wire Line
	5400 4250 5900 4250
Wire Wire Line
	5400 4350 6000 4350
Wire Wire Line
	8950 3150 8950 3750
Wire Wire Line
	8950 3750 9450 3750
Wire Wire Line
	8850 3250 8850 4050
Wire Wire Line
	8850 4050 9450 4050
Wire Wire Line
	8750 3350 8750 4350
Wire Wire Line
	8750 4350 9450 4350
Wire Wire Line
	5400 6050 9150 6050
Wire Wire Line
	9150 6050 9150 5850
Wire Wire Line
	9150 5850 9450 5850
Wire Wire Line
	5400 5950 9050 5950
Wire Wire Line
	9050 5950 9050 5550
Wire Wire Line
	9050 5550 9450 5550
Wire Wire Line
	5400 5850 8950 5850
Wire Wire Line
	8950 5850 8950 5250
Wire Wire Line
	8950 5250 9450 5250
Wire Wire Line
	5400 5750 8850 5750
Wire Wire Line
	8850 5750 8850 4950
Wire Wire Line
	8850 4950 9450 4950
Wire Wire Line
	5400 5650 8750 5650
Wire Wire Line
	8750 5650 8750 4650
Wire Wire Line
	8750 4650 9450 4650
Wire Wire Line
	5400 6150 9450 6150
Wire Wire Line
	6750 7250 6750 6850
Wire Wire Line
	6650 7250 6650 6750
Connection ~ 2700 800 
Wire Wire Line
	2300 1100 2300 1300
Wire Wire Line
	2700 1200 2700 1300
$Comp
L power:Earth #PWR05
U 1 1 605D68F6
P 2300 1300
F 0 "#PWR05" H 2300 1050 50  0001 C CNN
F 1 "Earth" H 2300 1150 50  0001 C CNN
F 2 "" H 2300 1300 50  0001 C CNN
F 3 "~" H 2300 1300 50  0001 C CNN
	1    2300 1300
	1    0    0    -1  
$EndComp
$Comp
L power:Earth #PWR06
U 1 1 605D6107
P 2700 1300
F 0 "#PWR06" H 2700 1050 50  0001 C CNN
F 1 "Earth" H 2700 1150 50  0001 C CNN
F 2 "" H 2700 1300 50  0001 C CNN
F 3 "~" H 2700 1300 50  0001 C CNN
	1    2700 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 900  2700 800 
Wire Wire Line
	2600 800  2700 800 
$Comp
L device:C C3
U 1 1 605898C6
P 2700 1050
F 0 "C3" H 2650 1400 50  0000 L CNN
F 1 "1uF" V 2850 950 50  0000 L CNN
F 2 "" H 2738 900 50  0001 C CNN
F 3 "" H 2700 1050 50  0001 C CNN
	1    2700 1050
	1    0    0    -1  
$EndComp
$Comp
L regul:LM1117-5.0 U2
U 1 1 60587CBC
P 2300 800
F 0 "U2" H 2300 1042 50  0000 C CNN
F 1 "LM1117-5.0" H 2300 951 50  0000 C CNN
F 2 "" H 2300 800 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm1117.pdf" H 2300 800 50  0001 C CNN
	1    2300 800 
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 2000 3400 3850
Connection ~ 900  3850
Wire Wire Line
	900  3850 900  800 
Wire Wire Line
	3400 3950 3400 4150
Wire Wire Line
	900  3850 900  6750
Connection ~ 3400 3950
Wire Wire Line
	3400 3950 3400 3850
Wire Wire Line
	6750 6850 6850 6850
Wire Wire Line
	6850 6850 6850 6350
Wire Wire Line
	6850 6350 9350 6350
Connection ~ 6750 6850
Connection ~ 9350 6350
Wire Wire Line
	9250 6250 6750 6250
Wire Wire Line
	6750 6250 6750 6750
Wire Wire Line
	6750 6750 6650 6750
Connection ~ 9250 6250
Connection ~ 6650 6750
Connection ~ 6100 6850
Wire Wire Line
	3400 6850 5250 6850
Wire Wire Line
	6200 6750 6650 6750
Connection ~ 6200 6750
Wire Wire Line
	900  6750 6200 6750
Wire Wire Line
	6100 7250 6100 6850
Wire Wire Line
	6200 7250 6200 6750
$Comp
L conn:Conn_01x03_Female J3
U 1 1 5F39ED37
P 6200 7450
F 0 "J3" V 6250 7400 50  0000 L CNN
F 1 "SERIAL OUT" V 6350 7450 50  0000 C CNN
F 2 "" H 6200 7450 50  0001 C CNN
F 3 "~" H 6200 7450 50  0001 C CNN
	1    6200 7450
	0    1    1    0   
$EndComp
Wire Wire Line
	6300 5550 5400 5550
Wire Wire Line
	6300 6200 6300 5550
$Comp
L device:R R3
U 1 1 5F3BA0BE
P 6300 6350
F 0 "R3" H 6370 6396 50  0000 L CNN
F 1 "1k" H 6370 6305 50  0000 L CNN
F 2 "" V 6230 6350 50  0001 C CNN
F 3 "" H 6300 6350 50  0001 C CNN
	1    6300 6350
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5800 3150 8950 3150
Wire Wire Line
	6000 3350 8750 3350
Wire Wire Line
	5900 3250 8850 3250
Connection ~ 6100 4100
Wire Wire Line
	5400 5300 5550 5300
Connection ~ 6100 5300
Wire Wire Line
	6100 5300 6100 4100
Wire Wire Line
	5400 5200 8250 5200
Wire Wire Line
	5400 5100 8050 5100
Wire Wire Line
	5400 5000 7950 5000
Wire Wire Line
	5400 4900 7850 4900
Wire Wire Line
	5400 4800 7650 4800
Wire Wire Line
	900  800  2000 800 
Wire Wire Line
	7650 4800 7650 2500
Wire Wire Line
	10750 2600 10750 2700
Connection ~ 10750 2600
Wire Wire Line
	10550 2600 10750 2600
Wire Wire Line
	10050 2600 10250 2600
Wire Wire Line
	10050 2500 10050 2600
$Comp
L device:R R6
U 1 1 60DA715E
P 10400 2600
F 0 "R6" V 10500 2600 50  0000 C CNN
F 1 "R" V 10300 2600 50  0000 C CNN
F 2 "" V 10330 2600 50  0001 C CNN
F 3 "" H 10400 2600 50  0001 C CNN
	1    10400 2600
	0    1    -1   0   
$EndComp
Wire Wire Line
	10750 2200 10750 2600
Connection ~ 10750 2200
Wire Wire Line
	10650 2200 10750 2200
Wire Wire Line
	10750 2000 10750 2200
Wire Wire Line
	10650 2000 10750 2000
Wire Wire Line
	8250 2500 8250 5200
Wire Wire Line
	8050 2500 8050 5100
Wire Wire Line
	7950 2500 7950 5000
Wire Wire Line
	7850 2500 7850 4900
$Comp
L device:C C7
U 1 1 5FA46930
P 5150 2250
F 0 "C7" H 5200 2100 50  0000 L CNN
F 1 "100nF" V 5000 2150 50  0000 L CNN
F 2 "" H 5188 2100 50  0001 C CNN
F 3 "" H 5150 2250 50  0001 C CNN
	1    5150 2250
	1    0    0    -1  
$EndComp
$Comp
L power:Earth #PWR015
U 1 1 5FBEF1AD
P 10750 2700
F 0 "#PWR015" H 10750 2450 50  0001 C CNN
F 1 "Earth" H 10750 2550 50  0001 C CNN
F 2 "" H 10750 2700 50  0001 C CNN
F 3 "~" H 10750 2700 50  0001 C CNN
	1    10750 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 1400 5450 1500
Wire Wire Line
	5550 1400 5550 1500
Wire Wire Line
	5650 1400 5650 1500
Wire Wire Line
	5750 1400 5750 1500
Wire Wire Line
	5850 1400 5850 1500
Wire Wire Line
	5950 1400 5950 1500
Wire Wire Line
	6050 1400 6050 1500
Wire Wire Line
	6150 1400 6150 1500
Wire Wire Line
	6250 1400 6250 1500
Wire Wire Line
	6350 1400 6350 1500
Wire Wire Line
	6450 1400 6450 1500
Wire Wire Line
	6550 1400 6550 1500
Wire Wire Line
	6750 1400 6750 1500
Wire Wire Line
	6850 1400 6850 1500
Wire Wire Line
	6950 1400 6950 1500
Wire Wire Line
	7050 1400 7050 1500
Wire Wire Line
	7150 1400 7150 1500
Wire Wire Line
	7250 1400 7250 1500
Wire Wire Line
	7350 1400 7350 1500
Wire Wire Line
	7450 1400 7450 1500
Wire Wire Line
	7550 1400 7550 1500
Wire Wire Line
	7650 1400 7650 1500
Wire Wire Line
	7750 1400 7750 1500
Wire Wire Line
	7850 1400 7850 1500
Wire Wire Line
	8050 1400 8050 1500
Wire Wire Line
	8150 1400 8150 1500
Wire Wire Line
	8250 1400 8250 1500
Wire Wire Line
	8350 1400 8350 1500
Wire Wire Line
	8450 1400 8450 1500
Wire Wire Line
	8550 1400 8550 1500
Wire Wire Line
	8650 1400 8650 1500
Wire Wire Line
	8750 1400 8750 1500
Wire Wire Line
	8850 1400 8850 1500
Wire Wire Line
	8950 1400 8950 1500
Wire Wire Line
	9050 1400 9050 1500
Wire Wire Line
	9150 1400 9150 1500
Wire Wire Line
	9350 1400 9350 1500
Wire Wire Line
	9450 1400 9450 1500
Wire Wire Line
	9550 1400 9550 1500
Wire Wire Line
	9650 1400 9650 1500
Wire Wire Line
	9750 1400 9750 1500
Wire Wire Line
	9850 1400 9850 1500
Wire Wire Line
	9950 1400 9950 1500
Wire Wire Line
	10050 1400 10050 1500
Wire Wire Line
	10150 1400 10150 1500
Wire Wire Line
	10250 1400 10250 1500
Wire Wire Line
	10350 1400 10350 1500
Wire Wire Line
	10450 1400 10450 1500
$Comp
L Driver_LED:TLC5957RTQ U4
U 1 1 60698778
P 7950 2000
F 0 "U4" V 7500 3900 50  0000 L CNN
F 1 "TLC5957RTQ" V 7500 4100 50  0000 L CNN
F 2 "Package_DFN_QFN:QFN-56-1EP_8x8mm_P0.5mm_EP5.6x5.6mm_ThermalVias" H 8100 -800 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/tlc5957.pdf" H 7950 -1100 50  0001 C CNN
	1    7950 2000
	0    -1   -1   0   
$EndComp
Connection ~ 6550 800 
Connection ~ 6750 800 
Wire Wire Line
	6750 800  6550 800 
Connection ~ 7150 800 
Connection ~ 7250 800 
Wire Wire Line
	7250 800  7150 800 
Connection ~ 7850 800 
Connection ~ 8050 800 
Wire Wire Line
	8050 800  7850 800 
Connection ~ 8850 800 
Connection ~ 8950 800 
Wire Wire Line
	8950 800  8850 800 
Connection ~ 9150 800 
Connection ~ 9350 800 
Wire Wire Line
	9350 800  9150 800 
Connection ~ 10350 800 
Wire Wire Line
	10450 800  10450 900 
Wire Wire Line
	10350 800  10450 800 
Connection ~ 10250 800 
Wire Wire Line
	10350 800  10350 900 
Wire Wire Line
	10250 800  10350 800 
Connection ~ 10150 800 
Wire Wire Line
	10250 800  10250 900 
Wire Wire Line
	10150 800  10250 800 
Connection ~ 10050 800 
Wire Wire Line
	10150 800  10150 900 
Wire Wire Line
	10050 800  10150 800 
Connection ~ 9950 800 
Wire Wire Line
	10050 800  10050 900 
Wire Wire Line
	9950 800  10050 800 
Connection ~ 9850 800 
Wire Wire Line
	9950 800  9950 900 
Wire Wire Line
	9850 800  9950 800 
Connection ~ 9750 800 
Wire Wire Line
	9850 800  9850 900 
Wire Wire Line
	9750 800  9850 800 
Connection ~ 9650 800 
Wire Wire Line
	9750 800  9750 900 
Wire Wire Line
	9650 800  9750 800 
Connection ~ 9550 800 
Wire Wire Line
	9650 800  9650 900 
Wire Wire Line
	9550 800  9650 800 
Connection ~ 9450 800 
Wire Wire Line
	9550 800  9550 900 
Wire Wire Line
	9450 800  9550 800 
Wire Wire Line
	9450 800  9450 900 
Wire Wire Line
	9350 800  9450 800 
Wire Wire Line
	9350 800  9350 900 
Connection ~ 9050 800 
Wire Wire Line
	9150 800  9150 900 
Wire Wire Line
	9050 800  9150 800 
Wire Wire Line
	9050 800  9050 900 
Wire Wire Line
	8950 800  9050 800 
Wire Wire Line
	8950 800  8950 900 
Connection ~ 8750 800 
Wire Wire Line
	8850 800  8850 900 
Wire Wire Line
	8750 800  8850 800 
Connection ~ 8650 800 
Wire Wire Line
	8750 800  8750 900 
Wire Wire Line
	8650 800  8750 800 
Connection ~ 8550 800 
Wire Wire Line
	8650 800  8650 900 
Wire Wire Line
	8550 800  8650 800 
Connection ~ 8450 800 
Wire Wire Line
	8550 800  8550 900 
Wire Wire Line
	8450 800  8550 800 
Connection ~ 8350 800 
Wire Wire Line
	8450 800  8450 900 
Wire Wire Line
	8350 800  8450 800 
Connection ~ 8250 800 
Wire Wire Line
	8350 800  8350 900 
Wire Wire Line
	8250 800  8350 800 
Connection ~ 8150 800 
Wire Wire Line
	8250 800  8250 900 
Wire Wire Line
	8150 800  8250 800 
Wire Wire Line
	8150 800  8150 900 
Wire Wire Line
	8050 800  8150 800 
Wire Wire Line
	8050 800  8050 900 
Connection ~ 7750 800 
Wire Wire Line
	7850 800  7850 900 
Wire Wire Line
	7750 800  7850 800 
Connection ~ 7650 800 
Wire Wire Line
	7750 800  7750 900 
Wire Wire Line
	7650 800  7750 800 
Connection ~ 7550 800 
Wire Wire Line
	7650 800  7650 900 
Wire Wire Line
	7550 800  7650 800 
Connection ~ 7450 800 
Wire Wire Line
	7550 800  7550 900 
Wire Wire Line
	7450 800  7550 800 
Connection ~ 7350 800 
Wire Wire Line
	7450 800  7450 900 
Wire Wire Line
	7350 800  7450 800 
Wire Wire Line
	7350 800  7350 900 
Wire Wire Line
	7250 800  7350 800 
Wire Wire Line
	7250 800  7250 900 
Connection ~ 7050 800 
Wire Wire Line
	7150 800  7150 900 
Wire Wire Line
	7050 800  7150 800 
Connection ~ 6950 800 
Wire Wire Line
	7050 800  7050 900 
Wire Wire Line
	6950 800  7050 800 
Connection ~ 6850 800 
Wire Wire Line
	6950 800  6950 900 
Wire Wire Line
	6850 800  6950 800 
Wire Wire Line
	6850 800  6850 900 
Wire Wire Line
	6750 800  6850 800 
Wire Wire Line
	6750 800  6750 900 
Connection ~ 6450 800 
Wire Wire Line
	6550 800  6550 900 
Wire Wire Line
	6450 800  6550 800 
Connection ~ 6350 800 
Wire Wire Line
	6450 800  6450 900 
Wire Wire Line
	6350 800  6450 800 
Connection ~ 6250 800 
Wire Wire Line
	6350 800  6350 900 
Wire Wire Line
	6250 800  6350 800 
Connection ~ 6150 800 
Wire Wire Line
	6250 800  6250 900 
Wire Wire Line
	6150 800  6250 800 
Connection ~ 6050 800 
Wire Wire Line
	6150 800  6150 900 
Wire Wire Line
	6050 800  6150 800 
Connection ~ 5950 800 
Wire Wire Line
	6050 800  6050 900 
Wire Wire Line
	5950 800  6050 800 
Connection ~ 5850 800 
Wire Wire Line
	5950 800  5950 900 
Wire Wire Line
	5850 800  5950 800 
Connection ~ 5750 800 
Wire Wire Line
	5850 800  5850 900 
Wire Wire Line
	5750 800  5850 800 
Connection ~ 5650 800 
Wire Wire Line
	5750 800  5750 900 
Wire Wire Line
	5650 800  5750 800 
Wire Wire Line
	5650 800  5650 900 
Wire Wire Line
	5550 800  5650 800 
Wire Wire Line
	5550 900  5550 800 
$Comp
L conn:Conn_02x12_Counter_Clockwise J2
U 1 1 609BAB12
P 5950 1200
F 0 "J2" H 6046 512 50  0000 R CNN
F 1 "Opto-Relays LEDs" V 6450 1550 50  0000 R CNN
F 2 "" H 5950 1200 50  0001 C CNN
F 3 "~" H 5950 1200 50  0001 C CNN
	1    5950 1200
	0    -1   -1   0   
$EndComp
$Comp
L conn:Conn_02x12_Counter_Clockwise J6
U 1 1 609C15A1
P 7250 1200
F 0 "J6" H 7346 512 50  0000 R CNN
F 1 "Opto-Relays LEDs" V 7750 1550 50  0000 R CNN
F 2 "" H 7250 1200 50  0001 C CNN
F 3 "~" H 7250 1200 50  0001 C CNN
	1    7250 1200
	0    -1   -1   0   
$EndComp
$Comp
L conn:Conn_02x12_Counter_Clockwise J7
U 1 1 609C3C65
P 8550 1200
F 0 "J7" H 8646 512 50  0000 R CNN
F 1 "Opto-Relays LEDs" V 9050 1550 50  0000 R CNN
F 2 "" H 8550 1200 50  0001 C CNN
F 3 "~" H 8550 1200 50  0001 C CNN
	1    8550 1200
	0    -1   -1   0   
$EndComp
$Comp
L conn:Conn_02x12_Counter_Clockwise J8
U 1 1 609C6DA0
P 9850 1200
F 0 "J8" H 9946 512 50  0000 R CNN
F 1 "Opto-Relays LEDs" V 10350 1550 50  0000 R CNN
F 2 "" H 9850 1200 50  0001 C CNN
F 3 "~" H 9850 1200 50  0001 C CNN
	1    9850 1200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5550 800  5450 800 
Wire Wire Line
	5450 800  5450 900 
Connection ~ 5550 800 
Wire Wire Line
	5250 2000 5150 2000
Wire Wire Line
	5150 2000 5150 2100
Connection ~ 5450 800 
Wire Wire Line
	5150 2000 3400 2000
Connection ~ 5150 2000
$Comp
L power:Earth #PWR010
U 1 1 60B0D0ED
P 5150 2500
F 0 "#PWR010" H 5150 2250 50  0001 C CNN
F 1 "Earth" H 5150 2350 50  0001 C CNN
F 2 "" H 5150 2500 50  0001 C CNN
F 3 "~" H 5150 2500 50  0001 C CNN
	1    5150 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 2400 5150 2500
Wire Wire Line
	2700 800  5450 800 
$Comp
L power:+5V #PWR014
U 1 1 5F3A6341
P 7050 3600
F 0 "#PWR014" H 7050 3450 50  0001 C CNN
F 1 "+5V" H 7065 3773 50  0000 C CNN
F 2 "" H 7050 3600 50  0001 C CNN
F 3 "" H 7050 3600 50  0001 C CNN
	1    7050 3600
	1    0    0    -1  
$EndComp
$Comp
L device:R R5
U 1 1 5F54FFDC
P 7050 4100
F 0 "R5" H 7120 4146 50  0000 L CNN
F 1 "1k" H 7120 4055 50  0000 L CNN
F 2 "" V 6980 4100 50  0001 C CNN
F 3 "" H 7050 4100 50  0001 C CNN
	1    7050 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 5300 7050 5300
Wire Wire Line
	7050 5300 7050 4250
Wire Wire Line
	5800 3450 5800 3150
Wire Wire Line
	5800 4150 5800 3450
Connection ~ 5800 3450
Wire Wire Line
	6500 4350 6500 4250
Wire Wire Line
	6800 4500 6700 4500
Wire Wire Line
	6500 4750 6500 4650
$Comp
L device:Crystal_GND2 Y1
U 1 1 5F4B6C54
P 6500 4500
F 0 "Y1" V 6300 4350 50  0000 L CNN
F 1 "16MHz" V 6600 4050 50  0000 L CNN
F 2 "" H 6500 4500 50  0001 C CNN
F 3 "" H 6500 4500 50  0001 C CNN
	1    6500 4500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6350 4750 6500 4750
Wire Wire Line
	5400 4550 6350 4550
Wire Wire Line
	6350 4550 6350 4750
Wire Wire Line
	6350 4450 5400 4450
Wire Wire Line
	6500 4250 6350 4250
Wire Wire Line
	6350 4250 6350 4450
$Comp
L power:Earth #PWR08
U 1 1 5F40D320
P 3350 5000
F 0 "#PWR08" H 3350 4750 50  0001 C CNN
F 1 "Earth" H 3350 4850 50  0001 C CNN
F 2 "" H 3350 5000 50  0001 C CNN
F 3 "~" H 3350 5000 50  0001 C CNN
	1    3350 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 4450 3350 4450
Wire Wire Line
	3350 4900 3350 5000
Wire Wire Line
	3350 4450 3350 4600
$Comp
L device:C C6
U 1 1 5F40A1DE
P 3350 4750
F 0 "C6" H 3400 4600 50  0000 L CNN
F 1 "100nF" V 3500 4700 50  0000 L CNN
F 2 "" H 3388 4600 50  0001 C CNN
F 3 "" H 3350 4750 50  0001 C CNN
	1    3350 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 4650 6800 4500
$Comp
L power:Earth #PWR012
U 1 1 5F4CA2CE
P 6800 4650
F 0 "#PWR012" H 6800 4400 50  0001 C CNN
F 1 "Earth" H 6800 4500 50  0001 C CNN
F 2 "" H 6800 4650 50  0001 C CNN
F 3 "~" H 6800 4650 50  0001 C CNN
	1    6800 4650
	1    0    0    -1  
$EndComp
Connection ~ 7050 3700
Wire Wire Line
	7050 3950 7050 3700
Wire Wire Line
	7050 3700 7050 3600
Wire Wire Line
	6100 4100 6100 3900
Connection ~ 6000 3800
Wire Wire Line
	6000 3800 6000 3350
Wire Wire Line
	6000 4350 6000 3800
Connection ~ 5900 3700
Wire Wire Line
	5900 3700 5900 3250
Wire Wire Line
	5900 4250 5900 3700
Wire Wire Line
	6100 3900 6200 3900
Wire Wire Line
	6000 3800 6200 3800
Wire Wire Line
	6700 3700 7050 3700
Wire Wire Line
	5900 3700 6200 3700
$Comp
L conn:Conn_02x03_Odd_Even J4
U 1 1 5F3A34CC
P 6400 3800
F 0 "J4" H 6350 4050 50  0000 C CNN
F 1 "ICSP" H 6550 4050 50  0000 C CNN
F 2 "" H 6400 3800 50  0001 C CNN
F 3 "~" H 6400 3800 50  0001 C CNN
	1    6400 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 4100 6100 4100
$Comp
L switches:SW_Push SW1
U 1 1 5F3BB16F
P 6450 4100
F 0 "SW1" H 6250 4150 50  0000 C CNN
F 1 "RESET" H 6700 4150 50  0000 C CNN
F 2 "" H 6450 4300 50  0001 C CNN
F 3 "" H 6450 4300 50  0001 C CNN
	1    6450 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6850 3450 6850 3800
Wire Wire Line
	5800 3450 6850 3450
Wire Wire Line
	6700 3800 6850 3800
Wire Wire Line
	6850 4100 6850 3900
Wire Wire Line
	6650 4100 6850 4100
Connection ~ 6850 4100
Wire Wire Line
	6850 3900 6700 3900
$Comp
L power:Earth #PWR013
U 1 1 5F3AB3A5
P 6850 4150
F 0 "#PWR013" H 6850 3900 50  0001 C CNN
F 1 "Earth" H 6850 4000 50  0001 C CNN
F 2 "" H 6850 4150 50  0001 C CNN
F 3 "~" H 6850 4150 50  0001 C CNN
	1    6850 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6850 4150 6850 4100
Wire Wire Line
	6550 6500 6550 6550
Wire Wire Line
	6300 6500 6300 6650
Wire Wire Line
	6100 6850 6750 6850
$Comp
L device:R R2
U 1 1 60271B04
P 5850 6350
F 0 "R2" H 5920 6396 50  0000 L CNN
F 1 "1k" H 5920 6305 50  0000 L CNN
F 2 "" V 5780 6350 50  0001 C CNN
F 3 "" H 5850 6350 50  0001 C CNN
	1    5850 6350
	1    0    0    -1  
$EndComp
$Comp
L device:LED D2
U 1 1 60272395
P 5850 7050
F 0 "D2" V 5889 6933 50  0000 R CNN
F 1 "YELLOW" H 5950 7200 50  0000 R CNN
F 2 "" H 5850 7050 50  0001 C CNN
F 3 "" H 5850 7050 50  0001 C CNN
	1    5850 7050
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5400 4700 5850 4700
Wire Wire Line
	5850 4700 5850 6200
Wire Wire Line
	5850 6500 5850 6900
$Comp
L power:Earth #PWR011
U 1 1 602E080B
P 5850 7350
F 0 "#PWR011" H 5850 7100 50  0001 C CNN
F 1 "Earth" H 5850 7200 50  0001 C CNN
F 2 "" H 5850 7350 50  0001 C CNN
F 3 "~" H 5850 7350 50  0001 C CNN
	1    5850 7350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 7200 5850 7350
Wire Wire Line
	5250 7250 5250 6850
Connection ~ 5250 6850
Wire Wire Line
	5250 6850 6100 6850
Wire Wire Line
	6300 6650 5450 6650
Wire Wire Line
	5450 6650 5450 7250
Connection ~ 6300 6650
Wire Wire Line
	6300 6650 6300 7250
Wire Wire Line
	6550 6550 5350 6550
Wire Wire Line
	5350 6550 5350 7250
Connection ~ 6550 6550
Wire Wire Line
	6550 6550 6550 7250
$Comp
L device:C C8
U 1 1 6062C8A2
P 5550 6350
F 0 "C8" H 5600 6200 50  0000 L CNN
F 1 "100nF" V 5400 6250 50  0000 L CNN
F 2 "" H 5588 6200 50  0001 C CNN
F 3 "" H 5550 6350 50  0001 C CNN
	1    5550 6350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 6200 5550 5300
Connection ~ 5550 5300
Wire Wire Line
	5550 5300 6100 5300
$Comp
L conn:Conn_01x04_Female J1
U 1 1 6065583C
P 5450 7450
F 0 "J1" V 5500 7350 50  0000 L CNN
F 1 "USB-RS232" V 5600 7150 50  0000 L CNN
F 2 "" H 5450 7450 50  0001 C CNN
F 3 "~" H 5450 7450 50  0001 C CNN
	1    5450 7450
	0    1    1    0   
$EndComp
Wire Wire Line
	5550 6500 5550 7250
$EndSCHEMATC
