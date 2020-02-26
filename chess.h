#define SIZE 8
#ifndef __CHESS_H
#define __CHESS_H

void createBoard(char board[][SIZE], char fen[]);
void printBoard(char board[][SIZE]);
int makeMove(char board[][SIZE], char pgn[], int isWhiteTurn);

#endif
