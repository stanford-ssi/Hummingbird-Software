#include "Arduino.h"
#include "RF95_Radio.h"

RF95_Radio* radio = nullptr;

// The message will be written here
uint8_t some_buffer_size = 100;
uint8_t msg[100];

void setup(){
    Serial.begin(9600);

    /* You have to initialize a RH_RF95 pointer
     * If you are reading this, the RF95_Radio
     * object can't create its own instance of
     * RH_RF95 without hanging. There is a problem
     * with .init() where it hangs, and the current
     * solution is just creating a dynamically allocated
     * instance of RH_RF95 object and pass it through the
     * RF95_Radio object.
     */ 
    RH_RF95* rf95 = new RH_RF95(10, 0);
    rf95->init();
    rf95->setFrequency(915.0);
    rf95->setTxPower(23, false);

    radio = new RF95_Radio(rf95);
}

void loop(){
    radio->_getMessage(msg);
    delay(1000);

    Serial.println(msg);
}
