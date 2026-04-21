#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "models/PetakProperti.hpp"

class Utility : public PetakProperti {
public:
    Utility(int id, const std::string& name, int hargaBeli);
    
    int getSewa(int diceRoll) const override;
};

#endif
