
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

void locate(int row, int col) {
    asm {
        ldx #0x0400       ; Dirección base de la pantalla en modo texto
        ldy row           ; Fila en Y
        lda col           ; Columna en A
        ldb #32           ; 32 columnas por fila

        mul               ; Multiplica Y * 32 y almacena en D
        leax D,X          ; Desplaza desde la base de pantalla 0x0400
        stx 0x0080        ; Guarda en 0x0080 la dirección de memoria
    }
}

void wait_for_keypress() {
    asm {
        wait_key:
            lda $FF02       ; Leer el estado del teclado
            cmpa #$FF       ; Comparar con $FF (sin tecla presionada)
            beq wait_key    ; Si es $FF, no hay tecla presionada, seguir esperando
            rts             ; Si se presionó una tecla, salir de la función
    }
}

// Ejemplo de uso para imprimir un carácter en la posición:
void print_char_at(char c) {
    asm {
        ldx 0x0080        ; Cargar dirección calculada
        lda c             ; Cargar el carácter a imprimir
        sta ,x            ; Escribir en la posición
    }
}

void Menu() {
    locate(5,5);
    print_char_at('A');
    wait_for_keypress();
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
