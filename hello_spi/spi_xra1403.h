/*  Interface defintion for SPI XRA1403 GPIO Expansion device
 *  
 *  Copyright (C) 2022  Kevin Peck <kevindpeck@gmail.com>
 * 
 *  This file may be distributed under the terms of the GNU GPLv3 license.
 */
#ifndef __SPI_XRA1403_H
#define __SPI_XRA1403_H

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
 
#define XRA1403_GSR 0x00u // read-only GPIO State P0-7
#define XRA1403_OCR 0x02u // read-write Output control P0-7 default 0xFF
#define XRA1403_PIR 0x04u // read-write Input polarity inversion P0-7 default 0x00
#define XRA1403_GCR 0x06u // read-write GPIO configuration for P0-7 default 0xFF
#define XRA1403_PUR 0x08u // read-write Input internal pull-up resistor enable/disabele P0-7 default 0x00
#define XRA1403_IER 0x0Au // read-write Input interrupt enable P0-7 default 0x00
#define XRA1403_TSCR 0x0Cu // read-write Output three-state control P0-7 default 0x00
#define XRA1403_ISR 0x0Eu // read-only Input interrupt status P0-7 default 0x00
#define XRA1403_REIR 0x10u // read-write Input rising edge interrupt enable P0-7 default 0x00
#define XRA1403_FEIR 0x12u // read-write Input falling edge interrupt enable P0-7 default 0x00
#define XRA1403_IFR 0x14u // read-write Input filter enable/disable P0-7 default 0xFF

#define XRA1403_WRITE 0x00
#define XRA1403_READ 0x01

#define XRA1403_OUTPUT 0x00
#define XRA1403_INPUT 0x01

#define XRA1403_INDISABLE 0x00
#define XRA1403_INENABLE 0x01

/* Return bank number from provided pin number P0-15
 */
#define XRA1403_PIN2BANK(pin) (int)((pin) / 8)

/* Update just one pin in data P0-15 */
#define XRA1403_SET_PIN(data,pin,val)  \
    (data) = ( ((val) & 0x01) ?  (data) | (0x01 << ((pin) % 8)) : (data) & ~(0x01 << ((pin) % 8)) )

/* Prepare packet for XRA1403 - 16 bit
 *      rw: 0 - write, 1 - read
 *    bank: 0 - P0-1, 1 - P8-15
 * control: Command/control code - see enum XRA1403_CMD
 *    data: 8 bit / unsigned char
 */
#define XRA1403_MSG(rw, bank, control, data) \
    (data) = ((rw==XRA1403_WRITE ? 0x0000 : 0x8000) | (( ((control)<<8) & 0x1E00)|(bank>0 ? 0x0100 : 0x0000)) | (data) & 0x00FF)

/* Two byte message length
 */
#define XRA1403_MSG_SIZE 1
#define XRA1403_MSG_BITS 16

/* Maximum clock speed XRA1403
 */
#define MAX_XRA1403_CLK 2E6 /* 26 Mhz is max - lower speed is more wiring tolerent */

#endif // spi_xra1403.h
