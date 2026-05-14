/**
 * SDL Interaction cpp implementation
 *
 */

#include "interaction.h"
#include <format>
#include <iostream>
#include <string>
#include "../game/setup.h"

// Phải khớp với renderer.cpp
static const int SIDEBAR_W = 260;
static const int BOARD_PAD = 40;
static const int STATUS_H  = 60;

/* ---------- Constructor / Destructor ---------- */

SDLInteraction::SDLInteraction() {}
SDLInteraction::~SDLInteraction() {}

void SDLInteraction::init(const RunConfig& config) {
    screenW_ = config.screenWidth;
    screenH_ = config.screenHeight;

    // dọn dẹp event queue từ trước
    SDL_Event e;
    while (SDL_PollEvent(&e)) {}
}

void SDLInteraction::close() {}

/* ---------- Private helpers ---------- */

bool SDLInteraction::waitForQuit(SDL_Event& e) {
    if (e.type == SDL_QUIT)
        throw QuitException();
    return false;
}

SDLInteraction::BoardLayout SDLInteraction::calcLayout() const {
    BoardLayout L;
    int boardAreaW = screenW_ - SIDEBAR_W - BOARD_PAD * 2; // available width for board
    int boardAreaH = screenH_ - STATUS_H  - BOARD_PAD * 2; // available height for board
    L.cellSize = std::min(boardAreaW, boardAreaH) / boardSize_; // board must fit both ways
    L.boardW   = L.cellSize * boardSize_;
    L.boardH   = L.cellSize * boardSize_;
    L.offsetX  = BOARD_PAD + (boardAreaW - L.boardW) / 2;
    L.offsetY  = BOARD_PAD + (boardAreaH - L.boardH) / 2;
    return L;
}

/**
 * Đọc số nguyên từ keyboard qua SDL events.
 *   - Gõ chữ số (0-9), Backspace để xoá, Enter để xác nhận.
 *   - Trả về -1 nếu input rỗng hoặc không hợp lệ.
 *   - Ném QuitException nếu cửa sổ bị đóng hoặc Escape.
 */
int SDLInteraction::readIntFromKeyboard() {
    std::string buf; // temporary text buffer storing typed digits
    SDL_Event e;

    while (true) {
        if (!SDL_WaitEvent(&e)) continue;
        waitForQuit(e);

        if (e.type != SDL_KEYDOWN) continue;

        SDL_Keycode key = e.key.keysym.sym;

        if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            if (buf.empty()) return -1;
            try { return std::stoi(buf); } // string to integer
            catch (...) { return -1; } // catch any exceptions
        }
        else if (key == SDLK_BACKSPACE) {
            if (!buf.empty()) buf.pop_back();
        }
        else if (key >= SDLK_0 && key <= SDLK_9) { // nhap so tu ban phim
            buf += static_cast<char>('0' + (key - SDLK_0));
        }
        else if (key >= SDLK_KP_0 && key <= SDLK_KP_9) { // nhap so tu numpad
            buf += static_cast<char>('0' + (key - SDLK_KP_0));
        }
        else if (key == SDLK_ESCAPE) {
            throw QuitException();
        }
    }
}

void SDLInteraction::pause(int timeout) {
    if (timeout > 0) {
        SDL_Delay(timeout);
        return;
    }

    // else wait for user interaction
    SDL_Event e;

    // flush stale events before waiting
    while (SDL_PollEvent(&e)) {
        waitForQuit(e);
    }

    while (true) {
        if (!SDL_WaitEvent(&e)) continue;
        waitForQuit(e);
        if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN)
            return; // user interaction detected, pause ends
    }
}


/**
 * Gõ số + Enter. Hợp lệ: BOARD_N_MIN <= size <= BOARD_N_MAX.
 *        SDL_Delay(600) để người dùng thấy màn hình xác nhận.
 */
bool SDLInteraction::selectSize(int* size) {
    int val = readIntFromKeyboard();
    if (val < BOARD_N_MIN || val > BOARD_N_MAX)
        return false;
    *size      = val;
    boardSize_ = val;
    SDL_Delay(600);  // pause để thấy showValidSelect
    return true;
}


