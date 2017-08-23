#ifndef MOTORS_H_
#define MOTORS_H_

#define NUM_AXES                  2
#define STEPS_PER_ROTATION        200
#define MICRO_STEPS               8
#define MM_PER_ROTATION           8.035
#define MICRO_STEPS_PER_ROTATION  STEPS_PER_ROTATION * MICRO_STEPS
#define STEPS_PER_MM              MICRO_STEPS_PER_ROTATION / MM_PER_ROTATION
#define MM_PER_STEP               MM_PER_ROTATION / MICRO_STEPS_PER_ROTATION

/* Setup for all stepper motors */
void motors_init(void);

/* Enables or Disables all of the stepper motors */
void motors_enable(void);
void motors_disable(void);

/* Given stepper motor will take one step */
void motors_step(char motor);

/* Moves stepper motors from cur_pos to new_pos at the feedrate speed */
void motors_move(void);

/* Zeros out motors to a 'home' position, Coordinates (0,0,0) */
void home(void);

/* Set & Get :: Position */
void set_position(float  x, float  y);
void get_position(float *x, float *y);

/* Set & Get :: mm or inches */
uint8_t get_mm_or_inches(void);
void set_mode_mm(void);
void set_mode_inches(void);

/* Set & Get :: Absolute or Relative Positioning */
uint8_t get_abs_or_rel(void);
void set_mode_abs(void);
void set_mode_rel(void);

/* Timing Function, returns last sign (-1 || 1) */
void my_delay_ms(uint32_t milliseconds);
void timing(uint32_t step_total, uint32_t step_current, uint8_t *last_sign);
void dwell(float seconds);

#endif  //  MOTORS_H_
