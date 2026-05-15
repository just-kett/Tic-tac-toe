/**
 * SDL Renderer cpp implementation
 *
 */
 
#include "renderer.h"
 
/* ---------- Importing ---------- */
 
#include <algorithm>
#include <cmath>
#include <format>
#include <iostream>
 
#include "../game/setup.h"
#include "../utils/config.h"
 
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
 
/* ---------- Definitions ---------- */
 
SDLRenderer::SDLRenderer() : I_Renderer()
{
}
 
/**
 * Mô tả: Destructor của SDLRenderer.
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ: Không (việc giải phóng thực hiện trong close()).
 */
SDLRenderer::~SDLRenderer()
{
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
void SDLRenderer::init(const RunConfig &config)
{
    // read config
    screenWidth = config.screenWidth;
    screenHeight = config.screenHeight;
    boardPadding = config.boardPadding;
    // init(...)
 
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"); // Use highest supported texture scaling quality
    TTF_Init();
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
 
    window = SDL_CreateWindow(
        "TicTacToe SDL",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,
        0);
 
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
 
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
 
    // load background image
    backgroundTexture = IMG_LoadTexture(renderer, "./assets/images.png");
    if (!backgroundTexture) {
        std::cerr << "[SDL_image ERROR] Could not load images.png. Error: " << IMG_GetError() << std::endl;
    }
 
    // load font - thử nhiều đường dẫn khác nhau
    const char* fontPaths[] = {
        "./assets/bpg-arial-webfont.ttf"      // system font (Windows)
    };
 
    for (const char* path : fontPaths) {
        font = TTF_OpenFont(path, 48);
        if (font) {
            std::cout << "[SDL_ttf] Loaded UI font from: " << path << std::endl;
            break;
        }
    }
 
    for (const char* path : fontPaths) {
        titleFont = TTF_OpenFont(path, 96);
        if (titleFont) {
            std::cout << "[SDL_ttf] Loaded title font from: " << path << std::endl;
            break;
        }
    }
 
    if (!font) {
        std::cerr << "[SDL_ttf ERROR] Could not load UI font. Error: " << TTF_GetError() << std::endl;
    }
    if (!titleFont) {
        std::cerr << "[SDL_ttf ERROR] Could not load title font. Error: " << TTF_GetError() << std::endl;
        titleFont = font;
    }
}
 
/**
 * Mô tả: Xóa toàn bộ nội dung màn hình (background tối).
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ: Reset frame hiện tại.
 */
void SDLRenderer::clearScreen()
{
    if (renderer == nullptr) {
        return;
    }
 
    if (backgroundTexture != nullptr) {
        SDL_Rect fullScreenRect = {0, 0, screenWidth, screenHeight};
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &fullScreenRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255); // dark background
        SDL_RenderClear(renderer);
    }
}
 
/**
 * Mô tả: Hiển thị frame đã render lên màn hình.
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ: Swap buffer để hiển thị nội dung.
 */
