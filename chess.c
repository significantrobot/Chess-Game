#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include "ass4.h"
#define SIZE 8


//MISC CONSTANTS
const char PieceList[6] = { 'P', 'R','N', 'B', 'Q', 'K' };
const char PAWN = 'P';
const char ROOK = 'R';
const char KNIGHT = 'N';
const char BISHOP = 'B';
const char QUEEN = 'Q';
const char KING = 'K';
const char CAPTURE = 'x';
const char PROMOTION = '=';
const char CHECK = '+';
const char MATE = '#';

//define in order to make functions in the code more readable
const int FALSE = 0;
const int TRUE = 1;
const int SUCCESS = 2;
const int FAIL = 3;

const char EMPTY = ' ';
const char FIRST_COL = 'a';
const char SEP[] = "/";

/*global flags in order to avoid the need
to pass them along as pointers across multiple functions
*/
int Cap = 0;
int Check = 0;
int Checkmate = 0;
int WhiteTurn = 0;
int Promotion = 0;
char PromPiece = '0';


/***********
*Function Name: isValidPiece
*Input: char Piece
*Output: int 0 ,1
*Function Operation:
    The function checks if the piece in the PGN input is a valid piece
    by comparing it to the list of valid pieces.
*************/

int isValidPiece(char Piece) {
    // flag if it is valid
    int Matches = 0;
    for (int i = 0; i < 6; i++) {
        if (PieceList[i] == Piece) {
            Matches = 1;
            //if valid piece
            return TRUE;
        }
    }
    //if not a valid piece
    return FALSE;
}

/***********
*Function Name: isValidCol
*Input: char col
*Output: int TRUE, FALSE
*Function Operation:
    The function checks if the column value in any given PGN input is valid.
*************/

int isValidCol(char col) {
    if (col >= 'a' && col <= 'h') {
        return TRUE;
    }
    return FALSE;
}

/***********
*Function Name: isValidRow
*Input: char col
*Output: int TRUE, FALSE
*Function Operation:
    The function checks if the row value in any given PGN input is valid.
*************/

int isValidRow(char row) {
    if (row >= '1' && row <= '8') {
        return TRUE;
    }
    return FALSE;
}

/***********
*Function Name: toDigit
*Input: char c
*Output: int
*Function Operation:
    Converts char to digit.
*************/

int toDigit(char c) {
    assert('0' <= c && c <= '9');
    return c - '0';
}

/***********
*Function Name: isLowercase
*Input: char c
*Output: int TRUE, FALSE
*Function Operation:
    Checks if char is lowercase.
*************/

int isLowercase(char c) {
    if (c >= 'a' && c <= 'z') {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/***********
*Function Name: isUppercase
*Input: char c
*Output: int TRUE, FALSE
*Function Operation:
    Checks if char is uppercase.
*************/

int isUppercase(char c) {
    if (c >= 'A' && c <= 'Z') {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/***********
*Function Name: isWhite
*Input: char Piece
*Output: int TRUE, FALSE
*Function Operation:
    Checks if a given piece is white.
    It only checks if the char is uppercase because if it wasn't a valid piece,
    it wouldn't have gotten to this test to begin with.
*************/

int isWhite(char Piece) {
    if (isUppercase(Piece)) {
        return TRUE;
    }
    else return FALSE;
}

/***********
*Function Name: isBlack
*Input: char Piece
*Output: int TRUE, FALSE
*Function Operation:
    Checks if a given piece is black.
    It only checks if the char is lowercase because if it wasn't a valid piece,
    it wouldn't have gotten to this test to begin with.
*************/

int isBlack(char Piece) {
    if (isLowercase(Piece)) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/***********
*Function Name: cmdLength
*Input: char pgn[]
*Output: int counter
*Function Operation:
    Counts how many move-related commands there are in a given pgn command.
    "Move-related" commands include piece, row/col it is moving from,
    row/col it is moving to, and whether it is capturing or not.

    "Non-move-related" commands are check '+', checkmate '#', or promotion '=X'.

*************/

int cmdLength(char pgn[]) {
    int counter = 0;
    for (int i = 0; i < strlen(pgn); i++) {
        if (pgn[i] == CHECK || pgn [i] == MATE) {
            counter++;
        }
        
        if (pgn[i] == PROMOTION) {
            counter = counter + 2;
        }
    }
    //counter represents number of non-move-related commands
    return strlen(pgn) - counter;
}

/***********
*Function Name: fillWithSize
*Input: int MultLocations[SIZE * SIZE][2]
*Output: void
*Function Operation:
    Fills an array that temporarily stores piece locations with SIZE,
    because SIZE is not a valid location on the board, and the content
    of the array can be navigated while MultLocations[][] != SIZE.

    This is necessary because (0,0) is a valid location on the board,
    and because there could be any number of temporarily locations that need
    to be inspected.
*************/

void fillWithSize(int MultLocations[][2]) {
    for (int i = 0; i < (SIZE * SIZE); i++) {
        for (int j = 0; j < 2; j++) {
        MultLocations[i][j] = SIZE;
        }
    }
}

/***********
*Function Name: checkMatePromPawn
*Input: char pgn[], char pgnmod[]
*Output: int TRUE, FALSE
*Function Operation:
    Inspects PGN command for non-move-related commands (check, mate, or promotion),
    and to see if the command is for a pawn.

    If the check/mate/promotion are in the input, then the corresponding global variable
    is flagged for future processing, and the pgn string is shortened to only move-related
    commands and stored in pgnmod[].

    This function also inspects to see if the command is given to a piece. If no piece
    is designated, then the command is for a pawn, and 'P' is added to the beginning
    of pgnmod[], for future processing.

*************/

int checkMatePromPawn(char pgn[], char pgnmod[]) {
    //Inspecting last, second to last, and third to last chars for +, #, and/or =X
    char LastChar = pgn[strlen(pgn) - 1];
    char SecLastChar = pgn[strlen(pgn) - 2];
    char ThirdLastChar = pgn[strlen(pgn) - 3];
    //stores how many chars need to removed from the end of pgn string for pgnmod
    int remove = 0;
    if (LastChar == CHECK) {
        //Triggers global variable "Check"
        Check = 1;
        remove = 1;
    }

    if (LastChar == MATE) {
        //Triggers global variable "Checkmate"
        Checkmate = 1;
        remove = 1;
    }

    if (SecLastChar == '=') {
        //Since pawns cannot be promoted to King or Pawn, =P or =K trigger Illegal Move
        if (isValidPiece(LastChar) == FALSE || LastChar == PAWN || LastChar == KING) {
            return FALSE;
        }
        //ONLY pawns can be promoted, so any other piece with =X command triggers Illegal Move
        if (isValidPiece(pgn[0])) {
            return FALSE;
        }
        //Stores the piece the pawn needs to be promoted to, for future use.
        PromPiece = LastChar;
        //Triggers global variable "Promotion"
        Promotion = 1;
        remove = 2;
    }

    if (ThirdLastChar == '=') {
        //Cannot promote to King/Pawn
        if (isValidPiece(SecLastChar) == FALSE || SecLastChar == PAWN || SecLastChar == KING) {
            return FALSE;
        }
        //Cannot promote non-pawn pieces
        if (isValidPiece(pgn[0])) {
            return FALSE;
        }
        PromPiece = SecLastChar;
        Promotion = 1;
        remove = 3;
    }


    //if it is a pawn command, 'P' is added to beginning of pgnmod[]
    if (isValidPiece(pgn[0]) == FALSE) {
        remove--;
        for (int i = 0; i < strlen(pgn) - remove; i++) {
            if (i == 0) {
                pgnmod[i] = PAWN;
            }
            else {
                pgnmod[i] = pgn[i - 1];
            }
        }
    }

    //if it is any other piece, + or # are removed
    else if (isValidPiece(pgn[0]) == TRUE) {
        for (int i = 0; i < strlen(pgn) - remove; i++) {
            pgnmod[i] = pgn[i];
        }
    }

    return TRUE;
}


/***********
*Function Name: findOppKingLocation
*Input: int KingLocation[], char board[][SIZE]
*Output: void
*Function Operation:
    Finds the locations of the opponent's king in order to check validity
    of check/mate commands. Stores location in KingLocation[]
*************/

void findOppKingLocation(int KingLocation[], char board[][SIZE]) {
    //if it is the white turn, looks for lowercase 'k' on board
    while (WhiteTurn == TRUE) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == KING + 32) {
                    KingLocation[0] = i;
                    KingLocation[1] = j;
                    return;
                }
            }
        }
    }
    //if black turn, looks for uppercase 'K'
    while (WhiteTurn == FALSE) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == KING) {
                    KingLocation[0] = i;
                    KingLocation[1] = j;
                    return;
                }
            }
        }
    }

}

/***********
*Function Name: findSameKingLocation
*Input: int KingLocation[], char board[][SIZE]
*Output: void
*Function Operation:
    Finds the locations of the player's king in order to check
    if the move endangers the king. Stores location in KingLocation[]
*************/

