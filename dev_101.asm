; Kangmin Kim, Jeff Caldwell       Sat Nov 12 19:46:33 2022
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                                   
global  _start                          ; program stage1no101

_start:                                         
        call    ReadInt                 ; read int; value placed in eax
        mov     [I2],eax                ; store eax at a
