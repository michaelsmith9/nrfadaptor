/************************************************************************/
/* File: nrf24l01_driver.c
/* Author: Michael Smith
/* Date of last edit: 23/10/2015
/* History:
/* -- 23.10.15 -> started
/************************************************************************/

#include "nrf24l01_driver.h"

/*
 * Initialise the CE DDR on the Atmega
 */
void nrf_init(void) {
	// set DDR for CE pin
	DDRB |= (1 << CE_PIN);
	// basic address
	uint8_t set_address[5] = {0x12, 0x34, 0x56, 0x78, 0x90};
	// basic channel
	uint8_t basic_channel = 41;
	
	// set TX address
	nrf_set_tx_address(set_address);
	nrf_delay(0x0F);
	// set RX address
	nrf_set_rx_address(set_address);
	nrf_delay(0x0F);
	
	/* write to EN_AA */
	// SS LOW
	PORTB &= ~(1 << SS_PIN);
	// write 0x21
	SPI_tx_byte(0x21);
	SPI_tx_byte(0x00);
	// SS high
	PORTB |= (1 << SS_PIN);
	// delay
	nrf_delay(0x0F);
	
	/* write to en_rx_addr */
	// SS LOW
	PORTB &= ~(1 << SS_PIN);
	// write 0x21
	SPI_tx_byte(0x22);
	// enable pipe 0
	SPI_tx_byte(0x01);
	// SS high
	PORTB |= (1 << SS_PIN);
	// delay
	nrf_delay(0x0F);
	
	/* write to rx_payload_length */
	// SS LOW
	PORTB &= ~(1 << SS_PIN);
	// write 0x21
	SPI_tx_byte(0x31);
	// 32 byte payload
	SPI_tx_byte(0x20);
	// SS high
	PORTB |= (1 << SS_PIN);
	// delay
	nrf_delay(0x0F);
	
	/* set the channel */
	nrf_set_chan(basic_channel);
	nrf_delay(0x0F);
	
	/* write to RF setup */
	// SS LOW
	PORTB &= ~(1 << SS_PIN);
	// write to RF setup
	SPI_tx_byte(0x26);
	// 0 dBm
	SPI_tx_byte(0x06);
	// SS high
	PORTB |= (1 << SS_PIN);
	// delay
	nrf_delay(0x0F);
	
	/* pwr up in config */
	// SS LOW
	PORTB &= ~(1 << SS_PIN);
	// write to config
	SPI_tx_byte(0x20);
	// power up
	SPI_tx_byte(0x02);
	// SS high
	PORTB |= (1 << SS_PIN);
	// delay
	nrf_delay(0x0F);
	
}

/*
 * Put NRF into RX MODE
 */
void nrf_rx_mode(void) {
	// SEND CS LOW
	PORTB &= ~(1 << SS_PIN);
	
	// WRITE 0X20
	SPI_tx_byte(0x20);
	// WRITE 0X73
	SPI_tx_byte(0x73);
	// SEND CE HIGH
	PORTB |= (1 << CE_PIN);
	
	// SEND CS HIGH
	PORTB |= (1 << SS_PIN);
	
	// wait for some clock cycles
	nrf_delay(0xFF);
	
}

/*
 * TX a packet
 * assume packet is 32 byte array
 */
void nrf_tx_packet(uint8_t* packet) {
	uint8_t i = 0;
	
	// send CE to 0
	PORTB &= ~(1 << CE_PIN);
	
	// put into TX mode
	// CS LOW
	PORTB &= ~(1 << SS_PIN);
	// WRITE 0x20
	SPI_tx_byte(0x20);
	// WRITE 0x72
	SPI_tx_byte(0x72);
	// CS HIGH
	PORTB |= (1 << SS_PIN);
	
	// wait for some clock cycles
	//nrf_delay(0xFF);
	_delay_us(40);
	
	// write to TX
	// CS LOW
	PORTB &= ~(1 << SS_PIN);
	// send 0xA0
	SPI_tx_byte(0xA0);
	// send 32 bytes
	for (i = 0; i< 32; i++) {
		
		SPI_tx_byte(packet[i]);
	}
	
	// wait for some clock cycles
	nrf_delay(0xFF);
	
	// CS HIGH
	PORTB |= (1 << SS_PIN);
	
	_delay_us(40);
	
	// CE pulse
	PORTB |= (1 << CE_PIN);
	
	// delay for approx 255 clock cycles ~15.9us
	//nrf_delay(0xFFFF);
	_delay_us(300);
	
	// send CE LOW
	PORTB &= ~(1 << CE_PIN);
	
	// wait for some clock cycles
	nrf_delay(0xFF);
	
}

