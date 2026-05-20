/**
 * Config cpp implementation
 *
 */

#include "config.h"

/* ---------- Importing ---------- */

#include <fstream>
#include <iomanip>
#include <sstream>

/* ---------- Definitions ---------- */

RunConfig parseArgs(int argc, char* argv[]) {
    RunConfig config;

    // duyệt qua từng argument (bỏ argv[0] là tên chương trình)
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // bật judge mode (tắt interactive)
        if (arg == "-j" || arg == "--judge") {
            config.judge_mode = true;
            config.interactive = false;

            // chỉ định file input
        } else if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
            config.input_file = argv[++i];  // lấy tham số tiếp theo

            // chỉ định file log
        } else if ((arg == "-l" || arg == "--log")) {
            if (i + 1 < argc) {
                config.log_file = argv[++i];  // lấy tên file
            } else {
                config.log_file = "";  // không có tham số -> disable log file
            }

            // nếu log_file rỗng thì không ghi ra file
            if (config.log_file == "") {
                config.to_file = false;
            }

            // bật verbose (log level DEBUG)
        } else if (arg == "-v" || arg == "--verbose") {
            config.verbose_flag = true;

            // bật GUI (SDL)
        } else if (arg == "-g" || arg == "--gui_flag") {
            config.gui_flag = true;

            // hiển thị help
        } else if (arg == "-h" || arg == "--help") {
            config.is_help = true;
        }
    }

    return config;
}

std::ostream& operator<<(std::ostream& os, const RunConfig& config) {
    os << std::boolalpha;  // in bool dưới dạng true/false
    os << "RunConfig {\n"
       << "  interactive:  " << config.interactive << "\n"
       << "  judge_mode:   " << config.judge_mode << "\n"
       << "  input_file:   \"" << config.input_file << "\"\n"
       << "  to_file:      " << config.to_file << "\n"
       << "  log_file:     \"" << config.log_file << "\"\n"
       << "  verbose_flag: " << config.verbose_flag << "\n"
       << "  gui_flag:     " << config.gui_flag << "\n"
       << "  is_help:      " << config.is_help << "\n"
       << "}";
    return os;
}

std::string RunConfig::toString() const {
    std::stringstream ss;
    ss << std::boolalpha;  // in bool dưới dạng true/false
    ss << "RunConfig {\n"
       << "  interactive:  " << interactive << "\n"
       << "  judge_mode:   " << judge_mode << "\n"
       << "  input_file:   \"" << input_file << "\"\n"
       << "  to_file:      " << to_file << "\n"
       << "  log_file:     \"" << log_file << "\"\n"
       << "  verbose_flag: " << verbose_flag << "\n"
       << "  gui_flag:     " << gui_flag << "\n"
       << "  is_help:      " << is_help << "\n"
       << "}";
    return ss.str();
}

std::string configHelpStr() {
    std::stringstream ss;

    // tiêu đề chương trình + version
    ss << "Tic-tac-toe Game (Version: " << VERSION << " by baluong.87)\n";

    // danh sách các option hỗ trợ
    ss << "Usage: \n"
       << "\t" << "--judge, -j" << "\t" << "Enable judge mode (no UI, standard output only)\n"
       << "\t" << "--input, -i" << "\t" << "Path to input file\n"
       << "\t" << "--log, -l" << "\t" << "Path to log file (default: log.txt)\n"
       << "\t" << "--verbose, -v" << "\t" << "Enable verbose flag (logging level to DEBUG)\n"
       << "\t" << "--gui_flag, -g" << "\t" << "Enable GUI flag (play game with GUI)\n";

    return ss.str();
}