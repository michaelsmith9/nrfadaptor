/*
 * nrf_adaptor_mk1.c
 *
 * Created: 10/22/2015 9:12:31 PM
 *  Author: michael
 */ 
/* ODD THINGS
	- 1us == 18.56us in real life...
 */
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "spi_driver.h"
#include "nrf24l01_driver.h"
#include "uart_driver.h"

//#define UART_RATE 51

// Function declarations
//void usart_init(void);
//void USART_SendByte(uint8_t u8Data);

// variable declarations
// input queue of 128 bytes
volatile uint8_t input_queue[128]; 
// pos in queue
volatile uint8_t pos_q = 0;
// echo queue and reset
volatile uint8_t echo_q = 0;

volatile uint8_t rec_test = 0;
volatile uint8_t send_test = 1; // # change for testing


// test data
uint8_t test_data[32] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0E, 0x0F, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};


int main(void)
{
	DDRC = 0xFF;
	usart_init();
	
	SPI_init();
	
	sei();
	
	uint8_t i = 0;

	PORTC |= ( 1 << 4);
	_delay_ms(20);
	PORTC &= ~(1<< 4);

	/*PORTB &= ~(1 << SS_PIN);

	for (i = 0; i < 32; i++) {
		SPI_tx_byte(test_data[i]);
	}

	PORTB |= (1 << SS_PIN);
	*/
	
	nrf_init();
	
	//uint8_t i = 0;
	uint8_t got_data = 0;
	uint8_t data_packet[32];

	DDRD |= (1 << 2) | (1 << 3);

	/*while(1) {
		//USART_SendString("test", 4);
		USART_SendByte(0x4E);
	}*/

	while(1)
	{
		// keep in echo
		if (send_test == 1) {
			/*for (i = 0; i < pos_q; i++) {
				USART_SendByte(input_queue[i]);
			}
			pos_q = 0;*/
		
			PORTD |= (1 << 2);
			// send SS low
			// try and send a packet
			nrf_tx_packet(test_data);
			PORTD &= ~(1 << 2);
			_delay_ms(30);
			nrf_rx_mode();
			// # change for testing
			//send_test = 0;
		} else if (rec_test == 1) {
			nrf_rx_mode();
			_delay_ms(20);
			got_data = nrf_rx_packet(data_packet);
			
			if (got_data == 1) {
				for (i = 0; i < 32; i++) {
					USART_SendByte(data_packet[i]);
				}
			}
			
			rec_test = 0;
		}
		
		//_delay_us(1600);
		_delay_ms(300);
		
	}
}

/*
 Use nrf send and receive:
 nrf_tx_packet
 nrf_rx_mode
 
 nrf_rx_packet
 nrf_rx_mode
 */

ISR(USART_RX_vect) {
    // echo the char
    char c = UDR0;
    
    // load into queue
    //input_queue[pos_q++] = (uint8_t)c;
    
    if (c == 'r') {
        rec_test = 1;
    } else if (c == 's') {
        send_test = 1;
    }   

}
