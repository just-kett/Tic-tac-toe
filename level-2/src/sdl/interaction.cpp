/**
 * SDL Interaction cpp implementation
 *
 */

#include "interaction.h"

/* ---------- Importing ---------- */

#include <string>

#include "../game/setup.h"

/* ---------- Layout Constants ---------- */
// Phải khớp với renderer.cpp
static const int SIDEBAR_W = 260;
static const int BOARD_PAD = 40;
static const int STATUS_H  = 60;

/* ---------- Constructor / Destructor ---------- */

SDLInteraction::SDLInteraction() {}
SDLInteraction::~SDLInteraction() {}

/* ---------- init / close ---------- */

/**
 * Mô tả: Lưu config và flush event queue còn tồn đọng.
 */
void SDLInteraction::init(const RunConfig& config) {
    screenW_ = config.screenWidth;
    screenH_ = config.screenHeight;

    // flush event queue từ trước
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

/**
 * Mô tả: Tính layout board — phải giống hệt SDLRenderer::calcLayout().
 */
SDLInteraction::BoardLayout SDLInteraction::calcLayout() const {
    BoardLayout L;
    int boardAreaW = screenW_ - SIDEBAR_W - BOARD_PAD * 2;
    int boardAreaH = screenH_ - STATUS_H  - BOARD_PAD * 2;
    L.cellSize = std::min(boardAreaW, boardAreaH) / boardSize_;
    L.boardW   = L.cellSize * boardSize_;
    L.boardH   = L.cellSize * boardSize_;
    L.offsetX  = BOARD_PAD + (boardAreaW - L.boardW) / 2;
    L.offsetY  = BOARD_PAD + (boardAreaH - L.boardH) / 2;
    return L;
}

/**
 * Mô tả: Đọc số nguyên từ keyboard qua SDL events.
 *   - Gõ chữ số (0-9), Backspace để xoá, Enter để xác nhận.
 *   - Trả về -1 nếu input rỗng hoặc không hợp lệ.
 *   - Ném QuitException nếu cửa sổ bị đóng hoặc Escape.
 */
int SDLInteraction::readIntFromKeyboard() {
    std::string buf;
    SDL_Event e;

    while (true) {
        if (!SDL_WaitEvent(&e)) continue;
        waitForQuit(e);

        if (e.type != SDL_KEYDOWN) continue;

        SDL_Keycode key = e.key.keysym.sym;

        if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            if (buf.empty()) return -1;
            try { return std::stoi(buf); }
            catch (...) { return -1; }
        }
        else if (key == SDLK_BACKSPACE) {
            if (!buf.empty()) buf.pop_back();
        }
        else if (key >= SDLK_0 && key <= SDLK_9) {
            buf += static_cast<char>('0' + (key - SDLK_0));
        }
        else if (key >= SDLK_KP_0 && key <= SDLK_KP_9) {
            buf += static_cast<char>('0' + (key - SDLK_KP_0));
        }
        else if (key == SDLK_ESCAPE) {
            throw QuitException();
        }
    }
}

/* ---------- pause ---------- */

void SDLInteraction::pause(int timeout) {
    if (timeout > 0) {
        SDL_Delay(timeout);
        return;
    }

    SDL_Event e;
    while (true) {
        if (!SDL_WaitEvent(&e)) continue;
        waitForQuit(e);
        if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN)
            return;
    }
}

/* ---------- selectSize ---------- */

/**
 * Mô tả: Gõ số + Enter. Hợp lệ: BOARD_N_MIN <= size <= BOARD_N_MAX.
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

/* ---------- selectGoal ---------- */

bool SDLInteraction::selectGoal(int* goal, const int size) {
    int val     = readIntFromKeyboard();
    int maxGoal = std::min(size, GOAL_MAX);
    if (val < 3 || val > maxGoal)
        return false;
    *goal = val;
    SDL_Delay(600);
    return true;
}

/* ---------- selectGameMode ---------- */

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
                return false;  // phím không hợp lệ → Engine hiện lỗi
        }
    }
}

/* ---------- selectBotLevel ---------- */

/**
 * Mô tả: Nhấn phím 0/1/2 để chọn bot level.
 *        0 = EASY, 1 = MEDIUM, 2 = HARD.
 */
bool SDLInteraction::selectBotLevel(BotLevel* levels, const int index) {
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

/* ---------- getPlayerMove ---------- */

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

            if (r < 0 || r >= boardSize_ || c < 0 || c >= boardSize_)
                return false;

            *row = r;
            *col = c;
            return true;
        }

        // --- Keyboard: mũi tên + Enter ---
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    keyRow_ = std::max(0, keyRow_ - 1); break;
                case SDLK_DOWN:
                    keyRow_ = std::min(boardSize_ - 1, keyRow_ + 1); break;
                case SDLK_LEFT:
                    keyCol_ = std::max(0, keyCol_ - 1); break;
                case SDLK_RIGHT:
                    keyCol_ = std::min(boardSize_ - 1, keyCol_ + 1); break;
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    *row = keyRow_;
                    *col = keyCol_;
                    return true;
                case SDLK_ESCAPE:
                    throw QuitException();
                default:
                    break;
            }
        }
    }
}
