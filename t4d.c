
//#include <math.h>
//#include <ctype.h>
//#include <stdio.h>
//#include <string.h>
//#include <conio.h>

#include <cmoc.h>
#include <coco.h>

#define SCREEN_WIDTH 32 // ancho de la pantalla en modo texto
#define SCREEN_BASE_ADDR 0x0400 // dirección base de la memoria de video en modo texto
#define KEYBOARD_STATUS 0xFF02 // dirección de registro de estado del teclado
#define KEYBOARD_DATA 0xFF00 // dirección de registro de datos del teclado

unsigned char i;
unsigned char numPlayers;
unsigned char pitLeft[2];
char names[7][11];
int scores[7];

void drawString(unsigned char x, unsigned char y, const char *str) {
    char *screenPos = (char *) (SCREEN_BASE_ADDR + x + y * SCREEN_WIDTH);
    memcpy(screenPos, str, strlen(str));
}

void waitForKeyPress() {
    unsigned char *keyStatus = (unsigned char *) KEYBOARD_STATUS;
    unsigned char *keyData = (unsigned char *) KEYBOARD_DATA;
    // espera hasta que se detecte una pulsación de tecla
    while ((*keyStatus & 0x01) == 0);  // espera a que el bit de "key ready" esté activo
}

void drawHeader() {
    cls(1);
    drawString(10, 2, "***************");
    drawString(10, 3, "* T E T R I S *");
    drawString(10, 4, "***************");
    drawString(9, 6, "SALVAKANTERO 2024");
}

void Menu() {
	drawHeader();
	waitForKeyPress();
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
    // tabla fake de las 5 mejores puntuaciones
    // puntuaciones de 0 a 4. Las dos ultimas son los puntos en juego
    for (i = 0; i < 5; i++) {
        strcpy(names[i], "DRAGON");
        scores[i] = 1400 - (i * 100);
    }

    //while (1) {
        Init(); // Inicializa el sistema y arranca el bucle principal
        // Después del bucle principal, verifica si las puntuaciones están en el TOP 5
        for (i = 0; i <= numPlayers; i++)
            CheckScores(i);
    //}

    return 0;
}
