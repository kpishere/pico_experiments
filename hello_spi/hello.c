/**
 * SPI XRA1403 Device 
 */

#include <stdio.h>
//#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "board_picksix.h"

#define TEST 3

volatile uint16_t output_data = 0x0000;


int main() {
    spi_xra1403_init();

    int pin = 13;
    int pin_button = 7;

#if TEST == 1
    // Fastest toggleing possible -- 324Khz
    XRA1403_MSG(XRA1403_WRITE, XRA1403_PIN2BANK(pin), XRA1403_OCR, output_data);
    cs_select(SPI0_GPIO_CS10_PIN);

    while (1) {
        // Set pin 
        XRA1403_SET_PIN(output_data,pin,1);
        spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA1403_MSG_SIZE);
        XRA1403_SET_PIN(output_data,pin,0);
        spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA1403_MSG_SIZE);
    }

    cs_deselect(SPI0_GPIO_CS10_PIN);
#endif

#if TEST == 2
    // Independant switch toggleing -- 275Khz
    XRA1403_MSG(XRA1403_WRITE, XRA1403_PIN2BANK(pin), XRA1403_OCR, output_data);

    while (1) {
        // Set pin 
        cs_select(SPI0_GPIO_CS10_PIN);
        XRA1403_SET_PIN(output_data,pin,1);
        spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA1403_MSG_SIZE);
        cs_deselect(SPI0_GPIO_CS10_PIN);

        // Clear pin
        cs_select(SPI0_GPIO_CS10_PIN);
        XRA1403_SET_PIN(output_data,pin,0);
        spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA1403_MSG_SIZE);
        cs_deselect(SPI0_GPIO_CS10_PIN);
    }
#endif

#if TEST == 3
    // Interrupt and response time 
    output_data = 0x0000;

    // Set up interrupt 
    cs_select(SPI0_GPIO_CS10_PIN);

    // Read pin config status
    XRA1403_MSG(XRA1403_READ, XRA1403_PIN2BANK(pin_button), XRA1403_GCR, output_data);
    spi_read16_blocking (spi0, (const uint16_t )output_data, (uint16_t *)&(output_data) ,XRA1403_MSG_SIZE);

    // Set pullup
    output_data = 0x0000;
    XRA1403_SET_PIN(output_data,pin_button,XRA1403_INENABLE);
    XRA1403_MSG(XRA1403_WRITE, XRA1403_PIN2BANK(pin_button), XRA1403_PUR, output_data);
    spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA1403_MSG_SIZE);

    // Read pullup status
    output_data = 0x0000;
    XRA1403_MSG(XRA1403_READ, XRA1403_PIN2BANK(pin_button), XRA1403_PUR, output_data);
    spi_read16_blocking (spi0, (const uint16_t )output_data, (uint16_t *)&(output_data) ,XRA1403_MSG_SIZE);

    // Enable interrupts
    XRA1403_SET_PIN(output_data,pin_button,XRA1403_INENABLE);
    XRA1403_MSG(XRA1403_WRITE, XRA1403_PIN2BANK(pin_button), XRA1403_IER, output_data);
    spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA1403_MSG_SIZE);

    // Read interrupt status
    output_data = 0x0000;
    XRA1403_MSG(XRA1403_READ, XRA1403_PIN2BANK(pin_button), XRA1403_IER, output_data);
    spi_read16_blocking (spi0, (const uint16_t )output_data, (uint16_t *)&(output_data) ,XRA1403_MSG_SIZE);

#if false
    // Clear interrupts
    XRA1403_MSG(XRA1403_READ, XRA1403_PIN2BANK(pin_button), XRA1403_GSR, output_data);
    spi_read16_blocking (spi0, (const uint16_t )output_data, (uint16_t *)&(output_data) ,XRA1403_MSG_SIZE);

    // Disable filter
    output_data = 0x0000;
    XRA1403_MSG(XRA1403_WRITE, XRA1403_PIN2BANK(pin_button), XRA1403_IFR, output_data);
    spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA1403_MSG_SIZE);
#endif

    cs_deselect(SPI0_GPIO_CS10_PIN);

    // Set un-triggered data value
    XRA1403_SET_PIN(output_data,pin,1);
    XRA1403_MSG(XRA1403_WRITE, XRA1403_PIN2BANK(pin), XRA1403_OCR, output_data);
    while (1) {
        cs_select(SPI0_GPIO_CS10_PIN);
#if false
        // Read Interrupt status
        XRA1403_MSG(XRA1403_READ, XRA1403_PIN2BANK(pin_button), XRA1403_ISR, output_data);
        spi_read16_blocking (spi0, (const uint16_t )output_data, (uint16_t *)&(output_data) ,XRA1403_MSG_SIZE);
        // Read Input/Output status
        XRA1403_MSG(XRA1403_READ, XRA1403_PIN2BANK(pin_button), XRA1403_GSR, output_data);
        spi_read16_blocking (spi0, (const uint16_t )output_data, (uint16_t *)&(output_data) ,XRA1403_MSG_SIZE);

#endif
        // Set pin 
        spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA1403_MSG_SIZE);

        cs_deselect(SPI0_GPIO_CS10_PIN);
        sleep_us(200);
    }
#endif
    return 0;
}