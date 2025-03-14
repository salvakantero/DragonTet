/*
==============================================================================

  .::DragonTet::. v1.1

  Programming: salvaKantero
  Betatesting: Luna_314

  An experimental Clone of 'Tetris' in text mode 
  for Dragon 32/64 and Tandy Color Computer 1/2/3.
  Based on Peter Swinkels' PC Qbasic code (QBBlocks v1.0).
  
==============================================================================

  CHANGELOG:
    2025-01-31 v1.1 
      Joystick support disabled by default, can be enabled in settings.
      Settings menu reorganised.

==============================================================================

  This file is part of "DragonTet". Copyright (C) 2025 @salvakantero

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

==============================================================================

Compilation Instructions:
use the CMOC compiler 0.1.89 or higher...
"cmoc --dragon -D Dragon -o dtetdr.bin dtet.c"
"cmoc --coco -D Coco -o dtetcc.bin dtet.c"

Testing:
use xroar to test (dragon)...
"xroar -run dtetdr.bin"

CAS files:
"perl bin2cas.pl -o dtetdr.cas -D dtetdr.bin"
"perl bin2cas.pl -o dtetcc.cas -C dtetcc.bin"
...then type "CLOADM" and "EXEC" already inside the emulator.
(after uploading the .cas file)

WAV files:
"perl bin2cas.pl -o dtetdr.wav -D dtetdr.bin"
"perl bin2cas.pl -o dtetcc.wav -C dtetcc.bin"

VDK file:
use DragonDOSui 1.0.3 or higher...
  - create a new disk (180kb) with .VDK extension
  - insert bynary file (select "dtetdr.bin")
  - type RUN"DTETDR.BIN" already inside the emulator

==============================================================================

*/


#include <cmoc.h>
#include <coco.h>

#define SCREEN_BASE 0x400 // base address of the video memory in text mode
#define SCREEN_WIDTH 32 // screen width in characters

#define EMPTY_BLOCK 128 // black block, no active pixels
#define FILLED_BLOCK 143 // // all pixels in the block are active (143 = green)
#define WHITE_BLOCK 207 // to highlight a complete line

#define SIDE_BLOCK_SIZE 4 // size of the shape's side (shape = 4x4 blocks)
#define LAST_SIDE_BLOCK 3 // to iterate over the current shape (0 to 3)
#define BLOCK_SIZE 16 // size of the shape in blocks (shape = 4x4 blocks)
#define NO_BLOCK '0' // character representing an empty block
#define PIT_WIDTH 10 // width of the pit in blocks
#define PIT_HEIGHT 16 // height of the pit in blocks
#define LINES_LEVEL 10 // lines per level
#define DROP_RATE_LEVEL 4 // decrease of waiting time per level for piece drop
#define MIN_DROP_RATE_LEVEL 6 // minimum waiting time for piece drop
#define PIECES_TRAP 12 // pieces played to generate a trap (1 player)
#define LINES_TRAP 3 // lines achieved to generate a trap (2 players)
#define INPUT_DELAY 7 // delay for processing inputs
#define JTHRESHOLD_LOW 16 // low level of joystick motion activation
#define JTHRESHOLD_HIGH 48 // high level of joystick motion activation

