@ flash2 3 leds example

#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/pads_bank0.h"
#include "hardware/regs/m0plus.h"

.equ led_pin1, 18
.equ led_pin2, 19
.equ led_pin3, 20

.equ alarm0_isr_offset, 0x40

.thumb_func
.global main
.align 4

.macro toggleled led, led1, led2 
    mov r0, \led 
    bl gpio_on 
    mov r0, \led1 
    bl gpio_off 
    mov r0, \led2
    bl gpio_off
.endm

main:  
    bl stdio_init_all

    mov r0, #led_pin1
    bl gpioinit
    mov r0, #led_pin2
    bl gpioinit
    mov r0, #led_pin3
    bl gpioinit

    bl set_alarm0_isr   @ set int. handler
    ldr r0, alarmtime   @ load time to sleep
    bl set_alarm0       @ set the first alarm

    mov r7, #0          @ counter
loop: 
    ldr r0, =printstr   @ string to print 
    mov r1, r7          @ counter 
    bl printf 
    mov r0, #1          @ add 1
    add r7, r0          @   to counter 
    b loop

set_alarm0:
    @ sets the next alarm on alarm 0
    @ r0 is the length of the alarm 

    @ enable timer 0 interrupt
    ldr r2, timerbase 
    mov r1, #1          @ for alarm 0 
    str r1, [r2, #TIMER_INTE_OFFSET]

    @ set alarm
    ldr r1, [r2, #TIMER_TIMELR_OFFSET]
    add r1, r0
    str r1, [r2, #TIMER_ALARM0_OFFSET]
    bx lr 

.thumb_func     @ necessary for interrupt handles
@ alarm 0 interrupt handler and state machine 
alarm_isr: 
    push {LR}   @ calls other routines
    @ clear interrupt
    ldr r2, timerbase 
    mov r1, #1  @ for alarm 0
    str r1, [r2, #TIMER_INTR_OFFSET]

    @ re-arm interrupt 
    ldr r0, alarmtime   @ load next alarm time 
    bl set_alarm0       @ set next allarm

    @ disable/enable leds base on state 
    ldr r2, =state  @ load address of state 
    ldr r3, [r2]    @ load value of state 
    mov r0, #1
    add r3, r0      @ increment state 
    str r3, [r2]    @ save state 
step1:
    mov r1, #1      @ case state 1
    cmp r3, r1
    bne step2       @ not 1, check next
    toggleled #led_pin1, #led_pin2, #led_pin3
    bl finish
step2:
    mov r1, #2      @ case state 2
    cmp r3, r1 
    bne step3       @ not 2, check next 
    toggleled #led_pin2, #led_pin1, #led_pin3
    bl finish
step3:
    toggleled #led_pin3, #led_pin1, #led_pin2
    mov r3, #0      @ set state back to 0
    ldr r2, =state  @ load addr. of state 
    str r3, [r2]    @ save state 0
    @ fall-thru to finish here 
finish:
    pop {pc}            @ return from interrupt

set_alarm0_isr:
    @ set irq handler to our routine 
    ldr r2, ppbbase
    ldr r1, vtoroffset
    add r2, r1
    ldr r1, [r2]    @ r1 has contents at [PPB_BASE + M0PLUS_VTOR_OFFSET], the vector
    mov r2, #alarm0_isr_offset  @ slot for alarm 0
    add r2, r1      @ r2 add offset to interrupt vector for alarm0_isr_offset
    ldr r0, =alarm_isr @ get pointer to our interrupt handler  
    str r0, [r2]

    @ enable alarm 0 irq (clear then set)
    mov r0, #1      @ alarm 0 is irq0 
    ldr r2, ppbbase
    ldr r1, clearint
    add r1, r2 
    str r0, [r1]    @ clear the interrupt
    ldr r1, setint
    add r1, r2
    str r0, [r1]    @ set the interrupt 
    bx lr 

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
timerbase:  .word TIMER_BASE
ppbbase:    .word PPB_BASE
vtoroffset: .word M0PLUS_VTOR_OFFSET
clearint:   .word M0PLUS_NVIC_ICPR_OFFSET
setint:     .word M0PLUS_NVIC_ISER_OFFSET
alarmtime:  .word 200000
printstr:   .asciz "Counting %d\n"

.data
state:  .word 0

