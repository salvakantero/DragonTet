/*

= DRAGONTET =
salvaKantero 2025

Compatible with Dragon 32/64 and COCO 1/2/3
Based on Peter Swinkels' PC Qbasic code. (QBBlocks v1.0)

use the CMOC compiler 0.1.89 or higher:
"cmoc --dragon -D Dragon -o dtet-dr.bin dtet.c"
"cmoc --coco -D Coco -o dtet-cc.bin dtet.c"
use xroar to test (dragon):
"xroar -run dtet-dr.bin"

level description:
level 1: 35 delay ticks
level 2: 30 delay ticks
level 3: 25 delay ticks + lower trap lines
level 4: 20 delay ticks + lower trap lines
level 5: 15 delay ticks + upper trap blocks
level 6: 10 delay ticks + upper trap blocks
level x:  5 delay ticks + upper trap blocks


TODO
====
- control de teclado para Dragon

- implementar líneas trampa
- pantalla de ayuda puntuaciones si < 16kb

- BUG puntos nivel 3+
*/


#include <cmoc.h>
#include <coco.h>

#define SCREEN_BASE 0x400 // base address of the video memory in text mode
#define SCREEN_WIDTH 32 // screen width in characters

#define EMPTY_BLOCK 128 // black block, no active pixels
#define FILLED_BLOCK 143 // all pixels in the block are active
#define WHITE_BLOCK 207 // to mark a completed line

#define SIDE_BLOCK_SIZE 4 // size of the shape's side (shape = 4x4 blocks)
#define LAST_SIDE_BLOCK 3 // to iterate over the current shape (0 to 3)
#define BLOCK_SIZE 16 // size of the shape in blocks (shape = 4x4 blocks)
#define NO_BLOCK '0' // character representing an empty block
#define PIT_WIDTH 10 // width of the pit in blocks
#define PIT_HEIGHT 16 // height of the pit in blocks
#define LINES_LEVEL 10 // lines per level
#define INPUT_DELAY 6 // delay for processing inputs

char key = '\0'; // key pressed
unsigned char numPlayers; // 0 = dragon1  1 = dragon2
BOOL newScore[2] = {FALSE, FALSE}; // TRUE = redraws the best scores table
BOOL gameOver[2] = {FALSE, FALSE}; // FALSE = game in progress, TRUE = finished
int dropRate[2] = {0, 0}; // 0 = lower the shape one position
int startTime[2] = {0, 0}; // system ticks since the shape has moved
char pit[2][PIT_WIDTH * PIT_HEIGHT]; // content of each pit in blocks
unsigned char level[2] = {1, 1}; // game levels (increases the speed)
int lines[2] = {0, 0}; // lines cleared
const unsigned char pitLeft[2] = {0, 22}; // X position of the left side of each pit
unsigned char shape[2] = {255, 255}; // shape type (0 to 6). 255 = shape not defined
unsigned char nextShape[2] = {255, 255}; // next shape type (0 to 6). 255 = shape not defined
unsigned char shapeAngle[2] = {0, 0}; // shape rotation (0 to 3)
int shapeX[2] = {0, 0}, shapeY[2] = {0, 0};	// shape XY position
char shapeMap[2][BLOCK_SIZE]; // shape design
char nextShapeMap[2][BLOCK_SIZE]; // next shape design
char rotatedMap[2][BLOCK_SIZE]; // design of the already rotated shape
unsigned char colourShift = 0; // colour shift effect in the title
BOOL emptyBackground = FALSE; // enables/disables the chars in the pit (options menu)
BOOL muted = FALSE; // enables/disables the sound effects and tunes
BOOL cancelled = FALSE; // TRUE = 'X' was pressed to cancel the game
unsigned char lastLines; // lines in the last move (for the status line)
int lastPoints; // points in the last move (for the status line)
unsigned char backgroundChar[2]; // character to fill in the pits with backgroundCharList
unsigned char backgroundCharList[8] = {42, 43, 39, 35, 28, 44, 47, 44 }; // * + . # \ , / .
unsigned int lastInputTime[2][3]; // for each player and action (rotate, left, right) 
unsigned char numPiecesPlayed; // allows trap lines/blocks to be generated (1 player)
unsigned char previousLevel[2]; // previous level for each player

// pos 0-5: fake values for the initial TOP 6
// pos 6-7: values for the current game
char names[8][11] = {"DRAGON","DRAGON","DRAGON","DRAGON","DRAGON","DRAGON","", ""};
unsigned int scores[8] = {2000, 1800, 1600, 1400, 1200, 1000, 0, 0};

// tunes
// hall of fame
unsigned char tune1Notes[] = { 201, 201, 201, 208, 195, 210, 218 };
unsigned char tune1Durations[] = { 4, 2, 2, 3, 3, 3, 7 };
// start of game
unsigned char tune2Notes[] = { 191, 200, 210, 216, 200, 180, 190, 200 };
unsigned char tune2Durations[] = { 2, 2, 2, 2, 4, 2, 2, 4 };
// game over
unsigned char tune3Notes[] = { 165, 140, 155, 135, 150, 130, 140, 120, 110, 100 };
unsigned char tune3Durations[] = { 3, 1, 3, 1, 3, 1, 2, 1, 2, 4 };


