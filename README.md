## WARNING FOR RADIO USE
INSTALL ANTENNA FOR RADIO MODULES (can just be a wire) and DO NOT UNPLUG ANTENNA DURING TRANSMSISION!
This can destroy the chip as the TX power gets reflected and has nowhere to go.

## Latest Update
Feb. 3 (Jesse F., Marc R., Luis S., and Pete!) Began developing classes for sending and receiving packets via the LoRa radio, and using the SD card to log data. Tested using the MCP9600 and ICM20948 together over I2C and transmitting the data

Question: What should the format of the packets look like? (Header, type, payload, checksum, etc);

Jan. 31 (Victor H., Sam C., Koichi K., Jesse F.): Tested old Olympus PTs and found one rated for 2500 PSI working. Named it Pressi the PT. New radio modules work! Also tested the MCP9600 thermocouple chip and successfully read data from a K-type thermocouple.

Jan. 29 (Sam C.): quickly updated .gitignore to ignore .vscode and its .json configuration files to avoid merge issues. Since everyone's VSCode may be configured differently.

Jan. 26 (Victor H., Sam C., Jesse F., Luis S., and Pete!): double-confirmed RFM95 lack of functionality, debugged load cell amplifier breakout board
- Used oscilloscope to make sure the MISO signal isn't being pulled up
- Commit "It was the damn regulator" - Found out the load cell amplifier's voltage regulator isn't functioning properly. When regulator bypassed and +3V3 directly supplied to chip, load cell reading works perfectly.

Question: Could there be a similar issue for the RFM95?

Jan. 19 (Sam C., Jesse F., Luis S. Marc R., Victor H.): confirmed SPI hardware communication issue with both RFM95 modules. Test procedure:
- first tested old and new code with no success. Found issue with SPI read and write (Jan 17 update.)
- used multimeter to confirm 3V3 becomes ~100mV across the 3V3 and GND terminals of the RFM95 chip (Jan 17).
- Tested each pin of the SPI protocol with logic analyzers to confirm idle SPI signals (Jan 19).
- Tested with CircuitPython and still could not initialize the RFM object (Jan 19).

Concern: with a non-RFM95 object, Teensy SPI still doesn't function properly (SPI object can be created, but readings are all 0).

Jan. 17 (Sam C.): organized code for compiling and uploading capability across platforms. Issue with RF95 module instantiation still exists. Suspecting that the module itself may have hardware issues, since the 3V3 voltage measured directly from the RFM95 chip (pinout here: https://www.hoperf.com/uploads/RFM96W-V2.0_1695351477.pdf) is not 3V3.
The exact functions having issues are spiRead and spiWrite in lib/Radiohead-master/RHSPIDriver.h and lib/Radiohead-master/RHSPIDriver.cpp. The function using them can be found in lib/Radiohead-master/RH_RF95.h and lib/Radiohead-master/RH_RF95.cpp.

## Initial setup 
The repository relies on a VSCode Extension called PlatformIO, this makes it easier for us to upload and run code on the Teensy boards.
Ensure that you have the C/C++ and PlatformIO installed, and then go to the PlatformIO tab on the VSCode and pick the /teensy_software directory.
More in depth instructions: https://platformio.org/install/ide?install=vscode

## Compiling and Uploading
There are two ways to build and upload your code.
1. Using the terminal
```
// Compiling code
pio run

// Uploading code. Note: make sure the board is connected to your machine.
pio run --target
```
2. Using the VSCode icons.
You will be utilizing three icons. One for building the code, one for uploading the code, and one to activate the serial monitor.
The serial monitor allows you to see what the Teensy is outputting, very useful for debugging.
Just follow the gif below to know where these icons are.
<!-- MANPAGE: BEGIN EXCLUDED SECTION -->
<div align="center">
    <img width="500" alt="demo" src="https://github.com/stanford-ssi/hummingbird/tree/TeensySoftware/teensy_software/.github/build.gif">
</div>
<!-- MANPAGE: END EXCLUDED SECTION -->

## Note 
Make sure you have all the libraries needed for the BME680 to run if you are testing that. See main.cpp for more.
If you're running this on the Arduino IDE, get the necessary libraries directly from the library manager in Arduino IDE.
To see what board this folder of code supports, see platformio.init. For this board it's Teensy 4.1 with the Arduino framework.

