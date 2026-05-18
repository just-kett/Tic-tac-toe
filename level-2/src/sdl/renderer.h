/**
 * SDL Renderer header file
 *
 */

#pragma once

/* ---------- Importing ---------- */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../game/interface/i_renderer.h"
#include "../game/logic.h"

/* ---------- Declarations ---------- */

class SDLRenderer : public I_Renderer {
   private:
    SDL_Window* window = nullptr;      
    SDL_Renderer* renderer = nullptr; 
    TTF_Font* font = nullptr;
    TTF_Font* titleFont = nullptr;
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* boardTexture = nullptr;
    int screenWidth;
    int screenHeight;
    int currentBoardSize;
    int boardPadding;
    
    
    void renderPresent();

   public:

    SDLRenderer();

    ~SDLRenderer();

    void init(const RunConfig& config) override;

    void clearScreen() override;

    void showSelectMenu(SelectType selectType, int context = NO_CONTEXT) override;

    void showInvalidSelect(SelectType selectType, int context = NO_CONTEXT) override;

    void showValidSelect(SelectType selectType, int context = NO_CONTEXT) override;

    void displayBoard(const char board[][BOARD_N_MAX], const int size, const bool showWinline) override;

    void showMove(const int row, const int col) override;

    void showInvalidMove() override;

    void showPlayer(const int player, const bool is_bot) override;

    void showResult(const int winner, const bool is_bot, const int size, const char board[][BOARD_N_MAX], const WinLine* winLine = nullptr) override;

    void printResult(const GameResult& gameResult) override;

    void close() override;
};