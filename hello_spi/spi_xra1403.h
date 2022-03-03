/*  Interface defintion for SPI XRA GPIO Expansion device
 *  
 *  Copyright (C) 2022  Kevin Peck <kevindpeck@gmail.com>
 * 
 *  This file may be distributed under the terms of the GNU GPLv3 license.
 */
#ifndef __SPI_XRA_H
#define __SPI_XRA_H

/* SPI Conventions for device
 *
 *  CS : Low is selected
 * CLK :
 *      CPOL=0 - Idle at zero, pulse at one
 *      CPHA=0 - Leading edge of clock pulse triggers read/write
 *      MAX_CLK - 26Mhz 
 * 
 * Data packet:
 * - 16 bit word defines all data transfers for send/receive
 * - Communication is half-duplex, only leading edge of clock pulse is read or write
 * - Write operation : first 8 bits are control, second 8 bits are out data, all return values are ignored
 * - Read operation : first 8 bits are control, second 8 bits are read data
 */
 
/* XRA registers */
#define XRA_GSR   0x00 /* GPIO State - Read-Only */
#define XRA_OCR   0x04 /* Output Control - Read/Write */
#define XRA_PIR   0x08 /* Input Polarity Inversion - Read/Write */
#define XRA_GCR   0x0C /* GPIO Configuration - Read/Write */
#define XRA_PUR   0x10 /* Input Internal Pull-up Resistor Enable/Disable - Read/Write */
#define XRA_IER   0x14 /* Input Interrupt Enable - Read/Write */
#define XRA_TSCR  0x18 /* Output Three-State Control - Read/Write */
#define XRA_ISR   0x1C /* Input Interrupt Status - Read-Only */
#define XRA_REIR  0x20 /* Input Rising Edge Interrupt Enable - Read/Write */
#define XRA_FEIR  0x24 /* Input Falling Edge Interrupt Enable - Read/Write */
#define XRA_IFR   0x28 /* Input Filter Enable/Disable - Read/Write */

#define XRA_READ 0x01
#define XRA_WRITE 0x00

#define XRA_OUTPUT 0x00
#define XRA_INPUT 0x01

#define XRA_INDISABLE 0x00
#define XRA_INENABLE 0x01

#define XRA_PULDIS 0x00
#define XRA_PULEN 0x01

/* Return bank number from provided pin number P0-15
 */
#define XRA_PIN2BANK(pin) ((pin) > 7)

/* Update just one pin in data P0-15 */
#define XRA_SET_PIN(data,pin,val)  \
    (data) = ( ((val) & 0x01) ? (data) | (0x01 << ((pin) % 8)) : (data) & ~(0x01 << ((pin) % 8)) )

/* Prepare packet for XRA - 16 bit
 *      rw: 0 - write, 1 - read
 *    bank: 0 - P0-1, 1 - P8-15
 * control: Command/control code - see enum XRA_CMD
 *    data: 8 bit / unsigned char
 */
#define XRA_MSG(rw, bank, control, data) \
    (data) = ((rw==XRA_WRITE ? 0x0000 : 0x8000) \
    | ((( ((control)<<8) & 0x3F00)|(bank>0 ? 0x0200 : 0x0000))) \
    | (data) & 0x00FF)

/* Two byte message length
 */
#define XRA_MSG_SIZE 1
#define XRA_MSG_BITS 16

/* Maximum clock speed XRA
 */
#define MAX_XRA_CLK 12E6 /* 26 Mhz is max - lower speed is more wiring tolerent */

#endif // spi_xra1403.h
