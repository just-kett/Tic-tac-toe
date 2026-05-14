/**
 * SDL Renderer cpp implementation
 *
 */

#include "renderer.h"

/* ---------- Importing ---------- */

#include <iostream>
#include <string>

#include "../game/setup.h"
#include "../utils/config.h"

/* ---------- Layout Constants ---------- */

static const SDL_Color COLOR_BG        = {15,  15,  25,  255};
static const SDL_Color COLOR_GRID      = {80,  80,  120, 255};
static const SDL_Color COLOR_X         = {70,  130, 255, 255};
static const SDL_Color COLOR_O         = {255, 90,  90,  255};
static const SDL_Color COLOR_WIN_HL    = {255, 220, 50,  180};
static const SDL_Color COLOR_LAST_MOVE = {100, 255, 150, 80};
static const SDL_Color COLOR_TEXT_MAIN = {230, 230, 230, 255};
static const SDL_Color COLOR_TEXT_DIM  = {130, 130, 160, 255};
static const SDL_Color COLOR_ERROR     = {255, 80,  80,  255};
static const SDL_Color COLOR_SUCCESS   = {80,  220, 120, 255};
static const SDL_Color COLOR_PANEL     = {25,  25,  45,  255};

static const int SIDEBAR_W       = 260;
static const int BOARD_PAD       = 40;
static const int STATUS_H        = 60;
static const int FONT_SIZE_LARGE = 28;
static const int FONT_SIZE_MED   = 20;
static const int FONT_SIZE_SMALL = 15;

/* ---------- Helper: thick circle (Bresenham) ---------- */

static void drawCircle(SDL_Renderer* r, int cx, int cy, int radius) {
    int x = radius, y = 0, err = 0;
    while (x >= y) {
        SDL_RenderDrawPoint(r, cx + x, cy + y);
        SDL_RenderDrawPoint(r, cx + y, cy + x);
        SDL_RenderDrawPoint(r, cx - y, cy + x);
        SDL_RenderDrawPoint(r, cx - x, cy + y);
        SDL_RenderDrawPoint(r, cx - x, cy - y);
        SDL_RenderDrawPoint(r, cx - y, cy - x);
        SDL_RenderDrawPoint(r, cx + y, cy - x);
        SDL_RenderDrawPoint(r, cx + x, cy - y);
        y++;
        err += 1 + 2 * y;
        if (2 * (err - x) + 1 > 0) { x--; err += 1 - 2 * x; }
    }
}

static void drawThickCircle(SDL_Renderer* r, int cx, int cy, int radius, int thickness) {
    for (int t = 0; t < thickness; t++)
        drawCircle(r, cx, cy, radius - t);
}

/* ---------- Constructor / Destructor ---------- */

SDLRenderer::SDLRenderer() : I_Renderer() {}
SDLRenderer::~SDLRenderer() {}

/* ---------- init / close ---------- */

void SDLRenderer::init(const RunConfig& config) {
    screenW_ = config.screenWidth;
    screenH_ = config.screenHeight;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());

    if (TTF_Init() < 0)
        throw std::runtime_error(std::string("TTF_Init failed: ") + TTF_GetError());

    window_ = SDL_CreateWindow(
        "TicTacToe",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenW_, screenH_,
        SDL_WINDOW_SHOWN);

    if (!window_)
        throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());

    renderer_ = SDL_CreateRenderer(window_, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer_)
        throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);

    fontLarge_ = TTF_OpenFont("assets/font.ttf", FONT_SIZE_LARGE);
    fontMed_   = TTF_OpenFont("assets/font.ttf", FONT_SIZE_MED);
    fontSmall_ = TTF_OpenFont("assets/font.ttf", FONT_SIZE_SMALL);

    if (!fontLarge_)
        SDL_Log("Warning: could not load font: %s", TTF_GetError());
}

void SDLRenderer::close() {
    if (fontLarge_) { TTF_CloseFont(fontLarge_); fontLarge_ = nullptr; }
    if (fontMed_)   { TTF_CloseFont(fontMed_);   fontMed_   = nullptr; }
    if (fontSmall_) { TTF_CloseFont(fontSmall_); fontSmall_ = nullptr; }
    if (renderer_)  { SDL_DestroyRenderer(renderer_); renderer_ = nullptr; }
    if (window_)    { SDL_DestroyWindow(window_);     window_   = nullptr; }
    TTF_Quit();
    SDL_Quit();
}

