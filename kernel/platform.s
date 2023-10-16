%macro pusha64 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popa64 0
	pop rax
	pop rbx
	pop rcx
	pop rdx
	pop rbp
	pop rdi
	pop rsi
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
%endmacro

extern isr_handler

global _isr_handler_asm
global _isr_names_asm

_isr_handler_asm:
	cld
	pusha64
	mov     rdi, rsp
	call    isr_handler
	mov     rsp, rax
	popa64
	add     rsp, 16
	iretq

%macro ISR_NAME 1
dq _ISR%1
%endmacro

%macro ISR_STUB_ERR 1
_ISR%1:
	push %1
	jmp _isr_handler_asm
%endmacro

%macro ISR_STUB_NO_ERR 1
_ISR%1:
	push 0
	push %1
	jmp _isr_handler_asm
%endmacro

ISR_STUB_NO_ERR  0
ISR_STUB_NO_ERR  1
ISR_STUB_NO_ERR  2
ISR_STUB_NO_ERR  3
ISR_STUB_NO_ERR  4
ISR_STUB_NO_ERR  5
ISR_STUB_NO_ERR  6
ISR_STUB_NO_ERR  7
ISR_STUB_ERR     8
ISR_STUB_NO_ERR  9
ISR_STUB_ERR    10
ISR_STUB_ERR    11
ISR_STUB_ERR    12
ISR_STUB_ERR    13
ISR_STUB_ERR    14
ISR_STUB_NO_ERR 15
ISR_STUB_NO_ERR 16
ISR_STUB_ERR    17
ISR_STUB_NO_ERR 18
ISR_STUB_NO_ERR 19
ISR_STUB_NO_ERR 20
ISR_STUB_ERR    21
ISR_STUB_NO_ERR 22
ISR_STUB_NO_ERR 23
ISR_STUB_NO_ERR 24
ISR_STUB_NO_ERR 25
ISR_STUB_NO_ERR 26
ISR_STUB_NO_ERR 27
ISR_STUB_NO_ERR 28
ISR_STUB_NO_ERR 29
ISR_STUB_NO_ERR 30
ISR_STUB_NO_ERR 31

ISR_STUB_NO_ERR 32
ISR_STUB_NO_ERR 33
ISR_STUB_NO_ERR 34
ISR_STUB_NO_ERR 35
ISR_STUB_NO_ERR 36
ISR_STUB_NO_ERR 37
ISR_STUB_NO_ERR 38
ISR_STUB_NO_ERR 39
ISR_STUB_NO_ERR 40
ISR_STUB_NO_ERR 41
ISR_STUB_NO_ERR 42
ISR_STUB_NO_ERR 43
ISR_STUB_NO_ERR 44
ISR_STUB_NO_ERR 45
ISR_STUB_NO_ERR 46
ISR_STUB_NO_ERR 47

_isr_names_asm:
	ISR_NAME  0
	ISR_NAME  1
	ISR_NAME  2
	ISR_NAME  3
	ISR_NAME  4
	ISR_NAME  5
	ISR_NAME  6
	ISR_NAME  7
	ISR_NAME  8
	ISR_NAME  9
	ISR_NAME 10
	ISR_NAME 11
	ISR_NAME 12
	ISR_NAME 13
	ISR_NAME 14
	ISR_NAME 15
	ISR_NAME 16
	ISR_NAME 17
	ISR_NAME 18
	ISR_NAME 19
	ISR_NAME 20
	ISR_NAME 21
	ISR_NAME 22
	ISR_NAME 23
	ISR_NAME 24
	ISR_NAME 25
	ISR_NAME 26
	ISR_NAME 27
	ISR_NAME 28
	ISR_NAME 29
	ISR_NAME 30
	ISR_NAME 31

	ISR_NAME 32
	ISR_NAME 33
	ISR_NAME 34
	ISR_NAME 35
	ISR_NAME 36
	ISR_NAME 37
	ISR_NAME 38
	ISR_NAME 39
	ISR_NAME 40
	ISR_NAME 41
	ISR_NAME 42
	ISR_NAME 43
	ISR_NAME 44
	ISR_NAME 45
	ISR_NAME 46
	ISR_NAME 47

global _load_idt_asm

_load_idt_asm:
	lidt [rdi]
	sti
	ret

global _load_gdt_and_tss_asm

_load_gdt_and_tss_asm:
	lgdt    [rdi]
	mov     ax, 0x40
	ltr     ax
	mov     ax, 0x10
	mov     ds, ax
	mov     es, ax
	mov     fs, ax
	mov     gs, ax
	mov     ss, ax
	pop     rdi
	mov     rax, 0x08
	push    rax
	push    rdi
	retfq
