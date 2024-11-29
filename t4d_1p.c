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
- simplificación por IA
- botón de pausa

*/

#include <cmoc.h>
#include <coco.h>

#define SCREEN_BASE 0x400 // base address of the video memory in text mode
#define SCREEN_WIDTH 32 // screen width in characters

#define EMPTY_BLOCK 128 // black block, no pixels
#define FILLED_BLOCK 143 // all pixels in the block are active
#define SIDE_BLOCK_COUNT 4 // size of the piece's side (piece = 4x4 blocks)
#define LAST_SIDE_BLOCK 3 // to iterate over the current piece (0 to 3)
#define BLOCK_COUNT 16 // size of the piece in blocks (piece = 4x4 blocks)
#define NO_BLOCK '0' // character representing an empty block
#define PIT_WIDTH 10 // width of the pit in blocks
#define PIT_HEIGHT 16 // height of the pit in blocks
#define LINES_LEVEL 10 // lines per level

char key = '\0';                // key pressed
BOOL newScore = FALSE;          // TRUE = redraws the best scores table
BOOL gameOver = FALSE; 			// FALSE = game in progress, TRUE = finished
int dropRate = 0; 			    // 0 = lower the piece one position
int startTime = 0;              // system ticks since the piece has moved
char pit[PIT_WIDTH * PIT_HEIGHT]; // content of each pit in blocks
unsigned char level = 1; 		// game levels (increases the speed)
int lines = 0; 				    // lines cleared
unsigned char shape = 255;      // piece type (0 to 6). 255 = piece not defined
unsigned char nextShape = 255;  // next piece type (0 to 6). 255 = piece not defined
unsigned char shapeAngle = 0; 	// piece rotation (0 to 3)
int shapeX = 0, shapeY = 0;		// piece XY position
char shapeMap[BLOCK_COUNT];      // piece design
char nextShapeMap[BLOCK_COUNT];  // next piece design
char rotatedMap[BLOCK_COUNT];    // design of the already rotated piece
unsigned char colourShift = 0;  // colour shift effect in the title
BOOL chequeredPit = TRUE;       // enables/disables the chequered pit (options menu)
BOOL autorepeatKeys = FALSE;    // enables/disables the auto-repeat of keys (options menu)

// pos 0-4: fake values for the initial TOP 5
// pos 5: values for the current game
char names[7][11] = {"DRAGON","DRAGON","DRAGON","DRAGON","DRAGON","DRAGON",""};
unsigned int scores[7] = {2000, 1800, 1600, 1400, 1200, 1000, 0};



void printBlock(int x, int y, unsigned char ch) {
    // calculates the memory address based on X and Y coordinates
    unsigned char *screenPos = (unsigned char *)(SCREEN_BASE + y * SCREEN_WIDTH + x);
    *screenPos = ch; // write character to video memory
}



void drawBlock(char blockColour, unsigned char pitX, unsigned char pitY) {
    // dragon semigraphic characters: 128 to 255  
    // +16:yellow +32:blue +48:red +64:white +80:cyan +96:magenta +112:orange 
    unsigned char colour = blockColour - NO_BLOCK; // (0 a 8)
    if (colour == 0) { // background
        if (chequeredPit) {
            locate(pitX, pitY);
            putchar(111); // "O" inverted
        } else
            printBlock(pitX, pitY, EMPTY_BLOCK);
        return;
    }
    printBlock(pitX, pitY, FILLED_BLOCK + ((colour - 1) << 4)); // <<4 = x16
}



void drawPitSeparator() {
    for (unsigned char y = 0; y < PIT_HEIGHT; y++)
        printBlock(PIT_WIDTH, y, EMPTY_BLOCK);
}



void drawPit() {
    // loop through and repaint the contents of the pit
    for (unsigned char pitY = 0; pitY < PIT_HEIGHT; pitY++) {
        unsigned char rowOffset = PIT_WIDTH * pitY;
        for (unsigned char pitX = 0; pitX < PIT_WIDTH; pitX++) {
            drawBlock(pit[rowOffset + pitX], pitX, pitY);
        }
    }
}