/* ---------- Private helpers ---------- */

void SDLRenderer::renderPresent() {
    SDL_RenderPresent(renderer_);
}

void SDLRenderer::drawRect(int x, int y, int w, int h, SDL_Color color, bool filled) {
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    if (filled) SDL_RenderFillRect(renderer_, &rect);
    else        SDL_RenderDrawRect(renderer_, &rect);
}

int SDLRenderer::drawText(const std::string& text, int x, int y,
                          SDL_Color color, TTF_Font* font) {
    if (!font || text.empty()) return 0;
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surface) return 0;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    int w = surface->w, h = surface->h;
    SDL_FreeSurface(surface); // Free the surface after creating the texture
    if (!texture) return 0;
    SDL_Rect dst = {x, y, w, h};
    SDL_RenderCopy(renderer_, texture, nullptr, &dst); // nullptr -> render entire texture
    SDL_DestroyTexture(texture);
    return w;
}

void SDLRenderer::drawTextCentered(const std::string& text, int x, int y,
                                   int maxW, SDL_Color color, TTF_Font* font) {
    if (!font || text.empty()) return;
    int tw = 0, th = 0;
    TTF_SizeUTF8(font, text.c_str(), &tw, &th);
    drawText(text, x + (maxW - tw) / 2, y, color, font);
}

SDLRenderer::BoardLayout SDLRenderer::calcLayout(int size) const {
    BoardLayout L;
    int boardAreaW = screenW_ - SIDEBAR_W - BOARD_PAD * 2;
    int boardAreaH = screenH_ - STATUS_H  - BOARD_PAD * 2;
    L.cellSize = std::min(boardAreaW, boardAreaH) / size;
    L.boardW   = L.cellSize * size;
    L.boardH   = L.cellSize * size;
    L.offsetX  = BOARD_PAD + (boardAreaW - L.boardW) / 2;
    L.offsetY  = BOARD_PAD + (boardAreaH - L.boardH) / 2;
    return L;
}

/**
 * Mô tả: Vẽ board từ cache.
 *        Được gọi bởi mọi hàm render để đảm bảo board luôn hiển thị.
 *        Thứ tự vẽ: nền -> highlight nước cuối -> lưới -> quân cờ.
 */
void SDLRenderer::drawBoard() {
    if (!boardInitialized_) return;

    BoardLayout L = calcLayout(boardSize_);

    // nền board
    drawRect(L.offsetX, L.offsetY, L.boardW, L.boardH, COLOR_PANEL, true);

    // highlight nước đi cuối cùng
    if (lastRow_ >= 0 && lastCol_ >= 0) {
        int x = L.offsetX + lastCol_ * L.cellSize + 2;
        int y = L.offsetY + lastRow_ * L.cellSize + 2;
        drawRect(x, y, L.cellSize - 4, L.cellSize - 4, COLOR_LAST_MOVE, true);
    }

    // lưới
    SDL_SetRenderDrawColor(renderer_,
        COLOR_GRID.r, COLOR_GRID.g, COLOR_GRID.b, COLOR_GRID.a);
    for (int i = 0; i <= boardSize_; i++) {
        SDL_RenderDrawLine(renderer_,
            L.offsetX,            L.offsetY + i * L.cellSize,
            L.offsetX + L.boardW, L.offsetY + i * L.cellSize);
        SDL_RenderDrawLine(renderer_,
            L.offsetX + i * L.cellSize, L.offsetY,
            L.offsetX + i * L.cellSize, L.offsetY + L.boardH);
    }

    // quân cờ X / O
    int pad = L.cellSize / 5;
    for (int r = 0; r < boardSize_; r++) {
        for (int c = 0; c < boardSize_; c++) {
            char sym = cachedBoard_[r][c];
            if (sym == '\0' || sym == ' ') continue;

            int cx = L.offsetX + c * L.cellSize + L.cellSize / 2; // c for center
            int cy = L.offsetY + r * L.cellSize + L.cellSize / 2;
            int x1 = L.offsetX + c * L.cellSize + pad; // x1, y1, x2, y2 define the bounding coordinates used to draw the X symbol inside a board cell
            int y1 = L.offsetY + r * L.cellSize + pad;
            int x2 = x1 + L.cellSize - pad * 2;
            int y2 = y1 + L.cellSize - pad * 2;

            if (sym == 'X') {
                SDL_SetRenderDrawColor(renderer_,
                    COLOR_X.r, COLOR_X.g, COLOR_X.b, COLOR_X.a);
                for (int t = -2; t <= 2; t++) {
                    SDL_RenderDrawLine(renderer_, x1 + t, y1, x2 + t, y2);
                    SDL_RenderDrawLine(renderer_, x2 + t, y1, x1 + t, y2);
                }
            } else if (sym == 'O') {
                SDL_SetRenderDrawColor(renderer_,
                    COLOR_O.r, COLOR_O.g, COLOR_O.b, COLOR_O.a);
                int radius = (L.cellSize / 2) - pad;
                drawThickCircle(renderer_, cx, cy, radius, 3);
            }
        }
    }
}





