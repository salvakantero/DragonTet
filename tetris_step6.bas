$Debug

'TETRIS.BAS:
'   - codigo original QBasic de Peter Swinkels (QBBlocks v1.0)
'TETRIS_STEP1:
'   - comprension y comentarios en el codigo
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
'   - se implementa la funcion NEXT
'TETRIS_STEP5:
'   - 2 jugadores
'   - 32x16 caracteres en pantalla
'TETRIS_STEP6:
'   - menu de inicio
'   - tabla de puntuaciones

DefInt A-Z

Const FALSE = 0
Const TRUE = -1

Const LASTSIDEBLOCK = 3 'para recorrer con bucles la pieza actual (0 a 3)
Const SIDEBLOCKCOUNT = 4 'tamano del lado de la pieza (4x4)
Const NOBLOCK$ = "0" 'caracter vacio en el foso

Const PITTOP = 1 'pos Y de la parte de arriba del foso (justo arriba de la pantalla)
Const PITWIDTH = 10 'ancho del foso
Const PITHEIGHT = 16 'alto del foso

'NumPlayers     Jugadores(0-1)
Common Shared NumPlayers

'GameOver       0=juego en curso -1=finalizado
'DropDate!      1=cayendo 0=parada al fondo del foso
'StartTime!     Tiempo desde que ha avanzado la pieza
'Pit$           Contenido de los fosos
'Level          Niveles de juego (velocidad)
'Lines          Lineas conseguidas
'Score          Puntuaciones
'PITLEFT        pos X del lado izquierdo de cada foso
Common Shared GameOver(), DropRate!(), StartTime!(), Pit$(), Level(), Lines(), Score(), PITLEFT()

'Shape          Tipo de pieza (0 a 6)
'ShapeAngle     Rotacion de la pieza (0 a 3)
'ShapeMap$      Diseno de la pieza
'ShapeX         Pos X de la pieza
'ShapeY         Pos Y de la pieza
Common Shared Shape(), ShapeAngle(), ShapeMap$(), ShapeX(), ShapeY()

'NextShape      Tipo de las siguientes piezas (0 a 6)
'ShapeMap$      Diseno de las siguientes piezas
Common Shared NextShape(), NextShapeMap$()




While TRUE
    Init 'inicializa el sistema y los fosos (y arranca el bucle principal)
Wend



Function GetShapeMap$ (Shape) 'formas de las figuras
    Select Case Shape
        Case 0
            Map$ = "0000333300000000" ' palo largo
        Case 1
            Map$ = "0000111000100000" ' L 1
        Case 2
            Map$ = "0000666060000000" ' L 2
        Case 3
            Map$ = "00000EE00EE00000" ' cubo
        Case 4
            Map$ = "0000022022000000" ' S 1
        Case 5
            Map$ = "0000440004400000" ' S 2
        Case 6
            Map$ = "0000555005000000" ' T
        Case Else
            Map$ = ""
    End Select
    GetShapeMap$ = Map$
End Function




Sub DisplayStatus ()
    Color 0, 2

    'player 1
    If GameOver(0) = TRUE Then
        Locate 8, PITLEFT(0)
        Print "GAME OVER!"
    End If
    Locate 2, 12: Print "=PLAYER 1="
    Locate 3, 12: Print "Level:" + Str$(Level(0)) 'pinta el num. de nivel
    Locate 4, 12: Print "Lines:" + Str$(Lines(0)) 'pinta las lineas
    Locate 5, 12: Print "Sc:" + Str$(Score(0)) 'pinta la puntuacion
    Locate 6, 12: Print "Next:"

    'player 2
    If NumPlayers = 1 Then
        If GameOver(1) = TRUE Then
            Locate 8, PITLEFT(1)
            Print "GAME OVER!"
        End If
        Locate 10, 12: Print "=PLAYER 2="
        Locate 11, 12: Print "Level:" + Str$(Level(1)) 'pinta el num. de nivel
        Locate 12, 12: Print "Lines:" + Str$(Lines(1)) 'pinta las lineas
        Locate 13, 12: Print "Sc:" + Str$(Score(1)) 'pinta la puntuacion
        Locate 14, 12: Print "Next:"
    End If

    For i = 0 To NumPlayers
        If GameOver(i) = FALSE Then DrawNextShape i
    Next i