void playTune(unsigned char notes[], unsigned char durations[], unsigned char numNotes) {
    if (muted) return;
    for (unsigned char i = 0; i < numNotes; i++) {
        if (inkey() != '\0') return;
        sound(notes[i], durations[i]);
        if (inkey() != '\0') return;
    }
}


void printBlock(int x, int y, unsigned char ch) {
    // calculates the memory address based on X and Y coordinates
    unsigned char *screenPos = (unsigned char *)(SCREEN_BASE + y * SCREEN_WIDTH + x);
    *screenPos = ch; // write character to video memory
}


void drawBlock(unsigned char x, unsigned char y, char blockColour, unsigned char i) {
    // dragon semigraphic characters: 128 to 255  
    // +16:yellow +32:blue +48:red +64:white +80:cyan +96:magenta +112:orange 
    unsigned char colour = blockColour - NO_BLOCK; // (0 a 8)
    x += pitLeft[i];
    if (colour == 0) { // background
        if (!emptyBackground)
            printBlock(x, y, backgroundChar[i]);
        else
            printBlock(x, y, EMPTY_BLOCK);
        return;
    }
    printBlock(x, y, FILLED_BLOCK + ((colour - 1) << 4)); // <<4 = x16
}


void drawPit(unsigned char i) {
    unsigned char x, y, rowOffset;
    // loop through and repaint the contents of the pit (i = dragon pit)
    for (y = 0; y < PIT_HEIGHT; y++) {
        rowOffset = y * PIT_WIDTH;
        for (x = 0; x < PIT_WIDTH; x++)
            drawBlock(x, y, pit[i][x + rowOffset], i);
    }
}


// check if a shape can move in the specified direction (i = dragon pit)
BOOL shapeCanMove(char *map, char xDir, char yDir, unsigned char i) {
    int x, y, blockX, blockY;
    // loop through all the blocks of the shape
    for (blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
        for (blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
            if (map[(SIDE_BLOCK_SIZE * blockY) + blockX] != NO_BLOCK) {
                // calculate the position in the pit
                x = (shapeX[i] + blockX) + xDir;
                y = (shapeY[i] + blockY) + yDir;
                // new shape appears at the top
                if (y < 0) // Allow only vertical movement until fully visible
                    return (xDir == 0);
                // check if the block is within the pit boundaries
                if (x >= 0 && x < PIT_WIDTH && y < PIT_HEIGHT) {
                    // if the position in the pit is not empty, it cannot move
                    if (pit[i][(y * PIT_WIDTH) + x] != NO_BLOCK)
                        return FALSE;
                }
                // if it is out of bounds, it cannot move 
                else return FALSE;
            }
        }
    }
    return TRUE; // can move!
}


void drawNextShape(unsigned char y, unsigned char i) {
    unsigned char blockX, blockY;
    char blockColour;
    // loop through all the blocks of the shape (4x4 grid)
    for (blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
        for (blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
            blockColour = nextShapeMap[i][(blockY * SIDE_BLOCK_SIZE) + blockX];
            // green background
            if (blockColour == NO_BLOCK) blockColour = '1';
            // different position depending on the player
            if (i == 0) drawBlock(blockX + 17, blockY + y, blockColour, i);
            else drawBlock(blockX - 5, blockY + y, blockColour, i);
        }
    }
}


void drawHeader(BOOL ingame, unsigned char shift) {
    unsigned char colour;
    const unsigned char colours[] = {2, 3, 4, 5, 6, 7, 8}; // colours available, excluding green
    unsigned char colourCount = sizeof(colours) / sizeof(colours[0]);
    unsigned char x = ingame ? 11 : 8;
    unsigned char y = ingame ? 2 : 1;
    unsigned char width = ingame ? 10 : 17;

    for (unsigned char pos = 0; pos < width; pos++) {
        // colour for the top line (to the left).
        colour = colours[(pos + shift) % colourCount];
        printBlock(x + pos, y, FILLED_BLOCK + ((colour - 1) << 4)); // <<4 = x16
        // colour for the bottom line (to the right)
        colour = colours[(pos + colourCount - shift) % colourCount];
        printBlock(x + pos, y+2, FILLED_BLOCK + ((colour - 1) << 4));
    }
    if (ingame) {
        locate(x, y+1); printf("DRAGONTET!");
    }
    else {
        locate(x, y+1); printf("D R A G O N T E T");
        locate(x, 5); printf("SALVAKANTERO 2025");
    }
}


