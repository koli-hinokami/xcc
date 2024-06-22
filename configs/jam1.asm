; vim:ts=8
opcode	.architecture	#1	: 
opcode	.segment	#1	: 
opcode	.subsegment 		:
opcode	db	#1		: <#1
opcode	dh	#1		: >#1
opcode	dw	#1		: =#1
opcode	cnop			: 
opcode	nop			: 0x00
opcode	proc			:
opcode	endp			:



;8 bit register to register moves
;		4*3 = 12
opcode	mov	a,	b	: 0x07
opcode	mov	a,	c	: 0x08
opcode	mov	a,	d	: 0x09
opcode	mov	b,	a	: 0x0A
opcode	mov	b,	c	: 0x0B
opcode	mov	b,	d	: 0x0C
opcode	mov	c,	a	: 0x0D
opcode	mov	c,	b	: 0x0E
opcode	mov	c,	d	: 0x0F
opcode	mov	d,	a	: 0x10
opcode	mov	d,	b	: 0x11
opcode	mov	d,	c	: 0x12

opcode	mov	tl,	a	: 0x13
opcode	mov	tl,	b	: 0x14
opcode	mov	tl,	c	: 0x15
opcode	mov	tl,	d	: 0x16
opcode	mov	th,	a	: 0x17
opcode	mov	th,	b	: 0x18
opcode	mov	th,	c	: 0x19
opcode	mov	th,	d	: 0x1a

opcode	mov	a,	tl	: 0x1b
opcode	mov	b,	tl	: 0x1c
opcode	mov	c,	tl	: 0x1d
opcode	mov	d,	tl	: 0x1e
opcode	mov	a,	th	: 0x1f
opcode	mov	b,	th	: 0x20
opcode	mov	c,	th	: 0x21
opcode	mov	d,	th	: 0x22


;16 bit move instructions
opcode	mov	ra,	tx	: 0x23
opcode	mov	tx,	ra	: 0x24
opcode	mov	sp,	tx	: 0x25
opcode	mov	tx,	sp	: 0x26
opcode	mov	si,	tx	: 0x27
opcode	mov	tx,	si	: 0x28
opcode	mov	di,	tx	: 0x29
opcode	mov	tx,	di	: 0x2A
opcode	mov	di,	si	: 0x2B
opcode	mov	si,	di	: 0x2C
opcode	mov	si,	sp	: 0x2D
opcode	mov	di,	sp	: 0x2E


;16 bit increment's and decrements
;	Empty slot		: 0x30
;opcode	dec	sp		: 0x31		;never used 
opcode	dec	si		: 0x32
opcode	dec	di		: 0x33
;opcode	inc	sp		: 0x34	        ;never used 
opcode	inc	si		: 0x35
opcode	inc	di		: 0x36

;IO
opcode	out	gpio,	a	: 0x37
opcode	in	a,	gpio	: 0x3e
opcode	out	spi_data, a 	: 0x37
opcode	out	spi_ctrl, a 	: 0x38
opcode	in	a,	spidata	: 0x3e
opcode	out	uart_data, a	: 0x39
opcode	in	a, uart_data	: 0x3a
opcode	in	a, uart_ctrl	: 0x3b
opcode	out	audio_data, a	: 0x3c
opcode	in	a, cntrl_data	: 0x3d
opcode	in	a,	vga	: 0x31

opcode	break			: 0 0x3f 0 0

;Memory Read
opcode	mov	a,	[si]	: 0x40
opcode	mov	b,	[si]	: 0x41
opcode	mov	c,	[si]	: 0x42
opcode	mov	d,	[si]	: 0x43
opcode	mov	a,	[di]	: 0x44
opcode	mov	b,	[di]	: 0x45
opcode	mov	c,	[di]	: 0x46
opcode	mov	d,	[di]	: 0x47
opcode	mov	a,	[tx]	: 0x48
opcode	mov	b,	[tx]	: 0x49
opcode	mov	c,	[tx]	: 0x4A
opcode	mov	d,	[tx]	: 0x4B