End Sub




Sub DrawBlock (BlockColor$, PitX, PitY, i) 'pinta bloque de pieza
    Color Val("&H" + BlockColor$) 'convierte el color de hexadecimal a int
    Locate PitY + PITTOP + 1, PitX + PITLEFT(i)
    Print Chr$(219) 'bloque relleno
End Sub




Sub DrawPit (i)
    'repinta el contenido del foso
    For PitY = 0 To PITHEIGHT - 1
        For PitX = 0 To PITWIDTH - 1
            BlockColor$ = Mid$(Pit$(i), ((PITWIDTH * PitY) + PitX) + 1, 1)
            DrawBlock BlockColor$, PitX, PitY, i
        Next PitX
    Next PitY
End Sub




'chequea si puede moverse a XDirection/YDirection
Function ShapeCanMove (Map$, XDirection, YDirection, i)
    'para todos los bloques de la pieza
    For BlockY = 0 To LASTSIDEBLOCK
        For BlockX = 0 To LASTSIDEBLOCK
            'si no es un bloque vacio
            If Not Mid$(Map$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1) = NOBLOCK$ Then
                'si el bloque esta dentro de los limites del foso
                PitX = (ShapeX(i) + BlockX) + XDirection
                PitY = (ShapeY(i) + BlockY) + YDirection
                If PitX >= 0 And PitX < PITWIDTH And PitY >= 0 And PitY < PITHEIGHT Then
                    'el lugar esta ocupado por una pieza
                    If Not Mid$(Pit$(i), (((PITWIDTH * PitY) + PitX) + 1), 1) = NOBLOCK$ Then
                        ShapeCanMove = FALSE 'no puede moverse
                        Exit Function
                    End If
                    'el bloque queda fuera de los limites del foso
                ElseIf PitX < 0 Or PitX >= PITWIDTH Or PitY >= PITHEIGHT Then
                    ShapeCanMove = FALSE 'no puede moverse
                    Exit Function
                End If
            End If
        Next BlockX
    Next BlockY
    ShapeCanMove = TRUE
End Function




Function GetRotatedShapeMap$ (Shape, Angle)
    NewBlockX = 0
    NewBlockY = 0
    Map$ = GetShapeMap$(Shape) 'pieza sin rotar
    RotatedMap$ = String$(SIDEBLOCKCOUNT * SIDEBLOCKCOUNT, NOBLOCK$) 'nueva pieza rotada

    If Angle = 0 Then 'vuelve a la posicion inicial
        GetRotatedShapeMap = Map$
        Exit Function
    Else
        'para todos los bloques de la pieza
        For BlockX = 0 To LASTSIDEBLOCK
            For BlockY = 0 To LASTSIDEBLOCK
                Select Case Angle
                    Case 1 '270' grados
                        NewBlockX = BlockY
                        NewBlockY = LASTSIDEBLOCK - BlockX
                    Case 2 '180 grados
                        NewBlockX = LASTSIDEBLOCK - BlockX
                        NewBlockY = LASTSIDEBLOCK - BlockY
                    Case 3 '90 grados
                        NewBlockX = LASTSIDEBLOCK - BlockY
                        NewBlockY = BlockX
                End Select
                'asigna a la pieza rotada el bloque correspondiente al angulo
                Mid$(RotatedMap$, ((SIDEBLOCKCOUNT * NewBlockY) + NewBlockX) + 1, 1) = Mid$(Map$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1)
            Next BlockY
        Next BlockX
    End If
    GetRotatedShapeMap = RotatedMap$
End Function




Sub CreateNextShape (i)
    NextShape(i) = Int(Rnd * 7) 'tipo de pieza. x7
    NextShapeMap$(i) = GetRotatedShapeMap$(NextShape(i), 0) 'composicion de la pieza
