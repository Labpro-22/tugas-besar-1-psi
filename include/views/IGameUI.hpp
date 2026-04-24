#ifndef IGAMEUI_HPP
#define IGAMEUI_HPP

#include <string>

class IGameUI {
public:
  virtual ~IGameUI() = default;

  virtual void showMessage(const std::string &msg) = 0;
  virtual void showInlineMessage(const std::string &msg) = 0;

  virtual int promptTaxChoice(int flatAmount, int percentage,
                              int totalWealth) = 0;

  virtual std::string promptInput(const std::string &promptTxt) = 0;
};

#endif
