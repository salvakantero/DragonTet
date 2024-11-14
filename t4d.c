/*

*** TETRIS4DRAGONS ***
   salvaKantero2024

Compatible with Dragon 32/64 and COCO

Based on Peter Swinkels' PC Qbasic code. (QBBlocks v1.0. Text mode, 1 player)

use the CMOC compiler 0.1.89 or higher:
"cmoc --dragon -o t4d.bin t4d.c"
use xroar to test:
"xroar -run t4d.bin"

BOOL = unsigned char
byte = unsigned char
sbyte = signed char
word = unsigned int
sword = signed int


TODO
====
- BUG se cuelga al rotar pieza en el borde
- BUG línea 16 con 2 players
- pit derecho pausado con 1 player
- menú con imagen de fondo y marquesina de color
- fondo negro con gráfico GDU
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

char key; // key pressed
byte numPlayers; // 0 = player1  1 = player2
// pos 0-4: fake values for the initial TOP 5
// pos 5-6: values for the current game
char names[7][11] = {"DRAGON","DRAGON","DRAGON","DRAGON","DRAGON","",""};
word scores[7] = {1400, 1300, 1200, 1100, 1000, 0, 0};

BOOL newScore;                      // TRUE = redraws the best scores table
BOOL gameOver[2]; 			        // FALSE = game in progress, TRUE = finished
int dropRate[2]; 					// 0 = lower the piece one position
int startTime[2];                   // system ticks since the piece has moved
char pit[2][PITWIDTH * PITHEIGHT];	// content of each pit in blocks
byte level[2]; 			            // game levels (speed)
word lines[2]; 				        // lines cleared
byte pitLeft[2]; 			        // X position of the left side of each pit

sbyte shape[2]; 		        // piece type (-1 to 6)
byte shapeAngle[2]; 	        // piece rotation (0 to 3)
char* shapeMap[2];				// piece design
sword shapeX[2], shapeY[2];		// piece XY position

sbyte nextShape[2];		        // type of the next piece (-1 to 6)
char* nextShapeMap[2];		    // design of the next piece



void drawBlock(char blockColour, byte pitX, byte pitY, byte i) {
    /*
    dragon semigraphic characters: 127 to 255  
    +16: yellow  
    +32: blue  
    +48: red  
    +64: white 
    +80: cyan  
    +96: magenta  
    +112: orange 
    */
    byte colour = blockColour-'0'; // (0 to 8)
    locate(pitX + pitLeft[i], pitY);
    // black background (empty block)
    if (colour == 0) {
        putchar(126);
        return;
    }
    // coloured filled block
    putchar(143 + ((colour-1) * 16));
}



void drawPit(byte i) {
    word pitY, pitX;
    // loop through and repaint the contents of the pit
    for (pitY = 0; pitY < PITHEIGHT; pitY++) {
        for (pitX = 0; pitX < PITWIDTH; pitX++) {
            // get the colour of the block at the current position
            char blockcolour = pit[i][(PITWIDTH * pitY) + pitX];
            // draw the block with the specified colour
            drawBlock(blockcolour, (byte)pitX, (byte)pitY, i);
        }
    }
}



