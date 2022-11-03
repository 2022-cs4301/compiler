; YOUR NAME(S)       Thu Nov 19 08:15:22 2020
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                           
global  _start                          ; program stage1no119

_start:                                 
        call    ReadInt                 ; read int; value placed in eax
        mov     [I0],eax                ; store eax at f
        call    ReadInt                 ; read int; value placed in eax
        mov     [I1],eax                ; store eax at g
        call    ReadInt                 ; read int; value placed in eax
        mov     [I2],eax                ; store eax at h
        mov     eax,[I0]                ; AReg = f
        cmp     eax,[I1]                ; compare f and g
        jle     .L0                     ; if f <= g then jump to set eax to TRUE
        mov     eax,[FALSE]             ; else set eax to FALSE
        jmp     .L1                     ; unconditionally jump
.L0:                                    
        mov     eax,[TRUE]              ; set eax to TRUE
.L1:                                    
        mov     [B2],eax                ; c = AReg
        mov     eax,[B0]                ; AReg = a
        cmp     eax,[B1]                ; compare a and b
        je      .L2                     ; if a = b then jump to set eax to TRUE
        mov     eax,[FALSE]             ; else set eax to FALSE
        jmp     .L3                     ; unconditionally jump
.L2:                                    
        mov     eax,[TRUE]              ; set eax to TRUE
.L3:                                    
        mov     [B3],eax                ; d = AReg
        mov     eax,[I0]                ; AReg = f
        cmp     eax,[I2]                ; compare f and h
        je      .L4                     ; if f = h then jump to set eax to TRUE
        mov     eax,[FALSE]             ; else set eax to FALSE
        jmp     .L5                     ; unconditionally jump
.L4:                                    
        mov     eax,[TRUE]              ; set eax to TRUE
.L5:                                    
        mov     [B4],eax                ; e = AReg
        mov     eax,[B2]                ; load c in eax
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
        mov     eax,[B3]                ; load d in eax
        cmp     eax,0                   ; compare to 0
        je      .L8                     ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L9                     ; unconditionally jump to .L9
.L8:                                    
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L9:                                    
        call    WriteString             ; write string to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[B4]                ; load e in eax
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
B0      dd      -1                      ; a
B1      dd      -1                      ; b
FALSE   dd      0                       ; false
TRUE    dd      -1                      ; true

SECTION .bss                            
B2      resd    1                       ; c
B3      resd    1                       ; d
B4      resd    1                       ; e
I0      resd    1                       ; f
I1      resd    1                       ; g
I2      resd    1                       ; h
