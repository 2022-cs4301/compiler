; YOUR NAME(S)       Wed Dec  1 10:32:50 2021
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                           
global  _start                          ; program stage2no226

_start:                                 
        mov     eax,[I1]                ; AReg = 0
        mov     [I0],eax                ; a = AReg
        cmp     eax,[I2]                ; compare a and 10
        jl      .L0                     ; if a < 10 then jump to set eax to TRUE
        mov     eax,[FALSE]             ; else set eax to FALSE
        jmp     .L1                     ; unconditionally jump
.L0:                                    
        mov     eax,[TRUE]              ; set eax to TRUE
.L1:                                    
        mov     [B0],eax                ; f = AReg
.L2:                                    ; while
        mov     eax,[B0]                ; AReg = f
        cmp     eax,0                   ; compare eax to 0
        je      .L3                     ; if f is false then jump to end while
        mov     eax,[I0]                ; AReg = a
        add     eax,[I3]                ; AReg = a + 1
        mov     [I0],eax                ; a = AReg
        cmp     eax,[I2]                ; compare a and 10
        jl      .L4                     ; if a < 10 then jump to set eax to TRUE
        mov     eax,[FALSE]             ; else set eax to FALSE
        jmp     .L5                     ; unconditionally jump
.L4:                                    
        mov     eax,[TRUE]              ; set eax to TRUE
.L5:                                    
        mov     [B0],eax                ; f = AReg
        cmp     eax,0                   ; compare to 0
        je      .L6                     ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L7                     ; unconditionally jump to .L7
.L6:                                    
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L7:                                    
        call    WriteString             ; write string to standard out

SECTION .data                           
TRUELIT db      'TRUE',0                ; literal string TRUE
FALSLIT db      'FALSE',0               ; literal string FALSE

SECTION .text                           
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[I0]                ; load a in eax
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        jmp     .L2                     ; end while
.L3:                                    
        mov     eax,[I0]                ; load a in eax
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[B0]                ; load f in eax
        cmp     eax,0                   ; compare to 0
        je      .L8                     ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L9                     ; unconditionally jump to .L9
.L8:                                    
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L9:                                    
        call    WriteString             ; write string to standard out
        call    Crlf                    ; write \r\n to standard out
        Exit    {0}                     

SECTION .data                           
I1      dd      0                       ; 0
I3      dd      1                       ; 1
I2      dd      10                      ; 10
FALSE   dd      0                       ; false
TRUE    dd      -1                      ; true

SECTION .bss                            
I0      resd    1                       ; a
B0      resd    1                       ; f
