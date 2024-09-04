; vim:ts=8:
	; Jam-1 Runtime library
	; (add libc functions here)
	.include "/etc/xcc/jam1/math.asm"
	.segment 7
puts:	.global
	push	ra			; enter
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
;CliWriteinteger:	; My routine for dumping integers to string
;	pop	c
;	pop	d
;	mov	b,	6
;CliWriteinteger_loop:
;	mov	tx,	CliWriteinteger_waituart
;CliWriteinteger_waituart:
;	in	a,	uart_ctrl
;	shl	a
;	jlc	tx
;	push	b
;	mov	a,	7
;	and	a,	c
;	mov	b,	48
;	add	a,	b
;	pop	b
;	out	uart_data, a
;	.times 3 slr	cd
;	dec	b
;	jnz	CliWriteinteger_loop
;	ret
putchar: .global	; stdcall void ¤(char ch)
	mov	tx,	putchar_l
putchar_l:
	in	a,	uart_ctrl
	shl	a
	jlc	tx
	pop	a
	out	uart_data, a
	ret
memset:	.global		; stdcall void ¤(void* area, uint8_t byte, size_t size)
	; TODO: Save `di`
	pop	di
	pop	a
	pop	cd
	mov	tx,	memset_l
memset_l:
	stosb
	dec	c
	jnz	tx
	dec	d
	jnz	tx
	ret
strlen:	.global
	pop	ab
regstrlen:	.global
	mov	d,	0
_l:	mov	tx,	ab
	mov	c,	[tx]
	tjz	c,	_r
	inc	ab
	inc	d
	jnz	_l
	break
_r:	mov	a,	d
	mov	b,	0
	ret
math_divide_16_8: .global	; stdcall uint16_t(uint16_t, uint8_t)
	pop	ab
	pop	c
	jmp	Math_Divide_16_8
	ret
math_abs:	.global
	pop	ab
	jmp	Math_ABS_16
	ret
fixed16_mul:	.global	; stdcall fixed16 ¤(fixed16 a, fixed16 b)
	pop	ab
	pop	cd
	jmp	Math_Multiply_fp88_fp88
	ret
fixed16_divide:	.global	; stdcall fixed16 ¤(fixed16 numerator, fixed16 denominator)
	pop	ab
	pop	cd
	jmp	Math_Divide_fp88_fp88
	ret
fixed16_reciprocal: .global	; stdcall fixed16 ¤(fixed16 num)
	mov	ab,	256
	pop	cd
	jmp	Math_Divide_fp88_fp88
	ret
fixed16_sinetab: .global
	dw	0
	dw	0
	dw	12
	dw	12
	dw	25
	dw	25
	dw	37
	dw	37
	dw	49
	dw	49
	dw	62
	dw	62
	dw	74
	dw	74
	dw	86
	dw	86
	dw	97
	dw	97
	dw	109
	dw	109
	dw	120
	dw	120
	dw	131
	dw	131
	dw	142
	dw	142
	dw	152
	dw	152
	dw	162
	dw	162
	dw	171
	dw	171
	dw	181
	dw	181
	dw	189
	dw	189
	dw	197
	dw	197
	dw	205
	dw	205
	dw	212
	dw	212
	dw	219
	dw	219
	dw	225
	dw	225
	dw	231
	dw	231
	dw	236
	dw	236
	dw	241
	dw	241
	dw	244
	dw	244
	dw	248
	dw	248
	dw	251
	dw	251
	dw	253
	dw	253
	dw	254
	dw	254
	dw	255
	dw	255
	dw	256
	dw	256
	dw	255
	dw	255
	dw	254
	dw	254
	dw	253
	dw	253
	dw	251
	dw	251
	dw	248
	dw	248
	dw	244
	dw	244
	dw	241
	dw	241
	dw	236
	dw	236
	dw	231
	dw	231
	dw	225
	dw	225
	dw	219
	dw	219
	dw	212
	dw	212
	dw	205
	dw	205
	dw	197
	dw	197
	dw	189
	dw	189
	dw	181
	dw	181
	dw	171
	dw	171
	dw	162
	dw	162
	dw	152
	dw	152
	dw	142
	dw	142
	dw	131
	dw	131
	dw	120
	dw	120
	dw	109
	dw	109
	dw	97
	dw	97
	dw	86
	dw	86
	dw	74
	dw	74
	dw	62
	dw	62
	dw	49
	dw	49
	dw	37
	dw	37
	dw	25
	dw	25
	dw	12
	dw	12
	dw	0
	dw	0
	dw	65524
	dw	65524
	dw	65511
	dw	65511
	dw	65499
	dw	65499
	dw	65487
	dw	65487
	dw	65474
	dw	65474
	dw	65462
	dw	65462
	dw	65450
	dw	65450
	dw	65439
	dw	65439
	dw	65427
	dw	65427
	dw	65416
	dw	65416
	dw	65405
	dw	65405
	dw	65394
	dw	65394
	dw	65384
	dw	65384
	dw	65374
	dw	65374
	dw	65365
	dw	65365
	dw	65355
	dw	65355
	dw	65347
	dw	65347
	dw	65339
	dw	65339
	dw	65331
	dw	65331
	dw	65324
	dw	65324
	dw	65317
	dw	65317
	dw	65311
	dw	65311
	dw	65305
	dw	65305
	dw	65300
	dw	65300
	dw	65295
	dw	65295
	dw	65292
	dw	65292
	dw	65288
	dw	65288
	dw	65285
	dw	65285
	dw	65283
	dw	65283
	dw	65282
	dw	65282
	dw	65281
	dw	65281
	dw	65280
	dw	65280
	dw	65281
	dw	65281
	dw	65282
	dw	65282
	dw	65283
	dw	65283
	dw	65285
	dw	65285
	dw	65288
	dw	65288
	dw	65292
	dw	65292
	dw	65295
	dw	65295
	dw	65300
	dw	65300
	dw	65305
	dw	65305
	dw	65311
	dw	65311
	dw	65317
	dw	65317
	dw	65324
	dw	65324
	dw	65331
	dw	65331
	dw	65339
	dw	65339
	dw	65347
	dw	65347
	dw	65355
	dw	65355
	dw	65365
	dw	65365
	dw	65374
	dw	65374
	dw	65384
	dw	65384
	dw	65394
	dw	65394
	dw	65405
	dw	65405
	dw	65416
	dw	65416
	dw	65427
	dw	65427
	dw	65439
	dw	65439
	dw	65450
	dw	65450
	dw	65462
	dw	65462
	dw	65474
	dw	65474
	dw	65487
	dw	65487
	dw	65499
	dw	65499
	dw	65511
	dw	65511
	dw	65524
	dw	65524