// check if a shape can move in the specified direction
BOOL shapeCanMove(char *map, char xDir, char yDir) {
    int pitX, pitY;
    // loop through all the blocks of the piece
    for (int blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
        for (int blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
            if (map[(SIDE_BLOCK_COUNT * blockY) + blockX] != NO_BLOCK) {
                // calculate the position in the pit
                pitX = (shapeX + blockX) + xDir;
                pitY = (shapeY + blockY) + yDir;
                if (pitY < 0) return TRUE; // new piece appears at the top
                // check if the block is within the pit boundaries
                if (pitX >= 0 && pitX < PIT_WIDTH && pitY < PIT_HEIGHT) {
                    // if the position in the pit is not empty, it cannot move
                    if (pit[(PIT_WIDTH * pitY) + pitX] != NO_BLOCK) {
                        return FALSE;
                    }
                } 
                // if it is out of bounds, it cannot move 
                else return FALSE;
            }
        }
    }
    return TRUE; // can move!
}



void drawNextShape() {
    // loop through all the blocks of the piece (4x4 grid)
    for (unsigned char blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
        for (unsigned char blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
            char blockColour = nextShapeMap[(SIDE_BLOCK_COUNT * blockY) + blockX];
            // determine the block type (1 = green, 5 = white)
            blockColour = (blockColour == NO_BLOCK) ? '1' : '5';
            drawBlock(blockColour, blockX + 23, blockY + 12);
        }
    }
}



void drawHeader(unsigned char x, unsigned char shift) {
    unsigned char colour;
    const unsigned char colours[] = {2, 3, 4, 5, 6, 7, 8}; // colours available, excluding green
    unsigned char colourCount = sizeof(colours) / sizeof(colours[0]);

    for (unsigned char pos = 0; pos < 15; pos++) {
        // colour for the top line (to the left).
        colour = colours[(pos + shift) % colourCount];
        printBlock(x + pos, 1, FILLED_BLOCK + ((colour - 1) << 4)); // <<4 = x16
        // colour for the bottom line (to the right)
        colour = colours[(pos + colourCount - shift) % colourCount];
        printBlock(x + pos, 3, FILLED_BLOCK + ((colour - 1) << 4));
    }
    locate(x, 2); printf("= T E T R I S =");
    locate(x - 1, 5); printf("SALVAKANTERO 2024");
}



void displayStatus(void) {
    if (gameOver == TRUE) {
        locate(0, 8);
        printf("GAME OVER!");
        return;
    }
    drawHeader(14, colourShift);
    locate(16, 7); printf("LEVEL:   %2u", level); // draws the level number
    locate(16, 8); printf("LINES:  %3d", lines); // draws the lines
    locate(16, 10); printf("SCORE:%5u", scores[6]); // draws the score
    locate(16, 11); printf("HIGH: %5u", scores[0]); // draws the high score
    locate(16, 13); printf("NEXT:");
    drawNextShape();
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
            return "0000444400000000"; // | red
        case 1:
            return "0000222000200000"; // _| yellow
        case 2:
            return "0000777070000000"; // |_ magenta
        case 3:
            return "0000033003300000"; // [] blue
        case 4:
            return "0000066066000000"; // S cyan
        case 5:
            return "0000880008800000"; // Z orange
        case 6:
            return "0000111001000000"; // T green
        default:
            return "";
    }
}



