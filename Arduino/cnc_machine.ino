#include "mikeyb_cnc.h"

//***********//
//  Endstop  //
//***********//
bool endstop() {
    for (int i = 9; i < 13; i++) {
        if (digitalRead(i) == HIGH)
            return true;
        else
            return false;
    }
}

//********//
//  Time  //
//********//
void throttle() {
    micro_sec = MAX_DELAY - (delay_range * (speed_percent/100.00));
    delayMicroseconds(micro_sec);
}

void smooth(int long step_count, int long step_total) {
    int beginning_end, ending_start,
        two_mm = 2 * steps_per_mm;
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
    if (speed_percent >= 50 && speed_percent <= 100) {
      if (step_count < beginning_end)
          delayMicroseconds(delay_count - (step_count * delay_time));
      if (step_count > ending_start) 
          delayMicroseconds((step_count - ending_start) * delay_time);
    }
}

void dwell(int microseconds) {
    delay(microseconds/1000);
    delayMicroseconds(microseconds%1000);
}

//**********//
//  Motors  //
//**********//
void motors_setup() {
    int i;
    if (NUM_AXES >= 1) {
        motors[0].step_pin = 2;
        motors[0].dir_pin = 5;
        motors[0].enable_pin=8;
        motors[0].limit_switch_pin=9;
    }
    if (NUM_AXES >= 2) {
        motors[1].step_pin=3;
        motors[1].dir_pin=6;
        motors[1].enable_pin=8;
        motors[1].limit_switch_pin=10;
    }
    if (NUM_AXES >= 3) {
        motors[2].step_pin=4;
        motors[2].dir_pin=7;
        motors[2].enable_pin=8;
        motors[2].limit_switch_pin=11;
    }
    if (NUM_AXES == 4) {
        motors[3].step_pin=12;
        motors[3].dir_pin=13;
        motors[3].enable_pin=8;
        motors[3].limit_switch_pin=11;
    }
    for (i = 0; i < NUM_AXES; ++i) {
        pinMode(motors[i].step_pin, OUTPUT);
        pinMode(motors[i].dir_pin, OUTPUT);
        pinMode(motors[i].enable_pin, OUTPUT);
        // add input pinmode for limit_switch_pin
    }
}

void motors_enable() {
    for (int i = 0; i < NUM_AXES; ++i)
        digitalWrite(motors[i].enable_pin, LOW);
}

void motors_disable() {
    for (int i = 0; i < NUM_AXES; ++i)
        digitalWrite(motors[i].enable_pin, HIGH);
}
void one_step(Motor motor) {
    digitalWrite(motor.step_pin, HIGH);
    delayMicroseconds(5);
    digitalWrite(motor.step_pin, LOW);
    throttle();
}

void move_axes() {
    int sx, sy;
    int long dx,  // Change in x
             dy,  // Change in y
             x = 0, y = 0;
    float m,      // Slope
          steps,  // Motor steps
          missed_steps;
    
    if (mode_mm == 1)
        steps = steps_per_mm;
    else
        steps = steps_per_mm * 25.4;
        
    if (mode_abs == 1) {
        dx = (new_pos.x - cur_pos.x) * steps;
        dy = (new_pos.y - cur_pos.y) * steps;
    }
    if (mode_abs == 0) {
        dx = new_pos.x * steps;
        dy = new_pos.y * steps;
    }

    if (dx > 0) {
        digitalWrite(motors[0].dir_pin, LOW);
        sx = 1;
    }
    else {
        digitalWrite(motors[0].dir_pin, HIGH);
        sx = -1;
    }
    if (dy > 0) {
        digitalWrite(motors[1].dir_pin, HIGH);
        sy = 1;
    }
    else {
        digitalWrite(motors[1].dir_pin, LOW);
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
      for (; x < dx && endstop() == false; x++) {
        smooth(x, dx);
        one_step(motors[0]);
        cur_pos.x += sx * mm_per_step;
        missed_steps = (m * x) - y;
        if (missed_steps > 0) {
          one_step(motors[1]);
          cur_pos.y += sy * mm_per_step;
          y++;
        }
      }
    }
    else {
      for (; y < dy && endstop() == false; y++) {
        smooth(y, dy);
        one_step(motors[1]);
        cur_pos.y += sy * mm_per_step;
        missed_steps = (m * y) - x;
        if (missed_steps > 0) {
          one_step(motors[0]);
          cur_pos.x += sx * mm_per_step;
          x++;
        }
      }
    }
    
}

