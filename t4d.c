/*

***TETRIS4DRAGON***
 salvaKantero 2024

Based on Peter Swinkels' Qbasic code. (QBBlocks v1.0)

compatible with Dragon 32/64 and COCO

use the CMOC compiler 0.1.89 or higher
"cmoc --dragon -o t4d.bin t4d.c"

*/

#include <cmoc.h>
#include <coco.h>

#define LASTSIDEBLOCK 3 // to iterate over the current piece (0 to 3)
#define SIDEBLOCKCOUNT 4 // size of the piece's side (4x4)
#define BLOCKCOUNT SIDEBLOCKCOUNT * SIDEBLOCKCOUNT // size of the piece in blocks
#define NOBLOCK '0' // character representing an empty block
#define PITWIDTH 10 // width of the pit in blocks
#define PITHEIGHT 15 // height of the pit in blocks

char key; // key pressed
unsigned char numPlayers; // players (1-2)
// fake values for the initial TOP 5
char names[7][11] = {"DRAGON","DRAGON","DRAGON","DRAGON","DRAGON","",""};
unsigned int scores[7] = {1400, 1300, 1200, 1100, 1000, 0, 0};
unsigned char newScore;

unsigned char gameOver[2]; 			// FALSE = game in progress, TRUE = finished
float dropRate[2]; 					// 1 = falling, 0 = stopped at the bottom of the pit
float startTime[2];                 // time since the piece has moved
char pit[2][PITWIDTH * PITHEIGHT];	// content of each pit in blocks
unsigned char level[2]; 			// game levels (speed)
unsigned int lines[2]; 				// lines cleared
unsigned char pitLeft[2]; 			// X position of the left side of each pit

unsigned char shape[2]; 		// piece type (0 to 6)
unsigned char shapeAngle[2]; 	// piece rotation (0 to 3)
char* shapeMap[2];				// piece design
int shapeX[2], shapeY[2];		// piece XY position

unsigned char nextShape[2];		// type of the next piece (0 to 6)
char* nextShapeMap[2];		    // design of the next piece



void drawBlock(char blockColour, unsigned char pitX, unsigned char pitY, unsigned char i) {
    int colour = blockColour-'0';
    locate(pitX + pitLeft[i], pitY);
    /*
    dragon semigraphic characters: 127 to 255  
    +16: yellow  
    +32: blue  
    +48: red  
    +64: beige  
    +80: turquoise  
    +96: magenta  
    +112: orange 
    */
    //printf("%c", 143 + (colour * 16));
    printf("%d", colour);
}



void drawPit(unsigned char i) {
    unsigned char pitY, pitX;
    // loop through and repaint the contents of the pit
    for (pitY = 0; pitY < PITHEIGHT; pitY++) {
        for (pitX = 0; pitX < PITWIDTH; pitX++) {
            // get the colour of the block at the current position
            char blockcolour = pit[i][(PITWIDTH * pitY) + pitX];
            // draw the block with the specified colour
            drawBlock(blockcolour, pitX, pitY, i);
        }
    }
    waitkey(0); // <---------------------- TEST!
}



// check if a shape can move in the specified direction
unsigned char shapeCanMove(char *map, unsigned char xDirection, 
                    unsigned char yDirection, unsigned char i) {
    int pitX, pitY;
    unsigned char blockX, blockY;
    // loop through all the blocks of the piece
    for (blockY = 0; blockY <= LASTSIDEBLOCK; blockY++) {
        for (blockX = 0; blockX <= LASTSIDEBLOCK; blockX++) {
            if (map[(SIDEBLOCKCOUNT * blockY) + blockX] != NOBLOCK) {
                // calculate the position in the pit
                pitX = (shapeX[i] + blockX) + xDirection;
                pitY = (shapeY[i] + blockY) + yDirection;
                // check if the block is within the pit boundaries
                if (pitX >= 0 && pitX < PITWIDTH && pitY >= 0 && pitY < PITHEIGHT) {
                    // if the position in the pit is not empty, it cannot move
                    if (pit[i][(PITWIDTH*pitY)+pitX] != NOBLOCK) {
                        return FALSE;
                    }
                } 
                // if it is out of bounds, it cannot move
                else if (pitX < 0 || pitX >= PITWIDTH || pitY >= PITHEIGHT) {                    
                    return FALSE;
                }
            }
        }
    }
    return TRUE; // can move!
}



