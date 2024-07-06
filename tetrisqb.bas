DefInt A-Z

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




Sub CheckForFullRows () 'busca filas completas
    FullRow = FALSE
    For PitY = 0 To PITHEIGHT - 1 'para todas las filas del pozo
        FullRow = TRUE
        For PitX = 0 To PITWIDTH - 1 'para todos los caracteres de la linea
            If Mid$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1) = NOBLOCK$ Then
                FullRow = FALSE 'hay al menos un hueco
                Exit For
            End If
        Next PitX
        If FullRow Then RemoveFullRow PitY 'fila completa, la borra
    Next PitY
End Sub




Sub SetPiece ()
    DropRate! = 1 'tiempo de caida
    Shape = Int(Rnd * 7) 'tipo de pieza. x7

    ShapeAngle = Int(Rnd * 4) 'angulo de rotacion de la pieza. x4
    ShapeMap$ = GetRotatedShapeMap$(Shape, ShapeAngle) 'composicion de la pieza
    ShapeX = RandomShapeX 'posicion X (aleatoria)
    ShapeY = -SIDEBLOCKCOUNT 'posicion Y (totalmente oculta)
End Sub




Sub DisplayStatus ()
    Color 4 'primer plano rojo
    Locate 2, 1
    If GameOver Then
        Print "Game over - press Enter to play a new game."
    Else
        Print "Score:" + Str$(Score) 'pinta los puntos (lineas)
    End If
End Sub




Sub DrawBlock (BlockColor$, PitX, PitY) 'pinta bloque de pieza
    'coordenadas en pixeles
    DrawX = PitX * BLOCKSCALE
    DrawY = PitY * BLOCKSCALE
    'pinta el bloque
    Line (DrawX + (PITLEFT * BLOCKSCALE), DrawY + (PITTOP * BLOCKSCALE))-Step(BLOCKSCALE, BLOCKSCALE), Val("&H" + BlockColor$), BF
    Line (DrawX + CInt(BLOCKSCALE / 10) + (PITLEFT * BLOCKSCALE), DrawY + CInt(BLOCKSCALE / 10) + (PITTOP * BLOCKSCALE))-Step(BLOCKSCALE - CInt(BLOCKSCALE / 5), BLOCKSCALE - CInt(BLOCKSCALE / 5)), 0, B
End Sub




Sub DrawShape (EraseShape)
    'para todos los bloques de la pieza
    For BlockX = 0 To LASTSIDEBLOCK
        For BlockY = 0 To LASTSIDEBLOCK
            PitX = ShapeX + BlockX
            PitY = ShapeY + BlockY
            'si el bloque esta dentro del pozo
            If PitX >= 0 And PitX < PITWIDTH And PitY >= 0 And PitY < PITHEIGHT Then
                If EraseShape Then 'hay que borrar la pieza
                    BlockColor$ = Mid$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1) 'color del pozo
                Else
                    'color de la pieza
                    BlockColor$ = Mid$(ShapeMap$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1)
                    'color del pozo
                    If BlockColor$ = NOBLOCK$ Then BlockColor$ = Mid$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1)
                End If
                DrawBlock BlockColor$, PitX, PitY 'pinta el bloque
            End If
        Next BlockY
    Next BlockX
End Sub




Sub DropShape () 'cae un caracter la pieza
    If ShapeCanMove(ShapeMap$, 0, 1) Then 'si se puede mover
        DrawShape TRUE 'borra la pieza
        'If DropRate! > 0 Then Sound 37, .3
        ShapeY = ShapeY + 1 'baja un caracter de altura
        DrawShape FALSE 'pinta la pieza
    Else
        SettleActiveShapeInPit 'mantiene la pieza inmovil

        GameOver = (ShapeY < 0) 'si llega arriba pierde partida

        CheckForFullRows 'busca lineas completadas
        DisplayStatus 'muestra puntos o mensajes

        If Not GameOver Then
            SetPiece 'genera una nueva pieza
            DrawShape FALSE 'pinta la pieza
        End If
    End If
End Sub




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
                    Case 1 '90 grados
                        NewBlockX = LASTSIDEBLOCK - BlockY
                        NewBlockY = BlockX
                    Case 2 '180 grados
                        NewBlockX = LASTSIDEBLOCK - BlockX
                        NewBlockY = LASTSIDEBLOCK - BlockY
                    Case 3 '27' grados
                        NewBlockX = BlockY
                        NewBlockY = LASTSIDEBLOCK - BlockX
                End Select
                'asigna a la pieza rotada el bloque correspondiente al angulo
                Mid$(RotatedMap$, ((SIDEBLOCKCOUNT * NewBlockY) + NewBlockX) + 1, 1) = Mid$(Map$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1)
            Next BlockY
        Next BlockX
    End If
    GetRotatedShapeMap = RotatedMap$
