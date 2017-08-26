#include "cnc.h"

uint8_t   mode_mm = 1,
          mode_abs = 1; 

void motors_init() {
    SETB(DDRB, PB0);  // Enable / Disable
    SETB(DDRD, PD2);  // X Step Pulse
    SETB(DDRD, PD5);  // X Direction
    SETB(DDRD, PD3);  // Y Step Pulse
    SETB(DDRD, PD6);  // Y Direction
    //SETB(DDRD, PD4);  // Z Step Pulse
    //SETB(DDRD, PD7);  // Z Direction
    
    motors_disable();
}

void motors_enable(void) {
    SETB(PORTB, PB0);
}

void motors_disable(void) {
    CLEARB(PORTB, PB0);
}

void motors_step(char motor) {
    if (motor == 'x' || motor == 'X') {
        SETB(PORTD, PD2);
        _delay_ms(5);
        CLEARB(PORTD, PD2);
    }
    else if (motor == 'y' || motor == 'Y') {
        SETB(PORTD, PD3);
        _delay_ms(5);
        CLEARB(PORTD, PD3);
    }
    else if (motor == 'z' || motor == 'Z') {
        SETB(PORTD, PD4);
        _delay_ms(5);
        CLEARB(PORTD, PD4);
    }
}

void motors_move(float new_pos_x, float new_pos_y) {
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
        dx = (new_pos_x - cur_pos.x) * steps;
        dy = (new_pos_y - cur_pos.y) * steps;
    }
    if (mode_abs == 0) {
        dx = new_pos_x * steps;
        dy = new_pos_y * steps;
    }

    if (dx > 0) {
        CLEARB(PORTD, PD5);
        sdx = 1;
    }
    else {
        SETB(PORTD, PD5);
        sdx = -1;
    }
    if (dy > 0) {
        SETB(PORTD, PD6);
        sdy = 1;
    }
    else {
        CLEARB(PORTD, PD6);
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
        sigmoid_smoothing(x, dx);
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
        sigmoid_smoothing(y, dy);
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
    set_position(0.0, 0.0);
    SETB(PORTD, PD5);
    CLEARB(PORTD, PD6);
    while (~GETB(PORTB, 1)) {
        motors_step('x');
        cur_pos.x -= MM_PER_STEP;
    }
    while (GETB(PORTB, 1)) {
        CLEARB(PORTD, PD5);
        _delay_ms(10);
        motors_step('x');
    }
    while (~GETB(PORTB, PB2)) {
        motors_step('y');
        cur_pos.y -= MM_PER_STEP;
    }
    while (GETB(PORTB, PB2)) {
        SETB(PORTD, PD6);
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

void dwell(float seconds) {
    seconds *= 5;
    uint8_t i = 0;
    for(;i < seconds; i++)
        _delay_ms(200);
}

/* Activation (Sigmoid) Function used to smooth the transition of speed
    
DELAY_MIN---   |  S1  |.-------------------.|  S2  |   ---Speed Max
          start|    .'|                     |'.    |end
               |  ,*  |       center        |  *.  |
DELAY_MAX---__,|-*    |                     |    *-|,__---Speed Min
                                      ending_start 
Step : 0 ----------------------------------------------------Step : step_total

(S1) Sigmoid One is the first sigmoid function      (s_m/(1+e^(-(10x-5))))           
(S2) Sigmoid Two is the second sigmoid fucntion     (s_m/(1+e^( (10x-5)))) 
    where x is the step_current
          s_m is the speed_max  
          e = 2.71828182846 

if (the step_total > sigmoid_size_mm) 
    the center steps will run at Speed Max
else 
    the center will not run. speed max will be shrunk by devisor
*/
void sigmoid_smoothing(uint32_t step_current, uint32_t step_total) {
    uint16_t sigmoid_steps = 5 * STEPS_PER_MM,  // sigmoid_steps = <mm> * STEPS_PER_MM
             delay_time_ms = DELAY_MAX;
    uint32_t ending_start  = step_total - sigmoid_steps;
    double   difference    = DELAY_MAX-DELAY_MIN,
             speed_max     = 1.0;   // 1.0 = 100%
    if (step_total < sigmoid_steps) {
        sigmoid_steps = step_total/2.0;         // Decrease sigmoid step total
        ending_start = sigmoid_steps;           // Set new center
        speed_max = step_current / difference;  // Decrease Speed Max
    }
    if (step_current < sigmoid_steps) {
        delay_time_ms-=(difference)*(speed_max/(1+pow(EXP,(-(10*(step_current/sigmoid_steps)-5)))));
        my_delay_ms(delay_time_ms);
    }
    else if (step_current > ending_start) {
        delay_time_ms+=(difference)*(speed_max/(1+pow(EXP,(10*(step_current-ending_start/sigmoid_steps)-5))));
        my_delay_ms(delay_time_ms);
    }
}



