#ifndef DICE_HPP
#define DICE_HPP

#include <utility>
#include <random>

class Dice {
private:
    int lastRoll1;
    int lastRoll2;
    std::mt19937 generator;

public:
    Dice();
    
    int roll();
    
    int getTotal() const;
    
    bool isDouble() const;
    
    std::pair<int, int> getLastRoll() const;
};

#endif
