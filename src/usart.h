#ifndef USART_H_
#define USART_H_

#define F_CPU           16000000UL
#define BAUD            9600
#define BRC             ((F_CPU/16/BAUD)-1)
#define RX_BUFFER_SIZE  128
#define TX_BUFFER_SIZE  128

/* Serial Initialization */
void usart_init(void);

/* Transmitting */
void tx_append(char c);
void usart_write(char  c[]);

/* RECEIVING    */
char usart_read(void);
char usart_peak(void);
uint8_t usart_available(void);

/* Ready for next command */
void usart_ready(void);

#endif
