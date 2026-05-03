#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "gameConfig.h"
#include <vector>

void initBoard(char board[][BOARD_N_MAX], const int size);
bool isValidMove(const char board[][BOARD_N_MAX], const int size, const int row, const int col);
void makeMove(char board[][BOARD_N_MAX], const int row, const int col, const char symbol);
bool isEmptyHead(char board[][BOARD_N_MAX], int size, int x, int y, const char symbol);
bool checkWin(char board[][BOARD_N_MAX], const int size, const char symbol, const int goal, EndRule rule = EndRule::OPEN_TWO);
bool checkDraw(char board[][BOARD_N_MAX], const int size);

pII botMove(char board[][BOARD_N_MAX], const int size, const int goal, const char symbol, const BotLevel level);
pII random_pick(char board[][BOARD_N_MAX], const int size);
pII simple_heuristic(char board[][BOARD_N_MAX], const int size, const int goal, const char botSymbol, const char playerSymbol);
pII hard_level(char board[][BOARD_N_MAX], const int size, const int goal, const char botSymbol, const char playerSymbol);

#endif 
