/**
 * Engine cpp implementation
 *
 */

#include "engine.h"

/* ---------- Importing ---------- */

#include <format>
#include <memory>
#include <optional>
#include <sstream>
#include <thread>
#include <vector>

#include "../utils/helper.h"
#include "../utils/logger.h"
#include "bot/bot_factory.h"
#include "logic.h"

Engine::Engine(const RunConfig* _config, I_Renderer* _iRenderer, I_Interaction* _iInteraction)
    : config(_config), iRenderer(_iRenderer), iInteraction(_iInteraction), isRunning(false), currentState(State::TITLE) {
}

Engine::~Engine() = default;

void Engine::init() {
    Logger::log("Engine initializing . . .");

    if (!sanity_check()) {
        throw NotImplementedException();
    }
    iRenderer->init(*config);
    iInteraction->init(*config);

    Logger::log("Engine initialized!");
    isRunning = true;
}

bool Engine::sanity_check() {
    bool isRendererGood = iRenderer != nullptr;
    bool isInteractionGood = iInteraction != nullptr;

    if (!isRendererGood) {
        Logger::log("Interface Renderer is not implemented!", Logger::Level::WARNING);
    }
    if (!isInteractionGood) {
        Logger::log("Interface Interaction is not implemented!", Logger::Level::WARNING);
    }

    isRunning = isRendererGood && isInteractionGood;
    return isRunning;
}

void Engine::startGame() {
    if (!sanity_check()) {
        throw NotImplementedException();
    }

    if (config->interactive) {
        iRenderer->showSelectMenu(SelectType::TITLE_UI);
        iInteraction->pause();
    }

    selectSize();
    selectGoal();
    selectGameMode();
    selectBotLevels();

    Logic::initBoard(gameSetup.board, gameSetup.size);
    Logger::log("Board initialized!");
    Logger::log("[Engine] Game started!");

    currentState = State::PLAYING;
}

void Engine::selectSize() {
    bool isSelected = false;

    while (!isSelected) {
        if (config->interactive) {
            iRenderer->showSelectMenu(SelectType::SIZE_UI);
        }

        isSelected = iInteraction->selectSize(&gameSetup.size);
        if (!isSelected && config->interactive) {
            iRenderer->showInvalidSelect(SelectType::SIZE_UI, gameSetup.size);
        }
    }

    if (config->interactive) {
        iRenderer->showValidSelect(SelectType::SIZE_UI, gameSetup.size);
    }
}

void Engine::selectGoal() {
    bool isSelected = false;

    while (!isSelected) {
        if (config->interactive) {
            iRenderer->showSelectMenu(SelectType::GOAL_UI, gameSetup.size);
        }

        isSelected = iInteraction->selectGoal(&gameSetup.goal, gameSetup.size);
        if (!isSelected && config->interactive) {
            iRenderer->showInvalidSelect(SelectType::GOAL_UI, gameSetup.goal);
        }
    }

    if (config->interactive) {
        iRenderer->showValidSelect(SelectType::GOAL_UI, gameSetup.goal);
    }
}

void Engine::selectGameMode() {
    bool isSelected = false;

    while (!isSelected) {
        if (config->interactive) {
            iRenderer->showSelectMenu(SelectType::GAME_MODE_UI);
        }

        isSelected = iInteraction->selectGameMode(&gameSetup.mode);
        if (!isSelected && config->interactive) {
            iRenderer->showInvalidSelect(SelectType::GAME_MODE_UI, (int)gameSetup.mode);
        }
    }

    if (config->interactive) {
        iRenderer->showValidSelect(SelectType::GAME_MODE_UI, (int)gameSetup.mode);
    }
}

void Engine::selectBotLevels() {
    if (gameSetup.mode == GameMode::PVE) {
        bool isSelected = false;
        while (!isSelected) {
            if (config->interactive) {
                iRenderer->showSelectMenu(SelectType::BOT_LEVEL_UI);
            }
            isSelected = iInteraction->selectBotLevel(gameSetup.levels, 1);
            if (!isSelected && config->interactive) {
                iRenderer->showInvalidSelect(SelectType::BOT_LEVEL_UI, (int)gameSetup.levels[1]);
            }
        }
        if (config->interactive) {
            iRenderer->showValidSelect(SelectType::BOT_LEVEL_UI, (int)gameSetup.levels[1]);
        }
    } else if (gameSetup.mode == GameMode::EVE) {
        for (int index = 0; index < 2; ++index) {
            bool isSelected = false;
            while (!isSelected) {
                if (config->interactive) {
                    iRenderer->showSelectMenu(SelectType::MUL_BOT_LEVEL_UI, index);
                }
                isSelected = iInteraction->selectBotLevel(gameSetup.levels, index);
                if (!isSelected && config->interactive) {
                    iRenderer->showInvalidSelect(SelectType::MUL_BOT_LEVEL_UI, (int)gameSetup.levels[index]);
                }
            }
            if (config->interactive) {
                iRenderer->showValidSelect(SelectType::MUL_BOT_LEVEL_UI, (int)gameSetup.levels[index]);
            }
        }
    }
}