void drawNextShape(unsigned char i) {
    unsigned char blockX, blockY;
    char blockcolour;
    // Para todos los bloques de la pieza
    for (blockX=0; blockX<=LASTSIDEBLOCK; blockX++) {
        for (blockY=0; blockY<=LASTSIDEBLOCK; blockY++) {
            // colour de la pieza
            blockcolour = nextShapeMap[i][(SIDEBLOCKCOUNT*blockY)+blockX];
            if (blockcolour == NOBLOCK) {
                blockcolour = '2';
            }
            if (i == 0) {
                drawBlock(blockcolour, blockX+17, blockY+4, i);
            } else {
                drawBlock(blockcolour, blockX-5, blockY+12, i);
            }
        }
    }
}

void displayStatus(void) {
    // player 1
    if (gameOver[0] == TRUE) {
        locate(pitLeft[0], 8);
        printf("GAME OVER!");
    }
    locate(11, 0); printf("=PLAYER 1=");
    locate(12, 1); printf("LEVEL: %u", level[0]); // pinta el num. de nivel
    locate(12, 2); printf("LINES: %u", lines[0]); // pinta las lineas
    locate(12, 3); printf("SC: %4d", scores[5]); // pinta la puntuacion
    locate(12, 4); printf("NEXT:");
    // player 2
    if (numPlayers == 2) {
        if (gameOver[1] == TRUE) {
			locate(pitLeft[1], 8);
			printf("GAME OVER!");
        }
        locate(11, 8); printf("=PLAYER 2=");
        locate(12, 9); printf("LEVEL: %u", level[1]); // pinta el num. de nivel
        locate(12, 10); printf("LINES: %u", lines[1]); // pinta las lineas
        locate(12, 11); printf("SC: %4d", scores[6]); // pinta la puntuacion
        locate(12, 12); printf("NEXT:");
    }
    unsigned char i;
    for (i=0; i<numPlayers; i++) {
        if (gameOver[i] == FALSE) {
			drawNextShape(i);
        }
    }
}

const char* getShapeMap(unsigned char shape) {
    switch (shape) {
        case 0:
            return "0000333300000000"; // Palo largo
        case 1:
            return "0000666000600000"; // L 1
        case 2:
            return "0000111010000000"; // L 2
        case 3:
            return "0000022002200000"; // Cubo
        case 4:
            return "0000055055000000"; // S 1
        case 5:
            return "0000770007700000"; // S 2
        case 6:
            return "0000444004000000"; // T
        default:
            return ""; // Cadena vacía para casos no válidos
    }
}

// rotar la pieza y devolver el mapa resultante
char* getRotatedShapeMap(unsigned char shape, unsigned char angle) {
	const char* map = getShapeMap(shape); // mapa sin rotar
	char* rotatedMap; // mapa rotado
    int newBlockX, newBlockY;
	unsigned char i;

    // si el ángulo es 0, copia directamente el mapa original en rotatedMap
    if (angle == 0) {
        for (i=0; i<BLOCKCOUNT; i++) {
            rotatedMap[i] = map[i];
        }
        return rotatedMap;
    }
    // Inicializa rotatedMap como vacía
    for (i=0; i<BLOCKCOUNT; i++) {
        rotatedMap[i] = NOBLOCK;
    }
    // Para otros ángulos, recorre todos los bloques
    for (int blockX=0; blockX<=LASTSIDEBLOCK; blockX++) {
        for (int blockY=0; blockY<=LASTSIDEBLOCK; blockY++) {
            switch (angle) {
                case 1: // 270 grados
                    newBlockX = blockY;
                    newBlockY = LASTSIDEBLOCK-blockX;
                    break;
                case 2: // 180 grados
                    newBlockX = LASTSIDEBLOCK-blockX;
                    newBlockY = LASTSIDEBLOCK-blockY;
                    break;
                case 3: // 90 grados
                    newBlockX = LASTSIDEBLOCK-blockY;
                    newBlockY = blockX;
                    break;
            }
            // asigna el bloque correspondiente al ángulo en rotatedMap
            rotatedMap[SIDEBLOCKCOUNT*newBlockY+newBlockX] = map[SIDEBLOCKCOUNT*blockY+blockX];
        }
    }
	return rotatedMap;
}

