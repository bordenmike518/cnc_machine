# cnc_machine

CNC Machine which will etch PCBs for rapid prototyping.

This is a G-Code interpreter for a cnc machine that I am working on.
It will be used to etch PCBs with a drill for in house prototyping.
There is a python terminal program that is used to connect over serial.

TODO :: Remove any Arduino dependant functions and use AVR library.
Completed
1. USART Serial communication.

Not Completed
1. Pin manipulation for stepper motor control.
2. Change delayMicroseconds -> _delay_ms.
3. Abstraction of files.