//***************//
//  Coordinates  //
//***************//
void origin() {
    new_pos.x = 0; new_pos.y = 0;
    digitalWrite(motors[0].dir_pin, HIGH);
    digitalWrite(motors[1].dir_pin, LOW);
    while (digitalRead(9) != HIGH) {
        one_step(motors[0]);
        cur_pos.x -= mm_per_step;
    }
    while (digitalRead(9) == HIGH) {
        digitalWrite(motors[0].dir_pin, LOW);
        delay(10);
        one_step(motors[0]);
    }
    while (digitalRead(10) != HIGH) {
        one_step(motors[1]);
        cur_pos.y -= mm_per_step;
    }
    while (digitalRead(10) == HIGH) {
        digitalWrite(motors[1].dir_pin, HIGH);
        delay(10);
        one_step(motors[1]);
    }
    cur_pos.x = 0; cur_pos.y = 0;
}

//*******************//
//  Process Command  //
//*******************//
float get_number(char character, float else_val) {
    char *input_ptr = &input_arr[0];
    while(*input_ptr && input_ptr && input_ptr < input_arr + input_index-1) {
        if (*input_ptr == character) {
            return atof(input_ptr+1);
        }
        input_ptr = strchr(input_ptr, ' ')+1;
    }
    return else_val;
}