void getRotatedShapeMap(unsigned char shape, unsigned char angle, char *rotatedMap) {
    const char *map = getShapeMap(shape); // Unrotated map
    // clear the rotatedMap array
    memset(rotatedMap, NO_BLOCK, BLOCK_COUNT);
    // no rotation needed, copy map directly
    if (angle == 0) {
        strncpy(rotatedMap, map, BLOCK_COUNT);
        return;
    }
    // Perform rotation for other angles
    for (int i = 0; i < BLOCK_COUNT; i++) {
        int blockX = i % SIDE_BLOCK_COUNT;
        int blockY = i / SIDE_BLOCK_COUNT;

        int newBlockX = 0, newBlockY = 0;
        switch (angle) {
            case 1: // 270 degrees
                newBlockX = blockY;
                newBlockY = LAST_SIDE_BLOCK - blockX;
                break;
            case 2: // 180 degrees
                newBlockX = LAST_SIDE_BLOCK - blockX;
                newBlockY = LAST_SIDE_BLOCK - blockY;
                break;
            case 3: // 90 degrees
                newBlockX = LAST_SIDE_BLOCK - blockY;
                newBlockY = blockX;
                break;
        }
        rotatedMap[newBlockY * SIDE_BLOCK_COUNT + newBlockX] = 
            map[blockY * SIDE_BLOCK_COUNT + blockX];
    }
}



void createNextShape() {
	nextShape = (unsigned char) rand() % 7; // piece type (0 to 6)
    strncpy(nextShapeMap, getShapeMap(nextShape), BLOCK_COUNT);
}



void createShape() {
    /* calculates the fall speed based on the level
        level 1: 35 delay ticks
        level 2: 30   "
        level 3: 25   "
        level 4: 20   "
        level 5: 15   "
        level 6: 10   "
        level x:  5   "
    */
    dropRate = 40 - (level * 5);
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
    shapeX = 3; // centre of the pit
    shapeY = -LAST_SIDE_BLOCK; // fully hidden
    shapeAngle = 0; // unrotated
    strncpy(shapeMap, getShapeMap(shape), BLOCK_COUNT);
    // generates the next piece
    createNextShape();
}



void drawShape(BOOL eraseShape) {
    int pitX = 0, pitY = 0;
    int blockX = 0, blockY = 0;
    char blockColour = NO_BLOCK;
    // iterates through all the blocks of the piece
    for (blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
        for (blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
            pitX = shapeX + blockX;
            pitY = shapeY + blockY;
            // check if the block is within the pit boundaries
            if (pitX >= 0 && pitX < PIT_WIDTH && pitY >= 0 && pitY < PIT_HEIGHT) {
                if (eraseShape == TRUE) {
                    // gets the colour of the pit at the current position
                    blockColour = pit[(PIT_WIDTH * pitY) + pitX];
                } else {
                    // gets the colour of the piece's block
                    blockColour = shapeMap[(SIDE_BLOCK_COUNT * blockY) + blockX];
                    // if the block is empty, take the colour of the pit at that position
                    if (blockColour == NO_BLOCK) {
                        blockColour = pit[(PIT_WIDTH * pitY) + pitX];
                    }
                }
                // draw or erase the block
                drawBlock(blockColour, (unsigned char)pitX, (unsigned char)pitY); 
            }
        }
    }
}



void removeFullRow(unsigned char removedRow) {
    unsigned char pitX = 0, pitY = 0, i = 0;
    char blockColour = NO_BLOCK;

    // line selection effect
    for (pitX = 0; pitX < PIT_WIDTH; pitX++) {
        printBlock(pitX, removedRow, 207);
    }

    for(i = 0; i < 2; i++) {
        drawHeader(14, ++colourShift);
    }

    // iterate from the removed row upwards
    for (pitY = removedRow; pitY > 0; pitY--) {
        for (pitX = 0; pitX < PIT_WIDTH; pitX++) {
            if (pitY == 0) {
                blockColour = NO_BLOCK; // the topmost line becomes empty
            } else {
                // copy the top row to the current one
                blockColour = pit[(pitY - 1) * PIT_WIDTH + pitX];
            }
            // assign the colour to the current row
            pit[pitY * PIT_WIDTH + pitX] = blockColour;
        }
    }
    // clear the first row after shifting all rows down
    for (pitX = 0; pitX < PIT_WIDTH; pitX++) {
        pit[pitX] = NO_BLOCK;
    }
}



