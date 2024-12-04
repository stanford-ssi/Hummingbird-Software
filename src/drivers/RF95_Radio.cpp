#include "RF95_Radio.h"

// Custom pinout for teensy 4.1
#define RFM95_CS     10  // "B"
#define RFM95_INT    0 // "C"
#define RFM95_RST    1  // "A"
#define RFM95_IRQN   RFM69_INT

#define BAUD_RATE 9600

class RF95_Radio(){
    
    // Constructor
    RF95_Radio::RF95_Radio(){
        Serial.begin(BAUD_RATE);

        rf95(RFM95_RST, RFM95_INT);  
        RF95_FREQ = 915.0;

        // Initialize RF95
        while (!rf95.init()) {
            Serial.println("LoRa radio init failed");
            Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
        }
        Serial.println("LoRa radio init OK!");

        // Initialize frequency
        while (!rf95.setFrequency(RF95_FREQ)){
           Serial.println("Frequency was not set, error!"); 
        }
        Serial.println("Succesfully set frequency to: ");
        Serial.print(RF95_FREQ);

        // Setting Transmit power
        rf.setTxPower(23, false);

        /* Note:
         * Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
         * The default transmitter power is 13dBm, using PA_BOOST.
         * If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
         * you can set transmitter powers from 5 to 23 dBm.
         */
    }

    void RF95_Radio::_getMessage(int bufferSize){
        const int secondBufferSize = 140;
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len)) {
          digitalWrite(LED_BUILTIN, HIGH);
          RH_RF95::printBuffer("Received: ", buf, len);
          Serial.print("Got: ");

          Serial.println((char*)buf);

          char secondBuffer[secondBufferSize];
          snprintf(buffer1, sizeof(buffer1), "The value of the received message is: %s\n", (char*)buf);

          Serial.print("RSSI: ");
          Serial.println(rf95.lastRssi(), DEC);

          _sendAck();
        } else {
            Serial.println("Receive failed!");
        } 
    }

    void RF95_Radio::_sendMessage(uint8_t packetLength, std::string message){
        const uint8_t NULL_SPACE = 1;

        delay(1000); // Delay added for each transmission
        
        char packet[packetLength + NULL_SPACE] = "";
        for (int i = 0; i < packetLength; i++){
            packet[i] = message[i];
        }
        
        Serial.println("Transmitting...");
        

    }

    void RF95_Radio::_changeFrequency(double freq){
         RF95_FREQ = freq;
        if (!rf95.setFrequency(RF95_FREQ)) {
            Serial.println("Setting new frequency failed!");
        }
    }

    void RF95_Radio::_sendACK(){
        // ACK message
        uint8_t data[] = "And hello back to you";
        
        // Sending the packet
        rf95.send(data, sizeof(data));
        rf95.waitPacketSent();
        Serial.println("Sent a reply");
        digitalWrite(LED_BUILTIN, LOW);
    }
}
