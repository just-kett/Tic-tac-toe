
#include "end_game_state.h"
#include "../../utils/logger.h"
#include "../setup.h"
#include "../logic.h"
#include "../interface/i_renderer.h"
#include "../interface/i_interaction.h"
#include <optional>

std::unique_ptr<IGameState> EndGameState::execute(GameContext& context) {
    Logger::log("[EndGameState] Executing end game state");

    if (context.config.interactive) {
        const WinLine* winLine = nullptr;
        std::optional<WinLine> maybeWinLine;

        if (context.gameResult.winner >= 0) {
            maybeWinLine = Logic::getWinLine(
                context.gameSetup.board,
                context.gameSetup.size,
                context.gameResult.winner == 0 ? 'X' : 'O',
                context.gameSetup.goal);
            if (maybeWinLine.has_value()) {
                winLine = &maybeWinLine.value();
            }
        }

        context.renderer.showResult(context.gameResult.winner, context.gameResult.isBot, 
                                    context.gameSetup.size, context.gameSetup.board, winLine);
    }

    if (context.config.judge_mode) {
        context.renderer.printResult(context.gameResult);
    }

    Logger::log("[EndGameState] Game ended!");
    
    // Return nullptr to signal end of game
    return nullptr;
}
