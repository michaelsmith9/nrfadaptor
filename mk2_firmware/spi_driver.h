/************************************************************************/
//* File: spi_driver.h
//* Author: Michael Smith
//* Date of last edit: 22/10/2015
//* History:
//* -- 22.10.15 -> started
/************************************************************************/

#define MISO_PIN 4
#define MOSI_PIN 3
#define SS_PIN 2
#define SCK_PIN 5

#include <avr/io.h>

void SPI_init(void);
uint8_t SPI_tx_byte(uint8_t data);
