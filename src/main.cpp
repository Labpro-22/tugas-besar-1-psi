#include <iostream>
#include <cstdlib>
#include <ctime>
#include "../Entities/Dice.hpp"
#include "../Entities/Player.hpp"
#include "../Exceptions/InsufficientFundsException.hpp"

int main() {
    std::srand(std::time(nullptr));

    Entities::Dice dadu;
    Entities::Player p1(1, "Alice", 1500);
    Entities::Player p2(2, "Bob", 200);

    dadu.roll();
    std::cout << "Dadu: " << dadu.getD1() << " + " << dadu.getD2() << " = " << dadu.getTotal() << "\n";
    std::cout << "Double: " << (dadu.isDouble() ? "Ya" : "Tidak") << "\n\n";

    std::cout << p1.getNama() << " saldo: " << p1.getUang() << "\n";
    std::cout << p2.getNama() << " saldo: " << p2.getUang() << "\n\n";

    try {
        long long tagihan = 500;
        std::cout << p2.getNama() << " bayar: " << tagihan << "\n";
        p2.kurangiUang(tagihan);
    } catch (const Exceptions::InsufficientFundsException& e) {
        std::cout << "[Exception] " << e.what() << "\n";
        p2.setStatus(Entities::PlayerStatus::BANKRUPT);
    }

    return 0;
}
