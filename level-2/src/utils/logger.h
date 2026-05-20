/**
 * Logger header file
 *
 */

#pragma once

/* ---------- Importing ---------- */

#include <fstream>
#include <string>

/* ---------- Declarations ---------- */

namespace Logger {

enum class Level {
    DEBUG,    // log chi tiết (debug)
    INFO,     // thông tin chung
    WARNING,  // cảnh báo
    ERROR,    // lỗi nghiêm trọng
    MSG,      // message thuần (không có prefix level)
};

inline std::string levelToString(Level level) {
    switch (level) {
        case Level::DEBUG:
            return "DEBUG";
        case Level::INFO:
            return "INFO";
        case Level::WARNING:
            return "WARN";
        case Level::ERROR:
            return "ERROR";
        case Level::MSG:
            return "";  // không hiển thị label
        default:
            return "UNKNOWN";
    }
}

inline static Level min_level = Level::INFO;

// ---------- Level Color (ANSI escape codes) ----------

const std::string RESET = "\033[0m";    // reset màu
const std::string RED = "\033[31m";     // màu đỏ (error)
const std::string GREEN = "\033[32m";   // màu xanh lá (info)
const std::string YELLOW = "\033[33m";  // màu vàng (warning)
const std::string BLUE = "\033[34m";    // màu xanh dương (debug)
const std::string CYAN = "\033[36m";    // màu cyan (ít dùng)

inline std::string getColor(Level level) {
    switch (level) {
        case Level::DEBUG:
            return BLUE;
        case Level::INFO:
            return GREEN;
        case Level::WARNING:
            return YELLOW;
        case Level::ERROR:
            return RED;
        default:
            return RESET;
    }
}

// ---------- Static Variables ----------

// file stream để ghi log ra file
inline static std::ofstream log_file;

// bật/tắt ghi log ra file
inline static bool write_to_file = false;

// xác định có đang ở judge mode hay không (ảnh hưởng format/log behavior)
inline static bool is_judge_mode = false;

// ---------- Functions ----------

void init(bool judge_mode, bool to_file = true, const std::string& path = "log.txt", bool verbose_flag = false);

void log(const std::string& msg, Level level = Level::INFO);

void close();

}  