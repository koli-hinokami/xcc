; vim:ts=8
	; Jam-1 crt0 for stdcall main(int argc, char** argp, char** envp)
	; (the code executed when program starts. be careful here)
	.segment 1
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
	mov	ab,	__crt0_leftstring
	push	b
	push	a
	call	puts
	nop
	pop	ra
	nop
	ret
	.segment 2
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
__crt0_leftstring:
	db 0x63
	db 0x72
	db 0x74
	db 0x30
	db 0x3a
	db 0x20
	db 0x6d
	db 0x61
	db 0x69
	db 0x6e
	db 0x20
	db 0x6c
	db 0x65
	db 0x66
	db 0x74
	db 0x0a
	db 0
	.segment 0
