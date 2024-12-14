
/*

= DRAGONTET =
salvaKantero 2025

2 player game version
Compatible with Dragon 32/64 and COCO
Based on Peter Swinkels' PC Qbasic code. (QBBlocks v1.0)

use the CMOC compiler 0.1.89 or higher:
"cmoc --dragon -o dtet.bin dtet.c"
use xroar to test:
"xroar -run dtet.bin"

TODO
====
- añadir 2º jugador simultaneo
- añadir basurilla a partir del nivel 4

- función PLAY
- efectos FX
  - pulsación de menú
  - línea completada
- melodías
  - melodía inicial
  - melodía al inicio de cada nivel
  - melodía al perder
- cargador con pantalla inicial (de texto)
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

char key = '\0'; // key pressed
unsigned char numPlayers; // 0 = dragon1  1 = dragon2
BOOL newScore = FALSE; // TRUE = redraws the best scores table
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
BOOL markedPit = TRUE; // enables/disables the marks in the pit (options menu)
BOOL autorepeatKeys = FALSE; // enables/disables the auto-repeat of keys (options menu)

// pos 0-5: fake values for the initial TOP 6
// pos 6-7: values for the current game
char names[8][11] = {"DRAGON","DRAGON","DRAGON","DRAGON","DRAGON","DRAGON","", ""};
unsigned int scores[8] = {2000, 1800, 1600, 1400, 1200, 1000, 0, 0};


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
        if (markedPit)
            printBlock(x, y, 42); // "*" inverted
        else
            printBlock(x, y, EMPTY_BLOCK);
        return;
    }
    printBlock(x, y, FILLED_BLOCK + ((colour - 1) << 4)); // <<4 = x16
}


void drawPit(unsigned char i) {
    // loop through and repaint the contents of the pit (i = dragon pit)
    for (unsigned char y = 0; y < PIT_HEIGHT; y++) {
        unsigned char rowOffset = y * PIT_WIDTH;
        for (unsigned char x = 0; x < PIT_WIDTH; x++)
            drawBlock(x, y, pit[i][x + rowOffset], i);
    }
}


// check if a shape can move in the specified direction (i = dragon pit)
BOOL shapeCanMove(char *map, char xDir, char yDir, unsigned char i) {
    int x, y;
    // loop through all the blocks of the shape
    for (int blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
        for (int blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
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
    // loop through all the blocks of the shape (4x4 grid)
    for (unsigned char blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
        for (unsigned char blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
            char blockColour = nextShapeMap[i][(blockY * SIDE_BLOCK_SIZE) + blockX];
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
        }
    }
    if (numPlayers == 0) {
        drawHeader(TRUE, ++colourShift);
        locate(11, 6);  printf("LEVEL:  %2u", level[0]);
        locate(11, 7); printf("LINES: %3d", lines[0]);
        locate(11, 8); printf("SC: %6u", scores[6]);
        locate(11, 9); printf("HI: %6u", scores[0]);
        locate(11, 12); printf("NEXT:");
        drawNextShape(11, 0);
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
}


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
    /* calculates the fall speed based on the level
        level 1: 35 delay ticks
        level 2: 30   "
        level 3: 25   "
        level 4: 20   "
        level 5: 15   "
        level 6: 10   "
        level x:  5   " */
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
    int x, y;
    char blockColour;
    // iterates through all the blocks of the shape
    for (int blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
        for (int blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
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


void setTrapBlock(unsigned char i) {
    int trapX = rand() % PIT_WIDTH; // random column in the pit
    int trapY = PIT_HEIGHT / 2 + rand() % (PIT_HEIGHT / 2); // rows only in the second half
}


void removeFullRow(unsigned char removedRow, unsigned char i) {
    unsigned char x, y;
    char blockColour;

    // line selection effect
    for (x = 0; x < PIT_WIDTH; x++)
        printBlock(x, removedRow, WHITE_BLOCK);
    delay(2);

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


void checkForFullRows(unsigned char i) { // searches for full rows
    BOOL fullRow;
    int numLines = 0;
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
            case 1: scores[j] += (100 * level[i]); break;
            case 2: scores[j] += (300 * level[i]); break;
            case 3: scores[j] += (500 * level[i]); break;
            case 4: scores[j] += (800 * level[i]);
        }
        // updates the total completed rows and calculates the level
        lines[i] += numLines;
        level[i] = (unsigned char)(lines[i] / LINES_LEVEL) + 1;

        // generates a trap block
        if (numLines > 1 && level[0] >= 4)
            setTrapBlock(0);
    }
}


void settleActiveShapeInPit(unsigned char i) {
    int x, y;
    char blockColour;
    for (int blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
        for (int blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
            x = shapeX[i] + blockX;
            y = shapeY[i] + blockY;
            blockColour = shapeMap[i][blockY * SIDE_BLOCK_SIZE + blockX];
            // checks that the block is within the pit boundaries and not an empty space
            if (blockColour != NO_BLOCK && x >= 0 && x < PIT_WIDTH && y >= 0 && y < PIT_HEIGHT)
                // locks the shape's block in the pit
                pit[i][y * PIT_WIDTH + x] = blockColour;
        }
    }
}


void dropShape(unsigned char i) {
    // checks if the shape can move down
    if (shapeCanMove(shapeMap[i], 0, 1, i)) {
        drawShape(TRUE, i);      // erases the current shape
        shapeY[i] += 1;          // moves the shape down by one position
        drawShape(FALSE, i);     // redraws the shape at the new position
        // 1 point for each line in rapid drop
        if (dropRate[i] == 0) scores[6+i]++;
    } else {
        settleActiveShapeInPit(i);
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
	// TOP 6 position (0-5)
    for(unsigned char pos = 0; pos < 6; pos++) {
        locate( 8, 7+pos); printf(".............");
        locate( 7, 7+pos); printf("%s", names[pos]);
        locate(20, 7+pos); printf("%5u", scores[pos]);
	}
	locate(3, 14); printf("PRESS ANY KEY TO CONTINUE!");
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

    roundWindow(0, 0, 13, 9, 64);
    roundWindow(18, 0, 31, 8, 64);
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
	locate(8, 8);  printf("AUTOREPEAT KEYS:");
    locate(8, 9);  printf("MARKED PIT:");
    locate(8, 10); printf("BACK");
    locate(2, 14); printf("SELECT OPTION (CURSOR/ENTER)");
    // on/off switches
    locate(25, 8);
    if (autorepeatKeys) printf("on ");
    else printf("off");
    locate(25, 9);
    if (markedPit) printf("on ");
    else printf("off");
}


void optionsMenu() {
    char optNumber = 0;
    drawOptionsMenu();
    drawMenuPtr(6, 8, optNumber, FALSE);
    do {
        drawHeader(FALSE, colourShift++);
        key = inkey();
        if (key == 10) { // cursor down
            drawMenuPtr(6, 8, optNumber, TRUE);
            if (optNumber++ == 2) optNumber = 0;
            drawMenuPtr(6, 8, optNumber, FALSE);
        }
        else if (key == 94) { // cursor up
            drawMenuPtr(6, 8, optNumber, TRUE);
            if (optNumber-- == 0) optNumber = 2;
            drawMenuPtr(6, 8, optNumber, FALSE);
        }
        else if (key == 13 || key == 32) { // enter or space bar
            switch(optNumber) {
                case 0:
                    autorepeatKeys = !autorepeatKeys;                
                    break;
                case 1:
                    markedPit = !markedPit;
                    break;
                case 2:
                    return;
            }
            drawOptionsMenu();
            drawMenuPtr(6, 8, optNumber, FALSE);
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
        }
        else if (key == 94) { // cursor up
            drawMenuPtr(8, 7, optNumber, TRUE);
            if (optNumber-- == 0) optNumber = 5;
            drawMenuPtr(8, 7, optNumber, FALSE);
        }
        else if (key == 13 || key == 32) { // enter or space bar
            drawMenuPtr(8, 7, optNumber, TRUE);
            switch (optNumber) {
                case 0: // 1p start game
                    numPlayers = 0;
                    return;
                case 1: // 2p start game
                    numPlayers = 1;
                    return;
                case 2: // high scores
                    drawHighScores();
                    break;
                case 3: // options menu
                    optionsMenu();
                    break;
                case 4: // show controls
                    drawHelp();
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
    setTimer(0);
    menu();
	cls(1); // green screen
    roundWindow(PIT_WIDTH, 0, pitLeft[1]-1, 15, 0);
    for (unsigned char i = 0; i < 2; i++) {
        gameOver[i] = FALSE; // game in progress
        level[i] = 1; // initial level
        lines[i] = 0; // lines cleared
        scores[6+i] = 0; // current score
        nextShape[i] = 255; // shape generation will be required
        createShape(i); // generate shape (shape, position)
        memset(pit[i], NO_BLOCK, PIT_WIDTH * PIT_HEIGHT); // initialize the empty pit
        drawPit(i);
    }
    displayStatus();
    if (numPlayers == 0) {
        locate(23,6); printf(" PLEASE ");
        locate(23,7); printf("  WAIT  ");
        locate(23,8); printf(" DRAGON ");
        locate(23,9); printf("   2!   ");
    }
}


void mainLoop() {
    unsigned char newAngle = 0;
    unsigned char i;

    // save the start time
    startTime[0] = startTime[1] = getTimer();

    while (TRUE) {
        const byte *joystickPositions = readJoystickPositions();
        key = inkey(); // read keypresses

        // handle auto-repeat for keys
        if (key == '\0' && autorepeatKeys && dropRate[0] != 0) {
            for (i = 0; i <= 9; i++) 
                *((unsigned char *)0x0150 + i) = 0xFF;
            delay(3);
        }

        // check for pause, exit, or game over actions
        if (key == 'H') { // pause
            while (inkey() != '\0'); // clear input buffer
            while (inkey() == '\0') delay(1); // wait for key press
            startTime[0] = startTime[1] = getTimer(); // reset timers
            continue;
        } else if (key == 'C') { // exit to main menu
            break;
        } else if ((key == 13 || key == 32) && gameOver[0]) { // Enter/space after game over
            break;
        }

        // check for shape movement and rotation (key or joystick)
        if (!gameOver[0]) {
            if (key == 'W' || key == 94 || joystickPositions[JOYSTK_LEFT_VERT] < 16) { // rotate
                newAngle = (shapeAngle[0] + 1) % 4;
                getRotatedShapeMap(shape[0], newAngle, rotatedMap[0]);
                if (shapeCanMove(rotatedMap[0], 0, 0, 0)) {
                    shapeAngle[0] = newAngle;
                    drawShape(TRUE, 0); // erase shape
                    strncpy(shapeMap[0], rotatedMap[0], BLOCK_SIZE);
                    drawShape(FALSE, 0); // redraw shape
                }
            } 
            else if (key == 'A' || key == 8 || joystickPositions[JOYSTK_LEFT_HORIZ] < 16) { // move left
                if (shapeCanMove(shapeMap[0], -1, 0, 0)) {
                    drawShape(TRUE, 0); // erase shape
                    shapeX[0]--;
                    drawShape(FALSE, 0); // redraw shape
                }
            } 
            else if (key == 'D' || key == 9 || joystickPositions[JOYSTK_LEFT_HORIZ] > 48) { // move right
                if (shapeCanMove(shapeMap[0], 1, 0, 0)) {
                    drawShape(TRUE, 0); // erase shape
                    shapeX[0]++;
                    drawShape(FALSE, 0); // redraw shape
                }
            }
            else if (key == 'S' || key == 10 || joystickPositions[JOYSTK_LEFT_VERT] > 48) { // fast drop
                dropRate[0] = 0;
            }
            // check if the falling time has been exceeded
            if (getTimer() >= startTime[0] + dropRate[0]) {
                dropShape(0); // shape moves down
                startTime[0] = getTimer(); // reset fall timer
            }
        }
    }
}


// check if the new score is high enough to enter the top 6
void checkScore(unsigned char player) {
    // indices: 0-1-2-3-4-5-[score p1]-[score p2]
    int i = player + 6;
    int j;
    newScore = FALSE;
    if (scores[i] > scores[5]) {
        // clear part of the screen
        for (j = 11; j < 15; j++) {
            locate(4, (unsigned char)j);
            printf("                       ");
        }
        locate(6, 12); printf("GOOD SCORE DRAGON %d", player + 1);
        locate(6, 13); printf("NAME?: ");

        char *response = readline();
        strncpy(names[i], response, 10);
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
        newScore = TRUE; // the score table has been updated
    }
}


int main() {
    while(TRUE) {
        srand(getTimer()); // random seed
        init();		
		mainLoop();
		// check the scores of the last game
        checkScore(0);
        if (numPlayers > 0)
            checkScore(1);
        // draw the scoreboard
        if (newScore) {
		    cls(1);
		    drawHeader(FALSE, colourShift);
		    drawHighScores();
        }
    }
    return 0;
}