void createNextShape(unsigned char i) {
	nextShape[i] = (unsigned char)(rand()%7); // Tipo de pieza (0 a 6)
	nextShapeMap[i] = getRotatedShapeMap(nextShape[i], 0); // Composición de la pieza
}

void createShape(unsigned char i) {
	// calcula la velocidad de caída en función del nivel
    dropRate[i] = 1-(level[i]*0.2f);
    if (dropRate[i] <= 0) {
        dropRate[i] = 0.1f;  // límite mínimo de velocidad de caída
    }
    // Si no es la primera pieza, toma el valor de nextShape
    if (nextShape[i] >= 0) {
        shape[i] = nextShape[i];
    } else {
        shape[i] = (unsigned char)rand()%7;  // nueva pieza aleatoria (0 a 6)
    }
	// composición de la pieza según su rotación
    shapeAngle[i] = 0;
    shapeMap[i] = getRotatedShapeMap(shape[i], shapeAngle[i]);
    // posición inicial (centro del foso en X, totalmente oculta en Y)
    shapeX[i] = 3;
    shapeY[i] = -SIDEBLOCKCOUNT;
    // genera la próxima pieza
    createNextShape(i);
}

void drawShape(unsigned char i, unsigned char eraseShape) {
    int pitX, pitY;
    unsigned char blockX, blockY;
    char blockcolour;

    // Recorre todos los bloques de la pieza
    for (blockX=0; blockX<=LASTSIDEBLOCK; blockX++) {
        for (blockY=0; blockY<=LASTSIDEBLOCK; blockY++) {
            pitX = shapeX[i]+blockX;
            pitY = shapeY[i]+blockY;
            // Verifica si el bloque está dentro de los límites del foso
            if (pitX>=0 && pitX<PITWIDTH && pitY>=0 && pitY<PITHEIGHT) {
                if (eraseShape) {
                    // Obtiene el colour del foso en la posición actual
                    blockcolour = pit[i][(PITWIDTH*pitY)+pitX];
                } else {
                    // Obtiene el colour del bloque de la pieza
                    blockcolour = shapeMap[i][(SIDEBLOCKCOUNT*blockY)+blockX];
                    // Si el bloque es vacío, toma el colour del foso en esa posición
                    if (blockcolour == NOBLOCK) {
                        blockcolour = pit[i][(PITWIDTH*pitY)+pitX];
                    }
                }
                drawBlock(blockcolour, (unsigned char)pitX, (unsigned char)pitY, i); // Pinta o borra el bloque
            }
        }
    }
}

void removeFullRow(unsigned char removedRow, unsigned char i) {
    unsigned char pitX, pitY;
    char blockcolour;
    // Itera desde la fila removida hacia arriba
    for (pitY=removedRow; pitY>0; pitY--) {
        for (pitX=0; pitX<PITWIDTH; pitX++) {
            if (pitY == 0) {
                blockcolour = NOBLOCK; // La línea más alta queda vacía
            } else {
                // Copia la línea superior en la actual
                blockcolour = pit[i][(pitY-1)*PITWIDTH+pitX];
            }
            // Asigna el colour a la línea actual
            pit[i][pitY*PITWIDTH+pitX] = blockcolour;
        }
    }
    // Vacía la primera fila después de bajar todas
    for (pitX = 0; pitX < PITWIDTH; pitX++) {
        pit[i][pitX] = NOBLOCK;
    }
}

void checkForFullRows(unsigned char i) { // busca filas completas
    unsigned char fullRow = FALSE;
    unsigned int numLines = 0;
    unsigned char pitX, pitY;
    unsigned char j = i+5;

    // Recorre todas las filas del foso
    for (pitY=0; pitY<PITHEIGHT; pitY++) {
        fullRow = TRUE;
        // Verifica cada bloque en la fila
        for (pitX=0; pitX<PITWIDTH; pitX++) {
            if (pit[i][pitY*PITWIDTH+pitX] == NOBLOCK) {
                fullRow = FALSE;  // Encuentra un hueco
                break;
            }
        }
        // Si la fila está completa, se elimina
        if (fullRow) {
            removeFullRow(pitY, i); // Función para eliminar la fila completa
            numLines++;
        }
    }
    // Actualiza la puntuación si se completaron líneas
    if (numLines > 0) {
        switch (numLines) {
            case 1:
                scores[j] += (100*level[i]);
                break;
            case 2:
                scores[j] += (300*level[i]);
                break;
            case 3:
                scores[j] += (500*level[i]);
                break;
            case 4:
                scores[j] += (800*level[i]); // Tetris!
                break;
        }
        // Actualiza el total de líneas completadas y calcula el nivel
        lines[i] += numLines;
        level[i] = (unsigned char)(lines[i]/10)+1;
    }
}

