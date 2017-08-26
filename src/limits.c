#include "cnc.h"

void limits_init(void) {
    CLEARB(DDRB, PB1); // X Input 
    SETB(PORTB, PB1);  // X Pull up resistor
    CLEARB(DDRB, PB2); // Y Input
    SETB(PORTB, PB2);  // Y Pull up resistor
    //CLEARB(DDRB, PB3); // Z Input
    //SETB(PORTB, PB3);  // Z Pull up resistor
}

bool limits_get(void) {
    uint8_t i = 1;
    for (; i < 3; i++) {    // Only checking PB1 & PB2. No Z axis yet
        if (~GETB(PINB, i))
            return true;
        else
            return false;
    }
}
