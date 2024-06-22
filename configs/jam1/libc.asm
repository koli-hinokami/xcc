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