End Function




Function GetShapeMap$ (Shape) 'formas de las figuras
    Select Case Shape
        Case 0
            Map$ = "0000333300000000" ' = palo largo
        Case 1
            Map$ = "0000111000100000" ' L 1
        Case 2
            Map$ = "0000666060000000" ' L 2
        Case 3
            Map$ = "00000EE00EE00000" ' # cubo
        Case 4
            Map$ = "0000022022000000" ' S 1
        Case 5
            Map$ = "0000555005000000" ' T
        Case 6
            Map$ = "0000440004400000" ' S 2
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




Function RandomShapeX () 'genera una posicion X aleatoria dentro del pozo
    IntendedShapeX = Int(Rnd * (PITWIDTH - 1)) 'posicion X deseada
    ShapeX = 0 'posicion X actual
    'chequea desde 0 hasta la posicion X deseada si puede moverse
    For XMove = 0 To IntendedShapeX
        If ShapeCanMove(ShapeMap$, 1, 0) Then
            ShapeX = ShapeX + 1
        Else
            Exit For
        End If
    Next XMove

    RandomShapeX = ShapeX 'posicion X final
End Function



'baja todas las lineas 1 unidad hasta la linea completada
Sub RemoveFullRow (RemovedRow)
    For PitY = RemovedRow To 0 Step -1 'para cada linea del pozo desde RemovedRow
        For PitX = 0 To PITWIDTH - 1 'para cada caracter de la linea
            If PitY = 0 Then
                BlockColor$ = NOBLOCK$ 'la linea mas alta queda vacia
            Else 'copia la linea superior a la actual
                BlockColor$ = Mid$(Pit$, ((PITWIDTH * (PitY - 1)) + PitX) + 1, 1)
            End If
            'aplica a la linea actual la que tenia encima
            Mid$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1) = BlockColor$
        Next PitX
    Next PitY
    Score = Score + 1 'aumenta la puntuacion
End Sub



'fija la pieza al pozo si ya no se puede mover
Sub SettleActiveShapeInPit ()
    For BlockY = 0 To LASTSIDEBLOCK 'para cada pos Y del bloque de la pieza
        For BlockX = 0 To LASTSIDEBLOCK 'para cada pos X del bloque de la pieza
            PitX = ShapeX + BlockX
            PitY = ShapeY + BlockY
            'si el bloque de la pieza esta en los limites del pozo
            If PitX >= 0 And PitX < PITWIDTH And PitY >= 0 And PitY < PITHEIGHT Then
                'si el bloque no es un hueco vacio
                If Not Mid$(ShapeMap$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1) = NOBLOCK$ Then
                    'fija el bloque de la pieza en el pozo
                    Mid$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1) = Mid$(ShapeMap$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1)
                End If
            End If
        Next BlockX
    Next BlockY
End Sub



'chequea si puede moverse a XDirection/YDirection
Function ShapeCanMove (Map$, XDirection, YDirection)
    'para todos los bloques de la pieza
    For BlockY = 0 To LASTSIDEBLOCK
        For BlockX = 0 To LASTSIDEBLOCK
            'si no es un bloque vacio
            If Not Mid$(Map$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1) = NOBLOCK$ Then
                'si el bloque esta dentro de los limites del pozo
                PitX = (ShapeX + BlockX) + XDirection
                PitY = (ShapeY + BlockY) + YDirection
                If PitX >= 0 And PitX < PITWIDTH And PitY >= 0 And PitY < PITHEIGHT Then
                    'el lugar esta ocupado por una pieza
                    If Not Mid$(Pit$, (((PITWIDTH * PitY) + PitX) + 1), 1) = NOBLOCK$ Then
                        ShapeCanMove = FALSE 'no puede moverse
                        Exit Function
                    End If
                    'el bloque queda fuera de los limites del pozo
                ElseIf PitX < 0 Or PitX >= PITWIDTH Or PitY >= PITHEIGHT Then
                    ShapeCanMove = FALSE 'no puede moverse
                    Exit Function
                End If
            End If
        Next BlockX
    Next BlockY
    ShapeCanMove = TRUE
End Function

