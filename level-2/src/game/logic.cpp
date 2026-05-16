/**
 * Logic cpp implementation
 *
 */

#include "logic.h"

/* ---------- Importing ---------- */

#include <format>
#include <limits>
#include <sstream>

#include "../utils/logger.h"

/* ---------- Definitions ---------- */

void Logic::initBoard(char board[][BOARD_N_MAX], const int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            board[i][j] = '-';
        }
    }
}

bool Logic::isValidMove(const char board[][BOARD_N_MAX], const int size, const int row, const int col) {
    if (row < 0 || row >= size || col < 0 || col >= size) {
        return false;
    }
    if (board[row][col] != '-') {
        return false;
    }
    return true;
}

void Logic::makeMove(char board[][BOARD_N_MAX], const int row, const int col, const char symbol) {
    board[row][col] = symbol;
}

bool Logic::isEmptyHead(const char board[][BOARD_N_MAX], const int size, int x, int y, const char symbol) {
    if (x < 0 || x >= size || y < 0 || y >= size) {
        return true;
    }
    if (board[x][y] == '-') {
        return true;
    }
    if (board[x][y] == symbol) {
        return true;
    }
    return false;
}

bool Logic::checkWin(const char board[][BOARD_N_MAX], const int size, const char symbol, const int goal, EndRule rule) {
    int dx[4] = {1, 0, 1, 1};
    int dy[4] = {0, 1, 1, -1};

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == symbol) {
                for (int k = 0; k < 4; k++) {
                    int len = 1;
                    int nx = i + dx[k];
                    int ny = j + dy[k];
                    int px = i - dx[k];
                    int py = j -dy[k];
                    
                    while (nx >= 0 && ny >= 0 && nx < size && ny < size && board[nx][ny] == symbol) {
                        len++;
                        nx += dx[k];
                        ny += dy[k];
                        if (len >= goal + 2) return true;
                    }
                    while (px >= 0 && py >= 0 && px < size && py < size && board[px][py] == symbol) {
                        len++;
                        px -= dx[k];
                        py -= dy[k];
                        if (len >= goal + 2) return true;
                    }
                    if (len >= goal) {
                        bool first_open = px < 0 || py < 0 || px >= size || py >= size || board[px][py] == symbol || board[px][py] == '-';
                        bool last_open = nx < 0 || ny < 0 || nx >= size || ny >= size || board[nx][ny] == symbol || board[nx][ny] == '-';
                        if (len >= goal + 1 && (first_open || last_open)) {
                            return true;
                        }
                        if (rule == EndRule::NONE) {
                            return true;
                        }
                        else if (rule == EndRule::OPEN_ONE) {
                            return first_open || last_open;
                        }
                        else if (rule == EndRule::OPEN_TWO) {
                            return first_open && last_open;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Logic::checkDraw(char board[][BOARD_N_MAX], const int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == '-') {
                return false;
            }
        }
    }
    return true;
}

std::optional<WinLine> Logic::getWinLine(
    const char board[][BOARD_N_MAX],
    const int size,
    const char symbol,
    const int goal,
    EndRule rule) {
    int dx[4] = {1, 0, 1, 1};
    int dy[4] = {0, 1, 1, -1};

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] != symbol) {
                continue;
            }

            for (int k = 0; k < 4; k++) {
                int len = 1;
                int nx = i + dx[k];
                int ny = j + dy[k];
                int px = i - dx[k];
                int py = j - dy[k];

                while (nx >= 0 && ny >= 0 && nx < size && ny < size && board[nx][ny] == symbol) {
                    len++;
                    nx += dx[k];
                    ny += dy[k];
                }
                while (px >= 0 && py >= 0 && px < size && py < size && board[px][py] == symbol) {
                    len++;
                    px -= dx[k];
                    py -= dy[k];
                }

                bool first_open = px < 0 || py < 0 || px >= size || py >= size || board[px][py] == symbol || board[px][py] == '-';
                bool last_open = nx < 0 || ny < 0 || nx >= size || ny >= size || board[nx][ny] == symbol || board[nx][ny] == '-';

                bool winning_line = false;
                if (len >= goal + 2) {
                    winning_line = true;
                } else if (len >= goal) {
                    if (rule == EndRule::NONE) {
                        winning_line = true;
                    } else if (rule == EndRule::OPEN_ONE) {
                        winning_line = first_open || last_open;
                    } else if (rule == EndRule::OPEN_TWO) {
                        winning_line = first_open && last_open;
                    }
                }

                if (!winning_line) {
                    continue;
                }

                WinLine win;
                int start_x = px + dx[k];
                int start_y = py + dy[k];
                int end_x = nx - dx[k];
                int end_y = ny - dy[k];

                for (int cx = start_x, cy = start_y;; cx += dx[k], cy += dy[k]) {
                    win.cells.emplace_back(cx, cy);
                    if (cx == end_x && cy == end_y) {
                        break;
                    }
                }

                return win;
            }
        }
    }

    return std::nullopt;
}