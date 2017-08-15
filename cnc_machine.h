#ifndef MIKEYB_CNC_V2_H
#define MIKEYB_CNC_V2_H

#include <stdlib.h>
#include <math.h>

//#define DEBUG_ON
#ifdef DEBUG_ON
#   define DEBUG(fmt) \
        Serial.print(fmt)
#   define DEBUGLN(fmt) \
        Serial.println(fmt)
#   define MAX_DELAY           (100)
#   define MIN_DELAY           (0)
#else
#   define DEBUG(fmt)
#   define DEBUGLN(fmt)
#   define MAX_DELAY           (500)
#   define MIN_DELAY           (1)
#endif

#define NUM_AXES            (2)
#define BAUD                (115200)
#define STEPS_PER_ROTATION  (200)
#define MICRO_STEPS         (8)
#define MM_PER_ROTATION     (8.035)
#define MAX_BUF             (64)

typedef struct {
    int step_pin;
    int dir_pin;
    int enable_pin;
    int limit_switch_pin;
} Motor;

typedef struct {
    float x;
    float y;
    float z;
} Coordinate;

Motor motors[NUM_AXES];
Coordinate cur_pos;
Coordinate new_pos;

int   mode_mm = 1,
      mode_abs = 1,
      input_index = 0, 
      micro_sec,
      micro_steps_per_rotation = STEPS_PER_ROTATION * MICRO_STEPS,
      steps_per_mm = micro_steps_per_rotation / MM_PER_ROTATION;
double delay_range = (MAX_DELAY - MIN_DELAY),
      speed_percent,
      mm_per_step = MM_PER_ROTATION / micro_steps_per_rotation;
char  input_char,
      input_arr[MAX_BUF];

//***********//
//  Endstop  //
//***********//
bool endstop();

//********//
//  Time  //
//********//
void throttle();
void smooth(int long step_count, int long step_total);
void dwell(int microseconds);

//**********//
//  Motors  //
//**********//
void motors_setup();
void motors_enable();
void motors_disable();
void one_step(Motor motor);
void move_axes();

//***************//
//  Coordinates  //
//***************//
void origin();

//*******************//
//  Process Command  //
//*******************//
float get_number(char character, float else_val);
void run_command();
void serial_ready();

#endif // MIKEYB_CNC_V2_H
