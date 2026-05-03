#include "gameEngine.h"
#include "gameInteraction.h"
#include "gameRenderer.h"
#include "gameLogic.h"
#include "gameLogger.h"
#include "gameHelp.h"

#include <vector>

void startGame(const RunConfig& config, GameSetup& gameSetup) {
    if (config.interactive) {
        clearScreen();
        showSelectMenu(SelectType::TITLE_UI);
    }

    bool validSize = false;
    while (!validSize) {
        if (config.interactive) {
            showSelectMenu(SelectType::SIZE_UI);
        }
        validSize = selectSize(&gameSetup.size);
        if (!validSize && config.interactive) {
            showInvalidMove();
        }
    }

    bool validGoal = false;
    while (!validGoal) {
        if (config.interactive) {
            showSelectMenu(SelectType::GOAL_UI);
        }
        validGoal = selectGoal(&gameSetup.goal, gameSetup.size);
        if (!validGoal && config.interactive) {
            showInvalidMove();
        }
    }

    bool validMode = false;
    while (!validMode) {
        if (config.interactive) {
            showSelectMenu(SelectType::GAME_MODE_UI);
        }
        validMode = selectGameMode(&gameSetup.mode);
        if (!validMode && config.interactive) {
            showInvalidMove();
        }
    }

    if (gameSetup.mode == GameMode::PVE) {
        bool validBotLevel = false;
        while (!validBotLevel) {
            showSelectMenu(SelectType::BOT_LEVEL_UI);
            validBotLevel = selectBotLevel(gameSetup.botLevels, 1);
            if (!validBotLevel && config.interactive) {
                showInvalidMove();
            }
        }
    }
    else if (gameSetup.mode == GameMode::EVE) {
        bool validBotLevel1 = false;
        while (!validBotLevel1) {
            showSelectMenu(SelectType::MUL_BOT_LEVEL_UI);
            validBotLevel1 = selectBotLevel(gameSetup.botLevels, 0);
            if (!validBotLevel1 && config.interactive) {
                showInvalidMove();
            }
        }
        
        bool validBotLevel2 = false;
        while (!validBotLevel2) {
            showSelectMenu(SelectType::BOT_LEVEL_UI);
            validBotLevel2 = selectBotLevel(gameSetup.botLevels, 1);
            if (!validBotLevel2 && config.interactive) {
                showInvalidMove();
            }
        }
    }

    initBoard(gameSetup.board, gameSetup.size);
}

GameResult playGame(const RunConfig& config, GameSetup& gameSetup) {
    GameResult result;
    int currentPlayer = 0;
    char symbols[2] = {'X', 'O'};
    int turns = 0;

    while (true) {
        if (config.interactive) {
            displayBoard(gameSetup.board, gameSetup.size);
        }
        
        bool bot = false;
        if (gameSetup.mode == GameMode::EVE) {
            bot = true;
        }  

        if (gameSetup.mode == GameMode::PVE && currentPlayer == 1) {
            bot = true;
        }

        if (config.interactive) {
            showPlayer(currentPlayer + 1, bot);
        }

        int row, col;
        bool validmove = false;
        while (!validmove) {
            if (bot) {
                pII point = measureExecutionTime(
                    "botmove",
                    [&]() {
                        return botMove(gameSetup.board,
                                       gameSetup.size,
                                       gameSetup.goal,
                                       symbols[currentPlayer],
                                       gameSetup.botLevels[currentPlayer]);
                    },
                    TIME_ENABLED);
                    
                    row = point.first;
                    col = point.second;
            }
            else {
                if (config.interactive) {
                    showSelectMenu(SelectType::PLAYER_UI);
                }
                if (!getPlayerMove(&row, &col)) {
                    if (config.interactive) {
                        showInvalidMove();
                    }
                    continue;
                }
            }
            if (!isValidMove(gameSetup.board, gameSetup.size, row, col)) {
                if (config.interactive) {
                    showInvalidMove();
                }
                continue;
            }
            validmove = true;
        }
        
        makeMove(gameSetup.board, row, col, symbols[currentPlayer]);
        turns++;

        if (config.interactive) {
            showMove(row, col);
        }

        if (checkWin(gameSetup.board, gameSetup.size, symbols[currentPlayer], gameSetup.goal)) {
            result.winner = currentPlayer;
            result.moveCount = turns;
            return result;
        }

        if (checkDraw(gameSetup.board, gameSetup.size)) {
            result.winner = -1;
            result.moveCount = turns;
            return result;
        }
        currentPlayer = 1 - currentPlayer;
    }
}

void endGame(const RunConfig& config, GameSetup& gameSetup, GameResult& gameResult) {
    if (config.interactive) {
        clearScreen();
        displayBoard(gameSetup.board, gameSetup.size);
        showResult(gameResult.winner, gameResult.moveCount > 0 && gameSetup.mode == GameMode::EVE);
    }

    if (config.judge_mode) {
        printResult(gameResult);                
    }
}
