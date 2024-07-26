$Debug

'TETRIS.BAS:
'   - codigo original QBasic de Peter Swinkels (QBBlocks v1.0)
'TETRIS_STEP1:
'   - comprension y comentarios
'   - se habilita la pieza 7
'TETRIS_STEP2:
'   - se convierte a modo texto
'TETRIS_STEP3:
'   - se reordenan las funciones
'   - se anaden niveles
'   - se usan los cursores
'   - se amplia el marcador
'TETRIS_STEP4:
'   - se elimina la posicion X inicial aleatoria
'   - se elimina la rotacion inicial aleatoria
'   - se cambia el sentido de rotacion a la izquierda
'   - se implementa la funcion NEXT PIECE
'TETRIS_STEP5:
'   - 2 jugadores
'   - 32x16 caracteres en pantalla
'TETRIS_STEP6:
'   - menu de inicio
'   - tabla de puntuaciones
'TETRIS_STEP7:
'   - BASIC clasico no precedural

DefInt A-Z

10 Rem ***TETRIS4DRAGON***
20 Rem  SALVAKANTERO 2024

25 Common Shared TRUE, FALSE
30 Common Shared NUMPLAYERS, NAMES$(), SCORES()
40 Common Shared GAMEOVER(), DROPRATE!(), STARTTIME!()
50 Common Shared PIT$(), LEVEL(), LINES(), PITLEFT()
60 Common Shared SHAPE(), SHAPEANGLE(), SHAPEMAP$(), SHAPEX(), SHAPEY()
70 Common Shared NEXTSHAPE(), NEXTSHAPEMAP$()

80 Randomize Timer
90 Screen 13 '320x200 256C MCGA
110 ReDim NAMES$(6): ReDim SCORES(6)
120 For I = 0 To 4
    130 NAMES$(I) = "DRAGON"
    140 SCORES(I) = 1400 - (I * 100)
150 Next I
160 TRUE = -1: FALSE = 0

200 Rem ===MAIN===
210 GoSub 1000 'INIT
230 For I = 0 To NUMPLAYERS
    240 GoSub 2000 'CHECKSCORES
250 Next I
260 GoTo 200

1000 Rem ===INIT===
1010 GoSub 3000 'MENU
1020 Cls
1030 ReDim GAMEOVER(NUMPLAYERS)
1040 ReDim LEVEL(NUMPLAYERS)
1050 ReDim LINES(NUMPLAYERS)
1060 ReDim PIT$(NUMPLAYERS)
1070 ReDim DROPRATE!(NUMPLAYERS)
1080 ReDim STARTTIME!(NUMPLAYERS)
1090 ReDim SHAPE(NUMPLAYERS)
1100 ReDim SHAPEMAP$(NUMPLAYERS)
1110 ReDim SHAPEANGLE(NUMPLAYERS)
1120 ReDim SHAPEX(NUMPLAYERS)
1130 ReDim SHAPEY(NUMPLAYERS)
1140 ReDim NEXTSHAPE(NUMPLAYERS)
1150 ReDim NEXTSHAPEMAP$(NUMPLAYERS)
1160 ReDim PITLEFT(1)
1170 PITLEFT(0) = 1
1180 PITLEFT(1) = 23
1190 For I = 0 To NUMPLAYERS
    1200 GAMEOVER(I) = FALSE
    1210 LEVEL(I) = 1
    1220 LINES(I) = 0
    1230 SCORES(I + 5) = 0
    1240 NEXTSHAPE(I) = -1
    1250 PIT$(I) = String$(10 * 16, "0")
    1260 GoSub 6000 'CREATESHAPE
    1270 GoSub 7000 'DRAWPIT
1280 Next I
1290 GoSub 8000 'DISPLAYSTATUS
1300 GoSub 9000 'GAMELOOP
1310 Return

2000 Rem ===CHECKSCORES===
2005 PI = I + 5
2010 If SCORES(PI) > SCORES(4) Then
    2020 GoSub 4000 'MENUHEADER
    2030 Locate 10, 6: Print "BUENA PUNTUACION JUGADOR " + Str$(I + 1)
    2040 Locate 11, 6: Input "NOMBRE?: ", NAMES$(PI)
    2050 If Len(NAMES$(PI)) > 10 Then NAMES$(PI) = Left$(NAMES$(PI), 10)
    2060 For J = 4 To 0 Step -1
        2070 If SCORES(PI) > SCORES(J) Then
            2080 If J < 4 Then
                2090 SCORES(J + 1) = SCORES(J)
                2100 NAMES$(J + 1) = NAMES$(J)
            2110 End If
        2120 Else
            2130 Exit For
        2140 End If
    2150 Next J
    2160 SCORES(J + 1) = SCORES(PI)
    2170 NAMES$(J + 1) = NAMES$(PI)
