

#include "select_mode_state.h"
#include "select_bot_state.h"
#include "../../utils/logger.h"
#include "../setup.h"
#include "../interface/i_renderer.h"
#include "../interface/i_interaction.h"

std::unique_ptr<IGameState> SelectModeState::execute(GameContext& context) {
    Logger::log("[SelectModeState] Executing select mode state");

    bool isSelected = false;

    while (!isSelected) {
        if (context.config.interactive) {
            context.renderer.showSelectMenu(SelectType::GAME_MODE_UI);
        }

        isSelected = context.interaction.selectGameMode(&context.gameSetup.mode);
        if (!isSelected && context.config.interactive) {
            context.renderer.showInvalidSelect(SelectType::GAME_MODE_UI, (int)context.gameSetup.mode);
        }
    }

    if (context.config.interactive) {
        context.renderer.showValidSelect(SelectType::GAME_MODE_UI, (int)context.gameSetup.mode);
    }

    Logger::log("[SelectModeState] Mode selected: " + modeToString((int)context.gameSetup.mode));

    return std::make_unique<SelectBotState>();
}
