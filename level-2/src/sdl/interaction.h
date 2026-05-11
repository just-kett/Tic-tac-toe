/**
 * SDL Interaction header file
 *
 */

#pragma once

/* ---------- Importing ---------- */

#include <SDL2/SDL.h>

#include "../game/interface/i_interaction.h"
#include "../game/setup.h"
#include "../utils/config.h"

/* ---------- Declarations ---------- */

/**
 * Mô tả: Lớp xử lý tương tác người dùng sử dụng SDL (event-based).
 * Đầu vào: Không (khởi tạo qua init với RunConfig).
 * Đầu ra: Không.
 * Tác dụng phụ:
 *   - Lắng nghe và xử lý SDL events (keyboard, mouse, quit,...).
 *   - Điều khiển luồng input cho game GUI.
 * NOTE: Hoạt động theo mô hình event loop, khác với terminal input.
 */
class SDLInteraction : public I_Interaction {
   private:
    // --- Screen dimensions (from RunConfig) ---
    int screenW_ = 800;
    int screenH_ = 800;

    // --- Board state ---
    int boardSize_ = 3;  // cập nhật khi selectSize() được gọi

    // --- Keyboard cursor (mũi tên + Enter) ---
    int keyRow_ = 0;
    int keyCol_ = 0;

    // --- Layout helper (phải khớp với SDLRenderer::calcLayout) ---
    struct BoardLayout {
        int cellSize;
        int boardW, boardH;
        int offsetX, offsetY;
    };
    BoardLayout calcLayout() const;

    // --- Private helpers ---
    bool waitForQuit(SDL_Event& e);
    int  readIntFromKeyboard();  // gõ số + Enter để xác nhận

   public:
    SDLInteraction();
    ~SDLInteraction();

    void init(const RunConfig& config) override;

    void pause(int timeout = 0) override;

    bool selectSize(int* size) override;
    bool selectGoal(int* goal, const int size) override;
    bool selectGameMode(GameMode* mode) override;
    bool selectBotLevel(BotLevel* levels, const int index) override;
    bool getPlayerMove(int* row, int* col) override;

    void close() override;
};
