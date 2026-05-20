
#include "select_bot_state.h"
#include "playing_state.h"
#include "../../utils/logger.h"
#include "../setup.h"
#include "../logic.h"
#include "../interface/i_renderer.h"
#include "../interface/i_interaction.h"

std::unique_ptr<IGameState> SelectBotState::execute(GameContext& context) {
    Logger::log("[SelectBotState] Executing select bot state");

    switch (context.gameSetup.mode) {
        case GameMode::PVP:
            Logger::log("[SelectBotState] PVP mode - no bot selection needed");
            break;
        case GameMode::PVE:
            selectBotLevelForPlayer(context, 1);
            break;
        case GameMode::EVE:
            selectBotLevelForPlayer(context, 0);
            selectBotLevelForPlayer(context, 1);
            break;
        default:
            Logger::log("[SelectBotState] Unknown game mode!", Logger::Level::WARNING);
            break;
    }

    Logic::initBoard(context.gameSetup.board, context.gameSetup.size);
    Logger::log("[SelectBotState] Board initialized!");

    return std::make_unique<PlayingState>();
}

void SelectBotState::selectBotLevelForPlayer(GameContext& context, int playerIndex) {
    bool isSelected = false;
    SelectType selectType = (context.gameSetup.mode == GameMode::EVE) 
        ? SelectType::MUL_BOT_LEVEL_UI 
        : SelectType::BOT_LEVEL_UI;

    while (!isSelected) {
        if (context.config.interactive) {
            context.renderer.showSelectMenu(selectType, playerIndex);
        }

        isSelected = context.interaction.selectBotLevel(context.gameSetup.levels, playerIndex);
        if (!isSelected && context.config.interactive) {
            context.renderer.showInvalidSelect(selectType, (int)context.gameSetup.levels[playerIndex]);
        }
    }

    if (context.config.interactive) {
        context.renderer.showValidSelect(selectType, (int)context.gameSetup.levels[playerIndex]);
    }

    Logger::log("[SelectBotState] Bot " + std::to_string(playerIndex) + " level selected: " + 
                botToString((int)context.gameSetup.levels[playerIndex]));
}
