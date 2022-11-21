; Kangmin Kim, Jeff Caldwell       Mon Nov 21 16:38:49 2022
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                                   
global  _start                          ; program stage1no101

_start:                                         
        call    ReadInt                 ; read int; value placed in eax
        mov     [I2],eax                ; store eax at a
        call    ReadInt                 ; read int; value placed in eax
        mov     [I3],eax                ; store eax at b
        call    ReadInt                 ; read int; value placed in eax
        mov     [I4],eax                ; store eax at c
        mov     eax,[I2]                ; load a in eax 
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[I3]                ; load b in eax 
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[I6]                ; AReg = 34
        sub     eax,[I5]                ; AReg = 34 - 3
        imul    dword [I1]              ; AReg = T0 * five
        mov     eax,[I2]                ; AReg = a
        mov     [T0],eax                ; T0 = AReg
        mov     [T0],eax                ; deassign AReg
        mov     eax,[I2]                ; AReg = a
        sub     eax,[I2]                ; AReg = a - a
