
#include "playing_state.h"
#include "end_game_state.h"
#include "../../utils/logger.h"
#include "../../utils/helper.h"
#include "../setup.h"
#include "../logic.h"
#include "../bot/bot_factory.h"
#include "../interface/i_renderer.h"
#include "../interface/i_interaction.h"
#include <format>
#include <memory>
#include <thread>
#include <vector>

bool PlayingState::isBotPlayer(const GameContext& context, int player) const {
    return context.gameSetup.mode == GameMode::EVE || 
           (context.gameSetup.mode == GameMode::PVE && player == 1);
}

std::unique_ptr<IGameState> PlayingState::execute(GameContext& context) {
    Logger::log("[PlayingState] Game started!");

    const char symbols[2] = {'X', 'O'};
    std::vector<std::unique_ptr<Bot>> bots;
    bots.reserve(2);

    if (context.gameSetup.mode == GameMode::PVE) {
        bots.emplace_back(nullptr);  
        bots.emplace_back(std::unique_ptr<Bot>(
            BotFactory::createBot(context.gameSetup.levels[1], symbols[1])
        ));
    } else if (context.gameSetup.mode == GameMode::EVE) {
        bots.emplace_back(std::unique_ptr<Bot>(
            BotFactory::createBot(context.gameSetup.levels[0], symbols[0])
        ));
        bots.emplace_back(std::unique_ptr<Bot>(
            BotFactory::createBot(context.gameSetup.levels[1], symbols[1])
        ));
    }

    int player = 0;

    while (context.isRunning) {
        Logger::log(std::format("[PlayingState] starting turn #{}", context.gameResult.turns), 
                   Logger::Level::DEBUG);

        if (context.config.interactive) {
            context.renderer.clearScreen();
            context.renderer.displayBoard(context.gameSetup.board, context.gameSetup.size, true);
            context.renderer.showPlayer(player, isBotPlayer(context, player));
        }

        int row = -1;
        int col = -1;

        if (isBotPlayer(context, player)) {
            pII point = measureExecutionTime(
                std::format("bot#{}->getMove()", player),
                [&]() {
                    return bots[player]->getMove(context.gameSetup.board, 
                                                  context.gameSetup.size, 
                                                  context.gameSetup.goal);
                },
                TIME_ENABLED);
            row = point.first;
            col = point.second;
        } else {
            bool isValid = false;
            do {
                if (context.config.interactive) {
                    context.renderer.showSelectMenu(SelectType::PLAYER_UI);
                }
                if (!context.interaction.getPlayerMove(&row, &col) || 
                    !Logic::isValidMove(context.gameSetup.board, context.gameSetup.size, row, col)) {
                    if (context.config.interactive) {
                        context.renderer.showInvalidMove();
                    }
                    continue;
                }
                isValid = true;
            } while (!isValid);
        }

        Logic::makeMove(context.gameSetup.board, row, col, symbols[player]);
        if (context.config.interactive) {
            context.renderer.showMove(row, col);
        }

        if (context.config.interactive) {
            context.renderer.clearScreen();
            context.renderer.displayBoard(context.gameSetup.board, context.gameSetup.size, true);
            context.renderer.showPlayer(player, isBotPlayer(context, player));
        }

        context.gameResult.turns += 1;
        Logger::log(std::format("player {} make move to ({}, {})", player + 1, row, col), 
                   Logger::Level::DEBUG);

        if (isBotPlayer(context, player) && context.config.interactive) {
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        }

        if (Logic::checkWin(context.gameSetup.board, context.gameSetup.size, 
                           symbols[player], context.gameSetup.goal)) {
            context.gameResult.winner = player;
            context.gameResult.isBot = isBotPlayer(context, player);
            Logger::log(std::format("[PlayingState] Player {} wins!", player));
            break;
        }

        if (Logic::checkDraw(context.gameSetup.board, context.gameSetup.size)) {
            context.gameResult.winner = DRAW_RESULT;
            context.gameResult.isBot = false;
            Logger::log("[PlayingState] Game is a draw!");
            break;
        }

        player = (player + 1) % 2;
        Logger::log("[PlayingState] turn done!", Logger::Level::DEBUG);
    }

    Logger::log("[PlayingState] Game done!");

    return std::make_unique<EndGameState>();
}
