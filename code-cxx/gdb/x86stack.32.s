	.file	"x86stack.c"
	.intel_syntax noprefix
	.text
	.globl	foobar
	.type	foobar, @function
foobar:
.LFB0:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	sub	esp, 16
	call	__x86.get_pc_thunk.ax
	add	eax, OFFSET FLAT:_GLOBAL_OFFSET_TABLE_
	mov	eax, DWORD PTR 8[ebp]
	add	eax, 2
	mov	DWORD PTR -4[ebp], eax
	mov	eax, DWORD PTR 12[ebp]
	add	eax, 3
	mov	DWORD PTR -8[ebp], eax
	mov	eax, DWORD PTR 16[ebp]
	add	eax, 4
	mov	DWORD PTR -12[ebp], eax
	mov	edx, DWORD PTR -4[ebp]
	mov	eax, DWORD PTR -8[ebp]
	add	edx, eax
	mov	eax, DWORD PTR -12[ebp]
	add	eax, edx
	mov	DWORD PTR -16[ebp], eax
	mov	eax, DWORD PTR -4[ebp]
	imul	eax, DWORD PTR -8[ebp]
	imul	eax, DWORD PTR -12[ebp]
	mov	edx, eax
	mov	eax, DWORD PTR -16[ebp]
	add	eax, edx
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	foobar, .-foobar
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	push	ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	mov	ebp, esp
	.cfi_def_cfa_register 5
	call	__x86.get_pc_thunk.ax
	add	eax, OFFSET FLAT:_GLOBAL_OFFSET_TABLE_
	push	99
	push	88
	push	77
	call	foobar
	add	esp, 12
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.section	.text.__x86.get_pc_thunk.ax,"axG",@progbits,__x86.get_pc_thunk.ax,comdat
	.globl	__x86.get_pc_thunk.ax
	.hidden	__x86.get_pc_thunk.ax
	.type	__x86.get_pc_thunk.ax, @function
__x86.get_pc_thunk.ax:
.LFB2:
	.cfi_startproc
	mov	eax, DWORD PTR [esp]
	ret
	.cfi_endproc
.LFE2:
	.ident	"GCC: (Debian 6.3.0-18+deb9u1) 6.3.0 20170516"
	.section	.note.GNU-stack,"",@progbits
