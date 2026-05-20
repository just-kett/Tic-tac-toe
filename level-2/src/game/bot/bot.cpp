/**
 * Bot cpp implementation
 *
 */

#include "bot.h"

/* ---------- Importing ---------- */

/* ---------- Definitions ---------- */

Bot::Bot(const BotLevel& _level, const char& _symbol)
    : level(_level), symbol(_symbol), op_symbol((symbol == 'O') ? 'X' : 'O') {
}

Bot::~Bot() {
}