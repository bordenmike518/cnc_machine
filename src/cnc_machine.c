#include "cnc.h"


int main(void) {

    uint8_t input_index = 0;
    char input_char,
         input_arr[MAX_BUF];

    /* Ininitializations */
    usart_init();
    limits_init();
    motors_init();
    
    /* Set program start parameters */
    set_position(0.0, 0.0);
    set_mode_abs();
    set_mode_mm();
    
    /* Ready for next */
    usart_ready();

    while(1) {
        while(usart_available() > 0) {
            _delay_ms(100);
            input_char = usart_read();
            if (input_index++ < BUFF_MAX-1 && input_char != '\0')
                input_arr[input_index] = input_char;
        }
        input_arr[input_index] = '\0';
        input_index = 0;
        motors_enable();
        gcode_interpret(&input_arr[0]);
        usart_ready();
    }

    return (0);
}
