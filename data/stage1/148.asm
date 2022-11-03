; YOUR NAME(S)       Thu Nov 19 08:15:24 2020
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                           
global  _start                          ; program stage1no148

_start:                                 
        call    ReadInt                 ; read int; value placed in eax
        mov     [I1],eax                ; store eax at a
        call    ReadInt                 ; read int; value placed in eax
        mov     [I2],eax                ; store eax at b
        call    ReadInt                 ; read int; value placed in eax
        mov     [I3],eax                ; store eax at c
        mov     eax,[I1]                ; AReg = a
        cmp     eax,[I0]                ; compare a and zero
        je      .L0                     ; if a = zero then jump to set eax to TRUE
        mov     eax,[FALSE]             ; else set eax to FALSE
        jmp     .L1                     ; unconditionally jump
.L0:                                    
        mov     eax,[TRUE]              ; set eax to TRUE
.L1:                                    
        mov     [B0],eax                ; d = AReg
        mov     eax,[I2]                ; AReg = b
        cmp     eax,[I3]                ; compare b and c
        jl      .L2                     ; if b < c then jump to set eax to TRUE
        mov     eax,[FALSE]             ; else set eax to FALSE
        jmp     .L3                     ; unconditionally jump
.L2:                                    
        mov     eax,[TRUE]              ; set eax to TRUE
.L3:                                    
        mov     [B1],eax                ; e = AReg
        mov     eax,[I1]                ; AReg = a
        cmp     eax,[I2]                ; compare a and b
        jg      .L4                     ; if a > b then jump to set eax to TRUE
        mov     eax,[FALSE]             ; else set eax to FALSE
        jmp     .L5                     ; unconditionally jump
.L4:                                    
        mov     eax,[TRUE]              ; set eax to TRUE
.L5:                                    
        mov     [B2],eax                ; f = AReg
        mov     eax,[I1]                ; load a in eax
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[I2]                ; load b in eax
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[I3]                ; load c in eax
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[B0]                ; load d in eax
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
        mov     eax,[B1]                ; load e in eax
        cmp     eax,0                   ; compare to 0
        je      .L8                     ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L9                     ; unconditionally jump to .L9
.L8:                                    
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L9:                                    
        call    WriteString             ; write string to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[B2]                ; load f in eax
        cmp     eax,0                   ; compare to 0
        je      .L10                    ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L11                    ; unconditionally jump to .L11
.L10:                                   
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L11:                                   
        call    WriteString             ; write string to standard out
        call    Crlf                    ; write \r\n to standard out
        Exit    {0}                     

SECTION .data                           
FALSE   dd      0                       ; false
TRUE    dd      -1                      ; true
I0      dd      0                       ; zero

SECTION .bss                            
I1      resd    1                       ; a
I2      resd    1                       ; b
I3      resd    1                       ; c
B0      resd    1                       ; d
B1      resd    1                       ; e
B2      resd    1                       ; f
