#include "cnc.h"

float get_number(char *gcode_line, char character, float else_val) {
    char *input_ptr = &gcode_line[0];
    while(*input_ptr && input_ptr && input_ptr < input_arr + input_index-1) {
        if (*input_ptr == character) {
            return atof(input_ptr+1);
        }
        input_ptr = strchr(input_ptr, ' ')+1;
    }
    return else_val;
}

void gcode_interpret(char *gcode_line) {
    uint8_t cmd = get_number(gcode_line, 'G', -1);
    switch(cmd) {
        case 0: // Rapid Move
            move_axes(get_number(gcode_line, 'X', cur_pos.x), \
                      get_number(gcode_line, 'Y', cur_pos.y), \
                      100);
            break;
        case 1: // Linear Move
            move_axes(get_number(gcode_line, 'X', cur_pos.x), \
                      get_number(gcode_line, 'Y', cur_pos.y), \
                      get_number(gcode_line, 'F', speed_percent));
            break;
        case 2: // Clockwise Arc Move
        case 3: // Counter-Clockwise Arc Move
                break;
        case 4: // Dwell(P:Seconds)
                dwell(get_number('P', get_number(gcode_line, 'S', 1000))*1000);
                speed_percent = get_number(gcode_line, 'F', speed_percent);
                break;
        case 10:
                cmd = get_number(gcode_line, 'L', -1);
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
                 new_pos.x = get_number(gcode_line, 'X', cur_pos.x);
                 new_pos.y = get_number(gcode_line, 'Y', cur_pos.y);
                 //new_pos.z = get_number(gcode_line, 'Z', cur_pos.z);
                 //retract_pos = get_number(gcode_line, 'R', 5);
                 dwell(get_number(gcode_line, 'P', 5000));
                 speed_percent = get_number(gcode_line, 'F', speed_percent);
                 //num_repeats = get_number(gcode_line, 'L', 1);
                 move_axes();
                 cur_pos.x=new_pos.x; cur_pos.y=new_pos.y; //cur_pos.z=new_pos.z;
                 break;
        case 90: // ABSOLUTE POSITIONING
                 mode_abs = 1; 
                 break;
        case 91: // mode_abs = 0; break;
        case 92: // OFFSET
                 cur_pos.x = get_number(gcode_line, 'X', cur_pos.x);
                 cur_pos.y = get_number(gcode_line, 'Y', cur_pos.y);
                 //cur_pos.z = get_number(gcode_line, 'Z', cur_pos.z);
                 speed_percent = get_number(gcode_line, 'F', speed_percent);
                 break;
        default: // DEFAULT
                 break;
    }
    
    cmd = get_number(gcode_line, 'M', -1);
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

