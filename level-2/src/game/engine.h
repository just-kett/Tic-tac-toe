
#pragma once

/* ---------- Importing ---------- */

#include "../utils/config.h"
#include "interface/i_interaction.h"
#include "interface/i_renderer.h"
#include "setup.h"
#include "state/game_state.h"
#include <memory>

class Engine {
   private:
    const RunConfig* config;      
    I_Renderer* iRenderer;        
    I_Interaction* iInteraction; 

    GameSetup gameSetup;  
    GameContext gameContext;
    
    std::unique_ptr<IGameState> currentState;
    
    bool sanity_check();

   public:
    Engine(const RunConfig* _config, I_Renderer* _iRenderer, I_Interaction* _iInteraction);
    ~Engine();

    void init();
    void close();
    void run();

    bool running() const { return gameContext.isRunning; }
    
};