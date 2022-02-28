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

#define SPI0_GPIO_CS10_PIN 5
#define SPI0_GPIO_CS11_PIN 1 
#define SPI0_GPIO_CS12_PIN 16

#define SPI0_SPI_RX_PIN 4
#define SPI0_SPI_SCK_PIN 2
#define SPI0_SPI_TX_PIN 3
#define SPI0_INTCS10_PIN 0 

void cs_select(uint cs);

void cs_deselect(uint cs);

void spi_xra1403_init();

#endif // board_picksix.h
