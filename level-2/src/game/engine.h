
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

    GameResult playLoop();
    bool isBotPlayer(const int player) const;

   public:
    Engine(const RunConfig* _config, I_Renderer* _iRenderer, I_Interaction* _iInteraction);
    ~Engine();

    void init();
    void close();

    void startGame();
    void selectSize();
    void selectGoal();
    void selectGameMode();
    void selectBotLevels();
    GameResult playGame();
    void endGame(const GameResult& gameResult);

    bool running() { return isRunning; };
    
};