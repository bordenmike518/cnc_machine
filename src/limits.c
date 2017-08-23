#include "cnc.h"

void limits_init(void) {
    CLEARB(DDRB, 1); // X Input 
    SETB(PORTB, 1);  // X Pull up resistor
    CLEARB(DDRB, 2); // Y Input
    SETB(PORTB, 2);  // Y Pull up resistor
    CLEARB(DDRB, 3); // Z Input
    SETB(PORTB, 3);  // Z Pull up resistor
}

bool limits_get(void) {
    uint8_t i = 1;
    for (; i < 4; i++) {
        if (~GETB(PINB, i))
            return true;
        else
            return false;
    }
}
