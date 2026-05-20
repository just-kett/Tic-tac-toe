/**
 * SDL Renderer cpp implementation
 *
 */

#include "renderer.h"

/* ---------- Importing ---------- */

#include <format>
#include <iostream>

#include "../game/setup.h"
#include "../utils/config.h"

/* ---------- Definitions ---------- */

SDLRenderer::SDLRenderer() : I_Renderer() {
}

/**
 * Mô tả: Destructor của SDLRenderer.
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ: Không (việc giải phóng thực hiện trong close()).
 */
SDLRenderer::~SDLRenderer() {
}

/**
 * Mô tả: Khởi tạo SDL, window và renderer.
 * Đầu vào: config - cấu hình màn hình và layout.
 * Đầu ra: Không.
 * Tác dụng phụ:
 *   - Khởi tạo SDL subsystem.
 *   - Tạo window và renderer.
 *   - Thiết lập chế độ blend.
 * NOTE: Phải gọi trước khi render.
 */
void SDLRenderer::init(const RunConfig& config) {
    // read config
    int screenWidth = config.screenWidth;
    int screenHeight = config.screenHeight;

    // int boardPadding = config.boardPadding;
    // init(...)

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    window = SDL_CreateWindow(
        "TicTacToe SDL",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,
        0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // load font
    // font = TTF_OpenFont("assets/font.ttf", font_size);
}

/**
 * Mô tả: Xóa toàn bộ nội dung màn hình (background tối).
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ: Reset frame hiện tại.
 */
void SDLRenderer::clearScreen() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);  // dark background
    SDL_RenderClear(renderer);
}

/**
 * Mô tả: Hiển thị frame đã render lên màn hình.
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ: Swap buffer để hiển thị nội dung.
 */
void SDLRenderer::renderPresent() {
    SDL_RenderPresent(renderer);
}

/**
 * Mô tả: Vẽ hình chữ nhật lên renderer.
 * Đầu vào:
 *   - x, y: tọa độ.
 *   - w, h: kích thước.
 *   - color: màu sắc.
 *   - filled: true nếu fill, false nếu vẽ viền.
 * Đầu ra: Không.
 * Tác dụng phụ: Vẽ trực tiếp lên renderer.
 */
void SDLRenderer::drawRect(int x, int y, int w, int h, SDL_Color color, bool filled) {
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    if (filled)
        SDL_RenderFillRect(renderer, &rect);
    else
        SDL_RenderDrawRect(renderer, &rect);
}

/**
 * Mô tả: Hiển thị menu lựa chọn.
 * Đầu vào: selectType, context.
 * Đầu ra: Không.
 * Tác dụng phụ: Vẽ UI lên màn hình.
 * TODO:
 *   - Bước 1: Xác định loại menu.
 *   - Bước 2: Render text tương ứng.
 *   - Trường hợp biên: selectType không hợp lệ.
 */
void SDLRenderer::showSelectMenu(SelectType selectType, int context) {

    if (selectType != SelectType::PLAYER_UI) {
    clearScreen();    
    SDL_Rect fullScreenRect = {0, 0, screenWidth, screenHeight};
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &fullScreenRect);
    }
 
    std::string prompt = "";
 
    switch (selectType) {   
    case SelectType::TITLE_UI:
        prompt = "TIC TAC TOE";
        break;
 
    case SelectType::SIZE_UI:
        prompt = "Enter Board Size";
        break;
 
    case SelectType::GOAL_UI:
        prompt = "Enter Goal";
        break;
 
    case SelectType::GAME_MODE_UI:
        prompt = "Enter Game Mode";
        break;
 
    case SelectType::BOT_LEVEL_UI:
        prompt = "Enter Bot Level";
        break;
    
    case SelectType::PLAYER_UI:
        prompt = "."; // placeholder
        break;
 
    case SelectType::MUL_BOT_LEVEL_UI:
        prompt = "Enter Bot " + std::to_string(context + 1) +
                 " level";
        break;
 
    default:
        break;
    }

    TTF_Font* renderFont = nullptr;
    if (selectType == SelectType::TITLE_UI && titleFont != nullptr) {
        renderFont = titleFont;
    } else {
        renderFont = font;
    }
 
    if (renderFont != nullptr)
    {
        SDL_Color shadowColor = {250, 250, 250, 255};
        SDL_Color textColor = {0, 0, 0, 200};

        SDL_Rect fullScreenRect = {0, 0, screenWidth, screenHeight};
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &fullScreenRect);
 
        std::string cleanPrompt = prompt;
        cleanPrompt.erase(std::remove(cleanPrompt.begin(), cleanPrompt.end(), '\n'), cleanPrompt.end());
 
        SDL_Surface *shadowSurface = TTF_RenderText_Blended_Wrapped(renderFont, cleanPrompt.c_str(), shadowColor, NULL);
        SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(renderFont, cleanPrompt.c_str(), textColor, NULL);
        if (shadowSurface != nullptr && textSurface != nullptr)
        {
            SDL_Texture *shadowTexture = SDL_CreateTextureFromSurface(renderer, shadowSurface);
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (shadowTexture != nullptr && textTexture != nullptr) {
                int textX = (screenWidth - textSurface->w) / 2;
                int textY = (screenHeight - textSurface->h) / 2; 
                SDL_Rect shadowQuad = {textX + 3, textY + 3, shadowSurface->w, shadowSurface->h};
                SDL_Rect renderQuad = {textX, textY, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, shadowTexture, NULL, &shadowQuad);
                SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
                SDL_DestroyTexture(shadowTexture);
                SDL_DestroyTexture(textTexture);
            } else {
                std::cerr << "[SDL_ttf ERROR] Could not create texture. Error: " << SDL_GetError() << std::endl;
            }
            SDL_FreeSurface(shadowSurface);
            SDL_FreeSurface(textSurface);
        }
        else
        {
            std::cerr << "[SDL_ttf ERROR] " << TTF_GetError() << std::endl;
            if (shadowSurface) SDL_FreeSurface(shadowSurface);
            if (textSurface) SDL_FreeSurface(textSurface);
        }
    }
    else
    {
        std::cerr << "[SDL_ttf ERROR] Font is nullptr " << std::endl;
    }

    if (selectType != SelectType::PLAYER_UI) {
        renderPresent();
    }

}
 
