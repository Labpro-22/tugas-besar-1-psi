#ifndef GAMEUI_HPP
#define GAMEUI_HPP

#include "views/IGameUI.hpp"

class GameUI : public IGameUI {
public:
  void showMessage(const std::string &msg) override;
  void showInlineMessage(const std::string &msg) override;

  int promptTaxChoice(int flatAmount, int percentage, int totalWealth) override;

  std::string promptInput(const std::string &promptTxt) override;
};

#endif
