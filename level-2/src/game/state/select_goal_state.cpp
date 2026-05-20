
#include "select_goal_state.h"
#include "select_mode_state.h"
#include "../../utils/logger.h"
#include "../setup.h"
#include "../interface/i_renderer.h"
#include "../interface/i_interaction.h"

std::unique_ptr<IGameState> SelectGoalState::execute(GameContext& context) {
    Logger::log("[SelectGoalState] Executing select goal state");

    bool isSelected = false;

    while (!isSelected) {
        if (context.config.interactive) {
            context.renderer.showSelectMenu(SelectType::GOAL_UI, context.gameSetup.size);
        }

        isSelected = context.interaction.selectGoal(&context.gameSetup.goal, context.gameSetup.size);
        if (!isSelected && context.config.interactive) {
            context.renderer.showInvalidSelect(SelectType::GOAL_UI, context.gameSetup.goal);
        }
    }

    if (context.config.interactive) {
        context.renderer.showValidSelect(SelectType::GOAL_UI, context.gameSetup.goal);
    }

    Logger::log("[SelectGoalState] Goal selected: " + std::to_string(context.gameSetup.goal));

    return std::make_unique<SelectModeState>();
}
