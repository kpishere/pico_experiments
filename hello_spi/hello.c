/**
 * SPI XRA Device 
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
    XRA_MSG(XRA_WRITE, XRA_PIN2BANK(pin), XRA_OCR, output_data);
    cs_select(GPIO_CS10_PIN);

    while (1) {
        // Set pin 
        XRA_SET_PIN(output_data,pin,1);
        spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA_MSG_SIZE);
        XRA_SET_PIN(output_data,pin,0);
        spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA_MSG_SIZE);
    }

    cs_deselect(GPIO_CS10_PIN);
#endif

#if TEST == 2
    // Independant switch toggleing -- 275Khz
    XRA_MSG(XRA_WRITE, XRA_PIN2BANK(pin), XRA_OCR, output_data);

    while (1) {
        // Set pin 
        cs_select(GPIO_CS10_PIN);
        XRA_SET_PIN(output_data,pin,1);
        spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA_MSG_SIZE);
        cs_deselect(GPIO_CS10_PIN);

        // Clear pin
        cs_select(GPIO_CS10_PIN);
        XRA_SET_PIN(output_data,pin,0);
        spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA_MSG_SIZE);
        cs_deselect(GPIO_CS10_PIN);
    }
#endif

#if TEST == 3
    // Interrupt and response time 
    output_data = 0x0000;


#if false
    // Set up interrupt 
    cs_select(GPIO_CS10_PIN);

    // Read pin config status
    XRA_MSG(XRA_READ, XRA_PIN2BANK(pin_button), XRA_GCR, output_data);
    spi_read16_blocking (spi0, (const uint16_t )output_data, (uint16_t *)&(output_data) ,XRA_MSG_SIZE);
    cs_deselect(GPIO_CS10_PIN);

    // Set pullup
    output_data = 0x0000;
    XRA_SET_PIN(output_data,pin_button,XRA_PULEN);
    XRA_MSG(XRA_WRITE, XRA_PIN2BANK(pin_button), XRA_PUR, output_data);
    spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA_MSG_SIZE);

    // Read pullup status
    output_data = 0x0000;
    XRA_MSG(XRA_READ, XRA_PIN2BANK(pin_button), XRA_PUR, output_data);
    spi_read16_blocking (spi0, (const uint16_t )output_data, (uint16_t *)&(output_data) ,XRA_MSG_SIZE);
#endif

    cs_select(GPIO_CS10_PIN);
    // Rising edge
    XRA_SET_PIN(output_data,pin_button,XRA_INENABLE);
    XRA_MSG(XRA_WRITE, XRA_PIN2BANK(pin_button), XRA_REIR, output_data);
    spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA_MSG_SIZE);
    cs_deselect(GPIO_CS10_PIN);

    cs_select(GPIO_CS10_PIN);
    // Enable interrupts
    XRA_SET_PIN(output_data,pin_button,XRA_INENABLE);
    XRA_MSG(XRA_WRITE, XRA_PIN2BANK(pin_button), XRA_IER, output_data);
    spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA_MSG_SIZE);
    cs_deselect(GPIO_CS10_PIN);

    // Clear interrupts
    cs_select(GPIO_CS10_PIN);
    XRA_MSG(XRA_READ, 0, XRA_GSR, output_data);
    spi_read16_blocking (spi0, (const uint16_t )output_data, (uint16_t *)&(output_data) ,XRA_MSG_SIZE);
    cs_deselect(GPIO_CS10_PIN);

    cs_select(GPIO_CS10_PIN);
    XRA_MSG(XRA_READ, 1, XRA_GSR, output_data);
    spi_read16_blocking (spi0, (const uint16_t )output_data, (uint16_t *)&(output_data) ,XRA_MSG_SIZE);
    cs_deselect(GPIO_CS10_PIN);


    cs_select(GPIO_CS10_PIN);
    // Read interrupt status
    output_data = 0x0000;
    XRA_MSG(XRA_READ, XRA_PIN2BANK(pin_button), XRA_IER, output_data);
    spi_read16_blocking (spi0, (const uint16_t )output_data, (uint16_t *)&(output_data) ,XRA_MSG_SIZE);
    cs_deselect(GPIO_CS10_PIN);


#if false
    // Clear interrupts
    XRA_MSG(XRA_READ, XRA_PIN2BANK(pin_button), XRA_GSR, output_data);
    spi_read16_blocking (spi0, (const uint16_t )output_data, (uint16_t *)&(output_data) ,XRA_MSG_SIZE);

    // Disable filter
    output_data = 0x0000;
    XRA_MSG(XRA_WRITE, XRA_PIN2BANK(pin_button), XRA_IFR, output_data);
    spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA_MSG_SIZE);
#endif


    // Set un-triggered data value
    XRA_SET_PIN(output_data,pin,1);
    XRA_MSG(XRA_WRITE, XRA_PIN2BANK(pin), XRA_OCR, output_data);
    while (1) {
        cs_select(GPIO_CS10_PIN);
#if false
        // Read Interrupt status
        XRA_MSG(XRA_READ, XRA_PIN2BANK(pin_button), XRA_ISR, output_data);
        spi_read16_blocking (spi0, (const uint16_t )output_data, (uint16_t *)&(output_data) ,XRA_MSG_SIZE);
        // Read Input/Output status
        XRA_MSG(XRA_READ, XRA_PIN2BANK(pin_button), XRA_GSR, output_data);
        spi_read16_blocking (spi0, (const uint16_t )output_data, (uint16_t *)&(output_data) ,XRA_MSG_SIZE);

#endif
        // Set pin 
        spi_write16_blocking (spi0, (const uint16_t *)&output_data,XRA_MSG_SIZE);

        cs_deselect(GPIO_CS10_PIN);
        sleep_us(200);
    }
#endif
    return 0;
}