2180 End If
2200 Return

3000 Rem ===MENU===
3010 GoSub 4000 'MENU HEADER
3020 Locate 9, 9: Print "1) 1 PLAYER GAME"
3030 Locate 10, 9: Print "2) 2 PLAYER GAME"
3040 Locate 11, 9: Print "3) HIGH SCORES"
3050 Locate 12, 9: Print "4) EXIT"
3060 Locate 15, 8: Print "SELECT OPTION (1-4)"
3070 Do
    3080 KEY$ = ""
    3090 Do While KEY$ = ""
        3100 KEY$ = InKey$
    3110 Loop
    3120 If KEY$ = "1" Then '1 PLAYER GAME
        3130 NUMPLAYERS = 0
        3140 Exit Do
    3150 ElseIf KEY$ = "2" Then '2 PLAYER GAME
        3160 NUMPLAYERS = 1
        3170 Exit Do
    3180 ElseIf KEY$ = "3" Then 'HIGH SCORES
        3190 For I = 0 To 4
            3200 Locate 9 + I, 9
            3210 Print "................"
            3220 Locate 9 + I, 9
            3230 Print NAMES$(I)
            3240 Locate 9 + I, 21
            3250 Print Using "#####"; SCORES(I)
        3260 Next I
        3270 Locate 15, 4
        3280 Print "PRESS ANY KEY TO CONTINUE..."
        3290 While InKey$ = ""
        3300 Wend
        3310 GoTo 3000 'MENU
    3320 ElseIf KEY$ = "4" Then
        3330 End
    3340 End If
3350 Loop
3360 Return

4000 Rem ===MENUHEADER===
4010 Color 0, 2
4020 Cls
4030 Locate 2, 10: Print "***************"
4040 Locate 3, 10: Print "* T E T R I S *"
4050 Locate 4, 10: Print "***************"
4060 Locate 6, 9: Print "SALVAKANTERO 2024"
4070 Return

6000 Rem ===CREATESHAPE===
6010 DROPRATE!(I) = 1 - (LEVEL(I) * 0.2)
6020 If DROPRATE!(I) <= 0 Then DROPRATE!(I) = .1
6030 If NEXTSHAPE(I) >= 0 Then SHAPE(I) = NEXTSHAPE(I) Else SHAPE(I) = Int(Rnd * 7)
6040 SHAPEANGLE(I) = 0
6045 CURRENTSHAPE = SHAPE(I): CURRENTANGLE = SHAPEANGLE(I)
6050 GoSub 10000 'GETROTATEDSHAPEMAP$
6055 SHAPEMAP$(I) = CURRENTSHAPEMAP$
6060 SHAPEX(I) = 3
6070 SHAPEY(I) = -4
6080 NEXTSHAPE(I) = Int(Rnd * 7)
6085 CURRENTSHAPE = NEXTSHAPE(I): CURRENTANGLE = 0
6090 GoSub 10000 'GETROTATEDSHAPEMAP$
6095 NEXTSHAPEMAP$(I) = CURRENTSHAPEMAP$
6100 Return

7000 Rem ===DRAWPIT===
7010 For PITY = 0 To 15
    7020 For PITX = 0 To 9
        7030 BLOCKCOLOR$ = Mid$(PIT$(I), ((10 * PITY) + PITX) + 1, 1)
        7035 BX = PITX: BY = PITY
        7040 GoSub 11000 'DRAWBLOCK
    7050 Next PITX
7060 Next PITY
7070 Return

8000 Rem ===DISPLAYSTATUS===
8010 Color 0, 2
8020 If GAMEOVER(0) = TRUE Then
    8030 Locate 8, PITLEFT(0)
    8040 Print "GAME OVER!"
8050 End If
8060 Locate 2, 12: Print "=PLAYER 1="
8070 Locate 3, 12: Print "LEVEL:" + Str$(LEVEL(0))
8080 Locate 4, 12: Print "LINES:" + Str$(LINES(0))
8090 Locate 5, 12: Print "SC:" + Str$(SCORES(5))
8100 Locate 6, 12: Print "NEXT:"
8110 If NUMPLAYERS = 1 Then
    8120 If GAMEOVER(1) = TRUE Then
        8130 Locate 8, PITLEFT(1)
        8140 Print "GAME OVER!"
    8150 End If
    8160 Locate 10, 12: Print "=PLAYER 2="
    8170 Locate 11, 12: Print "LEVEL:" + Str$(LEVEL(1))
    8180 Locate 12, 12: Print "LINES:" + Str$(LINES(1))
    8190 Locate 13, 12: Print "SC:" + Str$(SCORES(6))
    8200 Locate 14, 12: Print "NEXT:"
