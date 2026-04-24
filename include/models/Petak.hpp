#ifndef PETAK_HPP
#define PETAK_HPP

#include <string>

class Player;
class IGameUI;

class Petak {
protected:
  int id;
  std::string name;
  std::string shortName;
  std::string colorCode;

public:
  Petak(int id, const std::string &name);
  virtual ~Petak() = default;

  int getId() const;
  std::string getName() const;

  void setSkin(const std::string &sName, const std::string &cCode);
  std::string getShortName() const;
  std::string getColorCode() const;

  virtual void injak(Player &p, IGameUI &ui, int diceRoll = 0) = 0;
};

#endif