void run_command() {
    int cmd = get_number('G', -1);
    switch(cmd) {
        case 0: // Rapid Move
            new_pos.x = get_number('X', cur_pos.x);
            new_pos.y = get_number('Y', cur_pos.y);
            //cur_pos.z = get_number('Z', cur_pos.z);
            speed_percent = 100;
            move_axes();
            break;
        case 1: // Linear Move
            new_pos.x = get_number('X', cur_pos.x);
            new_pos.y = get_number('Y', cur_pos.y);
            //cur_pos.z = get_number('Z', cur_pos.z);
            speed_percent = get_number('F', speed_percent);
            move_axes();
            break;
        case 2: // Clockwise Arc Move
        case 3: // Counter-Clockwise Arc Move
                break;
        case 4: // Dwell(P:Seconds)
                dwell(get_number('P', get_number('S', 1000))*1000);
                speed_percent = get_number('F', speed_percent);
                break;
        case 10:
                cmd = get_number('L', -1);
                switch(cmd)
                    case 1: /* Set Tool Table (P:Tool Number,
                                               R:Radius of Tool,
                                               I:Front Angle(lathe),
                                               J:Back Angle(lathe),
                                               Q:Orientation(lathe))
                            */
                    case 2:/* Set Coordinate System (P:Coordinate System,
                                                     R:Rotation About Z axis)
                            */
                    case 10: /* Set Tool Table (P:Tool Number,
                                                R:Radius of Tool,
                                                I:Front Angle(lathe),
                                                J:Back Angle(lathe),
                                                Q:Orientation(lathe))
                             */
                    case 11: /* Set Tool Table (P:Tool Number,
                                                R:Radius of Tool,
                                                I:Front Angle(lathe),
                                                J:Back Angle(lathe),
                                                Q:Orientation(lathe))
                             */
                    case 20: /* Set Coordinate System (P:Coordinate System(0-9))
                             */
                    default:
                        break;
        case 17: // PLANE SELECTION :: X Y
                 break;
        //case 17.1: break; // PLANE SELECTION :: U V
        case 18: // PLANE SELECTION :: Z X
                 break;
        //case 18.1: break; // PLANE SELECTION :: W U
        case 19: // PLANE SELECTION :: Y Z
                 break;
        //case 19.1: break; // PLANE SELECTION :: V W
        case 20: // MM MODE
                 mode_mm = 0;
                 return;
        case 21: // INCHES MODE
                 mode_mm = 1;
                 return;
        case 28: // HOMING
                 speed_percent = get_number('F', speed_percent);
                 origin();  
                 break;
        case 29: // DETAILED Z-PROBE BED LEVELING
                 break;
        case 30: // SINGLE Z-PROBE BED LEVELING
                 break;
        case 31: // SET or REPORT CURRENT PROBE STATUS
                 break;
        case 32: // PROBE Z AND CALCULATE Z PLANE
                 break;
        case 82: // DRILL CYCLE
                 new_pos.x = get_number('X', cur_pos.x);
                 new_pos.y = get_number('Y', cur_pos.y);
                 //new_pos.z = get_number('Z', cur_pos.z);
                 //retract_pos = get_number('R', 5);
                 dwell(get_number('P', 5000));
                 speed_percent = get_number('F', speed_percent);
                 //num_repeats = get_number('L', 1);
                 move_axes();
                 cur_pos.x=new_pos.x; cur_pos.y=new_pos.y; //cur_pos.z=new_pos.z;
                 break;
        case 90: // ABSOLUTE POSITIONING
                 mode_abs = 1; 
                 break;
        case 91: // mode_abs = 0; break;
        case 92: // OFFSET
                 cur_pos.x = get_number('X', cur_pos.x);
                 cur_pos.y = get_number('Y', cur_pos.y);
                 //cur_pos.z = get_number('Z', cur_pos.z);
                 speed_percent = get_number('F', speed_percent);
                 break;
        default: // DEFAULT
                 break;
    }
    
    cmd = get_number('M', -1);
    switch(cmd) {
        case 1: break; // SLEEP OR CONDITIONAL STOP
        case 2: break; // PROGRAM END
        case 3: break; // SPINDLE ON, CLOCKWISE
        case 4: break; // SPINDLE ON, COUNTER-CLOCKWISE
        case 5: break; // SPINDLE OFF
        case 6: break; // TOOL CHANGE
        case 7: break; // MIST COOLANT ON
        case 8: break; // FLOOD COOLANT ON
        case 9: break; // COOLANT OFF
        case 10: break; // VACUUM ON
        case 11: break; // VACUUM OFF
        case 17: motors_enable(); break;
        case 18: motors_disable(); break;
        case 72: break; // PLAY A TONE OR SOUND
        case 73: break; // SET BUILD PERCENTAGE
        //case 100: help(); break;
        case 104: break; // SET EXTRUDER TEMPERATURE
        case 105: break; // GET EXTRUDER TEMPERATURE
        case 106: break; // FAN ON
        case 107: break; // FAN OFF
        case 112: break; // EMERGENCY STEP
        //case 114: print_coordinates(); break;
        default: break;
    }
}

void serial_ready() {
    input_index = 0;
    Serial.println("ready");
}

//*********//
//  SETUP  //
//*********//
void setup() {
    Serial.begin(BAUD);
    motors_setup();
    cur_pos.x = 0; cur_pos.y = 0;
    speed_percent = 50;
    serial_ready();
}

//********//
//  LOOP  //
//********//
void loop() {
    motors_disable();
    while(Serial.available() == 0) {};
    while(Serial.available() > 0) {
        delayMicroseconds(100);
        input_char = Serial.read();
        if (input_index < MAX_BUF - 1 && input_char != NULL)
            input_arr[input_index++] = input_char;
    }
    input_arr[input_index] = NULL;
    motors_enable();
    run_command();
    serial_ready();
}
