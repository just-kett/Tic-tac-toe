/**
 * Bot header file
 *
 */

#pragma once

/* ---------- Importing ---------- */

#include "../setup.h"

/* ---------- Declarations ---------- */

class Bot {
   protected:
    const BotLevel level;  // mức độ khó của bot
    const char symbol;     // ký hiệu của bot ('X' hoặc 'O')
    const char op_symbol;  // ký hiệu đối thủ

   public:

    Bot(const BotLevel& _level, const char& _symbol);

    virtual ~Bot();

    virtual pII getMove(char board[][BOARD_N_MAX], const int size, const int goal) = 0;
};