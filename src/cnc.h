#ifndef CNC_H_
#define CNC_H_

//#define DEBUG_ON
#ifdef DEBUG_ON
#   define DEBUG(fmt)       Serial.print(fmt)
#   define DEBUGLN(fmt)     Serial.println(fmt)
#   define DELAY_MAX        (100)
#   define DELAY_MIN        (0)
#else
#   define DEBUG(fmt)
#   define DEBUGLN(fmt)
#   define DELAY_MAX        (500)
#   define DELAY_MIN        (1)
#endif

#define BV(bit)                  (1 << (bit))
#define GETB(byte, bit)          (_SFR_BYTE(byte) &   BV(bit))
#define SETB(byte, bit)          (_SFR_BYTE(byte) |=  BV(bit))
#define CLEARB(byte, bit)        (_SFR_BYTE(byte) &= ~BV(bit))
#define TOGGLEB(byte, bit)       (_SFR_BYTE(byte) ^=  BV(bit))
#define BUFF_MAX                 (128)

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    uint8_t step_pin;
    uint8_t dir_pin;
    uint8_t enable_pin;
    uint8_t limit_switch_pin;
}Motor;

typedef struct {
    float x;
    float y;
    float z;
} Coordinate;

/* Global Position */
Coordinate cur_pos;
Coordinate new_pos;

/* Defined for the CNC Machine */
#include "limits.h"
#include "usart.h"
#include "motors.h"
#include "gcode.h"

#endif  // CNC_H_