/*
 * Checks if radio has received a packet
 * returns 1 if it has (and updates packet) or
 * else returns 0.
 */
uint8_t nrf_rx_packet(uint8_t* packet) {
	uint8_t was_data = 0;
	uint8_t i = 0;
	uint8_t ret_val = 0;
	
	// read the status register
	// send CS LOW
	PORTB &= ~(1 << SS_PIN);
	// send 0x07
	SPI_tx_byte(0x07);
	was_data = SPI_tx_byte(0xFF);
	// send CS high
	PORTB |= (1 << SS_PIN);
	
	// delay for some time
	//nrf_delay(0xFF);
	_delay_us(40);

	// if we received data
	if (!!(was_data << 6)) {
		// RX is data ready
		// send CS LOW
		PORTB &= ~(1 << SS_PIN);
		// send 0x61, read RX_PAYLOAD
		SPI_tx_byte(0x61);
		// then send FF 32 times and read into payload
		for (i = 0; i < 32; i++) {
			packet[i] = SPI_tx_byte(0xFF);
		}
		
		// send CS high
		PORTB |= (1 << SS_PIN);
		
		//nrf_delay(0xFF);
		_delay_us(40);
		
		// SS LOW
		// write 0xE2, then 0x00, then
		// send CE LOW
		PORTB &= ~(1 << SS_PIN);
		SPI_tx_byte(0xE2);
		SPI_tx_byte(0x00);
		PORTB &= ~(1 << CE_PIN);
		PORTB |= (1 << SS_PIN);
		
		//nrf_delay(0xFF);
		_delay_us(40);
		
		// clear the RX flag
		PORTB &= ~(1 << SS_PIN);
		SPI_tx_byte(0x27);
		SPI_tx_byte(0x40);
		PORTB |= (1 << SS_PIN);
		//_delay_us(40);
		
		ret_val = 1;
	}
	
	return ret_val;
}

/*
 * Sets the TX address where address is 5 bytes
 */
void nrf_set_tx_address(uint8_t* address) {
	uint8_t i = 0;
	// send SS LOW
	PORTB &= ~(1 << SS_PIN);
	// write 0x30
	SPI_tx_byte(0x30);
	// write address
	for (i = 0; i < 5; i++) {
		SPI_tx_byte(address[i]);
	}
	// send SS HIGH
	PORTB |= (1 << SS_PIN);
	
}

/*
 * Sets the RX address where address is 5 bytes
 */
void nrf_set_rx_address(uint8_t* address) {
	uint8_t i = 0;
	// send SS LOW
	PORTB &= ~(1 << SS_PIN);
	// write 0x2A
	SPI_tx_byte(0x2A);
	// write address
	for (i = 0; i < 5; i++) {
		SPI_tx_byte(address[i]);
	}
	// send SS HIGH
	PORTB |= (1 << SS_PIN);
}

/*
 * Set the NRF chan
 */
void nrf_set_chan(uint8_t chan) {
	// SS LOW
	PORTB &= ~(1 << SS_PIN);
	// write 0x25
	SPI_tx_byte(0x25);
	// send chan
	SPI_tx_byte(chan);
	// SS high
	PORTB |= (1 << SS_PIN);
}

/*
 * Delay for some clock cycles
 */
void nrf_delay(uint16_t delay) {
	while (delay > 0) {
		delay--;
	}
}
