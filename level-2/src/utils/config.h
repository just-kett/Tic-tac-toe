/**
 * Config header file
 *
 */

#pragma once

/* ---------- Importing ---------- */

#include <string>

/* ---------- Global Variables ---------- */

// version của chương trình
// TODO: sửa <STUDENT_ID> bằng mã số sinh viên
inline const std::string VERSION = "0.4.<25023041>";

/* ---------- Type Definitions ---------- */

struct RunConfig {
    // ---------- Core ----------

    bool interactive = true;  // bật/tắt chế độ tương tác (UI + input từ người dùng)
    bool judge_mode = false;  // chế độ chấm (không interactive, dùng để test/auto-run)
    std::string input_file;   // file input (dùng trong judge mode)

    bool to_file = true;               // ghi log ra file hay không
    std::string log_file = "log.txt";  // tên file log

    bool verbose_flag = false;  // bật log chi tiết (DEBUG)
    bool gui_flag = false;      // bật giao diện GUI (SDL) thay vì terminal

    bool is_help = false;  // nếu true thì chỉ hiển thị help và thoát

    // ---------- SDL Layout ----------

    int screenWidth = 800;   // chiều rộng cửa sổ SDL
    int screenHeight = 800;  // chiều cao cửa sổ SDL
    int boardPadding = 100;  // khoảng cách padding của board trong màn hình

    // ---------- Helper ----------

    friend std::ostream& operator<<(std::ostream& os, const RunConfig& config);

    std::string toString() const;
};

/* ---------- Declarations ---------- */

RunConfig parseArgs(int argc, char* argv[]);

std::string configHelpStr();