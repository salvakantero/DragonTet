DefInt A-Z

DECLARE FUNCTION GetRotatedShapeMap$ (Shape, Angle)
DECLARE FUNCTION GetShapeMap$ (Shape)
DECLARE FUNCTION RandomShapeX ()
DECLARE FUNCTION ShapeCanMove (Map$, XDirection, YDirection)
DECLARE SUB CheckForFullRows ()
DECLARE SUB SetPiece ()
DECLARE SUB DisplayStatus ()
DECLARE SUB DrawBlock (BlockColor$, PitX, PitY)
DECLARE SUB DrawShape (EraseShape)
DECLARE SUB DropShape ()
DECLARE SUB Init ()
DECLARE SUB Main ()
DECLARE SUB RemoveFullRow (RemovedRow)
DECLARE SUB SettleActiveShapeInPit ()

Const FALSE = 0
Const TRUE = -1

Const BLOCKSCALE = 24 'pixeles de cada bloque (para pasar coordenadas de caracter a pixeles)
Const LASTSIDEBLOCK = 3 'para recorrer con bucles la pieza
Const SIDEBLOCKCOUNT = 4 'tamano del lado de la pieza en caracteres
Const NOBLOCK$ = "0" 'caracter vacio en el pozo

Const PITLEFT = 8 'pos X en caracteres del lado izquierdo del pozo
Const PITTOP = 2 'pos Y en caracteres de la parte de arriba del pozo
Const PITWIDTH = 10 'ancho del pozo en caracteres
Const PITHEIGHT = 16 'alto del pozo en caracteres

'DropDate!      1=cayendo 0=parada al fondo del pozo
'GameOver       0=juego en curso 1=finalizado
'Pit$           Contenido del pozo
'Score          Puntuacion
'Shape          Tipo de pieza
'ShapeAngle     Rotacion de la pieza
'ShapeMap$      Diseno de la pieza
'ShapeX         Pos X de la pieza
'ShapeY         Pos Y de la pieza
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
    GameOver = FALSE 'comienza partida
    Score = 0 'puntuacion actual
    Pit$ = String$(PITWIDTH * PITHEIGHT, NOBLOCK$) 'inicializa el pozo vacio (tabla de 0)
    DisplayStatus 'pinta la puntuacion o mensajes arriba del pozo
End Sub

Sub Main () 'bucle principal
    StartTime! = Timer 'guarda el tiempo de inicio
    Do
        Key$ = ""
        Do While Key$ = "" 'mientras no se pulse una tecla
            If Not GameOver Then 'juego en curso
                'Si ha sido superado el tiempo de caida
                If Timer >= StartTime! + DropRate! Or StartTime! > Timer Then
                    DropShape 'la pieza avanza hacia abajo
                    StartTime! = Timer 'reinicia el temporizador
                End If
            End If
            Key$ = InKey$ 'lee pulsaciones del teclado
        Loop
        If Key$ = Chr$(27) Then 'si se pulsa ESC
            Screen 0
            End
        ElseIf GameOver Then 'fuera de juego
            If Key$ = Chr$(13) Then Init 'si se pulsa ENTER inicia juego
        Else
            'durante el juego
            Select Case Key$
                Case "A", "a" 'al pulsar A rota la pieza
                    DrawShape TRUE 'borra pieza
                    'cambia el angulo de la pieza
                    If ShapeAngle = 3 Then NewAngle = 0 Else NewAngle = ShapeAngle + 1
                    'modifica la pieza
                    RotatedMap$ = GetRotatedShapeMap(Shape, NewAngle)
                    If ShapeCanMove(RotatedMap$, 0, 0) Then 'si se puede mover...
                        ShapeAngle = NewAngle
                        ShapeMap$ = RotatedMap$
                    End If
                    DrawShape FALSE 'pinta pieza
                Case Chr$(0) + "K" 'al pulsar CURSOR IZDA. mueve a la izquierda si puede
                    DrawShape TRUE 'borra pieza
                    If ShapeCanMove(ShapeMap$, -1, 0) Then ShapeX = ShapeX - 1
                    DrawShape FALSE 'pinta pieza
                Case Chr$(0) + "M" 'al pulsar CURSOR DCHA. mueve a la derecha si puede
                    DrawShape TRUE 'borra pieza
                    If ShapeCanMove(ShapeMap$, 1, 0) Then ShapeX = ShapeX + 1
                    DrawShape FALSE 'pinta pieza
                Case " " 'al pulsar SPACE pone el tiempo de descenso a 0
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

