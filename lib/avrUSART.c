#include "avrUSART.h"

int main(void) {
	UBRR0H = (BRC >> 8);
	UBRR0L = BRC;
	
	UCSR0B = (1 << TXEN0)  | (1 << TXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	DDRB = (1 << PORTB0);
	
	sei();
     
    writeCharArray("HELLO\n");
    writeCharArray("woRLd\n");
     
    _delay_ms(1500);

    while (1) {
        char c = readChar();
        if (c == '1') {
            sbi(PORTB, PORTB0);
            writeCharArray("ON\n");
        }
        else if(c == '0') {
            cbi(PORTB, PORTB0);
            writeCharArray("OFF\n");
        }
    }
    return 0;
}

//////////////////////////////////////////////////

void initUSART(void) {
	UBRR0H = (BRC >> 8);
	UBRR0L = BRC;
    UCSR0B = (1 << TXEN0)  | (1 << TXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	sei();
}

void TorR(uint8_t n) {
    if (tORr == 0 && n == 1) {
        UCSR0B = (1 << RXEN0)  | (1 << RXCIE0);
        tORr = 1;
    }
    else if (tORr == 1 && n == 0) {
        UCSR0B = (1 << TXEN0)  | (1 << TXCIE0);
        tORr = 0;
    }
}

/* Transmitting */ 
void appendTX(char c) {
    TorR(0);
    txBuffer[txWritePos] = c;
    txWritePos++;
    if(txWritePos >= TX_BUFFER_SIZE)
        txWritePos = 0;
}
 
void writeCharArray(char c[]) {
    TorR(0);
    for(uint8_t i = 0; i < strlen(c); i++)
        appendTX(c[i]);
    if(UCSR0A & (1 << UDRE0))
        UDR0 = 0;
    _delay_ms(250);
}


/* RECEIVING    */
char peekChar(void) {
	TorR(1);
    char ret = '\0';
    if (rxReadPos != rxWritePos)
        ret = rxBuffer[rxReadPos];
    return ret;
}

char readChar(void) {
	TorR(1);
    char ret = '\0';
    if (rxReadPos != rxWritePos) {
        ret = rxBuffer[rxReadPos];
        rxReadPos++;
        if (rxReadPos >= RX_BUFFER_SIZE)
            rxReadPos = 0;
    }
    return ret;
}

/* RECEIVING    */
ISR(USART_RX_vect) {
	UCSR0B = (1 << RXEN0)  | (1 << RXCIE0);
    rxBuffer[rxWritePos] = UDR0;
    rxWritePos++;
    if (rxWritePos >= RX_BUFFER_SIZE)
        rxWritePos = 0;
}

/* Transmitting */
ISR(USART_TX_vect) {
    UCSR0B = (1 << TXEN0)  | (1 << TXCIE0);
    if(txReadPos != txWritePos)
    {
        UDR0 = txBuffer[txReadPos];
        txReadPos++;
        if(txReadPos >= TX_BUFFER_SIZE)
            txReadPos = 0;
    }
}
