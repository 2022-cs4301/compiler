; YOUR NAME(S)       Thu Nov 19 08:15:22 2020
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                           
global  _start                          ; program stage1no115

_start:                                 
        mov     eax,[B0]                ; AReg = yes
        and     eax,[B1]                ; AReg = yes and no
        mov     [B7],eax                ; d = AReg
        mov     eax,[B0]                ; AReg = yes
        mov     [B2],eax                ; a = AReg
        mov     eax,[B0]                ; AReg = yes
        or      eax,[B1]                ; AReg = yes or no
        mov     [B3],eax                ; b = AReg
        not     eax                     ; AReg = !AReg
        mov     [B4],eax                ; c = AReg
        mov     eax,[B7]                ; load d in eax
        cmp     eax,0                   ; compare to 0
        je      .L0                     ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L1                     ; unconditionally jump to .L1
.L0:                                    
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L1:                                    
        call    WriteString             ; write string to standard out

SECTION .data                           
TRUELIT db      'TRUE',0                ; literal string TRUE
FALSLIT db      'FALSE',0               ; literal string FALSE

SECTION .text                           
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[B2]                ; load a in eax
        cmp     eax,0                   ; compare to 0
        je      .L2                     ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L3                     ; unconditionally jump to .L3
.L2:                                    
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L3:                                    
        call    WriteString             ; write string to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[B3]                ; load b in eax
        cmp     eax,0                   ; compare to 0
        je      .L4                     ; jump if equal to print FALSE
        mov     edx,TRUELIT             ; load address of TRUE literal in edx
        jmp     .L5                     ; unconditionally jump to .L5
.L4:                                    
        mov     edx,FALSLIT             ; load address of FALSE literal in edx
.L5:                                    
        call    WriteString             ; write string to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[B4]                ; load c in eax
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
B1      dd      0                       ; no
B0      dd      -1                      ; yes

SECTION .bss                            
B2      resd    1                       ; a
B3      resd    1                       ; b
B4      resd    1                       ; c
B7      resd    1                       ; d
B8      resd    1                       ; e
B5      resd    1                       ; w
B6      resd    1                       ; z
