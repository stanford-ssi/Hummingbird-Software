#include "SDCardHandler.h"


// Instantiate the card hnadler, pass in BUILTIN_SDCARD
SDCardHandler::SDCardHandler(int chipSelect) {
    this->chipSelect = chipSelect;
}


// Start the SD card connection if returns false then error occured
// In main code make sure this passes or else throw error
bool SDCardHandler::beginSDcard(char* dataFile) {
    if (!SD.begin(chipSelect)) {
        return false; // Return false if SD initialization fails
    }
    myFile = SD.open(dataFile, FILE_WRITE);
    return myFile ? true : false;
}

// Write data to file
// TODO: figure out datatype for data & if we want to add a specified file/master file to store data
bool SDCardHandler::writeToCard(float data) {
    if (!myFile) return false; // Ensure file is open

    snprintf(buffer, sizeof(buffer), "The value of heat is: %.2f\n", data);
    myFile.write(buffer);
    myFile.flush();
    return true;
}
