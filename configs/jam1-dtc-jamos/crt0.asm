	; Jam-1 crt0 for stdcall main(int argc, char** argp, char** envp)
	; combined with DTC Forth threader and singlestack bc(1)-style primitives
	; (the code executed when program starts. be careful here)
	; Register allocation:
	;  tx • Macropackage
	;  ab • W
	;  cd • Top-of-stack
	;  si↑• Interpreter pointer
	;  sp↓• (predec/postinc) Parameterstack
	;  di • Framepointer
	;  ra • Pointer to next
	; Expr~ evaulation is done by default with replaing topofstack.
	; There is pha primitive to save topofstack, and primitives with it 
	;   fused in.
	; Binary operators always pull second operand from stack.
	.extern main
	.extern Math_Multiply_16_16
	.segment 1
start:	push	ra
	nop
	mov	tx,	sp
	mov	ab,	tx
	sxa	crt0_unwindsp
	mov	ab,	0
	push	ab		; envp
	push	di		; argv
	mov	a,	d
	mov	b,	0
	push	ab		; argc
	mov	ra,	next	; this is threaded code backend!
	mov	si,	unwindref
	mov	ab,	main
	jmp	main
	nop
	;mov	ab,	__crt0_leftstring
	;push	b
	;push	a
	;call	puts
	;nop
	pop	ra
	nop
	ret
unwindref:	dw	quietexit
	.segment 2
quietexit:
	mov	cd,	0
exit:	.global
	lxa	crt0_unwindsp
	mov	tx,	ab
	nop
	mov	sp,	tx
	pop	ra
	ret
next:	.global
	mov	ra,	next
	lodsb
	mov	b,	[si]
	inc	si
	mov	tx,	ab
	jmp	tx
p_enter:	.global
	push	si
	mov	si,	ab
	push	di
	nop
	mov	tx,	sp
	mov	di,	tx
	.times 6 inc si ;; skip CF of `jmp enter` and interpret thread after
	lodsb
	mov	b,	[si]
	inc	si
	mov	cd,	sp
	sub	cd,	ab
	mov	tx,	cd
	nop
	mov	sp,	tx
	ret
p_returndispose:	.global
	mov	tx,	di
	mov	sp,	tx
	nop
	pop	di
	lodsb
	mov	b,	[si]
	;inc	si
	pop	si
	mov	tx,	sp
	nop
	push	cd
	mov	cd,	tx
	add	ab,	cd
	pop	cd
	mov	sp,	ab
	ret
p_literal2_16:	.global
	push	cd
	lodsb
	mov	c,	a
	lodsb
	mov	d,	a
	ret
p_literal2_8:	.global
	push	c
	lodsb
	mov	c,	a
	ret
p_literal_16:	.global
	lodsb
	mov	c,	a
	lodsb
	mov	d,	a
	ret
p_literal_8:	.global
	lodsb
	mov	c,	a
	ret
p_literal_fp_16:	.global
	mov	cd,	di
	lodsb
	add	c,	a
	lodsb
	adc	d,	a
	ret
p_push_16:	.global
	push	cd
	ret
p_push_8:	.global
	push	c
	ret
p_execute:	.global
	mov	ab,	cd
	mov	tx,	cd
	jmp	tx
	ret
p_indexfp:	.global
	mov	ab,	di
	add	cd,	ab
	ret
p_read_abs_16:	.global
	lodsb
	mov	b,	[si]
	inc	si
	mov	tx,	ab
	mov	c,	[tx]
	inc	ab
	mov	tx,	ab
	mov	d,	[tx]
	ret
p_read_abs_8:	.global
	lodsb
	mov	b,	[si]
	inc	si
	mov	tx,	ab
	mov	c,	[tx]
	ret
p_read_fr_16:	.global
	lodsb
	mov	b,	[si]
	inc	si
	mov	cd,	di
	add	ab,	cd
	mov	tx,	ab
	mov	c,	[tx]
	inc	ab
	mov	tx,	ab
	mov	d,	[tx]
	ret
p_read_fr_8:	.global
	lodsb
	mov	b,	[si]
	inc	si
	mov	cd,	di
	add	ab,	cd
	mov	tx,	ab
	mov	c,	[tx]
	ret