void displayStatus() {
    unsigned char i;
    for (i = 0; i <= numPlayers; i++) {
        if (gameOver[i]) {
            locate(pitLeft[i], 8);
            printf("GAME OVER!");
            playTune(tune3Notes, tune3Durations, 10);
        }
    }
    if (numPlayers == 0) {        
        drawHeader(TRUE, ++colourShift);        
        locate(11, 6);  printf("LEVEL:  %2u", level[0]);
        locate(11, 7); printf("LINES: %3d", lines[0]);
        locate(11, 8); printf("SC: %6u", scores[6]);
        locate(11, 9); printf("HI: %6u", scores[0]);
        locate(11, 11); printf("NEXT:");
        drawNextShape(10, 0);
        // status line
        locate(11, 14); 
        if (lastLines > 0)       printf("%u LIN +%3d ", lastLines, lastPoints);
        else if (lastPoints > 0) printf("DROP   +%2d ", lastPoints);
        else                     printf("           ");
        lastLines = 0;
        lastPoints = 0;
    }
    else {
        // player 1
        locate(12, 0); printf("DRAGON 1");
        locate(11, 1); printf("LEVEL:  %2u", level[0]);
        locate(11, 2); printf("LINES: %3d", lines[0]);
        locate(11, 3); printf("SC: %6u", scores[6]);
        locate(11, 4); printf("NEXT:");
        // player 2
        locate(12, 8);  printf("DRAGON 2");
        locate(11, 9);  printf("LEVEL:  %2u", level[1]);
        locate(11, 10); printf("LINES: %3d", lines[1]);
        locate(11, 11); printf("SC: %6u", scores[7]);
        locate(11, 12); printf("NEXT:");
        drawNextShape(4, 0);
        drawNextShape(12, 1);
    }
}


const char* getShapeMap(unsigned char shape) {
    static const char* shapeMaps[] = {
        "0000444400000000", // | red
        "0000222000200000", // _| yellow
        "0000777070000000", // |_ magenta
        "0000033003300000", // [] blue
        "0000066066000000", // S cyan
        "0000880008800000", // Z orange
        "0000555005000000"  // T white
    };
    return shapeMaps[shape];
}score


void getRotatedShapeMap(unsigned char shape, unsigned char angle, char *rotatedMap) {
    const char *map = getShapeMap(shape); // Unrotated map
    // no rotation needed, copy map directly
    if (angle == 0) {
        strncpy(rotatedMap, map, BLOCK_SIZE);
        return;
    }
    // clear the rotatedMap array
    memset(rotatedMap, NO_BLOCK, BLOCK_SIZE);
    // Perform rotation for other angles
    for (int i = 0; i < BLOCK_SIZE; i++) {
        int blockX = i % SIDE_BLOCK_SIZE;
        int blockY = i / SIDE_BLOCK_SIZE;
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
        rotatedMap[newBlockY * SIDE_BLOCK_SIZE + newBlockX] = 
            map[blockY * SIDE_BLOCK_SIZE + blockX];
    }
}


void createNextShape(unsigned char i) {
	nextShape[i] = (unsigned char) rand() % 7; // shape type (0 to 6)
    strncpy(nextShapeMap[i], getShapeMap(nextShape[i]), BLOCK_SIZE);
}


void createShape(unsigned char i) {
    // calculates the fall speed based on the level
    dropRate[i] = (level[i] < 7) ? (40 - (level[i] * 5)) : 5;
    // if it's not the first shape, take the value of nextShape
    if (nextShape[i] != 255)
        shape[i] = nextShape[i];
    else // new random shape (0 to 6)
        shape[i] = (unsigned char) rand() % 7;
       
    shapeX[i] = 3; // centre of the pit
    shapeY[i] = -LAST_SIDE_BLOCK; // fully hidden
    shapeAngle[i] = 0; // unrotated
    strncpy(shapeMap[i], getShapeMap(shape[i]), BLOCK_SIZE);
    // generates the next shape
    createNextShape(i);
}


void drawShape(BOOL eraseShape, unsigned char i) {
    int x, y, blockX, blockY;
    char blockColour;
    // iterates through all the blocks of the shape
    for (blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
        for (blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
            x = shapeX[i] + blockX;
            y = shapeY[i] + blockY;
            // check if the block is within the pit boundaries
            if (x >= 0 && x < PIT_WIDTH && y >= 0 && y < PIT_HEIGHT) {
                if (eraseShape) {
                    // gets the colour of the pit at the current position
                    blockColour = pit[i][y * PIT_WIDTH + x];
                } else {
                    // gets the colour of the shape's block
                    blockColour = shapeMap[i][blockY * SIDE_BLOCK_SIZE + blockX];
                    // if the block is empty, take the colour of the pit at that position
                    if (blockColour == NO_BLOCK)
                        blockColour = pit[i][y * PIT_WIDTH + x];
                }
                // draw or erase the block
                drawBlock((unsigned char)x, (unsigned char)y, blockColour, i); 
            }
        }
    }
}


void removeFullRow(unsigned char removedRow, unsigned char i) {
    unsigned char x, y;
    char blockColour;

    // line selection effect
    for (x = 0 + pitLeft[i]; x < PIT_WIDTH + pitLeft[i]; x++)
        printBlock(x, removedRow, WHITE_BLOCK);

    // fx
    if (!muted)
        for (unsigned char j = 255; j > 60; j = j-5)
            sound(j,0);

    // iterate from the removed row upwards
    for (y = removedRow; y > 0; y--) {
        for (x = 0; x < PIT_WIDTH; x++) {
            // copy the top row to the current one
            blockColour = pit[i][(y - 1) * PIT_WIDTH + x];
            pit[i][y * PIT_WIDTH + x] = blockColour;
        }
    }
    // clear the first row after shifting all rows down
    for (x = 0; x < PIT_WIDTH; x++)
        pit[i][x] = NO_BLOCK;
}


