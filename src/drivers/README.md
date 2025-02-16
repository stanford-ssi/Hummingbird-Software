# History logs of Command Handler Developement:

## Log 1
Created base template for Command Handler files. The Command Handler will be handling sending, receiving, and calling functions/commands. The Command Handler relies on the radio driver to send and receive the base messages.
### radio_packet_t
The radio_packet_t struct stores three vital pieces of information: The packet's type (Command or data), the packet's length, and the packet's data/messsage. This will be the main struct that will be passed around to make it easier to write/access these vital packet information. 
A more intricate implementation of radio_packet_t will be implemented soon, but for simplicity sake and proof of concept we have this. 
### Functionality
The Command Handler file does not have anything functionality for now, nor is the direction of the functions in the file set in stone. But the very basics of it is to be able to send, receive, and process some sort of packet. This will be extended in future iterations, the radio module will have to be changed no matter what to provide broaden the capabilities of the Command Handler.

## Log 2
Merged Command Handler into the drivers directory. The radio_packet_struct.h was created to pass the struct declaration in both the radio and command handler. 
### RF95_Radio::_getMessage()
Planning on remaking the _getMessage() function to write to a buffer outside of the function, rathet than locally on the function. This enables us to be able to access and save the information that was received, rather than simply printing it out. This functionality is crucial if we want the command handler to do what it needs to do.
### To dos
Implementing a version of _getMessage() function would be the first step. Being able to access the buffer is crucial in many aspects of our codebase (Storing data, interpreting commands, creating logs, etc.) After implementing _getMessage(), implementing a way to process and execute commands given received by the command handler. Also implement a way to log all of these actions such that we have a reliable logging system.
