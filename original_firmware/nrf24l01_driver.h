/************************************************************************/
/* File: nrf24l01_driver.h
/* Author: Michael Smith
/* Date of last edit: 23/10/2015
/* History:
/* -- 23.10.15 -> started
/************************************************************************/

#include <avr/io.h>
#include <avr/delay.h>
#include "spi_driver.h"

#define CE_PIN 1

void nrf_init(void);
void nrf_rx_mode(void);
void nrf_tx_packet(uint8_t* packet);
uint8_t nrf_rx_packet(uint8_t* packet);
void nrf_set_rx_address(uint8_t* address);
void nrf_set_tx_address(uint8_t* address);
void nrf_set_chan(uint8_t chan);

static void nrf_delay(uint16_t delay);