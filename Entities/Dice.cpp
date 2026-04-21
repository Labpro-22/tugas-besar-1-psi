#include "Dice.hpp"
#include <cstdlib>

namespace Entities {

    Dice::Dice() : d1(0), d2(0), doubleCount(0) {}

    void Dice::roll() {
        d1 = (std::rand() % 6) + 1;
        d2 = (std::rand() % 6) + 1;
    }

    int Dice::getD1() const {
        return d1;
    }

    int Dice::getD2() const {
        return d2;
    }

    int Dice::getTotal() const {
        return d1 + d2;
    }

    bool Dice::isDouble() const {
        return (d1 == d2) && (d1 != 0);
    }

    int Dice::getDoubleCount() const {
        return doubleCount;
    }

    void Dice::resetDoubleCount() {
        doubleCount = 0;
    }

    void Dice::incrementDoubleCount() {
        doubleCount++;
    }

}
