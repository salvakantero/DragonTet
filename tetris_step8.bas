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
'   - 32x16 caracteres en pantalla, compatible con DRAGON 32/64
'TETRIS_STEP6:
'   - menu de inicio
'   - tabla de puntuaciones
'TETRIS_STEP7:
'   - BASIC clasico no precedural
'TETRIS STEP8:
'   - optimizacion y simplificacion
'   - nombres de variables compatibles con DRAGON 32/64
'       NP = NUMPLAYERS
'       NM$() = NAMES$()
'       SC() = SCORES()
'       GO() = GAMEOVER()
'       DR!() = DROPRATE!()
'       ST!() = STARTTIME()
'       PT$() = PIT$()
'       PL() = PITLEFT()
'       LV() = LEVEL()
'       LN() = LINES()
'       SP() = SHAPE()
'       SA() = SHAPEANGLE()
'       SM$() = SHAPEMAP$()
'       SX() = SHAPEX()
'       SY() = SHAPEY()
'       NS() = NEXTSHAPE()
'       NS$() = NEXTSHAPEMAP$()
'       BC$ = BLOCKCOLOR$
'------------------------------------------
'       PS = PARAMETER SHAPE
'       PA = PARAMETER ANGLE
'       PSM$ = PARAMETER SHAPEMAP$
'       PX = PARAMETER X
'       PY = PARAMETER Y


DefInt A-Z

10 Rem ***TETRIS4DRAGON***
20 Rem  SALVAKANTERO 2024

30 Common Shared TRUE, FALSE, NP, NM$(), SC()
40 Common Shared GO(), DR!(), ST!()
50 Common Shared PT$(), PL(), LV(), LN()
60 Common Shared SP(), SA(), SM$(), SX(), SY()
70 Common Shared NS(), NS$()

80 Randomize Timer
90 Screen 13 '320x200 256C MCGA
100 TRUE = -1: FALSE = 0
110 ReDim PL(1)
120 PL(0) = 1: PL(1) = 23

130 ReDim NM$(6): ReDim SC(6)
140 For I = 0 To 4
    150 NM$(I) = "DRAGON"
    160 SC(I) = 1400 - (I * 100)
170 Next I

200 Rem ===MAIN===
210 GoSub 1000 'INIT
230 For I = 0 To NP
    240 GoSub 2000 'CHECKSCORES
250 Next I
260 GoTo 200

1000 Rem ===INIT===
1010 GoSub 3000 'MENU
1020 Cls
1030 ReDim GO(NP)
1040 ReDim LV(NP)
1050 ReDim LN(NP)
1060 ReDim PT$(NP)
1070 ReDim DR!(NP)
1080 ReDim ST!(NP)
1090 ReDim SP(NP)
1100 ReDim SM$(NP)
1110 ReDim SA(NP)
1120 ReDim SX(NP)
1130 ReDim SY(NP)
1140 ReDim NS(NP)
1150 ReDim NS$(NP)
1190 For I = 0 To NP
    1200 GO(I) = FALSE
    1210 LV(I) = 1
    1220 LN(I) = 0
    1230 SC(I + 5) = 0
    1240 NS(I) = -1
    1250 PT$(I) = String$(10 * 16, "0")
    1260 GoSub 6000 'CREATESHAPE
    1270 GoSub 7000 'DRAWPIT
1280 Next I
1290 GoSub 8000 'DISPLAYSTATUS
1300 GoSub 9000 'GAMELOOP
1310 Return

