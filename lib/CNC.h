#ifndef CNC_H_
#define CNC_H_

//#define DEBUG_ON
#ifdef DEBUG_ON
#   define DEBUG(fmt)       Serial.print(fmt)
#   define DEBUGLN(fmt)     Serial.println(fmt)
#   define MAX_DELAY        (100)
#   define MIN_DELAY        (0)
#else
#   define DEBUG(fmt)
#   define DEBUGLN(fmt)
#   define MAX_DELAY        (500)
#   define MIN_DELAY        (1)
#endif

#define BV (bit)                 (1 << (bit))
#define GETB   (byte, bit)       ((byte) & (bit))
#define SETB   (byte, bit)       ((byte) |=  BV(bit))
#define CLEARB (byte, bit)       ((byte) &= ~BV(bit))
#define TOGGLEB(byte, bit)       ((byte) ^=  BV(bit))

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

/* Defined for the CNC Machine */
#include "usart.h"
#include "limits.h"
#include "motors.h"
#include "gcode.h"

#endif  // CNC_H_
