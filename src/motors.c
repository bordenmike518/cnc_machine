#include "cnc.h"

uint8_t   mode_mm = 1,
          mode_abs = 1; 

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
    if (motor == 'x' || motor == 'X') {
        SETB(PORTD, 2);
        _delay_ms(5);
        CLEARB(PORTD, 2);
    }
    else if (motor == 'y' || motor == 'Y') {
        SETB(PORTD, 3);
        _delay_ms(5);
        CLEARB(PORTD, 3);
    }
    else if (motor == 'z' || motor == 'Z') {
        SETB(PORTD, 4);
        _delay_ms(5);
        CLEARB(PORTD, 4);
    }
}

void motors_move(void) {
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
        steps = STEPS_PER_MM;
    else
        steps = STEPS_PER_MM * 25.4;
        
    if (mode_abs == 1) {
        dx = (new_pos.x - cur_pos.x) * steps;
        dy = (new_pos.y - cur_pos.y) * steps;
    }
    if (mode_abs == 0) {
        dx = new_pos.x * steps;
        dy = new_pos.y * steps;
    }

    if (dx > 0) {
        CLEARB(PORTD, 5);
        sdx = 1;
    }
    else {
        SETB(PORTD, 5);
        sdx = -1;
    }
    if (dy > 0) {
        SETB(PORTD, 6);
        sdy = 1;
    }
    else {
        CLEARB(PORTD, 6);
        sdy = -1;
    }

    dx = abs(dx);
    dy = abs(dy);
    
    if (dx == 0 || dy == 0)
      m = 0;
    else if (dx > dy)
      m = dy / (float)dx;
    else
      m = dx / (float)dy;

    if (dx >= dy) {
      for (; x < dx && limits_get() == false; x++) {
        motors_step('x');
        cur_pos.x += sdx * MM_PER_STEP;
        missed_steps = (m * x) - y;
        if (missed_steps > 0) {
          motors_step('y');
          cur_pos.y += sdy * MM_PER_STEP;
          y++;
        }
      }
    }
    else {
      for (; y < dy && limits_get() == false; y++) {
        motors_step('y');
        cur_pos.y += sdy * MM_PER_STEP;
        missed_steps = (m * y) - x;
        if (missed_steps > 0) {
          motors_step('x');
          cur_pos.x += sdx * MM_PER_STEP;
          x++;
        }
      }
    }
}

void home(void) {
    new_pos.x = 0; new_pos.y = 0;
    SETB(PORTD, 5);
    CLEARB(PORTD, 6);
    while (~GETB(PORTB, 1)) {
        motors_step('x');
        cur_pos.x -= MM_PER_STEP;
    }
    while (GETB(PORTB, 1)) {
        CLEARB(PORTD, 5);
        _delay_ms(10);
        motors_step('x');
    }
    while (~GETB(PORTB, 2)) {
        motors_step('y');
        cur_pos.y -= MM_PER_STEP;
    }
    while (GETB(PORTB, 2)) {
        SETB(PORTD, 6);
        _delay_ms(10);
        motors_step('y');
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

uint8_t get_mm_or_iches(void) {
    return mode_mm;
}
void set_mode_mm(void) {
    mode_mm = 0;
}
void set_mode_inches(void) {
    mode_mm = 1;
}

uint8_t get_abs_or_rel(void) {
    return mode_abs;
}
void set_mode_abs(void) {
    mode_abs = 1;
}
void set_mode_rel(void) {
    mode_abs = 0;
}

void my_delay_ms(uint32_t milliseconds) {
    uint32_t i = 0, centiseconds = milliseconds % 10;
    for(; i < centiseconds; i++) {
        _delay_ms(10);
    }
}

void timing(uint32_t step_total, uint32_t step_current, uint8_t *last_sign) {
    uint32_t beginning_end, ending_start,
             two_mm = 2 * STEPS_PER_MM, buff;
    double delay_count = 1000, delay_time;
    if (step_total < two_mm) {
        beginning_end = step_total/2.0;
        ending_start = beginning_end;
        delay_time = delay_count / beginning_end;
    }
    else {
        beginning_end = two_mm;
        ending_start = step_total - two_mm;
        delay_time = delay_count / two_mm;
    }
    uint8_t speed_percent = 50;
    if (speed_percent >= 50 && speed_percent <= 100) {
      if (step_current < beginning_end) {
          buff = delay_count - (step_current * (uint32_t)delay_time);
          my_delay_ms(buff);
      }
      if (step_current > ending_start) {
          buff = (step_current - ending_start) * (uint32_t)delay_time;
          my_delay_ms(buff);
      }
    }
}

void dwell(float seconds) {
    seconds *= 5;
    uint8_t i = 0;
    for(;i < seconds; i++)
        _delay_ms(200);
}
