#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <vector>

namespace GameLogic {
    class PetakProperti;
}

namespace Entities {

    enum class PlayerStatus {
        ACTIVE,
        BANKRUPT,
        JAILED
    };

    class Player {
    private:
        int id;
        std::string nama;
        long long uang;
        int posisi;
        PlayerStatus status;
        std::vector<GameLogic::PetakProperti*> aset;

    public:
        Player(int id, const std::string& nama, long long uangAwal);
        
        int getId() const;
        std::string getNama() const;
        long long getUang() const;
        int getPosisi() const;
        PlayerStatus getStatus() const;
        const std::vector<GameLogic::PetakProperti*>& getAset() const;

        void setPosisi(int lokasiBaru);
        void setStatus(PlayerStatus stat);
        
        void tambahUang(long long jumlah);
        void kurangiUang(long long jumlah); 
        
        void tambahAset(GameLogic::PetakProperti* properti);
    };

}

#endif
