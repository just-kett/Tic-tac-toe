
#pragma once

#include "game_state.h"

class PlayingState : public IGameState {
public:
    std::unique_ptr<IGameState> execute(GameContext& context) override;
    const char* getName() const override { return "PLAYING"; }

private:
    bool isBotPlayer(const GameContext& context, int player) const;
};
