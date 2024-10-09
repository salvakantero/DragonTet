'TETRIS.BAS:
'   - original QBasic code by Peter Swinkels (QBBlocks v1.0)
'TETRIS_STEP1:
'   - understanding and comments
'   - shape 7 is enabled
'TETRIS_STEP2:
'   - converts to text mode
'TETRIS_STEP3:
'   - levels are added
'   - the score is widened
'TETRIS_STEP4:
'   - the random initial X position is deleted
'   - the initial random rotation is eliminated
'   - the direction of rotation is changed to the left
'   - the NEXT PIECE function is implemented.
'TETRIS_STEP5:
'   - 2 players
'   - 32x16 characters on screen, compatible with DRAGON 32/64
'TETRIS_STEP6:
'   - menu
'   - score table
'TETRIS_STEP7:
'   - BASIC classic no precedural
'TETRIS STEP8:
'   - optimisation and simplification
'TETRIS STEP9:
'   - converting generic ugBASIC

' ***TETRIS4DRAGON***
'  SALVAKANTERO 2024

DEFINE FLOAT PRECISION SINGLE

DIM true AS SIGNED BYTE = -1
DIM false AS SIGNED BYTE = 0
DIM numPlayers AS BYTE = 2
DIM names$(6) AS STRING
DIM scores(6) AS WORD
DIM gameOver(2) AS BYTE
DIM dropRate(2) AS FLOAT
DIM startTime(2) AS FLOAT
DIM pit$(2) AS STRING
DIM level(2) AS BYTE
DIM lines(2) AS BYTE
DIM pitLeft(2) AS BYTE
DIM shape(2) AS BYTE
DIM shapeAngle(2) AS BYTE
DIM shapeMap$(2) AS STRING
DIM shapeX(2) AS BYTE
DIM shapeY(2) AS BYTE
DIM nextShape(2) AS SIGNED BYTE
DIM nextShapeMap$(2) AS STRING

RANDOMIZE TIMER
pitLeft(0) = 1: pitLeft(1) = 23
FOR i = 0 TO 4
	names$(i) = "DRAGON"
	scores(i) = 1400-(i*100)
NEXT


'===MAIN===
main:
GOSUB init 
FOR i = 0 TO numPlayers
    GOSUB checkScore
NEXT
GOTO main


'===INIT===
init:
GOSUB menu
CLS
FOR i = 0 TO numPlayers
    gameOver(i) = false
    level(i) = 1
    lines(i) = 0
    scores(i+5) = 0
    nextShape(i) = -1
	pit$(i) = STRING("0", 10*16)
    GOSUB createShape
    GOSUB drawPit
NEXT
GOSUB displayStatus
GOSUB gameLoop
RETURN


'=== CHECKSCORE ===
checkScore:
pi = i+5
IF scores(pi) > scores(4) THEN
    GOSUB menuHeader
    LOCATE 6, 10: PRINT "GREAT SCORE PLAYER " + STR$(i+1)
    LOCATE 6, 11: PRINT "NAME?: ";: names$(pi) = INPUT$(10)
    names$(pi) = UPPER$(names$(pi))
    DIM continue AS SIGNED BYTE
    continue = true
    FOR j = 4 TO 0 STEP -1
    	IF continue = true THEN
	        IF scores(pi) > scores(j) THEN
	            IF j < 4 THEN
	                scores(j+1) = scores(j)
	                names$(j+1) = names$(j)
	            ENDIF
	        ELSE
	            continue = false
	        ENDIF
	    ENDIF
    NEXT
    scores(j+1) = scores(pi)
    names$(j+1) = names$(pi)
ENDIF
RETURN


'=== MENU ===
menu:
GOSUB menuHeader
LOCATE 8, 9: PRINT "1) 1 PLAYER GAME"
LOCATE 8, 10: PRINT "2) 2 PLAYER GAME"
LOCATE 8, 11: PRINT "3) HIGH SCORES"
LOCATE 8, 12: PRINT "4) EXIT"
LOCATE 7, 15: PRINT "SELECT OPTION (1-4)"
loop:
key$ = ""
key$ = INKEY$
IF key$ = "" THEN GOTO loop
IF key$ = "1" THEN numPlayers = 0: RETURN
IF key$ = "2" THEN numPlayers = 1: RETURN
IF key$ = "3" THEN GOTO jump
GOTO jump2
jump:
FOR i = 0 TO 4
    LOCATE 8, 9+i: PRINT "................"
    LOCATE 8, 9+i: PRINT names$(i)
    LOCATE 20, 9+i: PRINT scores(i)
NEXT
LOCATE 3, 15
PRINT "PRESS ANY KEY TO CONTINUE..."
key$ = ""
loop2:
key$ = INKEY$
IF key$ = "" THEN GOTO loop2
GOTO menu
jump2:
IF key$ = "4" THEN END
RETURN