End Sub




Sub DrawNextShape (i)
    'para todos los bloques de la pieza
    For BlockX = 0 To LASTSIDEBLOCK
        For BlockY = 0 To LASTSIDEBLOCK
            'color de la pieza
            BlockColor$ = Mid$(NextShapeMap$(i), ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1)
            If BlockColor$ = NOBLOCK$ Then BlockColor$ = "2"
            If i = 0 Then
                DrawBlock BlockColor$, BlockX + 17, BlockY + 4, i 'pinta el bloque
            Else
                DrawBlock BlockColor$, BlockX - 5, BlockY + 12, i 'pinta el bloque
            End If
        Next BlockY
    Next BlockX
End Sub




Sub CreateShape (i)
    DropRate!(i) = 1 - (Level(i) * 0.2) 'tiempo de caida variable segun el nivel
    If DropRate!(i) <= 0 Then DropRate!(i) = .1 'limite de caida de .1
    'si no es la primera pieza la toma de NextShape
    'si es la primera la genera (0 a 6)
    If NextShape(i) >= 0 Then Shape(i) = NextShape(i) Else Shape(i) = Int(Rnd * 7)
    ShapeAngle(i) = 0
    ShapeMap$(i) = GetRotatedShapeMap$(Shape(i), ShapeAngle(i)) 'composicion de la pieza
    ShapeX(i) = 3 'posicion X inicial (centro del foso)
    ShapeY(i) = -SIDEBLOCKCOUNT 'posicion Y inicial (totalmente oculta)
    CreateNextShape (i)
End Sub




Sub DrawShape (i, EraseShape)
    'para todos los bloques de la pieza
    For BlockX = 0 To LASTSIDEBLOCK
        For BlockY = 0 To LASTSIDEBLOCK
            PitX = ShapeX(i) + BlockX
            PitY = ShapeY(i) + BlockY
            'si el bloque esta dentro del foso
            If PitX >= 0 And PitX < PITWIDTH And PitY >= 0 And PitY < PITHEIGHT Then
                If EraseShape Then 'hay que borrar la pieza
                    BlockColor$ = Mid$(Pit$(i), ((PITWIDTH * PitY) + PitX) + 1, 1) 'color del foso
                Else
                    'color de la pieza
                    BlockColor$ = Mid$(ShapeMap$(i), ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1)
                    'color del foso
                    If BlockColor$ = NOBLOCK$ Then BlockColor$ = Mid$(Pit$(i), ((PITWIDTH * PitY) + PitX) + 1, 1)
                End If
                DrawBlock BlockColor$, PitX, PitY, i 'pinta el bloque
            End If
        Next BlockY
    Next BlockX
End Sub




'fija la pieza al foso si ya no se puede mover
Sub SettleActiveShapeInPit (i)
    For BlockY = 0 To LASTSIDEBLOCK 'para cada pos Y del bloque de la pieza
        For BlockX = 0 To LASTSIDEBLOCK 'para cada pos X del bloque de la pieza
            PitX = ShapeX(i) + BlockX
            PitY = ShapeY(i) + BlockY
            'si el bloque de la pieza esta en los limites del foso
            If PitX >= 0 And PitX < PITWIDTH And PitY >= 0 And PitY < PITHEIGHT Then
                'si el bloque no es un hueco vacio
                If Not Mid$(ShapeMap$(i), ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1) = NOBLOCK$ Then
                    'fija el bloque de la pieza en el foso
                    Mid$(Pit$(i), ((PITWIDTH * PitY) + PitX) + 1, 1) = Mid$(ShapeMap$(i), ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1)
                End If
            End If
        Next BlockX
    Next BlockY
End Sub




