/*

*** DRAGONTET ***
salvaKantero 2025

    TEST ZONE

*/

#include <cmoc.h>
#include <coco.h>

#define SCREEN_BASE 0x400 // base address of the video memory in text mode
#define SCREEN_WIDTH 32 // screen width in characters

#define PEEK(addr) (*(unsigned char *)(addr))
#define POKE(addr, value) (*(unsigned char *)(addr) = (value))

#define KEYBOARD_BASE 0x0151
#define MASK_W        0x08 // Bit 3 en la posición 345 (0x0159)
#define MASK_A        0x20 // Bit 5 en la posición 339 (0x0153)
#define MASK_S        0x10 // Bit 4 en la posición 341 (0x0155)
#define MASK_D        0x40 // Bit 6 en la posición 342 (0x0156)

void printBlock(int x, int y, unsigned char ch) {
    // calculates the memory address based on X and Y coordinates
    unsigned char *screenPos = (unsigned char *)(SCREEN_BASE + y * SCREEN_WIDTH + x);
    *screenPos = ch; // write character to video memory
}

unsigned char isKeyPressed2(int address, unsigned char mask) {
    return !(*((int *)address) & mask);
}

void checkKeys() {
    POKE(0x0151, 0xFF); // Limpia la tabla de rollover antes de leer

    if (isKeyPressed2(KEYBOARD_BASE + 8, MASK_W)) {
        // Gira la pieza
        putchar('W');
    }
    if (isKeyPressed2(KEYBOARD_BASE + 2, MASK_A)) {
        // Mueve la pieza a la izquierda
        putchar('A');
    }
    if (isKeyPressed2(KEYBOARD_BASE + 4, MASK_S)) {
        // Baja la pieza rápidamente
        putchar('S');
    }
    if (isKeyPressed2(KEYBOARD_BASE + 5, MASK_D)) {
        // Mueve la pieza a la derecha
        putchar('D');
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
    }*/

    checkKeys();

    return 0;
}
