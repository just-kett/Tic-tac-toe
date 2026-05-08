/**
 * Bot LV3 cpp implementation
 *
 */

#include "bot_lv3.h"
#include "bot_lv2.h"

/* ---------- Importing ---------- */

#include <sstream>
#include <climits>
#include <algorithm>

#include "../../utils/logger.h"
#include "../logic.h"

/* ---------- Definitions ---------- */

/**
 * Mô tả:
 *   Constructor của BotLevel3.
 */
BotLevel3::BotLevel3(const BotLevel& _level, const char& _symbol)
    : BotLevel2(_level, _symbol) {
}

/**
 * Mô tả:
 *   Destructor của BotLevel3.
 */
BotLevel3::~BotLevel3() {
}

/**
 * Mô tả:
 *   Trả về nước đi của bot HARD.
 *
 * TODO:
 *   - Bước 1: Gọi hard_level để tìm nước đi tối ưu
 *   - Bước 2: Nếu không tìm được → fallback về logic của BotLevel2
 */
pII BotLevel3::getMove(char board[][BOARD_N_MAX], const int size, const int goal) {
    pII move = hard_level(board, size, goal, symbol, op_symbol);
    if (move.first >= 0 && move.second >= 0 && Logic::isValidMove(board, size, move.first, move.second)) {
        return move;
    }
    return BotLevel2::getMove(board, size, goal);
}

/**
 * Mô tả:
 *   Logic nâng cao cho bot HARD.
 *   Có thể sử dụng:
 *   - minimax
 *   - alpha-beta pruning
 *   - heuristic scoring
 *
 * TODO:
 *   - Bước 1: Duyệt các nước đi hợp lệ
 *   - Bước 2: Đánh giá từng nước (score)
 *   - Bước 3: Chọn nước có score tốt nhất
 *   - Trường hợp biên: không còn nước đi
 */

std::vector<pII> getMove(char board[][BOARD_N_MAX], const int size) {
    std::vector<std::pair<int, int>> moves;
    bool picked[BOARD_N_MAX][BOARD_N_MAX] = {false};
    int dx[8] = {1, 0, 1, 1, -1, 0, -1, -1};
    int dy[8] = {0, 1, 1, -1, 0, -1, -1, 1};

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == '-') continue;

            for (int rad = 1; rad <= 2; rad++) {
                for (int k = 0; k < 8; k++) {
                    int nx = i + rad * dx[k];
                    int ny = j + rad * dy[k];
                    if (nx >= 0 && ny >= 0 && nx < size && ny < size && !picked[nx][ny]) {
                        picked[nx][ny] = true;
                        moves.push_back({nx, ny});
                    }
                }
            }
        }
    }
    return moves;
}

int board_eval(char board[][BOARD_N_MAX], const int size, const int goal, const char symbol) {
    int score = 0;

    auto heuristic = [](bool first_open, bool last_open, int len, int goal) -> int {
        if (len >= goal) return 1000000;  
        if (!first_open && !last_open) {
            return 1;  
        }
        else if (first_open && last_open) {
            return 10000 * len;  
        }
        else {
            return 10 * len;  
        }
    };

    int dx[4] = {1, 0 , 1, 1};
    int dy[4] = {0, 1, 1, -1};

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] != symbol) continue;

            for (int k = 0; k < 4; k++) {
                bool first_open = true;
                bool last_open = true;
                
                int len = 1;
                int nx = i + dx[k];
                int ny = j + dy[k];

                if (i - dx[k] >= 0 && j - dy[k] >= 0 && board[i - dx[k]][j - dy[k]] == symbol) continue;
                if (i - dx[k] < 0 || j - dy[k] < 0 || board[i - dx[k]][j - dy[k]] != '-') {
                    first_open = false;
                }
                while (nx >= 0 && ny >= 0 && nx < size && ny < size && board[nx][ny] == symbol) {
                    len += 1;
                    nx += dx[k];
                    ny += dy[k];
                }
                if (nx < 0 || ny < 0 || nx >= size || ny >= size || board[nx][ny] != '-') {
                    last_open = false;
                }              
                score += heuristic(first_open, last_open, len, goal);
            }
        }
    }
    return score;
}

std::vector<pII> OrderMove(char board[][BOARD_N_MAX], const int size, const int goal, const char symbol, const char oppsymbol) {
    std::vector<std::pair<int, std::pair<int, int>>> orderedMove;
    std::vector<std::pair<int, int>> result;
    auto moves = getMove(board, size);

    for (const auto& m : moves) {
        int i = m.first;
        int j = m.second;
        if (board[i][j] == '-') {
            int score = helper::eval_score(board, i, j, size, goal, symbol, oppsymbol)
                        + 0.7 * helper::eval_score(board, i, j, size, goal, oppsymbol, symbol);
            orderedMove.push_back({score, {i, j}});
        }
    }
    
    while (!orderedMove.empty()) {
        auto it = std::max_element(orderedMove.begin(), orderedMove.end());
        result.push_back({it->second.first, it->second.second});
        orderedMove.erase(it);
    }
    return result;
}

int minimax(char board[][BOARD_N_MAX], const int size, const int goal, const char bot, const char player, int depth, bool isMax, int alpha, int beta) {
    if (Logic::checkWin(board, size, bot, goal)) return INT_MAX;
    if (Logic::checkWin(board, size, player, goal)) return INT_MIN;
    if (Logic::checkDraw(board, size)) return 0;
    if (depth == 0) return (board_eval(board, size, goal, bot) + 0.7 * board_eval(board, size, goal, player));

    auto move = OrderMove(board, size, goal, bot, player);

    if (isMax) {
        int best = INT_MIN;
        for (const auto& m : move) {
            int i = m.first;
            int j = m.second;

            if (!Logic::isValidMove(board, size, i, j)) continue;

            board[i][j] = bot;
            int val = minimax(board, size, goal, bot, player, depth - 1, false, alpha, beta);
            board[i][j] = '-';
            best = std::max(best, val);
            alpha = std::max(best, alpha);
            if (beta <= alpha) break;
        }
        return best;
    }
    else {
        int best = INT_MAX;
        for (const auto& m : move) {
            int i = m.first;
            int j = m.second;

            if (!Logic::isValidMove(board, size, i, j)) continue;

            board[i][j] = player;
            int val = minimax(board, size, goal, bot, player, depth - 1, true, alpha, beta);
            board[i][j] = '-';
            best = std::min(best, val);
            beta = std::min(best, beta);
            if (beta <= alpha) break;
        }
        return best;
    }
}



pII BotLevel3::hard_level(char board[][BOARD_N_MAX], const int size, const int goal, const char botSymbol, const char playerSymbol) {
    int row = -1;
    int col = -1;
    int depth = 4;
    int score = 0;
    int bestScore = INT_MIN;
    auto move = OrderMove(board, size, goal, botSymbol, playerSymbol);

    for (const auto& m : move) {
        int i = m.first;
        int j = m.second;

        if (!Logic::isValidMove(board, size, i, j)) continue;

        if (helper::WinMove(board, i, j, size, goal, botSymbol)) {
            return {i, j};
        }
        
        board[i][j] = botSymbol;
        score = minimax(board, size, goal, botSymbol, playerSymbol, depth - 1, false, INT_MIN, INT_MAX);
        board[i][j] = '-';
        if (helper::WinMove(board, i, j, size, goal, playerSymbol)) {
            score = INT_MAX;
        }
        if (score > bestScore) {
            bestScore = score;
            row = i;
            col = j;
        }
    }
    if (row != -1) return {row, col};
    return random_pick(board, size);
    throw NotImplementedException();
}