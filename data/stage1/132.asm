; YOUR NAME(S)       Thu Nov 19 08:15:23 2020
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                           
global  _start                          ; program stage1no132

_start:                                 
        call    ReadInt                 ; read int; value placed in eax
        mov     [I0],eax                ; store eax at a
        call    ReadInt                 ; read int; value placed in eax
        mov     [I1],eax                ; store eax at b
        cmp     eax,[I0]                ; compare b and a
        jne     .L0                     ; if a <> b then jump to set eax to TRUE
        mov     eax,[FALSE]             ; else set eax to FALSE
        jmp     .L1                     ; unconditionally jump
.L0:                                    
        mov     eax,[TRUE]              ; set eax to TRUE
.L1:                                    
        mov     [B0],eax                ; c = AReg
        mov     eax,[I0]                ; load a in eax
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[I1]                ; load b in eax
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[B0]                ; load c in eax
        cmp     eax,0                   ; compare to 0
        je      .L2                     ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L3                     ; unconditionally jump to .L3
.L2:                                    
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L3:                                    
        call    WriteString             ; write string to standard out

SECTION .data                           
TRUELIT db      'TRUE',0                ; literal string TRUE
FALSLIT db      'FALSE',0               ; literal string FALSE

SECTION .text                           
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[I0]                ; AReg = a
        cmp     eax,[I1]                ; compare a and b
        je      .L4                     ; if a = b then jump to set eax to TRUE
        mov     eax,[FALSE]             ; else set eax to FALSE
        jmp     .L5                     ; unconditionally jump
.L4:                                    
        mov     eax,[TRUE]              ; set eax to TRUE
.L5:                                    
        mov     [B0],eax                ; c = AReg
        mov     eax,[I0]                ; load a in eax
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[I1]                ; load b in eax
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[B0]                ; load c in eax
        cmp     eax,0                   ; compare to 0
        je      .L6                     ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L7                     ; unconditionally jump to .L7
.L6:                                    
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L7:                                    
        call    WriteString             ; write string to standard out
        call    Crlf                    ; write \r\n to standard out
        Exit    {0}                     

SECTION .data                           
FALSE   dd      0                       ; false
TRUE    dd      -1                      ; true

SECTION .bss                            
I0      resd    1                       ; a
I1      resd    1                       ; b
B0      resd    1                       ; c