2000 Rem ===CHECKSCORES===
2005 ID = I + 5
2010 If SC(ID) > SC(4) Then
    2020 GoSub 4000 'MENUHEADER
    2030 Locate 10, 6: Print "BUENA PUNTUACION JUGADOR " + Str$(I + 1)
    2040 Locate 11, 6: Input "NOMBRE?: ", NM$(ID)
    2050 If Len(NM$(ID)) > 10 Then NM$(ID) = Left$(NM$(ID), 10)
    2060 For J = 4 To 0 Step -1
        2070 If SC(ID) > SC(J) Then
            2080 If J < 4 Then
                2090 SC(J + 1) = SC(J)
                2100 NM$(J + 1) = NM$(J)
            2110 End If
        2120 Else
            2130 Exit For
        2140 End If
    2150 Next J
    2160 SC(J + 1) = SC(ID)
    2170 NM$(J + 1) = NM$(ID)
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
    3080 K$ = ""
    3090 Do While K$ = ""
        3100 K$ = InKey$
    3110 Loop
    3120 If K$ = "1" Then '1 PLAYER GAME
        3130 NP = 0
        3140 Exit Do
    3150 ElseIf K$ = "2" Then '2 PLAYER GAME
        3160 NP = 1
        3170 Exit Do
    3180 ElseIf K$ = "3" Then 'HIGH SCORES
        3190 For I = 0 To 4
            3200 Locate 9 + I, 9
            3210 Print "................"
            3220 Locate 9 + I, 9
            3230 Print NM$(I)
            3240 Locate 9 + I, 21
            3250 Print Using "#####"; SC(I)
        3260 Next I
        3270 Locate 15, 4
        3280 Print "PRESS ANY KEY TO CONTINUE..."
        3290 While InKey$ = ""
        3300 Wend
        3310 GoTo 3000 'MENU
    3320 ElseIf K$ = "4" Then
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
6010 DR!(I) = 1 - (LV(I) * 0.2)
6020 If DR!(I) <= 0 Then DR!(I) = .1
6030 If NS(I) >= 0 Then SP(I) = NS(I) Else SP(I) = Int(Rnd * 7)
6040 SA(I) = 0
6045 PS = SP(I): PA = SA(I)
6050 GoSub 10000 'GETROTATEDSHAPEMAP$
6055 SM$(I) = PSM$
6060 SX(I) = 3
6070 SY(I) = -4
6080 NS(I) = Int(Rnd * 7)
6085 PS = NS(I): PA = 0
6090 GoSub 10000 'GETROTATEDSHAPEMAP$
6095 NS$(I) = PSM$
6100 Return

7000 Rem ===DRAWPIT===
7010 For Y = 0 To 15
    7020 For X = 0 To 9
        7030 BC$ = Mid$(PT$(I), ((10 * Y) + X) + 1, 1)
        7035 PX = X: PY = Y
        7040 GoSub 11000 'DRAWBLOCK
    7050 Next X
7060 Next Y
7070 Return

8000 Rem ===DISPLAYSTATUS===
8010 For I = 0 To NP
    8020 If GO(I) = FALSE Then
        8030 For X = 0 To 3
            8040 For Y = 0 To 3
                8050 BC$ = Mid$(NS$(I), ((Y * 4) + X) + 1, 1)
                8060 If BC$ = "0" Then BC$ = "2"
                8070 If I = 0 Then
                    8080 PX = X + 17: PY = Y + 4
                    8090 GoSub 11000 'DRAWBLOCK
                8100 Else
                    8110 PX = X - 5: PY = Y + 12
                    8120 GoSub 11000 'DRAWBLOCK
                8130 End If
            8140 Next Y
        8150 Next X
    8160 End If
8170 Next I
8180 I = I - 1

8200 Color 0, 2
8210 If GO(0) = TRUE Then
    8220 Locate 8, PL(0)
    8230 Print "GAME OVER!"
8240 End If
8250 Locate 2, 12: Print "=PLAYER 1="
8260 Locate 3, 12: Print "LEVEL:" + Str$(LV(0))
8270 Locate 4, 12: Print "LINES:" + Str$(LN(0))
8280 Locate 5, 12: Print "SC:" + Str$(SC(5))
8290 Locate 6, 12: Print "NEXT:"
8300 If NP = 1 Then
    8310 If GO(1) = TRUE Then
        8320 Locate 8, PL(1)
        8330 Print "GAME OVER!"
    8340 End If
    8350 Locate 10, 12: Print "=PLAYER 2="
    8360 Locate 11, 12: Print "LEVEL:" + Str$(LV(1))
    8370 Locate 12, 12: Print "LINES:" + Str$(LN(1))
    8380 Locate 13, 12: Print "SC:" + Str$(SC(6))
    8390 Locate 14, 12: Print "NEXT:"
8400 End If
8410 Return

9000 Rem ===GAMELOOP===
9010 ReDim ROTATEKEY$(NP), LEFTKEY$(NP), DOWNKEY$(NP), RIGHTKEY$(NP)
9020 ST!(0) = Timer
9022 ROTATEKEY$(0) = "w": LEFTKEY$(0) = "a": DOWNKEY$(0) = "s": RIGHTKEY$(0) = "d"
9024 If NP > 0 Then
    9026 ST!(1) = Timer
    9028 ROTATEKEY$(1) = "i": LEFTKEY$(1) = "j": DOWNKEY$(1) = "k": RIGHTKEY$(1) = "l"
9029 End If

