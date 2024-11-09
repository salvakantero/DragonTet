/*

*** TETRIS4DRAGONS ***
   salvaKantero2024

Compatible with Dragon 32/64 and COCO

Based on Peter Swinkels' PC Qbasic code. (QBBlocks v1.0)

use the CMOC compiler 0.1.89 or higher
"cmoc --dragon -o t4d.bin t4d.c"


TODO
====
- línea 16 con 2 players
- desplazamiento izda.
- timing

*/

#include <cmoc.h>
#include <coco.h>

#define SIDEBLOCKCOUNT 4 // size of the piece's side (4x4)
#define LASTSIDEBLOCK SIDEBLOCKCOUNT - 1 // to iterate over the current piece (0 to 3)
#define BLOCKCOUNT SIDEBLOCKCOUNT * SIDEBLOCKCOUNT // size of the piece in blocks
#define NOBLOCK '0' // character representing an empty block
#define PITWIDTH 10 // width of the pit in blocks
#define PITHEIGHT 16 // height of the pit in blocks

char key; // key pressed
unsigned char numPlayers; // 0 = player1  1 = player2
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

char shape[2]; 		            // piece type (-1 to 6)
unsigned char shapeAngle[2]; 	// piece rotation (0 to 3)
char* shapeMap[2];				// piece design
int shapeX[2], shapeY[2];		// piece XY position

char nextShape[2];		        // type of the next piece (-1 to 6)
char* nextShapeMap[2];		    // design of the next piece