;Memory Write
opcode	mov	[si],	a	: 0x4C
opcode	mov	[si],	b	: 0x4D
opcode	mov	[si],	c	: 0x4E
opcode	mov	[si],	d	: 0x4F
opcode	mov	[di],	a	: 0x50
opcode	mov	[di],	b	: 0x51
opcode	mov	[di],	c	: 0x52
opcode	mov	[di],	d	: 0x53
opcode	mov	[tx],	a	: 0x54
opcode	mov	[tx],	b	: 0x55
opcode	mov	[tx],	c	: 0x56
opcode	mov	[tx],	d	: 0x57

opcode	lodsb			: 0x5b   ;a=[si++]
opcode	stosb			: 0x7e   ;[di++]=a

;flow control

opcode	call	tx		: 0x5c 0
opcode	call	di		: 0x5d 0
opcode	ret			: 0x5e 0

opcode	callBD		tx	: 0x5c
opcode	callBD		di	: 0x5d
opcode	retBD			: 0x5e


opcode	prebranch		: 0x5f
opcode	jmp tx			: 0x5f 0x60
opcode	jmpBD tx		: 0x60

;Single Flag
opcode	jo	tx		: 0x5f 0x61	; Overflow
opcode	jno	tx		: 0x5f 0x62	;!Overflow
opcode	js	tx		: 0x5f 0x63	; Sign
opcode	jns	tx		: 0x5f 0x64	;!Sign
opcode	jz	tx		: 0x5f 0x65	; Zero
opcode	jnz	tx		: 0x5f 0x66	;!Zero
opcode	je	tx		: 0x5f 0x65	; Zero
opcode	jne	tx		: 0x5f 0x66	;!Zero
opcode	jc	tx		: 0x5f 0x67	; Carry
opcode	jnc	tx		: 0x5f 0x68	;!Carry

; Unsigned operation
opcode	jnae	tx		: 0x5f 0x68	; < !Carry
opcode	jb	tx		: 0x5f 0x68	; < !Carry
opcode	jae	tx		: 0x5f 0x67	; >= Carry
opcode	jnb	tx		: 0x5f 0x67	; >= Carry
opcode	jbe	tx		: 0x5f 0x69	; <= !Carry OR Zero
opcode	jna	tx		: 0x5f 0x69	; <= !Carry OR Zero
opcode	ja	tx		: 0x5f 0x6A	; >  Carry AND !Zero
opcode	jnbe	tx		: 0x5f 0x6A	; >  Carry AND !Zero

; Signed operations
opcode	jl	tx		: 0x5f 0x6B	; <  Sign != Overflow
opcode	jnge	tx		: 0x5f 0x6B	; <  Sign != Overflow
opcode	jge	tx		: 0x5f 0x6C	; >= Sign == Overflow
opcode	jnl	tx		: 0x5f 0x6C	; >= Sign == Overflow
opcode	jle	tx		: 0x5f 0x6D	; <= Zero OR ( Sign != Overflow)
opcode	jng	tx		: 0x5f 0x6D	; <= Zero OR ( Sign != Overflow)
opcode	jg	tx		: 0x5f 0x6E	; >  !Zero AND ( Sign == Overflow)
opcode	jnle	tx		: 0x5f 0x6E	; >  !Zero AND ( Sign == Overflow)

; Logical Carry Operation
opcode	jlc	tx		: 0x5f 0x6F	; Logical Carry
opcode	jnlc	tx		: 0x5f 0x70	;!Logical Carry

opcode	jmp	di		: 0x5f 0x71	

; branch macros
opcode	call	#1		: 0x05 <#1 0x06 >#1 0x5c 0
opcode	jmp	#1		: 0x05 <#1 0x06 >#1 0x5f 0x60


;branch macro Single Flag
opcode	jo	#1		: 0x05 <#1 0x06 >#1 0x5f 0x61	 ; Overflow
opcode	jno	#1		: 0x05 <#1 0x06 >#1 0x5f 0x62	 ;!Overflow
opcode	js	#1		: 0x05 <#1 0x06 >#1 0x5f 0x63	 ; Sign
opcode	jns	#1		: 0x05 <#1 0x06 >#1 0x5f 0x64	 ;!Sign
opcode	jz	#1		: 0x05 <#1 0x06 >#1 0x5f 0x65	 ; Zero
opcode	jnz	#1		: 0x05 <#1 0x06 >#1 0x5f 0x66	 ;!Zero
opcode	je	#1		: 0x05 <#1 0x06 >#1 0x5f 0x65	 ; Zero
opcode	jne	#1		: 0x05 <#1 0x06 >#1 0x5f 0x66	 ;!Zero
opcode	jc	#1		: 0x05 <#1 0x06 >#1 0x5f 0x67	 ; Carry
opcode	jnc	#1		: 0x05 <#1 0x06 >#1 0x5f 0x68	 ;!Carry

