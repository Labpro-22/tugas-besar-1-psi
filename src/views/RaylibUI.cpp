#include "views/RaylibUI.hpp"

#ifdef USE_RAYLIB

#include "core/GameManager.hpp"
#include "models/PetakProperti.hpp"
#include "models/Player.hpp"
#include "models/Street.hpp"
#include "utils/FormatHelper.hpp"

#include <raylib.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
//  Color helpers
// ─────────────────────────────────────────────────────────────────────────────

static Color cellBg(const std::string &code) {
  if (code == "CK") return {101,  67,  33, 255}; // coklat
  if (code == "BM") return {100, 200, 240, 255}; // biru muda
  if (code == "PK") return {255, 160, 200, 255}; // pink
  if (code == "OR") return {255, 140,   0, 255}; // orange
  if (code == "MR") return {210,  30,  30, 255}; // merah
  if (code == "KN") return {240, 210,   0, 255}; // kuning
  if (code == "HJ") return { 40, 180,  40, 255}; // hijau
  if (code == "BT") return {  0,  40, 180, 255}; // biru tua
  if (code == "AB") return {160, 160, 160, 255}; // abu-abu (utility)
  return                   {210, 210, 215, 255}; // DF (action)
}

static Color cellFg(const std::string &code) {
  // dark backgrounds → white text
  if (code == "CK" || code == "MR" || code == "BT") return WHITE;
  return BLACK;
}

static Color playerColor(int id) {
  if (id == 1) return {220,  40,  40, 255}; // red
  if (id == 2) return { 40,  80, 220, 255}; // blue
  if (id == 3) return { 20, 160,  20, 255}; // green
  return             {200, 130,   0, 255};  // orange
}

