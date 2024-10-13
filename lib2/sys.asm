section .text
    global _start
    global sys_write
    global sys_exit
    global sys_open
    global sys_close
    global sys_read

    extern __c_entry
    extern write_u64
    extern write_i64

_start:
    ; 1st = arg count, 2nd first item
    ; c: rdi, rsi
    pop rdi
    pop rsi
    
    ; argv, argc 
    call __c_entry

; sys_exit:
;     mov rax, 60
;     syscall

; c: rdi, rsi, rdx, rcx, r8, r9
; c: int, const void*, size_t
; rdi=int, rsi=void*, rdx=size_t

; sys:rax, rdi, rsi, rdx, r10, r8, r9
; rax=fd, buf, count
; sys_write:
;     mov rax, 1
;     syscall
;     ret

; const char* pathname, int flags, mode_t mode
sys_open:
    mov rax, 2
    syscall
    ret

sys_close:
    mov rax, 3
    syscall
    ret

; sys_read:
;     mov rax, 0
;     syscall
;     ret

; calculate the decimal representation of an unsigned number and write it to a buffer
; inputs:
; - rdi: 64-bit un-signed integer value
; - rsi: buffer to write to. 64-bit effective address for beginning of 21-bytes. If null terminated, must be size 22, null terminator is not added for you.
; output: none
; clobbers: 
; - r10
; - r11
write_u64:
     ; save all registers
    push rax
    push r9
    push rdi
    ; push r10
    ; push r11

    ; load address to write to
    mov r9, rsi                                         ; move address to preferred register
    add r9, 20                                          ; go to just after end of string
    
    ; loop setup
    xor r11, r11                                        ; current loop itteration = 0
    mov r10, 10                                         ; use base 10
    
.loop_start:
    ; perform division
    mov rax, rdi                                        ; load current integer value               
    xor rdx, rdx                                        ; clear upper bits of division input
    div r10                                             ; rax, rdx(remaindier) = rdx:rax / r12
    mov rdi, rax                                        ; save current value

    ; write remaindier digit
    add rdx, '0'                                        ; convert to ascii digit
    mov byte [r9], dl                                   ; save lower bit of rdx (the rest should be 0)

    ; update loop data
    inc r11                                             ; update loop counter
    dec r9                                              ; update address to write to
    
    ; repeat until r11 > 21
    cmp r11, 21
    jng .loop_start

;loop_end:
    ; include sign
    mov byte [rsi], ' '                                 ; do not add '+' or '-' if number is unsigned

    ; recover all registers
    ; pop r11
    ; pop r10
    pop rdi
    pop r9
    pop rax
ret


; calculate the decimal representation of a signed number and write it to a buffer
; inputs:
; - rdi: 64-bit signed integer value. Must be between or equal to 9_223_372_036_854_775_807 and -9_223_372_036_854_775_808
; - rsi: buffer to write to. 64-bit effective address for beginning of 21-bytes. If null terminated, must be size 22, null terminator is not added for you.
; clobbers: 
; - r10 
; - r11
write_i64:
    test rdi, rdi                                       ; check the sign of the number
    jns .is_not_negaitve                                ; if signed, go to is_not_negative
    ; fall through to is_negative
; is_negative:
    neg rdi                                             ; convert rdi to unsigned number for algorithm of proc_calcu64 to work
    call write_i64                                      ; run the unsigned algorithm
    mov byte [rsi], '-'                                 ; write negative sign
ret                                                     ; return from proc_calci64

.is_not_negaitve:                                
    call write_u64                                      ; fill the buffer with the number
    mov byte [rsi], '+'                                 ; write positive sign
ret