void SDLRenderer::showInvalidSelect(SelectType selectType, int context) {}
 
void SDLRenderer::showValidSelect(SelectType selectType, int context) {}
 
void SDLRenderer::displayBoard(const char board[][BOARD_N_MAX], const int size, bool showWinline) {
    
    clearScreen();
    currentBoardSize = size;
    SDL_Rect fullScreenRect = {0, 0, screenWidth, screenHeight};
    SDL_RenderCopy(renderer, boardTexture, NULL, &fullScreenRect);

    int boardArea = std::min(screenWidth, screenHeight);
    int cellSize = boardArea / size;
    int startX = (screenWidth - boardArea) / 2;  
    int startY = (screenHeight - boardArea) / 2; 
    int endX = startX + boardArea;
    int endY = startY + boardArea;
 
    const SDL_Color GRID_COLOR = {150, 150, 150, 255};
    const SDL_Color PLAYER_X_COLOR = {40, 40, 40, 255};
    const SDL_Color PLAYER_O_COLOR = {40, 40, 40, 255};
 
    SDL_SetRenderDrawColor(renderer, GRID_COLOR.r, GRID_COLOR.g, GRID_COLOR.b, GRID_COLOR.a);
 
    int gridThickness = std::max(6, cellSize / 20);
    
    for (int i = 1; i < size; i++) {
        int y = startY + i * cellSize;
        drawThickLine(renderer, startX, y, endX, y, gridThickness);
    }
 
    for (int j = 1; j < size; j++) {
        int x = startX + j * cellSize;
        drawThickLine(renderer, x, startY, x, endY, gridThickness);
    }
 
    for (int i = 0; i < size; i++) {

        for (int j = 0; j < size; j++) {

           int cellX = startX + j * cellSize;
           int cellY = startY + i * cellSize;

           if (board[i][j] == 'X') {
             int padding = cellSize / 5;
             int thickness = std::max(6, cellSize / 14);

             Sint16 x1 = cellX + padding;
             Sint16 y1 = cellY + padding;

             Sint16 x2 = cellX + cellSize - padding;
             Sint16 y2 = cellY + cellSize - padding;

            thickLineRGBA(renderer,
                        x1,
                        y1,
                        x2,
                        y2,
                        thickness * 2,
                        PLAYER_X_COLOR.r,
                        PLAYER_X_COLOR.g,
                        PLAYER_X_COLOR.b,
                        PLAYER_X_COLOR.a);

            thickLineRGBA(renderer,
                        x2,
                        y1,
                        x1,
                        y2,
                        thickness * 2,
                        PLAYER_X_COLOR.r,
                        PLAYER_X_COLOR.g,
                        PLAYER_X_COLOR.b,
                        PLAYER_X_COLOR.a);
            
            }
            else if (board[i][j] == 'O') {
                   int centerX = cellX + cellSize / 2;
                   int centerY = cellY + cellSize / 2;

                   int radius = cellSize / 3;
                   int thickness = std::max(5, cellSize / 10);

                   for (int r = radius; r > radius - thickness; --r) {
                      aacircleRGBA(renderer,
                            centerX,
                            centerY,
                            r,
                            PLAYER_O_COLOR.r,
                            PLAYER_O_COLOR.g,
                            PLAYER_O_COLOR.b,
                            PLAYER_O_COLOR.a);
                    }
                }
            }
        }
    
    if (showWinline) {
        renderPresent();
    }
}

