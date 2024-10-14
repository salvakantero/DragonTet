	pragma	6809,6800compat,6809conv,m80ext,shadow,autobranchlength
	section	SECTION_NAME
_wait_for_keypress	export
_print_char_at	export
_main	export
_locate	export
_Menu	export
_Init	export
_CheckScores	export
	extern	shiftRightUnsigned
	extern	shiftRightSigned
	extern	shiftLeft
	extern	shiftByteRightUnsigned
	extern	shiftByteRightSigned
	extern	shiftByteLeft
	extern	_strcpy
	extern	_exit
	extern	_clrscr
	extern	SDIV8
	extern	SDIV16
	extern	MUL16
	extern	DIV8BY7
	extern	DIV8
	extern	DIV16BY10
	extern	DIV16
_CheckScores	equ	*
	pshs	U
	leau	,S
	leas	,U
	puls	U,PC
_Init	equ	*
	lbsr	_Menu
	lbsr	_clrscr
	ldd	#$01
	pshs	B
	leax	G00278+0,PCR
	ldb	,S+
	stb	,X
	ldd	#$17
	pshs	B
	leax	G00278+1,PCR
	ldb	,S+
	stb	,X
	rts
_Menu	equ	*
	ldd	#$05
	pshs	B,A
	ldd	#$05
	pshs	B,A
	lbsr	_locate
	leas	4,S
	ldb	#$41
	sex	promoting
	pshs	B,A
	lbsr	_print_char_at
	leas	2,S
	lbsr	_wait_for_keypress
	rts
_locate	equ	*
	pshs	U
	leau	,S
	ldx	#0x0400
	ldy	4,U
	lda	6,U
	ldb	#32
	mul
	leax	D,X
	stx	0x0080
	leas	,U
	puls	U,PC
_main	equ	*
	clra
	clrb
	clrb
	stb	G00276+0,PCR
	lbra	L00283
L00282	equ	*
	leax	S00281,PCR
	pshs	X
	leax	G00279+0,PCR
	ldb	G00276+0,PCR
	clra	index
	pshs	X
	ldx	#11
	lbsr	MUL16
	puls	X
	leax	D,X
	tfr	X,D
	pshs	B,A
	lbsr	_strcpy
	leas	4,S
	lda	G00276+0,PCR
	ldb	#$64
	mul	result
	clra	promotion
	pshs	B,A
	ldd	#$0578
	subd	,S++
	pshs	B,A
	leax	G00280+0,PCR
	ldb	G00276+0,PCR
	clra	index
	lslb
	rola
	leax	D,X
	ldd	,S++
	std	,X
	inc	G00276+0,PCR
L00283	equ	*
	ldb	G00276+0,PCR
	cmpb	#$05
	lblo	L00282
L00287	equ	*
	lbsr	_Init
	clra
	clrb
	clrb
	stb	G00276+0,PCR
	bra	L00290
L00289	equ	*
	ldb	G00276+0,PCR
	clra	promoting
	pshs	B,A
	lbsr	_CheckScores
	leas	2,S
	inc	G00276+0,PCR
L00290	equ	*
	ldb	G00276+0,PCR
	cmpb	G00277+0,PCR
	bls	L00289
	bra	L00287
_print_char_at	equ	*
	pshs	U
	leau	,S
	ldx	0x0080
	lda	5,U
	sta	,x
	leas	,U
	puls	U,PC
_wait_for_keypress	equ	*
wait_key
	lda	$FF02
	cmpa	#$FF
	beq	wait_key
	rts
S00281	equ	*
	fcc	"DRAGON"
	fcb	0
G00276	equ	*
	zmb	1
G00277	equ	*
	zmb	1
G00278	equ	*
	zmb	2
G00279	equ	*
	zmb	77
G00280	equ	*
	zmb	14
	endsection