void findSameKingLocation(int KingLocation[], char board[][SIZE]) {
    //finds uppercase 'K' if white turn
    while (WhiteTurn == TRUE) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == KING) {
                    KingLocation[0] = i;
                    KingLocation[1] = j;
                    return;
                }
            }
        }
    }
    //finds lowercase 'k' if black turn
    while (WhiteTurn == FALSE) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == KING + 32) {
                    KingLocation[0] = i;
                    KingLocation[1] = j;
                    return;
                }
            }
        }
    }

}



/**************************
* CREATE BOARD
 **************************/

 /***********
*Function Name: placePieces
*Input: char row[], char board[][SIZE], int whichrow
*Output: void
*Function Operation:
    Virtually places the pieces from the FEN row in the appropriate location
    on the board.

    For every iteration of this function, int whichrow will increase by one.
    This tells the function which row  on board it is filling per iteration.
*************/
void placePieces(char row[], char board[][SIZE], int whichrow) {
    int indexofFEN = 0, placeinrow = 0;
    while (row[indexofFEN]) {
        if (isdigit(row[indexofFEN])) {
            int nopiece = toDigit(row[indexofFEN++]);
            for (int j = 0; j < nopiece; j++) {
                board[whichrow][placeinrow++] = EMPTY;
            }
        }
        else {
            board[whichrow][placeinrow++] = row[indexofFEN++];
        }
    }
}

/***********
*Function Name: createBoard
*Input: char board[][], char fen[]
*Output: void
*Function Operation:
    Uses strtok function to split fen string into multiple strings
    at the '/', and then sends each token string to placePieces
    for each row to be filled, according to whichrow.
*************/

void createBoard(char board[][SIZE], char fen[]) {
    int whichrow = 0;
    char* fenRow = strtok(fen, SEP);
    while (fenRow != NULL) {
        placePieces(fenRow, board, whichrow);
        fenRow = strtok(NULL, SEP);
        whichrow++;
    }
}

/******************
* END
CREATE BOARD
*******************/


/**************************
* PRINT BOARD
 **************************/

 /***********
 *Function Name: printColumnLetters
 *Input: void
 *Output: void
 *Function Operation:
     Prints top/bottom row of chess board, with column letters.
 *************/

void printColumnLetters() {
    char column = toupper(FIRST_COL);
    printf("* |");
    for (int i = 0; i < SIZE; i++) {
        if (i) {
            printf(" ");
        }
        printf("%c", column);
        column++;
    }
    printf("| *\n");
}

/***********
 *Function Name: printSpacers
 *Input: void
 *Output: void
 *Function Operation:
     Prints row of spacers '-' on board top/bottom.
 *************/

void printSpacers() {
    printf("* -");
    for (int i = 0; i < SIZE; i++) {
        printf("--");
    }
    printf(" *\n");
}


/***********
 *Function Name: printBoard
 *Input: board
 *Output: void
 *Function Operation:
     Prints the board, including the piece characters in their respective locations.
 *************/
void printBoard(char board[][SIZE]) {
    printColumnLetters();
    printSpacers();
    int rowNumber = SIZE;
    for (int row = 0; row < SIZE; row++) {
        printf("%d ", rowNumber);
        for (int column = 0; column < SIZE; column++) {
            printf("|%c", board[row][column]);
        }
        printf("| %d\n", rowNumber--);
    }
    printSpacers();
    printColumnLetters();

}


/************
END
PRINT BOARD
***********/






/*
ROOK ACTIONS
*/


/***********
 *Function Name: rookObstacleTest
 *Input: int TargetLocation[2], int CurrentLocation[2], board[][SIZE]
 *Output: int SUCCESS,FAIL
 *Function Operation:
     Inspects if a ROOK command is legal. If there are any obstacles,
     or if a capture is taking place without there having been "x"
     in the PGN command (which would have triggered Cap = TRUE),
     then the move is illegal, and FAIL is returned.
 *************/

int rookObstacleTest(int TargetLocation[], int CurrentLocation[], char board[][SIZE]) {
    int CurrentRow = CurrentLocation[0], CurrentCol = CurrentLocation[1];
    int TargetRow = TargetLocation[0], TargetCol = TargetLocation[1];

    //MOVING TO THE LEFT
    if (CurrentRow == TargetRow && TargetCol < CurrentCol) {
        for (int i = CurrentCol - 1; i >= TargetCol; i--) {
            //LochCheck is the piece in the location being inspected.
            char LocCheck = board[CurrentRow][i];
            if (LocCheck != EMPTY) {
                //Checks if a legal capture is taking place for white/black turns.
                while (WhiteTurn == TRUE) {
                    if (isBlack(LocCheck) && i == TargetCol && Cap == TRUE) {
                        return SUCCESS;
                    }
                    return FAIL;
                }

                while (WhiteTurn == FALSE) {
                    if (isWhite(LocCheck) && i == TargetCol && Cap == TRUE) {
                        return SUCCESS;
                    }
                    return FAIL;
                }
            }
        }
    }


    //MOVING TO THE RIGHT. The logic is identical, only different direction parameters
    else if (CurrentRow == TargetRow && TargetCol > CurrentCol) {
        for (int i = CurrentCol + 1; i <= TargetCol; i++) {
            char LocCheck = board[CurrentRow][i];
            if (LocCheck != EMPTY) {
                while (WhiteTurn == TRUE) {
                    if (isBlack(LocCheck) && i == TargetCol && Cap == TRUE) {
                        return SUCCESS;
                    }
                    return FAIL;
                }

                while (WhiteTurn == FALSE) {
                    if (isWhite(LocCheck) && i == TargetCol && Cap == TRUE) {
                        return SUCCESS;
                    }
                    return FAIL;
                }
            }
        }  
    }


    //MOVING UP
    else if (CurrentCol == TargetCol && TargetRow < CurrentRow) {
        for (int i = CurrentRow - 1; i >= TargetRow; i--) {
            char LocCheck = board[i][CurrentCol];
            if (LocCheck != EMPTY) {
                while (WhiteTurn == TRUE) {
                    if (isBlack(LocCheck) && i == TargetRow && Cap == TRUE) {
                        return SUCCESS;
                    }
                    return FAIL;
                }

                while (WhiteTurn == FALSE) {
                    if (isWhite(LocCheck) && i == TargetRow && Cap == TRUE) {
                        return SUCCESS;
                    }
                    return FAIL;
                }
            }
        }
    }


    //MOVING DOWN
    else if (CurrentCol == TargetCol && TargetRow > CurrentRow) {
        for (int i = CurrentRow + 1; i <= TargetRow; i++) {
            char LocCheck = board[i][CurrentCol];
            if (LocCheck != EMPTY) {
                while (WhiteTurn == TRUE) {
                    if (isBlack(LocCheck) && i == TargetRow && Cap == TRUE) {
                        return SUCCESS;
                    }
                    return FAIL;
                }

                while (WhiteTurn == FALSE) {
                    if (isWhite(LocCheck) && i == TargetRow && Cap == TRUE) {
                        return SUCCESS;
                    }
                    return FAIL;
                }
            }
        }
    }


    //If the direction is illegal, FAIL is returned.
    if (CurrentRow != TargetRow && CurrentCol != TargetCol) {
        return FAIL;
    }
    
    //SUCCESS is return if no obstacles are found and there is no capture.
    return SUCCESS;
}

/***********
 *Function Name: multRookAccesss
 *Input: int LoxnsCanAccess[][2], TargetLocation[], MultLocations[][2], char board[][SIZE]
 *Output: int counter
 *Function Operation:
     Counts how many ROOKS can access the target location, and the return is used
     to determine whether the PGN command is legal relative to the situation on the board.

     E.g., if two rooks can access the target location, but the PGN does not specify
     which rook it is referring to, the move will be illegal.
 *************/

int multRookAccess(int LoxnsCanAccess[][2], int TargetLocation[], int MultLocations[][2], char board[][SIZE]) {
    int k = 0, counter = 0;
    int TestLocation[2];
    for (int TestPiece = 0; MultLocations[TestPiece][0] != SIZE; TestPiece++) {
        TestLocation[0] = MultLocations[TestPiece][0];
        TestLocation[1] = MultLocations[TestPiece][1];
        if (rookObstacleTest(TargetLocation, TestLocation, board) == SUCCESS) {
            LoxnsCanAccess[k][0] = TestLocation[0];
            LoxnsCanAccess[k][1] = TestLocation[1];
            k++;
            counter++;
        }
    }
    return counter;
}

/**
END ROOK ACTIONS
**/




/*
BISHOP ACTIONS
*/

/***********
 *Function Name: bishopMoveValidity
 *Input: int CheckRow, CheckCol, TargetLocation[], char LocCheck
 *Output: int FAIL, SUCCESS
 *Function Operation:
    In the event that the bishop's target location is occupied by another piece,
    this function tests if Cap = TRUE (if 'x' was in the PGN input) and if the piece
    is a piece that can be capture (black for white and white for black).
 *************/

int bishopMoveValidity(int CheckRow, int CheckCol, int TargetLocation[], char LocCheck) {
    while (WhiteTurn == TRUE) {
        if (CheckRow == TargetLocation[0] && CheckCol == TargetLocation[1] && isBlack(LocCheck) && Cap == TRUE) {
            return SUCCESS;
        }
        return FAIL;
    }

    while (WhiteTurn == FALSE) {
        if (CheckRow == TargetLocation[0] && CheckCol == TargetLocation[1] && isWhite(LocCheck) && Cap == TRUE) {
            return SUCCESS;
        }
        return FAIL;

    }
    return FAIL;
}