void setTrapLine(unsigned char i) {

}


void setTrapBlock(unsigned char i) {
    int trapX, trapY;
    unsigned char rowEmpty;
    unsigned char attempts = 5;

    // with two players generates block in the opposing pit
    if (numPlayers == 1)
        i = (i == 0) ? 1 : 0;

    while (attempts > 0) {
        // generate a random position from the sixth row downwards
        trapY = 5 + rand() % (PIT_HEIGHT - 5);
        // check if the row is empty
        rowEmpty = TRUE;
        for (trapX = 0; trapX < PIT_WIDTH; trapX++)
            if (pit[i][trapY * PIT_WIDTH + trapX] != NO_BLOCK) {
                rowEmpty = FALSE;
                break;
            }
        // we find an empty row, we place the block
        if (rowEmpty) {
            trapX = rand() % PIT_WIDTH;
            pit[i][trapY * PIT_WIDTH + trapX] = 1; // green block
            // paints the block at the moment when it goes to the opposite pit
            if (numPlayers == 1) 
                printBlock(trapX + pitLeft[i], trapY, FILLED_BLOCK);
            if (!muted) sound(1,1);
            return;
        }
        attempts--;
    }
}


void checkForFullRows(unsigned char i) { // searches for full rows
    BOOL fullRow;
    unsigned char numLines = 0;
    unsigned char x, y;
    unsigned char j = i + 6; // indices: 0-1-2-3-4-5-[score p1]-[score p2]
    // loops through all the rows in the pit
    for (y = 0; y < PIT_HEIGHT; y++) {
        fullRow = TRUE;
        // checks each block in the row
        for (x = 0; x < PIT_WIDTH; x++) {
            if (pit[i][y * PIT_WIDTH + x] == NO_BLOCK) {
                fullRow = FALSE; // finds an empty space
                break;
            }
        }
        // if the row is full, it gets removed
        if (fullRow) {
            removeFullRow(y, i);
            numLines++;
        }
    }
    // updates the score if rows were completed
    if (numLines > 0) {
        switch (numLines) {
            case 1: lastPoints = (100 * level[i]); break;
            case 2: lastPoints = (300 * level[i]); break;
            case 3: lastPoints = (500 * level[i]); break;
            case 4: lastPoints = (800 * level[i]);
        }
        // updates the total completed rows and calculates the level
        scores[j] += lastPoints;
        lines[i] += numLines;
        lastLines = numLines; // data for status line
        level[i] = (unsigned char)(lines[i] / LINES_LEVEL) + 1;

        // if it detects level change...
        if (previousLevel[i] < level[i] && !muted) {
            // level background char
            if (!emptyBackground && level[i] < 9)
                backgroundChar[i] = backgroundCharList[level[i]-1];
            // level change sound
            sound(230, 1); sound(220, 2);
            previousLevel[i] = level[i];
        }
        
        // 2 players and more than 1 line in a row
        // generates a trap line/block
        if (numPlayers > 0 && numLines > 1) {
            if (level[i] > 4) setTrapBlock(i);
            else if(level[i] > 2) setTrapLine(i);
        }
    }
    // player 1 every x pieces generates a trap line/block
    if (numPlayers == 0 && numPiecesPlayed == 10) {
        if (level[0] > 4) setTrapBlock(0);
        else if(level[0] > 2) setTrapLine(0);
        numPiecesPlayed = 0;
    }
}


void settleActiveShapeInPit(unsigned char i) {
    int x, y, blockX, blockY;
    char blockColour;
    for (blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
        for (blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
            x = shapeX[i] + blockX;
            y = shapeY[i] + blockY;
            blockColour = shapeMap[i][blockY * SIDE_BLOCK_SIZE + blockX];
            // checks that the block is within the pit boundaries and not an empty space
            if (blockColour != NO_BLOCK && x >= 0 && x < PIT_WIDTH && y >= 0 && y < PIT_HEIGHT)
                // locks the shape's block in the pit
                pit[i][y * PIT_WIDTH + x] = blockColour;
        }
    }
    if (numPlayers == 0)
        numPiecesPlayed++;
}


void dropShape(unsigned char i) {
    // checks if the shape can move down
    if (shapeCanMove(shapeMap[i], 0, 1, i)) {
        drawShape(TRUE, i);      // erases the current shape
        shapeY[i] += 1;          // moves the shape down by one position
        drawShape(FALSE, i);     // redraws the shape at the new position
        // 1 point for each line in rapid drop
        if (dropRate[i] == 0) {
            scores[6+i]++;
            lastPoints++; // data for status line
        }
    } else {
        settleActiveShapeInPit(i);
        if (!muted) sound(245, 0);
        // checks if the shape has reached the top (the game is lost)
        if (shapeY[i] < 0)
            gameOver[i] = TRUE;
        else {
            checkForFullRows(i);
            drawPit(i);
            createShape(i);
            drawShape(FALSE, i);
        }
        // displays the current game status, such as points or messages
        displayStatus();
    }
}


