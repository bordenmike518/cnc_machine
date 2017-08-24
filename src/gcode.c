#include "cnc.h"
#include "motors.h"

float get_number(char *gcode_line, char character, float else_val) {
    char *input_ptr = &gcode_line[0];
    uint8_t size_of = sizeof(gcode_line) % 8;
    while(*input_ptr && input_ptr && input_ptr != '\0') {
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
            // Feedrate
            motors_move(get_number(gcode_line, 'X', cur_pos.x),
                        get_number(gcode_line, 'Y', cur_pos.y));
            break;
        case 1: // Linear Move
            // Feedrate
            motors_move(get_number(gcode_line, 'X', cur_pos.x),
                        get_number(gcode_line, 'Y', cur_pos.y));
            break;
        case 2: // Clockwise Arc Move
        case 3: // Counter-Clockwise Arc Move
                break;
        case 4: // Dwell(P:Seconds)
                //dwell(get_number(gcode_line, 'P', 
                //      get_number(gcode_line, 'S', 1000))*1000);
                // Feedrate
                break;
        case 10:
                //cmd = get_number(gcode_line, 'L', -1);
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
                 set_mode_mm();
                 return;
        case 21: // INCHES MODE
                 set_mode_inches();
                 return;
        case 28: // HOMING
                 // Feedrate
                 home();  
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
                 break;
        case 90: // ABSOLUTE POSITIONING
                 set_mode_abs(); 
                 break;
        case 91: // RELATIVE POSITIONING
                 set_mode_rel();
                 break;
        case 92: // OFFSET
                 cur_pos.x = get_number(gcode_line, 'X', cur_pos.x);
                 cur_pos.y = get_number(gcode_line, 'Y', cur_pos.y);
                 //cur_pos.z = get_number(gcode_line, 'Z', cur_pos.z);
                 // Feedrate
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