void drawBlock(char blockColour, unsigned char pitX, unsigned char pitY, unsigned char i) {
    unsigned char colour = blockColour-'0'; // (0 to 8)
    locate(pitX + pitLeft[i], pitY);
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
    // black background (empty block)
    if (colour == 0) {
        printf("%c", 128);
        return;
    }
    // coloured filled block
    printf("%c", 143 + ((colour-1) * 16));
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
    // loop through all the blocks of the piece
    for (blockX = 0; blockX <= LASTSIDEBLOCK; blockX++) {
        for (blockY = 0; blockY <= LASTSIDEBLOCK; blockY++) {
            // piece colour
            blockcolour = nextShapeMap[i][(SIDEBLOCKCOUNT*blockY)+blockX];
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
    locate(12, 1); printf("LEVEL: %u", level[0]); // draws the level number
    locate(12, 2); printf("LINES: %u", lines[0]); // draws the lines
    locate(12, 3); printf("SC: %4d", scores[5]); // draws the score
    locate(12, 4); printf("NEXT:");

    // player 2
    if (numPlayers == 1) {
        if (gameOver[1] == TRUE) {
			locate(pitLeft[1], 8);
			printf("GAME OVER!");
        }
        locate(11, 8); printf("=PLAYER 2=");
        locate(12, 9); printf("LEVEL: %u", level[1]); // draws the level number
        locate(12, 10); printf("LINES: %u", lines[1]); // draws the lines
        locate(12, 11); printf("SC: %4d", scores[6]); // draws the score
        locate(12, 12); printf("NEXT:");
    }
    
    unsigned char i;
    for (i = 0; i <= numPlayers; i++) {
        if (gameOver[i] == FALSE) {
			drawNextShape(i);
        }
    }
}



const char* getShapeMap(unsigned char shape) {
    switch (shape) {
        case 0:
            return "0000444400000000"; // red |
        case 1:
            return "0000222000200000"; // yellow _|
        case 2:
            return "0000777070000000"; // magenta |_
        case 3:
            return "0000033003300000"; // blue []
        case 4:
            return "0000066066000000"; // cyan S
        case 5:
            return "0000880008800000"; // orange Z
        case 6:
            return "0000555005000000"; // white T
        default:
            return "";
    }
}



// rotates the piece and returns the resulting map
char* getRotatedShapeMap(char shape, unsigned char angle) {
	const char* map = getShapeMap(shape); // unrotated map
	char* rotatedMap; // rotated map
    int newBlockX, newBlockY;
	unsigned char i;

    // if the angle is 0, copy the original map directly into rotatedMap
    if (angle == 0) {
        for (i = 0; i < BLOCKCOUNT; i++) {
            rotatedMap[i] = map[i];                 // <----------------- necessary??
        }
        return rotatedMap;
    }
    // initialize rotatedMap as empty
    for (i = 0; i < BLOCKCOUNT; i++) {
        rotatedMap[i] = NOBLOCK;               // <----------------- necessary??
    }
    // for other angles, iterate through all blocks
    for (int blockX = 0; blockX <= LASTSIDEBLOCK; blockX++) {
        for (int blockY = 0; blockY <= LASTSIDEBLOCK; blockY++) {
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
            rotatedMap[SIDEBLOCKCOUNT * newBlockY + newBlockX] = map[SIDEBLOCKCOUNT * blockY + blockX];
        }
    }
	return rotatedMap;
}



void createNextShape(unsigned char i) {
	nextShape[i] = (unsigned char)(rand() % 7); // piece type (0 to 6)
	nextShapeMap[i] = getRotatedShapeMap(nextShape[i], 0); // piece composition  // <---- necessary?
}



void createShape(unsigned char i) {
	// calculates the fall speed based on the level
    dropRate[i] = 1-(level[i] * 0.2f);
    if (dropRate[i] <= 0) {
        dropRate[i] = 0.1f;  // minimum fall speed limit
    }
    // if it's not the first piece, take the value of nextShape
    if (nextShape[i] >= 0) {
        shape[i] = nextShape[i];
    } else {
        shape[i] = (unsigned char)rand() % 7;  // new random piece (0 to 6)
    }
    shapeAngle[i] = 0;
    shapeMap[i] = getRotatedShapeMap(shape[i], shapeAngle[i]);           // <-------- necessary?
    // initial position (centre of the pit in X, fully hidden in Y)
    shapeX[i] = 3;
    shapeY[i] = -SIDEBLOCKCOUNT;
    // generates the next piece
    createNextShape(i);
}



void drawShape(unsigned char i, unsigned char eraseShape) {
    int pitX, pitY;
    unsigned char blockX, blockY;
    char blockColour;

    // iterates through all the blocks of the piece
    for (blockX = 0; blockX <= LASTSIDEBLOCK; blockX++) {
        for (blockY = 0; blockY <= LASTSIDEBLOCK; blockY++) {
            pitX = shapeX[i] + blockX;
            pitY = shapeY[i] + blockY;
            // check if the block is within the pit boundaries
            if (pitX >= 0 && pitX < PITWIDTH && pitY >= 0 && pitY < PITHEIGHT) {
                if (eraseShape) {
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
                drawBlock(blockColour, (unsigned char)pitX, (unsigned char)pitY, i); 
            }
        }
    }
}



void removeFullRow(unsigned char removedRow, unsigned char i) {
    unsigned char pitX, pitY;
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



void checkForFullRows(unsigned char i) { // searches for full rows
    unsigned char fullRow = FALSE;
    unsigned int numLines = 0;
    unsigned char pitX, pitY;
    unsigned char j = i + 5;

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
                scores[j] += (100 * level[i]);
                break;
            case 2:
                scores[j] += (300 * level[i]);
                break;
            case 3:
                scores[j] += (500 * level[i]);
                break;
            case 4:
                scores[j] += (800 * level[i]); // Tetris!
                break;
        }
        // updates the total completed rows and calculates the level
        lines[i] += numLines;
        level[i] = (unsigned char)(lines[i]/10)+1;
    }
}



void settleActiveShapeInPit(unsigned char i) {
    unsigned char blockX, blockY;
    int pitX, pitY;
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



void dropShape(unsigned char i) {
    // checks if the piece can move down
    if (shapeCanMove(shapeMap[i], 0, 1, i)) {
        drawShape(i, TRUE);      // erases the current piece
        shapeY[i] += 1;          // moves the piece down by one position
        drawShape(i, FALSE);     // redraws the piece at the new position
    } else {
        settleActiveShapeInPit(i);
        // checks if the piece has reached the top and the game is lost
        gameOver[i] = (shapeY[i] < 0);

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
	unsigned char i;
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
	unsigned char i;
    menu();
	cls(1);
    pitLeft[0] = 0;
    pitLeft[1] = 22;
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
    unsigned char newAngle;
    unsigned char i;
    char* rotatedMap;

    // save the start time for player 1
    //startTime[0] = getTimer();
    //if (numPlayers > 0) {
        // save the start time for player 1
        //startTime[1] = getTimer();
    //}
    while (TRUE) {
        char key = '\0';
        // loop until a key is pressed
        while (key == '\0') {
            for (i = 0; i <= numPlayers; i++) {
                if (!gameOver[i]) { // game in progress
                    // if the falling time has been exceeded
                    //if (getTimer() >= startTime[i] + dropRate[i] || startTime[i] > getTimer()) {
                        dropShape(i); // the piece moves down
                        //startTime[i] = getTimer(); // reset the fall timer
                    //}
                }
            }
            key = inkey(); // read keypresses
        }

        if (key == 'X') { // if X is pressed, exit to the main menu
            break;
        } else {
            for (i = 0; i <= numPlayers; i++) {
                /*
                if (gameOver[i]) { // out of game
                    if (key == 'Z') { // if ENTER is pressed, restart game
                        break;
                    }
                } else { */
                if (!gameOver[i]) {
                    // assign keys for each player
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
                        case 'W': case 'I': // rotate key
                            drawShape(i, TRUE); // erase piece
                            newAngle = (shapeAngle[i] == 3) ? 0 : shapeAngle[i]+1;
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
void checkScores(unsigned char player) {
    unsigned char i = player + 5;
	unsigned char j;

    if (scores[i] > scores[4]) {
        drawHeader();
        locate(3, 10); printf("GOOD SCORE PLAYER %d", player+1);
        locate(6, 11); printf("NAME?: ");
        char *response = readline();
        strncpy(names[i], response, 10);
        names[i][10] = '\0';

        for (j = 4; j >= 0; j--) {
            if (scores[i] > scores[j]) {
                // shift scores and names down
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
        srand(getTimer()); // random seed
        init();		
		mainLoop();
		
        newScore = FALSE;
        for (i = 0; i <= numPlayers; i++) {
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