/***********
 *Function Name: bishopObstacleTest
 *Input: int TargetLocation[2], int CurrentLocation[2], board[][SIZE]
 *Output: int SUCCESS,FAIL
 *Function Operation:
     Inspects if a BISHOP command is legal. If there are any obstacles,
     or if a capture is taking place without there having been "x"
     in the PGN command, then the move is illegal,
     and FAIL is returned.
 *************/

int bishopObstacleTest(int TargetLocation[], int CurrentLocation[], char board[][SIZE]) {
    int CurrentRow = CurrentLocation[0], CurrentCol = CurrentLocation[1];
    int TargetRow = TargetLocation[0], TargetCol = TargetLocation[1];
    int Vertical, Horizontal;

    //MOVING DOWN AND RIGHT
    if (TargetCol > CurrentCol&& TargetRow > CurrentRow) {
        for (int CheckRow = CurrentRow + 1; CheckRow <= TargetRow; CheckRow++) {
            for (int CheckCol = CurrentCol + 1; CheckCol <= TargetCol; CheckCol++) {
                Vertical = abs(CurrentRow - CheckRow);
                Horizontal = abs(CurrentCol - CheckCol);
                //If the direction is legal
                if (Vertical - Horizontal == 0) {
                    //if the TargetLocation is occupied
                    char LocCheck = board[CheckRow][CheckCol];
                    if (LocCheck != EMPTY) {
                        return bishopMoveValidity(CheckRow, CheckCol, TargetLocation, LocCheck);
                    }
                }
            }
        }
    }

    //MOVING DOWN AND LEFT. Logic is identical, different direction
    else if (TargetCol < CurrentCol && TargetRow > CurrentRow) {
        for (int CheckRow = CurrentRow + 1; CheckRow <= TargetRow; CheckRow++) {
            for (int CheckCol = CurrentCol - 1; CheckCol >= TargetCol; CheckCol--) {
                Vertical = abs(CurrentRow - CheckRow);
                Horizontal = abs(CurrentCol - CheckCol);
                if (Vertical - Horizontal == 0) {
                    char LocCheck = board[CheckRow][CheckCol];
                    if (LocCheck != EMPTY) {
                        return bishopMoveValidity(CheckRow, CheckCol, TargetLocation, LocCheck);
                    }
                }
            }
        }
    }

    //MOVING UP AND RIGHT
    else if (TargetCol > CurrentCol&& TargetRow < CurrentRow) {
        for (int CheckRow = CurrentRow - 1; CheckRow >= TargetRow; CheckRow--) {
            for (int CheckCol = CurrentCol + 1; CheckCol <= TargetCol; CheckCol++) {
                Vertical = abs(CurrentRow - CheckRow);
                Horizontal = abs(CurrentCol - CheckCol);
                if (Vertical - Horizontal == 0) {
                    char LocCheck = board[CheckRow][CheckCol];
                    if (LocCheck != EMPTY) {
                        return bishopMoveValidity(CheckRow, CheckCol, TargetLocation, LocCheck);
                    }
                }
            }
        }
    }

    //MOVING UP AND LEFT
    else if (TargetCol < CurrentCol && TargetRow < CurrentRow) {
        for (int CheckRow = CurrentRow - 1; CheckRow >= TargetRow; CheckRow--) {
            for (int CheckCol = CurrentCol - 1; CheckCol >= TargetCol; CheckCol--) {
                Vertical = abs(CurrentRow - CheckRow);
                Horizontal = abs(CurrentCol - CheckCol);
                if (Vertical - Horizontal == 0) {
                    char LocCheck = board[CheckRow][CheckCol];
                    if (LocCheck != EMPTY) {
                        return bishopMoveValidity(CheckRow, CheckCol, TargetLocation, LocCheck);
                    }
                }
            }
        }
    }

    Vertical = abs(CurrentRow - TargetRow);
    Horizontal = abs(CurrentCol - TargetCol);
    //If the direction is illegal to begin with
    if (Vertical - Horizontal != 0) {
        return FAIL;
    }

    //If the direction is legal and there are no obstacles.
    return SUCCESS;
}

/***********
 *Function Name: multBishopAccesss
 *Input: int LoxnsCanAccess[][2], TargetLocation[], MultLocations[][2], char board[][SIZE]
 *Output: int counter
 *Function Operation:
     Counts how many Bishops can access the target location.

     Similar to multRookAccess.
 *************/

int multBishopAccess(int LoxnsCanAccess[][2], int TargetLocation[], int MultLocations[][2], char board[][SIZE]) {
    int k = 0, counter = 0;
    int TestLocation[2];
    for (int TestPiece = 0; MultLocations[TestPiece][0] != SIZE; TestPiece++) {
        TestLocation[0] = MultLocations[TestPiece][0];
        TestLocation[1] = MultLocations[TestPiece][1];
        if (bishopObstacleTest(TargetLocation, TestLocation, board) == SUCCESS) {
            LoxnsCanAccess[k][0] = TestLocation[0];
            LoxnsCanAccess[k][1] = TestLocation[1];
            k++;
            counter++;
        }
    }
    return counter;
}


/**
END BISHOP ACTIONS
**/


/*
QUEEN ACTIONS
*/

/***********
 *Function Name: queenObstacleTest
 *Input: int TargetLocation[2], int CurrentLocation[2], board[][SIZE]
 *Output: int SUCCESS,FAIL
 *Function Operation:
     Inspects if a QUEEN command is legal by inspecting its legality
     for Up/down/left/right actions (rook actions) and diagonal actions
     (bishop actions).
 *************/

int queenObstacleTest(int TargetLocation[], int CurrentLocation[], char board[][SIZE]) {
    //Checks legality of up/down/left/right moves
    int UDLRTest = rookObstacleTest(TargetLocation, CurrentLocation, board);
    //checks legality of diagonal moves
    int DiagonalTest = bishopObstacleTest(TargetLocation, CurrentLocation, board);
    int CurrentRow = CurrentLocation[0], CurrentCol = CurrentLocation[1];
    int TargetRow = TargetLocation[0], TargetCol = TargetLocation[1];

    //If the target is in a diagonal direction and if DiagonalTest was successful
    if (abs(CurrentRow - TargetRow) == abs(CurrentCol - TargetCol) && DiagonalTest == SUCCESS) {
        return SUCCESS;
    }
    //If the target is in up/down/left/right direction and UDLRTest was successful
    else if (UDLRTest == SUCCESS && (CurrentRow == TargetRow || CurrentCol == TargetCol)) {
        return SUCCESS;

    }

    //If the direction is not diagonal/UDLR direction, the move is illegal.
    else return FAIL;
}

/***********
 *Function Name: multQueenAccesss
 *Input: int LoxnsCanAccess[][2], TargetLocation[], MultLocations[][2], char board[][SIZE]
 *Output: int counter
 *Function Operation:
     Counts how many Queens can access the target location,
     relative to up/down/left/right directions and diagonal directions.

     This is relevant if a Pawn gets promoted to Queen.
     
 *************/

int multQueenAccess(int LoxnsCanAccess[][2], int TargetLocation[], int MultLocations[][2], char board[][SIZE]) {
    int QueenUDLR[SIZE * SIZE][2];
    fillWithSize(QueenUDLR);
    int QueenDiag[SIZE * SIZE][2];
    fillWithSize(QueenDiag);
    //Counts how many queens can access from UDLR
    int UDLRAccess = multRookAccess(QueenUDLR, TargetLocation, MultLocations, board);
    //Counts how many queens can access from diagonal direction
    int DiagonalAccess = multBishopAccess(QueenDiag, TargetLocation, MultLocations, board);
    
    //Records the locations of UDLR-accessible queens in LoxnsCanAccess
    int p = 0;
    for (int k = 0; QueenUDLR[k][0] != SIZE; k++) {
        LoxnsCanAccess[k][0] = QueenUDLR[k][0];
        LoxnsCanAccess[k][1] = QueenUDLR[k][1];
        p++;
    }

    //Records the locations of diagonal-accessible queens, after the UDLR queens
    int r = 0;
    for (p, r; QueenDiag[r][0] != SIZE; p++, r++) {
        LoxnsCanAccess[p][0] = QueenDiag[r][0];
        LoxnsCanAccess[p][1] = QueenDiag[r][1];
    }

    //Queens' locations have been stored, returns total number of queens that can access
    return (UDLRAccess + DiagonalAccess);
}

/*
END QUEEN ACTIONS
*/



/*
KING ACTIONS
*/

/***********
 *Function Name: kingMoveValidity
 *Input: int CheckRow, CheckCol, TargetLocation[], char LocCheck
 *Output: int FAIL, SUCCESS
 *Function Operation:
    In the event that the king's target location is occupied by another piece,
    this function tests if Cap = TRUE (if 'x' was in the PGN input) and if the piece
    is a piece that can be capture (black for white and white for black).

    If the target location is occupied by another King, the move is flagged
    as illegal by a different function (Player move cannot risk the king).
 *************/

