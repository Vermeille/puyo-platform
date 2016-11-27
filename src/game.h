#pragma once

#include "grid.h"

class Game {
   public:
    Game& operator=(const Game&) = default;
    Game() : down_cooldown_(3) {}

    void AddRocks(int amount) { pending_rocks_ += amount; }

    int GameMakeTurn(const std::string& cmd) {
        auto state = ApplyMove(cmd);

        --down_cooldown_;
        if (state == PuyoGrid::State::PlayerMove && down_cooldown_ == 0) {
            state = g_.Move(PuyoGrid::Direction::Down);
            down_cooldown_ = 3;
        }

        if (state == PuyoGrid::State::ProcessCollisions) {
            down_cooldown_ = 3;
            return ProcessRocksAndPhysics();
        }
        return 0;
    }

    bool HasLost() const { return g_.HasLost(); }

    std::string PrintGame() const;

    std::string PrintGameHTML() const;

   private:
    PuyoGrid::State ApplyMove(const std::string& cmd);
    int ProcessRocksAndPhysics();

    PuyoGrid g_;
    int down_cooldown_;
    int pending_rocks_ = 0;
};
