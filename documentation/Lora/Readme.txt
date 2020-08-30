Protocol Lora (Long Range) with OpenAVRc.

If you follow the MultiProtocol thread on Rcgroups, you know that it's possible to use a SX1276 RF module in 915Mhz or 868Mhz.

This system use the R9mm or R9 as receiver.

You can choose to buy a R9 EU receiver in 868Mhz or a R9 in 915Mhz for US.

For the Tx side, the project use a RFM95W who has a RFM95W in 868Mhz or 915Mhz.

The RFM95W accept only 3,3v levels.
If you have already a 4in1 module with STM32, you don't need to use a level convert.
If you use a DIY 4in1 with atmega328p 3,3v, you don't need to use a level convert.
If you use a DIY 4in1 with atmega328p 5v, you need to use a level convert.

See the schematic file PCB v2.3&Lora.pdf for connections.

The MPM code add the Lora feature only with a STM32 who has enough pins for command cc2500, a7105, cyrf6936, nrf24l01+ and rfm95w in same time.

Whis the atmega328, we need to do a choice. Our choice is to use the CS (chip select) of the cyrf6936.

Firmware Frsky R9:
You need to flash your R9 receiver with one of the two firmwares FCC or FLEX.
Our choice is to use FLEX.
You will find needed files in the R9_FCC and R9_FLEX folder with a special F-PORT version for each. 

Firmware MPM:
I have modified the MPM code for use an atmega328p (see Multiprotocol_Lora328P.zip MPM modified sources files).
On OpenAVRc side, two solutions:
1- Use your actual firmware an MultiMod protocol with custom 63 1(16ch without telemetry) or 63 3(8ch with telemetry).
2- Compile a new firmware with OpenAVRc_Builder, add MultiSX1276=YES in your build options.

The official value in MPM isn't 63 but 65. But the OpenAVRc need to be updated for accept up to 63 values.

Attention, you don't use a CYRF6936 chip in same time than RFM95W because the RFM95W use the same atmega328's CS pin .

**************************************************************************************************************
* Use this new feature if you want but not in France.                                                        *
* OpenAVRc's github is updated with this feature.                                                            *
**************************************************************************************************************

Hope you will like more OpenAVRc with this option.

@OpenAVRc Team