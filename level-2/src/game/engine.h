/**
 * Engine header file
 *
 */

#pragma once

/* ---------- Importing ---------- */

#include "../utils/config.h"
#include "interface/i_interaction.h"
#include "interface/i_renderer.h"
#include "setup.h"

enum class State {
    INIT, TITLE, SELECT_SIZE, SELECT_GOAL, SELECT_MODE,
    SELECT_BOT, PLAYING
};

class Engine {
   private:
    const RunConfig* config;      
    I_Renderer* iRenderer;        
    I_Interaction* iInteraction; 

    GameSetup gameSetup;  
    bool sanity_check();
    bool isRunning;
    State currentState = State::TITLE;

   public:
    Engine(const RunConfig* _config, I_Renderer* _iRenderer, I_Interaction* _iInteraction);
    ~Engine();

    void init();
    void close();

    void run();
    void handleEvents();
    void updateState();

    bool running() { return isRunning; };
    
};