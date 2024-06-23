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
	
; 16*16->32 Unsigned Multiplication routine, graciously provided by James Sharman
;             63 - 381 Cycles  
; __reg_abcd__ uint32_t ¤(__reg_ab__ uint16_t val1, __reg_cd__ uint16_t val2)
Math_Multiply_16_16: 
	push ra
	
	push a
	push b
	
	call Math_Multiply_16_8   ; abc=ab*c         ; low triple 
	
	mov tl,a 
	mov th,b 
	
	pop b
	pop a
	
	nop
	
	push tl
	push th
	push c
	
	mov c,d 	
	
	call Math_Multiply_16_8   ; abc=ab*c         ; high tripple 
	
	mov d,c     
	mov c,b
	mov b,a       ; shuffle them up into correct location. 
	
	pop a         ; c from low tripple. 
	add c,a
	incc d
	
	pop a         ; b from low tribble
	add b,a
	incc c
	incc d
	
	pop a         ; nothing to ripple add to. 

	pop ra
	ret
; 16*8->24 Unsigned Multiplication routine, graciously provided by James Sharman
; Paramaters: ab LHS Input
;             c  RHS Input
; Overwrites: 
; Returns:    abc result
;             9 - 168 Cycles  
Math_Multiply_16_8:		proc	
	test c
	jz Math_Multiply_16_8_zero

	push ra
	push d

	push b    ;save the upper half of LHS
	mov b,c   ;mov rhs into b. 
	
	call Math_Multiply_8_8_zt
	; cd now our return. 
	
	pop a     ; Get then upper half of LHS
	nop
	push c    ; and save result from first mul.
	push d
	
	;b is still RHS 
	call Math_Multiply_8_8_zt
	
	;cd 
	pop b     
	pop a     ; a is low 8 bits, not contended.  
	add b,c   ; b is some of overlap
	
	incc d    ; that add may carry into d.
	mov c,d 
	
	pop d
	pop ra
	ret
	endp

Math_Multiply_16_8_zero:
	mov a,c
	mov b,c
	ret
; Early out version that can be much faster if one param is zero. Or it adds 12 cycles worst case 
;             8-65 Cycles 
Math_Multiply_8_8_zt:	proc
	mov tx,Math_Multiply_8_8_retZ
	test a
	jz tx
	test b 
	jz tx
	; otherwise we fall into Math_Multiply_8_8
	endp	
	; ^ does a fall downwards
; 8*8->16 Unsigned Multiplication routine, graciously provided by James Sharman
; Paramaters: a LHS Input
;             b RHS Input
; Overwrites: 
; Returns:    cd result
;             53 Cycles  (8086  70-77)
Math_Multiply_8_8:	proc
	push b

	xor c,c       ; set result (low) to zero
	add b,b       ; get next highest bit of rhs
	mov d,c       ; copy zero to result (high) 
	addac c,a     ; conditionaly add lhs to result (low)
	incc d        ;   carry add over to result (high)

	shl c         ; shift cd up 1 place
	shl d
	add b,b       ; get next highest bit of rhs
	nop
	addac c,a     ; conditionaly add lhs to result (low)
	incc d        ;   carry add over to result (high)

	shl c         ; shift cd up 1 place
	shl d
	add b,b       ; get next highest bit of rhs
	nop
	addac c,a     ; conditionaly add lhs to result (low)
	incc d        ;   carry add over to result (high)

	shl c         ; shift cd up 1 place
	shl d
	add b,b       ; get next highest bit of rhs
	nop
	addac c,a     ; conditionaly add lhs to result (low)
	incc d        ;   carry add over to result (high)

	shl c         ; shift cd up 1 place
	shl d
	add b,b       ; get next highest bit of rhs
	nop
	addac c,a     ; conditionaly add lhs to result (low)
	incc d        ;   carry add over to result (high)

	shl c         ; shift cd up 1 place
	shl d
	add b,b       ; get next highest bit of rhs
	nop
	addac c,a     ; conditionaly add lhs to result (low)
	incc d        ;   carry add over to result (high)

	shl c         ; shift cd up 1 place
	shl d
	add b,b       ; get next highest bit of rhs
	nop
	addac c,a     ; conditionaly add lhs to result (low)
	incc d        ;   carry add over to result (high)

	shl c         ; shift cd up 1 place
	shl d
	add b,b       ; get next highest bit of rhs
	nop
	addac c,a     ; conditionaly add lhs to result (low)
	incc d        ;   carry add over to result (high)

	pop b
	ret
	endp
; return case for zero test version.
Math_Multiply_8_8_retZ:	proc
	xor c,c
	mov d,c
	ret
	endp
