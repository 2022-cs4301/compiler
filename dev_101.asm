; Kangmin Kim, Jeff Caldwell       Sun Nov 13 08:44:47 2022
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
