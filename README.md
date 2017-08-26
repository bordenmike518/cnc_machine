# cnc_machine

CNC Machine which will etch PCBs for rapid prototyping.

This is a G-Code interpreter for a cnc machine that I am working on.
It will be used to etch PCBs with a drill for in house prototyping.
There is a python terminal program that is used to connect over serial.

Hardware Used
- Computer System       :: Raspberry Pi 3
- Microcontroller Board :: Protoneer CNC Hat (Can also use with Arduino and Protoneer CNC Shield)
- Stepper Motor Driver  :: DRV8825
- Stepper Motor         :: 3 x Nema 17 (1.7 A 0.59 Nm 84 oz.in (42x42x48mm))
- Power Supply Unit     :: 12v 30a DC Universal Regulated Switching
- Bootloader Device     :: USBasp for CNC Hat, USBtiny for CNC Shield

[[ :: TODO :: ]]
1. Upload to Raspberry Pi 3 and Test
2. Debug. Test. Debug.
3. Move to phase two!~!