GameResult Engine::playGame() {
    return playLoop();
}

bool Engine::isBotPlayer(const int player) const {
    return gameSetup.mode == GameMode::EVE || (gameSetup.mode == GameMode::PVE && player == 1);
}

GameResult Engine::playLoop() {
    const char symbols[2] = {'X', 'O'};
    std::vector<std::unique_ptr<Bot>> bots;
    bots.reserve(2);

    if (gameSetup.mode == GameMode::PVE) {
        bots.emplace_back(nullptr);
        bots.emplace_back(std::unique_ptr<Bot>(BotFactory::createBot(gameSetup.levels[1], symbols[1])));
    } else if (gameSetup.mode == GameMode::EVE) {
        bots.emplace_back(std::unique_ptr<Bot>(BotFactory::createBot(gameSetup.levels[0], symbols[0])));
        bots.emplace_back(std::unique_ptr<Bot>(BotFactory::createBot(gameSetup.levels[1], symbols[1])));
    }

    GameResult gameResult(DRAW_RESULT, false, 0);
    int player = 0;

    while (isRunning) {
        Logger::log(std::format("[Engine] starting turn #{}", gameResult.turns), Logger::Level::DEBUG);

        if (config->interactive) {
            iRenderer->clearScreen();
            iRenderer->displayBoard(gameSetup.board, gameSetup.size);
            iRenderer->showPlayer(player, isBotPlayer(player));
        }

        int row = -1;
        int col = -1;

        if (isBotPlayer(player)) {
            pII point = measureExecutionTime(
                std::format("bot#{}->getMove()", player),
                [&]() {
                    return bots[player]->getMove(gameSetup.board, gameSetup.size, gameSetup.goal);
                },
                TIME_ENABLED);
            row = point.first;
            col = point.second;
        } else {
            bool isValid = false;
            do {
                if (config->interactive) {
                    iRenderer->showSelectMenu(SelectType::PLAYER_UI);
                }
                if (!iInteraction->getPlayerMove(&row, &col) || !Logic::isValidMove(gameSetup.board, gameSetup.size, row, col)) {
                    if (config->interactive) {
                        iRenderer->showInvalidMove();
                    }
                    continue;
                }
                isValid = true;
            } while (!isValid);
        }

        Logic::makeMove(gameSetup.board, row, col, symbols[player]);
        if (config->interactive) {
            iRenderer->showMove(row, col);
        }

        gameResult.turns += 1;
        Logger::log(std::format("player {} make move to ({}, {})", player + 1, row, col), Logger::Level::DEBUG);

        if (isBotPlayer(player) && config->interactive) {
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        }

        if (Logic::checkWin(gameSetup.board, gameSetup.size, symbols[player], gameSetup.goal)) {
            gameResult.winner = player;
            gameResult.isBot = isBotPlayer(player);
            break;
        }

        if (Logic::checkDraw(gameSetup.board, gameSetup.size)) {
            gameResult.winner = DRAW_RESULT;
            gameResult.isBot = false;
            break;
        }

        player = (player + 1) % 2;
        Logger::log("[Engine] turn done!", Logger::Level::DEBUG);
    }

    Logger::log("[Engine] Game done!");
    return gameResult;
}

void Engine::endGame(const GameResult& gameResult) {
    if (config->interactive) {
        const WinLine* winLine = nullptr;
        std::optional<WinLine> maybeWinLine;

        if (gameResult.winner >= 0) {
            maybeWinLine = Logic::getWinLine(
                gameSetup.board,
                gameSetup.size,
                gameResult.winner == 0 ? 'X' : 'O',
                gameSetup.goal);
            if (maybeWinLine.has_value()) {
                winLine = &maybeWinLine.value();
            }
        }

        iRenderer->showResult(gameResult.winner, gameResult.isBot, winLine);
    }

    if (config->judge_mode) {
        iRenderer->printResult(gameResult);
    }
}

void Engine::close() {
    Logger::log("Engine closing . . .");
    iRenderer->close();
    iInteraction->close();
    Logger::log("Engine closed!");
}