// Rounds the text window by drawing the corners
// +16:yellow +32:blue +48:red +64:white +80:cyan +96:magenta +112:orange 
void roundWindow(int ulx, int uly, int brx, int bry, unsigned char offset) {
    printBlock(ulx, uly, 129 + offset); // upper left corner
    printBlock(brx, uly, 130 + offset); // upper right corner
    printBlock(ulx, bry, 132 + offset); // bottom left corner
    printBlock(brx, bry, 136 + offset); // bottom right corner
}


void drawHighScores() {
    cls(1);
    roundWindow(0, 0, 31, 15, 80);  
    drawHeader(FALSE, colourShift++); 
	// TOP 6 position (0-5)
    for(unsigned char pos = 0; pos < 6; pos++) {
        locate( 8, 7+pos); printf(".............");
        locate( 7, 7+pos); printf("%s", names[pos]);
        locate(20, 7+pos); printf("%5u", scores[pos]);
	}
	locate(3, 14); printf("PRESS ANY KEY TO CONTINUE!");
    playTune(tune1Notes, tune1Durations, 7);
    while (TRUE) {
        if (inkey() != 0) break;
        drawHeader(FALSE, colourShift++);
        delay(2);
    }
}


void drawHelp() {
    cls(0);
    printf(                 "  =DRAGON 1=  ");
    locate(0, 1);  printf(  "              ");
    locate(0, 2);  printf(  " w ROTATE     ");
    locate(0, 3);  printf(  " s DROP       ");
    locate(0, 4);  printf(  " a MOVE LEFT  ");
    locate(0, 5);  printf(  " d MOVE RIGHT ");
    locate(0, 6);  printf(  "              ");
    locate(0, 7);  printf(  " + CURSORS    ");
    locate(0, 8);  printf(  " + JOYSTICK1  ");
    locate(0, 9);  printf(  "              ");

    locate(18, 0); printf(  "  =DRAGON 2=  ");
    locate(18, 1); printf(  "              ");
    locate(18, 2); printf(  " i ROTATE     ");
    locate(18, 3); printf(  " k DROP       ");
    locate(18, 4); printf(  " j MOVE LEFT  ");
    locate(18, 5); printf(  " l MOVE RIGHT ");
    locate(18, 6); printf(  "              ");
    locate(18, 7); printf(  " + JOYSTICK2  ");
    locate(18, 8); printf(  "              ");

    locate(0, 11); printf(" X = CANCEL/RETURN TO MAIN MENU ");
    locate(0, 12); printf(" H = PAUSE THE GAME             ");
	locate(0, 14); printf("   PRESS ANY KEY TO CONTINUE!   ");

    roundWindow(0, 0, 13, 9, 112);
    roundWindow(18, 0, 31, 8, 112);
    screen(0,1);
    waitkey(FALSE);
}


void drawMenuPtr(unsigned char x, unsigned char y, unsigned char offset, BOOL delete) {
    unsigned char pointer = delete ? 143 : 62; // empty block : arrow
    printBlock(x, y + offset, pointer);
}


void drawOptionsMenu() {
	cls(1);
    roundWindow(0, 0, 31, 15, 80);	
    locate(7, 8);  printf("EMPTY BACKGROUND:");
    locate(7, 9);  printf("MUTED:");
    locate(7, 10); printf("BACK");
    locate(2, 14); printf("SELECT OPTION (CURSOR/ENTER)");
    // on/off switches
    locate(25, 8);
    if (emptyBackground) printf("on ");
    else printf("off");
    locate(25, 9);
    if (muted) printf("on ");
    else printf("off");
}


void optionsMenu() {
    char optNumber = 0;
    drawOptionsMenu();
    drawMenuPtr(5, 8, optNumber, FALSE);
    do {
        drawHeader(FALSE, colourShift++);
        key = inkey();
        if (key == 10) { // cursor down
            drawMenuPtr(5, 8, optNumber, TRUE);
            if (optNumber++ == 2) optNumber = 0;
            drawMenuPtr(5, 8, optNumber, FALSE);
            if (!muted) sound(200,0);
        }
        else if (key == 94) { // cursor up
            drawMenuPtr(5, 8, optNumber, TRUE);
            if (optNumber-- == 0) optNumber = 2;
            drawMenuPtr(5, 8, optNumber, FALSE);
            if (!muted) sound(200,0);
        }
        else if (key == 13 || key == 32) { // enter or space bar
            if (!muted) sound(100,0);
            switch(optNumber) {
                case 0:
                    emptyBackground = !emptyBackground;
                    break;
                case 1:
                    muted = !muted;                
                    break;                    
                case 2:
                    return;
            }
            drawOptionsMenu();
            drawMenuPtr(5, 8, optNumber, FALSE);
        }
        delay(2);
    } while (TRUE);
}