; branch macro Unsigned operation
opcode	jnae	#1		: 0x05 <#1 0x06 >#1 0x5f 0x68	 ; < !Carry
opcode	jb	#1		: 0x05 <#1 0x06 >#1 0x5f 0x68	 ; < !Carry
opcode	jae	#1		: 0x05 <#1 0x06 >#1 0x5f 0x67	 ; >= Carry
opcode	jnb	#1		: 0x05 <#1 0x06 >#1 0x5f 0x67	 ; >= Carry
opcode	jbe	#1		: 0x05 <#1 0x06 >#1 0x5f 0x69	 ; <= !Carry OR Zero
opcode	jna	#1		: 0x05 <#1 0x06 >#1 0x5f 0x69	 ; <= !Carry OR Zero
opcode	ja	#1		: 0x05 <#1 0x06 >#1 0x5f 0x6A	 ; >  Carry AND !Zero
opcode	jnbe	#1		: 0x05 <#1 0x06 >#1 0x5f 0x6A	 ; >  Carry AND !Zero

; branch macro Signed operations
opcode	jl	#1		: 0x05 <#1 0x06 >#1 0x5f 0x6B	 ; <  Sign != Overflow
opcode	jnge	#1		: 0x05 <#1 0x06 >#1 0x5f 0x6B	 ; <  Sign != Overflow
opcode	jge	#1		: 0x05 <#1 0x06 >#1 0x5f 0x6C	 ; >= Sign == Overflow
opcode	jnl	#1		: 0x05 <#1 0x06 >#1 0x5f 0x6C	 ; >= Sign == Overflow
opcode	jle	#1		: 0x05 <#1 0x06 >#1 0x5f 0x6D	 ; <= Zero OR ( Sign != Overflow)
opcode	jng	#1		: 0x05 <#1 0x06 >#1 0x5f 0x6D	 ; <= Zero OR ( Sign != Overflow)
opcode	jg	#1		: 0x05 <#1 0x06 >#1 0x5f 0x6E	 ; >  !Zero AND ( Sign == Overflow)
opcode	jnle	#1		: 0x05 <#1 0x06 >#1 0x5f 0x6E	 ; >  !Zero AND ( Sign == Overflow)

opcode	jlc	#1		: 0x05 <#1 0x06 >#1 0x5f 0x6F	 ; Logical Carry
opcode	jnlc	#1		: 0x05 <#1 0x06 >#1 0x5f 0x70	 ;!Logical Carry



; Stack operations
opcode	push	a		: 0x72
opcode	push	b		: 0x73
opcode	push	c		: 0x74
opcode	push	d		: 0x75
opcode	push	tl		: 0x76
opcode	push	th		: 0x77

opcode	pop	a		: 0x78
opcode	pop	b		: 0x79
opcode	pop	c		: 0x7a
opcode	pop	d		: 0x7b
opcode	pop	tl		: 0x7c
opcode	pop	th		: 0x7d
	

; Fake 16 bit stack operations
opcode	push	tx		: 0x76 0x77
opcode	push	ra		: 0x24 0x76 0x77
opcode	push	sp		: 0x26 0x76 0x77
opcode	push	si		: 0x28 0x76 0x77
opcode	push	di		: 0x2A 0x76 0x77
opcode	pop	tx		: 0x7d 0x7c
opcode	pop	ra		: 0x7d 0x7c 0 0x23
opcode	pop	sp		: 0x7d 0x7c 0 0x25
opcode	pop	si		: 0x7d 0x7c 0 0x27
opcode	pop	di		: 0x7d 0x7c 0 0x29


;clear carry
opcode	clc			: 0x7F
;shifts
opcode	shl	a		: 0x80
opcode	shl	b		: 0x81
opcode	shl	c		: 0x82
opcode	shl	d		: 0x83
opcode	shr	a		: 0x84
opcode	shr	b		: 0x85
opcode	shr	c		: 0x86
opcode	shr	d		: 0x87