char key = '\0'; // key pressed
unsigned char numPlayers; // 0 = 1 player  1 = 2 players
BOOL newScore[2] = {FALSE, FALSE}; // TRUE = redraws the best scores table
BOOL gameOver[2] = {FALSE, FALSE}; // FALSE = game in progress, TRUE = finished
int dropRate[2] = {0, 0}; // waiting ticks to lower the piece one line
int startTime[2] = {0, 0}; // ticks elapsed since the piece went down
char pit[2][PIT_WIDTH * PIT_HEIGHT]; // content of each pit in blocks
unsigned char level[2] = {1, 1}; // current level
int lines[2] = {0, 0}; // lines cleared
const unsigned char pitLeft[2] = {0, 22}; // X position of the left side of each pit
unsigned char shape[2] = {255, 255}; // shape type (0 to 6). 255 = shape not defined
unsigned char nextShape[2] = {255, 255}; // next shape type (0 to 6). 255 = shape not defined
unsigned char shapeAngle[2] = {0, 0}; // shape rotation (0 to 3; every -90 degrees)
int shapeX[2] = {0, 0}, shapeY[2] = {0, 0};	// shape XY position
char shapeMap[2][BLOCK_SIZE]; // shape design (for example: "0000111001000000")
char nextShapeMap[2][BLOCK_SIZE]; // next shape design (for example: "0000111001000000")
char rotatedMap[2][BLOCK_SIZE]; // design of the already rotated shape
unsigned char colourShift = 0; // colour block scrolling effect in the title
BOOL autorepeatKeys = TRUE; // enables/disables autorepeat keys (settings menu)
BOOL background = TRUE; // enables/disables the chars in the pit (settings menu)
BOOL joystick = FALSE; // enables/disables joystick control (settings menu)
BOOL muted = FALSE; // enables/disables the sound effects and tunes (settings menu)
BOOL cancelled = FALSE; // TRUE when X is pressed during game play to exit
unsigned char lastLines; // lines in the last move (for the status line)
unsigned int lastPoints; // points in the last move (for the status line)
unsigned char backgroundChar[2]; // character to fill in the pits with backgroundCharList
const unsigned char backgroundCharList[8] = {42, 43, 39, 35, 28, 44, 47, 46 }; // * + . # \ , / .
unsigned int lastInputTime[2][3]; // time elapsed since last keystroke (0=rotate, 1=left, 2=right) 
unsigned char linesPiecesPlayed[2]; // counts lines/pieces played (to generate traps)
unsigned char previousLevel[2]; // to detect a change of player level

// pos 0-5: fake values for the initial TOP 6
// pos 6-7: values for the current game
char names[8][11] = {"DRAGON","DRAGON","DRAGON","DRAGON","DRAGON","DRAGON","",""};
unsigned int scores[8] = {2000, 1800, 1600, 1400, 1200, 1000, 0, 0};

// tunes
// hall of fame
const unsigned char tune1Notes[] = { 196, 196, 196, 204, 191, 206, 213 };
const unsigned char tune1Durations[] = { 4, 2, 2, 3, 3, 3, 7 };
// start of game
const unsigned char tune2Notes[] = { 190, 200, 210, 216, 200, 180, 190, 200 };
const unsigned char tune2Durations[] = { 2, 2, 2, 2, 4, 2, 2, 4 };
// game over
const unsigned char tune3Notes[] = { 170, 145, 155, 135, 150, 130, 140, 120, 110, 102 };
const unsigned char tune3Durations[] = { 3, 1, 3, 1, 3, 1, 2, 1, 2, 6 };


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
        printBlock(x, y, background ? backgroundChar[i] : EMPTY_BLOCK);
        return;
    }
    printBlock(x, y, FILLED_BLOCK + ((colour - 1) << 4)); // <<4 = x16
}


void drawPit(unsigned char i) {
    unsigned char x, y, rowOffset;
    // loop through and repaint the contents of the pit
    for (y = 0; y < PIT_HEIGHT; y++) {
        rowOffset = y * PIT_WIDTH;
        for (x = 0; x < PIT_WIDTH; x++)
            drawBlock(x, y, pit[i][x + rowOffset], i);
    }
}


void playTune(const unsigned char notes[], const unsigned char durations[], unsigned char numNotes) {
    if (muted) return; // sssshh!
    // plays the arrays of notes to form the melody
    for (unsigned char i = 0; i < numNotes; i++) {
        if (inkey() != '\0') return; // the melody ends
        sound(notes[i], durations[i]);
        if (inkey() != '\0') return; // another chance to finish the tune
    }
}