bool SDLInteraction::selectGoal(int* goal, const int size) {
    int val     = readIntFromKeyboard();
    int maxGoal = std::min(size, GOAL_MAX);
    if (val < 3 || val > maxGoal)
        return false;
    *goal = val;
    SDL_Delay(600);
    return true;
}

/**
 * Mô tả: Nhấn phím 0/1/2 (không cần Enter).
 *        0 = PVP, 1 = PVE, 2 = EVE.
 */
bool SDLInteraction::selectGameMode(GameMode* mode) {
    SDL_Event e;
    while (true) {
        if (!SDL_WaitEvent(&e)) continue;
        waitForQuit(e);
        if (e.type != SDL_KEYDOWN) continue;

        switch (e.key.keysym.sym) {
            case SDLK_0: case SDLK_KP_0:
                *mode = GameMode::PVP; SDL_Delay(600); return true;
            case SDLK_1: case SDLK_KP_1:
                *mode = GameMode::PVE; SDL_Delay(600); return true;
            case SDLK_2: case SDLK_KP_2:
                *mode = GameMode::EVE; SDL_Delay(600); return true;
            case SDLK_ESCAPE:
                throw QuitException();
            default:
                return false;  // phím không hợp lệ - Engine hiện lỗi
        }
    }
}

/**
 * Mô tả: Nhấn phím 0/1/2 để chọn bot level.
 *        0 = EASY, 1 = MEDIUM, 2 = HARD.
 */
bool SDLInteraction::selectBotLevel(BotLevel* levels, const int index) { // index decides which bot are we choosing different levels for
    if (index < 0 || index > 1) return false;

    SDL_Event e;
    while (true) {
        if (!SDL_WaitEvent(&e)) continue;
        waitForQuit(e);
        if (e.type != SDL_KEYDOWN) continue;

        switch (e.key.keysym.sym) {
            case SDLK_0: case SDLK_KP_0:
                levels[index] = BotLevel::EASY;   SDL_Delay(600); return true;
            case SDLK_1: case SDLK_KP_1:
                levels[index] = BotLevel::MEDIUM; SDL_Delay(600); return true;
            case SDLK_2: case SDLK_KP_2:
                levels[index] = BotLevel::HARD;   SDL_Delay(600); return true;
            case SDLK_ESCAPE:
                throw QuitException();
            default:
                return false;
        }
    }
}

/**
 * Mô tả: Chờ mouse click trên board, chuyển pixel → (row, col).
 *        Hỗ trợ thêm mũi tên + Enter cho keyboard navigation.
 *        Trả về false nếu click ngoài board (Engine sẽ gọi lại).
 */
bool SDLInteraction::getPlayerMove(int* row, int* col) {
    BoardLayout L = calcLayout();
    SDL_Event e;

    while (true) {
        if (!SDL_WaitEvent(&e)) continue;
        waitForQuit(e);

        // --- Mouse click ---
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mx = e.button.x;
            int my = e.button.y;

            if (mx < L.offsetX || mx >= L.offsetX + L.boardW ||
                my < L.offsetY || my >= L.offsetY + L.boardH)
                return false;  // click ngoài board

            int c = (mx - L.offsetX) / L.cellSize;
            int r = (my - L.offsetY) / L.cellSize;

            if (r < 0 || r >= boardSize_ || c < 0 || c >= boardSize_) // second validation acts as a safety check
                return false;

            *row = r;
            *col = c;
            return true;
        }

        // --- Keyboard: mũi tên + Enter ---
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    throw QuitException();
                default:
                    break;
            }
        }
    }
}


bool SDLInteraction::askPlayAgain() {
    // flush stale events so a previously queued keypress isn't misread
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        waitForQuit(e);
    }

    while (true) {
        if (!SDL_WaitEvent(&e)) continue;
        waitForQuit(e);

        if (e.type != SDL_KEYDOWN) continue;

        switch (e.key.keysym.sym) {
            case SDLK_y:
                SDL_Delay(600); // pause để thấy showValidSelect
                return true;
            case SDLK_n:
                return false;  // không ném QuitException — đây là lựa chọn hợp lệ
            default:
                break;  // bỏ qua phím không liên quan, tiếp tục chờ
        }
    }
}