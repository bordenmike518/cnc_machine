#include "cnc.h"
 
char tx_buffer[TX_BUFFER_SIZE];
uint8_t tx_read_pos = 0;
uint8_t tx_write_pos = 0;

char rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_read_pos = 0;
uint8_t rx_write_pos = 0;

void usart_init(void) {
	UBRR0H = (BRC >> 8);
	UBRR0L =  BRC;
    UCSR0B = (1 << RXCIE0) | (1 << TXCIE0) | (1 << UDRIE0) | (1 << RXEN0)  | (1 << TXEN0);
	UCSR0C = (1 << UPM01)  | (1 << UPM00)  | (1 << USBS0)  | (1 << UCSZ01) | (1 << UCSZ00);
	sei();
}

void tx_append(char c) {
    tx_buffer[tx_write_pos] = c;
    tx_write_pos++;
    if(tx_write_pos >= TX_BUFFER_SIZE)
        tx_write_pos = 0;
}
 
void usart_write(char c[]) {
    for(uint8_t i = 0; i < (sizeof(c)%sizeof(char)); i++)
        tx_append(c[i]);
    if(UCSR0A & (1 << UDRE0))
        UDR0 = 0;
    _delay_ms(250);
}

char usart_read(void) {
    char ret = '\0';
    if (rx_read_pos != rx_read_pos)
        ret = rx_buffer[rx_read_pos];
    return ret;
}

char usart_peek(void) {
    char ret = '\0';
    if (rx_read_pos != rx_write_pos) {
        ret = rx_buffer[rx_read_pos];
        rx_read_pos++;
        if (rx_read_pos >= RX_BUFFER_SIZE)
            rx_read_pos = 0;
    }
    return ret;
}

uint8_t usart_available(void) {
    return (sizeof(rx_buffer) % sizeof(char));
}

void usart_ready() {
    usart_write("Ready\n");
}

ISR(USART_RX_vect) {
    loop_until_bit_is_set(UCSR0A, RXC0);
    rx_buffer[rx_write_pos] = UDR0;
    rx_write_pos++;
    if (rx_write_pos >= RX_BUFFER_SIZE)
        rx_write_pos = 0;
}

ISR(USART_TX_vect) {    
    loop_until_bit_is_set(UCSR0A, UDRE0);
    if(tx_read_pos != tx_write_pos)
    {
        UDR0 = tx_buffer[tx_read_pos];
        tx_read_pos++;
        if(tx_read_pos >= TX_BUFFER_SIZE)
            tx_read_pos = 0;
    }
}
