/**
 * Setup header file
 *
 */

#pragma once

/* ---------- Importing ---------- */

#include <exception>
#include <random>
#include <utility>
#include "../utils/config.h"

/* ---------- Global Variables ---------- */

// ---------- Flag cấu hình ----------

inline const bool ALGORITHM_FLAG = true;  
inline const bool TIME_ENABLED = true;   
inline const int RANDOM_SEED = 8702;     

// ---------- Constants ----------

inline const int BOARD_N_MIN = 3;   
inline const int BOARD_N_MAX = 12;  
inline const int GOAL_MAX = 5;      
inline const int SLEEP_TIME = 1500;  
// ---------- Minimax / AI ----------

inline const int SCORE_INF = 1000;     
inline const int EVALUATE_SCORE = 10;

// ---------- Enum ----------

enum class BotLevel {
    EASY,
    MEDIUM,
    HARD,
    INVALID_LV
};

inline static std::string botToString(int v) {
    switch ((BotLevel)v) {
        case BotLevel::EASY:
            return "EASY";
        case BotLevel::MEDIUM:
            return "MEDIUM";
        case BotLevel::HARD:
            return "HARD";
        default:
            return "?";
    }
}

enum class GameMode {
    PVP,  
    PVE, 
    EVE,  
    INVALID_MODE
};

inline static std::string modeToString(int v) {
    switch ((GameMode)v) {
        case GameMode::PVP:
            return "PVP";
        case GameMode::PVE:
            return "PVE";
        case GameMode::EVE:
            return "EVE";
        default:
            return "?";
    }
}

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

constexpr int NO_CONTEXT = -1; 

enum class EndRule {
    NONE,     
    OPEN_ONE, 
    OPEN_TWO  
};

// ---------- Random ----------

inline std::mt19937 generator(RANDOM_SEED);

/* ---------- Type Definitions ---------- */

using pII = std::pair<int, int>;

struct GameSetup {
    char board[BOARD_N_MAX][BOARD_N_MAX];  
    int size;                              
    int goal;                              
    GameMode mode;                       
    BotLevel levels[2];                    
};

constexpr int DRAW_RESULT = -1; 

struct GameResult {
    int winner;  
    bool isBot;  
    int turns;   

    GameResult(int _winner, bool _isBot, int _turns)
        : winner(_winner), isBot(_isBot), turns(_turns) {};
};

class I_Renderer;
class I_Interaction;

struct GameContext {
    GameSetup& gameSetup;
    const RunConfig& config;
    I_Renderer& renderer;
    I_Interaction& interaction;
    bool isRunning;
    GameResult gameResult;

    GameContext(GameSetup& _setup, const RunConfig& _config, 
                I_Renderer& _renderer, I_Interaction& _interaction)
        : gameSetup(_setup), config(_config), renderer(_renderer), 
          interaction(_interaction), isRunning(true), 
          gameResult(DRAW_RESULT, false, 0) {}
};

class QuitException : public std::exception {
   public:
    const char* what() const noexcept override {
        return "User requested quit";
    }
};

class NotImplementedException : public std::exception {
   public:
    const char* what() const noexcept override {
        return "Functionality not implemented yet";
    }
};