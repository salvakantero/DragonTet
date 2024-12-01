
/*

*** TETRIS4DRAGONS ***
   salvaKantero2024

2 player game version
Compatible with Dragon 32/64 and COCO
Based on Peter Swinkels' PC Qbasic code. (QBBlocks v1.0)

use the CMOC compiler 0.1.89 or higher:
"cmoc --dragon -o t4d.bin t4d.c"
use xroar to test:
"xroar -run t4d.bin"

TODO
====
- añadir 2º jugador simultaneos
- redondear menús
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

#define EMPTY_BLOCK 128 // black block, no pixels
#define FILLED_BLOCK 143 // all pixels in the block are active
#define WHITE_BLOCK 207 // to mark a completed line
#define SIDE_BLOCK_COUNT 4 // size of the piece's side (piece = 4x4 blocks)
#define LAST_SIDE_BLOCK 3 // to iterate over the current piece (0 to 3)
#define BLOCK_COUNT 16 // size of the piece in blocks (piece = 4x4 blocks)
#define NO_BLOCK '0' // character representing an empty block
#define PIT_WIDTH 10 // width of the pit in blocks
#define PIT_HEIGHT 16 // height of the pit in blocks
#define LINES_LEVEL 10 // lines per level

char key = '\0'; // key pressed
unsigned char numPlayers; // 0 = player1  1 = player2
BOOL newScore = FALSE; // TRUE = redraws the best scores table
BOOL gameOver[2] = {FALSE, FALSE}; // FALSE = game in progress, TRUE = finished
int dropRate[2] = {0, 0}; // 0 = lower the piece one position
int startTime[2] = {0, 0}; // system ticks since the piece has moved
char pit[2][PIT_WIDTH * PIT_HEIGHT]; // content of each pit in blocks
unsigned char level[2] = {1, 1}; // game levels (increases the speed)
int lines[2] = {0, 0}; // lines cleared
const unsigned char pitLeft[2] = {0, 22}; // X position of the left side of each pit
unsigned char shape[2] = {255, 255}; // piece type (0 to 6). 255 = piece not defined
unsigned char nextShape[2] = {255, 255}; // next piece type (0 to 6). 255 = piece not defined
unsigned char shapeAngle[2] = {0, 0}; // piece rotation (0 to 3)
int shapeX[2] = {0, 0}, shapeY[2] = {0, 0};	// piece XY position
char shapeMap[2][BLOCK_COUNT]; // piece design
char nextShapeMap[2][BLOCK_COUNT]; // next piece design
char rotatedMap[2][BLOCK_COUNT]; // design of the already rotated piece
unsigned char colourShift = 0; // colour shift effect in the title
BOOL chequeredPit = TRUE; // enables/disables the chequered pit (options menu)
BOOL autorepeatKeys = FALSE; // enables/disables the auto-repeat of keys (options menu)

// pos 0-4: fake values for the initial TOP 5
// pos 5: values for the current game
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
    if (colour == 0) { // background
        if (chequeredPit) {
            locate(x + pitLeft[i], y);
            putchar(111); // "O" inverted
        } else
            printBlock(x, y, EMPTY_BLOCK);
        return;
    }
    printBlock(x, y, FILLED_BLOCK + ((colour - 1) << 4)); // <<4 = x16
}



void drawPit(unsigned char i) {
    // loop through and repaint the contents of the pit (i = player pit)
    for (unsigned char y = 0; y < PIT_HEIGHT; y++) {
        unsigned char rowOffset = y * PIT_WIDTH;
        for (unsigned char x = 0; x < PIT_WIDTH; x++)
            drawBlock(pit[i][x + rowOffset], x, y, i);
    }
}



// check if a piece can move in the specified direction (i = player pit)
BOOL PieceCanMove(char *map, char xDir, char yDir, unsigned char i) {
    int x, y;
    // loop through all the blocks of the piece
    for (int blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
        for (int blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
            if (map[(SIDE_BLOCK_COUNT * blockY) + blockX] != NO_BLOCK) {
                // calculate the position in the pit
                x = (shapeX[i] + blockX) + xDir;
                y = (shapeY[i] + blockY) + yDir;
                // new piece appears at the top
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



void drawNextPiece(unsigned char i) {
    // loop through all the blocks of the piece (4x4 grid)
    for (unsigned char blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
        for (unsigned char blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
            char blockColour = nextShapeMap[i][(blockY * SIDE_BLOCK_COUNT) + blockX];
            // determine the block type (1 = green, 5 = white)
            blockColour = (blockColour == NO_BLOCK) ? '1' : '5';
            // different heights depending on the player
            if (i == 0) drawBlock(blockColour, blockX + 17, blockY + 4, i);
            else drawBlock(blockColour, blockX - 5, blockY + 12, i);
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



void displayStatus() {
    // player 1
    if (gameOver[0] == TRUE) {
        locate(pitLeft[0], 8);
        printf("GAME OVER!");
    }
    locate(11, 0); printf("=PLAYER 1=");
    locate(11, 1); printf("LEVEL:  %2u", level[0]);
    locate(11, 2); printf("LINES: %3d", lines[0]);
    locate(11, 3); printf("SCORE:%5u", scores[6]);
    locate(11, 4); printf("NEXT:");

    // player 2
    if (numPlayers == 1) {
        if (gameOver[1] == TRUE) {
			locate(pitLeft[1], 8);
			printf("GAME OVER!");
        }
        locate(11, 8);  printf("=PLAYER 2=");
        locate(11, 9);  printf("LEVEL:  %2u", level[1]);
        locate(11, 10); printf("LINES: %3d", lines[1]);
        locate(11, 11); printf("SCORE:%5u", scores[7]);
        locate(11, 12); printf("NEXT:");
    }
    
    for (unsigned char i = 0; i <= numPlayers; i++)
        if (!gameOver[i]) drawNextPiece(i); // DEBUG
}



const char* getPieceMap(unsigned char shape) {
    static const char* shapeMaps[] = {
        "0000444400000000", // | red
        "0000222000200000", // _| yellow
        "0000777070000000", // |_ magenta
        "0000033003300000", // [] blue
        "0000066066000000", // S cyan
        "0000880008800000", // Z orange
        "0000111001000000"  // T green
    };
    return shapeMaps[shape];
}



void getRotatedPieceMap(unsigned char shape, unsigned char angle, char *rotatedMap) {
    const char *map = getPieceMap(shape); // Unrotated map
    // no rotation needed, copy map directly
    if (angle == 0) {
        strncpy(rotatedMap, map, BLOCK_COUNT);
        return;
    }
    // clear the rotatedMap array
    memset(rotatedMap, NO_BLOCK, BLOCK_COUNT);
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
    strncpy(nextShapeMap, getPieceMap(nextShape), BLOCK_COUNT);
}



void createShape() {
    /* calculates the fall speed based on the level
        level 1: 35 delay ticks
        level 2: 30   "
        level 3: 25   "
        level 4: 20   "
        level 5: 15   "
        level 6: 10   "
        level x:  5   " */
    dropRate = (level < 7) ? (40 - (level * 5)) : 5;
    // if it's not the first piece, take the value of nextShape
    if (nextShape != 255)
        shape = nextShape;
    else // new random piece (0 to 6)
        shape = (unsigned char) rand() % 7;
       
    shapeX = 3; // centre of the pit
    shapeY = -LAST_SIDE_BLOCK; // fully hidden
    shapeAngle = 0; // unrotated
    strncpy(shapeMap, getPieceMap(shape), BLOCK_COUNT);
    // generates the next piece
    createNextShape();
}



