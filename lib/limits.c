#include "cnc.h"

void limits_init(void) {
    CLEARB(DDRB, 1); // X Limit 
    SETB(PORTB, 1);
    CLEARB(DDRB, 2); // Y Limit
    SETB(PORTB, 2);
    CLEARB(DDRB, 3); // Z Limit
    SETB(PORTB, 3);
}

void limits(void) {
    uint8_t i = 1;
    for (; i < 4; i++) {
        if (GETB(PINB, i))
            return true;
        else
            return false;
    }
}