// check if a shape can move in the specified direction (i = player pit)
BOOL shapeCanMove(char *map, char xDir, char yDir, unsigned char i) {
    int x, y, blockX, blockY;
    // loop through all the blocks of the shape
    for (blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
        for (blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
            if (map[(SIDE_BLOCK_SIZE * blockY) + blockX] != NO_BLOCK) {
                // calculate the position in the pit
                x = shapeX[i] + blockX + xDir;
                y = shapeY[i] + blockY + yDir;
                // new shape appears at the top
                if (y < 0) // allow only vertical movement until fully visible
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
    unsigned char blockX, blockY, blockColour;
    // loop through all the blocks of the shape (4x4 grid)
    for (blockY = 0; blockY <= LAST_SIDE_BLOCK; blockY++) {
        for (blockX = 0; blockX <= LAST_SIDE_BLOCK; blockX++) {
            blockColour = nextShapeMap[i][(blockY * SIDE_BLOCK_SIZE) + blockX];
            if (blockColour == NO_BLOCK) blockColour = '1'; // green background
            else if (blockColour == '1') blockColour = '5'; // white over green
            // different position depending on the player
            drawBlock(i == 0 ? blockX + 17 : blockX - 5, blockY + y, blockColour, i);
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


// auxiliary function for drawing a player's status (2 player game)
void drawPlayerStatus(unsigned char i) {
    unsigned char yOffset = (i == 0) ? 0 : 8;
    locate(12, yOffset);  printf("DRAGON %u", i + 1);
    locate(11, yOffset + 1);  printf("LEVEL:  %2u", level[i]);
    locate(11, yOffset + 2);  printf("LINES: %3d", lines[i]);
    locate(11, yOffset + 3);  printf("SC: %6u", scores[i + 6]);
    locate(11, yOffset + 4);  printf("NEXT:");
    drawNextShape((i == 0) ? 4 : 12, i);
}


void displayStatus() {
    // 1 player only
    if (numPlayers == 0) {        
        drawHeader(TRUE, ++colourShift);        
        locate(11, 6);  printf("LEVEL:  %2u", level[0]);
        locate(11, 7);  printf("LINES: %3d", lines[0]);
        locate(11, 8);  printf("SC: %6u", scores[6]);
        locate(11, 9);  printf("HI: %6u", scores[6] > scores[0] ? scores[6] : scores[0]);
        locate(11, 11); printf("NEXT:");
        drawNextShape(10, 0);        
        // status line
        locate(11, 14); 
        if (lastLines > 0)          printf("%uLIN +%4u", lastLines, lastPoints);
        else if (lastPoints > 0)    printf("DROP   +%2u", lastPoints);
        else                        printf("          ");        
        lastLines = 0;
        lastPoints = 0;
    }
    else {
        drawPlayerStatus(0);  // Dragon 1
        drawPlayerStatus(1);  // Dragon 2
    }
    locate(14,15);
}


const char* getShapeMap(unsigned char shape) {
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


void getRotatedShapeMap(unsigned char shape, unsigned char angle, char *rotatedMap) {
    const char *map = getShapeMap(shape); // Unrotated map
    // no rotation needed, copy map directly
    if (angle == 0) {
        memcpy(rotatedMap, map, BLOCK_SIZE);
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


void createShape(unsigned char i) {
    /* calculates the fall speed based on the level...
        level 1: 36 delay ticks
        level 2: 32 delay ticks
        level 3: 28 delay ticks
        level 4: 24 delay ticks + lower trap lines
        level 5: 20 delay ticks + lower trap lines
        level 6: 16 delay ticks + upper trap blocks
        level 7: 12 delay ticks +   "     "    "
        level 8:  8 delay ticks +   "     "    "
        level x:  6 delay ticks +   "     "    "
    */
    dropRate[i] = (level[i] < 9) ? 
        (40 - (level[i] * DROP_RATE_LEVEL)) : MIN_DROP_RATE_LEVEL;
    // if it's not the first shape, take the value of nextShape
    shape[i] = (nextShape[i] != 255) ? nextShape[i] : (unsigned char)(rand() % 7); 
    shapeX[i] = 3; // centre of the pit
    shapeY[i] = -LAST_SIDE_BLOCK; // fully hidden
    shapeAngle[i] = 0; // unrotated
    memcpy(shapeMap[i], getShapeMap(shape[i]), BLOCK_SIZE);
    // generates the next shape
    nextShape[i] = (unsigned char) rand() % 7; // shape type (0 to 6)
    memcpy(nextShapeMap[i], getShapeMap(nextShape[i]), BLOCK_SIZE);
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


// bass sound when generating trap
void trapSound()
{
    if (!muted) {
        sound(60, 1);
        sound(0, 1);
    }
}


void setTrapLine(unsigned char i) {
    unsigned char x, y, emptyX;

    // with two players generates line in the opposing pit
    if (numPlayers == 1) {
        i = 1 - i;
        if (gameOver[i]) return;
    }

    // generates a random empty space on the new line
    emptyX = (unsigned char) rand() % PIT_WIDTH;

    // moves all rows up
    for (y = 1; y < PIT_HEIGHT; y++)
        for (x = 0; x < PIT_WIDTH; x++)
            pit[i][(y - 1) * PIT_WIDTH + x] = pit[i][y * PIT_WIDTH + x];

    // fill the last row with blocks, leaving an empty space.
    for (x = 0; x < PIT_WIDTH; x++)
        pit[i][(PIT_HEIGHT - 1) * PIT_WIDTH + x] = (x == emptyX) ? NO_BLOCK : '5'; // white 

    // redraws the pit at the moment (only when it goes to the opposite pit)
    if (numPlayers > 0) drawPit(i);
    trapSound();
}


void setTrapBlock(unsigned char i) {
    int x, y;
    unsigned char rowEmpty, attempts = 5;

    // with two players generates block in the opposing pit
    if (numPlayers == 1) {
        i = 1 - i;
        if (gameOver[i]) return;
    }

    while (attempts > 0) {
        // generate a random position from the sixth row downwards
        y = 5 + rand() % (PIT_HEIGHT - 5);
        // check if the row is empty
        rowEmpty = TRUE;
        for (x = 0; x < PIT_WIDTH; x++)
            if (pit[i][y * PIT_WIDTH + x] != NO_BLOCK) {
                rowEmpty = FALSE;
                break;
            }
        // we find an empty row, we place the block
        if (rowEmpty) {
            x = rand() % PIT_WIDTH;
            pit[i][y * PIT_WIDTH + x] = 5; // white block
            // paints the block at the moment when it goes to the opposite pit
            if (numPlayers > 0) printBlock(x + pitLeft[i], y, WHITE_BLOCK);
            trapSound();
            return;
        }
        attempts--;
    }
}


void checkForFullRows(unsigned char i) {
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
            if (numPlayers > 0 && level[i] > 3) // setting a trap
                linesPiecesPlayed[i]++;
        }
    }
    // updates the score if rows were completed
    if (numLines > 0) {
        const int pointsPerLine[] = {100, 300, 500, 800};
        lastPoints = pointsPerLine[numLines - 1] * level[i];

        // updates the total completed rows and calculates the level
        scores[j] += lastPoints;
        lines[i] += numLines;
        lastLines = numLines; // data for status line
        level[i] = (unsigned char)(lines[i] / LINES_LEVEL) + 1;

        // if it detects level change...
        if (previousLevel[i] < level[i]) {
            previousLevel[i] = level[i];
            // level background char
            if (background && level[i] < 9)
                backgroundChar[i] = backgroundCharList[level[i]-1];
            // level change sound
            if (!muted) {
                for (unsigned char k = 0; k < 245; k += 10)
                    sound(k,0);
            }
        }
    }
    // 1 player, every x pieces generates a trap line/block
    if (numPlayers == 0 && linesPiecesPlayed[0] >= PIECES_TRAP) {
        if (level[0] > 5) setTrapBlock(0);
        else if(level[0] > 3) setTrapLine(0);
        linesPiecesPlayed[0] = 0;
    }
    // 2 players, every x lines generates a trap line/block
    else if (numPlayers > 0 && linesPiecesPlayed[i] >= LINES_TRAP) {
        if (level[i] > 5) setTrapBlock(i);
        else if(level[i] > 3) setTrapLine(i);
        linesPiecesPlayed[i] = 0;
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
    if (numPlayers == 0 && level[0] > 3) // setting a trap
        linesPiecesPlayed[0]++;
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
        if (shapeY[i] < 0) {
            gameOver[i] = TRUE;
            locate(pitLeft[i], 8); printf("GAME OVER!");
            playTune(tune3Notes, tune3Durations, 10);
        }
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


// rounds the text window by drawing the corners
// offset: 128 +16:yellow +32:blue +48:red +64:white +80:cyan +96:magenta +112:orange 
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

    cls(0);
    locate(1, 2);  printf( "                             ");
    locate(1, 3);  printf( "        = s c o r e =        ");
    locate(1, 4);  printf( "                             ");
    locate(1, 5);  printf( " 1 LINE:  100 * LEVEL NUMBER ");
    locate(1, 6);  printf( " 2 LINES: 300 * LEVEL NUMBER ");
    locate(1, 7);  printf( " 3 LINES: 500 * LEVEL NUMBER ");
    locate(1, 8);  printf( " 4 LINES: 800 * LEVEL NUMBER ");
    locate(1, 9);  printf( "                             ");
    locate(1, 10); printf( "  SOFT DROP:  1 POINT * ROW  ");
    locate(1, 11); printf( "                             ");

	locate(0, 14); printf("   PRESS ANY KEY TO CONTINUE!   ");

    roundWindow(1, 2, 29, 11, 112);
    screen(0,1);
    waitkey(FALSE);
}


// cursor
void drawMenuPtr(unsigned char x, unsigned char y, unsigned char offset, BOOL delete) {
    unsigned char cursor = delete ? 143 : 62; // empty block : arrow
    printBlock(x, y + offset, cursor);
}


void drawSettingsMenu() {
	cls(1);
    roundWindow(0, 0, 31, 15, 80);
    locate(7, 8);  printf("AUTOREPEAT KEYS:");
    locate(7, 9);  printf("BACKGROUND:");
    locate(7, 10); printf("JOYSTICKS:");
    locate(7, 11); printf("MUTED:");
    locate(7, 12); printf("BACK");
    locate(2, 14); printf("SELECT OPTION (CURSOR/ENTER)");
    // on/off switches
    locate(24, 8); printf("%s", autorepeatKeys ? "on " : "off");
    locate(24, 9); printf("%s", background ? "on " : "off");
    locate(24, 10); printf("%s", joystick ? "on " : "off");
    locate(24, 11); printf("%s", muted ? "on " : "off");
}


void settingsMenu() {
    char optNumber = 0;
    drawSettingsMenu();
    drawMenuPtr(5, 8, optNumber, FALSE);
    do {
        drawHeader(FALSE, colourShift++);
        key = inkey();
        if (key == 10) { // cursor down
            drawMenuPtr(5, 8, optNumber, TRUE);
            if (optNumber++ == 4) optNumber = 0;
            drawMenuPtr(5, 8, optNumber, FALSE);
            if (!muted) sound(200,0);
        }
        else if (key == 94) { // cursor up
            drawMenuPtr(5, 8, optNumber, TRUE);
            if (optNumber-- == 0) optNumber = 4;
            drawMenuPtr(5, 8, optNumber, FALSE);
            if (!muted) sound(200,0);
        }
        else if (key == 13 || key == 32) { // enter or space bar
            if (!muted) sound(100,0);
            switch(optNumber) {
                case 0:
                    autorepeatKeys = !autorepeatKeys;                
                    break; 
                case 1:
                    background = !background;
                    break;
                case 2:
                    joystick = !joystick;                
                    break;  
                case 3:
                    muted = !muted;                
                    break;                                      
                case 4:
                    return;
            }
            drawSettingsMenu();
            drawMenuPtr(5, 8, optNumber, FALSE);
        }
        delay(2);
    } while (TRUE);
}


void drawMenu() {
	cls(1);
    roundWindow(0, 0, 31, 15, 80);
	locate(10, 7);  printf("1 DRAGON GAME");
    locate(10, 8);  printf("2 DRAGON GAME");
    locate(10, 9);  printf("HIGH SCORES");
    locate(10, 10); printf("SETTINGS");
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
                case 3: // settings menu
                    settingsMenu();
                    break;
                case 4: // show controls
                    drawHelp();
                    if (!muted) sound(100,0);
                    break;                
                case 5: // bye
                    cls(1);
                    printf("THANKS FOR PLAYING\nDRAGONTET 1.1!\n");
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


// logic to initialize the system
void init() {
    unsigned char i;
    setTimer(0); // initialises the system timer    
    menu();
	cls(1); // green screen
    roundWindow(PIT_WIDTH, 0, pitLeft[1]-1, 15, 0);
    // separators
    for (i = 1; i < PIT_HEIGHT-1; i++) {
        printBlock(PIT_WIDTH, i, 133);
        printBlock(SCREEN_WIDTH-PIT_WIDTH-1, i, 138);
    }

    for (i = 0; i < 2; i++) {
        level[i] = previousLevel[i] = 1; // initial level
        lines[i] = 0; // lines cleared
        scores[6+i] = 0; // current score
        nextShape[i] = 255; // shape generation will be required
        backgroundChar[i] = backgroundCharList[0]; // character to fill in the pits
        linesPiecesPlayed[i] = 0; // allows trap lines/blocks to be generated
        createShape(i); // generate shape (shape, position)
        memset(pit[i], NO_BLOCK, PIT_WIDTH * PIT_HEIGHT); // initialize the empty pit
        drawPit(i);
    }
    cancelled = FALSE;
    lastLines = 0;
    lastPoints = 0;
    displayStatus();
    if (numPlayers == 0) {
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
        memcpy(shapeMap[i], rotatedMap[i], BLOCK_SIZE);
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


void pauseKeyPressed() {
    while (inkey() != '\0'); // clear input buffer
    while (inkey() == '\0') delay(1); // wait for key press
    startTime[0] = startTime[1] = getTimer(); // reset timers
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


void mainLoop() {
    unsigned char i; // 0 = Dragon1, 1 = Dragon2

    playTune(tune2Notes, tune2Durations, 8);

    // initialise start times for both players
    startTime[0] = startTime[1] = getTimer();

    const byte *joystickPositions = NULL;

    while (TRUE) {
        if (joystick)
            joystickPositions = readJoystickPositions();

        /* Coco to Dragon key equivalences

        DRAGON  COCO
        ******  ****
        W       7       <-- player 1
        A       Q
        S       3
        D       T
        UP      SEMICOLON
        LEFT    HYPHEN
        DOWN    COMMA
        RIGHT   PERIOD

        I       Y       <-- player 2
        J       Z
        K       UP
        L       DOWN

        H       X       <-- common
        X       8
        ENTER   ENTER
        SPACE   SLASH

        */

#ifdef Dragon
        // check for pause, exit, or game over actions
        // pause
        if (isKeyPressed(KEY_PROBE_X, KEY_BIT_X)) {
            pauseKeyPressed();
            continue;
        // cancel/exit to main menu
        } else if (isKeyPressed(KEY_PROBE_8, KEY_BIT_8)) {
            if (!gameOver[0] || !gameOver[1])
                cancelled = TRUE;
            break;
        // enter/space after game over
        } else if ((isKeyPressed(KEY_PROBE_ENTER, KEY_BIT_ENTER) 
            || isKeyPressed(KEY_PROBE_SLASH, KEY_BIT_SLASH)) 
            && (gameOver[0] && gameOver[1])) {
            break;
        }
#endif
#ifdef Coco
        // check for pause, exit, or game over actions
        // pause
        if (isKeyPressed(KEY_PROBE_H, KEY_BIT_H)) {
            pauseKeyPressed();
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
#endif

        if (autorepeatKeys) {
#ifdef Dragon
            /////////////////// PLAYER 1 ///////////////////////
            if (!gameOver[0]) {
                // rotation (key, cursor, joystick)
                if (isKeyPressed(KEY_PROBE_7, KEY_BIT_7) 
                    || isKeyPressed(KEY_PROBE_SEMICOLON, KEY_BIT_SEMICOLON) 
                    || (joystick && joystickPositions[JOYSTK_LEFT_VERT] < JTHRESHOLD_LOW))
                    if (canProcessInput(0,0)) rotateKeyPressed(0);
                // move left (key, cursor, joystick)
                if (isKeyPressed(KEY_PROBE_Q, KEY_BIT_Q) 
                    || isKeyPressed(KEY_PROBE_HYPHEN, KEY_BIT_HYPHEN) 
                    || (joystick && joystickPositions[JOYSTK_LEFT_HORIZ] < JTHRESHOLD_LOW)) 
                    if (canProcessInput(0,1)) moveLeftKeyPressed(0);
                // fast drop (key, cursor, joystick)
                if (isKeyPressed(KEY_PROBE_3, KEY_BIT_3) 
                    || isKeyPressed(KEY_PROBE_COMMA, KEY_BIT_COMMA) 
                    || (joystick && joystickPositions[JOYSTK_LEFT_VERT] > JTHRESHOLD_HIGH)) 
                    dropRate[0] = 0;
                // move right (key, cursor, joystick)
                if (isKeyPressed(KEY_PROBE_T, KEY_BIT_T) 
                    || isKeyPressed(KEY_PROBE_PERIOD, KEY_BIT_PERIOD) 
                    || (joystick && joystickPositions[JOYSTK_LEFT_HORIZ] > JTHRESHOLD_HIGH)) 
                    if (canProcessInput(0,2)) moveRightKeyPressed(0);
            }
            //////////////////// PLAYER 2 ///////////////////////
            if (numPlayers > 0 && !gameOver[1]) {
                // rotation (key, cursor, joystick)
                if (isKeyPressed(KEY_PROBE_Y, KEY_BIT_Y) 
                    || (joystick && joystickPositions[JOYSTK_RIGHT_VERT] < JTHRESHOLD_LOW))
                    if (canProcessInput(1,0)) rotateKeyPressed(1);
                // move left (key joystick)
                if (isKeyPressed(KEY_PROBE_Z, KEY_BIT_Z) 
                    || (joystick && joystickPositions[JOYSTK_RIGHT_HORIZ] < JTHRESHOLD_LOW))
                    if (canProcessInput(1,1)) moveLeftKeyPressed(1);
                // fast drop (key, joystick)
                if (isKeyPressed(KEY_PROBE_UP, KEY_BIT_UP) 
                    || (joystick && joystickPositions[JOYSTK_RIGHT_VERT] > JTHRESHOLD_HIGH))
                    dropRate[1] = 0;
                // move right (key, joystick)
                if (isKeyPressed(KEY_PROBE_DOWN, KEY_BIT_DOWN) 
                    || (joystick && joystickPositions[JOYSTK_RIGHT_HORIZ] > JTHRESHOLD_HIGH))
                    if (canProcessInput(1,2)) moveRightKeyPressed(1);
            }            
#endif
#ifdef Coco
            /////////////////// PLAYER 1 ///////////////////////
            if (!gameOver[0]) {
                // rotation (key, cursor, joystick)
                if (isKeyPressed(KEY_PROBE_W, KEY_BIT_W) 
                    || isKeyPressed(KEY_PROBE_UP, KEY_BIT_UP) 
                    || (joystick && joystickPositions[JOYSTK_LEFT_VERT] < JTHRESHOLD_LOW))
                    if (canProcessInput(0,0)) rotateKeyPressed(0);
                // move left (key, cursor, joystick)
                if (isKeyPressed(KEY_PROBE_A, KEY_BIT_A) 
                    || isKeyPressed(KEY_PROBE_LEFT, KEY_BIT_LEFT) 
                    || (joystick && joystickPositions[JOYSTK_LEFT_HORIZ] < JTHRESHOLD_LOW))
                    if (canProcessInput(0,1)) moveLeftKeyPressed(0);
                // fast drop (key, cursor, joystick)
                if (isKeyPressed(KEY_PROBE_S, KEY_BIT_S) 
                    || isKeyPressed(KEY_PROBE_DOWN, KEY_BIT_DOWN) 
                    || (joystick && joystickPositions[JOYSTK_LEFT_VERT] > JTHRESHOLD_HIGH))
                    dropRate[0] = 0;
                // move right (key, cursor, joystick)
                if (isKeyPressed(KEY_PROBE_D, KEY_BIT_D) 
                    || isKeyPressed(KEY_PROBE_RIGHT, KEY_BIT_RIGHT) 
                    || (joystick && joystickPositions[JOYSTK_LEFT_HORIZ] > JTHRESHOLD_HIGH))
                    if (canProcessInput(0,2)) moveRightKeyPressed(0);
            }
            //////////////////// PLAYER 2 ///////////////////////
            if (numPlayers > 0 && !gameOver[1]) {
                // rotation (key, cursor, joystick)
                if (isKeyPressed(KEY_PROBE_I, KEY_BIT_I) 
                    || (joystick && joystickPositions[JOYSTK_RIGHT_VERT] < JTHRESHOLD_LOW))
                    if (canProcessInput(1,0)) rotateKeyPressed(1);
                // move left (key joystick)
                if (isKeyPressed(KEY_PROBE_J, KEY_BIT_J) 
                    || (joystick && joystickPositions[JOYSTK_RIGHT_HORIZ] < JTHRESHOLD_LOW))
                    if (canProcessInput(1,1)) moveLeftKeyPressed(1);
                // fast drop (key, joystick)
                if (isKeyPressed(KEY_PROBE_K, KEY_BIT_K) 
                    || (joystick && joystickPositions[JOYSTK_RIGHT_VERT] > JTHRESHOLD_HIGH))
                    dropRate[1] = 0;
                // move right (key, joystick)
                if (isKeyPressed(KEY_PROBE_L, KEY_BIT_L) 
                    || (joystick && joystickPositions[JOYSTK_RIGHT_HORIZ] > JTHRESHOLD_HIGH))
                    if (canProcessInput(1,2)) moveRightKeyPressed(1);
            }
#endif
        }
        else { // no autorepeat keys
            key = inkey(); // read keypresses
            /////////////////// PLAYER 1 ///////////////////////
            if (!gameOver[0]) {
                // rotation (key, cursor, joystick)
                if (key == 'W' || key == 94 || 
                    (joystick && joystickPositions[JOYSTK_LEFT_VERT] < JTHRESHOLD_LOW))
                    rotateKeyPressed(0);
                // move left (key, cursor, joystick)
                else if (key == 'A' || key == 8 || 
                    (joystick && joystickPositions[JOYSTK_LEFT_HORIZ] < JTHRESHOLD_LOW))
                    moveLeftKeyPressed(0);
                // fast drop (key, cursor, joystick)
                else if (key == 'S' || key == 10 || 
                    (joystick && joystickPositions[JOYSTK_LEFT_VERT] > JTHRESHOLD_HIGH))
                    dropRate[0] = 0;
                // move right (key, cursor, joystick)
                else if (key == 'D' || key == 9 || 
                    (joystick && joystickPositions[JOYSTK_LEFT_HORIZ] > JTHRESHOLD_HIGH))
                    moveRightKeyPressed(0);
            }
            //////////////////// PLAYER 2 ///////////////////////
            if (numPlayers > 0 && !gameOver[1]) {
                // rotation (key, joystick)
                if (key == 'I' || 
                    (joystick && joystickPositions[JOYSTK_RIGHT_VERT] < JTHRESHOLD_LOW))
                    rotateKeyPressed(1);
                // move left (key, joystick)
                else if (key == 'J' || 
                    (joystick && joystickPositions[JOYSTK_RIGHT_HORIZ] < JTHRESHOLD_LOW))
                    moveLeftKeyPressed(1);
                // fast drop (key, joystick)
                else if (key == 'K' || 
                    (joystick && joystickPositions[JOYSTK_RIGHT_VERT] > JTHRESHOLD_HIGH))
                    dropRate[1] = 0;
                // move right (key, joystick)
                else if (key == 'L' || 
                    (joystick && joystickPositions[JOYSTK_RIGHT_HORIZ] > JTHRESHOLD_HIGH))
                    moveRightKeyPressed(1);
            }        
        }
        // check if the falling time has been exceeded
        for (i = 0; i <= numPlayers; i++) {
            if (!gameOver[i] && getTimer() >= startTime[i] + dropRate[i]) {
                dropShape(i); // shape moves down
                startTime[i] = getTimer(); // reset fall timer
            }
        }
    }
}


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