void settleActiveShapeInPit(unsigned char i) {
    unsigned char blockX, blockY;
    int pitX, pitY;
    // Itera sobre cada bloque de la pieza
    for (blockY=0; blockY<=LASTSIDEBLOCK; blockY++) {
        for (blockX=0; blockX<=LASTSIDEBLOCK; blockX++) {
            pitX = shapeX[i]+blockX;
            pitY = shapeY[i]+blockY;
            // Verifica que el bloque esté dentro de los límites del foso
            if (pitX>=0 && pitX<PITWIDTH && pitY>=0 && pitY<PITHEIGHT) {
                // Comprueba que el bloque no sea un hueco vacío
                if (shapeMap[i][blockY*SIDEBLOCKCOUNT+blockX] != NOBLOCK) {
                    // Fija el bloque de la pieza en el foso
                    pit[i][pitY*PITWIDTH+pitX] = shapeMap[i][blockY*SIDEBLOCKCOUNT+blockX];
                }
            }
        }
    }
}

void dropShape(unsigned char i) {
    // Verifica si la pieza puede moverse hacia abajo
    if (shapeCanMove(shapeMap[i], 0, 1, i)) {
        drawShape(i, TRUE);      // Borra la pieza actual
        shapeY[i] += 1;          // Baja la pieza una posición
        drawShape(i, FALSE);     // Vuelve a dibujar la pieza en la nueva posición
    } else {
        // La pieza se establece en el foso
        settleActiveShapeInPit(i);
        // Verifica si la pieza alcanzó la parte superior y se pierde la partida
        gameOver[i] = (shapeY[i]<0);
        // Verifica si hay filas completas para eliminar
        checkForFullRows(i);
        // Redibuja el contenido del foso
        drawPit(i);
        // Si el juego no ha terminado, crea una nueva pieza y la dibuja
        if (!gameOver[i]) {
            createShape(i);
            drawShape(i, FALSE);
        }
        // Muestra el estado actual del juego, como puntos o mensajes
        displayStatus();
    }
}

void drawHighScores(void) {
	unsigned char i;
    for(i=0; i<5; i++) {
        locate(7, 8+i);  printf("...............");
        locate(7, 8+i);  printf("%s", names[i]);
        locate(19, 8+i); printf("%5d", scores[i]);
	}
	locate(2,14); printf("PRESS ANY KEY TO CONTINUE...");
	waitkey(0);
}

void drawHeader(void) {
    locate(8, 1); printf("***************");
    locate(8, 2); printf("* T E T R I S *");
    locate(8, 3); printf("***************");
    locate(7, 5); printf("SALVAKANTERO 2024");
}

void drawMenu(void) {
	cls(1);
	drawHeader();
	locate(7, 8);  printf("1)  1 PLAYER GAME");
    locate(7, 9);  printf("2)  2 PLAYER GAME");
    locate(7, 10); printf("3)  HIGH SCORES");
    locate(7, 11); printf("4)  EXIT");
    locate(6, 14); printf("SELECT OPTION (1-4)");	
}

