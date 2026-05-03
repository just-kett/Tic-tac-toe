#include "gameRenderer.h"
#include "gameConfig.h"

#include <iostream>
#include <format>

void clearScreen() {
    std::cout << "\n"
              << "\x1B[2J\x1B[H"
              << "\n";
    return;
}

void showSelectMenu(SelectType selectType) {
    switch (selectType) {
        case SelectType::TITLE_UI:
            std::cout << std::format(">----- Tic-tac-toe [Console v{}] -----<\n\n", VERSION);
            break;

        case SelectType::SIZE_UI:
            std::cout << "Size Input (NxN, " << 3 << " <= N <= " << BOARD_N_MAX << ")" << std::endl;  
            break;

        case SelectType::GOAL_UI:
            std::cout<< "Goal Input (3 - 5, goal <= size)" << std::endl;
            break;

        case SelectType::GAME_MODE_UI:
            std::cout << "(1) PvP | (2) PvE | (3) EvE" << std::endl;
            break;

        case SelectType::BOT_LEVEL_UI:
            std::cout << "(1) EASY | (2) MEDIUM | (3) HARD" << std::endl;
            break;

        case SelectType::PLAYER_UI:
            std::cout << "Enter Move (row, col): " << std::endl;
            break;

        case SelectType::MUL_BOT_LEVEL_UI:
            std::cout << "Select difficult for bots:\n(1) EASY | (2) MEDIUM | (3) HARD" <<std::endl;
            break;

        default:
            break;
    }

    return;
}

void displayBoard(const char board[][BOARD_N_MAX], const int size) {
    std::cout << "    ";
    for(int c = 0; c < size; c++) {
        std::cout << c;
        if(c < size - 1) std::cout << " ";
    }
    std::cout << std::endl;
    
    std::cout << "    ";
    for(int i = 0; i < 2 * size - 1; i++) {
        std::cout << "#";
    }
    std::cout << std::endl;
    
    for(int r = 0; r < size; r++) { 
        if (r >= 10) {
            std::cout << r << " #";
        }
        else {
            std::cout << r << "  #";
        }
        for(int c = 0; c < size; c++) {
            std::cout << board[r][c];
            if(c < size - 1) std::cout << " ";
        }
        std::cout << "#";
        std::cout << std::endl;
    }
    std::cout << "    ";
    for(int i = 0; i < 2 * size - 1; i++) {
        std::cout << "#";
    }
    std::cout << std::endl;
}

void showPlayer(int player, bool is_bot) {
    if (is_bot) {
        std::cout << "Bot " << player << " is thinking..." << std::endl;
        return;
    }

    switch (player) {
        case 1:
            std::cout << "Player 1 is thinking..." << std::endl;
            break;
        case 2:
            std::cout << "Player 2 is thinking..." << std::endl;
            break;
    }
}

void showMove(const int row, const int col) {
    std::cout << "Move placed at (" << row << ", " << col << ")" << std::endl;
}

void showInvalidMove() {
    std::cout << "Invalid Move!" << std::endl;
}

void showResult(const int winner, const bool is_bot) {
    if (winner == -1) {
        std::cout << "Draw!" << std::endl;
        return;
    }
    // Simplified version, assuming PvP for now
    if (is_bot) {
        std::cout << "Bot wins!" << std::endl;
    } else {
        std::cout << "Player " << (winner + 1) << " wins!" << std::endl;
    }
}

void printResult(const GameResult& gameResult) {
    std::cout << gameResult.winner << " ";
    std::cout << gameResult.moveCount;
}