9030 Do
    9040 KEY$ = ""
    9050 Do While KEY$ = ""
        9060 For I = 0 To NP
            9070 If GO(I) = FALSE Then
                9080 If Timer >= ST!(I) + DR!(I) Or ST!(I) > Timer Then
                    9090 GoSub 14000 'DROPSP
                    9100 ST!(I) = Timer
                9110 End If
            9120 End If
        9130 Next I
        9140 KEY$ = InKey$
    9150 Loop

    9160 If KEY$ = Chr$(27) Then
        9170 Return
    9180 Else
        9200 For I = 0 To NP
            9210 If GO(I) = TRUE Then
                9220 If KEY$ = Chr$(13) Then Return
            9230 Else
                9350 Select Case KEY$
                    9360 Case ROTATEKEY$(I)
                        9365 ERASESP = TRUE
                        9370 GoSub 12000 'DRAWSP
                        9380 If SA(I) = 3 Then NEWANGLE = 0 Else NEWANGLE = SA(I) + 1
                        9385 CURRENTSP = SP(I): CURRENTANGLE = NEWANGLE
                        9390 GoSub 10000 'GETROTATEDSM$
                        9395 ROTATEDMAP$ = CURRENTSM$
                        9398 BX = 0: BY = 0
                        9400 GoSub 13000 'SPCANMOVE
                        9405 If SPCANMOVE = TRUE Then
                            9410 SA(I) = NEWANGLE
                            9420 SM$(I) = ROTATEDMAP$
                        9430 End If
                        9435 ERASESP = FALSE
                        9440 GoSub 12000 'DRAWSP
                    9450 Case LEFTKEY$(I)
                        9455 ERASESP = TRUE
                        9460 GoSub 12000 'DRAWSP
                        9465 BX = -1: BY = 0: CURRENTSM$ = SM$(I)
                        9470 GoSub 13000 'SPCANMOVE
                        9472 If SPCANMOVE = TRUE Then SX(I) = SX(I) - 1
                        9475 ERASESP = FALSE
                        9480 GoSub 12000 'DRAWSP
                    9490 Case RIGHTKEY$(I)
                        9495 ERASESP = TRUE
                        9500 GoSub 12000 'DRAWSP
                        9505 BX = 1: BY = 0: CURRENTSM$ = SM$(I)
                        9510 GoSub 13000 'SPCANMOVE
                        9512 If SPCANMOVE = TRUE Then SX(I) = SX(I) + 1
                        9515 ERASESP = FALSE
                        9520 GoSub 12000 'DRAWSP
                    9530 Case DOWNKEY$(I)
                        9540 DR!(I) = 0
                9550 End Select
            9560 End If
        9570 Next I
    9580 End If
9590 Loop
9600 Return

10000 Rem ===GETROTATEDSM$===
10005 Rem PARAMS: CURRENTSP, CURRENTANGLE / CURRENTSM$
10010 NEWBLOCKX = 0
10020 NEWBLOCKY = 0
10030 Select Case CURRENTSP
    10040 Case 0
        10050 CURRENTSM$ = "0000333300000000" 'I
    10060 Case 1
        10070 CURRENTSM$ = "0000111000100000" 'L 1
    10080 Case 2
        10090 CURRENTSM$ = "0000666060000000" 'L 2
    10100 Case 3
        10110 CURRENTSM$ = "00000EE00EE00000" '[]
    10120 Case 4
        10130 CURRENTSM$ = "00000AA0AA000000" 'S 1
    10140 Case 5
        10150 CURRENTSM$ = "0000440004400000" 'S 2
    10160 Case 6
        10170 CURRENTSM$ = "0000555005000000" 'T
    10180 Case Else
        10190 CURRENTSM$ = ""
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
            10380 Mid$(ROTATEDMAP$, ((NEWBLOCKY * 4) + NEWBLOCKX) + 1, 1) = Mid$(CURRENTSM$, ((BLOCKY * 4) + BLOCKX) + 1, 1)
        10390 Next BLOCKY
    10400 Next BLOCKX
10500 End If
10510 CURRENTSM$ = ROTATEDMAP$
10520 Return

11000 Rem ===DRAWBLOCK===
11005 Rem PARAMS: BX, BY
11010 Color Val("&H" + BC$)
11020 Locate BY + 1, BX + PL(I)
11030 Print Chr$(219)
11040 Return

