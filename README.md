# Arduino-Wiegand-DoorAccessControl
This project is a very simple application to use Arduino as an access control device. 

## How it works
Pin 8 is a pulled up input, if it is lowered to 0V, all the cards readed will be registered into memory. If pin 8 is not driven to 0V, the program will check if the card is registered into the memory, if it is, the lock pin will be thriggered. If it is not registered, nothing happens. The code is self explanatory... I know a lot of things could be better designed, contributions will be wellcome! 

## I/O Arduino Ports   
* [7]  Inside Door
* [8]  Switch to register cards (keep the switch to 0V to register)
* [9]  Card Reader Buzzer (optional)
* [10] Card Reader LED (optional)
* [11] Lock
* [12] Lock Sensor (not tested)

## Thanks to
* monkeyboard for the Wiegand-Protocol-Library-for-Arduino