int kingMoveValidity(char LocCheck) {
    if (LocCheck != EMPTY) {
        while (WhiteTurn == TRUE) {
            if (isBlack(LocCheck) && Cap == TRUE) {
                return SUCCESS;
            }
            return FAIL;
        }


        while (WhiteTurn == FALSE) {
            if (isWhite(LocCheck) && Cap == TRUE) {
                return SUCCESS;
            }
            return FAIL;
        }
    }
    return SUCCESS;
}

/***********
 *Function Name: kingObstacleTest
 *Input: int TargetLocation[2], int CurrentLocation[2], board[][SIZE]
 *Output: int SUCCESS,FAIL
 *Function Operation:
     Inspects if a KING command is legal. If there are any obstacles,
     or if a capture is taking place without there having been "x"
     in the PGN command, then the move is illegal,
     and FAIL is returned.
 *************/

int kingObstacleTest(int TargetLocation[], int CurrentLocation[], char board[][SIZE]) {
    int CurrentRow = CurrentLocation[0], CurrentCol = CurrentLocation[1];
    int TargetRow = TargetLocation[0], TargetCol = TargetLocation[1];
    char LocCheck = board[TargetRow][TargetCol];
    
    //Move up
    if (TargetCol == CurrentCol && TargetRow == CurrentRow - 1){
        return kingMoveValidity(LocCheck);
    }

    //Move down
    else if (TargetCol == CurrentCol && TargetRow == CurrentRow + 1) {
        return kingMoveValidity(LocCheck);
    }

    //Move left
    else if (TargetRow == CurrentRow && TargetCol == CurrentCol - 1) {
        return kingMoveValidity(LocCheck);
    }

    //Move right
    else if (TargetRow == CurrentRow && TargetCol == CurrentCol + 1) {
        return kingMoveValidity(LocCheck);
    }

    //Move up and right
    else if (TargetRow == CurrentRow - 1 && TargetCol == CurrentCol + 1) {
        return kingMoveValidity(LocCheck);
    }

    //Move up and left
    else if (TargetRow == CurrentRow - 1 && TargetCol == CurrentCol - 1) {
        return kingMoveValidity(LocCheck);
    }

    //Move down and right
    else if (TargetRow == CurrentRow + 1 && TargetCol == CurrentCol + 1) {
        return kingMoveValidity(LocCheck);
    }

    //Move down and left
    else if (TargetRow == CurrentRow + 1 && TargetCol == CurrentCol - 1) {
        return kingMoveValidity(LocCheck);
    }

    //Any command other than those listed above is illegal
    return FAIL;
}

/***********
 *Function Name: multKingAccess
 *Input: int TargetLocation[2], int CurrentLocation[2], board[][SIZE]
 *Output: int SUCCESS,FAIL
 *Function Operation:
     Counts how many kings can access the target location.

     This is only relevant if the metargelim are feeling particularly
     sadistic and want to invent a new kind of "chess" game that has more than one king.

     See if it works, I dare you. If it doesn't work, I will have brought
     great shame and dishonor upon my family.
 *************/

int multKingAccess(int LoxnsCanAccess[][2], int TargetLocation[], int MultLocations[][2], char board[][SIZE]) {
    int k = 0, counter = 0;
    int TestLocation[2];
    for (int TestPiece = 0; MultLocations[TestPiece][0] != SIZE; TestPiece++) {
        TestLocation[0] = MultLocations[TestPiece][0];
        TestLocation[1] = MultLocations[TestPiece][1];
        if (kingObstacleTest(TargetLocation, TestLocation, board) == SUCCESS) {
            LoxnsCanAccess[k][0] = TestLocation[0];
            LoxnsCanAccess[k][1] = TestLocation[1];
            k++;
            counter++;
        }
    }
    return counter;

}

/*
END KING ACTIONS
*/


/*
KNIGHT ACTIONS
*/

/***********
 *Function Name: knightMoveValidity
 *Input: int CheckRow, CheckCol, TargetLocation[], char LocCheck
 *Output: int FAIL, SUCCESS
 *Function Operation:
    In the event that the knight's target location is occupied by another piece,
    this function tests if Cap = TRUE (if 'x' was in the PGN input) and if the piece
    is a piece that can be capture (black for white and white for black).

 *************/

int knightMoveValidity(char LocCheck) {
    if (LocCheck != EMPTY) {
        while (WhiteTurn == TRUE) {
            if (isBlack(LocCheck) && Cap == TRUE) {
                return SUCCESS;
            }
            return FAIL;
        }


        while (WhiteTurn == FALSE) {
            if (isWhite(LocCheck) && Cap == TRUE) {
                return SUCCESS;
            }
            return FAIL;
        }
    }
    return SUCCESS;
}

/***********
 *Function Name: knightObstacleTest
 *Input: int TargetLocation[2], int CurrentLocation[2], board[][SIZE]
 *Output: int SUCCESS,FAIL
 *Function Operation:
     Inspects if a KNIGHT command is legal. If there are any obstacles,
     or if a capture is taking place without there having been "x"
     in the PGN command, then the move is illegal,
     and FAIL is returned.
 *************/

int knightObstacleTest(int TargetLocation[], int CurrentLocation[], char board[][SIZE]) {
    int CurrentRow = CurrentLocation[0], CurrentCol = CurrentLocation[1];
    int TargetRow = TargetLocation[0], TargetCol = TargetLocation[1];
    char LocCheck = board[TargetRow][TargetCol];

    //up up right
    if (TargetRow == CurrentRow - 2 && TargetCol == CurrentCol + 1) {
        return knightMoveValidity(LocCheck);
    }

    //up right right
    else if (TargetRow == CurrentRow - 1 && TargetCol == CurrentCol + 2) {
        return knightMoveValidity(LocCheck);
    }

    //down right right
    else if (TargetRow == CurrentRow + 1 && TargetCol == CurrentCol + 2) {
        return knightMoveValidity(LocCheck);
    }

    //down down right
    else if (TargetRow == CurrentRow + 2 && TargetCol == CurrentCol + 1) {
        return knightMoveValidity(LocCheck);
    }

    //down down left
    else if (TargetRow == CurrentRow + 2 && TargetCol == CurrentCol - 1) {
        return knightMoveValidity(LocCheck);
    }

    //down left lft
    else if (TargetRow == CurrentRow + 1 && TargetCol == CurrentCol - 2) {
        return knightMoveValidity(LocCheck);
    }

    //up left left
    else if (TargetRow == CurrentRow - 1 && TargetCol == CurrentCol - 2) {
        return knightMoveValidity(LocCheck);
    }

    //up up left
    else if (TargetRow == CurrentRow - 2 && TargetCol == CurrentCol - 1) {
        return knightMoveValidity(LocCheck);
    }

    //Anything other than the above-listed moves is illegal.
    return FAIL;
}

/***********
 *Function Name: multKingAccess
 *Input: int TargetLocation[2], int CurrentLocation[2], board[][SIZE]
 *Output: int SUCCESS,FAIL
 *Function Operation:
     Counts how many knights can access the target location.

     Similar to multRookAccess.

 *************/

int multKnightAccess(int LoxnsCanAccess[][2], int TargetLocation[], int MultLocations[][2], char board[][SIZE]) {
    int k = 0, counter = 0;
    int TestLocation[2];
    for (int TestPiece = 0; MultLocations[TestPiece][0] != SIZE; TestPiece++) {
        TestLocation[0] = MultLocations[TestPiece][0];
        TestLocation[1] = MultLocations[TestPiece][1];
        if (knightObstacleTest(TargetLocation, TestLocation, board) == SUCCESS) {
            LoxnsCanAccess[k][0] = TestLocation[0];
            LoxnsCanAccess[k][1] = TestLocation[1];
            k++;
            counter++;
        }
    }
    return counter;

}

/*
END KNIGHT ACTIONS
*/



/*
PAWN ACTIONS
*/


/***********
 *Function Name: pawnMoveValidity
 *Input: int CheckRow, CheckCol, TargetLocation[], char LocCheck
 *Output: int FAIL, SUCCESS
 *Function Operation:
    In the event that the knight's target location is occupied by another piece,
    this function tests if Cap = TRUE (if 'x' was in the PGN input) and if the piece
    is a piece that can be capture (black for white and white for black).

 *************/

int pawnMoveValidity(char LocCheck) {
    while (WhiteTurn == TRUE) {
        if (isBlack(LocCheck) && Cap == TRUE) {
            return SUCCESS;
        }
        return FAIL;
    }


    while (WhiteTurn == FALSE) {
        if (isWhite(LocCheck) && Cap == TRUE) {
            return SUCCESS;
        }
        return FAIL;
    }

    return FAIL;
}

/***********
 *Function Name: pawnObstacleTest
 *Input: int TargetLocation[2], int CurrentLocation[2], board[][SIZE]
 *Output: int SUCCESS,FAIL
 *Function Operation:
     Inspects if a PAWN command is legal. If there are any obstacles,
     or if a capture is taking place without there having been "x"
     in the PGN command, then the move is illegal,
     and FAIL is returned.
 *************/

