/**
 * SDL Interaction header file
 *
 */

#pragma once

/* ---------- Importing ---------- */

#include <SDL2/SDL.h>

#include "../game/interface/i_interaction.h"
#include "../game/setup.h"
#include "../utils/config.h"

/* ---------- Declarations ---------- */

class SDLInteraction : public I_Interaction {
   private:
    int screenWidth;
    int screenHeight;
    int boardsize;

    int getInput();
    bool waitForQuit(SDL_Event& e);

   public:
    SDLInteraction();

    ~SDLInteraction();

    void init(const RunConfig& config) override;

    void pause(int timeout = 0) override;

    bool selectSize(int* size) override;

    bool selectGoal(int* goal, const int size) override;

    bool selectGameMode(GameMode* mode) override;

    bool selectBotLevel(BotLevel* levels, const int index) override;

    bool getPlayerMove(int* row, int* col) override;

    void close() override;
};