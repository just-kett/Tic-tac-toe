/**
 * Logic header file
 *
 */

#pragma once

/* ---------- Importing ---------- */

#include <optional>
#include <vector>

#include "setup.h"

/* ---------- Declarations ---------- */

struct WinLine {
    std::vector<std::pair<int, int>> cells;  
};

namespace Logic {

void initBoard(char board[][BOARD_N_MAX], const int size);

bool isValidMove(const char board[][BOARD_N_MAX], const int size, const int row, const int col);

void makeMove(char board[][BOARD_N_MAX], const int row, const int col, const char symbol);

bool isEmptyHead(const char board[][BOARD_N_MAX], const int size, int x, int y, const char symbol);

bool checkWin(const char board[][BOARD_N_MAX], const int size, const char symbol, const int goal, EndRule rule = EndRule::OPEN_TWO);

bool checkDraw(char board[][BOARD_N_MAX], const int size);

std::optional<WinLine> getWinLine(
    const char board[][BOARD_N_MAX],
    const int size,
    const char symbol,
    const int goal,
    EndRule rule = EndRule::OPEN_TWO);

} 