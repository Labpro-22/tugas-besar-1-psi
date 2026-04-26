#ifndef PETAKAKSI_HPP
#define PETAKAKSI_HPP

#include "models/Petak.hpp"

enum class ActionType { GO, JAIL, TAX_FLAT, TAX_FIXED, FREE_PARKING, FESTIVAL };

class PetakAksi : public Petak {
private:
  ActionType type;
  int amount;

public:
  PetakAksi(int id, const std::string &name, ActionType type, int amount = 0);

  ActionType getType() const;
  int getAmount() const;

  void injak(Player &p, IGameUI &ui, int diceRoll = 0, std::vector<std::unique_ptr<Petak>>* board = nullptr, std::vector<Player>* players = nullptr) override;
};

#endif
