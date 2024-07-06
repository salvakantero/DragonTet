DEFINT A-Z

DECLARE FUNCTION GetRotatedShapeMap$ (Shape, Angle)
DECLARE FUNCTION GetShapeMap$ (Shape)
DECLARE FUNCTION RandomShapeX ()
DECLARE FUNCTION ShapeCanMove (Map$, XDirection, YDirection)
DECLARE SUB CheckForFullRows ()
DECLARE SUB CreateShape ()
DECLARE SUB DisplayStatus ()
DECLARE SUB DrawBlock (BlockColor$, PitX, PitY)
DECLARE SUB DrawPit ()
DECLARE SUB DrawShape (EraseShape)
DECLARE SUB DropShape ()
DECLARE SUB InitializeGame ()
DECLARE SUB Main ()
DECLARE SUB RemoveFullRow (RemovedRow)
DECLARE SUB SettleActiveShapeInPit ()

CONST BLOCKSCALE = 24
CONST FALSE = 0
CONST LASTSIDEBLOCK = 3
CONST NOBLOCK$ = "0"
CONST PITHEIGHT = 16
CONST PITLEFT = 8
CONST PITTOP = 2
CONST PITWIDTH = 10
CONST SIDEBLOCKCOUNT = 4
CONST TRUE = -1

COMMON SHARED DropRate!, GameOver, Pit$, Score, Shape, ShapeAngle, ShapeMap$, ShapeX, ShapeY

InitializeGame
Main

SUB CheckForFullRows ()
    FullRow = FALSE

    FOR PitY = 0 TO PITHEIGHT - 1
       FullRow = TRUE
       FOR PitX = 0 TO PITWIDTH - 1
           IF MID$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1) = NOBLOCK$ THEN
              FullRow = FALSE
              EXIT FOR
           END IF
       NEXT PitX
       IF FullRow THEN RemoveFullRow PitY
    NEXT PitY
END SUB

SUB CreateShape ()
    DropRate! = 1
    Shape = INT(RND * 6)
    ShapeAngle = INT(RND * 4)
    ShapeMap$ = GetRotatedShapeMap$(Shape, ShapeAngle)
    ShapeX = RandomShapeX
    ShapeY = -SIDEBLOCKCOUNT
END SUB

SUB DisplayStatus ()
    COLOR 4
    LOCATE INT((PITTOP * BLOCKSCALE) / 16), INT((PITLEFT * BLOCKSCALE) / 8) + 1
    IF GameOver THEN
        PRINT "Game over - press Enter to play a new game."
    ELSE
        PRINT "Score:" + STR$(Score)
    END IF
END SUB

SUB DrawBlock (BlockColor$, PitX, PitY)
    DrawX = PitX * BLOCKSCALE
    DrawY = PitY * BLOCKSCALE

    LINE (DrawX + (PITLEFT * BLOCKSCALE), DrawY + (PITTOP * BLOCKSCALE))-STEP(BLOCKSCALE, BLOCKSCALE), VAL("&H" + BlockColor$), BF
    LINE (DrawX + CINT(BLOCKSCALE / 10) + (PITLEFT * BLOCKSCALE), DrawY + CINT(BLOCKSCALE / 10) + (PITTOP * BLOCKSCALE))-STEP(BLOCKSCALE - CINT(BLOCKSCALE / 5), BLOCKSCALE - CINT(BLOCKSCALE / 5)), 0, B
END SUB

SUB DrawPit ()
    LINE ((PITLEFT * BLOCKSCALE) - 1, (PITTOP * BLOCKSCALE) - 1)-STEP((PITWIDTH * BLOCKSCALE) + 2, (PITHEIGHT * BLOCKSCALE) + 2), 15, B
    LINE ((PITLEFT * BLOCKSCALE) - 1, (PITTOP * BLOCKSCALE) - 1)-STEP(PITWIDTH * BLOCKSCALE + 2, 0), 0

    FOR PitY = 0 TO PITHEIGHT - 1
        FOR PitX = 0 TO PITWIDTH - 1
            IF GameOver THEN
                BlockColor$ = "4"
            ELSE
                BlockColor$ = MID$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1)
            END IF

            DrawBlock BlockColor$, PitX, PitY
        NEXT PitX
    NEXT PitY
