; vim:ts=8:
	; Jam-1 Runtime library
	; (add libc functions here)
	.include "math.asm"
	.segment 7
puts:	.global ; {{{
	pop	ab
	; }}}
regputs: .global ; {{{
	push	ra
	push	si
	mov	si,	ab
_l:	lodsb
	tjz	a,	_ret
	mov	ra,	_l
	jmp	regputchar
_ret:	pop	si
	pop	ra
	ret
	; }}}
;CliWriteinteger:	; My routine for dumping integers to string {{{
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
; 	; }}}
RtlRegputchar:
regputchar: .global ; {{{
	push	a
	; }}}
putchar: .global	; stdcall void ¤(char ch) ; {{{
	txloop
	in	a,	uart_ctrl
	shl	a
	jlc	tx
	pop	a
	out	uart_data, a
	ret
	; }}}
memset:	.global		; stdcall void ¤(void* area, uint8_t byte, size_t size) ; {{{
	; TODO: Save `di`
	pop	di
	pop	a
	pop	cd
	mov	tx,	_l
_l:
	stosb
	dec	c
	jnz	tx
	dec	d
	jnz	tx
	ret
	; }}}
strlen:	.global ; {{{
	pop	ab
	; }}}
RtlRegstrlen: .global
regstrlen:	.global ; {{{
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
	; }}}
math_divide_16_8: .global	; stdcall uint16_t(uint16_t, uint8_t) ; {{{
	pop	ab
	pop	c
	jmp	Math_Divide_16_8
	ret
	; }}}
math_abs:	.global ; {{{
	pop	ab
	jmp	Math_ABS_16
	ret
	; }}}
fixed16_mul:	.global	; stdcall fixed16 ¤(fixed16 a, fixed16 b) ; {{{
	pop	ab
	pop	cd
	jmp	Math_Multiply_fp88_fp88
	ret
	; }}}
fixed16_divide:	.global	; stdcall fixed16 ¤(fixed16 numerator, fixed16 denominator) ; {{{
	pop	ab
	pop	cd
	jmp	Math_Divide_fp88_fp88
	ret
	; }}}
fixed16_reciprocal: .global	; stdcall fixed16 ¤(fixed16 num) ; {{{
	mov	ab,	256
	pop	cd
	jmp	Math_Divide_fp88_fp88
	ret
	; }}}
printf:	.global
RtlPrintf:	.global	; stdcall void RtlPrint(char* format, ...);{{{
	pop	ab	; Format string
	;}}}
