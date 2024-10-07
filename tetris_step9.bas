'TETRIS.BAS:
'   - original QBasic code by Peter Swinkels (QBBlocks v1.0)
'TETRIS_STEP1:
'   - understanding and comments
'   - shape 7 is enabled
'TETRIS_STEP2:
'   - converts to text mode
'TETRIS_STEP3:
'   - levels are added
'   - the score is widened
'TETRIS_STEP4:
'   - the random initial X position is deleted
'   - the initial random rotation is eliminated
'   - the direction of rotation is changed to the left
'   - the NEXT PIECE function is implemented.
'TETRIS_STEP5:
'   - 2 players
'   - 32x16 characters on screen, compatible with DRAGON 32/64
'TETRIS_STEP6:
'   - menu
'   - score table
'TETRIS_STEP7:
'   - BASIC classic no precedural
'TETRIS STEP8:
'   - optimisation and simplification
'TETRIS STEP9:
'   - converting ugBASIC (last step)

' ***TETRIS4DRAGON***
'  SALVAKANTERO 2024

DEFINE FLOAT PRECISION SINGLE

DIM true AS SIGNED BYTE = -1
DIM false AS SIGNED BYTE = 0
DIM numPlayers AS BYTE
DIM names$(6) AS STRING
DIM scores(6) AS WORD
DIM gameOver(2) AS BYTE
DIM dropRate(2) AS FLOAT
DIM startTime(2) AS FLOAT
DIM pit$(2) AS STRING
DIM level(2) AS BYTE
DIM lines(2) AS BYTE
DIM pitLeft(2) AS BYTE
DIM shape(2) AS BYTE
DIM shapeAngle(2) AS BYTE
DIM shapeMap$(2) AS STRING
DIM shapeX(2) AS BYTE
DIM shapeY(2) AS BYTE
DIM nextShape(2) AS BYTE
DIM nextShapeMap$(2) AS STRING

RANDOMIZE TIMER
pitLeft(0) = 1: pitLeft(1) = 23
FOR i = 0 TO 4
	names$(i) = "DRAGON"
	scores(i) = 1400-(i*100)
NEXT


