/**
 * Logger cpp implementation
 *
 */

#include "logger.h"

/* ---------- Importing ---------- */

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "config.h"

/* ---------- Definitions ---------- */

void Logger::init(bool judge_mode, bool to_file, const std::string& path, bool verbose_flag) {
    write_to_file = to_file;
    is_judge_mode = judge_mode;

    if (write_to_file) {
        // mở file log với chế độ ghi mới (xóa nội dung cũ)
        log_file.open(path, std::ios::out | std::ios::trunc);

        if (!log_file.is_open()) {
            // fallback nếu không mở được file
            std::cerr << "[Logger] Cannot open log file: " << path
                      << ". Falling back to console only." << std::endl;
            write_to_file = false;
        }
    }

    // tạo header log
    std::string header = "Tic-tac-toe Game (Version: " + std::string(VERSION) + " by baluong.87)\n";
    header += std::string(48, '-');

    // ghi header ra file hoặc console
    if (write_to_file) {
        log_file << header << std::endl;
    } else if (!is_judge_mode) {
        std::cout << header << std::endl;
    }

    // thiết lập level mặc định
    min_level = Level::INFO;

    // nếu verbose → cho phép DEBUG
    if (verbose_flag) {
        min_level = Level::DEBUG;
    }
}

void Logger::log(const std::string& msg, Level level) {
    // lọc log theo level
    if (static_cast<int>(level) < static_cast<int>(min_level)) {
        return;
    }

    // lấy thời gian hiện tại
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);

    // format phần level
    std::stringstream ss_lv;
    std::string formatted_lv;
    if (level != Level::MSG) {
        ss_lv << "[" << levelToString(level) << "]";
        formatted_lv = ss_lv.str();
    }

    // format message + timestamp
    std::stringstream ss_msg;
    ss_msg << (formatted_lv.empty() ? "" : " - ")
           << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] ";
    ss_msg << msg;

    std::string formatted_msg = ss_msg.str();

    // output ra file hoặc console
    if (write_to_file) {
        log_file << formatted_lv << formatted_msg << std::endl;
        log_file.flush();  // đảm bảo ghi ngay lập tức
    } else if (!is_judge_mode) {
        // console với màu theo level
        std::cout << getColor(level) << formatted_lv;
        std::cout << getColor(Level::MSG) << formatted_msg << RESET << std::endl;
    }
}

void Logger::close() {
    if (log_file.is_open()) {
        log_file.close();
    }
}