void SDLRenderer::drawPiecesOnly() {
    if (!boardInitialized_) return;

    BoardLayout L = calcLayout(boardSize_);

    // quân cờ X / O
    int pad = L.cellSize / 5;
    for (int r = 0; r < boardSize_; r++) {
        for (int c = 0; c < boardSize_; c++) {
            char sym = cachedBoard_[r][c];
            if (sym == '\0' || sym == ' ') continue;

            int cx = L.offsetX + c * L.cellSize + L.cellSize / 2; // c for center
            int cy = L.offsetY + r * L.cellSize + L.cellSize / 2;
            int x1 = L.offsetX + c * L.cellSize + pad; // x1, y1, x2, y2 define the bounding coordinates used to draw the X symbol inside a board cell
            int y1 = L.offsetY + r * L.cellSize + pad;
            int x2 = x1 + L.cellSize - pad * 2;
            int y2 = y1 + L.cellSize - pad * 2;

            if (sym == 'X') {
                SDL_SetRenderDrawColor(renderer_,
                    COLOR_X.r, COLOR_X.g, COLOR_X.b, COLOR_X.a);
                for (int t = -2; t <= 2; t++) {
                    SDL_RenderDrawLine(renderer_, x1 + t, y1, x2 + t, y2);
                    SDL_RenderDrawLine(renderer_, x2 + t, y1, x1 + t, y2);
                }
            } else if (sym == 'O') {
                SDL_SetRenderDrawColor(renderer_,
                    COLOR_O.r, COLOR_O.g, COLOR_O.b, COLOR_O.a);
                int radius = (L.cellSize / 2) - pad;
                drawThickCircle(renderer_, cx, cy, radius, 3);
            }
        }
    }
}


void SDLRenderer::drawSidebar() {
    int sx = screenW_ - SIDEBAR_W;
    drawRect(sx, 0, SIDEBAR_W, screenH_, COLOR_PANEL, true);

    std::string symbol   = (currentPlayer_ == 0) ? "[ X ]" : "[ O ]";
    std::string turnText = currentIsBot_
        ? "Bot's turn"
        : ("Player " + std::to_string(currentPlayer_ + 1) + "'s turn");
    SDL_Color col = (currentPlayer_ == 0) ? COLOR_X : COLOR_O;

    drawTextCentered("CURRENT TURN", sx, 75, SIDEBAR_W, COLOR_TEXT_DIM,  fontSmall_);
    drawTextCentered(symbol,         sx, 110,  SIDEBAR_W, col,             fontLarge_);
    drawTextCentered(turnText,       sx, 150, SIDEBAR_W, COLOR_TEXT_MAIN, fontMed_);
}

/* ---------- clearScreen ---------- */

void SDLRenderer::clearScreen() {
    SDL_SetRenderDrawColor(renderer_,
        COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, COLOR_BG.a);
    SDL_RenderClear(renderer_);
}

/* ---------- displayBoard ---------- */

/**
 * Mô tả: lưu trạng thái board - không render trực tiếp.
 *        Việc render thực sự xảy ra trong showPlayer() cùng một frame.
 */
void SDLRenderer::displayBoard(const char board[][BOARD_N_MAX], const int size) {
    boardSize_        = size;
    boardInitialized_ = true;

    for (int r = 0; r < size; r++)
        for (int c = 0; c < size; c++)
            cachedBoard_[r][c] = board[r][c];

    // không gọi renderPresent() ở đây
    // showPlayer() sẽ vẽ toàn bộ frame bao gồm board - cập nhật toàn bộ rồi render một lần (tránh thiếu đồng bộ)
}

