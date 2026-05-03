#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <string>
#include <utility>

// version
const std::string VERSION = "0.4.25023041";

// flag
const bool ALGORITHM_FLAG = true;
const bool TIME_ENABLED = true;

// Fixed seed ensures deterministic behavior during testing.
const int RANDOM_SEED = 2013;

// constants
const int BOARD_N_MAX = 12;
const int SLEEP_TIME = 1500;

// Global random number generator
#include <random>
extern std::mt19937 generator;

// Bot difficulty levels
enum class BotLevel {
    EASY,
    MEDIUM,
    HARD,
    INVALID_LV
};

// Game modes
enum class GameMode {
    PVP,
    PVE,
    EVE,
    INVALID_MODE
};

// UI menu types
enum class SelectType {
    TITLE_UI,
    SIZE_UI,
    GOAL_UI,
    GAME_MODE_UI,
    BOT_LEVEL_UI,
    PLAYER_UI,
    MUL_BOT_LEVEL_UI,
    INVALID_UI
};

// End-point rule
enum class EndRule {
    NONE,
    OPEN_ONE,
    OPEN_TWO
};

// Alias for board coordinate
typedef std::pair<int, int> pII;

// Runtime configuration
struct RunConfig {
    bool interactive = true;
    bool judge_mode = false;
    std::string input_file;

    bool to_file = true;
    std::string log_file = "log.txt";
};

// Game setup
struct GameSetup {
    int size = 3;
    int goal = 3;
    GameMode mode = GameMode::PVP;
    BotLevel botLevels[2] = {BotLevel::EASY, BotLevel::EASY};
    EndRule endRule = EndRule::OPEN_TWO;
    char playerSymbols[2] = {'X', 'O'};
    bool isBot[2] = {false, false};
    char board[BOARD_N_MAX][BOARD_N_MAX];
};

// Game result
struct GameResult {
    int winner = -1;  // -1: draw, 0: player1, 1: player2
    int moveCount = 0;
    bool isDraw = false;
};

#endif 