void drawMenu() {
	cls(1);
    roundWindow(0, 0, 31, 15, 80);
	locate(10, 7);  printf("1 DRAGON GAME");
    locate(10, 8);  printf("2 DRAGONS GAME");
    locate(10, 9);  printf("HIGH SCORES");
    locate(10, 10); printf("OPTIONS");
    locate(10, 11); printf("HELP");
    locate(10, 12); printf("EXIT");
    locate(2, 14); printf("SELECT OPTION (CURSOR/ENTER)");
}


void menu() {
    char optNumber = 0;
	drawMenu();
    drawMenuPtr(8, 7, optNumber, FALSE);

    do {
        drawHeader(FALSE, colourShift++);
        key = inkey();
        if (key == 10) { // cursor down
            drawMenuPtr(8, 7, optNumber, TRUE);
            if (optNumber++ == 5) optNumber = 0;
            drawMenuPtr(8, 7, optNumber, FALSE);
            if (!muted) sound(200,0);
        }
        else if (key == 94) { // cursor up
            drawMenuPtr(8, 7, optNumber, TRUE);
            if (optNumber-- == 0) optNumber = 5;
            drawMenuPtr(8, 7, optNumber, FALSE);
            if (!muted) sound(200,0);
        }
        else if (key == 13 || key == 32) { // enter or space bar
            if (!muted) sound(100,0);
            drawMenuPtr(8, 7, optNumber, TRUE);
            switch (optNumber) {
                case 0: // 1p start game
                    numPlayers = 0;
                    gameOver[0] = FALSE;
                    gameOver[1] = TRUE;
                    return;
                case 1: // 2p start game
                    numPlayers = 1;
                    gameOver[0] = FALSE;
                    gameOver[1] = FALSE;
                    return;
                case 2: // high scores
                    drawHighScores();
                    if (!muted) sound(100,0);
                    break;
                case 3: // options menu
                    optionsMenu();
                    break;
                case 4: // show controls
                    drawHelp();
                    if (!muted) sound(100,0);
                    break;                
                case 5: // bye
                    cls(1);
                    printf("THANKS FOR PLAYING DRAGONTET!\n");
                    exit(0);
                default:
                    break;
            }   
            drawMenu();
            drawMenuPtr(8, 7, optNumber, FALSE);     
        }
        delay(2);
    } while (TRUE);
}


// logic to initialize the system and pits
void init() {
    setTimer(0); // initialises the system timer    
    menu();
	cls(1); // green screen
    roundWindow(PIT_WIDTH, 0, pitLeft[1]-1, 15, 0);
    for (unsigned char i = 0; i < 2; i++) {
        level[i] = previousLevel[i] = 1; // initial level
        lines[i] = 0; // lines cleared
        scores[6+i] = 0; // current score
        nextShape[i] = 255; // shape generation will be required
        backgroundChar[i] = backgroundCharList[0]; // character to fill in the pits
        createShape(i); // generate shape (shape, position)
        memset(pit[i], NO_BLOCK, PIT_WIDTH * PIT_HEIGHT); // initialize the empty pit
        drawPit(i);
    }
    cancelled = FALSE;
    displayStatus();
    if (numPlayers == 0) {
        numPiecesPlayed = 0;
        locate(23,6); printf(" PLEASE ");
        locate(23,7); printf("  WAIT  ");
        locate(23,8); printf(" DRAGON ");
        locate(23,9); printf("   2!   ");
    }
}


void rotateKeyPressed(unsigned char i) {
    unsigned char nextAngle = (shapeAngle[i] + 1) % 4;
    getRotatedShapeMap(shape[i], nextAngle, rotatedMap[i]);
    if (shapeCanMove(rotatedMap[i], 0, 0, i)) {
        shapeAngle[i] = nextAngle;
        drawShape(TRUE, i); // erase shape
        strncpy(shapeMap[i], rotatedMap[i], BLOCK_SIZE);
        drawShape(FALSE, i); // redraw shape
    }
}


void moveLeftKeyPressed(unsigned char i) {
    if (shapeCanMove(shapeMap[i], -1, 0, i)) {
        drawShape(TRUE, i);
        shapeX[i]--;
        drawShape(FALSE, i);
    }
}


void moveRightKeyPressed(unsigned char i) {
    if (shapeCanMove(shapeMap[i], 1, 0, i)) {
        drawShape(TRUE, i);
        shapeX[i]++;
        drawShape(FALSE, i);
    }
}


// checks time since last action
// 0 = rotate
// 1 = move left
// 2 = move right
BOOL canProcessInput(unsigned char i, unsigned char action) {
    unsigned int currentTime = getTimer();
    if (currentTime - lastInputTime[i][action] >= INPUT_DELAY) {
        lastInputTime[i][action] = currentTime;
        return TRUE;
    }
    return FALSE;
}


