/*
 * nrf_adaptor_mk1.c
 *
 * Created: 10/22/2015 9:12:31 PM
 *  Author: michael
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "spi_driver.h"
#include "nrf24l01_driver.h"

#define F_CPU 16000000UL
#define UART_RATE 51

// Function declarations
void usart_init(void);
void USART_SendByte(uint8_t u8Data);

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
	nrf_init();
	sei();
	
	uint8_t i = 0;
	uint8_t got_data = 0;
	uint8_t data_packet[32];
	
	while(1)
	{
		// keep in echo
		if (send_test == 1) {
			/*for (i = 0; i < pos_q; i++) {
				USART_SendByte(input_queue[i]);
			}
			pos_q = 0;*/
		
			// send SS low
			// try and send a packet
			nrf_tx_packet(test_data);
			_delay_ms(10);
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
		
		_delay_ms(1000);
		
		
	}
}

/*
 Use nrf send and receive:
 nrf_tx_packet
 nrf_rx_mode
 
 nrf_rx_packet
 nrf_rx_mode
 */

/*
 * Initialises USART
 */
void usart_init(void) {
	// set the baud rate
	//UBRR0H = (unsigned char)UART_RATE;
	//UBRR0L = (unsigned char)(UART_RATE >> 8);
	
	UBRR0 = UART_RATE;
	
	// enable tx and rx
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
}

/*
 * Send a byte over USART
 */
void USART_SendByte(uint8_t u8Data){

	
	// Wait until last byte has been transmitted
	while((UCSR0A &(1<<UDRE0)) == 0);
	
	UDR0 = u8Data;
}

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