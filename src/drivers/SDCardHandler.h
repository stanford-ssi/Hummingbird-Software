#ifndef SDCARDHANDLER_H
#define SDCARDHANDLER_H

#include "Wire.h"
#include <SD.h>


class SDCardHandler {
public:
    SDCardHandler(int chipSelect = BUILTIN_SDCARD);
    bool beginSDcard(char* dataFile);
    bool writeToCard(float data);
    
private:
    int chipSelect;
    File myFile;
    char buffer[255];
};

#endif // SDCARDHANDLER_H
