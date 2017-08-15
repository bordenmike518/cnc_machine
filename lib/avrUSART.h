#ifndef AVRUSART_H_
#define AVRUSART_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#define F_CPU           16000000UL
#define BAUD            9600
#define BRC             ((F_CPU/16/BAUD)-1)
#define RX_BUFFER_SIZE  128
#define TX_BUFFER_SIZE  128

uint8_t tORr = 0;
/* Transmitting */ 
char txBuffer[TX_BUFFER_SIZE];
uint8_t txReadPos = 0;
uint8_t txWritePos = 0;

/* RECEIVING    */
char rxBuffer[RX_BUFFER_SIZE];
uint8_t rxReadPos = 0;
uint8_t rxWritePos = 0;

/* Set to transmit or receive, but only if not already set. */
void TorR(uint8_t n);

/* Transmitting */
void appendTX(char c);
void writeCharArray(char  c[]);

/* RECEIVING    */
char readChar(void);
char peekChar(void);


#endif
