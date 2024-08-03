$Debug

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
'   - variable names supported by DRAGON 32/64
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
'       K$ = KEY$
'       UK$ = ROTATEKEY$ = UP KEY
'       LK$ = LEFTKEY$
'       DK$ = DOWNKEY$
'       RK$ = RIGHTKEY$
'       NA = NEWANGLE
'       RM$ = ROTATEDMAP$
'       FR = FULLROW
'       RR = REMOVEDROW
'       NL = NUMLINES
'------------------------------------------
'       PS = PARAMETER SHAPE
'       PA = PARAMETER ANGLE
'       PS$ = PARAMETER SHAPEMAP$
'       PX = PARAMETER X
'       PY = PARAMETER Y
'       PE = PARAMETER ERASESHAPE
'       PM = PARAMETER SHAPECANMOVE

DefInt A-Z

10 Rem ***TETRIS4DRAGON***
20 Rem  SALVAKANTERO 2024

30 Common Shared TRUE, FALSE, NP, NM$(), SC(), GO(), DR!(), ST!()
50 Common Shared PT$(), PL(), LV(), LN(), SP(), SA(), SM$(), SX(), SY()
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
210 GoSub 1000 'INIT SUB
230 For I = 0 To NP
    240 GoSub 2000 'CHECKSCORES SUB
250 Next I
260 GoTo 200

1000 Rem ===INIT===
1010 GoSub 3000 'MENU SUB
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
    1260 GoSub 6000 'CREATESHAPE SUB
    1270 GoSub 7000 'DRAWPIT SUB
1280 Next I
1290 GoSub 8000 'DISPLAYSTATUS SUB
1300 GoSub 9000 'GAMELOOP SUB
1310 Return

2000 Rem ===CHECKSCORES===
2005 ID = I + 5
2010 If SC(ID) > SC(4) Then
    2020 GoSub 4000 'MENUHEADER SUB
    2030 Locate 10, 8: Print "GREAT SCORE PLAYER " + Str$(I + 1)
    2040 Locate 11, 8: Input "NAME?: ", NM$(ID)
    2050 If Len(NM$(ID)) > 10 Then NM$(ID) = Left$(NM$(ID), 10)
    2055 NM$(ID) = UCase$(NM$(ID))
    2060 For J = 4 To 0 Step -1
        2070 If SC(ID) > SC(J) Then
            2080 If J < 4 Then
                2090 SC(J + 1) = SC(J)
                2100 NM$(J + 1) = NM$(J)
            2110 End if
        2120 Else
            2130 Exit For
        2140 End If
    2150 Next J
    2160 SC(J + 1) = SC(ID)
    2170 NM$(J + 1) = NM$(ID)
2180 End If
2200 Return

3000 Rem ===MENU===
3010 GoSub 4000 'MENUHEADER SUB
3020 Locate 9, 9: Print "1) 1 PLAYER GAME"
3030 Locate 10, 9: Print "2) 2 PLAYER GAME"
3040 Locate 11, 9: Print "3) HIGH SCORES"
3050 Locate 12, 9: Print "4) EXIT"
3060 Locate 15, 8: Print "SELECT OPTION (1-4)"
3090 K$ = ""
3100 K$ = InKey$
3110 If K$ = "" Then 3100
3120 If K$ = "1" Then NP = 0: Return
3150 If K$ = "2" Then NP = 1: Return
3180 If K$ = "3" Then 3190
3185 GoTo 3320
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
3285 K$ = ""
3290 K$ = InKey$
3300 If K$ = "" Then 3290
3310 GoTo 3000 'MENU SUB
3320 If K$ = "4" Then End
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
6050 GoSub 10000 'GETROTATEDSHAPEMAP$ SUB
6055 SM$(I) = PS$
6060 SX(I) = 3
6070 SY(I) = -4
6080 NS(I) = Int(Rnd * 7)
6085 PS = NS(I): PA = 0
6090 GoSub 10000 'GETROTATEDSHAPEMAP$ SUB
6095 NS$(I) = PS$
6100 Return

7000 Rem ===DRAWPIT===
7010 For Y = 0 To 15
    7020 For X = 0 To 9
        7030 BC$ = Mid$(PT$(I), ((10 * Y) + X) + 1, 1)
        7035 PX = X: PY = Y
        7040 GoSub 11000 'DRAWBLOCK SUB
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
                    8090 GoSub 11000 'DRAWBLOCK SUB
                8100 Else
                    8110 PX = X - 5: PY = Y + 12
                    8120 GoSub 11000 'DRAWBLOCK SUB
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
9010 ReDim UK$(NP), LK$(NP), DK$(NP), RK$(NP)
9020 ST!(0) = Timer
9022 UK$(0) = "w": LK$(0) = "a": DK$(0) = "s": RK$(0) = "d"
9024 If NP > 0 Then
    9026 ST!(1) = Timer
    9028 UK$(1) = "i": LK$(1) = "j": DK$(1) = "k": RK$(1) = "l"
