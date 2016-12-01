#pragma once

#include <chrono>
#include <iostream>

#include "grid.h"

class Game {
   public:
    Game& operator=(const Game&) = default;
    Game()
        : down_cooldown_(3),
          last_action_time_(std::chrono::system_clock::now()),
          banned_(false) {}

    void AddRocks(int amount) { pending_rocks_ += amount; }

    void RefreshBanTimeout() {
        auto now = std::chrono::system_clock::now();
        if (now - last_action_time_ < std::chrono::milliseconds(750)) {
            banned_ = true;
        }
        last_action_time_ = now;
    }

    int GameMakeTurn(const std::string& cmd) {
        RefreshBanTimeout();
        if (banned_) {
            return 0;
        }

        auto state = ApplyMove(cmd);

        --down_cooldown_;
        if (state == PuyoGrid::State::PlayerMove && down_cooldown_ == 0) {
            state = g_.Move(PuyoGrid::Direction::Down);
            down_cooldown_ = 3;
        }

        if (state == PuyoGrid::State::ProcessCollisions) {
            down_cooldown_ = 3;
            g_.AddPuyos();
            return ProcessRocksAndPhysics();
        }
        return 0;
    }

    bool HasLost() const {
        if (banned_) {
            std::cerr << "banned\n";
        }
        if (g_.HasLost()) {
            std::cerr << "game failed\n";
        }
        return banned_ || g_.HasLost() ||
               (std::chrono::system_clock::now() - last_action_time_ >
                std::chrono::minutes(5));
    }

    std::string PrintGame() const;

    std::string PrintGameHTML() const;

   private:
    PuyoGrid::State ApplyMove(const std::string& cmd);
    int ProcessRocksAndPhysics();

    PuyoGrid g_;
    int down_cooldown_;
    int pending_rocks_ = 0;
    std::chrono::system_clock::time_point last_action_time_;
    bool banned_;
};