Sub DropShape (i) 'cae un caracter la pieza
    If ShapeCanMove(ShapeMap$(i), 0, 1, i) Then 'si se puede mover
        DrawShape i, TRUE 'borra la pieza
        ShapeY(i) = ShapeY(i) + 1 'baja un caracter de altura
        DrawShape i, FALSE 'pinta la pieza
    Else
        SettleActiveShapeInPit i 'mantiene la pieza inmovil
        GameOver(i) = (ShapeY(i) < 0) 'si llega arriba pierde partida
        CheckForFullRows (i) 'busca lineas completadas
        DrawPit (i)
        If Not GameOver(i) Then
            CreateShape (i) 'genera una nueva pieza
            DrawShape i, FALSE 'pinta la pieza
        End If
        DisplayStatus 'muestra puntos o mensajes
    End If
End Sub




'baja todas las lineas 1 unidad hasta la linea completada
Sub RemoveFullRow (RemovedRow, i)
    For PitY = RemovedRow To 0 Step -1 'para cada linea del foso desde RemovedRow
        For PitX = 0 To PITWIDTH - 1 'para cada caracter de la linea
            If PitY = 0 Then
                BlockColor$ = NOBLOCK$ 'la linea mas alta queda vacia
            Else 'copia la linea superior a la actual
                BlockColor$ = Mid$(Pit$(i), ((PITWIDTH * (PitY - 1)) + PitX) + 1, 1)
            End If
            'aplica a la linea actual la que tenia encima
            Mid$(Pit$(i), ((PITWIDTH * PitY) + PitX) + 1, 1) = BlockColor$
        Next PitX
    Next PitY
End Sub




Sub CheckForFullRows (i) 'busca filas completas
    FullRow = FALSE
    NumLines = 0
    For PitY = 0 To PITHEIGHT - 1 'para todas las filas del foso
        FullRow = TRUE
        For PitX = 0 To PITWIDTH - 1 'para todos los caracteres de la linea
            If Mid$(Pit$(i), ((PITWIDTH * PitY) + PitX) + 1, 1) = NOBLOCK$ Then
                FullRow = FALSE 'hay al menos un hueco
                Exit For
            End If
        Next PitX
        If FullRow Then
            RemoveFullRow PitY, i 'fila completa, la borra
            NumLines = NumLines + 1
        End If
    Next PitY
    If NumLines > 0 Then
        'puntuacion segun el numero de lineas conseguidas
        If NumLines = 1 Then Score(i) = Score(i) + (100 * Level(i))
        If NumLines = 2 Then Score(i) = Score(i) + (300 * Level(i))
        If NumLines = 3 Then Score(i) = Score(i) + (500 * Level(i))
        If NumLines = 4 Then Score(i) = Score(i) + (800 * Level(i)) 'tetris!
        'actualiza el total de lineas completadas
        Lines(i) = Lines(i) + NumLines
        'calcula el nivel basado en el total de lineas completadas
        Level(i) = (Lines(i) \ 10) + 1
    End If
End Sub




Sub Intro ()
    Color 0, 2
    Cls
    Locate 6, 10: Print "***************"
    Locate 7, 10: Print "* T E T R I S *"
    Locate 8, 10: Print "***************"
    Locate 10, 9: Print "SALVAKANTERO 2024"
    NumPlayers = 0
    While NumPlayers < 1 Or NumPlayers > 2
        Locate 13, 5
        Input "NUMBER OF PLAYERS (1-2)? ", NumPlayers
    Wend
End Sub




Sub Init () 'inicializa sistema y foso
    Randomize Timer 'semilla de aleatoriedad
    Screen 13 '320x200 256 colores MCGA
    Intro
    Cls
    NumPlayers = NumPlayers - 1
    ReDim GameOver(NumPlayers)
    ReDim Level(NumPlayers)
    ReDim Lines(NumPlayers)
    ReDim Score(NumPlayers)
    ReDim Pit$(NumPlayers)
    ReDim DropRate!(NumPlayers)
    ReDim StartTime!(NumPlayers)
    ReDim Shape(NumPlayers)
    ReDim ShapeMap$(NumPlayers)
    ReDim ShapeAngle(NumPlayers)
    ReDim ShapeX(NumPlayers)
    ReDim ShapeY(NumPlayers)
    ReDim NextShape(NumPlayers)
    ReDim NextShapeMap$(NumPlayers)

    ReDim PITLEFT(1)
    PITLEFT(0) = 1
    PITLEFT(1) = 23

    For i = 0 To NumPlayers
        GameOver(i) = FALSE 'partida en curso
        Level(i) = 1 'nivel inicial
        Lines(i) = 0 'lineas conseguidas
        Score(i) = 0 'puntuacion actual
        NextShape(i) = -1 'sera necesario generar pieza
        Pit$(i) = String$(PITWIDTH * PITHEIGHT, NOBLOCK$) 'inicializa el foso vacio (tabla de 0)
        CreateShape (i) 'genera pieza (forma, posicion)
        DrawPit (i) 'pinta foso
    Next i

    DisplayStatus 'pinta el marcador comun
    MainLoop
