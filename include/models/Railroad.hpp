#ifndef RAILROAD_HPP
#define RAILROAD_HPP

#include "models/PetakProperti.hpp"

class Railroad : public PetakProperti {
private:
    int baseRent;

public:
    Railroad(int id, const std::string& name, int hargaBeli, int baseRent);
    
    int getSewa(int diceRoll = 0) const override;
};

#endif
