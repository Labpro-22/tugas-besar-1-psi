#ifndef STREET_HPP
#define STREET_HPP

#include "models/PetakProperti.hpp"
#include <vector>

class Street : public PetakProperti {
private:
    std::vector<int> rentPrices;
    int houseCount;
    int housePrice;

public:
    Street(int id, const std::string& name, int hargaBeli, const std::vector<int>& rentPrices, int housePrice);
    
    int getHouseCount() const;
    void addHouse();
    int getHousePrice() const;

    int getSewa(int diceRoll = 0) const override;
};

#endif
