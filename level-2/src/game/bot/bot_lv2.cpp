/**
 * Bot LV2 cpp implementation
 *
 */

#include "bot_lv2.h"

/* ---------- Importing ---------- */

#include <sstream>
#include <algorithm>

#include "../../utils/logger.h"
#include "../logic.h"

/* ---------- Definitions ---------- */

/**
 * Mô tả:
 *   Constructor của BotLevel2.
 */
BotLevel2::BotLevel2(const BotLevel& _level, const char& _symbol)
    : BotLevel1(_level, _symbol) {
}

/**
 * Mô tả:
 *   Destructor của BotLevel2.
 */
BotLevel2::~BotLevel2() {
}

/**
 * Mô tả:
 *   Trả về nước đi của bot MEDIUM.
 *
 * TODO:
 *   - Gọi heuristic để chọn nước đi
 */
pII BotLevel2::getMove(char board[][BOARD_N_MAX], const int size, const int goal) {
    // TODO: gọi simple_heuristic với symbol và op_symbol
    return simple_heuristic(board, size, goal, symbol, op_symbol);
}

/**
 * Mô tả:
 *   Heuristic đơn giản:
 *   - Ưu tiên thắng
 *   - Chặn đối thủ
 *   - Fallback random
 *
 * TODO:
 *   - Bước 1: kiểm tra nước thắng
 *   - Bước 2: kiểm tra nước chặn
 *   - Bước 3: chọn random nếu không có
 */
namespace helper {

bool WinMove(char board[][BOARD_N_MAX], const int x, const int y, const int size, const int goal, const char symbol) {
    board[x][y] = symbol;
    bool win = Logic::checkWin(board, size, symbol, goal);
    board[x][y] = '-';
    return win;
}

int eval_score (char board[][BOARD_N_MAX], const int x, const int y, const int size, const int goal, const char symbol, const char op_symbol) {
    int score = 0;
    int dx[4] = {1, 0, 1, 1};
    int dy[4] = {0, 1, 1, -1};
    board[x][y] = symbol;

    for (int i = 0; i < 4; i++) {
        int len = 1;
        int nx = x + dx[i];
        int ny = y + dy[i];
        bool first_open = true;
        bool last_open = true;
        while (nx < size && ny < size && board[nx][ny] == symbol) {
            len += 1;
            nx += dx[i];
            ny += dy[i];
        }
        if (board[nx][ny] == op_symbol) last_open = false;

        int px = x - dx[i];
        int py = y - dy[i];
        while (px >= 0 && px < size && py >= 0 && py < size && board[px][py] == symbol) {
            len += 1;
            px -= dx[i];
            py -= dy[i];
        }
        if (board[px][py] == op_symbol) first_open = false;

        for (int i = 1; i < goal; i++) {
            if (len == goal - i) {
                if (!last_open && !first_open) {
                    score += 1;
                }
                else if (last_open && first_open) {
                    score += 15 * (goal - i);
                }
                else {
                    score += 5 * (goal - i);
                }
            }
        }
    }
    board[x][y] = '-';
    return score; 
}

}  // namespace helper

pII BotLevel2::simple_heuristic(char board[][BOARD_N_MAX], const int size, const int goal, const char botSymbol, const char playerSymbol) {
    std::vector<std::pair<double, std::pair<int, int>>> move;
    int row = -1;
    int col = -1;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == '-') {
                if (helper::WinMove(board, i, j, size, goal, botSymbol)) {
                    return {i, j};
                }
            }
            if (board[i][j] == '-') {
                int atk_score = helper::eval_score(board, i, j, size, goal, botSymbol, playerSymbol);
                int def_score = helper::eval_score(board, i, j , size, goal, playerSymbol, botSymbol);
                double score = atk_score + 0.7*def_score;
                if (helper::WinMove(board, i, j, size, goal, playerSymbol)) {
                    score += 10000;
                }
                move.push_back({score, {i, j}});
            }
        }
    }
    if(!move.empty()) {
        auto best = std::max_element(move.begin(), move.end());
        row = best->second.first;
        col = best->second.second;
    }

    if (row != -1) return {row, col};
    return random_pick(board, size);
}