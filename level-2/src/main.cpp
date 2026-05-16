
/* ---------- Importing ---------- */

#include <iostream>

#include "game/engine.h"
#include "utils/config.h"
#include "utils/logger.h"

// interface
#include "game/interface/i_interaction.h"
#include "game/interface/i_renderer.h"

// terminal
#include "terminal/interaction.h"
#include "terminal/renderer.h"

// sdl
#include "sdl/interaction.h"
#include "sdl/renderer.h"

int main(int argc, char* argv[]) {
    // parse config từ command line
    RunConfig config = parseArgs(argc, argv);

    // nếu có flag help thì in hướng dẫn và thoát
    if (config.is_help) {
        std::cout << configHelpStr();
        return 0;
    }

    // khởi tạo logger với các cấu hình tương ứng
    Logger::init(config.judge_mode, config.to_file, config.log_file, config.verbose_flag);
    Logger::log("Logger initialized!");

    // log chi tiết config (DEBUG level)
    Logger::log(config.toString(), Logger::Level::DEBUG);

    // khởi tạo các interface trừu tượng
    I_Renderer* iRenderer = nullptr;
    I_Interaction* iInteraction = nullptr;

    // chọn implementation dựa trên config
    if (!config.gui_flag || config.judge_mode) {
        // sử dụng terminal (CLI)
        iRenderer = new TerminalRenderer();
        Logger::log("Terminal Renderer initialized!");

        iInteraction = new TerminalInteraction();
        Logger::log("Terminal Interaction initialized!");
    } else {
        // sử dụng SDL (GUI)
        // NOTE: phần này có thể chưa hoàn thiện đầy đủ
        iRenderer = new SDLRenderer();
        Logger::log("SDL Renderer initialized!");

        iInteraction = new SDLInteraction();
        Logger::log("SDL Interaction initialized!");
    }
    Logger::log("Infra initialized!");

    // tạo game engine và inject dependency (config, renderer, interaction)
    Engine* engine = new Engine(
        &config,
        iRenderer,
        iInteraction);

    try {
        if (engine) {
            // khởi tạo engine (setup ban đầu)
            engine->init();

            // vòng đời game
            engine->startGame();                         // chuẩn bị bắt đầu game
            GameResult gameResult = engine->playGame();  // chạy game loop chính
            engine->endGame(gameResult);                 // xử lý sau khi game kết thúc
        }
    } catch (const QuitException& e) {
        // NOTE: bắt tín hiệu quit (ví dụ user thoát game giữa chừng)
        Logger::log("Quit signal received. Cleaning up...", Logger::Level::WARNING);
    } catch (const NotImplementedException& e) {
        // NOTE: bắt tín hiệu chưa cài đặt (ví dụ chưa hoàn thành toàn bộ mã nguồn)
        Logger::log("Some functions is not implemented. Cleaning up...", Logger::Level::WARNING);
    }

    // đóng tài nguyên nội bộ của engine
    engine->close();

    // giải phóng bộ nhớ engine
    delete engine;
    Logger::log("Engine destroyed!");

    // giải phóng renderer nếu đã được cấp phát
    if (iRenderer) {
        delete iRenderer;
    }

    // giải phóng interaction nếu đã được cấp phát
    if (iInteraction) {
        delete iInteraction;
    }
    Logger::log("Infra destroyed!");

    // đóng logger (flush và cleanup)
    Logger::log("Logger closing . . .");
    Logger::close();

    return 0;
}
