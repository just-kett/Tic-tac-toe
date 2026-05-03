#ifndef GAME_LOGGER_H
#define GAME_LOGGER_H

#include <string>

namespace GameLogger {

// Logging levels
enum class Level {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    MSG
};

// Functions
void init(bool judge_mode, bool to_file = true, const std::string& path = "log.txt");
void log(const std::string& msg, Level level = Level::INFO);
void close();

}  

#endif 