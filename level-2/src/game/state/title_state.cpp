
#include "title_state.h"
#include "select_size_state.h"
#include "../../utils/logger.h"
#include "../setup.h"
#include "../interface/i_renderer.h"
#include "../interface/i_interaction.h"

std::unique_ptr<IGameState> TitleState::execute(GameContext& context) {
    Logger::log("[TitleState] Executing title state");

    if (context.config.interactive) {
        context.renderer.showSelectMenu(SelectType::TITLE_UI);
        context.interaction.pause();
    }

    return std::make_unique<SelectSizeState>();
}