int pawnObstacleTest(int TargetLocation[], int CurrentLocation[], char board[][SIZE]) {
    int CurrentRow = CurrentLocation[0], CurrentCol = CurrentLocation[1];
    int TargetRow = TargetLocation[0], TargetCol = TargetLocation[1];
    char LocCheck = board[TargetRow][TargetCol];

    while (WhiteTurn == TRUE) {
        //up two
        if (TargetCol == CurrentCol && CurrentRow == SIZE - 2 && TargetRow == SIZE - 4 && LocCheck == EMPTY && board[SIZE-3][CurrentCol] == EMPTY) {
            return SUCCESS;
        }
        //up one
        else if (TargetCol == CurrentCol && TargetRow == CurrentRow - 1 && LocCheck == EMPTY) {
            if (TargetRow == 0 && Promotion == FALSE) {
                return FAIL;
            }
            return SUCCESS;
        }
        //eat up right
        else if (TargetCol == CurrentCol + 1 && TargetRow == CurrentRow - 1) {
            if (TargetRow == 0 && Promotion == FALSE) {
                return FAIL;
            }
            return pawnMoveValidity(LocCheck);
        }

        //eat up left
        else if (TargetCol == CurrentCol - 1 && TargetRow == CurrentRow - 1) {
            if (TargetRow == 0 && Promotion == FALSE) {
                return FAIL;
            }
            return pawnMoveValidity(LocCheck);
        }

        //no promotion

        else {
            return FAIL;
        }

    }


    //BLACK TURN
    while (WhiteTurn == FALSE) {
        //down two
        if (TargetCol == CurrentCol && CurrentRow == 1 && TargetRow == 3 && LocCheck == EMPTY && board[2][CurrentCol] == EMPTY) {
            return SUCCESS;
        }

        //down one
        else if (TargetCol == CurrentCol && TargetRow == CurrentRow + 1 && LocCheck == EMPTY) {
            if (TargetRow == 7 && Promotion == FALSE) {
                return FAIL;
            }
            return SUCCESS;
        }

        //eat down right
        else if (TargetCol == CurrentCol + 1 && TargetRow == CurrentRow + 1) {
            if (TargetRow == 7 && Promotion == FALSE) {
                return FAIL;
            }
            return pawnMoveValidity(LocCheck);
        }

        //eat down left
        else if (TargetCol == CurrentCol - 1 && TargetRow == CurrentRow + 1) {
            if (TargetRow == 7 && Promotion == FALSE) {
                return FAIL;
            }
            return pawnMoveValidity(LocCheck);
        }

        else {
            return FAIL;
        }
    }
    
    return FAIL;

}


/***********
 *Function Name: multPawnAccess
 *Input: int TargetLocation[2], int CurrentLocation[2], board[][SIZE]
 *Output: int SUCCESS,FAIL
 *Function Operation:
     Counts how many knights can access the target location.

     Similar to multRookAccess.
 *************/

int multPawnAccess(int LoxnsCanAccess[][2], int TargetLocation[], int MultLocations[][2], char board[][SIZE]) {
    int k = 0, counter = 0;
    int TestLocation[2];
    for (int TestPiece = 0; MultLocations[TestPiece][0] != SIZE; TestPiece++) {
        TestLocation[0] = MultLocations[TestPiece][0];
        TestLocation[1] = MultLocations[TestPiece][1];
        if (pawnObstacleTest(TargetLocation, TestLocation, board) == SUCCESS) {
            LoxnsCanAccess[k][0] = TestLocation[0];
            LoxnsCanAccess[k][1] = TestLocation[1];
            k++;
            counter++;
        }
    }
    return counter;
}


/*
END PAWN ACTIONS
*/



/*
KING RISK TESTS
*/


/***********
 *Function Name: kingCaptureTest
 *Input: int board[][SIZE]
 *Output: int SUCCESS,FAIL
 *Function Operation:
     Inspects if a CHECK or CHECKMATE command is valid by inspecting
     if the piece will be able to access the opponent's king in a next move.

     If PGN included Check or Mate, and it is indeed Check or Mate, return SUCCESS.
     Otherwise, returns FAIL.
 *************/

int kingCaptureTest(char board[][SIZE]) {
    int KingLocation[2] = { 0 };
    int TestLocation[2] = { 0 };
    char PlayerPiece = '0';
    //Locates opponent's king and stores in KingLocation[]
    findOppKingLocation(KingLocation, board);
    //Switch cap to TRUE because the test for next move assumes legal PGN with 'x'
    Cap = TRUE;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            PlayerPiece = board[i][j];
            TestLocation[0] = i;
            TestLocation[1] = j;

            //Examines for White turn
            if (WhiteTurn == TRUE) {
                //Examiens for respective cases of piece in question.
                switch (PlayerPiece) {

                case 'R': {
                    if (rookObstacleTest(KingLocation, TestLocation, board) == SUCCESS) {
                        if (Check == TRUE || Checkmate == TRUE) {
                            return SUCCESS;
                        }
                        else return FAIL;
                    }
                    break;
                }

                case 'Q': {
                    if (queenObstacleTest(KingLocation, TestLocation, board) == SUCCESS) {
                        if (Check == TRUE || Checkmate == TRUE) {
                            return SUCCESS;
                        }
                        else return FAIL;
                    }
                    break;
                }

                case 'B': {
                    if (bishopObstacleTest(KingLocation, TestLocation, board) == SUCCESS) {
                        if (Check == TRUE || Checkmate == TRUE) {
                            return SUCCESS;
                        }
                        else return FAIL;
                    }
                    break;
                }

                case 'N': {
                    if (knightObstacleTest(KingLocation, TestLocation, board) == SUCCESS) {
                        if (Check == TRUE || Checkmate == TRUE) {
                            return SUCCESS;
                        }
                        else return FAIL;
                    }
                    break;
                }

                case 'P': {
                    if (pawnObstacleTest(KingLocation, TestLocation, board) == SUCCESS) {
                        if (Check == TRUE || Checkmate == TRUE) {
                            return SUCCESS;
                        }
                        else return FAIL;
                    }
                    break;
                }

                default: {
                    return SUCCESS;
                }
                }

            }
        }
    }

    //Examines for black turn
    if (WhiteTurn == TRUE) {
        switch (PlayerPiece) {

        case 'r': {
            if (rookObstacleTest(KingLocation, TestLocation, board) == SUCCESS) {
                if (Check == TRUE || Checkmate == TRUE) {
                    return SUCCESS;
                }
                else return FAIL;
            }
            break;
        }

        case 'q': {
            if (queenObstacleTest(KingLocation, TestLocation, board) == SUCCESS) {
                if (Check == TRUE || Checkmate == TRUE) {
                    return SUCCESS;
                }
                else return FAIL;
            }
            break;
        }

        case 'b': {
            if (bishopObstacleTest(KingLocation, TestLocation, board) == SUCCESS) {
                if (Check == TRUE || Checkmate == TRUE) {
                    return SUCCESS;
                }
                else return FAIL;
            }
            break;
        }

        case 'n': {
            if (knightObstacleTest(KingLocation, TestLocation, board) == SUCCESS) {
                if (Check == TRUE || Checkmate == TRUE) {
                    return SUCCESS;
                }
                else return FAIL;
            }
            break;
        }

        case 'p': {
            if (pawnObstacleTest(KingLocation, TestLocation, board) == SUCCESS) {
                if (Check == TRUE || Checkmate == TRUE) {
                    return SUCCESS;
                }
                else return FAIL;
            }
            break;
        }

        default: {
            return SUCCESS;
        }
        }

    }

}

/***********
 *Function Name: kingRiskTest
 *Input: int board[][SIZE]
 *Output: int SUCCESS,FAIL
 *Function Operation:
     Inspects if a given move puts his/her King into a CHECK situation.

     If it does, returns FAIL. If the King is safe, returns SUCCESS.
 *************/