p_read_16:	.global
	mov	tx,	cd
	mov	a,	[tx]
	inc	cd
	mov	tx,	cd
	mov	d,	[tx]
	mov	c,	a
	ret
p_read_8:	.global
	mov	tx,	cd
	mov	c,	[tx]
	mov	d,	0
	ret
p_write_16:	.global
	pop	ab
	mov	tx,	ab
	mov	[tx],	c
	inc	ab
	mov	tx,	ab
	mov	[tx],	d
	ret
p_write_8:	.global
	pop	ab
	mov	tx,	ab
	mov	[tx],	c
	ret
p_sar1_8:	.global
	mov	a,	c
	shl	c
	mov	c,	a
	shr	c
	ret
p_sar1_16:	.global
	mov	a,	d
	nop
	shl	d
	nop
	mov	d,	a
	nop
	shr	d
	shr	c
	ret
p_shr1_16:	.global
	clc
	shr	d
	shr	c
	ret
p_shr1_8:	.global
	clc
	shr	c
	ret
p_shl1_16:	.global
	clc
	shl	c
	shl	d
	ret
p_shl1_8:	.global
	clc
	shl	c
	ret
p_add_16:	.global
	pop	ab
	add	cd,	ab
	ret
p_sub_16:	.global
	pop	ab
	sub	ab,	cd
	mov	cd,	ab
	ret
p_and_16:	.global
	pop	ab
	and	c,	a
	and	d,	b
	ret
p_or_16:	.global
	pop	ab
	or	c,	a
	or	d,	b
	ret
p_xor_16:	.global
	pop	ab
	xor	c,	a
	xor	d,	b
	ret
p_zeroextend:	.global
	xor	d,	d
	ret
p_mul_16:	.global
	pop	ab
	call	Math_Multiply_16_16
	mov	cd,	ab
	jmp	next
p_addlit:	.global
	lodsb
	add	c,	a
	lodsb
	adc	d,	a
	ret
p_jmp:	.global
	lodsb
	mov	b,	[si]
	;inc	si
	mov	si,	ab
	ret
p_preinc_16:	.global
	mov	tx,	cd
	mov	a,	[tx]
	inc	cd
	mov	tx,	cd
	mov	b,	[tx]
	inc	ab
	mov	[tx],	b
	dec	c
	incc	d
	dec	d
	mov	tx,	cd
	mov	[tx],	a
	mov	cd,	ab
p_negate_16:	.global
	not	c
	not	d
	inc	c
	incc	d
	ret
p_negate_8:	.global
	not	c
	inc	c
	ret
p_cjae_16:	.global
	pop ab
	sub ab, cd
	jae jump
	jmp nojump
p_cjb_16:	.global
	pop ab
	sub ab, cd
	jb jump
	jmp nojump
p_cjb_8:	.global
	pop a
	cmp a, c
	jb jump
	jmp nojump
p_cjbe_16:	.global
	pop ab
	sub ab, cd
	jb jump
	or a, b
	je jump
	jmp nojump
p_cje_16:	.global
	pop ab
	sub ab, cd
	or a, b
	je jump
	jmp nojump
p_cje_8:	.global
	pop a
	cmp a, c
	je jump
	jmp nojump
p_cjge_16:	.global
	pop ab
	sub ab, cd
	jge jump
	jmp nojump
p_cjl_16:	.global
	pop ab
	sub ab, cd
	jl jump
	jmp nojump
p_cjne_16:	.global
	pop ab
	sub ab, cd
	or a, b
	jne jump
	jmp nojump
p_cjne_8:	.global
	pop a
	cmp a, c
	jne jump
	jmp nojump
p_tjnz_16:	.global
	or c, d
	jnz jump
	jmp nojump
p_tjnz_8:	.global
	test c
	jnz jump
	jmp nojump
p_tjz_16:	.global
	or c, d
	jz jump
	jmp nojump
p_tjz_8:	.global
	test c
	jz jump
	jmp nojump
nojump:
	inc	si
	inc	si
	ret
jump:
	lodsb
	mov	b,	[si]
	mov	si,	ab
	ret
	.segment 2
crt0_unwindsp:	dw 0
	.segment 0
;	vim:ts=8
