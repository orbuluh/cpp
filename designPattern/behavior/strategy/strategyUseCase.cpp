#include "strategyUseCase.h"

namespace strategy {

const char* bossName(Boss t) {
    switch (t) {
        case Boss::MagmaDragoon: return "MagmaDragoon";
        case Boss::FrostWalrus: return "FrostWalrus";
        default: return "Zero"; // heuheu
    }
}



void demo() {
    MagaManX4Context x;
    Boss curBoss = Boss::MagmaDragoon;
    x.attack(curBoss);
    x.switchWeapon();
    x.attack(curBoss);
    x.beatBoss(curBoss);
    curBoss = Boss::FrostWalrus;
    x.attack(curBoss);
    x.switchWeapon();
    x.attack(curBoss);
}
} // namespace strategy