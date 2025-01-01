@ flash2 3 leds example

#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/pads_bank0.h"

.equ led_pin1, 18
.equ led_pin2, 19
.equ led_pin3, 20
.equ sleep_time, 200

.thumb_func
.global main

.macro toggleled led, time 
    mov r0, \led 
    bl gpio_on 
    ldr r0, =\time
    bl sleep_ms
    mov r0, \led
    bl gpio_off
.endm

main:  
    mov r0, #led_pin1
    bl gpioinit
    mov r0, #led_pin2
    bl gpioinit
    mov r0, #led_pin3
    bl gpioinit

loop: 
    toggleled #led_pin1, sleep_time
    toggleled #led_pin2, sleep_time
    toggleled #led_pin3, sleep_time
    b loop

@ Initialize the GPIO to SIO. ro = pin to init 
gpioinit: 
    mov r3, #1
    lsl r3, r0  @ shift r0 to pin number
    ldr r2, gpiobase
    str r3, [r2, #SIO_GPIO_OE_SET_OFFSET]
    str r3, [r2, #SIO_GPIO_OUT_CLR_OFFSET]

@ Enable input and output for the pin
    ldr r2, padsbank0 
    lsl r3, r0, #2  @ pin * 4 for reg addr 
    add r2, r3      @ adjust pointer to pin reg 

    mov r1, #PADS_BANK0_GPIO0_IE_BITS
    ldr r4, setoffset
    orr r2, r4
    str r1, [r2, #PADS_BANK0_GPIO0_OFFSET]

@ Set the function number to SIO 
    lsl r0, #3  @ each GPIO has 8 bytes of registers, multiply by 8
    ldr r2, iobank0     
    add r2, r0  @ set pointer with offset 
    mov r1, #IO_BANK0_GPIO3_CTRL_FUNCSEL_VALUE_SIO_3
    str r1, [r2, #IO_BANK0_GPIO0_CTRL_OFFSET]
    bx lr 

@ Turn on a gpio pin 
gpio_on:
    mov r3, #1
    lsl r3, r0
    ldr r2, gpiobase 
    str r3, [r2, #SIO_GPIO_OUT_SET_OFFSET]
    bx lr 

@ Turn off gpio pin 
gpio_off:
    mov r3, #1
    lsl r3, r0
    ldr r2, gpiobase 
    str r3, [r2, #SIO_GPIO_OUT_CLR_OFFSET]
    bx lr 

.align 4
gpiobase:   .word SIO_BASE
iobank0:    .word IO_BANK0_BASE
padsbank0:  .word PADS_BANK0_BASE
setoffset:  .word REG_ALIAS_SET_BITS



