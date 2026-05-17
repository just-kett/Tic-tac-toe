/**
 * SDL Interaction cpp implementation
 *
 */

#include "interaction.h"
#include "renderer.h"
#include "../game/setup.h"

/* ---------- Importing ---------- */

#include <format>
#include <iostream>

/* ---------- Definitions ---------- */

bool SDLInteraction::waitForQuit(SDL_Event& e) {

    if (e.type == SDL_QUIT) {
        throw QuitException();
    }
    return false;

}

SDLInteraction::SDLInteraction() {}

SDLInteraction::~SDLInteraction() {}

void SDLInteraction::init(const RunConfig& config) {

    screenWidth = config.screenWidth;
    screenHeight = config.screenHeight;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {}

}

void SDLInteraction::pause(int timeout) {

    if (timeout > 0) {
        SDL_Delay(timeout);
        return;
    }

    bool waiting = true;
    SDL_Event event;

    while (waiting) {

        waitForQuit(event);

        if (event.type == SDLK_ESCAPE) {
            throw QuitException();
        }
        
        if (event.type == SDL_KEYDOWN || SDL_MOUSEBUTTONDOWN) {
            waiting = false;
        }
    }

    SDL_Delay(3000);

}

int SDLInteraction::getInput() {

    std::string buffer;
    SDL_Event event;

    while (true) {

        while (SDL_PollEvent(&event)) {

            waitForQuit(event);

            if (event.type != SDL_KEYDOWN) {
                continue;
            }

            SDL_Keycode key = event.key.keysym.sym;

            if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {

                if (buffer.empty()) {
                    return -1;
                }

                try {
                    return std::stoi(buffer);
                }
                catch (...) {
                    return -1;
                }
            }
            else if (key == SDLK_BACKSPACE) {
                
                if (!buffer.empty()) {
                    buffer.pop_back();
                }
            }
            else if (key >= SDLK_0 && key <= SDLK_9) {
                buffer += static_cast<char>('0' + (key - SDLK_0));
            }
            else if  (key >= SDLK_KP_0 && key <= SDLK_KP_9) {
                buffer += static_cast<char>('0' + (key - SDLK_KP_0));
            }
            else if (key == SDLK_ESCAPE) {
                throw QuitException();
            }
        }
    }
    SDL_Delay(1000);
}


bool SDLInteraction::selectSize(int* size) {

    int val = getInput();
    if (val < BOARD_N_MIN || val > BOARD_N_MAX) {
        return false;
    }
    *size = val;
    boardsize = val;
    SDL_Delay(1000);
    return true;

}

bool SDLInteraction::selectGoal(int* goal, const int size) {

    int val = getInput();
    int maxGoal = std::min(size, GOAL_MAX);
    if (val < 3 || val > maxGoal) {
        return false;
    }
    *goal = val;
    SDL_Delay(1000);
    return true;

}

bool SDLInteraction::selectGameMode(GameMode* mode) {

    SDL_Event event;
    while (true) {
        if (!SDL_WaitEvent(&event)) {
            continue;
        }
        waitForQuit(event);
        if (event.type != SDL_KEYDOWN) {
            continue;
        }
        SDL_Keycode choice = event.key.keysym.sym;
        switch(choice) {
            case SDLK_1: case SDLK_KP_1: 
                *mode = GameMode::PVP;
                SDL_Delay(1000);
                return true;
            case SDLK_KP_2: case SDLK_2:
                *mode = GameMode::PVE;
                SDL_Delay(1000);
                return true; 
            case SDLK_KP_3: case SDLK_3:
                *mode = GameMode::EVE;
                SDL_Delay(1000);
                return true;
            case SDLK_ESCAPE:
                throw QuitException();
            default:
                return false;
        }
    }

}

bool SDLInteraction::selectBotLevel(BotLevel* levels, const int index) {
 
    SDL_Event event;
    
    while (true) {

        if (!SDL_WaitEvent(&event)) {
            continue;
        }
        waitForQuit(event);
        if (event.type != SDL_KEYDOWN) {
            continue;
        }

        SDL_Keycode choice = event.key.keysym.sym;
        switch (choice) {
            case SDLK_1: case SDLK_KP_1:
                levels[index] = BotLevel::EASY;
                SDL_Delay(1000);
                return true;
            case SDLK_2: case SDLK_KP_2:
                levels[index] = BotLevel::MEDIUM;
                SDL_Delay(1000);
                return true;
            case SDLK_3: case SDLK_KP_3:
                levels[index] = BotLevel::HARD;
                SDL_Delay(1000);
                return true;
            case SDLK_ESCAPE:
                throw QuitException();
            default:
                return false;
        }
    }

}

bool SDLInteraction::getPlayerMove(int* row, int* col) {
    SDL_Event event;

    while (true) {
        
        while (SDL_PollEvent(&event)) {
            waitForQuit(event);

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;

                int boardArea = std::min(screenWidth, screenHeight);
                int cellSize = boardArea / std::max(1, boardsize);
                int startX = (screenWidth - boardArea) / 2;
                int startY = (screenHeight - boardArea) / 2;

                if (x < startX || x >= startX + boardArea || y < startY || y >= startY + boardArea) {
                    return false; 
                }

                int clickedCol = (x - startX) / cellSize;
                int clickedRow = (y - startY) / cellSize;

                *row = clickedRow;
                *col = clickedCol;
                return true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    throw QuitException();
                }
            }
        }
    }

    SDL_Delay(1000);

}

void SDLInteraction::close() {}