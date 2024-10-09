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
'9010 ReDim ROTATEKEY$(NUMPLAYERS), DOWNKEY$(NUMPLAYERS)
'9015 ReDim LEFTKEY$(NUMPLAYERS), RIGHTKEY$(NUMPLAYERS)
startTime(0) = TIMER
rotateKey$(0) = "w": downKey$(0) = "s": leftKey$(0) = "a": rightKey$(0) = "d"
IF numPlayers > 0 THEN
    startTime(1) = TIMER
    rotateKey$(1) = "i": downKey$(1) = "k": leftKey$(1) = "j": rightKey$(1) = "l"
ENDIF
DO
    key$ = ""
    DO WHILE key$ = ""
        FOR i = 0 TO numPlayers
            IF gameOver(i) = false THEN
                IF TIMER >= startTime(i)+dropRate(i) OR startTime(i) > TIMER THEN
                    GOSUB dropShape
                    startTime(i) = TIMER
                ENDIF
            ENDIF
        NEXT
        key$ = INKEY$
    LOOP
    IF key$ = CHR$(27) THEN
        RETURN
    ELSE
        FOR i = 0 TO numPlayers
            IF gameOver(i) = true THEN
                IF key$ = CHR$(13) THEN RETURN
            ELSE
                SELECT CASE key$
                    CASE rotateKey$(i)
                        eraseShape = true
                        GOSUB drawShape
                        IF shapeAngle(i) = 3 THEN newAngle = 0 ELSE newAngle = shapeAngle(i)+1
                        currentShape = shape(i)
                        currentAngle = newAngle
                        GoSub getRotatedShapeMap
                        rotatedMap$ = currentShapeMap$
                        bX = 0: bY = 0
                        GOSUB shapeCanMove
                        IF shapeCanMove = true THEN
                            shapeAngle(i) = newAngle
                            shapeMap$(i) = rotatedMap$
                        ENDIF
                        eraseShape = false
                        GOSUB drawShape
                    CASE leftKey$(i)
                        eraseShape = true
                        GOSUB drawShape
                        bX = -1: bY = 0: currentShapeMap$ = shapeMap$(i)
                        GOSUB shapeCanMove
                        IF shapeCanMove = true THEN shapeX(i) = shapeX(i)-1
                        eraseShape = false
                        GOSUB drawShape
                    CASE rightKey$(i)
                        eraseShape = true
                        GOSUB drawShape
                        bX = 1: bY = 0: currentShapeMap$ = shapeMap$(i)
                        GOSUB shapeCanMove
                        IF shapeCanMove = true THEN shapeX(i) = shapeX(i)+1
                        eraseShape = false
                        GOSUB drawShape
                    CASE downKey$(i)
                        dropRate(i) = 0
                END SELECT
            ENDIF
        NEXT
    ENDIF
LOOP
RETURN


'===GETROTATEDSHAPEMAP$===
getRotatedShapeMap:
10005 REM PARAMS: CURRENTSHAPE, CURRENTANGLE / CURRENTSHAPEMAP$
10010 NEWBLOCKX = 0
10020 NEWBLOCKY = 0
10030 Select Case CURRENTSHAPE
    10040 Case 0
        10050 CURRENTSHAPEMAP$ = "0000333300000000" 'I
    10060 Case 1
        10070 CURRENTSHAPEMAP$ = "0000111000100000" 'L 1
    10080 Case 2
        10090 CURRENTSHAPEMAP$ = "0000666060000000" 'L 2
    10100 Case 3
        10110 CURRENTSHAPEMAP$ = "00000EE00EE00000" '[]
    10120 Case 4
        10130 CURRENTSHAPEMAP$ = "00000AA0AA000000" 'S 1
    10140 Case 5
        10150 CURRENTSHAPEMAP$ = "0000440004400000" 'S 2
    10160 Case 6
        10170 CURRENTSHAPEMAP$ = "0000555005000000" 'T
    10180 Case Else
        10190 CURRENTSHAPEMAP$ = ""
10200 End Select
10210 ROTATEDMAP$ = String$(4 * 4, "0")
10220 If CURRENTANGLE = 0 Then '0
    10230 Return
10240 Else
    10250 For BLOCKX = 0 To 3
        10260 For BLOCKY = 0 To 3
            10270 Select Case CURRENTANGLE
                10280 Case 1 '270
                    10290 NEWBLOCKX = BLOCKY
                    10300 NEWBLOCKY = 3 - BLOCKX
                10310 Case 2 '180
                    10320 NEWBLOCKX = 3 - BLOCKX
                    10330 NEWBLOCKY = 3 - BLOCKY
                10340 Case 3 '90
                    10350 NEWBLOCKX = 3 - BLOCKY
                    10360 NEWBLOCKY = BLOCKX
            10370 End Select
            10380 Mid$(ROTATEDMAP$, ((NEWBLOCKY * 4) + NEWBLOCKX) + 1, 1) = Mid$(CURRENTSHAPEMAP$, ((BLOCKY * 4) + BLOCKX) + 1, 1)
        10390 Next BLOCKY
    10400 Next BLOCKX
