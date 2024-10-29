
#include <cmoc.h>
#include <coco.h>

#define SCREEN_WIDTH 32 // ancho de la pantalla en modo texto
#define SCREEN_BASE_ADDR 0x0400 // dirección base de la memoria de video en modo texto

#define LASTSIDEBLOCK 3 // para recorrer con bucles la pieza actual (0 a 3)
#define SIDEBLOCKCOUNT 4 // tamano del lado de la pieza (4x4)
#define NOBLOCK '0' // carácter que representa un bloque vacío
#define PITTOP 0 // pos Y de la parte de arriba del foso (justo arriba de la pantalla)
#define PITWIDTH 10 // ancho del foso
#define PITHEIGHT 16 // alto del foso

/*
unsigned char:	byte
signed char:	char/sbyte
unsigned int:	word
signed int:		int/sword
*/

char key;
unsigned char numPlayers;
unsigned char gameOver[2];
unsigned char level[2];
unsigned int lines[2];
unsigned char pitLeft[2];
char pit[2][PITWIDTH * PITHEIGHT];
int nextShape[2];
char names[7][11] = {"DRAGON","DRAGON","DRAGON","DRAGON","DRAGON","",""};
unsigned int scores[7] = {1400, 1300, 1200, 1100, 1000, 0, 0};

void drawNextShape(unsigned char player) {
/*
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
    Next BlockX */
}

void displayStatus(void) {
	unsigned char i;
    // player 1
    if (gameOver[0] == TRUE) {
        locate(pitLeft[0], 8);
        printf("GAME OVER!");
    }
    locate(12, 1); printf("=PLAYER 1=");
    locate(12, 2); printf("LEVEL: %u", level[0]); // pinta el num. de nivel
    locate(12, 3); printf("LINES: %u", lines[0]); // pinta las lineas
    locate(12, 4); printf("SC: %d", scores[5]); // pinta la puntuacion
    locate(12, 5); printf("NEXT:");
    // player 2
    if (numPlayers == 2) {
        if (gameOver[1] == TRUE) {
			locate(pitLeft[1], 8);
			printf("GAME OVER!");
        }
        locate(12, 9); printf("=PLAYER 2=");
        locate(12, 10); printf("LEVEL: %u", level[1]); // pinta el num. de nivel
        locate(12, 11); printf("LINES: %u", lines[1]); // pinta las lineas
        locate(12, 12); printf("SC: %d", scores[6]); // pinta la puntuacion
        locate(12, 13); printf("NEXT:");
    }
    for (i=0; i<numPlayers; i++)
        if (gameOver[i] == FALSE)
			drawNextShape(i);

	waitkey(0);
}

void drawBlock(char blockColor, word pitX, word pitY, byte player) { // pinta bloque de pieza
    /*
	int color = strtol(&blockColor, NULL, 16);
    locate(pitX + pitLeft(player), pitY + 1);
    printf("%c", 219); */
}

void drawPit(unsigned char player) {
	/*
    // repinta el contenido del foso
	for (word pitY = 0; pitY < PITHEIGHT; pitY++) {
        for (word pitX = 0; pitX < PITWIDTH; pitX++) {
            blockColor = pit[player][(PITWIDTH * pitY) + pitX];
            drawBlock(blockColor, pitX, pitY, player);
        }
    }*/
}

void createShape(unsigned char player) {
	/*
    DropRate!(i) = 1 - (Level(i) * 0.2) 'tiempo de caida variable segun el nivel
    If DropRate!(i) <= 0 Then DropRate!(i) = .1 'limite de caida de .1
    'si no es la primera pieza la toma de NextShape
    'si es la primera la genera (0 a 6)
    If NextShape(i) >= 0 Then Shape(i) = NextShape(i) Else Shape(i) = Int(Rnd * 7)
    ShapeAngle(i) = 0
    ShapeMap$(i) = GetRotatedShapeMap$(Shape(i), ShapeAngle(i)) 'composicion de la pieza
    ShapeX(i) = 3 'posicion X inicial (centro del foso)
    ShapeY(i) = -SIDEBLOCKCOUNT 'posicion Y inicial (totalmente oculta)
    CreateNextShape (i) */
}

void drawHighScores(void) {
	unsigned char i;
    for(i = 0; i < 5; i++) {
        locate(7, 8 + i);  printf("...............");
        locate(7, 8 + i);  printf("%s", names[i]);
        locate(19, 8 + i); printf("%5d", scores[i]);
	}
	locate(2,14); printf("PRESS ANY KEY TO CONTINUE...");
	waitkey(0);
}

void drawHeader(void) {
    locate(8, 1); printf("***************");
    locate(8, 2); printf("* T E T R I S *");
    locate(8, 3); printf("***************");
    locate(7, 5); printf("SALVAKANTERO 2024");
}

void drawMenu(void) {
	cls(1);
	drawHeader();
	locate(7, 8);  printf("1)  1 PLAYER GAME");
    locate(7, 9);  printf("2)  2 PLAYER GAME");
    locate(7, 10); printf("3)  HIGH SCORES");
    locate(7, 11); printf("4)  EXIT");
    locate(6, 14); printf("SELECT OPTION (1-4)");	
}

void menu(void) {
	drawMenu();
    do {
        key = inkey();		
        if (key == '1')	{
            numPlayers = 1;
            break;
        } 
		else if (key == '2') {
            numPlayers = 2;
            break;
        } 
		else if (key == '3') {			
            drawHighScores();
			drawMenu();
        } 
		else if (key == '4') {
			cls(1);
            exit(0);
		}
    } while (TRUE);
}

// lógica para inicializar el sistema y los fosos
void init(void) {
	unsigned char i;
    menu();
	cls(1);
    pitLeft[0] = 1;
    pitLeft[1] = 23;
	for(i=0; i<numPlayers; i++) {
        gameOver[i] = FALSE; // partida en curso
        level[i] = 1; // nivel inicial
        lines[i] = 0; // lineas conseguidas
        scores[i+5] = 0; // puntuacion actual
        nextShape[i] = -1; // sera necesario generar pieza
        memset(pit[i], NOBLOCK, PITWIDTH * PITHEIGHT); // inicializa el foso vacío
        createShape(i); // genera pieza (forma, posición)
        drawPit(i); //pinta foso
    }
    displayStatus();
}

void mainLoop(void) { // bucle principal
/*
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
            Exit Sub
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
    Loop */
}

// verifica si la nueva puntuación es lo suficientemente alta para entrar en el top 5
void checkScores(unsigned char player) {
    unsigned char i = player + 5;
	unsigned char j;

    if (scores[i] > scores[4]) {
        drawHeader();
        locate(3, 10); printf("BUENA PUNTUACION JUGADOR %d", player+1);
        locate(6, 11); printf("NOMBRE?: ");
        char *response = readline();
        strncpy(names[i], response, 10);
        names[i][10] = '\0';

        for (j=4; j>=0; j--) {
            if (scores[i] > scores[j]) {
                // desplaza puntuaciones y nombres hacia abajo
                if (j < 4) {
                    scores[j+1] = scores[j];
                    strncpy(names[j+1], names[j], 10);
                }
            } 
			else {
                break;
            }
        }
        scores[j+1] = scores[i];
        strncpy(names[j+1], names[i], 10);
    }
}

int main(void) {
	unsigned char i;
	
    while(TRUE) {
        init();		
		mainLoop();
		
        for (i=0; i<numPlayers; i++) {
            checkScores(i);
		}
		cls(1);
		drawHeader();
		drawHighScores();
    }
    return 0;
}