/* ---------- showMove ---------- */

/**
 * Mô tả: Lưu vị trí nước đi cuối để highlight.
 *        Không render trực tiếp - showPlayer() sẽ flush frame.
 */
void SDLRenderer::showMove(const int row, const int col) {
    lastRow_ = row;
    lastCol_ = col; // These belong to the SDLRenderer object itself, they persist after the function finishes
}

/* ---------- showPlayer ---------- */

/**
 * Mô tả: Render toàn bộ frame: nền → board → sidebar → present.
 *        Đây là hàm duy nhất gọi renderPresent() trong game loop.
 */
void SDLRenderer::showPlayer(const int player, const bool is_bot) {
    clearScreen();
    drawBoard();  // board luôn là lớp nền

    currentPlayer_ = player;
    currentIsBot_  = is_bot;
    // sidebar
    drawSidebar();

    renderPresent();  // flush toàn bộ frame một lần duy nhất
}

/* ---------- showResult ---------- */

void SDLRenderer::showResult(const int winner, const bool is_bot,
                             const WinLine* winLine) {
    clearScreen();
    drawBoard();  // board vẫn hiển thị phía sau overlay
    drawSidebar();

    // highlight đường thắng
    if (winLine && !winLine->cells.empty()) {
        BoardLayout L = calcLayout(boardSize_);
        for (auto& [r, c] : winLine->cells) {
            int x = L.offsetX + c * L.cellSize + 2;
            int y = L.offsetY + r * L.cellSize + 2;
            drawRect(x, y, L.cellSize - 4, L.cellSize - 4, COLOR_WIN_HL, true);
        }
        // drawBoard();  // vẽ lại quân cờ lên trên highlight
        drawPiecesOnly();
        drawSidebar(); // sidebar vẫn hiển thị để không mất context người chơi
    }

    // màn che tối
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 160);
    SDL_Rect overlay = {0, 0, screenW_, screenH_};
    SDL_RenderFillRect(renderer_, &overlay);

    // banner kết quả
    int bw = 420, bh = 100;
    int bx = (screenW_ - SIDEBAR_W - bw) / 2;
    int by = (screenH_ - bh) / 2;

    drawRect(bx, by, bw, bh, COLOR_PANEL, true);
    drawRect(bx, by, bw, bh, COLOR_GRID,  false);

    std::string msg;
    SDL_Color   msgCol;
    if (winner == DRAW_RESULT) {
        msg    = "It's a Draw!";
        msgCol = COLOR_TEXT_DIM;
    } else {
        std::string who = is_bot ? "Bot" : ("Player " + std::to_string(winner + 1));
        msg    = who + " wins!";
        msgCol = (winner == 0) ? COLOR_X : COLOR_O;
    }

    drawTextCentered(msg,                    bx, by + 20, bw, msgCol,         fontLarge_);
    drawTextCentered("Press \"R\" to play again or \"Esc\" to quit", bx, by + 62, bw, COLOR_TEXT_DIM, fontSmall_);

    renderPresent();
}

/* ---------- showSelectMenu ---------- */

