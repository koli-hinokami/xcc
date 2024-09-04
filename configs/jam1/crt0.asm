; vim:ts=8
	; Jam-1 crt0 for stdcall main(int argc, char** argp, char** envp)
	; (the code executed when program starts. be careful here)
_start:	push	ra
	mov	si,	0
	mov	ab,	__libc_envp
	push	b
	push	a
	mov	ab,	__libc_argv
	push	b
	push	a
	mov	ab,	1
	push	b
	push	a
	call	main
	nop
	pop	ra
	nop
	ret
__libc_argv:	
	dw	__libc_argv_0
__libc_envp:	
	dw	0
__libc_argv_0:
	db	106
	db	97
	db	109
	db	49
	db	112
	db	116
	db	111
	db	103
	db	0
