/************************************************************************/
/* File: spi_driver.c        
/* Author: Michael Smith
/* Date of last edit: 22/10/2015
/* History:
/* -- 22.10.15 -> started
/************************************************************************/

#include "spi_driver.h"

/*
 * Initialise SPI
 */
void SPI_init(void) {
	// set MOSI and SCK as outputs
	DDRB |= (1 << MOSI_PIN) | (1 << SCK_PIN) | (1 << SS_PIN);
	// enable SPI, MSTR, SCK = fsck/16 
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	// send SS high
	PORTB |= (1 << SS_PIN);
}

/*
 * Transmit a byte using SPI
 */
uint8_t SPI_tx_byte(uint8_t data) {
	// start tx
	SPDR = data;
	// wait until sent
	while (!(SPSR & (1 << SPIF)));
	// return the SPDR
	return SPDR;
}
