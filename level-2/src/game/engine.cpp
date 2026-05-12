/**
 * Engine cpp implementation
 *
 */

#include "engine.h"

/* ---------- Importing ---------- */

#include <chrono>
#include <optional>
#include <sstream>
#include <thread>
#include <vector>

#include "../utils/helper.h"
#include "../utils/logger.h"
#include "bot/bot_factory.h"
#include "logic.h"
#include "SDL2/SDL.h"

Engine::Engine(const RunConfig* _config, I_Renderer* _iRenderer, I_Interaction* _iInteraction) {

    config = _config;
    iRenderer = _iRenderer;
    iInteraction = _iInteraction;

}

Engine::~Engine() {}

void Engine::init() {

    Logger::log("Engine initializing . . .");

    iRenderer->init(*config);     
    iInteraction->init(*config); 

    Logger::log("Engine initialized!");

}

bool Engine::sanity_check() {

    bool isRendererGood = iRenderer;
    if (!isRendererGood) {
        Logger::log("Interface Renderer is not implemented!", Logger::Level::WARNING);
    }

    bool isInteractionGood = iInteraction;
    if (!isInteractionGood) {
        Logger::log("Interface Interaction is not implemented!", Logger::Level::WARNING);
    }

    return isRendererGood && isInteractionGood;

}

void Engine::run() {

    handleEvents();
    updateState();
    if (config->interactive) {
        iInteraction->pause();
    }
    while (isRunning && sanity_check()) {
        if (currentState != State::PLAYING) {
            SDL_Delay(100);
        }
        else {
            SDL_Delay(16); 
        }
    }

}

void Engine::handleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch(event.type) {
        case SDL_QUIT:
            isRunning = false;
            break;
        default:
            break;
    }
}

void Engine::updateState() {

}

void Engine::close() {

    Logger::log("Engine closing . . .");

    iRenderer->close();
    iInteraction->close();

    Logger::log("Engine closed!");

}