
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
byte pitLeft[2];
char names[7][11] = {"DRAGON","DRAGON","DRAGON","DRAGON","DRAGON","",""};
word scores[7] = {1400, 1300, 1200, 1100, 1000, 0, 0};
char pit[2][PITWIDTH * PITHEIGHT];

/*
void drawString(word x, word y, const char *str) {
    char *screenPos = (char *) (SCREEN_BASE_ADDR + x + y * SCREEN_WIDTH);
    memcpy(screenPos, str, strlen(str));
}*/

void drawHighScores() {
    for(i = 0; i < 5; i++) {
        locate(7, 8 + i); printf("................");
        locate(7, 8 + i); printf("%s", names[i]);
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
        gameOver(i) = FALSE; // partida en curso
        level(i) = 1; // nivel inicial
        lines(i) = 0 // lineas conseguidas
        scores(i + 5) = 0; // puntuacion actual
        nextShape(i) = -1; // sera necesario generar pieza
        memset(pit(i), NOBLOCK, PITWIDTH * PITHEIGHT); // inicializa el foso vacío
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
        Init();
        for (i = 0; i <= numPlayers; i++)
            checkScores(i);
    //}
    return 0;
}
