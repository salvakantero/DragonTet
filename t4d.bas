DefInt A-Z

10 Rem ***TETRIS4DRAGON***
20 Rem  SALVAKANTERO 2024
30 Common Shared NUMPLAYERS, NAMES$(), SCORES()
40 Common Shared GAMEOVER(), DROPRATE!(), STARTTIME!()
50 Common Shared PIT$(), LEVEL(), LINES(), PITLEFT()
60 Common Shared SHAPE(), SHAPEANGLE(), SHAPEMAP$(), SHAPEX(), SHAPEY()
70 Common Shared NEXTSHAPE(), NEXTSHAPEMAP$()
80 Randomize Timer 'semilla de aleatoriedad
90 Screen 13 '320x200 256 colores MCGA
100 Rem TABLA FAKE DEL TOP 5
110 ReDim NAMES$(6): ReDim SCORES(6)
120 For I = 0 To 4
    130 NAMES$(I) = "DRAGON"
    140 SCORES(I) = 1400 - (I * 100)
150 Next I

200 Rem ===BUCLE PRINCIPAL===
210 GoSub 1000 'INICIALIZACION
220 Rem ACTUALIZA TOP 5
230 For I = 0 To NUMPLAYERS
    240 GoSub 2000 'EVALUA PUNTUACIONES
250 Next I
260 GoTo 200

1000 Rem ===INICIALIZACION===
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
    1200 GAMEOVER(I) = FALSE 'partida en curso
    1210 LEVEL(I) = 1 'nivel inicial
    1220 LINES(I) = 0 'lineas conseguidas
    1230 SCORES(I + 5) = 0 'puntuacion actual
    1240 NEXTSHAPE(I) = -1 'sera necesario generar pieza
    1250 PIT$(I) = String$(PITWIDTH * PITHEIGHT, NOBLOCK$) 'inicializa el foso vacio (tabla de 0)
    '1260 CreateShape (i) 'genera pieza (forma, posicion)
    '1270 DrawPit (i) 'pinta foso
1280 Next I
'1290 DisplayStatus 'pinta el marcador comun
'1300 MainLoop
1310 Return

2000 Rem ===EVALUACION PUNTOS===
2010 Return

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
        '3190 HighScores
        '3200 Menu
    3210 ElseIf Key$ = "4" Then
        3220 End
    3230 End If
3240 Loop
3250 Return

4000 Rem ===MENU HEADER===
4010 Color 0, 2
4020 Cls
4030 Locate 2, 10: Print "***************"
4040 Locate 3, 10: Print "* T E T R I S *"
4050 Locate 4, 10: Print "***************"
4060 Locate 6, 9: Print "SALVAKANTERO 2024"
4070 Return

5000 Rem ===HIGH SCORES===

