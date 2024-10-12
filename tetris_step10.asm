	pragma	6809,6800compat,6809conv,m80ext,shadow,autobranchlength
	section	SECTION_NAME
_main	export
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
	rts
_Menu	equ	*
	rts
_main	equ	*
	clra
	clrb
	clrb
	stb	G00273+0,PCR
	lbra	L00279
L00278	equ	*
	leax	S00277,PCR
	pshs	X
	leax	G00275+0,PCR
	ldb	G00273+0,PCR
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
	lda	G00273+0,PCR
	ldb	#$64
	mul	result
	clra	promotion
	pshs	B,A
	ldd	#$0578
	subd	,S++
	pshs	B,A
	leax	G00276+0,PCR
	ldb	G00273+0,PCR
	clra	index
	lslb
	rola
	leax	D,X
	ldd	,S++
	std	,X
	inc	G00273+0,PCR
L00279	equ	*
	ldb	G00273+0,PCR
	cmpb	#$05
	lblo	L00278
L00283	equ	*
	lbsr	_Init
	clra
	clrb
	clrb
	stb	G00273+0,PCR
	bra	L00286
L00285	equ	*
	ldb	G00273+0,PCR
	clra	promoting
	pshs	B,A
	lbsr	_CheckScores
	leas	2,S
	inc	G00273+0,PCR
L00286	equ	*
	ldb	G00273+0,PCR
	cmpb	G00274+0,PCR
	bls	L00285
	bra	L00283
S00277	equ	*
	fcc	"DRAGON"
	fcb	0
G00273	equ	*
	zmb	1
G00274	equ	*
	zmb	1
G00275	equ	*
	zmb	77
G00276	equ	*
	zmb	14
	endsection
