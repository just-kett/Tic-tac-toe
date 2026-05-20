/**
 * SDL Renderer cpp implementation
 *
 */
 
#include "renderer.h"
 
/* ---------- Importing ---------- */
 
#include <algorithm>
#include <cmath>
#include <format>
#include <iostream>
 
#include "../game/setup.h"
#include "../utils/config.h"
 
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
 
/* ---------- Definitions ---------- */
 
SDLRenderer::SDLRenderer() : I_Renderer() {}
 
SDLRenderer::~SDLRenderer() {}
 
void SDLRenderer::init(const RunConfig &config) {

    screenWidth = config.screenWidth;
    screenHeight = config.screenHeight;
    boardPadding = config.boardPadding;
 
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    TTF_Init();
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
 
    window = SDL_CreateWindow(
        "TicTacToe SDL",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,
        0);
 
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
 
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
 
    backgroundTexture = IMG_LoadTexture(renderer, "./assets/images.png");
    if (!backgroundTexture) {
        std::cerr << "[SDL_image ERROR] Could not load images.png. Error: " << IMG_GetError() << std::endl;
    }

    boardTexture = IMG_LoadTexture(renderer, "./assets/board.png");
    if (!boardTexture) {
        std::cerr << "[SDL_image ERRO] Could not load board.png. Error: " << IMG_GetError() << std::endl;
    }
 
    const char* fontPaths[] = {
        "./assets/font.ttf"      
    };
 
    for (const char* path : fontPaths) {
        font = TTF_OpenFont(path, 100);

        if (font) {
            std::cout << "[SDL_ttf] Loaded UI font from: " << path << std::endl;
            break;
        }
    }
 
    for (const char* path : fontPaths) {
        titleFont = TTF_OpenFont(path, 180);
        if (titleFont) {
            std::cout << "[SDL_ttf] Loaded title font from: " << path << std::endl;
            break;
        }
    }

    for (const char* path : fontPaths) {
        endgameFont = TTF_OpenFont(path, 150);
        if (endgameFont) {
            std::cout << "[SDL_ttf] Loaded engame font from: " << path << std::endl;
        }
    }
 
    if (!font) {
        std::cerr << "[SDL_ttf ERROR] Could not load UI font. Error: " << TTF_GetError() << std::endl;
    }
    if (!titleFont) {
        std::cerr << "[SDL_ttf ERROR] Could not load title font. Error: " << TTF_GetError() << std::endl;
        titleFont = font;
    }

}
 
void SDLRenderer::clearScreen() {

    if (renderer == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
    SDL_RenderClear(renderer);

}
 
void SDLRenderer::renderPresent()
{
    SDL_RenderPresent(renderer);
}
 
static void drawThickLine(SDL_Renderer *renderer,
                          int x1, int y1,
                          int x2, int y2,
                          int thickness)
{
    int half = thickness / 2;
    for (int dy = -half; dy <= half; dy++)
    {
        for (int dx = -half; dx <= half; dx++)
        {
            SDL_RenderDrawLine(renderer,
                               x1 + dx, y1 + dy,
                               x2 + dx, y2 + dy);
        }
    }
}
 
void SDLRenderer::showSelectMenu(SelectType selectType, int context) {

    if (selectType != SelectType::PLAYER_UI) {
    clearScreen();    
    SDL_Rect fullScreenRect = {0, 0, screenWidth, screenHeight};
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &fullScreenRect);
    }
 
    std::string prompt = "";
 
    switch (selectType) {   
    case SelectType::TITLE_UI:
        prompt = "TIC TAC TOE";
        break;
 
    case SelectType::SIZE_UI:
        prompt = "Enter Board Size";
        break;
 
    case SelectType::GOAL_UI:
        prompt = "Enter Goal";
        break;
 
    case SelectType::GAME_MODE_UI:
        prompt = "Enter Game Mode";
        break;
 
    case SelectType::BOT_LEVEL_UI:
        prompt = "Enter Bot Level";
        break;
    
    case SelectType::PLAYER_UI:
        prompt = "."; // placeholder
        break;
 
    case SelectType::MUL_BOT_LEVEL_UI:
        prompt = "Enter Bot " + std::to_string(context + 1) +
                 " level";
        break;
 
    default:
        break;
    }

    TTF_Font* renderFont = nullptr;
    if (selectType == SelectType::TITLE_UI && titleFont != nullptr) {
        renderFont = titleFont;
    } else {
        renderFont = font;
    }
 
    if (renderFont != nullptr)
    {
        SDL_Color shadowColor = {250, 250, 250, 255};
        SDL_Color textColor = {0, 0, 0, 200};

        SDL_Rect fullScreenRect = {0, 0, screenWidth, screenHeight};
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &fullScreenRect);
 
        std::string cleanPrompt = prompt;
        cleanPrompt.erase(std::remove(cleanPrompt.begin(), cleanPrompt.end(), '\n'), cleanPrompt.end());
 
        SDL_Surface *shadowSurface = TTF_RenderText_Blended_Wrapped(renderFont, cleanPrompt.c_str(), shadowColor, NULL);
        SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(renderFont, cleanPrompt.c_str(), textColor, NULL);
        if (shadowSurface != nullptr && textSurface != nullptr)
        {
            SDL_Texture *shadowTexture = SDL_CreateTextureFromSurface(renderer, shadowSurface);
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (shadowTexture != nullptr && textTexture != nullptr) {
                int textX = (screenWidth - textSurface->w) / 2;
                int textY = (screenHeight - textSurface->h) / 2; 
                SDL_Rect shadowQuad = {textX + 3, textY + 3, shadowSurface->w, shadowSurface->h};
                SDL_Rect renderQuad = {textX, textY, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, shadowTexture, NULL, &shadowQuad);
                SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
                SDL_DestroyTexture(shadowTexture);
                SDL_DestroyTexture(textTexture);
            } else {
                std::cerr << "[SDL_ttf ERROR] Could not create texture. Error: " << SDL_GetError() << std::endl;
            }
            SDL_FreeSurface(shadowSurface);
            SDL_FreeSurface(textSurface);
        }
        else
        {
            std::cerr << "[SDL_ttf ERROR] " << TTF_GetError() << std::endl;
            if (shadowSurface) SDL_FreeSurface(shadowSurface);
            if (textSurface) SDL_FreeSurface(textSurface);
        }
    }
    else
    {
        std::cerr << "[SDL_ttf ERROR] Font is nullptr " << std::endl;
    }

    if (selectType != SelectType::PLAYER_UI) {
        renderPresent();
    }

}
 