void SDLRenderer::renderPresent()
{
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
// displayboard helper 1
static void drawThickLine(SDL_Renderer *renderer,
                          int x1, int y1,
                          int x2, int y2,
                          int thickness)
{
    int half = thickness / 2;
    for (int dy = -half; dy <= half; dy++)
    {
        for (int dx = -half; dx <= half; dx++)
        {
            SDL_RenderDrawLine(renderer,
                               x1 + dx, y1 + dy,
                               x2 + dx, y2 + dy);
        }
    }
}
// displayboard helper 2
static void fillCircle(SDL_Renderer *renderer,
                       int centerX,
                       int centerY,
                       int radius)
{
    for (int dy = -radius; dy <= radius; dy++)
    {
        int dx = (int)std::sqrt(radius * radius - dy * dy);
        SDL_RenderDrawLine(renderer,
                           centerX - dx, centerY + dy,
                           centerX + dx, centerY + dy);
    }
}
 
void SDLRenderer::drawRect(int x, int y, int w, int h, SDL_Color color, bool filled)
{
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
void SDLRenderer::showSelectMenu(SelectType selectType, int context)
{
    // TODO: Render menu UI tương ứng với selectType
    clearScreen();
 
    std::string prompt = "";
 
    switch (selectType)
    {
    case SelectType::TITLE_UI:
        prompt = "TIC TAC TOE";
        break;
 
    case SelectType::SIZE_UI:
        prompt = "Select Board Size";
        break;
 
    case SelectType::GOAL_UI:
        prompt = "Select Goal";
        break;
 
    case SelectType::GAME_MODE_UI:
        prompt = "Select GameMode";
        break;
 
    case SelectType::BOT_LEVEL_UI:
        prompt = "Select Bot Level";
        break;
 
    case SelectType::PLAYER_UI:
        prompt = "Pick a move: ";
        break;
 
    case SelectType::MUL_BOT_LEVEL_UI:
        prompt = "Select Bot " + std::to_string(context) +
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
        SDL_Color textColor = {250, 250, 250, 255};
        SDL_Color shadowColor = {0, 0, 0, 200};
 
        std::string cleanPrompt = prompt;
        cleanPrompt.erase(std::remove(cleanPrompt.begin(), cleanPrompt.end(), '\n'), cleanPrompt.end());
 
        SDL_Surface *shadowSurface = TTF_RenderText_Blended(renderFont, cleanPrompt.c_str(), shadowColor);
        SDL_Surface *textSurface = TTF_RenderText_Blended(renderFont, cleanPrompt.c_str(), textColor);
        if (shadowSurface != nullptr && textSurface != nullptr)
        {
            SDL_Texture *shadowTexture = SDL_CreateTextureFromSurface(renderer, shadowSurface);
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (shadowTexture != nullptr && textTexture != nullptr) {
                int textX = (selectType == SelectType::TITLE_UI)
                                ? 100
                                : (screenWidth - textSurface->w) / 2;
                int textY = (selectType == SelectType::TITLE_UI)
                                ? 50
                                : (screenHeight - textSurface->h) / 2;
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
 
    renderPresent();
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
void SDLRenderer::showInvalidSelect(SelectType selectType, int context)
{
    // TODO: Render thông báo lỗi
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
void SDLRenderer::showValidSelect(SelectType selectType, int context)
{
    // TODO: Render thông báo thành công
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
void SDLRenderer::displayBoard(const char board[][BOARD_N_MAX], const int size)
{
    currentBoardSize = size;
 
    // TODO: Render board
    // layout
    int boardArea = screenWidth - 2 * boardPadding;
    int cellSize = boardArea / size;
    int startX = boardPadding;
    int startY = boardPadding;
    int endX = startX + boardArea;
    int endY = startY + boardArea;
 
    const SDL_Color GRID_COLOR = {255, 255, 255, 255};
    const SDL_Color PLAYER_X_COLOR = {255, 50, 50, 255};
    const SDL_Color PLAYER_O_COLOR = {50, 100, 255, 255};
 
    // draw grid with white lines
    SDL_SetRenderDrawColor(renderer, GRID_COLOR.r, GRID_COLOR.g, GRID_COLOR.b, GRID_COLOR.a);
 
    for (int i = 0; i <= size; i++)
    {
        int y = startY + i * cellSize;
        SDL_RenderDrawLine(renderer, startX, y, endX, y);
    }
 
    for (int j = 0; j <= size; j++)
    {
        int x = startX + j * cellSize;
        SDL_RenderDrawLine(renderer, x, startY, x, endY);
    }
 
    // draw X/O in each cell
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (board[i][j] == 'X')
            {
                SDL_SetRenderDrawColor(renderer,
                                       PLAYER_X_COLOR.r,
                                       PLAYER_X_COLOR.g,
                                       PLAYER_X_COLOR.b,
                                       PLAYER_X_COLOR.a);
 
                int cellX = startX + j * cellSize;
                int cellY = startY + i * cellSize;
                int padding = cellSize / 5;
                int thickness = std::max(4, cellSize / 15);
 
                drawThickLine(renderer,
                              cellX + padding,
                              cellY + padding,
                              cellX + cellSize - padding,
                              cellY + cellSize - padding,
                              thickness);
                drawThickLine(renderer,
                              cellX + cellSize - padding,
                              cellY + padding,
                              cellX + padding,
                              cellY + cellSize - padding,
                              thickness);
            }
            else if (board[i][j] == 'O')
            {
                int centerX = startX + j * cellSize + cellSize / 2;
                int centerY = startY + i * cellSize + cellSize / 2;
                int outerRadius = cellSize / 4;
                int ringThickness = std::max(4, cellSize / 15);
 
                SDL_SetRenderDrawColor(renderer,
                                       PLAYER_O_COLOR.r,
                                       PLAYER_O_COLOR.g,
                                       PLAYER_O_COLOR.b,
                                       PLAYER_O_COLOR.a);
                fillCircle(renderer, centerX, centerY, outerRadius);
 
                SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
                fillCircle(renderer, centerX, centerY, std::max(0, outerRadius - ringThickness));
            }
        }
    }
 
    renderPresent();
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
void SDLRenderer::showMove(const int row, const int col)
{
    if (currentBoardSize <= 0)
    {
        return; // base case
    }
 
    int boardArea = screenWidth - 2 * boardPadding;
    int cellSize = boardArea / currentBoardSize;
    int cellX = boardPadding + col * cellSize;
    int cellY = boardPadding + row * cellSize;
 
    SDL_Rect highlight = {cellX + 2, cellY + 2, cellSize - 4, cellSize - 4};
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 100);
    SDL_RenderFillRect(renderer, &highlight);
    renderPresent();
}
 
/**
 * Mô tả: Hiển thị thông báo nước đi không hợp lệ.
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ: Hiển thị lỗi.
 * TODO:
 *   - Bước 1: Render thông báo lỗi.
 */
void SDLRenderer::showInvalidMove()
{
    // TODO: Render invalid move message
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
void SDLRenderer::showPlayer(const int player, const bool is_bot)
{
    // TODO: Render player info
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
void SDLRenderer::showResult(const int winner, const bool is_bot, const WinLine *winLine)
{
    // TODO: Render result
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
void SDLRenderer::printResult(const GameResult &gameResult)
{
    // TODO: Print result
}
 
/**
 * Mô tả: Giải phóng tài nguyên SDL.
 * Đầu vào: Không.
 * Đầu ra: Không.
 * Tác dụng phụ:
 *   - Destroy renderer và window.
 *   - Shutdown SDL subsystem.
 */
void SDLRenderer::close()
{
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    if (titleFont) {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
 
    TTF_Quit();
    IMG_Quit();
 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}