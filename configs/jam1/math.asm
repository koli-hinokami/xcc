; ------------------------------------------------------------------------------
; ----------- Math subroutines graciously provided by James Sharman ------------
; ------------------------------------------------------------------------------
	.segment 7 ; code


; Paramaters: a = numerator low
;             b = numerator high
;             c = denominator
; Overwrites: 
; Returns:    a = quotient low
;             b = quotient high
;             d = remainder
;             57- 109 cycles!         (8086 is 80-90)
Math_Divide_16_8:
	test b
	mov tx,Math_Divide_8_8
	jz tx       ; (8) if b is zero,  use the 8/8 version

	push a      ; (2)   save quotient low
	mov a,b	    ; (1)

;original divide code
	xor d,d     ; (1) remainder
	xor b,b     ; (1) quotient

Math_Divide_16_8_noHeader:
	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	; quotient in b 

	pop a       ; (2)  ; restore low numerator
	cnop ;shouldn't need this anymore

	mov tx,Math_Divide_8_8_noHeader  ;(4)
	jmp tx      ; (3) divide low 8 bit by denominator



; Paramaters: a = numerator
;             c = denominator
; Overwrites: 
; Returns:    a = quotient 
;             d = remainder
;             49 cycles!
Math_Divide_8_8:	proc
	xor d,d     ; (1) remainder
Math_Divide_8_8_noHeader:
	push b      ; (2)
	xor b,b     ; (1) quotient
	
	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	shl a       ; (1) shift top bit out of numerator
	shl d       ; (1) (remainder *2) + new bit
	cmp d,c     ; (1) remainder, denominator
	addc b,b    ; (1) (Quotient *2 ) +carry
	subae d,c   ; (1) remainder-=denominator if above|equal

	mov a,b     ; (1) move quotient into a
	pop b       ; (2)
	ret         ; (2)
	endp



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



; Early out version that can be much faster if one param is zero. Or it adds 12 cycles worst case 
;             8-45 Cycles 
Math_Multiply_8_8_8_zt:	
	mov tx,Math_Multiply_8_8_8_retZ
	test a
	jz tx
	test b 
	jz tx
	; otherwise we fall into Math_Multiply_8_8_8

; Paramaters: a LHS Input
;             b RHS Input
; Overwrites: d
; Returns:    c result
;             37 Cycles  
Math_Multiply_8_8_8:	proc
	push b

	xor c,c       ; set result (low) to zero
	add b,b       ; get next highest bit of rhs
	addac c,a     ; conditionaly add lhs to result (low)

	shl c         ; shift cd up 1 place
	add b,b       ; get next highest bit of rhs
	nop
	addac c,a     ; conditionaly add lhs to result (low)

	shl c         ; shift cd up 1 place
	add b,b       ; get next highest bit of rhs
	nop
	addac c,a     ; conditionaly add lhs to result (low)

	shl c         ; shift cd up 1 place
	add b,b       ; get next highest bit of rhs
	nop
	addac c,a     ; conditionaly add lhs to result (low)

	shl c         ; shift cd up 1 place
	add b,b       ; get next highest bit of rhs
	nop
	addac c,a     ; conditionaly add lhs to result (low)

	shl c         ; shift cd up 1 place
	add b,b       ; get next highest bit of rhs
	nop
	addac c,a     ; conditionaly add lhs to result (low)

	shl c         ; shift cd up 1 place
	add b,b       ; get next highest bit of rhs
	nop
	addac c,a     ; conditionaly add lhs to result (low)

	shl c         ; shift cd up 1 place
	add b,b       ; get next highest bit of rhs
	nop
	addac c,a     ; conditionaly add lhs to result (low)

	pop b
	ret
	endp

; return case for zero test version.
Math_Multiply_8_8_8_retZ:
	xor c,c
	ret






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




; Paramaters: ab LHS Input
;             c  RHS Input
; Overwrites: 
; Returns:    ab result
;             9 - 146 Cycles  
Math_Multiply_16_8_16:		
	test c
	jz Math_Multiply_16_8_16_zero

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
	
	nop
	
	;b is still RHS 
	call Math_Multiply_8_8_8_zt
	
	;cd 
	pop b     
	pop a     ; a is low 8 bits, not contended.  
	add b,c   ; b is some of overlap
	
	pop d
	pop ra
	ret

Math_Multiply_16_8_16_zero:
	mov a,c
	mov b,c
	ret




; Paramaters: ab LHS Input
;             cd  RHS Input
; Overwrites: 
; Returns:    abcd result
;             63 - 381 Cycles  
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



; Paramaters: ab LHS Input	-Signed!
;             cd  RHS Input	/
; Overwrites: cd
; Returns:    ab  result
;             73 - 391
Math_Multiply_fp88_fp88:	proc	
	push ra

	mov tl,b
	xor b,d 
	push b                        ; save combined sign data from L&R
	mov b,tl                                                       ; 10
	
	test b
	jns Math_Multiply_fp88_fp88_LHSPos	                           ;4 - 12
	mov tl,c
	xor c,c
	dec c   
	xor a,c
	xor b,c
	inc a
	incc b              
	mov c,tl              