;addition 4*3 = 12
opcode	add	a,	b	: 0x88
opcode	add	a,	c	: 0x89
opcode	add	a,	d	: 0x8A
opcode	add	b,	a	: 0x8B
opcode	add	b,	c	: 0x8C
opcode	add	b,	d	: 0x8D
opcode	add	c,	a	: 0x8E
opcode	add	c,	b	: 0x8F
opcode	add	c,	d	: 0x90
opcode	add	d,	a	: 0x91
opcode	add	d,	b	: 0x92
opcode	add	d,	c	: 0x93
opcode	add	b,	b	: 0x59

;addition 4*3 = 12 (With carry)
opcode	addc	a,	b	: 0x94
opcode	addc	a,	c	: 0x95
opcode	addc	a,	d	: 0x96
opcode	addc	b,	a	: 0x97
opcode	addc	b,	b	: 0x58
opcode	addc	b,	c	: 0x98
opcode	addc	b,	d	: 0x99
opcode	addc	c,	a	: 0x9A
opcode	addc	c,	b	: 0x9B
opcode	addc	c,	d	: 0x9C
opcode	addc	d,	a	: 0x9D
opcode	addc	d,	b	: 0x9E
opcode	addc	d,	c	: 0x9F

opcode	adc	a,	b	: 0x94
opcode	adc	a,	c	: 0x95
opcode	adc	a,	d	: 0x96
opcode	adc	b,	a	: 0x97
opcode	adc	b,	b	: 0x58
opcode	adc	b,	c	: 0x98
opcode	adc	b,	d	: 0x99
opcode	adc	c,	a	: 0x9A
opcode	adc	c,	b	: 0x9B
opcode	adc	c,	d	: 0x9C
opcode	adc	d,	a	: 0x9D
opcode	adc	d,	b	: 0x9E
opcode	adc	d,	c	: 0x9F

opcode	addac	c,	a	: 0x5a

;8 bit increment 
opcode	inc	a		: 0xA0
opcode	inc	b		: 0xA1
opcode	inc	c		: 0xA2
opcode	inc	d		: 0xA3

opcode	incc	a		: 0xA4
opcode	incc	b		: 0xA5
opcode	incc	c		: 0xA6
opcode	incc	d		: 0xA7
opcode	incc	si		: 0x34

;subtract 
opcode	sub	a,	b	: 0xA8
opcode	sub	a,	c	: 0xA9
opcode	sub	a,	d	: 0xAA
opcode	sub	b,	a	: 0xAB
opcode	sub	b,	c	: 0xAC
opcode	sub	b,	d	: 0xAD
opcode	sub	c,	a	: 0xAE
opcode	sub	c,	b	: 0xAF
opcode	sub	c,	d	: 0xB0
opcode	sub	d,	a	: 0xB1
opcode	sub	d,	b	: 0xB2
opcode	sub	d,	c	: 0xB3
opcode	subae	d,	c	: 0x2f

;subtract 
opcode	subb	a,	b	: 0xB4
opcode	subb	a,	c	: 0xB5
opcode	subb	a,	d	: 0xB6
opcode	subb	b,	a	: 0xB7
opcode	subb	b,	c	: 0xB8
opcode	subb	b,	d	: 0xB9
opcode	subb	c,	a	: 0xBA
opcode	subb	c,	b	: 0xBB
opcode	subb	c,	d	: 0xBC
opcode	subb	d,	a	: 0xBD
opcode	subb	d,	b	: 0xBE
opcode	subb	d,	c	: 0xBF

opcode	sbb	a,	b	: 0xB4
opcode	sbb	a,	c	: 0xB5
opcode	sbb	a,	d	: 0xB6
opcode	sbb	b,	a	: 0xB7
opcode	sbb	b,	c	: 0xB8
opcode	sbb	b,	d	: 0xB9
opcode	sbb	c,	a	: 0xBA
opcode	sbb	c,	b	: 0xBB
opcode	sbb	c,	d	: 0xBC
opcode	sbb	d,	a	: 0xBD
opcode	sbb	d,	b	: 0xBE
opcode	sbb	d,	c	: 0xBF

;8 bit decrement
opcode	dec	a		: 0xC0
opcode	dec	b		: 0xC1
opcode	dec	c		: 0xC2
opcode	dec	d		: 0xC3

