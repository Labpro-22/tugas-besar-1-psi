#ifndef DICE_HPP
#define DICE_HPP

namespace Entities {

    class Dice {
    private:
        int d1;
        int d2;
        int doubleCount;

    public:
        Dice();
        
        void roll();
        
        int getD1() const;
        int getD2() const;
        int getTotal() const;
        
        bool isDouble() const;
        
        int getDoubleCount() const;
        void resetDoubleCount();
        void incrementDoubleCount();
    };

}

#endif