9029 End If

9030 Do
    9040 K$ = ""
    9050 Do While K$ = ""
        9060 For I = 0 To NP
            9070 If GO(I) = FALSE Then
                9080 If Timer >= ST!(I) + DR!(I) Or ST!(I) > Timer Then
                    9090 GoSub 14000 'DROPSHAPE SUB
                    9100 ST!(I) = Timer
                9110 End If
            9120 End If
        9130 Next I
        9140 K$ = InKey$
    9150 Loop

    9160 If K$ = Chr$(27) Then
        9170 Return
    9180 Else
        9200 For I = 0 To NP
            9210 If GO(I) = TRUE Then
                9220 If K$ = Chr$(13) Then Return
            9230 Else
                9350 Select Case K$
                    9360 Case UK$(I)
                        9365 PE = TRUE
                        9370 GoSub 12000 'DRAWSHAPE SUB
                        9380 If SA(I) = 3 Then NA = 0 Else NA = SA(I) + 1
                        9385 PS = SP(I): PA = NA
                        9390 GoSub 10000 'GETROTATEDSHAPEMAP$ SUB
                        9395 RM$ = PS$
                        9398 PX = 0: PY = 0
                        9400 GoSub 13000 'SHAPECANMOVE SUB
                        9405 If PM = TRUE Then
                            9410 SA(I) = NA
                            9420 SM$(I) = RM$
                        9430 End If
                        9435 PE = FALSE
                        9440 GoSub 12000 'DRAWSHAPE SUB

                    9450 Case LK$(I)
                        9455 PE = TRUE
                        9460 GoSub 12000 'DRAWSHAPE SUB
                        9465 PX = -1: PY = 0: PS$ = SM$(I)
                        9470 GoSub 13000 'SHAPECANMOVE SUB
                        9472 If PM = TRUE Then SX(I) = SX(I) - 1
                        9475 PE = FALSE
                        9480 GoSub 12000 'DRAWSHAPE SUB

                    9490 Case RK$(I)
                        9495 PE = TRUE
                        9500 GoSub 12000 'DRAWSHAPE SUB
                        9505 PX = 1: PY = 0: PS$ = SM$(I)
                        9510 GoSub 13000 'SHAPECANMOVE SUB
                        9512 If PM = TRUE Then SX(I) = SX(I) + 1
                        9515 PE = FALSE
                        9520 GoSub 12000 'DRAWSHAPE SUB

                    9530 Case DK$(I)
                        9540 DR!(I) = 0
                9550 End Select
            9560 End If
        9570 Next I
    9580 End If
9590 Loop
9600 Return

10000 Rem ===GETROTATEDSHAPEMAP$===
10005 Rem PARAMS: PS, PA / PS$
10010 X2 = 0
10020 Y2 = 0
10030 Select Case PS
    10040 Case 0
        10050 PS$ = "0000333300000000" 'I
    10060 Case 1
        10070 PS$ = "0000111000100000" 'L 1
    10080 Case 2
        10090 PS$ = "0000666060000000" 'L 2
    10100 Case 3
        10110 PS$ = "00000EE00EE00000" '[]
    10120 Case 4
        10130 PS$ = "00000AA0AA000000" 'S 1
    10140 Case 5
        10150 PS$ = "0000440004400000" 'S 2
    10160 Case 6
        10170 PS$ = "0000555005000000" 'T
    10180 Case Else
        10190 PS$ = ""
10200 End Select
10210 RM$ = String$(4 * 4, "0")
10220 If PA = 0 Then '0
    10230 Return
10240 Else
    10250 For X = 0 To 3
        10260 For Y = 0 To 3
            10270 Select Case PA
                10280 Case 1 '270
                    10290 X2 = Y
                    10300 Y2 = 3 - X
                10310 Case 2 '180
                    10320 X2 = 3 - X
                    10330 Y2 = 3 - Y
                10340 Case 3 '90
                    10350 X2 = 3 - Y
                    10360 Y2 = X
            10370 End Select
            10380 Mid$(RM$, ((Y2 * 4) + X2) + 1, 1) = Mid$(PS$, ((Y * 4) + X) + 1, 1)
        10390 Next Y
    10400 Next X
10500 End If
10510 PS$ = RM$
10520 Return

11000 Rem ===DRAWBLOCK===
11005 Rem PARAMS: PX, PY
11010 Color Val("&H" + BC$)
11020 Locate PY + 1, PX + PL(I)
11030 Print Chr$(219)
11040 Return

