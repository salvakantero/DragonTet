DefInt A-Z

DECLARE FUNCTION GetRotatedShapeMap$ (Shape, Angle)
DECLARE FUNCTION GetShapeMap$ (Shape)
DECLARE FUNCTION RandomShapeX ()
DECLARE FUNCTION ShapeCanMove (Map$, XDirection, YDirection)
DECLARE SUB CheckForFullRows ()
DECLARE SUB SetPiece ()
DECLARE SUB DisplayStatus ()
DECLARE SUB DrawBlock (BlockColor$, PitX, PitY)
DECLARE SUB DrawPit ()
DECLARE SUB DrawShape (EraseShape)
DECLARE SUB DropShape ()
DECLARE SUB Init ()
DECLARE SUB Main ()
DECLARE SUB RemoveFullRow (RemovedRow)
DECLARE SUB SettleActiveShapeInPit ()

Const FALSE = 0
Const TRUE = -1

Const BLOCKSCALE = 24
Const LASTSIDEBLOCK = 3
Const NOBLOCK$ = "0" 'caracter vacio en el pozo
Const PITHEIGHT = 16 'alto del pozo en caracteres
Const PITLEFT = 8 'pos X para mensajes
Const PITTOP = 2 'pos Y para mensajes
Const PITWIDTH = 10 'ancho del pozo en caracteres
Const SIDEBLOCKCOUNT = 4

'DropDate!      1=cayendo 0=parada al fondo del pozo
'GameOver       0=juego en curso
'Pit$           Contenido del pozo
'Score          Puntuacion
'Shape
'ShapeAngle
'ShapeMap$
'ShapeX
'ShapeY
Common Shared DropRate!, GameOver, Pit$, Score, Shape, ShapeAngle, ShapeMap$, ShapeX, ShapeY

Init 'iniciaiza el sistema y el pozo
Main 'bucle principal

Sub CheckForFullRows ()
    FullRow = FALSE

    For PitY = 0 To PITHEIGHT - 1
        FullRow = TRUE
        For PitX = 0 To PITWIDTH - 1
            If Mid$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1) = NOBLOCK$ Then
                FullRow = FALSE
                Exit For
            End If
        Next PitX
        If FullRow Then RemoveFullRow PitY
    Next PitY
End Sub

Sub SetPiece ()
    DropRate! = 1
    Shape = Int(Rnd * 6)
    ShapeAngle = Int(Rnd * 4)
    ShapeMap$ = GetRotatedShapeMap$(Shape, ShapeAngle)
    ShapeX = RandomShapeX
    ShapeY = -SIDEBLOCKCOUNT
End Sub

Sub DisplayStatus ()
    Color 4 'primer plano rojo
    Locate Int((PITTOP * BLOCKSCALE) / 16), Int((PITLEFT * BLOCKSCALE) / 8) + 1 'arriba a la izquierda del pozo
    If GameOver Then
        Print "Game over - press Enter to play a new game."
    Else
        Print "Score:" + Str$(Score) 'pinta los puntos (lineas)
    End If
End Sub

Sub DrawBlock (BlockColor$, PitX, PitY)
    DrawX = PitX * BLOCKSCALE
    DrawY = PitY * BLOCKSCALE

    Line (DrawX + (PITLEFT * BLOCKSCALE), DrawY + (PITTOP * BLOCKSCALE))-Step(BLOCKSCALE, BLOCKSCALE), Val("&H" + BlockColor$), BF
    Line (DrawX + CInt(BLOCKSCALE / 10) + (PITLEFT * BLOCKSCALE), DrawY + CInt(BLOCKSCALE / 10) + (PITTOP * BLOCKSCALE))-Step(BLOCKSCALE - CInt(BLOCKSCALE / 5), BLOCKSCALE - CInt(BLOCKSCALE / 5)), 0, B
End Sub