void SDLRenderer::showInvalidSelect(SelectType selectType, int context) {}
 
void SDLRenderer::showValidSelect(SelectType selectType, int context) {}
 
void SDLRenderer::displayBoard(const char board[][BOARD_N_MAX], const int size, bool showWinline) {
    currentBoardSize = size;
    SDL_Rect fullScreenRect = {0, 0, screenWidth, screenHeight};
    SDL_RenderCopy(renderer, boardTexture, NULL, &fullScreenRect);

    int boardArea = std::min(screenWidth, screenHeight);
    int cellSize = boardArea / size;
    int startX = (screenWidth - boardArea) / 2;  
    int startY = (screenHeight - boardArea) / 2; 
    int endX = startX + boardArea;
    int endY = startY + boardArea;
 
    const SDL_Color GRID_COLOR = {150, 150, 150, 255};
    const SDL_Color PLAYER_X_COLOR = {40, 40, 40, 255};
    const SDL_Color PLAYER_O_COLOR = {40, 40, 40, 255};
 
    SDL_SetRenderDrawColor(renderer, GRID_COLOR.r, GRID_COLOR.g, GRID_COLOR.b, GRID_COLOR.a);
 
    int gridThickness = std::max(6, cellSize / 20);
    
    for (int i = 1; i < size; i++) {
        int y = startY + i * cellSize;
        drawThickLine(renderer, startX, y, endX, y, gridThickness);
    }
 
    for (int j = 1; j < size; j++) {
        int x = startX + j * cellSize;
        drawThickLine(renderer, x, startY, x, endY, gridThickness);
    }
 
    for (int i = 0; i < size; i++) {

        for (int j = 0; j < size; j++) {

           int cellX = startX + j * cellSize;
           int cellY = startY + i * cellSize;

           if (board[i][j] == 'X') {
             int padding = cellSize / 5;
             int thickness = std::max(6, cellSize / 14);

             Sint16 x1 = cellX + padding;
             Sint16 y1 = cellY + padding;

             Sint16 x2 = cellX + cellSize - padding;
             Sint16 y2 = cellY + cellSize - padding;

            thickLineRGBA(renderer,
                        x1,
                        y1,
                        x2,
                        y2,
                        thickness * 2,
                        PLAYER_X_COLOR.r,
                        PLAYER_X_COLOR.g,
                        PLAYER_X_COLOR.b,
                        PLAYER_X_COLOR.a);

            thickLineRGBA(renderer,
                        x2,
                        y1,
                        x1,
                        y2,
                        thickness * 2,
                        PLAYER_X_COLOR.r,
                        PLAYER_X_COLOR.g,
                        PLAYER_X_COLOR.b,
                        PLAYER_X_COLOR.a);
            
            }
            else if (board[i][j] == 'O') {
                   int centerX = cellX + cellSize / 2;
                   int centerY = cellY + cellSize / 2;

                   int radius = cellSize / 3;
                   int thickness = std::max(5, cellSize / 10);

                   for (int r = radius; r > radius - thickness; --r) {
                      aacircleRGBA(renderer,
                            centerX,
                            centerY,
                            r,
                            PLAYER_O_COLOR.r,
                            PLAYER_O_COLOR.g,
                            PLAYER_O_COLOR.b,
                            PLAYER_O_COLOR.a);
                    }
                }
            }
        }
    
    if (showWinline) {
        renderPresent();
    }
}

