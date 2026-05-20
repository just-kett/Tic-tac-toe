/**
 * Bot LV2 header file
 *
 */

#pragma once

/* ---------- Importing ---------- */

#include "../setup.h"
#include "bot_lv1.h"

/* ---------- Declarations ---------- */

namespace helper {
    bool WinMove(char board[][BOARD_N_MAX], const int x, const int y, const int size, const int goal, const char symbol);
    int eval_score (char board[][BOARD_N_MAX], const int x, const int y, const int size, const int goal, const char symbol, const char op_symbol);
}

class BotLevel2 : public BotLevel1 {
   protected:

    pII simple_heuristic(char board[][BOARD_N_MAX], const int size, const int goal, const char botSymbol, const char playerSymbol);

   public:

    BotLevel2(const BotLevel& _level, const char& _symbol);

    ~BotLevel2() override;

    pII getMove(char board[][BOARD_N_MAX], const int size, const int goal) override;
};