#! /bin/bash
args=("$@")

avr-gcc -std=c11 -g -Os -mmcu=atmega328p -c usart.c 
avr-gcc -std=c11 -g -Os -mmcu=atmega328p -c limits.c 
avr-gcc -std=c11 -g -Os -mmcu=atmega328p -c motors.c 
avr-gcc -std=c11 -g -Os -mmcu=atmega328p -c gcode.c

ar cr libcnc.a usart.o limits.o motors.o gcode.o

avr-gcc -std=c11 -g -Os -mmcu=atmega328p -c cnc_machine.c

avr-gcc -g -mmcu=atmega328p cnc_machine.o libcnc.a -o cnc_machine.elf

avr-objcopy -j .text -j .data -O ihex cnc_machine.elf cnc_machine.hex

rm *.o
rm *.a
rm *.elf