/*
#define PEEK(addr) (*(unsigned char *)(addr))
#define POKE(addr, value) (*(unsigned char *)(addr) = (value))

#define DRAGON_W !(PEEK(0x0159) & 0x10)
#define DRAGON_A !(PEEK(0x0153) & 0x04)
#define DRAGON_S !(PEEK(0x0155) & 0x10)
#define DRAGON_D !(PEEK(0x0156) & 0x04)
#define DRAGON_I !(PEEK(0x0153) & 0x08)
#define DRAGON_J !(PEEK(0x0154) & 0x08)
#define DRAGON_K !(PEEK(0x0155) & 0x08)
#define DRAGON_L !(PEEK(0x0156) & 0x08)*/


void mainLoop() {
    unsigned char i; // 0 = Dragon1, 1 = Dragon2

    playTune(tune2Notes, tune2Durations, 8);

    // initialise start times for both players
    startTime[0] = startTime[1] = getTimer();

    while (TRUE) {
        const byte *joystickPositions = readJoystickPositions();

#ifdef Dragon
        // check for pause, exit, or game over actions
        key = inkey();
        // pause
        if (key == 'H') {
            while (inkey() != '\0'); // clear input buffer
            while (inkey() == '\0') delay(1); // wait for key press
            startTime[0] = startTime[1] = getTimer(); // reset timers
            continue;
        // cancel/exit to main menu
        } else if (key == 'X') {
            if (!gameOver[0] || !gameOver[1])
                cancelled = TRUE;
            break;
        // enter/space after game over
        } else if ((key == 13 || key == 32) && (gameOver[0] && gameOver[1])) {
            break;
        }
        //////////////////////// PLAYER 1 ///////////////////////////
        // rotation (key, cursor, joystick)
        if (key == 'W' || key == 94 || joystickPositions[JOYSTK_LEFT_VERT] < 16)
            rotateKeyPressed(0);
        // move left (key, cursor, joystick)
        if (key == 'A' || key == 8 || joystickPositions[JOYSTK_LEFT_HORIZ] < 16)
            moveLeftKeyPressed(0);
        // fast drop (key, cursor, joystick)
        if (key == 'S' || key == 10 || joystickPositions[JOYSTK_LEFT_VERT] > 48)
            dropRate[0] = 0;            
        // move right (key, cursor, joystick)
        if (key == 'D' || key == 9 || joystickPositions[JOYSTK_LEFT_HORIZ] > 48)
            moveRightKeyPressed(0);

        //////////////////////// PLAYER 2 ///////////////////////////
        // rotation (key, joystick)
        if (key == 'I' || joystickPositions[JOYSTK_RIGHT_VERT] < 16)
            rotateKeyPressed(1);
        // move left (key, joystick)
        if (key == 'J' || joystickPositions[JOYSTK_RIGHT_HORIZ] < 16)
            moveLeftKeyPressed(1);
        // fast drop (key, joystick)
        if (key == 'K' || joystickPositions[JOYSTK_RIGHT_VERT] > 48)
            dropRate[1] = 0;              
        // move right (key, joystick)
        if (key == 'L' || joystickPositions[JOYSTK_RIGHT_HORIZ] > 48)
            moveRightKeyPressed(1);
#endif

#ifdef Coco
        // check for pause, exit, or game over actions
        // pause
        if (isKeyPressed(KEY_PROBE_H, KEY_BIT_H)) {
            while (inkey() != '\0'); // clear input buffer
            while (inkey() == '\0') delay(1); // wait for key press
            startTime[0] = startTime[1] = getTimer(); // reset timers
            continue;
        // cancel/exit to main menu
        } else if (isKeyPressed(KEY_PROBE_X, KEY_BIT_X)) {
            if (!gameOver[0] || !gameOver[1])
                cancelled = TRUE;
            break;
        // enter/space after game over
        } else if ((isKeyPressed(KEY_PROBE_ENTER, KEY_BIT_ENTER) 
            || isKeyPressed(KEY_PROBE_SPACE, KEY_BIT_SPACE)) 
            && (gameOver[0] && gameOver[1])) {
            break;
        }
        /////////////////// PLAYER 1 ///////////////////////
        // rotation (key, cursor, joystick)
        if (isKeyPressed(KEY_PROBE_W, KEY_BIT_W) 
            || isKeyPressed(KEY_PROBE_UP, KEY_BIT_UP) 
            || joystickPositions[JOYSTK_LEFT_VERT] < 16)
            if (canProcessInput(0,0)) rotateKeyPressed(0);
        // move left (key, cursor, joystick)
        if (isKeyPressed(KEY_PROBE_A, KEY_BIT_A) 
            || isKeyPressed(KEY_PROBE_LEFT, KEY_BIT_LEFT) 
            || joystickPositions[JOYSTK_LEFT_HORIZ] < 16) 
            if (canProcessInput(0,1)) moveLeftKeyPressed(0);
        // fast drop (key, cursor, joystick)
        if (isKeyPressed(KEY_PROBE_S, KEY_BIT_S) 
            || isKeyPressed(KEY_PROBE_DOWN, KEY_BIT_DOWN) 
            || joystickPositions[JOYSTK_LEFT_VERT] > 48) 
            dropRate[0] = 0;
        // move right (key, cursor, joystick)
        if (isKeyPressed(KEY_PROBE_D, KEY_BIT_D) 
            || isKeyPressed(KEY_PROBE_RIGHT, KEY_BIT_RIGHT) 
            || joystickPositions[JOYSTK_LEFT_HORIZ] > 48) 
            if (canProcessInput(0,2)) moveRightKeyPressed(0);

        //////////////////// PLAYER 2 ///////////////////////
        if (numPlayers > 0) {
            // rotation (key, cursor, joystick)
            if (isKeyPressed(KEY_PROBE_I, KEY_BIT_I) 
                || joystickPositions[JOYSTK_RIGHT_VERT] < 16) 
                if (canProcessInput(1,0)) rotateKeyPressed(1);
            // move left (key joystick)
            if (isKeyPressed(KEY_PROBE_J, KEY_BIT_J) 
                || joystickPositions[JOYSTK_RIGHT_HORIZ] < 16) 
                if (canProcessInput(1,1)) moveLeftKeyPressed(1);
            // fast drop (key, joystick)
            if (isKeyPressed(KEY_PROBE_K, KEY_BIT_K) 
                || joystickPositions[JOYSTK_RIGHT_VERT] > 48) 
                dropRate[1] = 0;
            // move right (key, joystick)
            if (isKeyPressed(KEY_PROBE_L, KEY_BIT_L) 
                || joystickPositions[JOYSTK_RIGHT_HORIZ] > 48) 
                if (canProcessInput(1,2)) moveRightKeyPressed(1);
        }            
#endif
        // check if the falling time has been exceeded
        for (i = 0; i <= numPlayers; i++) {
            if (!gameOver[i] && getTimer() >= startTime[i] + dropRate[i]) {
                dropShape(i); // shape moves down
                startTime[i] = getTimer(); // reset fall timer
            }
        }
    }
}


