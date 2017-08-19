from serial import Serial as ser
from time import sleep
from sys import stdout

VERSION = 1.0

try:
    Serial = ser("/dev/ttyACM0", 115200)
except:
    try:
        Serial = ser("/dev/ttyACM1", 115200)
    except:
        try:
            Serial = ser("\\.\COM3", 115200)
        except:
            print("ERROR :: Serial port not connected")
            exit()
#           try:
#                Serial = ser("\\.\COM2", 115200)
#            except:
                
Serial.readline()
Serial.write("Next");
intro = '''G-Code Interpreter V_1.0
Type "help" for more information.'''
ready_symbol = ">>$ "

print (intro)


# While connected..
while (True):

    # Wait for a microcontroller to be ready for next command.
    while(Serial.readline() != "ready\r\n"):
        pass

    # Get the next command, remove any leading spaces, then split by spaces.
    command = raw_input(ready_symbol)
    command = command.strip()
    command = command.split(" ")

    if (command[0] == "exit"):
        exit();

    # Prints a list of all possible commands.
    elif (command[0] == "help"):
        print ('''
The following are a list of possible commands and their parameters ::

exit           ::  Exits the program.

## INFORMATION ##
help           ::  List of possible commands
version        ::  Current version of G-Code Interpreter.

## WORKING WITH FILES ##
run_gcode      ::  Runs the gcode file of name provided.
             >>  [filename].gcode

## MOTOR COMMANDS ##
enable_motors  ::  Enable motors, constant power. (Default)
disable_motors ::  Disable motors, no power.
move_to        ::  Moves endpoint of CNC.
             >>  X[position] - X Position(float 0.00-MAX DEPTH)
             >>  Y[position] - Y Position(float 0.00-MAX WIDTH)
             >>  Z[position] - Z Position(float 0.00-MAX HEIGHT)
             >>  F[percent]  - Feedrate  (int 0-100)
rmove_to       ::  Moves endpoint of CNC to XYZ position at full speed.
             >>  X[position] - X Position(float 0.00-MAX DEPTH)
             >>  Y[position] - Y Position(float 0.00-MAX WIDTH)
             >>  Z[position] - Z Position(float 0.00-MAX HEIGHT)
where          ::  Returns X, Y, Z position and current feedrate.
home           ::  Sends the endpoint home.
             >>  F[percent] - Feedrate(int 0-100)
units          ::  Sets the units.
             >>  mm   - Millimeters
             >>  inch - Inches
absolute       ::  Absolute positioning, relative to home position.
relative       ::  Relatvie positioning, relative to current position.
        ''')

    # Prints the current version
    elif (command[0] == "version"):
        print("Version %.1f" % (VERSION))

    # Runs G-Code of given file.
    elif (command[0] == "run_gcode"):
        if (len(command) != 2):
            print ("ERROR :: Missing file name")
            continue
        file_name = command[1]
        try:
            gcode_file = open(file_name,"r")
        except:
            print ("ERROR :: Couldn not open %s." % (file_name))
        for i, _ in enumerate(gcode_file):
            pass
        i+=1
        gcode_file.seek(0, 0)
        try:
            for index, line in enumerate(gcode_file):
                Serial.write(line+'\n')
                done = (float(index)/(i))*100
                stdout.write("File read percentage: %.2f%% \r"% (done))
                stdout.flush()
                if (index < i-1):
                    while(Serial.readline() != "ready\r\n"):
                        pass
            print("File read percentage :: 100.00% <<SUCCESS>>\r")
        except:
            print("File read percentage :: %.2f%% <<FAIL>>\r"% (done))
        print("Closing file         :: %s" % (file_name))
        gcode_file.close()

    # Linear to given X Y Z location at F feedrate.
    elif (command[0] == "move_to"):
        cmd = command[1:]
        Serial.write("G01 " + ' '.join(cmd))

    # Rapid linear move to given X Y Z location.
    elif (command[0] == "rmove_to"):
        cmd = command[1:]
        Serial.write("G00 " + ' '.join(cmd))

    # Prints where the endpoint in located.
    elif (command[0] == "where"):
        Serial.write("M114")
        print (Serial.readline())

    # Sends the endpoint to the home position
    elif (command[0] == "home"):
        feed = command[1:]
        Serial.write("G28 " + ''.join(feed))

    # Sets units to either mm or inches
    elif (command[0] == "set_units"):
        units = command[1]
        units = units.replace(" ","")
        if (units == "mm"):
            Serial.write("G21")
        elif (units == "inch"):
            Serial.write("G00")
        else:
            print("ERROR :: Incorrect input parameter")
            print("ERROR :: Possible inputs are")
            print("      >> mm")
            print("      >> inch")

    # Sets coordinates to absolute (relative to origin)
    elif (command[0] == "absolute"):
        Serial.write("G90")

    # Sets coordinates to absolute (relative to current position)
    elif (command[0] == "relative"):
        Serial.write("G91")

