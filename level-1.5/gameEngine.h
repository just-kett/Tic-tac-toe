#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "gameConfig.h"

void startGame(const RunConfig& config, GameSetup& gameSetup);
GameResult playGame(const RunConfig& config, GameSetup& gameSetup);
void endGame(const RunConfig& config, GameSetup& gameSetup, GameResult& gameResult);

#endif 