Math_Multiply_fp88_fp88_LHSPos:

	test d
	jns Math_Multiply_fp88_fp88_RHSPos	
	mov tl,a 
 	xor a,a
 	dec a   
 	xor c,a
 	xor d,a
 	inc c
 	incc d              
	mov a,tl
Math_Multiply_fp88_fp88_RHSPos:
	
	push a
	push b

	nop
	call Math_Multiply_16_8   ; abc=ab*c         ; low triple                 9 - 168
	
	mov th,b 
	pop b
	pop a
	
	nop	
	push th
	push c               ;cb from first mul on stack.
	mov c,d
	
	call Math_Multiply_16_8_16   ; ab=ab*c        ; high double               9 - 146	
	
	pop d         ; c from low tripple. 
	add b,d
	
	pop d         ; b from low tribble
	add a,d
	incc b

	pop c         ; sign data of result. 	
	test c
	jns Math_Multiply_fp88_fp88_ResultPos	
	xor c,c       ;negate if needed.
	dec c   
	xor a,c
	xor b,c
	inc a
	incc b              
Math_Multiply_fp88_fp88_ResultPos:

	pop ra
	ret
	endp




; Paramaters: ab LHS Input
; Overwrites:   
; Returns:    ab  result
;             67 - 371
Math_Square_fp88:	proc
	push ra

	push c
	push d                                      ; 9

	test b
	jns Math_Square_fp88_Pos	                ; 4 - 12
	mov tl,c
	xor c,c
	dec c   
	xor a,c
	xor b,c
	inc a
	incc b              
	mov c,tl              
Math_Square_fp88_Pos:
	
	push a
	push b
	mov c,a                                    ; 5

	call Math_Multiply_16_8   ; abc=ab*c         ; low triple                 9 - 168
	
	mov th,b 
	pop b
	pop a
	
	nop	
	push th
	push c               ;cb from first mul on stack.
	mov c,b                                                  ; 11

	call Math_Multiply_16_8_16   ; ab=ab*c        ; high double               9 - 146	
	
	pop d         ; c from low tripple. 
	add b,d
	
	pop d         ; b from low tribble
	add a,d
	incc b

	pop d
	pop c
	pop ra
	ret                                                           ; 20
	endp
	
; Paramaters: ab  numerator
;             cd  denominator
; Overwrites: cd
; Returns:    ab  result
; notes:      RHS only has 7 bit's precision. 
;  
; Varients:            
;   Math_Divide_fp88_fp88_unsigned         -  unsigned version (both values should be positive) 
;   Math_Divide_fp88_fp88_denom_unsigned   -  denominator unsigned  (numerator can be negative)  
;   Math_Divide_fp88_fp88                  -  Generic version 

Math_Divide_fp88_fp88:	proc
	test d
	jns Math_Divide_fp88_fp88_denom_unsigned
	
	; we need to invert cd
	push a
	push b
	
	mov a,c
	mov b,d
	xor c,c
	xor d,d 
	sub c,a
	subb d,b
	
	pop b
	pop a
	
	test b
	
	js Math_Divide_fp88_fp88_Both_negative
	
	push ra  
	
	call Math_Divide_fp88_fp88_unsigned

	;and on the return negate result.
	
	mov c,a
	mov d,b
	xor a,a
	xor b,b 
	sub a,c
	subb b,d	
			
	pop ra	
	ret 
	endp
Math_Divide_fp88_fp88_Both_negative:	proc
	; both are negative, so result will be posative 
	; invert ab
	push c
	push d
	
	mov c,a
	mov d,b
	xor a,a
	xor b,b 
	sub a,c
	subb b,d
	
	pop d
	pop c 
	
	jmp Math_Divide_fp88_fp88_unsigned	; tailcall unsigned version. 	
	endp


Math_Divide_fp88_fp88_denom_unsigned:	proc
	test b
	jns Math_Divide_fp88_fp88_unsigned
	
	push ra
	
	; we need to invert ab
	push c
	push d
	
	mov c,a
	mov d,b
	xor a,a
	xor b,b 
	sub a,c
	subb b,d
	
	pop d
	pop c 
	
	call Math_Divide_fp88_fp88_unsigned

	;and on the return negate result.    
	
	mov c,a
	mov d,b
	xor a,a
	xor b,b 
	sub a,c
	subb b,d	
			
	pop ra	
	ret 
	endp
	

Math_Divide_fp88_fp88_unsigned:		proc
	push a
	
	xor a,a 
	test d
	jz Math_Divide_fp88_fp88_denom_hz
	
	mov tx,Math_Divide_fp88_fp88_denom_hnz_loop
Math_Divide_fp88_fp88_denom_hnz_loop:
	inc a
	clc
	shr d
	shr c
	test d
	jnz tx              ; d = 0,   a is bits need to shift to get there.
	