Sub DrawPit ()
    Line ((PITLEFT * BLOCKSCALE) - 1, (PITTOP * BLOCKSCALE) - 1)-Step((PITWIDTH * BLOCKSCALE) + 2, (PITHEIGHT * BLOCKSCALE) + 2), 15, B
    Line ((PITLEFT * BLOCKSCALE) - 1, (PITTOP * BLOCKSCALE) - 1)-Step(PITWIDTH * BLOCKSCALE + 2, 0), 0

    For PitY = 0 To PITHEIGHT - 1
        For PitX = 0 To PITWIDTH - 1
            If GameOver Then
                BlockColor$ = "4"
            Else
                BlockColor$ = Mid$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1)
            End If

            DrawBlock BlockColor$, PitX, PitY
        Next PitX
    Next PitY
End Sub

Sub DrawShape (EraseShape)
    For BlockX = 0 To LASTSIDEBLOCK
        For BlockY = 0 To LASTSIDEBLOCK
            PitX = ShapeX + BlockX
            PitY = ShapeY + BlockY
            If PitX >= 0 And PitX < PITWIDTH And PitY >= 0 And PitY < PITHEIGHT Then
                If EraseShape Then
                    BlockColor$ = Mid$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1)
                Else
                    BlockColor$ = Mid$(ShapeMap$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1)
                    If BlockColor$ = NOBLOCK$ Then BlockColor$ = Mid$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1)
                End If
                DrawBlock BlockColor$, PitX, PitY
            End If
        Next BlockY
    Next BlockX
End Sub

Sub DropShape ()
    If ShapeCanMove(ShapeMap$, 0, 1) Then
        DrawShape TRUE
        If DropRate! > 0 Then Sound 37, .3
        ShapeY = ShapeY + 1
        DrawShape FALSE
    Else
        SettleActiveShapeInPit

        GameOver = (ShapeY < 0)

        CheckForFullRows
        DrawPit
        DisplayStatus

        If Not GameOver Then
            SetPiece
            DrawShape FALSE
        End If
    End If
End Sub

Function GetRotatedShapeMap$ (Shape, Angle)
    Map$ = GetShapeMap$(Shape)
    NewBlockX = 0
    NewBlockY = 0
    RotatedMap$ = String$(SIDEBLOCKCOUNT * SIDEBLOCKCOUNT, NOBLOCK$)

    If Angle = 0 Then
        GetRotatedShapeMap = Map$
        Exit Function
    Else
        For BlockX = 0 To LASTSIDEBLOCK
            For BlockY = 0 To LASTSIDEBLOCK
                Select Case Angle
                    Case 1
                        NewBlockX = LASTSIDEBLOCK - BlockY
                        NewBlockY = BlockX
                    Case 2
                        NewBlockX = LASTSIDEBLOCK - BlockX
                        NewBlockY = LASTSIDEBLOCK - BlockY
                    Case 3
                        NewBlockX = BlockY
                        NewBlockY = LASTSIDEBLOCK - BlockX
                End Select

                Mid$(RotatedMap$, ((SIDEBLOCKCOUNT * NewBlockY) + NewBlockX) + 1, 1) = Mid$(Map$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1)
            Next BlockY
        Next BlockX
    End If

    GetRotatedShapeMap = RotatedMap$
End Function

Function GetShapeMap$ (Shape)
    Select Case Shape
        Case 0
            Map$ = "0000333300000000"
        Case 1
            Map$ = "0000111000100000"
        Case 2
            Map$ = "0000666060000000"
        Case 3
            Map$ = "00000EE00EE00000"
        Case 4
            Map$ = "0000022022000000"
        Case 5
            Map$ = "0000555005000000"
        Case 6
            Map$ = "0000440004400000"
        Case Else
            Map$ = ""
    End Select

    GetShapeMap$ = Map$
End Function


Sub Init () 'inicializa sistema y pozo
    Randomize Timer 'semilla de aleatoriedad
    Screen 12 '640x480 16 colores
    Color 9 'primer plano azul claro
    Locate 1, 1 'primera posicion superior izquierda
    Print "*** TETRIS4DRAGON SalvaKantero 2024 ***"
    SetPiece 'genera pieza (forma, posicion)
    GameOver = FALSE 'juego en curso
    Score = 0 'puntuacion actual
    Pit$ = String$(PITWIDTH * PITHEIGHT, NOBLOCK$) 'inicializa el pozo vacio (tabla de 0)
    DrawPit 'pinta los bordes del pozo
    DisplayStatus 'pinta la puntuacion o mensajes arriba del pozo
