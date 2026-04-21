#ifndef PETAKPROPERTI_HPP
#define PETAKPROPERTI_HPP

#include "models/Petak.hpp"

class Player;

class PetakProperti : public Petak {
protected:
    Player* owner;
    int hargaBeli;
    bool isMortgaged;

public:
    PetakProperti(int id, const std::string& name, int hargaBeli);
    virtual ~PetakProperti() = default;

    Player* getOwner() const;
    void setOwner(Player* newOwner);

    int getHargaBeli() const;
    
    bool getIsMortgaged() const;
    void setMortgaged(bool status);

    virtual int getSewa(int diceRoll = 0) const = 0;
    
    void injak(Player& p) override;
};

#endif
