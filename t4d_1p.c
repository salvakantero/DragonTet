/*

*** TETRIS4DRAGONS ***
   salvaKantero2024

1 player game TEST version
Compatible with Dragon 32/64 and COCO
Based on Peter Swinkels' PC Qbasic code. (QBBlocks v1.0)

use the CMOC compiler 0.1.89 or higher:
"cmoc --dragon -o t4d_1p.bin t4d_1p.c"
use xroar to test:
"xroar -run t4d_1p.bin"

TODO
====
- BUG se cuelga al rotar pieza en el borde
- centrar textos
- marquesina de color en el título
- teclado con repetición automática
- menú con imagen de fondo
- limitar tamaño de tipos en lo posible

*/

#include <cmoc.h>
#include <coco.h>

#define SIDEBLOCKCOUNT 4 // size of the piece's side (4x4)
#define LASTSIDEBLOCK 3 // to iterate over the current piece (0 to 3)
#define BLOCKCOUNT 16 // size of the piece in blocks
#define NOBLOCK '0' // character representing an empty block
#define PITWIDTH 10 // width of the pit in blocks
#define PITHEIGHT 16 // height of the pit in blocks
#define LINESLEVEL 10 // lines per level

#define DEBUGX 12
#define DEBUGY 15

char key;                       // key pressed
BOOL newScore;                  // TRUE = redraws the best scores table
BOOL gameOver; 			        // FALSE = game in progress, TRUE = finished
int dropRate; 					// 0 = lower the piece one position
int startTime;                  // system ticks since the piece has moved
char pit[PITWIDTH * PITHEIGHT]; // content of each pit in blocks
unsigned char level; 			// game levels (speed)
int lines; 				        // lines cleared
unsigned char shape, nextShape; // piece type (0 to 6). 255 = piece not defined
unsigned char shapeAngle; 	    // piece rotation (0 to 3)
char *shapeMap, *nextShapeMap;  // piece design
int shapeX, shapeY;		        // piece XY position

// pos 0-4: fake values for the initial TOP 5
// pos 5: values for the current game
char names[6][11] = {"DRAGON","DRAGON","DRAGON","DRAGON","DRAGON",""};
unsigned int scores[6] = {1400, 1300, 1200, 1100, 1000, 0};



void drawBlock(char blockColour, unsigned char pitX, unsigned char pitY) {
    /*
    dragon semigraphic characters: 128 to 255  
    +16: yellow  
    +32: blue  
    +48: red  
    +64: white 
    +80: cyan  
    +96: magenta  
    +112: orange 
    */
    unsigned char colour = blockColour - NOBLOCK; // (0 to 8)
    locate(pitX, pitY);
    // black background (empty block)
    if (colour == 0) {
        putchar(111);
        return;
    }
    // coloured filled block
    putchar(143 + ((colour - 1) * 16));
}



void drawPitSeparator() {
    unsigned char y;
    for (y = 0; y < PITHEIGHT; y++) {
        locate(PITWIDTH, y);
        putchar(128);
    }
}



void drawPit() {
    unsigned char pitY, pitX;
    // loop through and repaint the contents of the pit
    for (pitY = 0; pitY < PITHEIGHT; pitY++) {
        for (pitX = 0; pitX < PITWIDTH; pitX++) {
            // get the colour of the block at the current position
            char blockcolour = pit[(PITWIDTH * pitY) + pitX];
            // draw the block with the specified colour
            drawBlock(blockcolour, pitX, pitY);
        }
    }
}



