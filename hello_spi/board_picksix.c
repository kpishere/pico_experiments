/*  Board specific configuration parameters for PickSix Board
 *  
 *  Copyright (C) 2022  Kevin Peck <kevindpeck@gmail.com>
 * 
 *  This file may be distributed under the terms of the GNU GPLv3 license.
 */
#include "board_picksix.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"

void
cs_select(uint cs) {
    asm volatile("nop \n nop \n nop");
    gpio_put(cs, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

void
cs_deselect(uint cs) {
    asm volatile("nop \n nop \n nop");
    gpio_put(cs, 1);
    asm volatile("nop \n nop \n nop");
}

extern volatile uint16_t output_data;
void gpio_callback_handler(uint gpio, uint32_t events) {
    if(gpio == SPI0_INTCS10_PIN && (events & GPIO_IRQ_LEVEL_LOW) ) {
        uint16_t data_p0 = 0x0000, data_p1 = 0x0000;
        uint16_t int_mask = 0x00;

        // Read both ports for interrupts
        cs_select(SPI0_GPIO_CS10_PIN);
        XRA1403_MSG(XRA1403_READ, 0, XRA1403_ISR, data_p0);
        spi_read16_blocking (spi0, (const uint16_t )data_p0, (uint16_t *)&(data_p0) ,XRA1403_MSG_SIZE);

        XRA1403_MSG(XRA1403_READ, 1, XRA1403_ISR, data_p1);
        spi_read16_blocking (spi0, (const uint16_t )data_p0, (uint16_t *)&(data_p1) ,XRA1403_MSG_SIZE);

        int_mask = ((data_p1 & 0x00FF) << 8) | (data_p0 & 0x00FF);

        // Clear interrupts
        XRA1403_MSG(XRA1403_READ, 0, XRA1403_GSR, data_p0);
        spi_read16_blocking (spi0, (const uint16_t )data_p0, (uint16_t *)&(data_p0) ,XRA1403_MSG_SIZE);

        XRA1403_MSG(XRA1403_READ, 1, XRA1403_GSR, data_p1);
        spi_read16_blocking (spi0, (const uint16_t )data_p0, (uint16_t *)&(data_p1) ,XRA1403_MSG_SIZE);
        cs_deselect(SPI0_GPIO_CS10_PIN);

        // Act on interrupts
        if( int_mask & 0x0100 ) {
            // Set pin
            XRA1403_SET_PIN(output_data,0,0);
        }
    }
}

void
spi_xra1403_init() {
    // Configure SPI Bus
    spi_init(spi0, MAX_XRA1403_CLK);
    
    gpio_set_function(SPI0_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_INTCS10_PIN, GPIO_FUNC_XIP);
    gpio_set_dir(SPI0_INTCS10_PIN, GPIO_IN);
    gpio_pull_up(SPI0_INTCS10_PIN);

    spi_set_format (spi0, XRA1403_MSG_BITS, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    uint cs_pins[] = {SPI0_GPIO_CS10_PIN, SPI0_GPIO_CS11_PIN, SPI0_GPIO_CS12_PIN, 0xFF};
    for(int i=0; cs_pins[i] != 0xFF; i++) {
        // Chip select is active-low, so we'll initialise it to a driven-high state
        gpio_init(cs_pins[i]);
        gpio_set_dir(cs_pins[i], GPIO_OUT);
        gpio_put(cs_pins[i], 1);
    }

    // Configure XRA1403 SPI GPIO for PickSix
    uint16_t io_config[] = { 0x00BF /*1011 1111*/, 0x0A /*0000 1010*/ , 0xFFFF};

    // Set Input/Output
    cs_select(SPI0_GPIO_CS10_PIN);
    for(int i=0; io_config[i] != 0xFFFF; i++)
    {
        XRA1403_MSG(XRA1403_WRITE, i, XRA1403_GCR, io_config[i]);
        spi_write16_blocking (spi0, (const uint16_t *)&(io_config[i]),XRA1403_MSG_SIZE);
    }
    cs_deselect(SPI0_GPIO_CS10_PIN);

    // Configure/register handler    
    gpio_set_irq_enabled_with_callback(SPI0_INTCS10_PIN, GPIO_IRQ_LEVEL_LOW, true, &gpio_callback_handler);
}