regprintf:	.global
RtlRegprintf:	.global	; varfastcall void RtlRegprintf(reg_ab char* format, stdcall ...);{{{ // Non-reentrant!
	; `printf` implementation in assembly. Non-reentrant~!
	;
	; DI is pointer to variadics. At return it is moved to SP to unwind
	; all of variadics passed here - it's variadic stdcall, y'know.
	;
	; Regvariables:
	;	reg_si	char	*	formatstring
	;	reg_di	void	*	varargs
	;
	; {{{ Entry code
	mov	tx,	sp	; preserve pointer to varargs
	mov	cd,	tx
	push	si		; load pointer to format
	mov	si,	ab
	push	di		; load pointer to varargs
	mov	di,	cd
	push	ra
	; }}}
	; {{{ Main loop + _outchar tappoing
	.global	RtlRegprintf_loop
_loop:	; Get charater
	lodsb
	tjz	a,	_ret		; End of string - done
	cje	a,	b='%',	_fmt	; Handle formats
	; Output charater
_outchar:
	call	RtlRegputchar
	jmp	_loop
	; }}}
	; {{{ Format dispatcher entry and init
	.global	RtlRegprintf_fmt
_fmt:	
	; Zap locals
	push	di
	 mov	di,	_leftjustify
	 push	a
	  xor	a,	a
	  .times 10 stosb
	 pop	a
	pop	di
	; }}}
	; {{{ Flagreading
	; Set up flagreading
	mov	c,	1
	lodsb
	.global	RtlRegprintf_flags
	; Conversion flags
	cjne	a,	b='-',	_noleftjustify
	stc	_leftjustify 
	lodsb
_noleftjustify:
	cjne	a,	b='+',	_noforcesign
	stc	_forcesign   
	lodsb   
_noforcesign:     
	cjne	a,	b=' ',	_noaltforcesign
	stc	_altforcesign  
	lodsb 
_noaltforcesign:  
	cjne	a,	b='#',	_noaltconversion
	stc	_altconversion  
	lodsb
_noaltconversion: 
	cjne	a,	b='0',	_noleadingzero
	stc	_leadingzero  
	lodsb  
_noleadingzero:   
	; }}}
	; {{{ Flagreading -> Fieldwidth
	.global	RtlRegprintf_flags2
	;Field width
	mov	cd,	0
	cje	a,	b='*',	_fieldwidth_asarg
_fieldwidthloop:
	cja	a,	b='9',	_nofieldwidth
	mov	b,	'0'
	cmp	a,	b
	jb	tx
	sub	a,	b
	push	a	;cd*=10
	 sal	cd
	 mov	ab,	cd	;save *2
	 sal	cd
	 sal	cd
	 add	cd,	ab	;*8 + *2 == *10
	pop	a
	add	c,	a
	incc	d
	lodsb
	jmp	_fieldwidthloop
_fieldwidth_asarg:
	lodsb
	mov	cd,	di_autoinc
_nofieldwidth:
	sxc	_fieldwidth
	; }}}
	; {{{ Fieldreading -> Precision
	; Precision
	mov	cd,	0
	cjne	a,	b='.',	_noprecision
	lodsb
	cje	a,	b='*',	_precision_asarg
_precisionloop:
	cja	a,	b='9',	_endprecision
	mov	b,	'0'
	sub	a,	b
	jb	tx
	push	a	;cd*=10
	 sal	cd
	 mov	ab,	cd	;save *2
	 sal	cd
	 sal	cd
	 add	cd,	ab	;*8 + *2 == *10
	pop	a
	add	c,	a
	incc	d
	lodsb
	jmp	_precisionloop
_precision_asarg:
	lodsb
	mov	cd,	di_autoinc
	jmp	_endprecision
_noprecision:
	mov	cd,	-1
_endprecision:
	sxc	_precision
	; }}}
	; {{{ Flagreading -> Lengthmod
	.global	RtlRegprintf_flags3
	; Length modifier
	cje	a,	b='h',	_lengthmodifier_h
	cjne	a,	b='l',	_nolengthmodifier
	mov	a,	2
	sta	_lengthmodifier
	lodsb
	jmp	_nolengthmodifier
_lengthmodifier_h:
	mov	a,	1
	sta	_lengthmodifier
	lodsb
_nolengthmodifier:
	; }}}
	; {{{ Format switch
	.global	RtlRegprintf_formatswitch
	; Format switch
	cje	a,	b='%',	_outchar
	cje	a,	b='c',	_char
	cje	a,	b='s',	_str
	cje	a,	b='d',	_int
	cje	a,	b='i',	_int
	;cje	a,	b='o',	_octal
	;cje	a,	b='x',	_hex
	;cje	a,	b='X',	_capitalhex
	;cje	a,	b='u',	_unsigned
	;cje	a,	b='f',	_float
	;cje	a,	b='F',	_capitalfloat
	;cje	a,	b='e',	_exp
	;cje	a,	b='E',	_capitalexp
	;cje	a,	b='g',	_autofloat
	;cje	a,	b='G',	_capitalautofloat
	;cje	a,	b='n',	_number
	;cje	a,	b='p',	_pointer
	jmp	_unkf
	; Unrecognized format
	.global RtlRegprintf_unkf
_unkf:	break
	jmp	_loop
	; }}}
	; {{{ Old padding routine - `void (_paddings)(reg_ab str)`
;_paddings:	; void(reg_ab char* str); // Output while handling paddings
;	push	ab
;	call	regstrlen
;	lxc	_fieldwidth
;	sub	cd,	ab
;	tjs	d,	_nopad
;	mov	b,	c
;	or	b,	d
;	jz	_nopad
;	lda	_leftjustify
;	tjz	a,	_prepad
;	jmp	_postpad
;_nopad:	; Don't pad
;	pop	ab
;	call	RtlRegputs
;	jmp	_loop
;_prepad: ; Pad before
;	mov	a,	32
;	push	cd
;	 call	RtlRegputchar
;	pop	cd
;	djnz	c,	_prepad
;	tjz	d,	_prepad_finalize
;	djnz	d,	_prepad
;_prepad_finalize:
;	pop	ab
;	call	RtlRegputs
;	jmp	_loop
;_postpad: ; Pad after
;	pop	ab
;	push	cd
;	 call	RtlRegputs
;	pop	cd
;_postpadloop:
;	mov	a,	' '
;	push	cd
;	 call	RtlRegputchar
;	pop	cd
;	djnz	c,	_postpadloop
;	tjz	d,	_loop
;	djnz	d,	_postpadloop
;	jmp	_loop
	; }}}
	; {{{ New padding routine - also `void (_paddings)(reg_ab str)`
_paddings:
	mov	c,	-1
	stc	_precision
	stc	_precision+1
	push	di
	mov	di,	ab
	mov	d,	0	; Number of chars printed
	jmp	_str_tap
	; }}}
	; {{{ %s formatter
_str:	; String formatter
	.global	RtlRegprintf_str
	lxc	_precision
	;tjnz	d,	_str_e
	mov	d,	0	; Number of chars printed
	; DI spoof to target string
	push	di
	mov	ab,	di_autoinc
	mov	di,	ab
_str_tap:
	; Pre-pad
	ldb	_leftjustify
	; | Disabling
	tjz	b,	_str_noprepad
	; | String length
	mov	ab,	di
	push	cd
	 call	RtlRegstrlen
	pop	cd
	; | Clipping
	ldb	_precision+1		; No precision - no clip
	tjs	b,	_str_prepad_noclip
	cmp	a,	c
	jbe	_str_prepad_noclip
	mov	a,	c
_str_prepad_noclip:
	; | Number of chars to pad
	ldb	_fieldwidth
	tjz	b,	_str_noprepad
	sub	a,	b
	not	a
	inc	a
	; | Pad loop
	tjz	a,	_str_noprepad
_str_prepad:
	push	a
	 mov	a,	' '
	 push	cd
	  call	RtlRegputchar
	 pop	cd
	pop	a
	djnz	a,	_str_prepad
_str_noprepad:
	; String + truncation
	push	si
	mov	si,	di
_str_l:	lodsb
	tjz	a,	_str_p
	call	RtlRegputchar
	inc	d
	djnz	c,	_str_l
_str_p:	pop	si
	; Post pad
	ldb	_leftjustify
	; | Disabling
	tjnz	b,	_str_nopostpad
	; | How much to pad
	ldb	_precision+1
	js	_str_nopostpad
	lxa	_fieldwidth
	tjz	a,	_str_nopostpad
	sub	a,	d
	mov	d,	a
	; | Pad loop
	tjz	d,	_str_nopostpad
_str_postpad:
	mov	a,	' '
	push	d
	 call	RtlRegputchar
	pop	d
	djnz	d,	_str_postpad
_str_nopostpad:
	; End
	pop	di
	jmp	_loop
	; }}}
	; {{{ %c formatter
	.global	RtlRegprintf_str_end
_char:	mov	a,	di_autoinc
	sta	_charbuf
	mov	ab,	_charbuf
	jmp	_paddings
	; }}}
	; {{{ %i formatter
_int:	mov	cd,	di_autoinc
	; Take an integer and convert it to a string
	push	di
	mov	di,	_buf
	; Sign
	tjns	d,	_int_positive
	neg	cd
	mov	a,	'-'
	stosb
	jmp	_int_donesign
_int_positive:	lda	_forcesign ; To plus
	tjz	a,	_int_space
	mov	a,	'+'
	stosb
	jmp	_int_donesign
_int_space:
	lda	_altforcesign ; To space
	tjz	a,	_int_donesign
	mov	a,	' '
	stosb
_int_donesign:
	; Preserve the pointer to digits for RtlRegreversestring
	push	di
	; Register reallocation due to division routine
	mov	ab,	cd
	; . Digits *in reverse order* because of call to RtlRegreversestring
	lxc	_precision
	push	c
_int_l: ; | Emit a digit
	mov	c,	10
	call	Math_Divide_16_8
	mov	c,	'0'
	add	d,	c
	mov	di_autoinc,	d
	; | Loop back on precision
	pop	c
	dec	c
	push	c
	jns	_int_l
	; | Loop back on number
	mov	c,	a
	or	c,	b
	jnz	_int_l
	; ' Done with digits
	pop	c
	mov	c,	0
	stosb
	; Reverse the string
	pop	ab
	call	RtlRegreversestring
	; Tailcall to formatter
	pop	di
	mov	ab,	_buf
	jmp	_paddings
	; }}}
	; {{{ Return code
_ret:	mov	cd,	di
	pop	ra
	pop	di
	pop	si
	; It's stdcall, not cdecl
	mov	tx,	cd
	nop
	mov	sp,	tx
	nop
	ret
	; }}}
	; {{{ Locals
	; Locals
	.segment 6
                     ; '%'
_leftjustify:	db 0 ; '-'
_forcesign:	db 0 ; '+'
_altforcesign:	db 0 ; ' '
_altconversion:	db 0 ; '#'
_leadingzero:	db 0 ; '0'
_fieldwidth:	dw 0 ; %i
_precision:	dw 0 ; '.'%i
_lengthmodifier: db 0; h-1,l-2
_charbuf:	.times 2 db 0
_buf:		.times 32 db 0
	.segment 5
	; }}}
	;}}}
