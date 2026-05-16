/**
 * Ternimal Renderer cpp implementation
 *
 */

#include "renderer.h"

/* ---------- Importing ---------- */

#include <format>
#include <iostream>

#include "../game/setup.h"
#include "../utils/config.h"

/* ---------- Definitions ---------- */

TerminalRenderer::TerminalRenderer() : I_Renderer() {}

TerminalRenderer::~TerminalRenderer() {}

void TerminalRenderer::init(const RunConfig& config) {}

void TerminalRenderer::clearScreen() {
    std::cout << "\n"
              << "\x1B[2J\x1B[H"
              << "\n";
    return;
}

void TerminalRenderer::showSelectMenu(SelectType selectType, int context) {
    switch (selectType) {
        case SelectType::TITLE_UI:
            std::cout << std::format(">----- Tic-tac-toe [Console v{}] -----<\n", VERSION);
            break;

        case SelectType::SIZE_UI:
            std::cout << "Size Input (NxN, " << BOARD_N_MIN << " <= N <= " << BOARD_N_MAX << ")" << std::endl;  
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
}

void TerminalRenderer::showInvalidSelect(SelectType selectType, int context) {
    switch (selectType) {
        case SelectType::SIZE_UI:
            std::cout << "Invalid size. Please enter a number between " << BOARD_N_MIN << " and " << BOARD_N_MAX << "." << std::endl;
            break;
        case SelectType::GOAL_UI:
            std::cout << "Invalid goal. Please enter a number between " << BOARD_N_MIN << " and " << context << "." << std::endl;
            break;
        case SelectType::GAME_MODE_UI:
            std::cout << "Invalid game mode. Choose 1, 2 or 3." << std::endl;
            break;
        case SelectType::BOT_LEVEL_UI:
        case SelectType::MUL_BOT_LEVEL_UI:
            std::cout << "Invalid bot level. Choose 1, 2 or 3." << std::endl;
            break;
        case SelectType::PLAYER_UI:
            std::cout << "Invalid move. Please enter valid row and column." << std::endl;
            break;
        default:
            std::cout << "Invalid selection." << std::endl;
            break;
    }
}

void TerminalRenderer::showValidSelect(SelectType selectType, int context) {
    switch (selectType) {
        case SelectType::SIZE_UI:
            std::cout << "Selected board size: " << context << std::endl;
            break;
        case SelectType::GOAL_UI:
            std::cout << "Selected goal: " << context << std::endl;
            break;
        case SelectType::GAME_MODE_UI:
            std::cout << "Selected game mode: " << modeToString(context) << std::endl;
            break;
        case SelectType::BOT_LEVEL_UI:
        case SelectType::MUL_BOT_LEVEL_UI:
            std::cout << "Selected bot level: " << botToString(context) << std::endl;
            break;
        default:
            break;
    }
}

void TerminalRenderer::displayBoard(const char board[][BOARD_N_MAX], const int size) {
    std::cout << "    ";
    for (int c = 0; c < size; c++) {
        std::cout << c;
        if (c < size - 1) std::cout << " ";
    }
    std::cout << std::endl;

    std::cout << "    ";
    for (int i = 0; i < 2 * size - 1; i++) {
        std::cout << "#";
    }
    std::cout << std::endl;

    for (int r = 0; r < size; r++) {
        if (r >= 10) {
            std::cout << r << " #";
        } else {
            std::cout << r << "  #";
        }
        for (int c = 0; c < size; c++) {
            std::cout << board[r][c];
            if (c < size - 1) std::cout << " ";
        }
        std::cout << "#";
        std::cout << std::endl;
    }
    std::cout << "    ";
    for (int i = 0; i < 2 * size - 1; i++) {
        std::cout << "#";
    }
    std::cout << std::endl;
}

void TerminalRenderer::showPlayer(int player, bool is_bot) {
    if (is_bot) {
        std::cout << "Bot " << (player + 1) << " is thinking..." << std::endl;
        return;
    }
    std::cout << "Player " << (player + 1) << " is thinking..." << std::endl;
}

void TerminalRenderer::showMove(const int row, const int col) {
    std::cout << "Move placed at (" << row << ", " << col << ")" << std::endl;
}

void TerminalRenderer::showInvalidMove() {
    std::cout << "Invalid move!" << std::endl;
}

void TerminalRenderer::showResult(const int winner, const bool is_bot, const WinLine* winLine) {
    if (winner == -1) {
        std::cout << "Draw!" << std::endl;
        return;
    }
    if (is_bot) {
        std::cout << "Bot " << (winner + 1) << " wins!" << std::endl;
    } else {
        std::cout << "Player " << (winner + 1) << " wins!" << std::endl;
    }
}

void TerminalRenderer::printResult(const GameResult& gameResult) {
    std::cout << gameResult.winner << " " << gameResult.turns;
}

void TerminalRenderer::close() {}