8210 End If
8220 For I = 0 To NUMPLAYERS
    8230 If GAMEOVER(I) = FALSE Then
        8240 For BLOCKX = 0 To 3
            8250 For BLOCKY = 0 To 3
                8260 BLOCKCOLOR$ = Mid$(NEXTSHAPEMAP$(I), ((4 * BLOCKY) + BLOCKX) + 1, 1)
                8270 If BLOCKCOLOR$ = "0" Then BLOCKCOLOR$ = "2"
                8280 If I = 0 Then
                    8285 BX = BLOCKX + 17: BY = BLOCKY + 4
                    8290 GoSub 11000 'DRAWBLOCK
                8300 Else
                    8305 BX = BLOCKX - 5: BY = BLOCKY + 12
                    8310 GoSub 11000 'DRAWBLOCK
                8320 End If
            8330 Next BLOCKY
        8340 Next BLOCKX
    8350 End If
8360 Next I
8370 Return

9000 Rem ===GAMELOOP===
9010 STARTTIME!(0) = Timer
9020 If NUMPLAYERS > 0 Then STARTTIME!(1) = Timer
9030 Do
    9040 KEY$ = ""
    9050 Do While KEY$ = ""
        9060 For I = 0 To NUMPLAYERS
            9070 If GAMEOVER(I) = FALSE Then
                9080 If Timer >= STARTTIME!(I) + DROPRATE!(I) Or STARTTIME!(I) > Timer Then
                    9090 GoSub 14000 'DROPSHAPE
                    9100 STARTTIME!(I) = Timer
                9110 End If
            9120 End If
        9130 Next I
        9140 KEY$ = InKey$
    9150 Loop
    9160 If KEY$ = Chr$(27) Then
        9170 Return
    9180 Else
        9200 For I = 0 To NUMPLAYERS
            9210 If GAMEOVER(I) = TRUE Then
                9220 If KEY$ = Chr$(13) Then Return
            9230 Else
                9240 If I = 0 Then
                    9250 ROTATEKEY$ = "w"
                    9260 LEFTKEY$ = "a"
                    9270 DOWNKEY$ = "s"
                    9280 RIGHTKEY$ = "d"
                9290 Else
                    9300 ROTATEKEY$ = "i"
                    9310 LEFTKEY$ = "j"
                    9320 DOWNKEY$ = "k"
                    9330 RIGHTKEY$ = "l"
                9340 End If
                9350 Select Case KEY$
                    9360 Case ROTATEKEY$
                        9365 ERASESHAPE = TRUE
                        9370 GoSub 12000 'DRAWSHAPE
                        9380 If SHAPEANGLE(I) = 3 Then NEWANGLE = 0 Else NEWANGLE = SHAPEANGLE(I) + 1
                        9385 CURRENTSHAPE = SHAPE(I): CURRENTANGLE = NEWANGLE
                        9390 GoSub 10000 'GETROTATEDSHAPEMAP$
                        9395 ROTATEDMAP$ = CURRENTSHAPEMAP$
                        9398 BX = 0: BY = 0
                        9400 GoSub 13000 'SHAPECANMOVE
                        9405 If SHAPECANMOVE = TRUEN Then
                            9410 SHAPEANGLE(I) = NEWANGLE
                            9420 SHAPEMAP$(I) = ROTATEDMAP$
                        9430 End If
                        9435 ERASESHAPE = FALSE
                        9440 GoSub 12000 'DRAWSHAPE
                    9450 Case LEFTKEY$
                        9455 ERASESHAPE = TRUE
                        9460 GoSub 12000 'DRAWSHAPE
                        9465 BX = -1: BY = 0: CURRENTSHAPEMAP$ = SHAPEMAP$(I)
                        9470 GoSub 13000 'SHAPECANMOVE
                        9472 If SHAPECANMOVE = TRUE Then SHAPEX(I) = SHAPEX(I) - 1
                        9475 ERASESHAPE = FALSE
                        9480 GoSub 12000 'DRAWSHAPE
                    9490 Case RIGHTKEY$
                        9495 ERASESHAPE = TRUE
                        9500 GoSub 12000 'DRAWSHAPE
                        9505 BX = 1: BY = 0: CURRENTSHAPEMAP$ = SHAPEMAP$(I)
                        9510 GoSub 13000 'SHAPECANMOVE
                        9512 If SHAPECANMOVE = TRUE Then SHAPEX(I) = SHAPEX(I) + 1
                        9515 ERASESHAPE = FALSE
                        9520 GoSub 12000 'DRAWSHAPE
                    9530 Case DOWNKEY$
                        9540 DROPRATE!(I) = 0
                9550 End Select
            9560 End If
        9570 Next I
    9580 End If
9590 Loop
9600 Return

