#include "cnc.h"

void usart_init(void) {
	UBRR0H = (BRC >> 8);
	UBRR0L = BRC;
    UCSR0B = (1 << TXEN0)  | (1 << TXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	sei();
}

void appendTX(char c) {
    txBuffer[txWritePos] = c;
    txWritePos++;
    if(txWritePos >= TX_BUFFER_SIZE)
        txWritePos = 0;
}
 
void usart_write(char c[]) {
    for(uint8_t i = 0; i < strlen(c); i++)
        appendTX(c[i]);
    if(UCSR0A & (1 << UDRE0))
        UDR0 = 0;
    _delay_ms(250);
}

char usart_read(void) {
    char ret = '\0';
    if (rxReadPos != rxWritePos)
        ret = rxBuffer[rxReadPos];
    return ret;
}

char usart_peek(void) {
    char ret = '\0';
    if (rxReadPos != rxWritePos) {
        ret = rxBuffer[rxReadPos];
        rxReadPos++;
        if (rxReadPos >= RX_BUFFER_SIZE)
            rxReadPos = 0;
    }
    return ret;
}

uint8_t usart_available(void) {
    return (sizeof(rxBuffer) % 8);
}

void usart_ready() {
    usart_write("Ready\n");
}

ISR(USART_RX_vect) {
    loop_until_bit_is_set(UCSR0A, RXC0);
    rxBuffer[rxWritePos] = UDR0;
    rxWritePos++;
    if (rxWritePos >= RX_BUFFER_SIZE)
        rxWritePos = 0;
}

ISR(USART_TX_vect) {    
    loop_until_bit_is_set(UCSR0A, UDRE0);
    if(txReadPos != txWritePos)
    {
        UDR0 = txBuffer[txReadPos];
        txReadPos++;
        if(txReadPos >= TX_BUFFER_SIZE)
            txReadPos = 0;
    }
}