Math_Divide_fp88_fp88_denom_hz:
	test c              
	jns Math_Divide_fp88_fp88_denom_lmsbz
	clc
	shr c 
	inc a               ; make sure msb of c is zero  (Our quick divide only goes to 7 bits)               	
Math_Divide_fp88_fp88_denom_lmsbz:	
	; we have shifted th denominator down by 'a' bits  
	; shuffle original ab into abd= ab<<8
	; then shift down by the same number of bits as denominator. 
	
	mov d,b 
	pop b   ; b = original a
	
	nop
	push c  ; our 7 bit denominator.  
	mov c,a 
	xor a,a 
	test c
	
	jz Math_Divide_fp88_fp88_normalized
	
	
	mov tx,Math_Divide_fp88_fp88_norm_lp
Math_Divide_fp88_fp88_norm_lp:
	clc
	shr d
	shr b
	shr a 
	dec c
	jnz tx
	
	;abd = modified numerator	

Math_Divide_fp88_fp88_normalized:    
	pop c
	
	; now we can fall unto the partial 24_8 divide.
	endp
; Paramaters: a = numerator low
;             b = numerator high
;             d = numerator top    - assumed less than denominator (i.e. result fits in 16 bits) 
;             c = denominator
; Returns:    a = quotient low
;             b = quotient high
;             d = remainder (overwrite) 
Math_Divide_fp88_util_24_8:	proc
	test d
	jz Math_Divide_16_8       ; (8) if d is zero,  use the 16/8 version

	push a      ; (2)   save quotient low
	mov a,b	    ; (1)   

	xor b,b     ; (1) quotient
	jmp Math_Divide_16_8_noHeader	
	endp
	
	 
; Paramaters: a = numerator low     (Assumes the rusult fits in 8 bits)
;             b = numerator high
;             c = denominator
; Returns:    
;             a = quotient   
;             d = remainder (overwrite) 
Math_Divide_fp88_util_16_8:	proc
	test b
	jz Math_Divide_8_8       ; (8) if d is zero,  use the 16/8 version

	mov d,b	    ; (1)   
	jmp Math_Divide_8_8_noHeader		
	endp
	
	
; Paramaters: abd = numerator 
;             c   = denominator
; Returns:    abd = quotient 
Math_Divide_u24_u8:	proc
	push ra
	test d
	jnz full_div

	call Math_Divide_16_8       ; (8) if d is zero,  use the 16/8 version
	xor d,d                     ; but we kill of remainder, that should be top 8 bits of result.
	pop ra
	ret

full_div:
	push a      ;  save quotient low
	push b      ;  and mid 
	mov a,d	    ;  start with the top 8 bits 

	call Math_Divide_8_8
	
	; a is top 8 bits of result 
	; c is donominator 
	; d is remainder 

	pop tl  ; mid
	pop th  ; low 
	xor b,b     
	push a      ;  top 8 bits of result, needs to finish in d
	push th     ; re-save low
	mov a,tl    ; put mid into a 
	
	call Math_Divide_16_8_noHeader			
	
	pop d       ; restore top 8 bits of result.	
	pop ra
	ret
	endp
	
	
; Paramaters: abd = numerator 
;             c   = denominator
; Returns:    abd = quotient 
; Overites:   c
Math_Divide_s24_u8:	proc
	test d
	jns Math_Divide_u24_u8   ; if it's not negative, easy option. 

	push ra

	; invert abd 
	mov tl,c
	xor c,c
	dec c        ; c = 0xFF	
	xor a,c
	xor b,c
	xor d,c 
	inc a
	incc b
	incc d 
	mov c, tl 	

	call Math_Divide_u24_u8

	mov tl,c
	xor c,c
	dec c        ; c = 0xFF	
	xor a,c
	xor b,c
	xor d,c 
	inc a
	incc b
	incc d 
	mov c, tl 	

	pop ra
	ret	
	endp
	
	
	

; Paramaters: ab = numerator
;             c  = denominator
; Overwrites: d
; Returns:    ab = quotient
Math_Divide_fp88_u8:	proc

	test b
	js _negative
	
	jmp Math_Divide_16_8   ; tailcall the simple divide. 
_negative:
	push ra	
	
	xor d,d
	dec d  
	xor a,d
	xor b,d
	inc a
	incc b     ; ab*=-1
	
	call Math_Divide_16_8
	
	xor d,d
	dec d  
	xor a,d
	xor b,d
	inc a
	incc b     ; ab*=-1
	
	pop ra
	ret
	endp

	
	
	
; Paramaters: ab  = value 
; Overwrites: 
; Returns:    ab  = absolute value 
;              c  = 0/ff for sign of original;
; Time:       10 or 15 (12.5) cycles
;             14 or 19 (16.5) cycles if you care about register C
Math_ABS_16:	proc
	xor c,c
	test b
	jns Math_ABS_16_done ; 8 cycles
	
	dec c   
	xor a,c
	xor b,c
	inc a
	incc b              ; 5 cycles	

Math_ABS_16_done:
	ret
	endp


; vim:ts=8:noet