// check if a shape can move in the specified direction
BOOL shapeCanMove(char *map, char xDirection, char yDirection) {
    int pitX, pitY;
    int blockX, blockY;
    // loop through all the blocks of the piece
    for (blockY = 0; blockY <= LASTSIDEBLOCK; blockY++) {
        for (blockX = 0; blockX <= LASTSIDEBLOCK; blockX++) {
            if (map[(SIDEBLOCKCOUNT * blockY) + blockX] != NOBLOCK) {
                // calculate the position in the pit
                pitX = (shapeX + blockX) + xDirection;
                pitY = (shapeY + blockY) + yDirection;
                // check if the block is within the pit boundaries
                if (pitX >= 0 && pitX < PITWIDTH && pitY >= 0 && pitY < PITHEIGHT) {
                    // if the position in the pit is not empty, it cannot move
                    if (pit[(PITWIDTH * pitY) + pitX] != NOBLOCK) {
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



void drawNextShape() {
    unsigned char blockX, blockY;
    char blockcolour;
    // loop through all the blocks of the piece
    for (blockX = 0; blockX <= LASTSIDEBLOCK; blockX++) {
        for (blockY = 0; blockY <= LASTSIDEBLOCK; blockY++) {
            // piece colour
            blockcolour = nextShapeMap[(SIDEBLOCKCOUNT * blockY) + blockX];
            if (blockcolour == NOBLOCK) {
                blockcolour = '1'; // green background
            }
            drawBlock(blockcolour, blockX + 17, blockY + 7);
        }
    }
}



void displayStatus(void) {
    if (gameOver == TRUE) {
        locate(0, 8);
        printf("GAME OVER!");
    }
    locate(11, 1); printf("=PLAYER 1=");
    locate(11, 3); printf("LEVEL:  %2u", level); // draws the level number
    locate(11, 4); printf("LINES: %3u", lines); // draws the lines
    locate(11, 5); printf("SC:  %5u", scores[5]); // draws the score
    locate(11, 7); printf("NEXT:");
    if (gameOver == FALSE) {
        drawNextShape();
    }
}



const char* getShapeMap(unsigned char shape) {
    /* colours:
        0 - black
        1 - green
        2 - yellow
        3 - blue
        4 - red
        5 - white
        6 - cyan
        7 - magenta
        8 - orange
    */
    switch (shape) {
        case 0:
            return "0000666600000000"; // | cyan
        case 1:
            return "0000333000300000"; // _| blue
        case 2:
            return "0000888080000000"; // |_ orange
        case 3:
            return "0000022002200000"; // [] yellow
        case 4:
            return "0000055055000000"; // S white
        case 5:
            return "0000440004400000"; // Z red
        case 6:
            return "0000777007000000"; // T magenta
        default:
            return "";
    }
}



// rotates the piece and returns the resulting map
char* getRotatedShapeMap(unsigned char shape, unsigned char angle) {
	const char *map = getShapeMap(shape); // unrotated map
    // if the angle is 0, return the original map
    if (angle == 0) {
        return (char*) map;
    }
    // for other angles, iterate through all blocks
	char *rotatedMap; // rotated map
    int newBlockX, newBlockY;
    int blockX, blockY;
    for (blockX = 0; blockX <= LASTSIDEBLOCK; blockX++) {
        for (blockY = 0; blockY <= LASTSIDEBLOCK; blockY++) {
            switch (angle) {
                case 1: // 270 degrees
                    newBlockX = blockY;
                    newBlockY = LASTSIDEBLOCK - blockX;
                    break;
                case 2: // 180 degrees
                    newBlockX = LASTSIDEBLOCK - blockX;
                    newBlockY = LASTSIDEBLOCK - blockY;
                    break;
                case 3: // 90 degrees
                    newBlockX = LASTSIDEBLOCK - blockY;
                    newBlockY = blockX;
                    break;
            }
            // assign the corresponding block to the rotatedMap for the angle
            rotatedMap[SIDEBLOCKCOUNT * newBlockY + newBlockX] = 
                map[SIDEBLOCKCOUNT * blockY + blockX];
        }
    }
	return rotatedMap;
}



void createNextShape() {
	nextShape = (unsigned char) rand() % 7; // piece type (0 to 6)
	nextShapeMap = getRotatedShapeMap(nextShape, 0); // piece composition
}



void createShape() {
    /* calculates the fall speed based on the level
        level 1: 30 delay ticks
        level 2: 25   "
        level 3: 20   "
        level 4: 15   "
        level 5: 10   "
        level x:  5   "
    */
    dropRate = 35 - (level * 5);
    // minimum fall speed limit
    if (dropRate <= 0) {
        dropRate = 1;
    }
    // if it's not the first piece, take the value of nextShape
    if (nextShape != 255) {
        shape = nextShape;
    } else {
        shape = (unsigned char) rand() % 7;  // new random piece (0 to 6)
    }    
    shapeAngle = 0;
    shapeMap = getRotatedShapeMap(shape, 0);
    // initial position (centre of the pit in X, fully hidden in Y)
    shapeX = 3;
    shapeY = -LASTSIDEBLOCK;
    // generates the next piece
    createNextShape();
}



void drawShape(BOOL eraseShape) {
    int pitX, pitY;
    int blockX, blockY;
    char blockColour;
    // iterates through all the blocks of the piece
    for (blockX = 0; blockX <= LASTSIDEBLOCK; blockX++) {
        for (blockY = 0; blockY <= LASTSIDEBLOCK; blockY++) {
            pitX = shapeX + blockX;
            pitY = shapeY + blockY;
            // check if the block is within the pit boundaries
            if (pitX >= 0 && pitX < PITWIDTH && pitY >= 0 && pitY < PITHEIGHT) {
                if (eraseShape == TRUE) {
                    // gets the colour of the pit at the current position
                    blockColour = pit[(PITWIDTH * pitY) + pitX];
                } else {
                    // gets the colour of the piece's block
                    blockColour = shapeMap[(SIDEBLOCKCOUNT * blockY) + blockX];
                    // if the block is empty, take the colour of the pit at that position
                    if (blockColour == NOBLOCK) {
                        blockColour = pit[(PITWIDTH * pitY) + pitX];
                    }
                }
                // draw or erase the block
                drawBlock(blockColour, (unsigned char)pitX, (unsigned char)pitY); 
            }
        }
    }
}



void removeFullRow(unsigned char removedRow) {
    unsigned char pitX, pitY;
    char blockColour;
    // iterate from the removed row upwards
    for (pitY = removedRow; pitY > 0; pitY--) {
        for (pitX = 0; pitX < PITWIDTH; pitX++) {
            if (pitY == 0) {
                blockColour = NOBLOCK; // the topmost line becomes empty
            } else {
                // copy the top row to the current one
                blockColour = pit[(pitY - 1) * PITWIDTH + pitX];
            }
            // assign the colour to the current row
            pit[pitY * PITWIDTH + pitX] = blockColour;
        }
    }
    // clear the first row after shifting all rows down
    for (pitX = 0; pitX < PITWIDTH; pitX++) {
        pit[pitX] = NOBLOCK;
    }
}



void checkForFullRows() { // searches for full rows
    BOOL fullRow = FALSE;
    int numLines = 0;
    unsigned char pitX, pitY;
    unsigned char j = 5;
    // loops through all the rows in the pit
    for (pitY = 0; pitY < PITHEIGHT; pitY++) {
        fullRow = TRUE;
        // checks each block in the row
        for (pitX = 0; pitX < PITWIDTH; pitX++) {
            if (pit[pitY * PITWIDTH + pitX] == NOBLOCK) {
                fullRow = FALSE; // finds an empty space
                break;
            }
        }
        // if the row is full, it gets removed
        if (fullRow) {
            removeFullRow(pitY);
            numLines++;
        }
    }
    // updates the score if rows were completed
    if (numLines > 0) {
        switch (numLines) {
            case 1:
                scores[j] += (100 * level); break;
            case 2:
                scores[j] += (300 * level); break;
            case 3:
                scores[j] += (500 * level); break;
            case 4:
                scores[j] += (800 * level);
        }
        // updates the total completed rows and calculates the level
        lines += numLines;
        level = (unsigned char)(lines / LINESLEVEL) + 1;
    }
}



void settleActiveShapeInPit() {
    int blockX, blockY;
    int pitX, pitY;
    for (blockY = 0; blockY <= LASTSIDEBLOCK; blockY++) {
        for (blockX = 0; blockX <= LASTSIDEBLOCK; blockX++) {
            pitX = shapeX + blockX;
            pitY = shapeY + blockY;
            // checks that the block is within the pit boundaries
            if (pitX >= 0 && pitX < PITWIDTH && pitY >= 0 && pitY < PITHEIGHT) {
                // checks that the block is not an empty space
                if (shapeMap[blockY * SIDEBLOCKCOUNT + blockX] != NOBLOCK) {
                    // fixes the piece's block in the pit
                    pit[pitY * PITWIDTH + pitX] = shapeMap[blockY * SIDEBLOCKCOUNT + blockX];
                }
            }
        }
    }
}



void dropShape() {
    // checks if the piece can move down
    if (shapeCanMove(shapeMap, 0, 1)) {
        drawShape(TRUE);      // erases the current piece
        shapeY += 1;          // moves the piece down by one position
        drawShape(FALSE);     // redraws the piece at the new position
    } else {
        settleActiveShapeInPit();
        // checks if the piece has reached the top and the game is lost
        gameOver = (shapeY < 0);

        checkForFullRows();
        drawPit();

        // if the game is not over, creates a new piece and draws it
        if (gameOver == FALSE) {
            createShape();
            drawShape(FALSE);
        }
        // displays the current game status, such as points or messages
        displayStatus();
    }
}



void drawHighScores() {
	unsigned char pos; // TOP 5 position (0-4)
    for(pos = 0; pos < 5; pos++) {
        locate(7, 8+pos);  printf("...............");
        locate(7, 8+pos);  printf("%s", names[pos]);
        locate(19, 8+pos); printf("%5d", scores[pos]);
	}
	locate(2, 14); printf("PRESS ANY KEY TO CONTINUE...");
	waitkey(0);
}



void drawHeader(unsigned char x) {
    unsigned char pos, colour = 2;
    for (pos = 0; pos < 15; pos++) {
        locate(x+pos, 1); putchar(143 + ((colour - 1) * 16));
        locate(x+pos, 3); putchar(143 + ((colour - 1) * 16));
        if (colour == 2) {
            colour = 8;
        }
        else {
            colour = 2;
        }
    }
    locate(x, 2); printf("= T E T R I S =");
    locate(x-1, 5); printf("SALVAKANTERO 2024");
}



void drawMenu() {
	cls(1);
	drawHeader(8);
	locate(7, 8);  printf("1)  START GAME");
    locate(7, 9);  printf("2)  HIGH SCORES");
    locate(7, 10); printf("3)  EXIT");
    locate(6, 14); printf("SELECT OPTION (1-3)");	
}



void menu() {
	drawMenu();
    do {
        key = inkey();		
        if (key == '1')	{ // start game
            break;
        } 
		else if (key == '2') { // high scores		
            drawHighScores();
			drawMenu();
        } 
		else if (key == '3') { // exit
			cls(1);
            exit(0);
		}
    } while (TRUE);
}



// logic to initialize the system and pits
void init() {
    setTimer(0);
    menu();
	cls(1); // green screen
    gameOver = FALSE; // game in progress
    level = 1; // initial level
    lines = 0; // lines cleared
    scores[5] = 0; // current score
    nextShape = 255; // piece generation will be required
    memset(pit, NOBLOCK, PITWIDTH * PITHEIGHT); // initialize the empty pit
    createShape(); // generate piece (shape, position)
    drawPit();
    displayStatus();
    drawPitSeparator();
}



void mainLoop() {
    unsigned char newAngle = 0;
    char *rotatedMap;

    // save the start time
    startTime = getTimer();

    while (TRUE) {
        char key = '\0';
        // loop until a key is pressed
        while (key == '\0') {
            if (gameOver == FALSE) { // game in progress
                // if the falling time has been exceeded
                if (getTimer() >= startTime + dropRate || startTime > getTimer()) {
                    dropShape(); // the piece moves down
                    startTime = getTimer(); // reset the fall timer
                }
            }
            key = inkey(); // read keypresses
        }

        if (key == 'X') { // if X is pressed, exit to the main menu
            break;
        } else {
            if (gameOver == FALSE) { // game in progress
                switch (key) {
                    case 'W': // rotate key
                        drawShape(TRUE); // erase piece                            
                        if (shapeAngle == 3) {
                            // 270 degrees. Reset angle
                            newAngle = 0;
                        }
                        else {
                            // increases the angle by 90 degrees
                            newAngle = shapeAngle + 1;
                        }
                        rotatedMap = getRotatedShapeMap(shape, newAngle);
                        if (shapeCanMove(rotatedMap, 0, 0)) {
                            shapeAngle = newAngle;
                            shapeMap = rotatedMap;
                        }
                        drawShape(FALSE); 
                        break;

                    case 'A': // move left
                        if (shapeCanMove(shapeMap, -1, 0)) {
                            drawShape(TRUE); // erase piece
                            shapeX--;
                            drawShape(FALSE);
                        }
                        break;

                    case 'D': // move right
                        if (shapeCanMove(shapeMap, 1, 0)) {
                            drawShape(TRUE); // erase piece
                            shapeX++;
                            drawShape(FALSE);
                        }
                        break;

                    case 'S': // set the descent time to 0
                        dropRate = 0;
                        break;
                }
            }
        }
    }
}



// check if the new score is high enough to enter the top 5
void checkScores() {
    unsigned char i = 5; // 0-1-2-3-4-[current]
	unsigned char j;

    if (scores[i] > scores[4]) {
        locate(15, 12); printf("GOOD SCORE!!");
        locate(15, 13); printf("NAME?:");
        locate(14, 20);
        char *response = readline();
        strncpy(names[i], response, 10);
        names[i][10] = '\0'; // ensure the name is null-terminated
        // find the correct position in the top 5 list for the new score
        for (j = 4; j >= 0; j--) {
            if (scores[i] >= scores[j]) {
                // shift scores/names down
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
        // place the new score and name in the correct position in the top 5
        scores[j+1] = scores[i];
        strncpy(names[j+1], names[i], 10);
    }
}



int main() {
    while(TRUE) {
        srand(getTimer()); // random seed
        init();		
		mainLoop();
		// check the scores of the last game
        newScore = FALSE;
        checkScores();
        // draw the scoreboard
        if (newScore == TRUE) {
		    cls(1);
		    drawHeader(8);
		    drawHighScores();
        }
    }
    return 0;
}