void checkForFullRows() { // searches for full rows
    BOOL fullRow = FALSE;
    int numLines = 0;
    unsigned char pitX = 0, pitY = 0;
    unsigned char j = 6;
    // loops through all the rows in the pit
    for (pitY = 0; pitY < PIT_HEIGHT; pitY++) {
        fullRow = TRUE;
        // checks each block in the row
        for (pitX = 0; pitX < PIT_WIDTH; pitX++) {
            if (pit[pitY * PIT_WIDTH + pitX] == NO_BLOCK) {
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
        level = (unsigned char)(lines / LINES_LEVEL) + 1;
    }
}



void settleActiveShapeInPit() {
    int blockX = 0, blockY = 0;
    int pitX = 0, pitY = 0;
    for (blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
        for (blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
            pitX = shapeX + blockX;
            pitY = shapeY + blockY;
            // checks that the block is within the pit boundaries
            if (pitX >= 0 && pitX < PIT_WIDTH && pitY >= 0 && pitY < PIT_HEIGHT) {
                // checks that the block is not an empty space
                if (shapeMap[blockY * SIDE_BLOCK_COUNT + blockX] != NO_BLOCK) {
                    // fixes the piece's block in the pit
                    pit[pitY * PIT_WIDTH + pitX] = shapeMap[blockY * SIDE_BLOCK_COUNT + blockX];
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
        // 1 point for each line in rapid drop
        if (dropRate == 0) {
            scores[6]++;
        }
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
	unsigned char pos = 0; // TOP 5 position (0-4)
    for(pos = 0; pos < 6; pos++) {
        locate(7, 7+pos);  printf("...............");
        locate(7, 7+pos);  printf("%s", names[pos]);
        locate(19, 7+pos); printf("%5u", scores[pos]);
	}
	locate(2, 14); printf("PRESS ANY KEY TO CONTINUE...");
    do {
        if (inkey() != '') {
            break;
        }
        drawHeader(8, colourShift++);
        delay(2);
    }
	while (TRUE);
}



void drawOptionsMenu() {
	cls(1);
	locate(4, 8);  printf("1)  AUTOREPEAT KEYS:");
    locate(4, 9);  printf("2)  CHEQUERED PIT:");
    locate(4, 10); printf("3)  BACK");
    locate(7, 14); printf("SELECT OPTION (1-3)");
    // on/off switches
    locate(25, 8);
    if (autorepeatKeys == TRUE) { printf("on "); }
    else { printf("off"); }
    locate(25, 9);
    if (chequeredPit == TRUE) { printf("on "); }
    else { printf("off"); }
}



void optionsMenu() {
    drawOptionsMenu();

    do {
        drawHeader(8, colourShift++);
        key = inkey();		
        if (key == '1')	{
            if (autorepeatKeys == TRUE) {
                autorepeatKeys = FALSE;
            } else { autorepeatKeys = TRUE; }
            drawOptionsMenu();
        }
        else if (key == '2') {	
            if (chequeredPit == TRUE) {
                chequeredPit = FALSE;
            } else { chequeredPit = TRUE; }
            drawOptionsMenu();
        }
		else if (key == '3') { // back	
            break;
        } 
        delay(2);
    } while (TRUE);
}



void drawMenu() {
	cls(1);
	locate(8, 8);  printf("1)  START GAME");
    locate(8, 9);  printf("2)  OPTIONS");
    locate(8, 10); printf("3)  HIGH SCORES");
    locate(8, 11); printf("4)  EXIT");
    locate(7, 14); printf("SELECT OPTION (1-4)");
}



void menu() {
	drawMenu();

    do {
        drawHeader(8, colourShift++);
        key = inkey();		
        if (key == '1')	{ // start game
            break;
        }
        else if (key == '2') { // options menu		
            optionsMenu();
			drawMenu();
        }
		else if (key == '3') { // high scores		
            drawHighScores();
			drawMenu();
        } 
		else if (key == '4') { // exit
			cls(1);
            printf("THANKS FOR PLAYING T4D!\n");
            exit(0);
		}
        delay(2);
    } while (TRUE);
}



// logic to initialize the system and pits
void init() {
    setTimer(0);
    menu();
	cls(1); // green screen

    // rounds the text window
    printBlock(PIT_WIDTH+1, 0, 129); // upper left corner
    printBlock(SCREEN_WIDTH-1, 0, 130); // upper right corner
    printBlock(PIT_WIDTH+1, PIT_HEIGHT-1, 132); // bottom left corner
    printBlock(SCREEN_WIDTH-1, PIT_HEIGHT-1, 136); // bottom right corner

    gameOver = FALSE; // game in progress
    level = 1; // initial level
    lines = 0; // lines cleared
    scores[6] = 0; // current score
    nextShape = 255; // piece generation will be required
    memset(pit, NO_BLOCK, PIT_WIDTH * PIT_HEIGHT); // initialize the empty pit
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
        key = '\0';
        // loop until a key is pressed
        while (key == '\0') {
            if (gameOver == FALSE) { // game in progress
                // if the falling time has been exceeded
                if (getTimer() >= startTime + dropRate) {
                    dropShape(); // the piece moves down
                    startTime = getTimer(); // reset the fall timer
                }
            }
            key = inkey(); // read keypresses
                        
            // auto-repeat
            if (autorepeatKeys == TRUE) {
                for (unsigned char i = 0; i <= 9; i++) {
                    *((unsigned char *)0x0150 + i) = 0xFF;
                }
                // waits 2ms for rotation and 1ms for moving the piece
                delay(1);
                if (key == 'W') { delay(1); }
            }
        }

        if (key == 'X') { // if X is pressed, exit to the main menu
            break;
        } else {
            if (gameOver == FALSE) { // game in progress
                switch (key) {
                    case 'W': // rotate key                      
                        if (shapeAngle == 3) {
                            // 270 degrees. Reset angle
                            newAngle = 0;
                        }
                        else {
                            // increases the angle by 90 degrees
                            newAngle = shapeAngle + 1;
                        }
                        getRotatedShapeMap(shape, newAngle, rotatedMap);
                        if (shapeCanMove(rotatedMap, 0, 0)) {
                            shapeAngle = newAngle;
                            drawShape(TRUE); // erase piece                            
                            strncpy(shapeMap, rotatedMap, BLOCK_COUNT);
                            drawShape(FALSE);
                        }                         
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



// check if the new score is high enough to enter the top 6
void checkScores() {
    int j; // indices: 0-1-2-3-4-5-[current]
    if (scores[6] > scores[5]) {
        // clear part of the screen
        for (j = 10; j < 16; j++) {
            locate(12, (unsigned char)j);
            printf("                   ");
        }
        locate(15, 12); printf("GOOD SCORE!!");
        locate(15, 13); printf("NAME?:");
        locate(14, 20);

        char *response = readline();
        strncpy(names[6], response, 10);
        names[6][10] = '\0'; // ensure the name is null-terminated

        // find the correct position in the top 5 list for the new score
        for (j = 5; j >= 0; j--) {
            if (scores[6] > scores[j]) {
                // shift scores/names down
                if (j < 5) {
                    scores[j+1] = scores[j];
                    strncpy(names[j+1], names[j], 10);
                }
            } else {
                break;
            }
        }
        // insert the new score and name into the correct position
        scores[j+1] = scores[6];
        strncpy(names[j+1], names[6], 10);
        newScore = TRUE; // the score table has been updated
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
		    drawHeader(8, colourShift);
		    drawHighScores();
        }
    }
    return 0;
}
