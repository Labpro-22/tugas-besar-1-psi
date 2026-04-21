#include "models/Dice.hpp"

Dice::Dice() : lastRoll1(0), lastRoll2(0) {
    std::random_device rd;
    generator.seed(rd());
}

int Dice::roll() {
    std::uniform_int_distribution<> dis(1, 6);
    lastRoll1 = dis(generator);
    lastRoll2 = dis(generator);
    return getTotal();
}

int Dice::getTotal() const {
    return lastRoll1 + lastRoll2;
}

bool Dice::isDouble() const {
    return lastRoll1 == lastRoll2 && lastRoll1 != 0;
}

std::pair<int, int> Dice::getLastRoll() const {
    return {lastRoll1, lastRoll2};
}
