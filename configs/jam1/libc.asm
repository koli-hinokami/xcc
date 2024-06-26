; vim:ts=8:
	; Jam-1 Runtime library
	; (add libc functions here)
puts:	push	ra			; enter
	push	si
puts_m:	mov	tx,	sp		; mov	ab,	si
	mov	a,	tl
	mov	b,	th
	mov	cd,	4		; add	ab,	-4
	add	a,	c
	adc	b,	d
	mov	tl,	a		; mov	tx,	ab
	mov	th,	b
	mov	c,	[tx]		; mov	c,	[tx]
	inc	a			; inc	ab
	incc	b
	mov	tl,	a		; mov	tx,	ab
	mov	th,	b
	mov	d,	[tx]		; mov	d,	[tx]
	mov	a,	c		; mov	ab,	cd
	mov	b,	d
	mov	tl,	a		; mov	si,	ab
	mov	th,	b
	nop
	mov	si,	tx		; (end macrolisting)
puts_l:	in	a,	uart_ctrl
	shl	a
	mov	tx,	puts_l
	jlc	tx
	lodsb
	mov	tx,	puts_r
	test	a
	jz	tx
	out	uart_data, a
	mov	tx,	puts_l
	jmp	tx
puts_r: pop	si
	pop	ra
	.times	2	pop	c
	ret
CliWriteinteger:	; My routine for dumping integers to string
	pop	c
	pop	d
	mov	b,	6
CliWriteinteger_loop:
	mov	tx,	CliWriteinteger_waituart
CliWriteinteger_waituart:
	in	a,	uart_ctrl
	shl	a
	jlc	tx
	push	b
	mov	a,	7
	and	a,	c
	mov	b,	48
	add	a,	b
	pop	b
	out	uart_data, a
	.times 3 slr	cd
	dec	b
	jnz	CliWriteinteger_loop
	ret
fixed16_mul:		; stdcall fixed16 ¤(fixed16 a, fixed16 b)
	pop	ab
	pop	cd
	call	Math_Multiply_fp88_fp88
	ret
fixed16_divide:		; stdcall fixed16 ¤(fixed16 numerator, fixed16 denominator)
	pop	ab
	pop	cd
	call	Math_Divide_fp88_fp88
	ret
fixed16_reciprocal:	; stdcall fixed16 ¤(fixed16 num)
	mov	ab,	256
	pop	cd
	call	Math_Divide_fp88_fp88
	ret