/**
 * Mô tả: Hiển thị thông báo lựa chọn không hợp lệ.
 * Đầu vào: selectType, context.
 * Đầu ra: Không.
 * Tác dụng phụ: Hiển thị thông báo lỗi.
 * TODO:
 *   - Bước 1: Xác định loại lỗi.
 *   - Bước 2: Render text cảnh báo.
 */
void SDLRenderer::showInvalidSelect(SelectType selectType, int context) {
    // TODO: Render thông báo lỗi
    throw NotImplementedException();
}

/**
 * Mô tả: Hiển thị thông báo lựa chọn hợp lệ.
 * Đầu vào: selectType, context.
 * Đầu ra: Không.
 * Tác dụng phụ: Hiển thị xác nhận.
 * TODO:
 *   - Bước 1: Xác định loại selection.
 *   - Bước 2: Render thông báo thành công.
 */
void SDLRenderer::showValidSelect(SelectType selectType, int context) {
    // TODO: Render thông báo thành công
    throw NotImplementedException();
}

/**
 * Mô tả: Vẽ bàn cờ lên màn hình.
 * Đầu vào: board, size.
 * Đầu ra: Không.
 * Tác dụng phụ: Render grid và ký hiệu.
 * TODO:
 *   - Bước 1: Tính toán layout ô.
 *   - Bước 2: Vẽ grid.
 *   - Bước 3: Vẽ X/O.
 */
void SDLRenderer::displayBoard(const char board[][BOARD_N_MAX], const int size) {
    // TODO: Render board
    throw NotImplementedException();
}

/**
 * Mô tả: Hiển thị nước đi vừa thực hiện.
 * Đầu vào: row, col.
 * Đầu ra: Không.
 * Tác dụng phụ: Highlight ô.
 * TODO:
 *   - Bước 1: Xác định vị trí ô.
 *   - Bước 2: Vẽ highlight.
 */
void SDLRenderer::showMove(const int row, const int col) {
    // TODO: Highlight move
    throw NotImplementedException();
}

/**
 * Mô tả: Hiển thị thông báo nước đi không hợp lệ.
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ: Hiển thị lỗi.
 * TODO:
 *   - Bước 1: Render thông báo lỗi.
 */
void SDLRenderer::showInvalidMove() {
    // TODO: Render invalid move message
    throw NotImplementedException();
}

/**
 * Mô tả: Hiển thị người chơi hiện tại.
 * Đầu vào: player, is_bot.
 * Đầu ra: Không.
 * Tác dụng phụ: Hiển thị thông tin turn.
 * TODO:
 *   - Bước 1: Xác định text.
 *   - Bước 2: Render lên màn hình.
 */
void SDLRenderer::showPlayer(const int player, const bool is_bot) {
    // TODO: Render player info
    throw NotImplementedException();
}

/**
 * Mô tả: Hiển thị kết quả game.
 * Đầu vào: winner, is_bot, winLine.
 * Đầu ra: Không.
 * Tác dụng phụ: Hiển thị kết quả và highlight đường thắng.
 * TODO:
 *   - Bước 1: Kiểm tra draw hoặc win.
 *   - Bước 2: Render text.
 *   - Bước 3: Highlight winLine nếu có.
 */
void SDLRenderer::showResult(const int winner, const bool is_bot, const WinLine* winLine) {
    // TODO: Render result
    throw NotImplementedException();
}

/**
 * Mô tả: In kết quả ra stdout (judge mode).
 * Đầu vào: gameResult.
 * Đầu ra: Không.
 * Tác dụng phụ: In console.
 * TODO:
 *   - Bước 1: Format output.
 *   - Bước 2: In ra std::cout.
 */
void SDLRenderer::printResult(const GameResult& gameResult) {
    // TODO: Print result
    throw NotImplementedException();
}

/**
 * Mô tả: Giải phóng tài nguyên SDL.
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ:
 *   - Destroy renderer và window.
 *   - Shutdown SDL subsystem.
 */
void SDLRenderer::close() {
    // if (font) {
    //     TTF_CloseFont(font);
    //     font = nullptr;
    // }

    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}