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

/**
 * Mô tả:
 *   Constructor của TerminalRenderer.
 *
 * TODO:
 *   - Khởi tạo trạng thái ban đầu nếu cần
 */
TerminalRenderer::TerminalRenderer() : I_Renderer() {
}

/**
 * Mô tả:
 *   Destructor của TerminalRenderer.
 *
 * TODO:
 *   - Giải phóng tài nguyên nếu có
 */
TerminalRenderer::~TerminalRenderer() {
    // TODO: cleanup nếu cần
}

/**
 * Mô tả:
 *   Khởi tạo renderer với config.
 *
 * TODO:
 *   - Đọc config
 *   - Thiết lập môi trường hiển thị
 */
void TerminalRenderer::init(const RunConfig& config) {
    
}

/**
 * Mô tả:
 *   Xóa màn hình terminal.
 *
 * TODO:
 *   - Sử dụng ANSI escape sequence để clear screen
 */
void TerminalRenderer::clearScreen() {
    std::cout << "\n"
              << "\x1B[2J\x1B[H"
              << "\n";
    return;
}

/**
 * Mô tả:
 *   Hiển thị menu lựa chọn.
 *
 * TODO:
 *   - Bước 1: kiểm tra selectType
 *   - Bước 2: in menu tương ứng
 *   - Trường hợp biên: selectType không hợp lệ
 */
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

/**
 * Mô tả:
 *   Hiển thị thông báo lựa chọn không hợp lệ.
 *
 * TODO:
 *   - Bước 1: xác định loại lỗi
 *   - Bước 2: in thông báo tương ứng
 */
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

/**
 * Mô tả:
 *   Hiển thị thông báo lựa chọn hợp lệ.
 *
 * TODO:
 *   - Bước 1: xác định loại select
 *   - Bước 2: in thông báo xác nhận
 */
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

/**
 * Mô tả:
 *   Hiển thị bàn cờ.
 *
 * TODO:
 *   - Bước 1: in header
 *   - Bước 2: in trục tọa độ
 *   - Bước 3: duyệt board và in từng ô
 */
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

/**
 * Mô tả:
 *   Hiển thị thông tin player hiện tại.
 *
 * TODO:
 *   - In player index
 *   - Nếu là bot thì thêm label "(Bot)"
 */
void TerminalRenderer::showPlayer(int player, bool is_bot) {
    if (is_bot) {
        std::cout << "Bot " << (player + 1) << " is thinking..." << std::endl;
        return;
    }
    std::cout << "Player " << (player + 1) << " is thinking..." << std::endl;
}

/**
 * Mô tả:
 *   Hiển thị nước đi vừa thực hiện.
 *
 * TODO:
 *   - In ra tọa độ (row, col)
 */
void TerminalRenderer::showMove(const int row, const int col) {
    std::cout << "Move placed at (" << row << ", " << col << ")" << std::endl;
}

/**
 * Mô tả:
 *   Hiển thị thông báo nước đi không hợp lệ.
 *
 * TODO:
 *   - In message lỗi
 */
void TerminalRenderer::showInvalidMove() {
    std::cout << "Invalid move!" << std::endl;
}

/**
 * Mô tả:
 *   Hiển thị kết quả game.
 *
 * TODO:
 *   - Bước 1: kiểm tra draw hoặc winner
 *   - Bước 2: in kết quả
 *   - Trường hợp biên: winner = -1
 */
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

/**
 * Mô tả:
 *   In kết quả đơn giản (judge mode).
 *
 * TODO:
 *   - In winner và số lượt
 */
void TerminalRenderer::printResult(const GameResult& gameResult) {
    std::cout << gameResult.winner << " " << gameResult.turns;
}

/**
 * Mô tả:
 *   Đóng renderer.
 *
 * TODO:
 *   - Giải phóng tài nguyên nếu có
 */
void TerminalRenderer::close() {
    // TODO: cleanup renderer
}