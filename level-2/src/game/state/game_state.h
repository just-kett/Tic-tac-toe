

#pragma once

#include "../setup.h"
#include <memory>

struct GameContext;

class IGameState {
public:
    virtual ~IGameState() = default;

    virtual std::unique_ptr<IGameState> execute(GameContext& context) = 0;

    virtual const char* getName() const = 0;
};
