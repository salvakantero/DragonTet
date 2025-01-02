/*

*** DRAGONTET ***
salvaKantero 2025

    TEST ZONE

*/

#include <cmoc.h>
#include <coco.h>

#define SCREEN_BASE 0x400 // base address of the video memory in text mode
#define SCREEN_WIDTH 32 // screen width in characters

void printBlock(int x, int y, unsigned char ch) {
    // calculates the memory address based on X and Y coordinates
    unsigned char *screenPos = (unsigned char *)(SCREEN_BASE + y * SCREEN_WIDTH + x);
    *screenPos = ch; // write character to video memory
}

void checkKeys() {
    while (TRUE) {
        if (isKeyPressed(KEY_PROBE_Q, KEY_BIT_Q)) putchar('Q');
        if (isKeyPressed(KEY_PROBE_W, KEY_BIT_W)) putchar('W');
        if (isKeyPressed(KEY_PROBE_E, KEY_BIT_E)) putchar('E');
        if (isKeyPressed(KEY_PROBE_R, KEY_BIT_R)) putchar('R');
        if (isKeyPressed(KEY_PROBE_T, KEY_BIT_T)) putchar('T');
        if (isKeyPressed(KEY_PROBE_Y, KEY_BIT_Y)) putchar('Y');
        if (isKeyPressed(KEY_PROBE_U, KEY_BIT_U)) putchar('U');
        if (isKeyPressed(KEY_PROBE_I, KEY_BIT_I)) putchar('I');
        if (isKeyPressed(KEY_PROBE_O, KEY_BIT_O)) putchar('O');
        if (isKeyPressed(KEY_PROBE_P, KEY_BIT_P)) putchar('P');

        if (isKeyPressed(KEY_PROBE_A, KEY_BIT_A)) putchar('A');
        if (isKeyPressed(KEY_PROBE_S, KEY_BIT_S)) putchar('S');
        if (isKeyPressed(KEY_PROBE_D, KEY_BIT_D)) putchar('D');
        if (isKeyPressed(KEY_PROBE_F, KEY_BIT_F)) putchar('F');
        if (isKeyPressed(KEY_PROBE_G, KEY_BIT_G)) putchar('G');
        if (isKeyPressed(KEY_PROBE_H, KEY_BIT_H)) putchar('H');
        if (isKeyPressed(KEY_PROBE_J, KEY_BIT_J)) putchar('J');
        if (isKeyPressed(KEY_PROBE_K, KEY_BIT_K)) putchar('K');
        if (isKeyPressed(KEY_PROBE_L, KEY_BIT_L)) putchar('L');

        if (isKeyPressed(KEY_PROBE_Z, KEY_BIT_Z)) putchar('Z');
        if (isKeyPressed(KEY_PROBE_X, KEY_BIT_X)) putchar('X');
        if (isKeyPressed(KEY_PROBE_C, KEY_BIT_C)) putchar('C');
        if (isKeyPressed(KEY_PROBE_V, KEY_BIT_V)) putchar('V');
        if (isKeyPressed(KEY_PROBE_B, KEY_BIT_B)) putchar('B');
        if (isKeyPressed(KEY_PROBE_N, KEY_BIT_N)) putchar('N');
        if (isKeyPressed(KEY_PROBE_M, KEY_BIT_M)) putchar('M');

        if (isKeyPressed(KEY_PROBE_UP, KEY_BIT_UP)) putchar('u');
        if (isKeyPressed(KEY_PROBE_DOWN, KEY_BIT_DOWN)) putchar('d');
        if (isKeyPressed(KEY_PROBE_LEFT, KEY_BIT_LEFT)) putchar('l');
        if (isKeyPressed(KEY_PROBE_RIGHT, KEY_BIT_RIGHT)) putchar('r');
        if (isKeyPressed(KEY_PROBE_SPACE, KEY_BIT_SPACE)) putchar('-');

        if (isKeyPressed(KEY_PROBE_1, KEY_BIT_1)) putchar('1');
        if (isKeyPressed(KEY_PROBE_2, KEY_BIT_2)) putchar('2');
        if (isKeyPressed(KEY_PROBE_3, KEY_BIT_3)) putchar('3');
        if (isKeyPressed(KEY_PROBE_4, KEY_BIT_4)) putchar('4');
        if (isKeyPressed(KEY_PROBE_5, KEY_BIT_5)) putchar('5');
        if (isKeyPressed(KEY_PROBE_6, KEY_BIT_6)) putchar('6');
        if (isKeyPressed(KEY_PROBE_7, KEY_BIT_7)) putchar('7');
        if (isKeyPressed(KEY_PROBE_8, KEY_BIT_8)) putchar('8');
        if (isKeyPressed(KEY_PROBE_9, KEY_BIT_9)) putchar('9');
        if (isKeyPressed(KEY_PROBE_0, KEY_BIT_0)) putchar('0');

        if (isKeyPressed(KEY_PROBE_AT, KEY_BIT_AT)) putchar('@');
        if (isKeyPressed(KEY_PROBE_COLON, KEY_BIT_COLON)) putchar('.');
        if (isKeyPressed(KEY_PROBE_SEMICOLON, KEY_BIT_SEMICOLON)) putchar(';');
        if (isKeyPressed(KEY_PROBE_COMMA, KEY_BIT_COMMA)) putchar(',');
        if (isKeyPressed(KEY_PROBE_HYPHEN, KEY_BIT_HYPHEN)) putchar('h');
        if (isKeyPressed(KEY_PROBE_PERIOD, KEY_BIT_PERIOD)) putchar('p');
        if (isKeyPressed(KEY_PROBE_SLASH, KEY_BIT_SLASH)) putchar('/');
        if (isKeyPressed(KEY_PROBE_ENTER, KEY_BIT_ENTER)) putchar('e');
        if (isKeyPressed(KEY_PROBE_CLEAR, KEY_BIT_CLEAR)) putchar('c');
        if (isKeyPressed(KEY_PROBE_BREAK, KEY_BIT_BREAK)) putchar('b');
        if (isKeyPressed(KEY_PROBE_ALT, KEY_BIT_ALT)) putchar('a');
        if (isKeyPressed(KEY_PROBE_CTRL, KEY_BIT_CTRL)) putchar('t');
        if (isKeyPressed(KEY_PROBE_F1, KEY_BIT_F1)) putchar('1');
        if (isKeyPressed(KEY_PROBE_F2, KEY_BIT_F2)) putchar('2');
        if (isKeyPressed(KEY_PROBE_SHIFT, KEY_BIT_SHIFT)) putchar('s');
    }
}

int main() {
    cls(1);
    
    /*
    // Recorrer todas las posiciones válidas de la pantalla
    unsigned char ch = 0;
    for (int y = 0; y < 16; y++) { // Desde 0 hasta la altura de la pantalla
        for (int x = 0; x < 32; x++) { // Desde 0 hasta el ancho de la pantalla
            printBlock(x, y, ch); // Imprime valores de prueba, ajustados al rango de caracteres
            if (ch < 256) ch++;
        }
    } */

    checkKeys();

    return 0;
}
