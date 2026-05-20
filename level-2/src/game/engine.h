

#pragma once

/* ---------- Importing ---------- */

#include "../utils/config.h"
#include "interface/i_interaction.h"
#include "interface/i_renderer.h"
#include "setup.h"

enum class State {
    INIT, TITLE, SELECT_SIZE, SELECT_GOAL, SELECT_MODE,
    SELECT_BOT, PLAYING
};

class Engine {
   private:
    const RunConfig* config;      // con trỏ tới cấu hình chạy chương trình (không sở hữu)
    I_Renderer* iRenderer;        // renderer dùng để vẽ UI
    I_Interaction* iInteraction;  // interaction dùng để nhận input

    GameSetup gameSetup;  
    bool sanity_check();

   public:
    /**
     * Mô tả:
     *   Constructor của Engine.
     *   Nhận các dependency cần thiết và lưu lại để sử dụng trong vòng đời game.
     *
     * Đầu vào:
     *   - _config: con trỏ tới cấu hình chương trình
     *   - _iRenderer: renderer cụ thể
     *   - _iInteraction: interaction cụ thể
     *
     * Đầu ra:
     *   - Không có
     *
     * Tác dụng phụ:
     *   - Thiết lập trạng thái ban đầu của engine
     */
    Engine(const RunConfig* _config, I_Renderer* _iRenderer, I_Interaction* _iInteraction);

    /**
     * Mô tả:
     *   Destructor của Engine.
     *   Dùng để giải phóng hoặc cleanup các tài nguyên nội bộ nếu có.
     *
     * Đầu vào:
     *   - Không có
     *
     * Đầu ra:
     *   - Không có
     *
     * Tác dụng phụ:
     *   - Giải phóng tài nguyên
     */
    ~Engine();

    /**
     * Mô tả:
     *   Khởi tạo engine trước khi bắt đầu game.
     *   Có thể bao gồm:
     *   - Kiểm tra config (sanity_check)
     *   - Thiết lập gameSetup
     *   - Khởi tạo renderer/interaction nếu cần
     *
     * Đầu vào:
     *   - Không có
     *
     * Đầu ra:
     *   - Không có
     *
     * Tác dụng phụ:
     *   - Thay đổi trạng thái nội bộ của engine
     */
    void init();
    void close();

    void startGame();
    void selectSize();
    void selectGoal();
    void selectGameMode();
    void selectBotLevels();
    GameResult playGame();
    void endGame(const GameResult& gameResult);

    bool running() { return isRunning; };
    
};