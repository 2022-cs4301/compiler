; YOUR NAME(S)       Thu Nov 19 08:15:22 2020
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                           
global  _start                          ; program stage1no111

_start:                                 
        mov     eax,[I1]                ; AReg = five
        imul    dword [I2]              ; AReg = five * a
