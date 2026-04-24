#ifndef PETAKPROPERTI_HPP
#define PETAKPROPERTI_HPP

#include "models/Petak.hpp"

class Player;

class PetakProperti : public Petak {
protected:
  Player *owner;
  int hargaBeli;
  bool isMortgaged;

public:
  PetakProperti(int id, const std::string &name, int hargaBeli);
  virtual ~PetakProperti() = default;

  Player *getOwner() const;
  void setOwner(Player *newOwner);

  int getHargaBeli() const;

  bool getIsMortgaged() const;
  void setMortgaged(bool status);

  virtual void gadai();
  virtual void tebus();
  virtual void lelang();

  virtual int getSewa(int diceRoll = 0) const = 0;

  virtual std::string getGroupName() const = 0;
  virtual int getTotalWorth() const = 0;
  virtual void printDeed(IGameUI &ui) const = 0;

  void injak(Player &p, IGameUI &ui, int diceRoll = 0) override;
};

#endif