/*
void mainLoop() {
    unsigned char i; // 0 = Dragon1, 1 = Dragon2

    // Inicializa los tiempos de inicio de ambos jugadores
    startTime[0] = startTime[1] = getTimer();

    while (TRUE) {
        // Lee teclas solo una vez por iteración
        key = inkey();

        // Dragon 1: Procesa sus teclas
        if (DRAGON_W) rotateKeyPressed(0);
        if (DRAGON_A) moveLeftKeyPressed(0);
        if (DRAGON_D) moveRightKeyPressed(0);
        if (DRAGON_S) dropRate[0] = 0;

        // Dragon 2: Procesa sus teclas
        if (DRAGON_I) rotateKeyPressed(1);
        if (DRAGON_J) moveLeftKeyPressed(1);
        if (DRAGON_L) moveRightKeyPressed(1);
        if (DRAGON_K) dropRate[1] = 0;

        // No limpies la tabla de rollover aquí. Hazlo solo si es absolutamente necesario.
        // Limpieza de tabla de rollover (después de procesar todo)
        for (int addr = 0x0151; addr <= 0x0159; addr++) {
            *((unsigned char *)addr) = 0xFF;
        }

        // Procesa el tiempo de caída para ambos jugadores
        for (i = 0; i < 2; i++) {
            if (gameOver[i]) continue;
            // Verifica si ha pasado el tiempo de caída
            if (getTimer() >= startTime[i] + dropRate[i]) {
                dropShape(i);
                startTime[i] = getTimer();
            }
        }
    }
}*/


// check if the new score is high enough to enter the top 6
void checkScore(unsigned char player) {
    // indices: 0-1-2-3-4-5-[score p1]-[score p2]
    int i = player + 6;
    int j;
    newScore[player] = FALSE;
    if (scores[i] > scores[5]) {
        cls(1);
        roundWindow(0, 0, 31, 15, 80);        
        drawHeader(FALSE, ++colourShift);
        locate(10, 8); printf("*** %u ***", scores[i]);
        locate(7, 11); printf("GOOD SCORE DRAGON %d", player + 1);
        locate(7, 12); printf("NAME?: ");

        char *response = readline();
        // check if the response is empty
        if (response == NULL || response[0] == '\0') {
            strncpy(names[i], "?", 10);
        } else {
            strncpy(names[i], response, 10);
        }
        names[i][10] = '\0'; // ensure the name is null-terminated

        // find the correct position in the top 6 list for the new score
        for (j = 5; j >= 0; j--) {
            if (scores[i] > scores[j]) {
                // shift scores/names down
                if (j < 5) {
                    scores[j+1] = scores[j];
                    strncpy(names[j+1], names[j], 10);
                }
            } else
                break;
        }
        // insert the new score and name into the correct position
        scores[j+1] = scores[i];
        strncpy(names[j+1], names[i], 10);
        newScore[player] = TRUE; // the score table has been updated
    }
}


int main() {
    while(TRUE) {
        srand(getTimer()); // random seed
        init();	// logic to initialize the system and pits	
		mainLoop();
        while (inkey() != '\0'); // clear input buffer
		// check the scores of the last game
        if (!cancelled) {
            checkScore(0);
            if (numPlayers > 0)
                checkScore(1);
            // draw the scoreboard
            if (newScore[0] || newScore[1]) {
                cls(1);
                drawHeader(FALSE, colourShift);
                drawHighScores();
            }
        }
    }
    return 0;
}