;and 
opcode	and	a,	b	: 0xC4
opcode	and	a,	c	: 0xC5
opcode	and	a,	d	: 0xC6
opcode	and	b,	a	: 0xC7
opcode	and	b,	c	: 0xC8
opcode	and	b,	d	: 0xC9
opcode	and	c,	a	: 0xCA
opcode	and	c,	b	: 0xCB
opcode	and	c,	d	: 0xCC
opcode	and	d,	a	: 0xCD
opcode	and	d,	b	: 0xCE
opcode	and	d,	c	: 0xCF

;or
opcode	or	a,	b	: 0xD0
opcode	or	a,	c	: 0xD1
opcode	or	a,	d	: 0xD2
opcode	or	b,	a	: 0xD3
opcode	or	b,	c	: 0xD4
opcode	or	b,	d	: 0xD5
opcode	or	c,	a	: 0xD6
opcode	or	c,	b	: 0xD7
opcode	or	c,	d	: 0xD8
opcode	or	d,	a	: 0xD9
opcode	or	d,	b	: 0xDA
opcode	or	d,	c	: 0xDB

;xor
opcode	xor	a,	b	: 0xDC
opcode	xor	a,	c	: 0xDD
opcode	xor	a,	d	: 0xDE
opcode	xor	b,	a	: 0xDF
opcode	xor	b,	c	: 0xE0
opcode	xor	b,	d	: 0xE1
opcode	xor	c,	a	: 0xE2
opcode	xor	c,	b	: 0xE3
opcode	xor	c,	d	: 0xE4
opcode	xor	d,	a	: 0xE5
opcode	xor	d,	b	: 0xE6
opcode	xor	d,	c	: 0xE7
opcode	xor	a,	a	: 0xE8
opcode	xor	b,	b	: 0xE9
opcode	xor	c,	c	: 0xEA
opcode	xor	d,	d	: 0xEB

;not
opcode	not	a		: 0xEC
opcode	not	b		: 0xED
opcode	not	c		: 0xEE
opcode	not	d		: 0xEF

;cmp
opcode	cmp	a,	b	: 0xF0
opcode	cmp	a,	c	: 0xF1
opcode	cmp	a,	d	: 0xF2
opcode	cmp	b,	a	: 0xF3
opcode	cmp	b,	c	: 0xF4
opcode	cmp	b,	d	: 0xF5
opcode	cmp	c,	a	: 0xF6
opcode	cmp	c,	b	: 0xF7
opcode	cmp	c,	d	: 0xF8
opcode	cmp	d,	a	: 0xF9
opcode	cmp	d,	b	: 0xFA
opcode	cmp	d,	c	: 0xFB

;test
opcode	test	a		: 0xFC
opcode	test	b		: 0xFD
opcode	test	c		: 0xFE
opcode	test	d		: 0xFF

; Macro shifts
opcode	sal	ab		: 0x7F 0x80 0x81
opcode	sal	cd		: 0x7F 0x82 0x83
opcode	sal	a		: 0x7F 0x80
opcode	sal	b		: 0x7F 0x81
opcode	sal	c		: 0x7F 0x82
opcode	sal	d		: 0x7F 0x83
opcode	slr	ab		: 0x7F 0x85 0x84
opcode	slr	cd		: 0x7F 0x87 0x86
opcode	slr	a		: 0x7F 0x84
opcode	slr	b		: 0x7F 0x85
opcode	slr	c		: 0x7F 0x86
opcode	slr	d		: 0x7F 0x87

;constant loads
opcode	mov	a,	#1	: 0x01 #1
opcode	mov	b,	#1	: 0x02 #1
opcode	mov	c,	#1	: 0x03 #1
opcode	mov	d,	#1	: 0x04 #1
opcode	mov	tl,	#1	: 0x05 #1
opcode	mov	th,	#1	: 0x06 #1

;fake constant loads
opcode	mov	tx,	#1	: 0x05 <#1 0x06 >#1
opcode	mov	ab,	#1	: 0x01 <#1 0x02 >#1
opcode	mov	cd,	#1	: 0x03 <#1 0x04 >#1
opcode	mov	si,	#1	: 0x05 <#1 0x06 >#1 0x27
opcode	mov	di,	#1	: 0x05 <#1 0x06 >#1 0x29
opcode	mov	ra,	#1	: 0x05 <#1 0x06 >#1 0x23