// Wrap long text into lines that fit within maxWidth pixels at given fontSize
static std::vector<std::string> wrapText(const std::string &text,
                                         int maxWidth, int fontSize) {
  std::vector<std::string> lines;
  std::istringstream ss(text);
  std::string word, cur;
  while (ss >> word) {
    std::string test = cur.empty() ? word : cur + " " + word;
    if (MeasureText(test.c_str(), fontSize) > maxWidth && !cur.empty()) {
      lines.push_back(cur);
      cur = word;
    } else {
      cur = test;
    }
  }
  if (!cur.empty()) lines.push_back(cur);
  if (lines.empty()) lines.push_back("");
  return lines;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Board index → (col, row) in an 11×11 grid
// ─────────────────────────────────────────────────────────────────────────────
std::pair<int, int> RaylibUI::idxToGrid(int idx) {
  if (idx >= 20 && idx <= 30) return {idx - 20, 0};      // top row
  if (idx >= 31 && idx <= 39) return {10, idx - 30};     // right col
  if (idx >= 0  && idx <= 10) return {10 - idx, 10};     // bottom row
  if (idx >= 11 && idx <= 19) return {0, 20 - idx};      // left col
  return {-1, -1};
}

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────────────────────────────────────────
RaylibUI::RaylibUI(const GameManager *gm) : gm_(gm) {
  SetTraceLogLevel(LOG_WARNING);
  InitWindow(SCR_W, SCR_H, "Nimonspoli");
  SetTargetFPS(60);
}

RaylibUI::~RaylibUI() {
  if (!WindowShouldClose() && IsWindowReady()) {
    showMessage("");
    showMessage("[Permainan selesai. Tekan Enter atau tutup jendela untuk keluar.]");
    curPrompt_ = "";
    inputBuf_  = "";
    while (!WindowShouldClose()) {
      if (IsKeyPressed(KEY_ENTER)) break;
      BeginDrawing();
      drawFrame();
      EndDrawing();
    }
  }
  CloseWindow();
}

// ─────────────────────────────────────────────────────────────────────────────
//  IGameUI implementation
// ─────────────────────────────────────────────────────────────────────────────
void RaylibUI::showMessage(const std::string &msg) {
  // Split on '\n' so multi-line messages are stored as separate entries
  std::istringstream ss(msg);
  std::string line;
  while (std::getline(ss, line)) {
    log_.push_back(line);
    if (log_.size() > 300) log_.pop_front();
  }
}

void RaylibUI::showInlineMessage(const std::string &msg) {
  // Append to last log line (inline, no newline)
  if (!log_.empty())
    log_.back() += msg;
  else
    log_.push_back(msg);
}

int RaylibUI::promptTaxChoice(int flat, int pct, int /*totalWealth*/) {
  showMessage("Pilih opsi pembayaran pajak:");
  showMessage("1. Bayar flat " + formatUang(flat));
  showMessage("2. Bayar " + std::to_string(pct) + "% dari total kekayaan");
  showMessage("(Pilih sebelum menghitung kekayaan!)");
  while (true) {
    std::string s = promptInput("Pilihan (1/2): ");
    if (s == "1" || s == "2") return std::stoi(s);
    showMessage("[!] Masukkan 1 atau 2.");
  }
}

std::string RaylibUI::promptInput(const std::string &prompt) {
  curPrompt_ = prompt;
  inputBuf_.clear();

  while (!WindowShouldClose()) {
    // Collect typed characters this frame
    int ch = GetCharPressed();
    while (ch > 0) {
      if (ch >= 32 && ch < 127)
        inputBuf_ += static_cast<char>(ch);
      ch = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && !inputBuf_.empty())
      inputBuf_.pop_back();
    if (IsKeyPressed(KEY_ENTER)) {
      std::string result = inputBuf_;
      inputBuf_.clear();
      curPrompt_.clear();
      // Echo to log so the conversation is visible
      showMessage(prompt + result);
      return result;
    }

    BeginDrawing();
    drawFrame();
    EndDrawing();
  }
  return "EXIT";
}

// ─────────────────────────────────────────────────────────────────────────────
//  Top-level render
// ─────────────────────────────────────────────────────────────────────────────
void RaylibUI::drawFrame() {
  ClearBackground({20, 20, 30, 255});
  drawHeader();
  drawBoard();
  drawPlayerPanels();
  drawLog();
  drawInputBar();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Header bar
// ─────────────────────────────────────────────────────────────────────────────
void RaylibUI::drawHeader() {
  DrawRectangle(0, 0, SCR_W, 52, {30, 30, 50, 255});
  DrawText("NIMONSPOLI", 14, 12, 26, GOLD);

  if (gm_) {
    int turn    = gm_->getTurnCount();
    int maxTurn = gm_->getMaxTurn();
    int curIdx  = gm_->getCurrentPlayerIndex();

    std::string turnStr = "TURN " + std::to_string(turn);
    if (maxTurn > 0) turnStr += " / " + std::to_string(maxTurn);
    DrawText(turnStr.c_str(), SCR_W / 2 - MeasureText(turnStr.c_str(), 18) / 2,
             16, 18, LIGHTGRAY);

    const auto &players = gm_->getPlayers();
    if (curIdx >= 0 && curIdx < static_cast<int>(players.size())) {
      std::string active = "Giliran: " + players[curIdx].getName();
      DrawText(active.c_str(), SCR_W - MeasureText(active.c_str(), 16) - 14,
               18, 16, YELLOW);
    }
  }
  DrawLine(0, 52, SCR_W, 52, {60, 60, 80, 255});
}

// ─────────────────────────────────────────────────────────────────────────────
//  Board
// ─────────────────────────────────────────────────────────────────────────────
void RaylibUI::drawBoard() {
  // Center green felt background
  int innerX = BD_X + CELL;
  int innerY = BD_Y + CELL;
  int innerSz = CELL * 9;
  DrawRectangle(innerX, innerY, innerSz, innerSz, {30, 100, 50, 255});

  // Center label
  const char *logo = "NIMONSPOLI";
  int lw = MeasureText(logo, 22);
  DrawText(logo, innerX + (innerSz - lw) / 2, innerY + innerSz / 2 - 11,
           22, {180, 255, 180, 200});

  // Draw all 40 cells
  if (!gm_ || gm_->getBoard().empty()) return;
  for (int idx = 0; idx < static_cast<int>(gm_->getBoard().size()); ++idx) {
    auto [col, row] = idxToGrid(idx);
    if (col < 0) continue;
    drawCell(idx, col, row);
  }
}

void RaylibUI::drawCell(int idx, int col, int row) {
  const auto &board = gm_->getBoard();
  if (idx < 0 || idx >= static_cast<int>(board.size())) return;

  const Petak *petak = board[idx].get();
  int x = BD_X + col * CELL;
  int y = BD_Y + row * CELL;

  Color bg  = cellBg(petak->getColorCode());
  Color fg  = cellFg(petak->getColorCode());

  // Cell background + border
  DrawRectangle(x, y, CELL, CELL, bg);
  DrawRectangleLines(x, y, CELL, CELL, {0, 0, 0, 200});

  // Short name (centered, top area)
  const char *name = petak->getShortName().c_str();
  int nw = MeasureText(name, 9);
  DrawText(name, x + (CELL - nw) / 2, y + 4, 9, fg);

  // Owner + buildings (bottom-left)
  if (auto *prop = dynamic_cast<const PetakProperti *>(petak)) {
    if (const Player *owner = prop->getOwner()) {
      std::string info = "P" + std::to_string(owner->getId());
      if (auto *s = dynamic_cast<const Street *>(prop)) {
        if (s->isHotelBuilt())     info += "*";
        else for (int i = 0; i < s->getHouseCount(); ++i) info += "^";
      }
      DrawText(info.c_str(), x + 2, y + CELL - 13, 8,
               playerColor(owner->getId()));
    }
  }

  // Player tokens — small coloured circles with id
  const auto &players = gm_->getPlayers();
  int slot = 0;
  for (const auto &pl : players) {
    if (pl.getPosition() != idx) continue;
    int tx = x + 2 + slot * 15;
    int ty = y + CELL / 2 - 6;
    if (tx + 12 > x + CELL - 1) tx = x + CELL - 14;
    DrawCircle(tx + 6, ty + 6, 6, playerColor(pl.getId()));
    char pid[2] = {static_cast<char>('0' + pl.getId()), 0};
    DrawText(pid, tx + 3, ty + 1, 8, WHITE);
    ++slot;
  }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Player info panels (right panel, top)
// ─────────────────────────────────────────────────────────────────────────────
void RaylibUI::drawPlayerPanels() {
  if (!gm_ || gm_->getPlayers().empty()) return;
  const auto &players   = gm_->getPlayers();
  int         curIdx    = gm_->getCurrentPlayerIndex();

  for (int i = 0; i < static_cast<int>(players.size()); ++i) {
    const Player &pl = players[i];
    int px = RP_X;
    int py = BD_Y + i * (PL_H + PL_PAD);
    int pw = RP_W;

    bool isActive = (i == curIdx &&
                     pl.getStatus() != PlayerStatus::BANKRUPT);
    Color panelBg = isActive ? Color{60, 60, 90, 255}
                             : Color{40, 40, 55, 255};

    DrawRectangle(px, py, pw, PL_H, panelBg);
    // Colored left accent
    DrawRectangle(px, py, 5, PL_H, playerColor(pl.getId()));

    if (isActive)
      DrawRectangleLines(px, py, pw, PL_H, YELLOW);

    // Player name + money
    std::string nameStr = "P" + std::to_string(pl.getId()) +
                          "  " + pl.getName();
    DrawText(nameStr.c_str(), px + 10, py + 5, 14, WHITE);

    std::string moneyStr = formatUang(pl.getMoney());
    DrawText(moneyStr.c_str(),
             px + pw - MeasureText(moneyStr.c_str(), 14) - 8,
             py + 5, 14, GREEN);

    // Status + cards in hand
    std::string status;
    switch (pl.getStatus()) {
      case PlayerStatus::ACTIVE:   status = "AKTIF";    break;
      case PlayerStatus::IN_JAIL:  status = "PENJARA";  break;
      case PlayerStatus::BANKRUPT: status = "BANGKRUT"; break;
    }
    Color stCol = (pl.getStatus() == PlayerStatus::BANKRUPT) ? RED
                : (pl.getStatus() == PlayerStatus::IN_JAIL)  ? ORANGE
                                                             : LIGHTGRAY;
    DrawText(status.c_str(), px + 10, py + 24, 11, stCol);

    // Cards in hand (abbreviated)
    std::string hand = "Kartu: ";
    for (int c = 0; c < pl.getHandSize(); ++c) {
      if (c > 0) hand += ", ";
      hand += pl.getHand()[c]->getTypeName().substr(0, 4);
    }
    if (pl.getHandSize() == 0) hand += "-";
    DrawText(hand.c_str(), px + 80, py + 24, 11, LIGHTGRAY);
  }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Scrollable message log
// ─────────────────────────────────────────────────────────────────────────────
void RaylibUI::drawLog() {
  constexpr int FS   = 13;  // font size
  constexpr int LH   = 16;  // line height

  int logH = SCR_H - LOG_Y - INPUT_H - 12;
  int linesVisible = logH / LH;

  // Log background
  DrawRectangle(RP_X, LOG_Y, RP_W, logH, {28, 28, 38, 255});
  DrawRectangleLines(RP_X, LOG_Y, RP_W, logH, {55, 55, 75, 255});

  // Expand each stored line into display lines (word-wrap)
  std::vector<std::string> display;
  for (const auto &entry : log_) {
    auto wrapped = wrapText(entry, RP_W - 8, FS);
    for (auto &l : wrapped)
      display.push_back(l);
  }

  int start = std::max(0, static_cast<int>(display.size()) - linesVisible);
  int dy    = LOG_Y + 4;

  for (int i = start; i < static_cast<int>(display.size()); ++i) {
    // Alternate row tint for readability
    if ((i % 2) == 0)
      DrawRectangle(RP_X + 1, dy, RP_W - 2, LH, {33, 33, 45, 255});

    Color tc = LIGHTGRAY;
    if (!display[i].empty() && display[i][0] == '[')
      tc = YELLOW; // highlight warnings/errors
    else if (display[i].find("BANGKRUT") != std::string::npos)
      tc = RED;
    else if (display[i].find("berhasil") != std::string::npos ||
             display[i].find("menjadi milikmu") != std::string::npos)
      tc = GREEN;

    DrawText(display[i].c_str(), RP_X + 5, dy + 2, FS, tc);
    dy += LH;
    if (dy + LH > LOG_Y + logH) break;
  }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Input bar
// ─────────────────────────────────────────────────────────────────────────────
void RaylibUI::drawInputBar() {
  int iy = SCR_H - INPUT_H - 4;

  DrawRectangle(RP_X, iy, RP_W, INPUT_H, {38, 38, 55, 255});
  DrawRectangleLines(RP_X, iy, RP_W, INPUT_H, {100, 100, 150, 255});

  // Prompt label
  std::string prompt = curPrompt_.empty() ? "> " : curPrompt_;
  int pw = MeasureText(prompt.c_str(), 16);
  DrawText(prompt.c_str(), RP_X + 8, iy + 10, 16, YELLOW);

  // Input text + blinking cursor
  std::string display = inputBuf_;
  bool showCursor = (static_cast<int>(GetTime() * 2) % 2) == 0;
  if (showCursor) display += "_";

  DrawText(display.c_str(), RP_X + 8 + pw + 4, iy + 10, 16, WHITE);

  // Hint
  DrawText("Enter = konfirmasi  |  Backspace = hapus",
           RP_X + 8, iy + 34, 11, {100, 100, 130, 255});
}

#endif // USE_RAYLIB
