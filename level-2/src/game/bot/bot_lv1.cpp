/**
 * Bot LV1 cpp implementation
 *
 */

#include "bot_lv1.h"

/* ---------- Importing ---------- */

#include <sstream>

#include "../../utils/logger.h"

/* ---------- Definitions ---------- */

BotLevel1::BotLevel1(const BotLevel& _level, const char& _symbol)
    : Bot(_level, _symbol) {
}

BotLevel1::~BotLevel1() {}

pII BotLevel1::getMove(char board[][BOARD_N_MAX], const int size, const int goal) {

    return random_pick(board, size);

}

pII BotLevel1::random_pick(char board[][BOARD_N_MAX], const int size) {

    std::vector<std::vector<int>> emptyCells;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == '-') {
                emptyCells.push_back({i, j});
            }
        }
    }

    if (emptyCells.empty()) {
        return {-1, -1};
    }

    int row, col;

    srand(time(NULL));
    int index = rand() % emptyCells.size();

    row = emptyCells[index][0];
    col = emptyCells[index][1];
    return {row, col};
}