END SUB

SUB DrawShape (EraseShape)
FOR BlockX = 0 TO LASTSIDEBLOCK
    FOR BlockY = 0 TO LASTSIDEBLOCK
        PitX = ShapeX + BlockX
        PitY = ShapeY + BlockY
        IF PitX >= 0 AND PitX < PITWIDTH AND PitY >= 0 AND PitY < PITHEIGHT THEN
            IF EraseShape THEN
                BlockColor$ = MID$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1)
            ELSE
                BlockColor$ = MID$(ShapeMap$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1)
                IF BlockColor$ = NOBLOCK$ THEN BlockColor$ = MID$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1)
            END IF
            DrawBlock BlockColor$, PitX, PitY
        END IF
    NEXT BlockY
NEXT BlockX
END SUB

SUB DropShape ()
    IF ShapeCanMove(ShapeMap$, 0, 1) THEN
        DrawShape TRUE
        IF DropRate! > 0 THEN SOUND 37, .3
        ShapeY = ShapeY + 1
        DrawShape FALSE
    ELSE
        SettleActiveShapeInPit
       
        GameOver = (ShapeY < 0)

        CheckForFullRows
        DrawPit
        DisplayStatus
       
        IF NOT GameOver THEN
            CreateShape
            DrawShape FALSE
        END IF
    END IF
END SUB

FUNCTION GetRotatedShapeMap$ (Shape, Angle)
    Map$ = GetShapeMap$(Shape)
    NewBlockX = 0
    NewBlockY = 0
    RotatedMap$ = STRING$(SIDEBLOCKCOUNT * SIDEBLOCKCOUNT, NOBLOCK$)

    IF Angle = 0 THEN
        GetRotatedShapeMap = Map$
        EXIT FUNCTION
    ELSE
        FOR BlockX = 0 TO LASTSIDEBLOCK
            FOR BlockY = 0 TO LASTSIDEBLOCK
                SELECT CASE Angle
                    CASE 1
                        NewBlockX = LASTSIDEBLOCK - BlockY
                        NewBlockY = BlockX
                    CASE 2
                        NewBlockX = LASTSIDEBLOCK - BlockX
                        NewBlockY = LASTSIDEBLOCK - BlockY
                    CASE 3
                        NewBlockX = BlockY
                        NewBlockY = LASTSIDEBLOCK - BlockX
                END SELECT

                MID$(RotatedMap$, ((SIDEBLOCKCOUNT * NewBlockY) + NewBlockX) + 1, 1) = MID$(Map$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1)
            NEXT BlockY
        NEXT BlockX
    END IF

    GetRotatedShapeMap = RotatedMap$
END FUNCTION

FUNCTION GetShapeMap$ (Shape)
    SELECT CASE Shape
        CASE 0
           Map$ = "0000333300000000"
        CASE 1
           Map$ = "0000111000100000"
        CASE 2
           Map$ = "0000666060000000"
        CASE 3
           Map$ = "00000EE00EE00000"
        CASE 4
           Map$ = "0000022022000000"
        CASE 5
           Map$ = "0000555005000000"
        CASE 6
           Map$ = "0000440004400000"
        CASE ELSE
           Map$ = ""
    END SELECT

    GetShapeMap$ = Map$
END FUNCTION

SUB InitializeGame ()
    RANDOMIZE TIMER
    PLAY "ML L64"

    SCREEN 12
    COLOR 9
    LOCATE 1, 1
    PRINT "QBBlocks v1.00 - by: Peter Swinkels, ***2019***"

    CreateShape

    GameOver = FALSE
    Pit$ = STRING$(PITWIDTH * PITHEIGHT, NOBLOCK$)
    Score = 0

    DrawPit
    DisplayStatus
END SUB

