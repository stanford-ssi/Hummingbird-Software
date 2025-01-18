## Latest Update
Jan. 17 (Sam C.): organized code for compiling and uploading capability across platforms. Issue with RF95 module instantiation still exists. Suspecting that the module itself may have hardware issues, since the 3V3 voltage measured directly from the RFM95 chip (pinout here: https://www.hoperf.com/uploads/RFM96W-V2.0_1695351477.pdf) is not 3V3.

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

