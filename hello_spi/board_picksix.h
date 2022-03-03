/*  Board specific configuration parameters for PickSix Board
 *  
 *  Copyright (C) 2022  Kevin Peck <kevindpeck@gmail.com>
 * 
 *  This file may be distributed under the terms of the GNU GPLv3 license.
 */
#ifndef __BOARD_PICKSIX_H
#define __BOARD_PICKSIX_H

#include "pico/stdlib.h"
#include "spi_xra1403.h"
#include "hardware/spi.h"

#if true
#define AZSMZ_12864_LCD 
#endif

// PICO GPIO Pins
#define INTCS10_PIN 0 
#define GPIO_CS10_PIN 5
#define GPIO_CS11_PIN 1 
#define GPIO_CS12_PIN 16

#define SPI0_SCK_PIN 2
#define SPI0_TX_PIN 3
#define SPI0_RX_PIN 4

// XRA1403 GPIO Expander Pins
#define SPI0_SPI_P1_PIN 6
#define SPI0_SPI_SEN1_PIN 7
#define SPI0_SPI_P2_PIN 8
#define SPI0_SPI_SEN2_PIN 9
#define SPI0_SPI_P3_PIN 10
#define SPI0_SPI_SEN3_PIN 11

#define SPI0_SPI_HB1_PIN 12
#define SPI0_SPI_HT1_PIN 13
#define SPI0_SPI_HT2_PIN 14
#define SPI0_SPI_HT3_PIN 15

void cs_select(uint cs);

void cs_deselect(uint cs);

void spi_xra1403_init();

#endif // board_picksix.h
