
//#include <math.h>
//#include <ctype.h>
//#include <stdio.h>
//#include <string.h>
//#include <conio.h>

#include <basic.h>

unsigned char i;
unsigned char numPlayers;
unsigned char pitLeft[2];
char names[7][11];
int scores[7];

void Menu() {
}

void Init() {
    // Lógica para inicializar el sistema y los fosos
    Menu();
    clrscr();
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

    while (1) {
        Init(); // Inicializa el sistema y arranca el bucle principal
        // Después del bucle principal, verifica si las puntuaciones están en el TOP 5
        for (i = 0; i <= numPlayers; i++)
            CheckScores(i);
    }

    return 0;
}
