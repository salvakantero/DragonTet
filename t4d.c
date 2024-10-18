
#include <cmoc.h>
#include <coco.h>

#define SCREEN_WIDTH 32 // ancho de la pantalla en modo texto
#define SCREEN_BASE_ADDR 0x0400 // dirección base de la memoria de video en modo texto
#define KEYBOARD_STATUS 0xFF02 // dirección de registro de estado del teclado
#define KEYBOARD_DATA 0xFF00 // dirección de registro de datos del teclado

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
char names[7][11] = {"DRAGON1","DRAGON2","DRAGON3","DRAGON4","DRAGON5","",""};
word scores[7] = {1400, 1300, 1200, 1100, 100, 0, 0};

void drawString(word x, word y, const char *str) {
    char *screenPos = (char *) (SCREEN_BASE_ADDR + x + y * SCREEN_WIDTH);
    memcpy(screenPos, str, strlen(str));
}

void drawHighScores() {
	char strScore[20];
    for(i = 0; i < 5; i++) {
        drawString(8, 8 + i, "................");
        drawString(8, 8 + i, names[i]);
		sprintf(strScore, "%5d", scores[i]);
        drawString(20, 8 + i, strScore);
	}
    drawString(2, 14, "PRESS ANY KEY TO CONTINUE...");
	waitkey(0);
}

void drawHeader() {
	cls(1);
    drawString(9, 1, "***************");
    drawString(9, 2, "* T E T R I S *");
    drawString(9, 3, "***************");
    drawString(8, 5, "SALVAKANTERO 2024");
}

void drawMenu() {
	drawHeader();
	drawString(8, 8, "1)  1 PLAYER GAME");
    drawString(8, 9, "2)  2 PLAYER GAME");
    drawString(8, 10, "3)  HIGH SCORES");
    drawString(8, 11, "4)  EXIT");
    drawString(7, 14, "SELECT OPTION (1-4)");	
}

void Menu() {
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

void Init() {
    // lógica para inicializar el sistema y los fosos
    Menu();
    pitLeft[0] = 1;
    pitLeft[1] = 23;
}

void CheckScores(byte player) {
    // Lógica para verificar si las puntuaciones son de TOP 5
}

int main(void) {
    //while (1) {
        Init(); // Inicializa el sistema y arranca el bucle principal
        // Después del bucle principal, verifica si las puntuaciones están en el TOP 5
        for (i = 0; i <= numPlayers; i++)
            CheckScores(i);
    //}
    return 0;
}