int kingRiskTest(char board[][SIZE]) {
    int SameKingLocation[2] = { 0 };
    int TestLocation[2] = { 0 };
    //Finds location of player king
    findSameKingLocation(SameKingLocation, board);
    Cap = TRUE;

    /*
    In order to see if the OPPONENT's next turn
    can capture the King, then White Turn is turned into FALSE if it was TRUE,
    and to TRUE if it was FALSE, to properly examine the opponent's next moves.

    Once the test is over, it returns WhiteTurn to what it was before.
    */

    if (WhiteTurn == TRUE) {
        WhiteTurn = FALSE;
        //Scans the board to see if any opponent pieces have acces to KingLocation
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                char EnemyPiece = board[i][j];
                TestLocation[0] = i;
                TestLocation[1] = j;
                if (EnemyPiece == ROOK + 32) {
                    if (rookObstacleTest(SameKingLocation, TestLocation, board) == SUCCESS) {
                        WhiteTurn = TRUE;
                        return FAIL;
                    }
                }
                else if (EnemyPiece == PAWN + 32) {
                    if (pawnObstacleTest(SameKingLocation, TestLocation, board) == SUCCESS) {
                        WhiteTurn = TRUE;
                        return FAIL;
                    }
                }
                else if (EnemyPiece == BISHOP + 32) {
                    if (bishopObstacleTest(SameKingLocation, TestLocation, board) == SUCCESS) {
                        WhiteTurn = TRUE;
                        return FAIL;
                    }
                }
                else if (EnemyPiece == QUEEN + 32) {
                    if (queenObstacleTest(SameKingLocation, TestLocation, board) == SUCCESS) {
                        WhiteTurn = TRUE;
                        return FAIL;
                    }
                }
                else if (EnemyPiece == KNIGHT + 32) {
                    if (knightObstacleTest(SameKingLocation, TestLocation, board) == SUCCESS) {
                        WhiteTurn = TRUE;
                        return FAIL;
                    }
                }

            }
        }
        WhiteTurn = TRUE;
    }


    //Logic identical for black turn
    if (WhiteTurn == FALSE) {
        WhiteTurn = TRUE;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                char EnemyPiece = board[i][j];
                TestLocation[0] = i;
                TestLocation[1] = j;
                if (EnemyPiece == ROOK) {
                    if (rookObstacleTest(SameKingLocation, TestLocation, board) == SUCCESS) {
                        WhiteTurn = FALSE;
                        return FAIL;
                    }
                }
                else if (EnemyPiece == PAWN) {
                    if (pawnObstacleTest(SameKingLocation, TestLocation, board) == SUCCESS) {
                        WhiteTurn = FALSE;
                        return FAIL;
                    }
                }
                else if (EnemyPiece == BISHOP) {
                    if (bishopObstacleTest(SameKingLocation, TestLocation, board) == SUCCESS) {
                        WhiteTurn = FALSE;
                        return FAIL;
                    }
                }
                else if (EnemyPiece == QUEEN) {
                    if (queenObstacleTest(SameKingLocation, TestLocation, board) == SUCCESS) {
                        WhiteTurn = FALSE;
                        return FAIL;
                    }
                }
                else if (EnemyPiece == KNIGHT) {
                    if (knightObstacleTest(SameKingLocation, TestLocation, board) == SUCCESS) {
                        WhiteTurn = FALSE;
                        return FAIL;
                    }
                }

            }
        }
        WhiteTurn = FALSE;
    }


    //If no enemy pieces can access the King, return SUCCESS
    return SUCCESS;
}

/*
END KING RISK TESTS
*/



/**
DEFINING LOCATIONS
**/


/***********
 *Function Name: defineTargetLocation
 *Input: int TargetLocation[], char TargetCol, char TargetRow
 *Output: void
 *Function Operation:
     Converts PGN input to destination coordinate relative to board,
     stores in TargetLocation[].
 *************/

void defineTargetLocation(int TargetLocation[], char TargetCol, char TargetRow) {
    TargetLocation[0] = SIZE - (TargetRow - '0');
    TargetLocation[1] = TargetCol - 'a'; 
}

/***********
 *Function Name: findUnknownCol
 *Input: int TargetLocation[], CurrentLocation, LxnsCanAccess, char board
 *Output: int counter
 *Function Operation:
     When the PGN input provides the ROW of the origin piece, this function
     finds which pieces are on that same row and stores their location
     in LxnsCanAccess[][].
 *************/


int findUnknownCol(int TargetLocation[], int CurrentLocation[], int LxnsCanAccess[][2], char board[][SIZE]) {
    int counter = 0;
    int TestCol = SIZE;
    for (int k = 0; LxnsCanAccess[k][0] != SIZE; k++) {
        int CurrentRow = CurrentLocation[0];
        int TestLocRow = LxnsCanAccess[k][0];
        if (TestLocRow == CurrentRow) {
            counter++;
            TestCol = LxnsCanAccess[k][1];
        }
    }

    if (counter == 1) {
        CurrentLocation[1] = TestCol;
    }

    return counter;
}

/***********
 *Function Name: findUnknownRow
 *Input: int TargetLocation[], CurrentLocation, LxnsCanAccess, char board
 *Output: int counter
 *Function Operation:
     When the PGN input provides the COL of the origin piece, this function
     finds which pieces are on that same col and stores their location
     in LxnsCanAccess[][].
 *************/

int findUnknownRow(int TargetLocation[], int CurrentLocation[], int LxnsCanAccess[][2], char board[][SIZE]) {
    int counter = 0;
    int TestRow = SIZE;
    for (int k = 0; LxnsCanAccess[k][0] != SIZE; k++) {
        int CurrentCol = CurrentLocation[1];
        int TestLocCol = LxnsCanAccess[k][1];
        if (TestLocCol == CurrentCol) {
            counter++;
            TestRow = LxnsCanAccess[k][0];
        }
    }

    if (counter == 1) {
        CurrentLocation[0] = TestRow;
    }

    return counter;
}

/***********
 *Function Name: colKnownFromInput
 *Input: char pgn[], int TargetLocation[], CurrentLocation, LxnsCanAccess, char board
 *Output: int
 *Function Operation:
     When the PGN input provides COL of the origin piece, this function
     converts the PGN col input to a coordinate on board, and uses findUnknownRow
     to find the row.
 *************/

int colKnownFromInput(char pgn[], int TargetLocation[], int CurrentLocation[], int LxnsCanAccess[][2], char board[][SIZE]) {
    if (isValidCol(pgn[1])) {
        //Converts the PGN col input to a coordinate
        CurrentLocation[1] = pgn[1] - 'a';
        return findUnknownRow(TargetLocation, CurrentLocation, LxnsCanAccess, board);
    }
    return 1;
}


/***********
 *Function Name: rowKnownFromInput
 *Input: char pgn[], int TargetLocation[], CurrentLocation, LxnsCanAccess, char board
 *Output: int
 *Function Operation:
     When the PGN input provides ROW of the origin piece, this function
     converts the PGN row input to a coordinate on board, and uses findUnknownCol
     to find the col.
 *************/
int rowKnownFromInput(char pgn[], int TargetLocation[], int CurrentLocation[], int LxnsCanAccess[][2], char board[][SIZE]) {
    if (isValidRow(pgn[1])) {
        //Converts the PGN row input to a coordinate
        CurrentLocation[0] = SIZE - (pgn[1] - '0');
        return findUnknownCol(TargetLocation, CurrentLocation, LxnsCanAccess, board);
    }
    return 1;
}


/**
END DEFINING LOCATIONS
***/



/****
VALIDITY/COUNTING TESTS
***/

/***********
 *Function Name: captureLegality
 *Input: char TargetCol, TargetRow, int TargetLocation[], char board
 *Output: int TRUE, FALSE
 *Function Operation:
     When there is an 'x' in a PGN input, this function inspects if there is actually
     an enemy piece at the designated target location. If yes, returns TRUE. Otherwise,
     returns FALSE.
 *************/

int captureLegality(char TargetCol, char TargetRow, int TargetLocation[], char board[][SIZE]) {
    //Converts target location in PGN to a coordinate
    defineTargetLocation(TargetLocation, TargetCol, TargetRow);
    int ActionRow = TargetLocation[0], ActionCol = TargetLocation[1];
    
    while (WhiteTurn == TRUE) {
        //Checks if target location is black
        if (isBlack(board[ActionRow][ActionCol]) == FALSE) {
            return FALSE;
        }
        else return TRUE;
    }

    while (WhiteTurn == FALSE) {
        //Checks if target location is white
        if (isWhite(board[ActionRow][ActionCol]) == FALSE) {
            return FALSE;
        }
        else return TRUE;
    }

    //If target location is empty, then the 'x' input is invalid.
    return FAIL;
}

/***********
 *Function Name: howManyPieces
 *Input: char Piece, board[][]
 *Output: int counter
 *Function Operation:
     Counts how many pieces exist for a given command.
     For example, if the piece does not exist, the the return from this function
     will indicate an invalid command.
     Also, if there are multiple pieces that can access the same target location,
     and the PGN input does not designate which one, the return from this function
     will indicate an invalid command.
 *************/

int howManyPieces(char Piece, char board[][SIZE]) {
    while (WhiteTurn == TRUE) {
        int counter = 0;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == Piece) {
                    counter++;
                }
            }
        }
        return counter;
    }

    while (WhiteTurn == FALSE) {
        int counter = 0;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == Piece + 32) {
                    counter++;
                }
            }
        }
        return counter;
    }  

    return FAIL;
}

/***********
 *Function Name: findMultPieces
 *Input: char Piece, board[][]
 *Output: void
 *Function Operation:
     When there is more than once piece of a given type for a command,
     and the command is valid, then this function finds where those pieces are
     and stores them in MultLocations[][]. This is later used to determine
     legality of moves.
 *************/

void findMultPieces(char Piece, int MultLocations[][2], char board[][SIZE]) {
    while (WhiteTurn == TRUE) {
        int k = 0;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == Piece) {
                    MultLocations[k][0] = i;
                    MultLocations[k][1] = j;
                    k++;
                }
            }
        }
        return;
    }

    while (WhiteTurn == FALSE) {
        int k = 0;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == Piece + 32) {
                    MultLocations[k][0] = i;
                    MultLocations[k][1] = j;
                    k++;
                }
            }
        }
        return;
    }

}

/***********
 *Function Name: howManyCanAccess
 *Input: char Piece, int LoxnsCanAccess, TargetLocation, MultLocations, char board
 *Output: int
 *Function Operation:
     When more than one of the pieces found by findMultPieces can access
     the target location, this function finds where those pieces are
     based on MultLocations[][].
     This is later used to determine legality of moves.
     char "Piece" is the piece designated in the PGN string.
 *************/