'=== MENUHEADER ===
menuHeader:
CLS
LOCATE 9, 2: PRINT "***************"
LOCATE 9, 3: PRINT "* T E T R I S *"
LOCATE 9, 4: PRINT "***************"
LOCATE 8, 6: PRINT "SALVAKANTERO 2024"
RETURN


'=== CREATESHAPE ===
createShape:
dropRate(i) = 1-(level(i)*0.2)
IF dropRate(i) <= 0.0 THEN dropRate(i) = 0.1
IF nextShape(i) >= 0 THEN 
	shape(i) = nextShape(i) 
ELSE 
	shape(i) = RND(7)
ENDIF
shapeAngle(i) = 0
currentShape = shape(i): currentAngle = shapeAngle(i)
GOSUB getRotatedShapeMap
shapeMap$(i) = currentShapeMap$
shapeX(i) = 3
shapeY(i) = -4
nextShape(i) = RND(7)
currentShape = nextShape(i): currentAngle = 0
GOSUB getRotatedShapeMap
nextShapeMap$(i) = currentShapeMap$
RETURN


'=== DRAWPIT ===
drawPit:
FOR pitY = 0 TO 15
	FOR pitX = 0 TO 9
        blockColor$ = MID$(pit$(i), ((10*pitY)+pitX)+1, 1)
        bX = pitX: bY = pitY
        GOSUB drawBlock
    NEXT
NEXT
RETURN


'=== DISPLAYSTATUS ===
displayStatus:
FOR i = 0 TO numPlayers
    IF gameOver(i) = false THEN
        FOR blockX = 0 TO 3
            FOR blockY = 0 TO 3
                blockColor$ = MID$(nextShapeMap$(i), ((4*blockY)+blockX)+1, 1)
                IF blockColor$ = "0" THEN blockColor$ = "2"
                IF i = 0 THEN
                    bX = blockX+17
                    bY = blockY+4
                    GOSUB drawBlock
                ELSE
                    bX = blockX-5
                    bY = blockY+12
                    GOSUB drawBlock
                ENDIF
            NEXT
        NEXT
    ENDIF
NEXT
i = i-1
'8200 Color 0, 2
IF gameOver(0) = true THEN
    LOCATE 8, pitLeft(0)
    PRINT "GAME OVER!"
ENDIF
LOCATE 2, 12: PRINT "=PLAYER 1="
LOCATE 3, 12: PRINT "LEVEL:" + STR$(level(0))
LOCATE 4, 12: PRINT "LINES:" + STR$(lines(0))
LOCATE 5, 12: PRINT "SC:" + STR$(scores(5))
LOCATE 6, 12: PRINT "NEXT:"
IF numPlayers = 1 THEN
    IF gameOver(1) = true THEN
        LOCATE 8, pitLeft(1)
        PRINT "GAME OVER!"
    ENDIF
    LOCATE 10, 12: PRINT "=PLAYER 2="
    LOCATE 11, 12: PRINT "LEVEL:" + STR$(level(1))
    LOCATE 12, 12: PRINT "LINES:" + STR$(lines(1))
    LOCATE 13, 12: PRINT "SC:" + STR$(scores(6))
    LOCATE 14, 12: PRINT "NEXT:"
ENDIF
RETURN


'=== GAMELOOP ===
gameLoop:
RETURN


'===GETROTATEDSHAPEMAP$===
getRotatedShapeMap:
RETURN


'=== DRAWBLOCK ===
drawBlock:
' PARAMS: bX, bY
'Color Val("&H" + BLOCKCOLOR$)
LOCATE bY+1, bX+pitLeft(i)
PRINT CHR$(219)
RETURN


'=== DRAWSHAPE ===
drawShape:
' PARAMS: eraseShape
FOR blockX = 0 TO 3
    FOR blockY = 0 TO 3
        pitX = shapeX(i)+blockX
        pitY = shapeY(i)+blockY
        IF pitX >= 0 AND pitX < 10 AND pitY >= 0 AND pitY < 16 THEN
            IF eraseShape = true THEN
                blockColor$ = MID$(pit$(i), ((pitY*10)+pitX)+1, 1)
            ELSE
                blockColor$ = MID$(shapeMap$(i), ((blockY*4)+blockX)+1, 1)
                If blockColor$ = "0" THEN blockColor$ = MID$(pit$(i), ((pitY*10)+pitX)+1, 1)
            ENDIF
            bX = pitX: bY = pitY
            GOSUB drawBlock
        ENDIF
    NEXT
NEXT
RETURN


'=== SHAPECANMOVE ===
shapeCanMove:
RETURN


'=== DROPSHAPE ===
dropShape:
RETURN


