#ifndef PETAK_HPP
#define PETAK_HPP

#include <string>

class Player;

class Petak {
protected:
    int id;
    std::string name;
    std::string shortName;
    std::string colorCode;

public:
    Petak(int id, const std::string& name);
    virtual ~Petak() = default;

    int getId() const;
    std::string getName() const;

    // Display helpers
    void setSkin(const std::string& sName, const std::string& cCode);
    std::string getShortName() const;
    std::string getColorCode() const;

    // Pure virtual method inherited by all types of Petak
    virtual void injak(Player& p, int diceRoll = 0) = 0;
};

#endif