int howManyCanAccess(char Piece, int LoxnsCanAccess[][2], int TargetLocation[], int MultLocations[][2], char board[][SIZE]) {
    if (Piece == ROOK) {
        return multRookAccess(LoxnsCanAccess, TargetLocation, MultLocations, board);
    }
    if (Piece == BISHOP) {
        return multBishopAccess(LoxnsCanAccess, TargetLocation, MultLocations, board);
    }

    if (Piece == QUEEN) {
        return multQueenAccess(LoxnsCanAccess, TargetLocation, MultLocations, board);
    }

    if (Piece == KING) {
        return multKingAccess(LoxnsCanAccess, TargetLocation, MultLocations, board);
    }

    if (Piece == KNIGHT) {
        return multKnightAccess(LoxnsCanAccess, TargetLocation, MultLocations, board);
    }

    if (Piece == PAWN) {
        return multPawnAccess(LoxnsCanAccess, TargetLocation, MultLocations, board);
    }

    return FAIL;
}

/***********
 *Function Name: pgnLegality
 *Input: char pgn[], int pgnLength
 *Output: int TRUE, FALSE
 *Function Operation:
     This inspects the legality of a given PGN input relative to its length.
     Note that this is performed on pgnmod[], after '+', '#', and/or '=X'
     have been stripped from it.

     Examines possible permutations of where every part of the command belongs
     in a PGN string of a given length. If the PGN input is invalid, then the move
     processing never begins. Hence, move processing only takes place on legal PGN
     inputs, and this function ensures that.
 *************/

int pgnLegality(char pgn[], int pgnLength) {
    switch (pgnLength)
    {
    case 3: {
        if (isValidPiece(pgn[0]) == FALSE) {
            return FALSE;
        }
        if (isValidCol(pgn[1]) == FALSE) {
            return FALSE;
        }
        if (isValidRow(pgn[2]) == FALSE) {
            return FALSE;
        }
        return TRUE;
    }
    case 4: {
        if (isValidPiece(pgn[0]) == FALSE) {
            return FALSE;
        }
        if (isValidRow(pgn[1]) == FALSE && isValidCol(pgn[1]) == FALSE && pgn[1] != 'x') {
            return FALSE;
        }
        if (isValidCol(pgn[2]) == FALSE) {
            return FALSE;
        }
        if (isValidRow(pgn[3]) == FALSE) {
            return FALSE;
        }
        return TRUE;

    }
    case 5: {
        if (pgn[2] == CAPTURE) {

            if (isValidPiece(pgn[0]) == FALSE) {
                return FALSE;
            }
            if (isValidRow(pgn[1]) == FALSE && isValidCol(pgn[1]) == FALSE) {
                return FALSE;
            }

            if (isValidCol(pgn[3]) == FALSE) {
                return FALSE;
            }
            if (isValidRow(pgn[4]) == FALSE) {
                return FALSE;
            }

        }

        else {
            if (isValidPiece(pgn[0]) == FALSE) {
                return FALSE;
            }
            if (isValidCol(pgn[1]) == FALSE) {
                return FALSE;
            }
            if (isValidRow(pgn[2]) == FALSE) {
                return FALSE;
            }
            if (isValidCol(pgn[3]) == FALSE) {
                return FALSE;
            }
            if (isValidRow(pgn[4]) == FALSE) {
                return FALSE;
            }

        }

        return TRUE;
    }

    case 6: {
        if (pgn[3] != CAPTURE) {
            return FALSE;
        }

        return TRUE;
    }

    default: {
        return FALSE;
    }
    }
}


/**
END VALIDITY/COUNTING TESTS
***/


/***
CARRYING OUT MOVE
**/

/***********
 *Function Name: eraseCurrentLocation
 *Input: int CurrentLocation[], char board[][]
 *Output: void
 *Function Operation:
     When the move has been proven to be legal, then the piece is moved,
     and its current location is cleared.
 *************/

void eraseCurrentLocation(int CurrentLocation[], char board[][SIZE]) {
    int CurrentRow = CurrentLocation[0];
    int CurrentColumn = CurrentLocation[1];
    board[CurrentRow][CurrentColumn] = EMPTY;
}

/***********
 *Function Name: moveCurrentPiece
 *Input: char PlayerPiece, int CurrentLocation[], char board[][]
 *Output: void
 *Function Operation:
     When the move has been proven to be legal, then the piece is moved,
     and it is written into its target location.
 *************/

void moveCurrentPiece(char PlayerPiece, int TargetLocation[], char board[][SIZE]) {
    int TargetRow = TargetLocation[0], TargetCol = TargetLocation[1];
    while (WhiteTurn == TRUE) {
        if (Promotion == TRUE && TargetRow == 0) {
            board[TargetRow][TargetCol] = PromPiece;
            return;
        }
        board[TargetRow][TargetCol] = PlayerPiece;
        return;
    }
    while (WhiteTurn == FALSE) {
        if (Promotion == TRUE && TargetRow == 7) {
            board[TargetRow][TargetCol] = PromPiece + 32;
            return;
        }
        board[TargetRow][TargetCol] = PlayerPiece + 32;
        return;
    }
}

/***********
 *Function Name: performMove
 *Input: char pgn[], int CurrentLocation, int TargetLocation, char board[][]
 *Output: int TRUE, FALSE
 *Function Operation:
     When there are no obstalces to moving the piece in question, the final
     tests are performed and the piece is moved.

     This function examines:
     -If the command is to move the current piece to its current location,
     the move is illegal

     -If the command is a Check and does not designate otherwise, or vice versa,
     the move is ilelgal

     -If the command exposes the player's king to a check/mate,
     the move is illegal.

     When the move is illegal, FALSE is returned, and the move is not performed.
 *************/

int performMove(char pgn[], int CurrentLocation[], int TargetLocation[], char board[][SIZE]) {
    int CRow = CurrentLocation[0], CCol = CurrentLocation[1];
    int TRow = TargetLocation[0], TCol = TargetLocation[1];
    char TempPiece = board[TRow][TCol];
    //If currentlocation is same as targetlocation
    if (CRow == TRow && CCol == TCol) {
        return FALSE;
    }
    /*
    The move needs to "take place" in order to test check/mate or self-check/mate,
    so the piece is moved before these tests are run. If they fail, the move is undone
    and FALSE is returned.
    */
    eraseCurrentLocation(CurrentLocation, board);
    moveCurrentPiece(pgn[0], TargetLocation, board);
    //Examines if a check/mate is valid.
    if (kingCaptureTest(board) == FAIL) {
        //Undos move if check/mate is invalid
        if (WhiteTurn == TRUE) {
            board[CRow][CCol] = pgn[0];
            board[TRow][TCol] = TempPiece;
        }
        if (WhiteTurn == FALSE) {
            board[CRow][CCol] = pgn[0] + 32;
            board[TRow][TCol] = TempPiece;
        }
        return FALSE;
    }
    //Examines if a move endangers the king
    if (kingRiskTest(board) == FAIL) {
        //Undos move if king is endangered
        if (WhiteTurn == TRUE) {
            board[CRow][CCol] = pgn[0];
            board[TRow][TCol] = TempPiece;
        }
        if (WhiteTurn == FALSE) {
            board[CRow][CCol] = pgn[0] + 32;
            board[TRow][TCol] = TempPiece;
        }
        return FALSE;
    }
    return TRUE;
}

/**
END CARRYING OUT MOVE
***/




/*
CLASSIFYING PGN INPUT
*/


/***********
 *Function Name: simpleCommandActions
 *Input: char pgn[], int CurrentLocation, int TargetLocation, char board[][]
 *Output: int TRUE, FALSE
 *Function Operation:
     The series of actions to be performed for simple move-related commands.
     Example: "Rb1" is a simple command - no capture, no location clarifications, etc.
 *************/

int simpleCommandActions(char pgn[], int TargetLocation[], int CurrentLocation[], char board[][SIZE]) {
    //Target location is defined based on PGN input
    defineTargetLocation(TargetLocation, pgn[1], pgn[2]);
    int PieceCount = howManyPieces(pgn[0], board);
    //If the piece the PGN calls for does not exist, FALSE is returned.
    if (PieceCount == 0) {
        return FALSE;
    }
    //Used to temporarily store possible locations of piece in question
    int TempLocations[SIZE * SIZE][2], LocationsThatCanAccess[SIZE * SIZE][2];
    //Because SIZE isn't a real location on the board, it is set as default.
    fillWithSize(TempLocations);
    fillWithSize(LocationsThatCanAccess);
    //Finds the locations of multiple pieces.
    findMultPieces(pgn[0], TempLocations, board);
    /*
    Examines how many of the pieces in question can access the target location
    based on TempLocations. If it is more than once piece, than a simple PGN
    command is insufficient for the desired move, and FALSE is returned.
    */
    if (howManyCanAccess(pgn[0], LocationsThatCanAccess, TargetLocation, TempLocations, board) != 1) {
        return FALSE;
    }
    //If there is only one possible location, then that is the current location.
    CurrentLocation[0] = LocationsThatCanAccess[0][0];
    CurrentLocation[1] = LocationsThatCanAccess[0][1];
    //If king-related tests are successful, performMove carries out the move.
    return performMove(pgn, CurrentLocation, TargetLocation, board);
}

