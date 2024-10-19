
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
unsigned long:	dword
signed long:	sdword
*/

byte key;
byte i;
byte numPlayers;
byte gameOver[2];
byte level[2];
byte lines[2];
byte pitLeft[2];
char pit[2][PITWIDTH * PITHEIGHT];
sbyte nextShape[2];
char names[7][11] = {"DRAGON","DRAGON","DRAGON","DRAGON","DRAGON","",""};
word scores[7] = {1400, 1300, 1200, 1100, 1000, 0, 0};

/*
void drawString(word x, word y, const char *str) {
    char *screenPos = (char *) (SCREEN_BASE_ADDR + x + y * SCREEN_WIDTH);
    memcpy(screenPos, str, strlen(str));
}*/

void displayStatus() {
	/*
    Color 0, 2

    'player 1
    If GameOver(0) = TRUE Then
        Locate 8, PITLEFT(0)
        Print "GAME OVER!"
    End If
    Locate 2, 12: Print "=PLAYER 1="
    Locate 3, 12: Print "Level:" + Str$(Level(0)) 'pinta el num. de nivel
    Locate 4, 12: Print "Lines:" + Str$(Lines(0)) 'pinta las lineas
    Locate 5, 12: Print "Sc:" + Str$(Scores(5)) 'pinta la puntuacion
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
        Locate 13, 12: Print "Sc:" + Str$(Scores(6)) 'pinta la puntuacion
        Locate 14, 12: Print "Next:"
    End If

    For i = 0 To NumPlayers
        If GameOver(i) = FALSE Then DrawNextShape i
    Next i */
}

void drawPit(i) {
	/*
    'repinta el contenido del foso
    For PitY = 0 To PITHEIGHT - 1
        For PitX = 0 To PITWIDTH - 1
            BlockColor$ = Mid$(Pit$(i), ((PITWIDTH * PitY) + PitX) + 1, 1)
            DrawBlock BlockColor$, PitX, PitY, i
        Next PitX
    Next PitY */
}

void createShape(i) {
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

void drawHighScores() {
    for(i = 0; i < 5; i++) {
        locate(7, 8 + i);  printf("................");
        locate(7, 8 + i);  printf("%s", names[i]);
        locate(19, 8 + i); printf("%5d", scores[i]);
	}
	locate(2,14); printf("PRESS ANY KEY TO CONTINUE...");
	waitkey(0);
}

void drawHeader() {
    locate(8, 1); printf("***************");
    locate(8, 2); printf("* T E T R I S *");
    locate(8, 3); printf("***************");
    locate(7, 5); printf("SALVAKANTERO 2024");
}

void drawMenu() {
	cls(1);
	drawHeader();
	locate(7, 8);  printf("1)  1 PLAYER GAME");
    locate(7, 9);  printf("2)  2 PLAYER GAME");
    locate(7, 10); printf("3)  HIGH SCORES");
    locate(7, 11); printf("4)  EXIT");
    locate(6, 14); printf("SELECT OPTION (1-4)");	
}

void menu() {
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
void init() {
    menu();
    pitLeft[0] = 1;
    pitLeft[1] = 23;
	for(i = 0; i <= numPlayers; i++) {
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

// lógica para verificar si las puntuaciones son de TOP 5
void checkScores(byte player) {
}

int main(void) {
    //while (1) {
        init();
        for (i = 0; i <= numPlayers; i++)
            checkScores(i);
    //}
    return 0;
}
