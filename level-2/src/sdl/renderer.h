/**
 * SDL Renderer header file
 *
 */

#pragma once

/* ---------- Importing ---------- */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "../game/interface/i_renderer.h"
#include "../game/logic.h"

/* ---------- Declarations ---------- */

/**
 * Mô tả: Lớp Renderer sử dụng SDL để hiển thị giao diện đồ họa (GUI).
 * Đầu vào: Không (khởi tạo thông qua init với RunConfig).
 * Đầu ra: Không.
 * Tác dụng phụ:
 *   - Tạo cửa sổ SDL và context render.
 *   - Vẽ các thành phần giao diện (board, text, UI).
 * NOTE: Phải gọi init() trước khi sử dụng các hàm render.
 */
class SDLRenderer : public I_Renderer {
   private:
    // --- SDL core ---
    SDL_Window*   window_   = nullptr;
    SDL_Renderer* renderer_ = nullptr;

    // --- Fonts ---
    TTF_Font* fontLarge_ = nullptr;
    TTF_Font* fontMed_   = nullptr;
    TTF_Font* fontSmall_ = nullptr;

    // --- Screen state ---
    int screenW_   = 800;
    int screenH_   = 800;
    int boardSize_ = 3;

    // --- Cached board state ---
    // Lưu trạng thái board để có thể vẽ lại bất kỳ lúc nào
    char cachedBoard_[BOARD_N_MAX][BOARD_N_MAX] = {};
    bool boardInitialized_ = false;
    int  lastRow_ = -1;  // vị trí nước đi cuối (để highlight)
    int  lastCol_ = -1;

    // --- Layout helper ---
    struct BoardLayout {
        int cellSize;
        int boardW, boardH;
        int offsetX, offsetY;
    };
    BoardLayout calcLayout(int size) const;

    // --- Private draw helpers ---
    void renderPresent();
    void drawRect(int x, int y, int w, int h, SDL_Color color, bool filled);
    int  drawText(const std::string& text, int x, int y,
                  SDL_Color color, TTF_Font* font);
    void drawTextCentered(const std::string& text, int x, int y,
                          int maxW, SDL_Color color, TTF_Font* font);

    // Vẽ board từ cache — được gọi bởi mọi hàm render
    void drawBoard();

   public:
    SDLRenderer();
    ~SDLRenderer();

    void init(const RunConfig& config) override;
    void clearScreen() override;

    void showSelectMenu(SelectType selectType, int context = NO_CONTEXT) override;
    void showInvalidSelect(SelectType selectType, int context = NO_CONTEXT) override;
    void showValidSelect(SelectType selectType, int context = NO_CONTEXT) override;

    void displayBoard(const char board[][BOARD_N_MAX], const int size) override;
    void showMove(const int row, const int col) override;
    void showInvalidMove() override;

    void showPlayer(const int player, const bool is_bot) override;
    void showResult(const int winner, const bool is_bot,
                    const WinLine* winLine = nullptr) override;
    void printResult(const GameResult& gameResult) override;

    void close() override;
};