/***********
 *Function Name: singleClarificationAxns
 *Input: char pgn[], int CurrentLocation, int TargetLocation, char board[][]
 *Output: int TRUE, FALSE
 *Function Operation:
     The series of actions to be performed for "single clarification" commands.
     Example: "Rab1" and "R1b1" are single clarificaiton commands.
     They contain one character that clarifies location.
 *************/

int singleClarificationAxns(char pgn[], int TargetLocation[], int CurrentLocation[], char board[][SIZE]) {
    //Target location is defined.
    defineTargetLocation(TargetLocation, pgn[2], pgn[3]);
    //If there is no such valid piece, FALSE is returned.
    int PieceCount = howManyPieces(pgn[0], board);
    if (PieceCount == 0) {
        return FALSE;
    }
    //Similar to simpleCommandActions
    int TempLocations[SIZE * SIZE][2], LocationsThatCanAccess[SIZE * SIZE][2];
    fillWithSize(TempLocations);
    fillWithSize(LocationsThatCanAccess);
    findMultPieces(pgn[0], TempLocations, board);
    howManyCanAccess(pgn[0], LocationsThatCanAccess, TargetLocation, TempLocations, board);
    //If the PGN input designates column:
    if (rowKnownFromInput(pgn, TargetLocation, CurrentLocation, LocationsThatCanAccess, board) != 1) {
        return FALSE;
    }
    //If the PGN input designates row:
    if (colKnownFromInput(pgn, TargetLocation, CurrentLocation, LocationsThatCanAccess, board) != 1) {
        return FALSE;
    }
    //If king-related tests are successful, performMove carries out the move.
    return performMove(pgn, CurrentLocation, TargetLocation, board);

}


/***********
 *Function Name: doubleClarificationAxns
 *Input: char pgn[], int CurrentLocation, int TargetLocation, char board[][]
 *Output: int TRUE, FALSE
 *Function Operation:
     The series of actions to be performed for "double clarification" commands.
     Example: "Ra1b1" is a double clarificaiton command.
     It contains characters that precisely designate the origin location.
 *************/

int doubleClarificationAxns(char pgn[], int TargetLocation[], int CurrentLocation[], char board[][SIZE]) {
    //CurrentLocation is designated based on the PGN input
    CurrentLocation[0] = SIZE - (pgn[2] - '0');
    CurrentLocation[1] = pgn[1] - 'a';
    
    //ObstacleTest is run based on PGN input. If no obstacles, performMove is called
    if (pgn[0] == ROOK) {
        if (rookObstacleTest(TargetLocation, CurrentLocation, board) == SUCCESS) {
            return performMove (pgn, CurrentLocation, TargetLocation, board);
        }
        
        else {
            return FALSE;
        }
    }

    else if (pgn[0] == BISHOP) {
        if (bishopObstacleTest(TargetLocation, CurrentLocation, board) == SUCCESS) {
            return performMove(pgn, CurrentLocation, TargetLocation, board);
        }

        else {
            return FALSE;
        }
    }

    if (pgn[0] == QUEEN) {
        if (queenObstacleTest(TargetLocation, CurrentLocation, board) == SUCCESS) {
            return performMove(pgn, CurrentLocation, TargetLocation, board);
        }

        else {
            return FALSE;
        }
    }

    if (pgn[0] == KING) {
        if (kingObstacleTest(TargetLocation, CurrentLocation, board) == SUCCESS) {
            return performMove(pgn, CurrentLocation, TargetLocation, board);
        }

        else {
            return FALSE;
        }
    }

    if (pgn[0] == KNIGHT) {
        if (knightObstacleTest(TargetLocation, CurrentLocation, board) == SUCCESS) {
            return performMove(pgn, CurrentLocation, TargetLocation, board);
        }

        else {
            return FALSE;
        }

    }

    if (pgn[0] == PAWN) {
        if (pawnObstacleTest(TargetLocation, CurrentLocation, board) == SUCCESS) {
            return performMove(pgn, CurrentLocation, TargetLocation, board);
        }

        else {
            return FALSE;
        }

    }

    //If the input relates to an invalid piece, FALSE is returned.
    return FALSE;
}


/***********
 *Function Name: captureOrSingleClar
 *Input: char pgn[], int CurrentLocation, int TargetLocation, char board[][]
 *Output: int TRUE, FALSE
 *Function Operation:
     Determines if a PGN move-command string that is 4 characters long is a capture command
     or a single clarificaiton command.
 *************/


int captureOrSingleClar(char pgn[], int TargetLocation[], int CurrentLocation[], char board[][SIZE]) {
    //If capture command:
    if (pgn[1] == CAPTURE) {
        //Tests legality of capture
        if (captureLegality(pgn[2], pgn[3], TargetLocation, board) == FALSE) {
            return FALSE;
        }
        //Triggers Cap flag if valid
        Cap = TRUE;
        //removes 'x' from string and calls simpleCommandActions
        char strippedPGN[3] = { pgn[0], pgn[2], pgn[3] };
        return simpleCommandActions(strippedPGN, TargetLocation, CurrentLocation, board);
    }

    //Otherwise, it is a single clarification command.
    else {
        return singleClarificationAxns(pgn, TargetLocation, CurrentLocation, board);
    }
}

/***********
 *Function Name: captureOrDoubleClar
 *Input: char pgn[], int CurrentLocation, int TargetLocation, char board[][]
 *Output: int TRUE, FALSE
 *Function Operation:
     Determines if a PGN move-command string that is 5 characters long is
     a capture-and-single-clarification command
     or a double clarification command and calls the appropriate function.
 *************/

int captureOrDoubleClar(char pgn[], int TargetLocation[], int CurrentLocation[], char board[][SIZE]) {
    //Target location is defined
    defineTargetLocation(TargetLocation, pgn[3], pgn[4]);
    //If capture command
    if (pgn[2] == CAPTURE) {
        //Tests capture legality
        if (captureLegality(pgn[3], pgn[4], TargetLocation, board) == FALSE) {
            return FALSE;
        }
        //If valid, flags Cap
        Cap = TRUE;
        //Strips 'x' from command and calls singleClarificationAxns
        char strippedPGN[4] = { pgn[0], pgn[1], pgn[3], pgn[4] };
        return singleClarificationAxns(strippedPGN, TargetLocation, CurrentLocation, board);
    }

    //Otherwise, it is a double clarification command.
    else {
        return doubleClarificationAxns(pgn, TargetLocation, CurrentLocation, board);
    }
}

/****
END CLASSIFYING PGN INPUT
***/







 /***********
  *Function Name: makeMove
  *Input: char pgn[], char board[][], int isWhiteTurn
  *Output: int TRUE, FALSE
  *Function Operation:
      Initializes at default all the necessary arrays/values for any given command.
      Determines the nature of each command and its legality,
      and calls the appropriate move commands based on these tests.
      Returns TRUE if the command is completely legal and FALSE
      if there is anything illegal about the command whatsoever.
  *************/



int makeMove(char board[][SIZE], char pgn[], int isWhiteTurn){
    //Setting turn to a global variable to avoid passing across multiple functions
    WhiteTurn = isWhiteTurn;
    //Resetting the global variables
    Cap = FALSE;
    Check = FALSE;
    Checkmate = FALSE;
    Promotion = FALSE;
    PromPiece = '0';
    //Initializing TargetLocation, CurrentLocation, and pgnmod arrays
    int TargetLocation[2] = { 0,0 };
    int CurrentLocation[2] = { 0,0 };
    char pgnmod[9] = { 0 };
    //Testing if this is a check,mate,promotion, or pawn move
    if (checkMatePromPawn(pgn, pgnmod) == FALSE) {
        return FALSE;
    }
    int pgnLength = strlen(pgnmod);
    //Testing if the PGN command is legal relative to its length
    if  (pgnLegality(pgnmod, pgnLength) == FALSE) {
        return FALSE;
    }
    
    //Determining number of move-related commands in the PGN input
    int CmdLength = (cmdLength(pgnmod));      
    switch (CmdLength) {
        //There is no default to this switch/case
    case 3: {
        //3 move-related commands. Example: Ra1
        return simpleCommandActions(pgnmod, TargetLocation, CurrentLocation, board);
    }

    case 4: {
        //4 move-related commands. Example: R8a1 or Rxa1 or Raa1
        return captureOrSingleClar(pgnmod, TargetLocation, CurrentLocation, board);
    }

    case 5: {
        //5 move-related commands. Exampe: Ra8a1 or R8xa1 or Raxa1
        return captureOrDoubleClar(pgnmod, TargetLocation, CurrentLocation, board);
    }

    case 6: {
        /*
        A legal PGN input with 6 move-related commands can ONLY be a double
        clarification command with a capture command, such as: Ra8xa1.
        The legality of the capture is tested. If it is legal, the 'x' is stripped,
        Cap is made TRUE, and doubleClarificationAxns is called to test the move itself.
        */
        if (captureLegality(pgnmod[4], pgnmod[5], TargetLocation, board) == FALSE) {
            return FALSE;
        }
        Cap = TRUE;
        char strippedPGN[5] = { pgnmod[0], pgnmod[1], pgnmod[2], pgnmod[4], pgnmod[5] };
        return doubleClarificationAxns(strippedPGN, TargetLocation, CurrentLocation, board);
    }
    }

    return TRUE;
}

 