SUB Main ()
    StartTime! = TIMER
    DO
        Key$ = ""
        DO WHILE Key$ = ""
            IF NOT GameOver THEN
                IF TIMER >= StartTime! + DropRate! OR StartTime! > TIMER THEN
                    DropShape
                    StartTime! = TIMER
                END IF
            END IF
            Key$ = INKEY$
        LOOP
        IF Key$ = CHR$(27) THEN
            SCREEN 0
            END
        ELSEIF GameOver THEN
            IF Key$ = CHR$(13) THEN InitializeGame
        ELSE
            SELECT CASE Key$
               CASE "A", "a"
                   DrawShape TRUE
                   IF ShapeAngle = 3 THEN NewAngle = 0 ELSE NewAngle = ShapeAngle + 1
                   RotatedMap$ = GetRotatedShapeMap(Shape, NewAngle)
                   IF ShapeCanMove(RotatedMap$, 0, 0) THEN
                       ShapeAngle = NewAngle
                       ShapeMap$ = RotatedMap$
                   END IF
                   DrawShape FALSE
               CASE CHR$(0) + "K"
                   DrawShape TRUE
                   IF ShapeCanMove(ShapeMap$, -1, 0) THEN ShapeX = ShapeX - 1
                   DrawShape FALSE
               CASE CHR$(0) + "M"
                   DrawShape TRUE
                   IF ShapeCanMove(ShapeMap$, 1, 0) THEN ShapeX = ShapeX + 1
                   DrawShape FALSE
               CASE " "
                   DropRate! = 0
            END SELECT
        END IF
    LOOP
END SUB

FUNCTION RandomShapeX ()
    IntendedShapeX = INT(RND * (PITWIDTH - 1))
    ShapeX = 0

    FOR XMove = 0 TO IntendedShapeX
        IF ShapeCanMove(ShapeMap$, 1, 0) THEN
            ShapeX = ShapeX + 1
        ELSE
            EXIT FOR
        END IF
    NEXT XMove

    RandomShapeX = ShapeX
END FUNCTION

SUB RemoveFullRow (RemovedRow)
    FOR PitY = RemovedRow TO 0 STEP -1
        FOR PitX = 0 TO PITWIDTH - 1
            IF PitY = 0 THEN
                BlockColor$ = NOBLOCK$
            ELSE
                BlockColor$ = MID$(Pit$, ((PITWIDTH * (PitY - 1)) + PitX) + 1, 1)
            END IF

            MID$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1) = BlockColor$
        NEXT PitX
    NEXT PitY

    Score = Score + 1
END SUB

SUB SettleActiveShapeInPit ()
    PLAY "N21"

    FOR BlockY = 0 TO LASTSIDEBLOCK
        FOR BlockX = 0 TO LASTSIDEBLOCK
            PitX = ShapeX + BlockX
            PitY = ShapeY + BlockY
            IF PitX >= 0 AND PitX < PITWIDTH AND PitY >= 0 AND PitY < PITHEIGHT THEN
                IF NOT MID$(ShapeMap$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1) = NOBLOCK$ THEN
                   MID$(Pit$, ((PITWIDTH * PitY) + PitX) + 1, 1) = MID$(ShapeMap$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1)
                END IF
            END IF
        NEXT BlockX
    NEXT BlockY
END SUB

FUNCTION ShapeCanMove (Map$, XDirection, YDirection)
    FOR BlockY = 0 TO LASTSIDEBLOCK
        FOR BlockX = 0 TO LASTSIDEBLOCK
            IF NOT MID$(Map$, ((SIDEBLOCKCOUNT * BlockY) + BlockX) + 1, 1) = NOBLOCK$ THEN
                PitX = (ShapeX + BlockX) + XDirection
                PitY = (ShapeY + BlockY) + YDirection
                IF PitX >= 0 AND PitX < PITWIDTH AND PitY >= 0 AND PitY < PITHEIGHT THEN
                    IF NOT MID$(Pit$, (((PITWIDTH * PitY) + PitX) + 1), 1) = NOBLOCK$ THEN
                        ShapeCanMove = FALSE
                        EXIT FUNCTION
                    END IF
                ELSEIF PitX < 0 OR PitX >= PITWIDTH OR PitY >= PITHEIGHT THEN
                    ShapeCanMove = FALSE
                    EXIT FUNCTION
                END IF
            END IF
        NEXT BlockX
    NEXT BlockY

    ShapeCanMove = TRUE
END FUNCTION