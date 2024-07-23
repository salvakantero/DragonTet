DefInt A-Z

10 Rem ***TETRIS4DRAGON***
20 Rem  SALVAKANTERO 2024

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
    1200 GAMEOVER(I) = 0
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
2005 PI = I + 5 '(PLAYER1 = 5, PLAYER2 = 6)
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
    3080 Key$ = ""
    3090 Do While Key$ = "" 'mientras no se pulse una tecla
        3100 Key$ = InKey$
    3110 Loop
    3120 If Key$ = "1" Then
        3130 NUMPLAYERS = 0
        3140 Exit Do
    3150 ElseIf Key$ = "2" Then
        3160 NUMPLAYERS = 1
        3170 Exit Do
    3180 ElseIf Key$ = "3" Then
        3190 GoSub 5000 'HIGHSCORES
        3200 GoTo 3000 'MENU
    3210 ElseIf Key$ = "4" Then
        3220 End
    3230 End If
3240 Loop
3250 Return

4000 Rem ===MENUHEADER===
4010 Color 0, 2
4020 Cls
4030 Locate 2, 10: Print "***************"
4040 Locate 3, 10: Print "* T E T R I S *"
4050 Locate 4, 10: Print "***************"
4060 Locate 6, 9: Print "SALVAKANTERO 2024"
4070 Return

5000 Rem ===HIGHSCORES===
5010 For I = 0 To 4
    5020 Locate 9 + I, 9
    5030 Print "................"
    5040 Locate 9 + I, 9
    5050 Print NAMES$(I)
    5060 Locate 9 + I, 21
    5070 Print Using "#####"; SCORES(I)
5080 Next I
5090 Locate 15, 4
5100 Print "Press any key to continue..."
5110 While InKey$ = ""
5120 Wend
5130 Return

6000 Rem ===CREATESHAPE===
6010 DROPRATE!(I) = 1 - (LEVEL(I) * 0.2) 'tiempo de caida variable segun el nivel
6020 If DROPRATE!(I) <= 0 Then DROPRATE!(I) = .1 'limite de caida de .1
'si no es la primera pieza la toma de NextShape
'si es la primera la genera (0 a 6)
6030 If NEXTSHAPE(I) >= 0 Then SHAPE(I) = NEXTSHAPE(I) Else SHAPE(I) = Int(Rnd * 7)
6040 SHAPEANGLE(I) = 0
6050 'SHAPEMAP$(I) = GetRotatedShapeMap$(SHAPE(I), SHAPEANGLE(I)) 'composicion de la pieza
6060 SHAPEX(I) = 3 'posicion X inicial (centro del foso)
6070 SHAPEY(I) = -4 'posicion Y inicial (totalmente oculta)
6080 'CreateNextShape (i)
6090 Return

7000 Rem ===DRAWPIT===
'repinta el contenido del foso
7010 For PitY = 0 To 15
    7020 For PitX = 0 To 9
        7030 BlockColor$ = Mid$(PIT$(I), ((10 * PitY) + PitX) + 1, 1)
        7040 'DrawBlock BlockColor$, PitX, PitY, i
    7050 Next PitX
7060 Next PitY
7070 Return

8000 Rem ===DISPLAYSTATUS===
8010 Color 0, 2
8020 If GAMEOVER(0) = -1 Then
    8030 Locate 8, PITLEFT(0)
    8040 Print "GAME OVER!"
8050 End If
8060 Locate 2, 12: Print "=PLAYER 1="
8070 Locate 3, 12: Print "Level:" + Str$(LEVEL(0))
8080 Locate 4, 12: Print "Lines:" + Str$(LINES(0))
8090 Locate 5, 12: Print "Sc:" + Str$(SCORES(5))
8100 Locate 6, 12: Print "Next:"
8110 If NUMPLAYERS = 1 Then
    8120 If GAMEOVER(1) = -1 Then
        8130 Locate 8, PITLEFT(1)
        8140 Print "GAME OVER!"
    8150 End If
    8160 Locate 10, 12: Print "=PLAYER 2="
    8170 Locate 11, 12: Print "Level:" + Str$(LEVEL(1))
    8180 Locate 12, 12: Print "Lines:" + Str$(LINES(1))
    8190 Locate 13, 12: Print "Sc:" + Str$(SCORES(6))
    8200 Locate 14, 12: Print "Next:"
8210 End If
8220 For I = 0 To NUMPLAYERS
    8230 'If GameOver(i) = 0 Then DrawNextShape i
8240 Next I
8250 Return

9000 Rem ===GAMELOOP===
9010 STARTTIME!(0) = Timer
9020 If NUMPLAYERS > 0 Then STARTTIME!(1) = Timer
9030 Do
    9040 Key$ = ""
    9050 Do While Key$ = ""
        9060 For I = 0 To NUMPLAYERS
            9070 If GAMEOVER(I) = 0 Then
                9080 If Timer >= STARTTIME!(I) + DROPRATE!(I) Or STARTTIME!(I) > Timer Then
                    9090 'DropShape i
                    9100 STARTTIME!(I) = Timer
                9110 End If
            9120 End If
        9130 Next I
        9140 Key$ = InKey$
    9150 Loop
    9160 If Key$ = Chr$(27) Then
        9170 Return
    9180 Else
        9200 For I = 0 To NUMPLAYERS
            9210 If GAMEOVER(I) = -1 Then
                9220 If Key$ = Chr$(13) Then Return
            9230 Else
                9240 If I = 0 Then
                    9250 RotateKey$ = "w"
                    9260 LeftKey$ = "a"
                    9270 DownKey$ = "s"
                    9280 RightKey$ = "d"
                9290 Else
                    9300 RotateKey$ = "i"
                    9310 LeftKey$ = "j"
                    9320 DownKey$ = "k"
                    9330 RightKey$ = "l"
                9340 End If
                9350 Select Case Key$
                    9360 Case RotateKey$
                        9370 'DrawShape i, -1
                        9380 If SHAPEANGLE(I) = 3 Then NewAngle = 0 Else NewAngle = SHAPEANGLE(I) + 1
                        9390 'RotatedMap$ = GetRotatedShapeMap(Shape(i), NewAngle)
                        9400 'If ShapeCanMove(RotatedMap$, 0, 0, i) Then
                        9410 SHAPEANGLE(I) = NewAngle
                        9420 SHAPEMAP$(I) = RotatedMap$
                        9430 'End If
                        9440 'DrawShape i, 0
                    9450 Case LeftKey$
                        9460 'DrawShape i, -1
                        9470 'If ShapeCanMove(ShapeMap$(i), -1, 0, i) Then ShapeX(i) = ShapeX(i) - 1
                        9480 'DrawShape i, 0
                    9490 Case RightKey$
                        9500 'DrawShape i, -1
                        9510 'If ShapeCanMove(ShapeMap$(i), 1, 0, i) Then ShapeX(i) = ShapeX(i) + 1
                        9520 'DrawShape i, 0
                    9530 Case DownKey$
                        9540 DROPRATE!(I) = 0
                9550 End Select
            9560 End If
        9570 Next I
    9580 End If
9590 Loop
9600 Return