End Sub




Sub MainLoop () 'bucle principal
    StartTime!(0) = Timer 'guarda el tiempo de inicio para jugador 0
    If NumPlayers > 0 Then
        StartTime!(1) = Timer 'guarda el tiempo de inicio para jugador 1
    End If

    Do
        Key$ = ""
        Do While Key$ = "" 'mientras no se pulse una tecla
            For i = 0 To NumPlayers 'bucle para ambos jugadores
                If Not GameOver(i) Then 'juego en curso
                    'Si ha sido superado el tiempo de caida
                    If Timer >= StartTime!(i) + DropRate!(i) Or StartTime!(i) > Timer Then
                        DropShape i 'la pieza avanza hacia abajo
                        StartTime!(i) = Timer 'reinicia el temporizador de caida
                    End If
                End If
            Next i
            Key$ = InKey$ 'lee pulsaciones del teclado
        Loop
        If Key$ = Chr$(27) Then 'si se pulsa ESC sale
            Screen 0
            End
        Else
            For i = 0 To NumPlayers 'bucle para ambos jugadores
                If GameOver(i) = TRUE Then 'fuera de juego
                    If Key$ = Chr$(13) Then
                        Exit Sub 'si se pulsa ENTER reinicia juego
                    End If
                Else
                    ' Asignar teclas para cada jugador
                    If i = 0 Then
                        RotateKey$ = "w"
                        LeftKey$ = "a"
                        DownKey$ = "s"
                        RightKey$ = "d"
                    Else
                        RotateKey$ = "i"
                        LeftKey$ = "j"
                        DownKey$ = "k"
                        RightKey$ = "l"
                    End If
                    Select Case Key$
                        Case RotateKey$ 'al pulsar la tecla de rotaci�n, gira la pieza
                            DrawShape i, TRUE 'borra pieza
                            'cambia el �ngulo de la pieza
                            If ShapeAngle(i) = 3 Then NewAngle = 0 Else NewAngle = ShapeAngle(i) + 1
                            'modifica la pieza
                            RotatedMap$ = GetRotatedShapeMap(Shape(i), NewAngle)
                            If ShapeCanMove(RotatedMap$, 0, 0, i) Then 'si se puede mover...
                                ShapeAngle(i) = NewAngle
                                ShapeMap$(i) = RotatedMap$
                            End If
                            DrawShape i, FALSE 'pinta pieza

                        Case LeftKey$ 'al pulsar la tecla izquierda, mueve a la izquierda si puede
                            DrawShape i, TRUE 'borra pieza
                            If ShapeCanMove(ShapeMap$(i), -1, 0, i) Then ShapeX(i) = ShapeX(i) - 1
                            DrawShape i, FALSE 'pinta pieza

                        Case RightKey$ 'al pulsar la tecla derecha, mueve a la derecha si puede
                            DrawShape i, TRUE 'borra pieza
                            If ShapeCanMove(ShapeMap$(i), 1, 0, i) Then ShapeX(i) = ShapeX(i) + 1
                            DrawShape i, FALSE 'pinta pieza

                        Case DownKey$ 'al pulsar la tecla abajo, pone el tiempo de descenso a 0
                            DropRate!(i) = 0
                    End Select
                End If
            Next i
        End If
    Loop
End Sub