10000 Rem ===GETROTATEDSHAPEMAP$===
10005 Rem PARAMS: CURRENTSHAPE, CURRENTANGLE / CURRENTSHAPEMAP$
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
10220 If CURRENTANGLE = 0 Then
    10230 Return
10240 Else
    10250 For BLOCKX = 0 To 3
        10260 For BLOCKY = 0 To 3
            10270 Select Case CURRENTANGLE
                10280 Case 1 '270' grados
                    10290 NEWBLOCKX = BLOCKY
                    10300 NEWBLOCKY = 3 - BLOCKX
                10310 Case 2 '180 grados
                    10320 NEWBLOCKX = 3 - BLOCKX
                    10330 NEWBLOCKY = 3 - BLOCKY
                10340 Case 3 '90 grados
                    10350 NEWBLOCKX = 3 - BLOCKY
                    10360 NEWBLOCKY = BLOCKX
            10370 End Select
            10380 Mid$(ROTATEDMAP$, ((NEWBLOCKY * 4) + NEWBLOCKX) + 1, 1) = Mid$(CURRENTSHAPEMAP$, ((BLOCKY * 4) + BLOCKX) + 1, 1)
        10390 Next BLOCKY
    10400 Next BLOCKX
10500 End If
10510 CURRENTSHAPEMAP$ = ROTATEDMAP$
10520 Return

11000 Rem ===DRAWBLOCK===
11005 Rem PARAMS: BX, BY
11010 Color Val("&H" + BLOCKCOLOR$)
11020 Locate BY + 1, BX + PITLEFT(I)
11030 Print Chr$(219)
11040 Return

12000 Rem ===DRAWSHAPE===
12005 Rem PARAMS: ERASESHAPE
12010 For BLOCKX = 0 To 3
    12020 For BLOCKY = 0 To 3
        12030 PITX = SHAPEX(I) + BLOCKX
        12040 PITY = SHAPEY(I) + BLOCKY
        12050 If PITX >= 0 And PITX < 10 And PITY >= 0 And PITY < 16 Then
            12060 If ERASESHAPE = TRUE Then
                12070 BLOCKCOLOR$ = Mid$(PIT$(I), ((PITY * 10) + PITX) + 1, 1)
            12080 Else
                12090 BLOCKCOLOR$ = Mid$(SHAPEMAP$(I), ((BLOCKY * 4) + BLOCKX) + 1, 1)
                12100 If BLOCKCOLOR$ = "0" Then BLOCKCOLOR$ = Mid$(PIT$(I), ((PITY * 10) + PITX) + 1, 1)
            12110 End If
            12120 BX = PITX: BY = PITY
            12130 GoSub 11000 'DRAWBLOCK
        12140 End If
    12150 Next BLOCKY
12160 Next BLOCKX
12170 Return

13000 Rem ===SHAPECANMOVE===
13010 Rem PARAMS: CURRENTSHAPEMAP$, BX, BY / SHAPECANMOVE
13020 For BLOCKY = 0 To 3
    13030 For BLOCKX = 0 To 3
        13040 If Not Mid$(CURRENTSHAPEMAP$, ((BLOCKY * 4) + BLOCKX) + 1, 1) = "0" Then
            13050 PITX = (SHAPEX(I) + BLOCKX) + BX
            13060 PITY = (SHAPEY(I) + BLOCKY) + BY
            13070 If PITX >= 0 And PITX < 10 And PITY >= 0 And PITY < 16 Then
                13080 If Not Mid$(PIT$(I), (((PITY * 10) + PITX) + 1), 1) = "0" Then
                    13090 SHAPECANMOVE = FALSE
                    13100 Return
                13110 End If
            13120 ElseIf PITX < 0 Or PITX >= 10 Or PITY >= 16 Then
                13130 SHAPECANMOVE = FALSE
                13140 Return
            13150 End If
        13160 End If
    13170 Next BLOCKX
13180 Next BLOCKY
13190 SHAPECANMOVE = TRUE
13200 Return

14000 Rem ===DROPSHAPE===
14010 BX = 0: BY = 1: CURRENTSHAPEMAP$ = SHAPEMAP$(I)
14020 GoSub 13000 'SHAPECANMOVE
14030 If SHAPECANMOVE = TRUE Then
    14040 ERASESHAPE = TRUE
    14050 GoSub 12000 'DRAWSHAPE
    14060 SHAPEY(I) = SHAPEY(I) + 1
    14070 ERASESHAPE = FALSE
    14080 GoSub 12000 'DRAWSHAPE
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
        GoSub 6000 'CREATESHAPE
        ERASESHAPE = FALSE
        GoSub 12000 'DRAWSHAPE
    End If
    GoSub 8000 'DISPLAYSTATUS
End If
Return