void SDLRenderer::showSelectMenu(SelectType selectType, int context) {
    clearScreen();

    int startY = screenH_ / 4;
    int areaW  = screenW_ - SIDEBAR_W;

    auto title = [&](const std::string& t) {
        drawTextCentered(t, 0, startY, areaW, COLOR_TEXT_MAIN, fontLarge_);
    };
    auto sub = [&](const std::string& t, int dy) {
        drawTextCentered(t, 0, startY + dy, areaW, COLOR_TEXT_DIM, fontMed_);
    };

    switch (selectType) {
        case SelectType::TITLE_UI:
            drawTextCentered("TicTacToe",
                0, screenH_ / 3, areaW, COLOR_X, fontLarge_);
            drawTextCentered("Press any key to start",
                0, screenH_ / 3 + 60, areaW, COLOR_TEXT_DIM, fontMed_);
            break;

        case SelectType::SIZE_UI:
            title("Board Size");
            sub("Enter size (" +
                std::to_string(BOARD_N_MIN) + " - " +
                std::to_string(BOARD_N_MAX) + ") then Enter:", 50);
            if (context != NO_CONTEXT)
                sub("> " + std::to_string(context), 90);
            break;

        case SelectType::GOAL_UI:
            title("Win Condition");
            sub("How many in a row to win?", 50);
            sub("(3 - " + std::to_string(GOAL_MAX) + ") then Enter:", 80);
            if (context != NO_CONTEXT)
                sub("> " + std::to_string(context), 120);
            break;

        case SelectType::GAME_MODE_UI:
            title("Game Mode");
            sub("Press:  0 = PVP   1 = PVE   2 = EVE", 50);
            break;

        case SelectType::BOT_LEVEL_UI:
            title("Bot Level");
            sub("Press:  0 = Easy   1 = Medium   2 = Hard", 50);
            if (context != NO_CONTEXT)
                sub("Setting for Player " + std::to_string(context + 1), 90);
            break;

        case SelectType::MUL_BOT_LEVEL_UI:
            title("Bot Levels (EVE)");
            sub("Press:  0 = Easy   1 = Medium   2 = Hard", 50);
            if (context != NO_CONTEXT)
                sub("Setting for Bot " + std::to_string(context + 1), 90);
            break;

        case SelectType::PLAYER_UI:
            drawBoard();
            drawSidebar();  // sidebar luôn hiển thị trong game
            {
                int barY = screenH_ - STATUS_H;
                drawRect(0, barY, screenW_ - SIDEBAR_W, STATUS_H, COLOR_PANEL, true);
                drawTextCentered("Click a cell or use arrow keys + Enter",
                                0, barY + (STATUS_H - FONT_SIZE_SMALL) / 2,
                                screenW_ - SIDEBAR_W, COLOR_TEXT_DIM, fontSmall_);
            }
            break;

        default:
            title("Select");
            break;
    }

    renderPresent();
}

/* ---------- showInvalidSelect ---------- */

void SDLRenderer::showInvalidSelect(SelectType selectType, int context) {
    // giữ nguyên nội dung màn hình, chỉ thêm thanh lỗi phía dưới
    int barH = STATUS_H;
    int barY = screenH_ - barH;
    drawRect(0, barY, screenW_ - SIDEBAR_W, barH, {60, 10, 10, 220}, true);

    std::string msg = "Invalid input";
    if (context != NO_CONTEXT)
        msg += " (" + std::to_string(context) + ")";
    msg += " — try again";

    drawTextCentered(msg, 0, barY + (barH - FONT_SIZE_MED) / 2,
                     screenW_ - SIDEBAR_W, COLOR_ERROR, fontMed_);
    renderPresent();
}

/* ---------- showValidSelect ---------- */

void SDLRenderer::showValidSelect(SelectType selectType, int context) {
    int barH = STATUS_H;
    int barY = screenH_ - barH;
    drawRect(0, barY, screenW_ - SIDEBAR_W, barH, {10, 50, 20, 220}, true);

    std::string msg = "OK";
    if (context != NO_CONTEXT)
        msg += ": " + std::to_string(context);

    drawTextCentered(msg, 0, barY + (barH - FONT_SIZE_MED) / 2,
                     screenW_ - SIDEBAR_W, COLOR_SUCCESS, fontMed_);
    renderPresent();
}

/* ---------- showInvalidMove ---------- */

void SDLRenderer::showInvalidMove() {
    clearScreen();
    drawBoard();  // board tetap visible
    drawSidebar(); // sidebar vẫn hiển thị để không mất context người chơi

    int barH = STATUS_H;
    int barY = screenH_ - barH;
    drawRect(0, barY, screenW_ - SIDEBAR_W, barH, {60, 10, 10, 220}, true);
    drawTextCentered("Invalid move — try another cell",
                     0, barY + (barH - FONT_SIZE_MED) / 2,
                     screenW_ - SIDEBAR_W, COLOR_ERROR, fontMed_);
    renderPresent();
}

/* ---------- printResult (judge mode) ---------- */

void SDLRenderer::printResult(const GameResult& gameResult) {
    if (gameResult.winner == DRAW_RESULT)
        std::cout << "DRAW\n";
    else
        std::cout << "WINNER: Player " << (gameResult.winner + 1)
                  << (gameResult.isBot ? " (Bot)" : "") << "\n";
    std::cout << "TURNS: " << gameResult.turns << "\n";
}
