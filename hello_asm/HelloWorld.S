.thumb_func
.global main

main:
    mov r7, #0
    bl stdio_init_all
loop:
    ldr r0, =helloworld
    add r7,#1
    mov r1,r7
    bl printf
    b loop

.data
    .align 4
helloworld:     .asciz "Hello World %d\n"