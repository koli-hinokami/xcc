; vim:ts=8
	; Jam-1 crt0 for stdcall main(int argc, char** argp, char** envp)
	; (the code executed when program starts. be careful here)
	.extern main
	.segment 1
start:	push	ra
	mov	tx,	sp
	mov	ab,	tx
	sxa	crt0_unwindsp
	mov	si,	0
	mov	ab,	libc_envp
	push	ab
	mov	ab,	libc_argv
	push	ab
	mov	ab,	1
	push	ab
	call	main
	nop
	;mov	ab,	__crt0_leftstring
	;push	b
	;push	a
	;call	puts
	;nop
	pop	ra
	nop
	ret
exit:	.global
	lxa	crt0_unwindsp
	mov	tx,	ab
	nop
	mov	sp,	tx
	pop	ra
	ret
	.segment 2
libc_argv:	dw	libc_argv_0
		dw	0
libc_envp:	dw	0
libc_argv_0:	.sz	"jam1prog"
crt0_leftstring:
	db 0
crt0_unwindsp:	dw 0
	.segment 0