12000 Rem ===DRAWSP===
12005 Rem PARAMS: ERASESP
12010 For BLOCKX = 0 To 3
    12020 For BLOCKY = 0 To 3
        12030 PITX = SX(I) + BLOCKX
        12040 PITY = SY(I) + BLOCKY
        12050 If PITX >= 0 And PITX < 10 And PITY >= 0 And PITY < 16 Then
            12060 If ERASESP = TRUE Then
                12070 BC$ = Mid$(PT$(I), ((PITY * 10) + PITX) + 1, 1)
            12080 Else
                12090 BC$ = Mid$(SM$(I), ((BLOCKY * 4) + BLOCKX) + 1, 1)
                12100 If BC$ = "0" Then BC$ = Mid$(PT$(I), ((PITY * 10) + PITX) + 1, 1)
            12110 End If
            12120 BX = PITX: BY = PITY
            12130 GoSub 11000 'DRAWBLOCK
        12140 End If
    12150 Next BLOCKY
12160 Next BLOCKX
12170 Return

13000 Rem ===SPCANMOVE===
13010 Rem PARAMS: CURRENTSM$, BX, BY / SPCANMOVE
13020 For BLOCKY = 0 To 3
    13030 For BLOCKX = 0 To 3
        13040 If Not Mid$(CURRENTSM$, ((BLOCKY * 4) + BLOCKX) + 1, 1) = "0" Then
            13050 PITX = (SX(I) + BLOCKX) + BX
            13060 PITY = (SY(I) + BLOCKY) + BY
            13070 If PITX >= 0 And PITX < 10 And PITY >= 0 And PITY < 16 Then
                13080 If Not Mid$(PT$(I), (((PITY * 10) + PITX) + 1), 1) = "0" Then
                    13090 SPCANMOVE = FALSE
                    13100 Return
                13110 End If
            13120 ElseIf PITX < 0 Or PITX >= 10 Or PITY >= 16 Then
                13130 SPCANMOVE = FALSE
                13140 Return
            13150 End If
        13160 End If
    13170 Next BLOCKX
13180 Next BLOCKY
13190 SPCANMOVE = TRUE
13200 Return

14000 Rem ===DROPSP===
14010 BX = 0: BY = 1: CURRENTSM$ = SM$(I)
14020 GoSub 13000 'SPCANMOVE
14030 If SPCANMOVE = TRUE Then
    14040 ERASESP = TRUE
    14050 GoSub 12000 'DRAWSP
    14060 SY(I) = SY(I) + 1
    14070 ERASESP = FALSE
    14080 GoSub 12000 'DRAWSP
Else
    '===SETTLEACTIVESPINPIT===
    For BLOCKY = 0 To 3
        For BLOCKX = 0 To 3
            PITX = SX(I) + BLOCKX
            PITY = SY(I) + BLOCKY
            If PITX >= 0 And PITX < 10 And PITY >= 0 And PITY < 16 Then
                If Not Mid$(SM$(I), ((BLOCKY * 4) + BLOCKX) + 1, 1) = "0" Then
                    Mid$(PT$(I), ((PITY * 10) + PITX) + 1, 1) = Mid$(SM$(I), ((BLOCKY * 4) + BLOCKX) + 1, 1)
                End If
            End If
        Next BLOCKX
    Next BLOCKY

    GO(I) = (SY(I) < 0)
    '===CHECKFORFULLROWS===
    FULLROW = FALSE
    NUMLN = 0
    For PITY = 0 To 15
        FULLROW = TRUE
        For PITX = 0 To 9
            If Mid$(PT$(I), ((PITY * 10) + PITX) + 1, 1) = "0" Then
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
                        BC$ = "0"
                    Else
                        BC$ = Mid$(PT$(I), (((PITYY - 1) * 10) + PITXX) + 1, 1)
                    End If
                    Mid$(PT$(I), ((PITYY * 10) + PITXX) + 1, 1) = BC$
                Next PITXX
            Next PITYY

            NUMLN = NUMLN + 1
        End If
    Next PITY
    If NUMLN > 0 Then
        J = I + 5
        If NUMLN = 1 Then SC(J) = SC(J) + (100 * LV(I))
        If NUMLN = 2 Then SC(J) = SC(J) + (300 * LV(I))
        If NUMLN = 3 Then SC(J) = SC(J) + (500 * LV(I))
        If NUMLN = 4 Then SC(J) = SC(J) + (800 * LV(I)) 'TETRIS!
        LN(I) = LN(I) + NUMLN
        LV(I) = (LN(I) \ 10) + 1
    End If

    GoSub 7000 'DRAWPIT
    If GO(I) = FALSE Then
        GoSub 6000 'CREATESP
        ERASESP = FALSE
        GoSub 12000 'DRAWSP
    End If
    GoSub 8000 'DISPLAYSTATUS
End If
Return

