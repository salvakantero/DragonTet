
#include <cmoc.h>
#include <coco.h>

#define SCREEN_WIDTH 32 // ancho de la pantalla en modo texto
#define SCREEN_BASE_ADDR 0x0400 // dirección base de la memoria de video en modo texto
#define KEYBOARD_STATUS 0xFF02 // dirección de registro de estado del teclado
#define KEYBOARD_DATA 0xFF00 // dirección de registro de datos del teclado

char key;
unsigned char i;
unsigned char numPlayers;
unsigned char pitLeft[2];
char names[7][11] = {"DRAGON","DRAGON","DRAGON","DRAGON","DRAGON","",""};
int scores[7] = {1400, 1300, 1200, 1100, 1000, 0, 0};

void drawString(int x, int y, const char *str) {
    char *screenPos = (char *) (SCREEN_BASE_ADDR + x + y * SCREEN_WIDTH);
    memcpy(screenPos, str, strlen(str));
}

void drawHighScores() {
    for(i = 0; i < 5; i++) {
        drawString(9, 9 + i, "................");
        drawString(9, 9 + i, names[i]);
        drawString(21, 9 + i, "1000");
	}
    drawString(4, 15, "Press any key to continue...");
}

void drawHeader() {
    drawString(9, 1, "***************");
    drawString(9, 2, "* T E T R I S *");
    drawString(9, 3, "***************");
    drawString(8, 5, "SALVAKANTERO 2024");
}

void drawMenu() {
	drawString(8, 8, "1) 1 PLAYER GAME");
    drawString(8, 9, "2) 2 PLAYER GAME");
    drawString(8, 10, "3) HIGH SCORES");
    drawString(8, 11, "4) EXIT");
    drawString(7, 14, "SELECT OPTION (1-4)");	
}

void Menu() {
	cls(1);
	drawHeader();
	drawMenu();
    do {
        key = getch();
        if (key == '1') {
            numPlayers = 1;
            break;
        } else if (key == '2') {
            numPlayers = 2;
            break;
        } else if (key == '3') {
            drawHighScores();
			drawMenu();
        } else if (key == '4')
            exit(0);     
    } while (1);
}

void Init() {
    // lógica para inicializar el sistema y los fosos
    Menu();
    pitLeft[0] = 1;
    pitLeft[1] = 23;
}

void CheckScores(unsigned char player) {
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
