; YOUR NAME(S)       Thu Nov 19 08:15:22 2020
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                           
global  _start                          ; program stage1no108

_start:                                 
        call    ReadInt                 ; read int; value placed in eax
        mov     [I1],eax                ; store eax at a
