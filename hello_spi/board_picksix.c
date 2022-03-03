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
    if(gpio == INTCS10_PIN && (events & GPIO_IRQ_LEVEL_LOW) ) {
        uint16_t data_p0 = 0x0000, data_p1 = 0x0000;
        uint16_t int_mask = 0x00;

        // Read both ports for interrupts
        cs_select(GPIO_CS10_PIN);
        XRA_MSG(XRA_READ, 0, XRA_ISR, data_p0);
        spi_read16_blocking (spi0, (const uint16_t )data_p0, (uint16_t *)&(data_p0) ,XRA_MSG_SIZE);
        cs_deselect(GPIO_CS10_PIN);

        cs_select(GPIO_CS10_PIN);
        XRA_MSG(XRA_READ, 1, XRA_ISR, data_p1);
        spi_read16_blocking (spi0, (const uint16_t )data_p0, (uint16_t *)&(data_p1) ,XRA_MSG_SIZE);
        cs_deselect(GPIO_CS10_PIN);

        int_mask = ((data_p1 & 0x00FF) << 8) | (data_p0 & 0x00FF);

        // Clear interrupts
        cs_select(GPIO_CS10_PIN);
        XRA_MSG(XRA_READ, 0, XRA_GSR, data_p0);
        spi_read16_blocking (spi0, (const uint16_t )data_p0, (uint16_t *)&(data_p0) ,XRA_MSG_SIZE);
        cs_deselect(GPIO_CS10_PIN);

        cs_select(GPIO_CS10_PIN);
        XRA_MSG(XRA_READ, 1, XRA_GSR, data_p1);
        spi_read16_blocking (spi0, (const uint16_t )data_p0, (uint16_t *)&(data_p1) ,XRA_MSG_SIZE);
        cs_deselect(GPIO_CS10_PIN);

        // Act on interrupts
        if( int_mask & 0x0100 ) {
            // Set pin
            XRA_SET_PIN(output_data,0,0);
        }
    }
}

void
spi_xra1403_init() {
    // Configure SPI Bus
    spi_init(spi0, MAX_XRA_CLK);
    
    gpio_set_function(SPI0_RX_PIN, GPIO_FUNC_SPI);
    gpio_pull_up(SPI0_RX_PIN);
    gpio_set_function(SPI0_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI0_TX_PIN, GPIO_FUNC_SPI);

    spi_set_format (spi0, XRA_MSG_BITS, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

#ifdef AZSMZ_12864_LCD
    // Using CS12 as input pin for encoder button - SPI2 connector has no valid chip select
    uint cs_pins[] = {GPIO_CS10_PIN, GPIO_CS11_PIN, 0xFF};
    uint sen_pins[] = {GPIO_CS12_PIN, 0xFF};
#else
    uint sen_pins[] = {0xFF};
    uint cs_pins[] = {GPIO_CS10_PIN, GPIO_CS11_PIN, GPIO_CS12_PIN, 0xFF};
#endif
    // Define Chip selects
    for(int i=0; cs_pins[i] != 0xFF; i++) {
        // Chip select is active-low, so we'll initialise it to a driven-high state
        gpio_init(cs_pins[i]);
        gpio_set_dir(cs_pins[i], GPIO_OUT);
        gpio_put(cs_pins[i], 1);
    }
    
    // Configure Other sense pins pins
    for(int i=0; sen_pins[i] != 0xFF; i++) {
        gpio_init(sen_pins[i]);
        gpio_set_dir(sen_pins[i], GPIO_IN);
        gpio_pull_up(sen_pins[i]);
    }

    // Configure XRA SPI GPIO for PickSix
    uint16_t io_config[] = { 0x00BF /*1011 1111*/, 0x0A /*0000 1010*/ , 0xFFFF};

    // Configure PICO SENx pins - Pullup
    uint pins[] = {SPI0_SPI_SEN1_PIN, SPI0_SPI_SEN2_PIN, SPI0_SPI_SEN3_PIN, 0xFF};
    uint sets[] = {0x0000, 0x0000};
    for(int i=0; pins[i] != 0xFF; i++) {
        XRA_SET_PIN(sets[XRA_PIN2BANK(pins[i])],pins[i],XRA_PULEN);
    }    

    // Set Input/Output
    for(int i=0; io_config[i] != 0xFFFF; i++)
    {
        // Set I/O
    cs_select(GPIO_CS10_PIN);
        XRA_MSG(XRA_WRITE, i, XRA_GCR, io_config[i]);
        spi_write16_blocking (spi0, (const uint16_t *)&(io_config[i]),XRA_MSG_SIZE);
        io_config[i] = 0x0000;
    cs_deselect(GPIO_CS10_PIN);

    cs_select(GPIO_CS10_PIN);
        XRA_MSG(XRA_READ, i, XRA_GCR, io_config[i]);
        spi_read16_blocking (spi0, (const uint16_t )io_config[i], (uint16_t *)&(io_config[i]) ,XRA_MSG_SIZE);
    cs_deselect(GPIO_CS10_PIN);

    cs_select(GPIO_CS10_PIN);
        // Set output values low
        io_config[i] = 0x0000;
        XRA_MSG(XRA_WRITE, i, XRA_GSR, io_config[i]);
        spi_write16_blocking (spi0, (const uint16_t *)&(io_config[i]),XRA_MSG_SIZE);
    cs_deselect(GPIO_CS10_PIN);

    cs_select(GPIO_CS10_PIN);
        XRA_MSG(XRA_READ, i, XRA_GSR, io_config[i]);
        spi_read16_blocking (spi0, (const uint16_t )io_config[i], (uint16_t *)&(io_config[i]) ,XRA_MSG_SIZE);
    cs_deselect(GPIO_CS10_PIN);

    cs_select(GPIO_CS10_PIN);
        // Set pullups
        XRA_MSG(XRA_WRITE, i, XRA_PUR, sets[i]);
        spi_write16_blocking (spi0, (const uint16_t *)&(sets[i]),XRA_MSG_SIZE);
        sets[i] = 0x0000;
    cs_deselect(GPIO_CS10_PIN);

    cs_select(GPIO_CS10_PIN);
        XRA_MSG(XRA_READ, i, XRA_PUR, io_config[i]);
        spi_read16_blocking (spi0, (const uint16_t )io_config[i], (uint16_t *)&(io_config[i]) ,XRA_MSG_SIZE);
    cs_deselect(GPIO_CS10_PIN);
    }

    // Configure/register handler for interupt pin 
    gpio_set_function(INTCS10_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(INTCS10_PIN, GPIO_IN);
    gpio_pull_up(INTCS10_PIN);
    gpio_set_irq_enabled_with_callback(INTCS10_PIN, GPIO_IRQ_LEVEL_LOW, true, &gpio_callback_handler);
}

