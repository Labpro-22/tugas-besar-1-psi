#include "Player.hpp"
#include "../Exceptions/InsufficientFundsException.hpp"

namespace Entities {

    Player::Player(int id, const std::string& nama, long long uangAwal) {
        this->id = id;
        this->nama = nama;
        this->uang = uangAwal;
        this->posisi = 0;
        this->status = PlayerStatus::ACTIVE;
    }

    int Player::getId() const { return id; }
    
    std::string Player::getNama() const { return nama; }
    
    long long Player::getUang() const { return uang; }
    
    int Player::getPosisi() const { return posisi; }
    
    PlayerStatus Player::getStatus() const { return status; }
    
    const std::vector<GameLogic::PetakProperti*>& Player::getAset() const { return aset; }

    void Player::setPosisi(int lokasiBaru) { 
        posisi = lokasiBaru; 
    }
    
    void Player::setStatus(PlayerStatus stat) { 
        status = stat; 
    }

    void Player::tambahUang(long long jumlah) {
        if (jumlah > 0) {
            uang += jumlah;
        }
    }

    void Player::kurangiUang(long long jumlah) {
        if (uang - jumlah < 0) {
            std::string msg = "Pemain " + nama + " tidak memiliki saldo yang cukup! Saldo saat ini: " 
                              + std::to_string(uang) + ", Kekurangan: " + std::to_string(jumlah - uang);
            throw Exceptions::InsufficientFundsException(msg);
        }
        uang -= jumlah;
    }

    void Player::tambahAset(GameLogic::PetakProperti* properti) {
        if (properti != nullptr) {
            aset.push_back(properti);
        }
    }

}
