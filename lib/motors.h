#ifndef MOTORS_H_
#define MOTORS_H_

#define NUM_AXES            (2)
#define STEPS_PER_ROTATION  (200)
#define MICRO_STEPS         (8)
#define MM_PER_ROTATION     (8.035)

/* Setup for all stepper motors */
void motors_init(void);

/* Enables or Disables all of the stepper motors */
void motors_enable(void);
void motors_disable(void);

/* Given stepper motor will take one step */
void motors_step(char motor);

/* Moves stepper motors from cur_pos to new_pos at the feedrate speed */
void motors_move(Coordinate cur_pos, Coordinate new_pos, uint16_t feedrate);

/* Zeros out motors to a 'home' position, Coordinates (0,0,0) */
void home(void);

/* Set & Get :: Position */
void set_position(float  x, float  y);
void get_position(float *x, float *y);

#endif  //  MOTORS_H_
