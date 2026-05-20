/**
 * Bot Factory header file
 *
 */

#pragma once

/* ---------- Importing ---------- */

#include "../setup.h"
#include "bot.h"

/* ---------- Declarations ---------- */

class BotFactory {
   public:

    static Bot* createBot(BotLevel level, const char& symbol);
    
};