void drawShape(BOOL eraseShape) {
    int pitX, pitY;
    int blockX, blockY;
    char blockColour;
    // iterates through all the blocks of the piece
    for (blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
        for (blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
            pitX = shapeX + blockX;
            pitY = shapeY + blockY;
            // check if the block is within the pit boundaries
            if (pitX >= 0 && pitX < PIT_WIDTH && pitY >= 0 && pitY < PIT_HEIGHT) {
                if (eraseShape) {
                    // gets the colour of the pit at the current position
                    blockColour = pit[pitY * PIT_WIDTH + pitX];
                } else {
                    // gets the colour of the piece's block
                    blockColour = shapeMap[blockY * SIDE_BLOCK_COUNT + blockX];
                    // if the block is empty, take the colour of the pit at that position
                    if (blockColour == NO_BLOCK)
                        blockColour = pit[pitY * PIT_WIDTH + pitX];
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

    // line selection effect
    for (pitX = 0; pitX < PIT_WIDTH; pitX++)
        printBlock(pitX, removedRow, WHITE_BLOCK);

    drawHeader(14, ++colourShift);
    delay(1);

    // iterate from the removed row upwards
    for (pitY = removedRow; pitY > 0; pitY--) {
        for (pitX = 0; pitX < PIT_WIDTH; pitX++) {
            // copy the top row to the current one
            blockColour = pit[(pitY - 1) * PIT_WIDTH + pitX];
            pit[pitY * PIT_WIDTH + pitX] = blockColour;
        }
    }
    // clear the first row after shifting all rows down
    for (pitX = 0; pitX < PIT_WIDTH; pitX++)
        pit[pitX] = NO_BLOCK;
}



void checkForFullRows() { // searches for full rows
    BOOL fullRow;
    int numLines = 0;
    unsigned char pitX, pitY;
    unsigned char j = 6; // indices: 0-1-2-3-4-5-[current score]
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
            case 1: scores[j] += (100 * level); break;
            case 2: scores[j] += (300 * level); break;
            case 3: scores[j] += (500 * level); break;
            case 4: scores[j] += (800 * level);
        }
        // updates the total completed rows and calculates the level
        lines += numLines;
        level = (unsigned char)(lines / LINES_LEVEL) + 1;
    }
}



void settleActiveShapeInPit() {
    int blockX, blockY, pitX, pitY;
    char blockColour;
    for (blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
        for (blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
            pitX = shapeX + blockX;
            pitY = shapeY + blockY;
            blockColour = shapeMap[blockY * SIDE_BLOCK_COUNT + blockX];
            // checks that the block is within the pit boundaries and not an empty space
            if (blockColour != NO_BLOCK && pitX >= 0 && pitX < PIT_WIDTH && pitY >= 0 && pitY < PIT_HEIGHT)
                // fixes the piece's block in the pit
                pit[pitY * PIT_WIDTH + pitX] = blockColour;
        }
    }
}



void dropShape() {
    // checks if the piece can move down
    if (PieceCanMove(shapeMap, 0, 1)) {
        drawShape(TRUE);      // erases the current piece
        shapeY += 1;          // moves the piece down by one position
        drawShape(FALSE);     // redraws the piece at the new position
        // 1 point for each line in rapid drop
        if (dropRate == 0) scores[6]++;
    } else {
        settleActiveShapeInPit();
        // checks if the piece has reached the top (the game is lost)
        if (shapeY < 0)
            gameOver = TRUE;
        else {
            checkForFullRows();
            drawPit();
            createShape();
            drawShape(FALSE);
        }
        // displays the current game status, such as points or messages
        displayStatus();
    }
}



void drawHighScores() {
	// TOP 6 position (0-5)
    for(unsigned char pos = 0; pos < 6; pos++) {
        locate( 7, 7+pos); printf("...............");
        locate( 7, 7+pos); printf("%s", names[pos]);
        locate(19, 7+pos); printf("%5u", scores[pos]);
	}
	locate(2, 14); printf("PRESS ANY KEY TO CONTINUE...");
    while (TRUE) {
        if (inkey() != 0) break;
        drawHeader(8, colourShift++);
        delay(2);
    }
}



void drawOptionsMenu() {
	cls(1);
	locate(4, 8);  printf("1)  AUTOREPEAT KEYS:");
    locate(4, 9);  printf("2)  CHEQUERED PIT:");
    locate(4, 10); printf("3)  BACK");
    locate(7, 14); printf("SELECT OPTION (1-3)");
    // on/off switches
    locate(25, 8);
    if (autorepeatKeys) printf("on ");
    else printf("off");
    locate(25, 9);
    if (chequeredPit) printf("on ");
    else printf("off");
}



void optionsMenu() {
    drawOptionsMenu();
    while (TRUE) {
        drawHeader(8, colourShift++);
        key = inkey();		
        if (key == '1')	{
            autorepeatKeys = !autorepeatKeys;
            drawOptionsMenu();
        }
        else if (key == '2') {	
            chequeredPit = !chequeredPit;
            drawOptionsMenu();
        }
		else if (key == '3') { // back	
            break;
        } 
        delay(2);
    }
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

        switch (key) {
            case '1':
                return; // start game
            case '2':
                optionsMenu();
                drawMenu(); // options menu
                break;
            case '3':
                drawHighScores();
                drawMenu(); // high scores
                break;
            case '4':
			    cls(1);
                printf("THANKS FOR PLAYING T4D!\n");
                exit(0);
            default:
                break;
        }
        delay(2);
    } while (TRUE);
}



// rounds the text window
void roundWindow() {
    printBlock(PIT_WIDTH+1, 0, 129); // upper left corner
    printBlock(SCREEN_WIDTH-1, 0, 130); // upper right corner
    printBlock(PIT_WIDTH+1, PIT_HEIGHT-1, 132); // bottom left corner
    printBlock(SCREEN_WIDTH-1, PIT_HEIGHT-1, 136); // bottom right corner
}



// logic to initialize the system and pits
void init() {
    setTimer(0);
    menu();
	cls(1); // green screen
    roundWindow();
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

    // save the start time
    startTime = getTimer();

    while (TRUE) {
        // if the falling time has been exceeded
        if (!gameOver && getTimer() >= startTime + dropRate) {
            dropShape(); // the piece moves down
            startTime = getTimer(); // reset the fall timer
        }

        key = inkey(); // read keypresses
        
        if (key == '\0') {
            if (autorepeatKeys == TRUE) { // auto-repeat
                for (unsigned char i = 0; i <= 9; i++)
                    *((unsigned char *)0x0150 + i) = 0xFF;
                delay(2);
            }
            continue;
        }

        // Pause
        if (key == 'H') { // empties the input buffer
            while (inkey() != '\0');
            // Wait until a key is pressed to exit the pause.
            while (inkey() == '\0')
                delay(1);
            // Set the timer after the pause
            startTime = getTimer();
            continue;
        }

        if (key == 'X') // if X is pressed, exit to the main menu
            break;

        if (!gameOver) {
            if (gameOver == FALSE) { // game in progress
                switch (key) {
                    case 'W': // rotate key                      
                        // reset the angle or increase it by 90 degrees
                        newAngle = (shapeAngle == 3) ? 0 : shapeAngle + 1;
                        getRotatedPieceMap(shape, newAngle, rotatedMap);
                        if (PieceCanMove(rotatedMap, 0, 0)) {
                            shapeAngle = newAngle;
                            drawShape(TRUE); // erase piece                            
                            strncpy(shapeMap, rotatedMap, BLOCK_COUNT);
                            drawShape(FALSE);
                        }                         
                        break;
                    case 'A': // move left
                        if (PieceCanMove(shapeMap, -1, 0)) {
                            drawShape(TRUE); // erase piece
                            shapeX--;
                            drawShape(FALSE);
                        }
                        break;
                    case 'D': // move right
                        if (PieceCanMove(shapeMap, 1, 0)) {
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
    newScore = FALSE;
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
            } else
                break;
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
        checkScores();
        // draw the scoreboard
        if (newScore) {
		    cls(1);
		    drawHeader(8, colourShift);
		    drawHighScores();
        }
    }
    return 0;
}
