@ r1 = pointer into hexstr
@ r4 - register to convert and print 
@ r5 - loop index
@ r6 - current character working register
@ r7 - mask register

.thumb_func
.global main

main:   bl stdio_init_all

printexample:
    @ load r4 with 0x12AB
    mov r4, #0x12
    lsl r4,#8
    mov r7, #0xab
    add r4, r7

    @ set pointer to output location in memory
    ldr r1, =hexstr
    add r1, #9

@ loop for r5 - 8 to 1 step -1
    mov r5, #8
loop4: mov r6, r4
    mov r7, #0x0f   @ digit/nibble mask
    and r6, r7
@ if r6 >= 10 then goto letter
    cmp r6, #10
    bge letter
@ else value is ascii 0-9
    add r6,#'0'
    b cont
letter: @ digit A to F 
    add r6, #('A'-10)
cont: @end if 
    strb r6, [r1]
    sub r1, #1
    lsr r4, #4 @ shift off nibble we'd just processed
@ next r5 
    sub r5, #1
    bne loop4

repeat:
    ldr r0, =printstr 
    ldr r1, =hexstr @ string to print 
    bl printf
    b repeat

.align 4
.data
hexstr: .asciz "0x12345678"
printstr: .asciz "Register = %s\n"
