#include "gameInteraction.h"
#include "gameLogger.h"
#include "gameConfig.h"

#include <iostream>
#include <string>
#include <format>
#include <cctype>

// Global file input
std::ifstream GameInteraction::global_file_in;

// Given
std::streambuf* initInteraction(const RunConfig& config) {
    std::streambuf* cin_backup = nullptr;

    if (!config.interactive && !config.input_file.empty()) {
        GameInteraction::global_file_in.open(config.input_file);
        if (GameInteraction::global_file_in.is_open()) {
            cin_backup = std::cin.rdbuf();
            std::cin.rdbuf(GameInteraction::global_file_in.rdbuf());

            GameLogger::log(std::format("redirected cin to: {}", config.input_file));
        } else {
            GameLogger::log("failed to open input file, using console.", GameLogger::Level::ERROR);
        }
    }
    return cin_backup;
}

// Given
void closeInteraction(std::streambuf* cin_backup) {
    if (cin_backup) {
        std::cin.rdbuf(cin_backup);
        GameLogger::log("fallback using 'std::cin' input stream.");
    }

    if (GameInteraction::global_file_in.is_open()) {
        GameInteraction::global_file_in.close();
    }
}

bool validateInput(std::string& input) {
    for (char i : input) {
        if (not std::isdigit(i)) {
            return false;
        }
    }
    return true;
}

bool getInput(int* val) {
    std::string input;

    if (!(std::cin >> input)) {
        return false; 
    }

    if (input.empty() || !validateInput(input)) {
        return false;
    }

    try {
        *val = std::stoi(input);
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }

    return true;
}

bool selectSize(int* size) {
    if (!getInput(size)) {
        return false;
    }

    if (*size < 3 || *size > BOARD_N_MAX) {
        return false;
    }
    return true;
}

bool selectGoal(int* goal, const int size) {
    if (!getInput(goal)) {
        return false;
    }
    
    if (*goal < 3 || *goal > size) {
        return false;
    }
    return true;
}

bool selectGameMode(GameMode* mode) {
    int choice;
    if (!getInput(&choice)) {
        return false;
    }

    switch (choice) {
        case 1:
            *mode = GameMode::PVP;
            return true;
        case 2:
            *mode = GameMode::PVE;
            return true;
        case 3:
            *mode = GameMode::EVE;
            return true;
        default:
            *mode = GameMode::INVALID_MODE;
            return false;
    }
}

bool selectBotLevel(BotLevel* levels, const int index) {
    int choice;
    if (!getInput(&choice)) {
        return false;
    }

    switch (choice) {
        case 1:
            levels[index] = BotLevel::EASY;
            return true;
        case 2:
            levels[index] = BotLevel::MEDIUM;
            return true;
        case 3:
            levels[index] = BotLevel::HARD;
            return true;
        default:
            return false;
    }
}

bool getPlayerMove(int* row, int* col) {
    if (!getInput(row)) {
        return false;
    }
    if (!getInput(col)) {
        return false;
    }
    return true;
}
