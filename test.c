/*

*** TETRIS4DRAGONS ***
   salvaKantero2024

    TEST ZONE
    15
    42
    43
    46
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
    }*/

    printf("TEST");
    screen(0,1);    
    printf("TEST");
    waitkey(FALSE);
    return 0;
}