// check if a shape can move in the specified direction
byte shapeCanMove(char *map, sbyte xDirection, sbyte yDirection, byte i) {
    sword pitX, pitY;
    sword blockX, blockY;
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
                    if (pit[i][(PITWIDTH * pitY) + pitX] != NOBLOCK) {
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



void drawNextShape(byte i) {
    byte blockX, blockY;
    char blockcolour;
    // loop through all the blocks of the piece
    for (blockX = 0; blockX <= LASTSIDEBLOCK; blockX++) {
        for (blockY = 0; blockY <= LASTSIDEBLOCK; blockY++) {
            // piece colour
            blockcolour = nextShapeMap[i][(SIDEBLOCKCOUNT * blockY) + blockX];
            if (blockcolour == NOBLOCK) {
                blockcolour = '1';
            }
            if (i == 0) { // player 1
                drawBlock(blockcolour, blockX + 17, blockY + 4, i);
            } else { // player 2
                drawBlock(blockcolour, blockX - 5, blockY + 12, i);
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
    locate(11, 1); printf("LEVEL:  %2u", level[0]); // draws the level number
    locate(11, 2); printf("LINES: %3u", lines[0]); // draws the lines
    locate(11, 3); printf("SC:  %5d", scores[5]); // draws the score
    locate(11, 4); printf("NEXT:");

    // player 2
    if (numPlayers == 1) {
        if (gameOver[1] == TRUE) {
			locate(pitLeft[1], 8);
			printf("GAME OVER!");
        }
        locate(11, 8);  printf("=PLAYER 2=");
        locate(11, 9);  printf("LEVEL:  %2u", level[1]); // draws the level number
        locate(11, 10); printf("LINES: %3u", lines[1]); // draws the lines
        locate(11, 11); printf("SC:  %5d", scores[6]); // draws the score
        locate(11, 12); printf("NEXT:");
    }
    
    byte i; // player 1-2
    for (i = 0; i <= numPlayers; i++) {
        if (gameOver[i] == FALSE) {
			drawNextShape(i);
        }
    }
}



const char* getShapeMap(byte shape) {
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
char* getRotatedShapeMap(char shape, byte angle) {
	const char* map = getShapeMap(shape); // unrotated map
    // if the angle is 0, return the original map
    if (angle == 0) {
        return (char*)map;
    }
    // for other angles, iterate through all blocks
	char* rotatedMap; // rotated map
    sword newBlockX, newBlockY;
    sword blockX, blockY;
	byte i;
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



void createNextShape(byte i) {
	nextShape[i] = (sbyte)(rand() % 7); // piece type (0 to 6)
	nextShapeMap[i] = getRotatedShapeMap(nextShape[i], 0); // piece composition
}



void createShape(byte i) {
    /* calculates the fall speed based on the level
        level 1: 30 delay ticks
        level 2: 25   "
        level 3: 20   "
        level 4: 15   "
        level 5: 10   "
        level x:  5   "
    */
    dropRate[i] = 35 - (level[i] * 5);
    // minimum fall speed limit
    if (dropRate[i] <= 0) {
        dropRate[i] = 1;
    }
    // if it's not the first piece, take the value of nextShape
    if (nextShape[i] >= 0) {
        shape[i] = nextShape[i];
    } else {
        shape[i] = (sbyte)(rand() % 7);  // new random piece (0 to 6)
    }    
    shapeAngle[i] = 0;
    shapeMap[i] = getRotatedShapeMap(shape[i], 0);
    // initial position (centre of the pit in X, fully hidden in Y)
    shapeX[i] = 3;
    shapeY[i] = -LASTSIDEBLOCK;
    // generates the next piece
    createNextShape(i);
}



void drawShape(byte i, BOOL eraseShape) {
    sword pitX, pitY;
    sword blockX, blockY;
    char blockColour;
    // iterates through all the blocks of the piece
    for (blockX = 0; blockX <= LASTSIDEBLOCK; blockX++) {
        for (blockY = 0; blockY <= LASTSIDEBLOCK; blockY++) {
            pitX = shapeX[i] + blockX;
            pitY = shapeY[i] + blockY;
            // check if the block is within the pit boundaries
            if (pitX >= 0 && pitX < PITWIDTH && pitY >= 0 && pitY < PITHEIGHT) {
                if (eraseShape == TRUE) {
                    // gets the colour of the pit at the current position
                    blockColour = pit[i][(PITWIDTH * pitY) + pitX];
                } else {
                    // gets the colour of the piece's block
                    blockColour = shapeMap[i][(SIDEBLOCKCOUNT * blockY) + blockX];
                    // if the block is empty, take the colour of the pit at that position
                    if (blockColour == NOBLOCK) {
                        blockColour = pit[i][(PITWIDTH * pitY) + pitX];
                    }
                }
                // draw or erase the block
                drawBlock(blockColour, (byte)pitX, (byte)pitY, i); 
            }
        }
    }
}



void removeFullRow(byte removedRow, byte i) {
    byte pitX, pitY;
    char blockColour;
    // iterate from the removed row upwards
    for (pitY = removedRow; pitY > 0; pitY--) {
        for (pitX = 0; pitX < PITWIDTH; pitX++) {
            if (pitY == 0) {
                blockColour = NOBLOCK; // the topmost line becomes empty
            } else {
                // copy the top row to the current one
                blockColour = pit[i][(pitY - 1) * PITWIDTH + pitX];
            }
            // assign the colour to the current row
            pit[i][pitY * PITWIDTH + pitX] = blockColour;
        }
    }
    // clear the first row after shifting all rows down
    for (pitX = 0; pitX < PITWIDTH; pitX++) {
        pit[i][pitX] = NOBLOCK;
    }
}



void checkForFullRows(byte i) { // searches for full rows
    BOOL fullRow = FALSE;
    word numLines = 0;
    byte pitX, pitY;
    byte j = i + 5;
    // loops through all the rows in the pit
    for (pitY = 0; pitY < PITHEIGHT; pitY++) {
        fullRow = TRUE;
        // checks each block in the row
        for (pitX = 0; pitX < PITWIDTH; pitX++) {
            if (pit[i][pitY * PITWIDTH + pitX] == NOBLOCK) {
                fullRow = FALSE; // finds an empty space
                break;
            }
        }
        // if the row is full, it gets removed
        if (fullRow) {
            removeFullRow(pitY, i);
            numLines++;
        }
    }
    // updates the score if rows were completed
    if (numLines > 0) {
        switch (numLines) {
            case 1:
                scores[j] += (100 * level[i]); break;
            case 2:
                scores[j] += (300 * level[i]); break;
            case 3:
                scores[j] += (500 * level[i]); break;
            case 4:
                scores[j] += (800 * level[i]);
        }
        // updates the total completed rows and calculates the level
        lines[i] += numLines;
        level[i] = (byte)(lines[i] / LINESLEVEL) + 1;
    }
}



void settleActiveShapeInPit(byte i) {
    sword blockX, blockY;
    sword pitX, pitY;
    for (blockY = 0; blockY <= LASTSIDEBLOCK; blockY++) {
        for (blockX = 0; blockX <= LASTSIDEBLOCK; blockX++) {
            pitX = shapeX[i] + blockX;
            pitY = shapeY[i] + blockY;
            // checks that the block is within the pit boundaries
            if (pitX >= 0 && pitX < PITWIDTH && pitY >= 0 && pitY < PITHEIGHT) {
                // checks that the block is not an empty space
                if (shapeMap[i][blockY * SIDEBLOCKCOUNT + blockX] != NOBLOCK) {
                    // fixes the piece's block in the pit
                    pit[i][pitY * PITWIDTH + pitX] = shapeMap[i][blockY * SIDEBLOCKCOUNT + blockX];
                }
            }
        }
    }
}



void dropShape(byte i) {
    // checks if the piece can move down
    if (shapeCanMove(shapeMap[i], 0, 1, i)) {
        drawShape(i, TRUE);      // erases the current piece
        shapeY[i] += 1;          // moves the piece down by one position
        drawShape(i, FALSE);     // redraws the piece at the new position
    } else {
        settleActiveShapeInPit(i);
        // checks if the piece has reached the top and the game is lost
        gameOver[i] = (shapeY[i] <= 0);

        checkForFullRows(i);
        drawPit(i);

        // if the game is not over, creates a new piece and draws it
        if (!gameOver[i]) {
            createShape(i);
            drawShape(i, FALSE);
        }
        // displays the current game status, such as points or messages
        displayStatus();
    }
}



void drawHighScores(void) {
	byte i; // TOP 5 position (0-4)
    for(i = 0; i < 5; i++) {
        locate(7, 8+i);  printf("...............");
        locate(7, 8+i);  printf("%s", names[i]);
        locate(19, 8+i); printf("%5d", scores[i]);
	}
	locate(2, 14); printf("PRESS ANY KEY TO CONTINUE...");
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
        if (key == '1')	{ // 1 player game
            numPlayers = 0;
            break;
        } 
		else if (key == '2') { // 2 player game
            numPlayers = 1;
            break;
        } 
		else if (key == '3') { // high scores		
            drawHighScores();
			drawMenu();
        } 
		else if (key == '4') { // exit
			cls(1);
            exit(0);
		}
    } while (TRUE);
}



// logic to initialize the system and pits
void init(void) {
	byte i; // player 1-2
    menu();
	cls(1); // green screen
	for(i = 0; i <= numPlayers; i++) {
        gameOver[i] = FALSE; // game in progress
        level[i] = 1; // initial level
        lines[i] = 0; // lines cleared
        scores[i+5] = 0; // current score
        nextShape[i] = -1; // piece generation will be required
        memset(pit[i], NOBLOCK, PITWIDTH * PITHEIGHT); // initialize the empty pit
        createShape(i); // generate piece (shape, position)
        drawPit(i);
    }
    displayStatus();
}



void mainLoop() {
    byte newAngle = 0;
    byte i; // player 1-2
    char* rotatedMap;

    // save the start time for both players
    word ticks = getTimer();
    startTime[0] = ticks;
    startTime[1] = ticks;

    while (TRUE) {
        char key = '\0';
        // loop until a key is pressed
        while (key == '\0') {
            for (i = 0; i <= numPlayers; i++) {
                if (!gameOver[i]) { // game in progress
                    ticks = getTimer();
                    // if the falling time has been exceeded
                    if (ticks >= startTime[i] + dropRate[i] || startTime[i] > ticks) {
                        dropShape(i); // the piece moves down
                        startTime[i] = getTimer(); // reset the fall timer
                    }
                }
            }
            key = inkey(); // read keypresses
        }

        if (key == 'X') { // if X is pressed, exit to the main menu
            break;
        } else {
            for (i = 0; i <= numPlayers; i++) {
                if (!gameOver[i]) {
                    switch (key) {
                        case 'W': case 'I': // rotate key
                            drawShape(i, TRUE); // erase piece                            
                            if (shapeAngle[i] == 3) {
                                // 270 degrees. Reset angle
                                newAngle = 0;
                            }
                            else {
                                // increases the angle by 90 degrees
                                newAngle = shapeAngle[i] + 1;
                            }
                            rotatedMap = getRotatedShapeMap(shape[i], newAngle);
                            if (shapeCanMove(rotatedMap, 0, 0, i)) {
                                shapeAngle[i] = newAngle;
                                shapeMap[i] = rotatedMap;
                            }
                            drawShape(i, FALSE); 
                            break;

                        case 'A': case 'J': // move left
                            if (shapeCanMove(shapeMap[i], -1, 0, i)) {
                                drawShape(i, TRUE); // erase piece
                                shapeX[i]--;
                                drawShape(i, FALSE);
                            }
                            break;

                        case 'D': case 'L': // move right
                            if (shapeCanMove(shapeMap[i], 1, 0, i)) {
                                drawShape(i, TRUE); // erase piece
                                shapeX[i]++;
                                drawShape(i, FALSE);
                            }
                            break;

                        case 'S': case 'K': // set the descent time to 0
                            dropRate[i] = 0;
                            break;
                    }
                }
            }
        }
    }
}



// check if the new score is high enough to enter the top 5
void checkScores(byte player) {
    byte i = player + 5; // position 6-7 depending on player number
	byte j;

    if (scores[i] > scores[4]) {
        // drawHeader();     <--------------------    test
        locate(6, 10); printf(" GOOD SCORE PLAYER %d ", player+1);
        locate(6, 11); printf(" NAME?:               ");
        locate(14, 11);
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



int main(void) {
	byte i; // player 1-2

    // position X of the pits
    pitLeft[0] = 0;
    pitLeft[1] = 22;
	
    while(TRUE) {
        srand(getTimer()); // random seed
        init();		
		mainLoop();
		// check the scores of the last game
        newScore = FALSE;
        for (i = 0; i <= numPlayers; i++) {
            checkScores(i);
		}
        // draw the scoreboard
        if (newScore) {
		    cls(1);
		    drawHeader();
		    drawHighScores();
        }
    }
    return 0;
}
