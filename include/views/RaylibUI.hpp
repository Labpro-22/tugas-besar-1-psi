#ifndef RAYLIBUI_HPP
#define RAYLIBUI_HPP

#ifdef USE_RAYLIB

#include "views/IGameUI.hpp"
#include <deque>
#include <string>

class GameManager;

class RaylibUI : public IGameUI {
public:
  explicit RaylibUI(const GameManager *gm);
  ~RaylibUI();

  // IGameUI interface
  void        showMessage(const std::string &msg) override;
  void        showInlineMessage(const std::string &msg) override;
  int         promptTaxChoice(int flat, int pct, int totalWealth) override;
  std::string promptInput(const std::string &prompt) override;

private:
  const GameManager *gm_;

  std::deque<std::string> log_;
  std::string             inputBuf_;
  std::string             curPrompt_;

  static constexpr int SCR_W  = 1400;
  static constexpr int SCR_H  = 840;
  static constexpr int CELL   = 60;
  static constexpr int COLS   = 11;
  static constexpr int BD_X   = 10;
  static constexpr int BD_Y   = 60;
  static constexpr int BD_SZ  = CELL * COLS;

  static constexpr int RP_X   = BD_X + BD_SZ + 18;
  static constexpr int RP_W   = SCR_W - RP_X - 8;

  static constexpr int PL_H   = 48;
  static constexpr int PL_PAD = 6;
  static constexpr int LOG_Y  = BD_Y + 4 * (PL_H + PL_PAD) + 14;
  static constexpr int INPUT_H = 56;

  void drawFrame();
  void drawHeader();
  void drawBoard();
  void drawCell(int boardIdx, int col, int row);
  void drawPlayerPanels();
  void drawLog();
  void drawInputBar();

  static std::pair<int, int> idxToGrid(int idx);
};

#endif // USE_RAYLIB
#endif // RAYLIBUI_HPP