End Sub

Sub Main () 'bucle principal
    StartTime! = Timer 'guarda el tiempo de inicio
    Do
        Key$ = ""
        Do While Key$ = "" 'mientras no se pulse una tecla
            If Not GameOver Then 'juego en curso
                If Timer >= StartTime! + DropRate! Or StartTime! > Timer Then
                    DropShape
                    StartTime! = Timer
                End If
            End If
            Key$ = InKey$
        Loop
        If Key$ = Chr$(27) Then
            Screen 0
            End
        ElseIf GameOver Then
            If Key$ = Chr$(13) Then Init
        Else
            Select Case Key$
                Case "A", "a"
                    DrawShape TRUE
                    If ShapeAngle = 3 Then NewAngle = 0 Else NewAngle = ShapeAngle + 1
                    RotatedMap$ = GetRotatedShapeMap(Shape, NewAngle)
                    If ShapeCanMove(RotatedMap$, 0, 0) Then
                        ShapeAngle = NewAngle
                        ShapeMap$ = RotatedMap$
                    End If
                    DrawShape FALSE
                Case Chr$(0) + "K"
                    DrawShape TRUE
                    If ShapeCanMove(ShapeMap$, -1, 0) Then ShapeX = ShapeX - 1
                    DrawShape FALSE
                Case Chr$(0) + "M"
                    DrawShape TRUE
                    If ShapeCanMove(ShapeMap$, 1, 0) Then ShapeX = ShapeX + 1
                    DrawShape FALSE
                Case " "
                    DropRate! = 0
            End Select
        End If
    Loop
End Sub

Function RandomShapeX ()
    IntendedShapeX = Int(Rnd * (PITWIDTH - 1))
    ShapeX = 0

    For XMove = 0 To IntendedShapeX
        If ShapeCanMove(ShapeMap$, 1, 0) Then
            ShapeX = ShapeX + 1
        Else
            Exit For
        End If
    Next XMove

    RandomShapeX = ShapeX
End Function

Sub RemoveFullRow (RemovedRow)
    For PitY = RemovedRow To 0 Step -1
        For PitX = 0 To PITWIDTH - 1
            If PitY = 0 Then
                BlockColor$ = NOBLOCK$
            Else
                BlockColor$ = Mid$(Pit$, ((PITWIDTH * (PitY - 1)) + PitX) + 1, 1)
            End If

            Mid$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1) = BlockColor$
        Next PitX
    Next PitY

    Score = Score + 1
End Sub

Sub SettleActiveShapeInPit ()
    Play "N21"

    For BlockY = 0 To LASTSIDEBLOCK
        For BlockX = 0 To LASTSIDEBLOCK
            PitX = ShapeX + BlockX
            PitY = ShapeY + BlockY
            If PitX >= 0 And PitX < PITWIDTH And PitY >= 0 And PitY < PITHEIGHT Then
                If Not Mid$(ShapeMap$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1) = NOBLOCK$ Then
                    Mid$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1) = Mid$(ShapeMap$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1)
                End If
            End If
        Next BlockX
    Next BlockY
End Sub

Function ShapeCanMove (Map$, XDirection, YDirection)
    For BlockY = 0 To LASTSIDEBLOCK
        For BlockX = 0 To LASTSIDEBLOCK
            If Not Mid$(Map$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1) = NOBLOCK$ Then
                PitX = (ShapeX + BlockX) + XDirection
                PitY = (ShapeY + BlockY) + YDirection
                If PitX >= 0 And PitX < PITWIDTH And PitY >= 0 And PitY < PITHEIGHT Then
                    If Not Mid$(Pit$, (((PITWIDTH * PitY) + PitX) + 1), 1) = NOBLOCK$ Then
                        ShapeCanMove = FALSE
                        Exit Function
                    End If
                ElseIf PitX < 0 Or PitX >= PITWIDTH Or PitY >= PITHEIGHT Then
                    ShapeCanMove = FALSE
                    Exit Function
                End If
            End If
        Next BlockX
    Next BlockY

    ShapeCanMove = TRUE
End Function

