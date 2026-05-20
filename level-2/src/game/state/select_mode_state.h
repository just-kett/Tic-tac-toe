

#pragma once

#include "game_state.h"

class SelectModeState : public IGameState {
public:
    std::unique_ptr<IGameState> execute(GameContext& context) override;
    const char* getName() const override { return "SELECT_MODE"; }
};
