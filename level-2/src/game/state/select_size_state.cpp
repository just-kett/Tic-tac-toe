

#include "select_size_state.h"
#include "select_goal_state.h"
#include "../../utils/logger.h"
#include "../setup.h"
#include "../interface/i_renderer.h"
#include "../interface/i_interaction.h"

std::unique_ptr<IGameState> SelectSizeState::execute(GameContext& context) {
    Logger::log("[SelectSizeState] Executing select size state");

    bool isSelected = false;

    while (!isSelected) {
        if (context.config.interactive) {
            context.renderer.showSelectMenu(SelectType::SIZE_UI);
        }

        isSelected = context.interaction.selectSize(&context.gameSetup.size);
        if (!isSelected && context.config.interactive) {
            context.renderer.showInvalidSelect(SelectType::SIZE_UI, context.gameSetup.size);
        }
    }

    if (context.config.interactive) {
        context.renderer.showValidSelect(SelectType::SIZE_UI, context.gameSetup.size);
    }

    Logger::log("[SelectSizeState] Size selected: " + std::to_string(context.gameSetup.size));

    return std::make_unique<SelectGoalState>();
}
