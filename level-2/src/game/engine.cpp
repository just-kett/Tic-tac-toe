
#include "engine.h"

/* ---------- Importing ---------- */

#include <format>
#include <memory>

#include "../utils/logger.h"
#include "state/title_state.h"

Engine::Engine(const RunConfig* _config, I_Renderer* _iRenderer, I_Interaction* _iInteraction)
    : config(_config), 
      iRenderer(_iRenderer), 
      iInteraction(_iInteraction),
      gameSetup{},
      gameContext(gameSetup, *_config, *_iRenderer, *_iInteraction),
      currentState(std::make_unique<TitleState>()) {
}

Engine::~Engine() = default;

void Engine::init() {
    Logger::log("Engine initializing . . .");

    if (!sanity_check()) {
        throw NotImplementedException();
    }
    iRenderer->init(*config);
    iInteraction->init(*config);

    Logger::log("Engine initialized!");
}

bool Engine::sanity_check() {
    bool isRendererGood = iRenderer != nullptr;
    bool isInteractionGood = iInteraction != nullptr;

    if (!isRendererGood) {
        Logger::log("Interface Renderer is not implemented!", Logger::Level::WARNING);
    }
    if (!isInteractionGood) {
        Logger::log("Interface Interaction is not implemented!", Logger::Level::WARNING);
    }

    return isRendererGood && isInteractionGood;
}

void Engine::run() {
    Logger::log("[Engine] Game started!");
    gameContext.isRunning = true;

    while (gameContext.isRunning && currentState) {
        Logger::log(std::format("[Engine] Current state: {}", currentState->getName()));
  
        auto nextState = currentState->execute(gameContext);
        currentState = std::move(nextState);

        if (!currentState) {
            gameContext.isRunning = false;
        }
    }

    Logger::log("[Engine] Game finished!");
}

void Engine::close() {
    Logger::log("Engine closing . . .");
    iRenderer->close();
    iInteraction->close();
    Logger::log("Engine closed!");
}