12000 Rem ===DRAWSHAPE===
12005 Rem PARAMS: PE
12010 For X = 0 To 3
    12020 For Y = 0 To 3
        12030 X2 = SX(I) + X
        12040 Y2 = SY(I) + Y
        12050 If X2 >= 0 And X2 < 10 And Y2 >= 0 And Y2 < 16 Then
            12060 If PE = TRUE Then
                12070 BC$ = Mid$(PT$(I), ((Y2 * 10) + X2) + 1, 1)
            12080 Else
                12090 BC$ = Mid$(SM$(I), ((Y * 4) + X) + 1, 1)
                12100 If BC$ = "0" Then BC$ = Mid$(PT$(I), ((Y2 * 10) + X2) + 1, 1)
            12110 End If
            12120 PX = X2: PY = Y2
            12130 GoSub 11000 'DRAWBLOCK SUB
        12140 End If
    12150 Next Y
12160 Next X
12170 Return

13000 Rem ===SHAPECANMOVE===
13010 Rem PARAMS: PS$, PX, PY / PM
13020 For Y = 0 To 3
    13030 For X = 0 To 3
        13040 If Not Mid$(PS$, ((Y * 4) + X) + 1, 1) = "0" Then
            13050 X2 = (SX(I) + X) + PX
            13060 Y2 = (SY(I) + Y) + PY
            13070 If X2 >= 0 And X2 < 10 And Y2 >= 0 And Y2 < 16 Then
                13080 If Not Mid$(PT$(I), (((Y2 * 10) + X2) + 1), 1) = "0" Then
                    13090 PM = FALSE
                    13100 Return
                13110 End If
            13120 ElseIf X2 < 0 Or X2 >= 10 Or Y2 >= 16 Then
                13130 PM = FALSE
                13140 Return
            13150 End If
        13160 End If
    13170 Next X
13180 Next Y
13190 PM = TRUE
13200 Return

14000 Rem ===DROPSHAPE===
14010 PX = 0: PY = 1: PS$ = SM$(I)
14020 GoSub 13000 'SHAPECANMOVE SUB
14030 If PM = TRUE Then
    14040 PE = TRUE
    14050 GoSub 12000 'DRAWSHAPE SUB
    14060 SY(I) = SY(I) + 1
    14070 PE = FALSE
    14080 GoSub 12000 'DRAWSHAPE SUB
Else
    '===SETTLEACTIVESHAPEINPIT===
    For Y = 0 To 3
        For X = 0 To 3
            X2 = SX(I) + X
            Y2 = SY(I) + Y
            If X2 >= 0 And X2 < 10 And Y2 >= 0 And Y2 < 16 Then
                If Not Mid$(SM$(I), ((Y * 4) + X) + 1, 1) = "0" Then
                    Mid$(PT$(I), ((Y2 * 10) + X2) + 1, 1) = Mid$(SM$(I), ((Y * 4) + X) + 1, 1)
                End If
            End If
        Next X
    Next Y
    GO(I) = (SY(I) < 0)
    '===CHECKFORFULLROWS===
    FR = FALSE
    NL = 0
    For Y = 0 To 15
        FR = TRUE
        For X = 0 To 9
            If Mid$(PT$(I), ((Y * 10) + X) + 1, 1) = "0" Then
                FR = FALSE
                Exit For
            End If
        Next X
        If FR = TRUE Then
            '===REMOVEFULLROW===
            RR = Y
            For Y2 = RR To 0 Step -1
                For X2 = 0 To 9
                    If Y2 = 0 Then
                        BC$ = "0"
                    Else
                        BC$ = Mid$(PT$(I), (((Y2 - 1) * 10) + X2) + 1, 1)
                    End If
                    Mid$(PT$(I), ((Y2 * 10) + X2) + 1, 1) = BC$
                Next X2
            Next Y2
            NL = NL + 1
        End If
    Next Y
    If NL > 0 Then
        J = I + 5
        If NL = 1 Then SC(J) = SC(J) + (100 * LV(I))
        If NL = 2 Then SC(J) = SC(J) + (300 * LV(I))
        If NL = 3 Then SC(J) = SC(J) + (500 * LV(I))
        If NL = 4 Then SC(J) = SC(J) + (800 * LV(I)) 'TETRIS!
        LN(I) = LN(I) + NL
        LV(I) = (LN(I) \ 10) + 1
    End If
    GoSub 7000 'DRAWPIT SUB
    If GO(I) = FALSE Then
        GoSub 6000 'CREATESHAPE SUB
        PE = FALSE
        GoSub 12000 'DRAWSHAPE SUB
    End If
    GoSub 8000 'DISPLAYSTATUS SUB
End If
Return