RtlReversestring:	.global	; void stdcall(mut char * str) // Reverse {{{
	pop	ab
	; }}}
RtlRegreversestring:	.global	; void fastcall(reg_ab str) {{{
	push	si	; register reg_si char * i;
	push	di	; register reg_di char * j;
_main:	mov	si,	ab
	mov	di,	ab
	; Get to end of string
	mov	tx,	_l1
_l1:	lodsb
	test	a
	jnz	tx
	dec	si
	dec	si
	; Reverse the string
				; do{
_l2:	mov	a,	[si]	; __builtin_xchg_byte(si,di)
	mov	b,	[di]
	mov	[di],	a
	mov	[si],	b
	dec	si		; di++,si--
	inc	di
	mov	ab,	si	; }while(si<di)
	mov	cd,	di
	sub	ab,	cd
	jae	_l2
_ret:	pop	di
	pop	si
	ret
	; }}}
RtlMemset:	.global	; {{{ void(void* data, char val, size_t size)
	; lock repne cmpxchg [_p], a=0, b=1
	pop	ab
	sxa	_d
	pop	a
	sxa	_v
	pop	cd
	push	di
	lxa	_d
	mov	di,	ab
	lxa	_v
	
	mov	tx,	_l
_l:	stosb
	dec	c
	incc	d
	dec	d
	mov	b,	c
	or	b,	d
	jnz	tx
	; lock mov [_p], 0
	pop	di
	ret
_d:	dw 0
_v:	db 0
_s:	dw 0
_p:	db 0
	; }}}
fixed16_sinetab: .global ; {{{
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
	; }}}
; vim:ts=8:fdm=marker
