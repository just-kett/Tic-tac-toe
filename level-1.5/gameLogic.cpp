#include "gameLogic.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <algorithm>

void initBoard(char board[][BOARD_N_MAX], const int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            board[i][j] = '-';
        }
    }
}

bool isValidMove(const char board[][BOARD_N_MAX], const int size, const int row, const int col) {
    if (row < 0 || row >= size || col < 0 || col >= size) {
        return false; 
    }
    if (board[row][col] != '-') {
        return false;
    }
    return true;
}

void makeMove(char board[][BOARD_N_MAX], const int row, const int col, const char symbol) {
    board[row][col] = symbol;
}

bool isEmptyHead(char board[][BOARD_N_MAX], int size, int x, int y, const char symbol) {
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

bool checkWin(char board[][BOARD_N_MAX], const int size, const char symbol, const int goal, EndRule rule) {
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
                    int py = j - dy[k];
                    
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

bool checkDraw(char board[][BOARD_N_MAX], const int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == '-') {
                return false;
            }
        }
    }
    return true;
}

pII botMove(char board[][BOARD_N_MAX], const int size, const int goal, const char symbol, const BotLevel level) {
    char opponent = (symbol == 'X') ? 'O' : 'X';

    switch (level) {
        case BotLevel::EASY:
            return random_pick(board, size);
        case BotLevel::MEDIUM:
            return simple_heuristic(board, size, goal, symbol, opponent);
        case BotLevel::HARD:
            return hard_level(board, size, goal, symbol, opponent);
        default:
            return random_pick(board, size);
    }
}

pII random_pick(char board[][BOARD_N_MAX], const int size) {
    std::vector<std::vector<int>> emptyCells;

    for (int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            if (board[i][j] == '-') {
                emptyCells.push_back({i, j});
            }
        }
    }
    int row, col;
    
    srand(time(NULL));
    int index = rand() % emptyCells.size();

    row = emptyCells[index][0];
    col = emptyCells[index][1];
    
    return std::make_pair(row, col);
}

bool WinMove(char board[][BOARD_N_MAX], const int x, const int y, const int size, const int goal, const char symbol) {
    board[x][y] = symbol;
    bool win = checkWin(board, size, symbol, goal);
    board[x][y] = '-';
    return win;
}

int eval_score(char board[][BOARD_N_MAX], const int x, const int y, const int size, const int goal, const char firstsymbol, const char secondsymbol) {
    int score = 0;
    int dx[4] = {1, 0, 1, 1};
    int dy[4] = {0, 1, 1, -1};
    board[x][y] = firstsymbol;
    
    for (int i = 0; i < 4; i++) {
        int len = 1;
        int nx = x + dx[i];
        int ny = y + dy[i];
        bool first_open = true;
        bool last_open = true;
        while(nx < size && ny < size && board[nx][ny] == firstsymbol) {
            len += 1;
            nx += dx[i];
            ny += dy[i];
        }
        if (board[nx][ny] == secondsymbol) last_open = false;

        nx = x - dx[i];
        ny = y - dy[i];
        while (nx >= 0 && nx < size && ny >=0 && ny < size && board[nx][ny] == firstsymbol) {
            len += 1;
            nx -= dx[i];
            ny -= dy[i];
        } 
        if (board[nx][ny] == secondsymbol) first_open = false;

        for (int i = 1; i < goal; i++) {
            if (len == goal - i) {
                if (!last_open && !first_open) {
                    score += 0;
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

pII simple_heuristic(char board[][BOARD_N_MAX], const int size, const int goal, const char botSymbol, const char playerSymbol) {
    std::vector<std::pair<double, std::pair<int, int>>> move;
    int row = -1; 
    int col = -1;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == '-') {     
                if (WinMove(board, i, j, size, goal, botSymbol)) {
                    return {i, j};
                }
            }
            if (board[i][j] == '-') {
                int atk_score = eval_score(board, i, j, size, goal, botSymbol, playerSymbol);
                int def_score = eval_score(board, i, j, size, goal, playerSymbol, botSymbol);
                double score = atk_score + 0.7*def_score;
                if (WinMove(board, i, j, size, goal, playerSymbol)) {
                    score += 100000;
                }
                move.push_back({score, {i, j}});
            }
        }
    }
    if (!move.empty()) {
        auto best = std::max_element(move.begin(), move.end());
        row = best->second.first;
        col = best->second.second;
    }
    if (row != -1) return {row, col};
    
    return random_pick(board, size);
}

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
            int score = eval_score(board, i, j, size, goal, symbol, oppsymbol)
                        + 0.7 * eval_score(board, i, j, size, goal, oppsymbol, symbol);
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
    if (checkWin(board, size, bot, goal)) return INT_MAX;
    if (checkWin(board, size, player, goal)) return INT_MIN;
    if (checkDraw(board, size)) return 0;
    if (depth == 0) return (board_eval(board, size, goal, bot) + 0.7 * board_eval(board, size, goal, player));

    auto move = OrderMove(board, size, goal, bot, player);

    if (isMax) {
        int best = INT_MIN;
        for (const auto& m : move) {
            int i = m.first;
            int j = m.second;

            if (!isValidMove(board, size, i, j)) continue;

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

            if (!isValidMove(board, size, i, j)) continue;

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

pII hard_level(char board[][BOARD_N_MAX], const int size, const int goal, const char botSymbol, const char playerSymbol) {
    int row = -1;
    int col = -1;
    int depth = 4;
    int score = 0;
    int bestScore = INT_MIN;
    auto move = OrderMove(board, size, goal, botSymbol, playerSymbol);

    for (const auto& m : move) {
        int i = m.first;
        int j = m.second;

        if (!isValidMove(board, size, i, j)) continue;

        if (WinMove(board, i, j, size, goal, botSymbol)) {
            return {i, j};
        }
        
        board[i][j] = botSymbol;
        score = minimax(board, size, goal, botSymbol, playerSymbol, depth - 1, false, INT_MIN, INT_MAX);
        board[i][j] = '-';
        if (WinMove(board, i, j, size, goal, playerSymbol)) {
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
}