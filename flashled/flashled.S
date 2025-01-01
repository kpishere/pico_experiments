@ flash 3 leds example

.equ led_pin1, 18
.equ led_pin2, 19
.equ led_pin3, 20
.equ gpio_out, 1
.equ sleep_time, 200

.thumb_func
.global main

main:  
    mov r0,#led_pin1
    bl gpio_init
    mov r0,#led_pin1
    mov r1,#gpio_out
    bl link_gpio_set_dir
    mov r0, #led_pin2
    bl gpio_init 
    mov r0,#led_pin2
    mov r1,#gpio_out
    bl link_gpio_set_dir
    mov r0, #led_pin3
    bl gpio_init 
    mov r0,#led_pin3
    mov r1,#gpio_out
    bl link_gpio_set_dir
loop: 
    mov r0, #led_pin1
    mov r1, #1
    bl link_gpio_put
    ldr r0, =sleep_time
    bl sleep_ms

    mov r0, #led_pin1
    mov r1, #0
    bl link_gpio_put
    mov r0, #led_pin2
    mov r1, #1
    bl link_gpio_put
    ldr r0, =sleep_time
    bl sleep_ms

    mov r0, #led_pin2
    mov r1, #0
    bl link_gpio_put
    mov r0, #led_pin3
    mov r1, #1
    bl link_gpio_put
    ldr r0, =sleep_time
    bl sleep_ms
    
    mov r0, #led_pin3
    mov r1, #0
    bl link_gpio_put
    b loop 
