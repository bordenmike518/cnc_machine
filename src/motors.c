#include "cnc.h"

Motor motors[NUM_AXES];
Coordinate cur_pos;
Coordinate new_pos;

uint8_t   mode_mm = 1,
          mode_abs = 1,
          input_index = 0; 
uint16_t  micro_steps_per_rotation = STEPS_PER_ROTATION * MICRO_STEPS,
          steps_per_mm = micro_steps_per_rotation / MM_PER_ROTATION;
double    mm_per_step = MM_PER_ROTATION / micro_steps_per_rotation;

void motors_init() {
    SETB(DDRB, 0);  // Enable / Disable
    SETB(DDRD, 2);  // X Step Pulse
    SETB(DDRD, 5);  // X Direction
    SETB(DDRD, 3);  // Y Step Pulse
    SETB(DDRD, 6);  // Y Direction
    //SETB(DDRD, PD4);  // Z Step Pulse
    //SETB(DDRD, PD7);  // Z Direction
    
    motors_disable();
}

void motors_enable(void) {
    SETB(PORTB, 0);
}

void motors_disable(void) {
    CLEARB(PORTB, 0);
}

void motors_step(char motor) {
    if (motor == "x" || motor == "X") {
        SETB(PORTD, 2);
        _delay_ms(5);
        CLEARB(PORTD, 2);
    }
    else if (motor == "y" || motor == "Y") {
        SETB(PORTD, 3);
        _delay_ms(5);
        CLEARB(PORTD, 3);
    }
    else if (motor == "z" || motor == "Z") {
        SETB(PORTD, 4);
        _delay_ms(5);
        CLEARB(PORTD, 4);
    }
}

void motors_move(Coordinate curPos, Coordinate newPos, uint16_t feedrate) {
    uint8_t  sdx,       // The sign of the change in X
             sdy;       // The sign of the change in Y
    uint32_t dx,        // Change in x
             dy,        // Change in y
             x = 0,     // X index
             y = 0;     // Y index
    float    m,         // Slope
             steps,     // Motor steps
             missed_steps;
    
    if (mode_mm == 1)
        steps = steps_per_mm;
    else
        steps = steps_per_mm * 25.4;
        
    if (mode_abs == 1) {
        dx = (newPos.x - curPos.x) * steps;
        dy = (newPos.y - curPos.y) * steps;
    }
    if (mode_abs == 0) {
        dx = newPos.x * steps;
        dy = newPos.y * steps;
    }

    if (dx > 0) {
        CLEARB(PORTD, 5);
        sx = 1;
    }
    else {
        SETB(PORTD, 5);
        sx = -1;
    }
    if (dy > 0) {
        SETB(PORTD, 6);
        sy = 1;
    }
    else {
        CLEARB(PORTD, 6);
        sy = -1;
    }

    dx = abs(dx);
    dy = abs(dy);
    
    if (dx == 0 || dy == 0)
      m = 0;
    else if (dx > dy)
      m = dy / float(dx);
    else
      m = dx / float(dy);

    if (dx >= dy) {
      for (; x < dx && limits() == false; x++) {
        smooth(x, dx);
        motors_step("x");
        curPos.x += sx * mm_per_step;
        missed_steps = (m * x) - y;
        if (missed_steps > 0) {
          motors_step("y");
          curPos.y += sy * mm_per_step;
          y++;
        }
      }
    }
    else {
      for (; y < dy && limits() == false; y++) {
        smooth(y, dy);
        motors_step("y");
        curPos.y += sy * mm_per_step;
        missed_steps = (m * y) - x;
        if (missed_steps > 0) {
          motors_step("x");
          curPos.x += sx * mm_per_step;
          x++;
        }
      }
    }
}

void home(void) {
    new_pos.x = 0; new_pos.y = 0;
    SETB(PORTD, 5);
    CLEAR(PORTD, 6);
    while (~GETB(PORTB, 1)) {
        motors_step("x");
        cur_pos.x -= mm_per_step;
    }
    while (GETB(PORTB, 1)) {
        CLEARB(PORTD, 5);
        delay(10);
        motors_step("x");
    }
    while (~GETB(PORTB, 2)) {
        motors_step("y");
        cur_pos.y -= mm_per_step;
    }
    while (GETB(PORTB, 2)) {
        SETB(PORTD, 6);
        delay(10);
        motors_step("y");
    }
    cur_pos.x = 0; cur_pos.y = 0; 
}

void set_position(float  x, float  y) {
    cur_pos.x = x;
    cur_pos.y = y;
}

void get_position(float *x, float *y) {
    *x = cur_pos.x;
    *y = cur_pos.y;
}
