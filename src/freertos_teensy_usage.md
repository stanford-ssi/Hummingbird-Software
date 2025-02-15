Last updated: Feb. 14, 2025 by Sam C.

1. The FreeRTOS library for the Teensy 4.1 board is included in '''lib/freertos-teensy-lib''' as a submodule. To use the submodule, navigate to lib and use the command
'''
> git submodule init
'''
And the next time you pull from GitHub, you cancdo 
'''
git pull --recursive-submodules
''' 
to ensure that the submodule is up to date. See this website for more details: https://gist.github.com/gitaarik/8735255

2. To use the FreeRTOS library, you must include '''arduino_freertos.h''' and '''avr/pgmspace.h'''. This library overwrites existing Arduino settings, so any Arduino constants you use (OUTPUT, INPUT, HIGH, LOW, etc.) needs to be defined with '''arduino::''' to indicate that it's part of the Arduino library. You can also include the  '''using namespace arduino;''' line at the top of the file you're running to avoid this issue.

3. Different FreeRTOS functions require different libraries to be included. For example, to use mutexes and semaphores, you need to include "semphr.h". Check '''freertos-teensy-lib/src''' to see which files should be added.

4. The FreeRTOS library also does a weird thing with code that blocks until '''Serial''' is available. Including any form of code of this sort basically stalls the program forever. So you want to avoid any calls like '''while (!(Serial.available()))''' in your '''void setup()''' function. Also, in '''void setup()''' and every task you want to run Serial in (see point 4), initialize '''Serial''' by:
'''
Serial.begin(0);
delay(2'000);
'''

5. With each task created with '''xTaskCreate()''', you will need to re-initiate '''Serial''' if you're intending to print with the task.

6. I'll add anything to here if other troubles arise. Good luck and have fun with multithreading!