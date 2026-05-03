#include "gameLogger.h"
#include "gameConfig.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string>

// Level Color
const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string CYAN = "\033[36m";

// Static Variables
static std::ofstream log_file;
static bool write_to_file = false;
static bool is_judge_mode = false;
static GameLogger::Level min_level = GameLogger::Level::DEBUG;

/**
 * Convert logging level to printable string.
 */
inline std::string levelToString(GameLogger::Level level) {
    switch (level) {
        case GameLogger::Level::DEBUG:
            return "DEBUG";
        case GameLogger::Level::INFO:
            return "INFO";
        case GameLogger::Level::WARNING:
            return "WARN";
        case GameLogger::Level::ERROR:
            return "ERROR";
        case GameLogger::Level::MSG:
            return "";
        default:
            return "UNKNOWN";
    }
}

/**
 * Return terminal color code corresponding to a log level.
 */
inline std::string getColor(GameLogger::Level level) {
    switch (level) {
        case GameLogger::Level::DEBUG:
            return BLUE;
        case GameLogger::Level::INFO:
            return GREEN;
        case GameLogger::Level::WARNING:
            return YELLOW;
        case GameLogger::Level::ERROR:
            return RED;
        default:
            return RESET;
    }
}

void GameLogger::init(bool judge_mode, bool to_file, const std::string& path) {
    write_to_file = to_file;
    is_judge_mode = judge_mode;

    if (write_to_file) {
        // write new log contents
        log_file.open(path, std::ios::out | std::ios::trunc);
        if (!log_file.is_open()) {
            std::cerr << "[Logger] Cannot open log file: " << path
                      << ". Falling back to console only." << std::endl;
            write_to_file = false;  // fallback
        }
    }

    std::string header = "Tic-tac-toe Game (Version: " + std::string(VERSION) + ")\n";
    header += std::string(48, '-');

    if (write_to_file) {
        log_file << header << std::endl;
    }

    if (!is_judge_mode) {
        std::cout << header << std::endl;
    }
}

void GameLogger::log(const std::string& msg, Level level) {
    // Only log if current level is important as or more than min_level
    if (static_cast<int>(level) < static_cast<int>(min_level)) {
        return;
    }

    auto now = std::time(nullptr);

    // NOTE: std::localtime is not thread-safe but acceptable here
    // since the program is single-threaded.
    auto tm = *std::localtime(&now);

    // format: '[Level] - [YYYY-MM-DD HH:MM:SS] Message'
    std::stringstream ss_lv;
    std::string formatted_lv;
    if (level != Level::MSG) {
        ss_lv << "[" << levelToString(level) << "]";
        formatted_lv = ss_lv.str();
    }

    std::stringstream ss_msg;
    ss_msg << (formatted_lv.empty() ? "" : " - ")
           << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] ";
    ss_msg << msg;
    std::string formatted_msg = ss_msg.str();

    // to output stream
    if (write_to_file) {
        log_file << formatted_lv << formatted_msg << std::endl;
        log_file.flush();
    }

    if (!is_judge_mode) {
        std::cout << getColor(level) << formatted_lv;
        std::cout << getColor(Level::MSG) << formatted_msg << RESET << std::endl;
    }
}

void GameLogger::close() {
    if (log_file.is_open()) {
        log_file.close();
    }
}
