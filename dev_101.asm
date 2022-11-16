; Kangmin Kim, Jeff Caldwell       Wed Nov 16 13:06:35 2022
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
