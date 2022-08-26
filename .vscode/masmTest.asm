.386
.model flat, stdcall
option casemap :none

include C:\masm32\include\windows.inc
include C:\masm32\include\kernel32.inc
include C:\masm32\include\user32.inc

includelib C:\masm32\lib\kernel32.lib
includelib C:\masm32\lib\user32.lib

include C:\masm32\macros\macros.asm

.data
MsgBoxCaption	db "Kangmin Kim", 0
MsgBoxText		db "CS4301 MASM test", 0
MsgBoxCaption2	db "Testing message box ...", 0
MsgBoxText2		db "Succssful. Exiting ...", 0
DebugText		db 20 DUP(00H)

.data?
hInstance	HINSTANCE ?

.code
start:

invoke	MessageBox, hInstance, addr MsgBoxText, addr MsgBoxCaption, MB_SYSTEMMODAL or MB_ICONINFORMATION or MB_YESNO
cmp		eax, IDYES
je		J001
invoke ExitProcess, NULL

J001:

pushad
fn		wsprintf, addr DebugText, cfm$("Value : %-.8IXH(%lu) - Text:%hs"), eax
fn		MessageBoxEx, hInstance, addr DebugText, cfm$("Calling a message in Debugging mode"), MB_OK, NULL
popad

invoke MessageBox, hInstance, addr MsgBoxText2, addr MsgBoxCaption2, MB_SYSTEMMODAL or MB_ICONINFORMATION or MB_OK
invoke ExitProcess, NULL
end start