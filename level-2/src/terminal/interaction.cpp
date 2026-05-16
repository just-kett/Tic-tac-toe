/**
 * Terminal Interaction cpp implementation
 *
 */

#include "interaction.h"

/* ---------- Importing ---------- */

#include <chrono>
#include <format>
#include <iostream>
#include <limits>
#include <thread>

#include "../utils/logger.h"

/* ---------- Definitions ---------- */

TerminalInteraction::TerminalInteraction() {}

TerminalInteraction::~TerminalInteraction() {}

void TerminalInteraction::init(const RunConfig& config) {
    initInteraction(config);
}

void TerminalInteraction::initInteraction(const RunConfig& config) {
    cin_backup = nullptr;

    if (!config.interactive && !config.input_file.empty()) {
        global_file_in.open(config.input_file);
        if (global_file_in.is_open()) {
            cin_backup = std::cin.rdbuf();
            std::cin.rdbuf(global_file_in.rdbuf());

            Logger::log(std::format("redirected cin to: {}", config.input_file));
        } else {
            Logger::log("failed to open input file, using console.", Logger::Level::ERROR);
        }
    }
}

void TerminalInteraction::closeInteraction() {
    if (cin_backup) {
        std::cin.rdbuf(cin_backup);
        Logger::log("fallback using 'std::cin' input stream.");
    }

    if (global_file_in.is_open()) {
        global_file_in.close();
    }
}

bool TerminalInteraction::validateInput(const std::string& input) {
    for (char i : input) {
        if (not std::isdigit(i)) {
            return false;
        }
    }
    return true;
}

bool TerminalInteraction::getInput(int* val) {
    std::string input;

    if (!(std::cin >> input)) {
        return false;
    }
    if (input.empty() || !validateInput(input)) {
        return false;
    }
    try {
        *val = std::stoi(input);
    }
    catch (const std::out_of_range&) {
        return false;
    }
    catch (const std::invalid_argument&) {
        return false;
    }
    return true;
}

void TerminalInteraction::pause(int timeout) {
    if (timeout > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
        return;
    }
    std::string line;
    if (std::getline(std::cin, line) && line.empty()) {
    } else if (!std::cin.good() && !std::cin.eof()) {
        std::cin.clear();
        std::getline(std::cin, line);
    }
}

bool TerminalInteraction::selectSize(int* size) {
    if (!getInput(size)) {
        return false;
    }
    if (*size < BOARD_N_MIN || *size > BOARD_N_MAX) {
        return false;
    }
    return true;
}

bool TerminalInteraction::selectGoal(int* goal, const int size) {
    if (!getInput(goal)) {
        return false;
    }
    if (*goal < BOARD_N_MIN || *goal > size) {
        return false;
    }
    return true;
}

bool TerminalInteraction::selectGameMode(GameMode* mode) {
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

bool TerminalInteraction::selectBotLevel(BotLevel* levels, const int index) {
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

bool TerminalInteraction::getPlayerMove(int* row, int* col) {
    if (!getInput(row)) {
        return false;
    }
    if (!getInput(col)) {
        return false;
    }
    return true;
}

void TerminalInteraction::close() {
    closeInteraction();
}