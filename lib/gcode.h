#ifndef GCODE_H_
#define GCODE_H_

float get_number(char *gcode_line, char character, float else_val);

void gcode_interpret(char *gcode_line);

#endif  // GCODE_H_