void SDLRenderer::showMove(const int row, const int col) {}
 
void SDLRenderer::showInvalidMove() {}
 
void SDLRenderer::showPlayer(const int player, const bool is_bot) {}
 
void SDLRenderer::showResult(const int winner, const bool is_bot, const int size, const char board[][BOARD_N_MAX], const WinLine *winLine) {

    displayBoard(board, size, false);

    if (winLine && !winLine->cells.empty()) {

       int boardArea = std::min(screenWidth, screenHeight);
       int cellSize = boardArea / size;
       int thickness = std::max(6, cellSize / 18);
       const SDL_Color HIGH_LIGHT_COLOR = {40, 40, 40, 255};

       auto firstCell = winLine->cells.front();
       auto lastCell = winLine->cells.back();
       Sint16 x1 = firstCell.second * cellSize + cellSize / 2;
       Sint16 y1 = firstCell.first * cellSize + cellSize / 2;
       Sint16 x2 = lastCell.second * cellSize + cellSize / 2;
       Sint16 y2 = lastCell.first * cellSize + cellSize / 2;

       thickLineRGBA(renderer, 
                     x2, 
                     y2, 
                     x1, 
                     y1, 
                     thickness * 2, 
                     HIGH_LIGHT_COLOR.r, 
                     HIGH_LIGHT_COLOR.g, 
                     HIGH_LIGHT_COLOR.b, 
                     HIGH_LIGHT_COLOR.a);
    
       renderPresent();

    }

    SDL_Delay(1500);
    clearScreen();
    SDL_Rect fullScreenRect = {0, 0, screenWidth, screenHeight};
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &fullScreenRect);
    TTF_Font* renderFont = nullptr;
    renderFont = endgameFont;

    std::string prompt = "";

    switch (winner) {
        case -1:
           prompt = "Draw!";
           break;
        case 0:
           prompt = "Player 1 won!";
           break;
        case 1:
           prompt = "Player 2 won!";
           break;
        default:
           break;
    }

    if (renderFont != nullptr) {

        SDL_Color shadowColor = {250, 250, 250, 255};
        SDL_Color textColor = {0, 0, 0, 200};

        SDL_Rect fullScreenRect = {0, 0, screenWidth, screenHeight};
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &fullScreenRect);
 
        std::string cleanPrompt = prompt;
        cleanPrompt.erase(std::remove(cleanPrompt.begin(), cleanPrompt.end(), '\n'), cleanPrompt.end());
 
        SDL_Surface *shadowSurface = TTF_RenderText_Blended_Wrapped(renderFont, cleanPrompt.c_str(), shadowColor, NULL);
        SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(renderFont, cleanPrompt.c_str(), textColor, NULL);
        if (shadowSurface != nullptr && textSurface != nullptr)
        {
            SDL_Texture *shadowTexture = SDL_CreateTextureFromSurface(renderer, shadowSurface);
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (shadowTexture != nullptr && textTexture != nullptr) {
                int textX = (screenWidth - textSurface->w) / 2;
                int textY = (screenHeight - textSurface->h) / 2; 
                SDL_Rect shadowQuad = {textX + 3, textY + 3, shadowSurface->w, shadowSurface->h};
                SDL_Rect renderQuad = {textX, textY, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, shadowTexture, NULL, &shadowQuad);
                SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
                SDL_DestroyTexture(shadowTexture);
                SDL_DestroyTexture(textTexture);
            } else {
                std::cerr << "[SDL_ttf ERROR] Could not create texture. Error: " << SDL_GetError() << std::endl;
            }
            SDL_FreeSurface(shadowSurface);
            SDL_FreeSurface(textSurface);
        }
        else
        {
            std::cerr << "[SDL_ttf ERROR] " << TTF_GetError() << std::endl;
            if (shadowSurface) SDL_FreeSurface(shadowSurface);
            if (textSurface) SDL_FreeSurface(textSurface);
        }
    }
    else
    {
        std::cerr << "[SDL_ttf ERROR] Font is nullptr " << std::endl;
    }

    renderPresent();
                
}
 
void SDLRenderer::printResult(const GameResult &gameResult) {}
 
void SDLRenderer::close() {

    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    if (titleFont) {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
 
    TTF_Quit();
    IMG_Quit();
 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}