10500 End If
10510 CURRENTSHAPEMAP$ = ROTATEDMAP$
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
' PARAMS: currentShapeMap$, bX, bY / shapeCanMove
FOR blockY = 0 TO 3
    FOR blockX = 0 TO 3
        IF NOT MID$(currentShapeMap$, ((blockY*4)+blockX)+1, 1) = "0" THEN
            pitX = (shapeX(i)+blockX)+bX
            pitY = (shapeY(i)+blockY)+bY
            IF pitX >= 0 AND pitX < 10 AND pitY >= 0 AND pitY < 16 THEN
                IF NOT MID$(pit$(i), (((pitY*10)+pitX)+1), 1) = "0" THEN
                    shapeCanMove = false
                    RETURN
                ENDIF
            ELSE IF pitX < 0 OR pitX >= 10 OR pitY >= 16 THEN
                shapeCanMove = false
                RETURN
            ENDIF
        ENDIF
    NEXT
NEXT
shapeCanMove = true
RETURN


'=== DROPSHAPE ===
dropShape:
14010 BX = 0: BY = 1: CURRENTSHAPEMAP$ = SHAPEMAP$(I)
14020 GoSub 13000 'SHAPECANMOVE SUB
14030 If SHAPECANMOVE = TRUE Then
    14040 ERASESHAPE = TRUE
    14050 GoSub 12000 'DRAWSHAPE SUB
    14060 SHAPEY(I) = SHAPEY(I) + 1
    14070 ERASESHAPE = FALSE
    14080 GoSub 12000 'DRAWSHAPE SUB
Else
    '===SETTLEACTIVESHAPEINPIT===
    For BLOCKY = 0 To 3
        For BLOCKX = 0 To 3
            PITX = SHAPEX(I) + BLOCKX
            PITY = SHAPEY(I) + BLOCKY
            If PITX >= 0 And PITX < 10 And PITY >= 0 And PITY < 16 Then
                If Not Mid$(SHAPEMAP$(I), ((BLOCKY * 4) + BLOCKX) + 1, 1) = "0" Then
                    Mid$(PIT$(I), ((PITY * 10) + PITX) + 1, 1) = Mid$(SHAPEMAP$(I), ((BLOCKY * 4) + BLOCKX) + 1, 1)
                End If
            End If
        Next BLOCKX
    Next BLOCKY
    GAMEOVER(I) = (SHAPEY(I) < 0)
    '===CHECKFORFULLROWS===
    FULLROW = FALSE
    NUMLINES = 0
    For PITY = 0 To 15
        FULLROW = TRUE
        For PITX = 0 To 9
            If Mid$(PIT$(I), ((PITY * 10) + PITX) + 1, 1) = "0" Then
                FULLROW = FALSE
                Exit For
            End If
        Next PITX
        If FULLROW = TRUE Then
            '===REMOVEFULLROW===
            REMOVEDROW = PITY
            For PITYY = REMOVEDROW To 0 Step -1
                For PITXX = 0 To 9
                    If PITYY = 0 Then
                        BLOCKCOLOR$ = "0"
                    Else
                        BLOCKCOLOR$ = Mid$(PIT$(I), (((PITYY - 1) * 10) + PITXX) + 1, 1)
                    End If
                    Mid$(PIT$(I), ((PITYY * 10) + PITXX) + 1, 1) = BLOCKCOLOR$
                Next PITXX
            Next PITYY
            NUMLINES = NUMLINES + 1
        End If
    Next PITY
    If NUMLINES > 0 Then
        J = I + 5
        If NUMLINES = 1 Then SCORES(J) = SCORES(J) + (100 * LEVEL(I))
        If NUMLINES = 2 Then SCORES(J) = SCORES(J) + (300 * LEVEL(I))
        If NUMLINES = 3 Then SCORES(J) = SCORES(J) + (500 * LEVEL(I))
        If NUMLINES = 4 Then SCORES(J) = SCORES(J) + (800 * LEVEL(I)) 'TETRIS!
        LINES(I) = LINES(I) + NUMLINES
        LEVEL(I) = (LINES(I) \ 10) + 1
    End If

    GoSub 7000 'DRAWPIT
    If GAMEOVER(I) = FALSE Then
        GoSub 6000 'CREATESHAPE SUB
        ERASESHAPE = FALSE
        GoSub 12000 'DRAWSHAPE SUB
    End If
    GoSub 8000 'DISPLAYSTATUS SUB
End If
RETURN


