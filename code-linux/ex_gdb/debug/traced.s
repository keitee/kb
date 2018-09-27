	.file	"traced.c"
	.intel_syntax noprefix
# GNU C (Debian 4.9.2-10) version 4.9.2 (x86_64-linux-gnu)
#	compiled by GNU C version 4.9.2, GMP version 6.0.0, MPFR version 3.1.2-p3, MPC version 1.0.2
# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed:  -imultiarch x86_64-linux-gnu traced.c -masm=intel
# -mtune=generic -march=x86-64 -auxbase-strip traced.s -fverbose-asm
# options enabled:  -faggressive-loop-optimizations
# -fasynchronous-unwind-tables -fauto-inc-dec -fcommon
# -fdelete-null-pointer-checks -fdwarf2-cfi-asm -fearly-inlining
# -feliminate-unused-debug-types -ffunction-cse -fgcse-lm -fgnu-runtime
# -fgnu-unique -fident -finline-atomics -fira-hoist-pressure
# -fira-share-save-slots -fira-share-spill-slots -fivopts
# -fkeep-static-consts -fleading-underscore -fmath-errno
# -fmerge-debug-strings -fpeephole -fprefetch-loop-arrays
# -freg-struct-return -fsched-critical-path-heuristic
# -fsched-dep-count-heuristic -fsched-group-heuristic -fsched-interblock
# -fsched-last-insn-heuristic -fsched-rank-heuristic -fsched-spec
# -fsched-spec-insn-heuristic -fsched-stalled-insns-dep -fshow-column
# -fsigned-zeros -fsplit-ivs-in-unroller -fstrict-volatile-bitfields
# -fsync-libcalls -ftrapping-math -ftree-coalesce-vars -ftree-cselim
# -ftree-forwprop -ftree-loop-if-convert -ftree-loop-im -ftree-loop-ivcanon
# -ftree-loop-optimize -ftree-parallelize-loops= -ftree-phiprop
# -ftree-reassoc -ftree-scev-cprop -funit-at-a-time -funwind-tables
# -fverbose-asm -fzero-initialized-in-bss -m128bit-long-double -m64 -m80387
# -malign-stringops -mavx256-split-unaligned-load
# -mavx256-split-unaligned-store -mfancy-math-387 -mfp-ret-in-387 -mfxsr
# -mglibc -mieee-fp -mlong-double-80 -mmmx -mno-sse4 -mpush-args -mred-zone
# -msse -msse2 -mtls-direct-seg-refs

	.section	.rodata
.LC0:
	.string	"i = %d\n"
	.text
	.globl	do_stuff
	.type	do_stuff, @function
do_stuff:
.LFB0:
	.cfi_startproc
	push	rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp	#,
	.cfi_def_cfa_register 6
	sub	rsp, 32	#,
	mov	DWORD PTR [rbp-20], edi	# my_arg, my_arg
	mov	eax, DWORD PTR [rbp-20]	# tmp86, my_arg
	add	eax, 2	# tmp85,
	mov	DWORD PTR [rbp-8], eax	# my_local, tmp85
	mov	DWORD PTR [rbp-4], 0	# i,
	jmp	.L2	#
.L3:
	mov	eax, DWORD PTR [rbp-4]	# tmp87, i
	mov	esi, eax	#, tmp87
	mov	edi, OFFSET FLAT:.LC0	#,
	mov	eax, 0	#,
	call	printf	#
	add	DWORD PTR [rbp-4], 1	# i,
.L2:
	mov	eax, DWORD PTR [rbp-4]	# tmp88, i
	cmp	eax, DWORD PTR [rbp-8]	# tmp88, my_local
	jl	.L3	#,
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	do_stuff, .-do_stuff
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	push	rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp	#,
	.cfi_def_cfa_register 6
	mov	edi, 2	#,
	call	do_stuff	#
	mov	eax, 0	# D.2213,
	pop	rbp	#
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (Debian 4.9.2-10) 4.9.2"
	.section	.note.GNU-stack,"",@progbits
