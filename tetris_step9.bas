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
'TETRIS STEP9 AND LAST:
'   - adapt to DRAGON 32/64 BASIC

10 '***TETRIS4DRAGON***
20 ' SALVAKANTERO 2024

25 CLEAR 500
30 DIM NP, TRUE, FALSE
40 DIM NM$(6), SC(6)
50 DIM G0(1), LV(1), LN(1)
55 DIM PT$(1), DR(1), ST(1)
60 DIM SP(1), SM$(1), SA(1)
65 DIM SX(1), SY(1), PL(1)
70 DIM NS(1), NS$(1)
100 TRUE=1: FALSE=0
120 PL(0)=1: PL(1)=23
140 FOR I=0 TO 4
150 NM$(I)="DRAGON"
160 SC(I)=1400-(I*100)
170 NEXT I

200 '***MAIN***
210 GOSUB 1000 'INIT
230 FOR I=0 TO NP
240 GOSUB 2000 'CHECKSCORES
250 NEXT I
260 GOTO 200

1000 '***INIT***
1010 GOSUB 3000 'MENU
1020 CLS
1190 FOR I=0 TO NP
1200 G0(I)=FALSE
1210 LV(I)=1
1220 LN(I)=0
1230 SC(I+5)=0
1240 NS(I)=-1
1250 FOR J=0 TO 160
1255 PT$(I)=PT$(I)+"0"
1258 NEXT J
1260 GOSUB 6000 'CREATESHAPE
1270 GOSUB 7000 'DRAWPIT
1280 NEXT I
1290 GOSUB 8000 'DISPLAYSTATUS
1300 GOSUB 9000 'GAMELOOP
1310 RETURN

2000 '***CHECKSCORES***
2005 ID=I+5
2010 If SC(ID)<=SC(4) THEN 2200
2020 GOSUB 4000 'MENUHEADER
2030 PRINT @32*10+8,"GREAT SCORE PLAYER"+STR$(I+1)
2040 PRINT @32*11+8,"NAME:";
2045 INPUT NM$(ID)
2050 IF LEN(NM$(ID))>10 THEN NM$(ID)=LEFT$(NM$(ID),10)
2060 FOR J=4 TO 0 STEP -1
2070 IF SC(ID) > SC(J) THEN 2080
2075 GOTO 2140
2080 IF J<4 THEN 2090
2085 GOTO 2150
2090 SC(J+1)=SC(J)
2100 NM$(J+1)=NM$(J)
2110 GOTO 2150
2140 EXIT FOR
2150 NEXT J
2160 SC(J+1)=SC(ID)
2170 NM$(J+1)=NM$(ID)
2200 RETURN

3000 '***MENU***
3010 GOSUB 4000 'MENUHEADER
3020 PRINT @32*8+8,"1) 1 PLAYER GAME"
3030 PRINT @32*9+8,"2) 2 PLAYER GAME"
3040 PRINT @32*10+8,"3) HIGH SCORES"
3050 PRINT @32*11+8,"4) EXIT"
3060 PRINT @32*14+7,"SELECT OPTION(1-4)"
3090 K$=""
3100 K$=INKEY$
3110 IF K$="" THEN 3100
3120 IF K$="1" THEN NP=0: RETURN
3150 IF K$="2" THEN NP=1: RETURN
3180 IF K$="3" THEN 3190
3185 GOTO 3320
3190 FOR I=0 TO 4
3200 PRINT @(32*(7+I))+8,"................"
3220 PRINT @(32*(7+I))+8,NM$(I)
3240 PRINT @(32*(7+I))+20,USING "#####"; SC(I)
3260 NEXT I
3270 PRINT @32*14+4,"PRESS ANY KEY TO CONTINUE"
3285 K$=""
3290 K$=INKEY$
3300 IF K$="" THEN 3290
3310 GOTO 3000 'MENU
3320 IF K$="4" THEN CLS: END
3360 RETURN

4000 '***MENUHEADER***
4020 CLS
4030 PRINT @3+9,"***************"
4040 PRINT @32*2+9,"* T E T R I S *"
4050 PRINT @32*3+9,"***************"
4060 PRINT @32*5+8,"SALVAKANTERO 2024"
4070 RETURN

6000 '***CREATESHAPE***
6010 DR(I)=1-(LV(I)*0.2)
6020 IF DR(I)<=0 THEN DR(I)=0.1
6030 IF NS(I)>=0 THEN SP(I)=NS(I) ELSE SP(I)=RND(7)
6040 SA(I)=0
6045 PS=SP(I): PA=SA(I)
6050 GOSUB 10000 'GETROTATEDSM$
6055 SM$(I)=PS$
6060 SX(I)=3
6070 SY(I)=-4
6080 NS(I)=RND(7)
6085 PS=NS(I): PA=0
6090 GOSUB 10000 'GETROTATEDSM$
6095 NS$(I)=PS$
6100 RETURN

7000 '***DRAWPIT***
7010 For Y=0 To 15
7020 For X=0 To 9
7030 BC$=MID$(PT$(I),((10*Y)+X)+1,1)
7035 PX=X: PY=Y
7040 GOSUB 11000 'DRAWBLOCK
7050 NEXT X
7060 NEXT Y
7070 RETURN

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

11000 '***DRAWBLOCK***
11005 'PARAMS: PX, PY
11010 'Color Val("&H" + BC$)
11020 PRINT @(32*(PY+1))+PX+PL(I),Chr$(128)
11040 RETURN

12000 '***DRAWSHAPE***
12005 'PARAMS: PE
12010 FOR X=0 TO 3
12020 FOR Y=0 TO 3
12030 X2=SX(I)+X
12040 Y2=SY(I)+Y
12050 IF X2>=0 AND X2<10 AND Y2>=0 AND Y2<16 THEN 12060
12055 GOTO 12150
12060 IF PE=TRUE THEN BC$=MID$(PT$(I),((Y2*10)+X2)+1,1) ELSE 12090
12070 GOTO 12120
12090 BC$=MID$(SM$(I),((Y*4)+X)+1,1)
12100 IF BC$="0" THEN BC$=MID$(PT$(I),((Y2*10)+X2)+1,1)
12120 PX=X2: PY=Y2
12130 GOSUB 11000 'DRAWBLOCK
12150 NEXT Y
12160 NEXT X
12170 RETURN

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