void menu(void) {
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
void init(void) {
	unsigned char i;
    menu();
	cls(1);
    pitLeft[0] = 0;
    pitLeft[1] = 22;
	for(i=0; i<numPlayers; i++) {
        gameOver[i] = FALSE; // partida en curso
        level[i] = 1; // nivel inicial
        lines[i] = 0; // lineas conseguidas
        scores[i+5] = 0; // puntuacion actual
        nextShape[i] = -1; // sera necesario generar pieza
        memset(pit[i], NOBLOCK, PITWIDTH*PITHEIGHT); // inicializa el foso vacío
        createShape(i); // genera pieza (forma, posición)
        drawPit(i); //pinta foso
    }
    displayStatus();
}

void mainLoop() {
    unsigned char newAngle;
    unsigned char i;
    char* rotatedMap;

    // Guarda el tiempo de inicio para el jugador 0
    startTime[0] = getTimer();
    if (numPlayers > 0) {
        // Guarda el tiempo de inicio para el jugador 1
        startTime[1] = getTimer();
    }
    while (TRUE) {
        char key = '\0';
        // Bucle hasta que se pulse una tecla
        while (key == '\0') {
            for (i=0; i<=numPlayers; i++) { // Bucle para ambos jugadores
                if (!gameOver[i]) { // Juego en curso
                    // Si ha sido superado el tiempo de caída
                    if (getTimer() >= startTime[i]+dropRate[i] || startTime[i] > getTimer()) {
                        dropShape(i); // La pieza avanza hacia abajo
                        startTime[i] = getTimer(); // Reinicia el temporizador de caída
                    }
                }
            }
            key = inkey(); // Lee pulsaciones del teclado
        }

        if (key == 'X') { // Si se pulsa X, sale
            break;
        } else {
            for (i=0; i<=numPlayers; i++) { // Bucle para ambos jugadores
                if (gameOver[i]) { // Fuera de juego
                    if (key == 'Z') { // Si se pulsa ENTER, reinicia juego
                        break;
                    }
                } else {
                    // Asigna teclas para cada jugador
                    char rotateKey, leftKey, downKey, rightKey;
                    if (i == 0) {
                        rotateKey = 'W';
                        leftKey = 'A';
                        downKey = 'S';
                        rightKey = 'D';
                    } else {
                        rotateKey = 'I';
                        leftKey = 'J';
                        downKey = 'K';
                        rightKey = 'L';
                    }

                    switch (key) {
                        case 'W': case 'I': // Tecla de rotación
                            drawShape(i, TRUE); // Borra pieza
                            newAngle = (shapeAngle[i] == 3) ? 0 : shapeAngle[i]+1;
                            rotatedMap = getRotatedShapeMap(shape[i], newAngle);
                            if (shapeCanMove(rotatedMap, 0, 0, i)) { // Si se puede mover...
                                shapeAngle[i] = newAngle;
                                shapeMap[i] = rotatedMap;
                            }
                            drawShape(i, FALSE); // Pinta pieza
                            break;

                        case 'A': case 'J': // Mueve a la izquierda
                            drawShape(i, TRUE); // Borra pieza
                            if (shapeCanMove(shapeMap[i], -1, 0, i)) shapeX[i]--;
                            drawShape(i, FALSE); // Pinta pieza
                            break;

                        case 'D': case 'L': // Mueve a la derecha
                            drawShape(i, TRUE); // Borra pieza
                            if (shapeCanMove(shapeMap[i], 1, 0, i)) shapeX[i]++;
                            drawShape(i, FALSE); // Pinta pieza
                            break;

                        case 'S': case 'K': // Pone el tiempo de descenso a 0
                            dropRate[i] = 0;
                            break;
                    }
                }
            }
        }
    }
}

// verifica si la nueva puntuación es lo suficientemente alta para entrar en el top 5
void checkScores(unsigned char player) {
    unsigned char i = player + 5;
	unsigned char j;

    if (scores[i] > scores[4]) {
        drawHeader();
        locate(3, 10); printf("BUENA PUNTUACION JUGADOR %d", player+1);
        locate(6, 11); printf("NOMBRE?: ");
        char *response = readline();
        strncpy(names[i], response, 10);
        names[i][10] = '\0';

        for (j=4; j>=0; j--) {
            if (scores[i] > scores[j]) {
                // desplaza puntuaciones y nombres hacia abajo
                if (j < 4) {
                    scores[j+1] = scores[j];
                    strncpy(names[j+1], names[j], 10);
                }
                newScore = TRUE;
            }
			else {
                break;
            }
        }
        scores[j+1] = scores[i];
        strncpy(names[j+1], names[i], 10);
    }
}

int main(void) {
	unsigned char i;
	
    while(TRUE) {
        init();		
		mainLoop();
		
        newScore = FALSE;
        for (i=0; i<numPlayers; i++) {
            checkScores(i);
		}
        if (newScore) {
		    cls(1);
		    drawHeader();
		    drawHighScores();
        }
    }
    return 0;
}
