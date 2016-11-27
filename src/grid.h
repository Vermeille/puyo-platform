#pragma once

#include <algorithm>
#include <array>
#include <cassert>

static constexpr int GRID_LINES = 12;
static constexpr int GRID_COLS = 6;

enum class Puyo { Red, Green, Blue, Yellow, Rock, None };

template <class T>
using Grid = std::array<std::array<T, GRID_LINES>, GRID_COLS>;

char PuyoToChar(Puyo p);

class PuyoGrid {
   public:
    enum class PuyoConfig { Up, Right, Down, Left };
    enum class State { PlayerMove, ProcessCollisions };
    enum class Direction { Left, Right, Down };

    PuyoGrid& operator=(const PuyoGrid&) = default;
    PuyoGrid();

    bool HasLost() const {
        return grid_[GRID_COLS / 2][GRID_LINES - 2] != Puyo::None;
    }

    void RotateRight();
    void RotateLeft();

    Grid<char> Print() const;

    State Move(Direction dir);

    int ProcessCollisions();

    void AddRocks(int amount);

    int puyo_x() const { return puyo_x_; }
    int puyo_y() const { return puyo_y_; }
    Puyo puyo1() const { return puyo1_; }
    Puyo puyo2() const { return puyo2_; }
    PuyoConfig puyo_config() const { return puyo_config_; }
    std::string puyo_config_as_str() const;

   private:
    void AddPuyos();
    void ProcessFalls();
    int Explode();

    int chain_power(int chain) const;
    int group_bonus(int explosions) const;

    bool IsTopRowFull() const {
        for (int c = 0; c < GRID_COLS; ++c) {
            if (IsEmpty(c, GRID_LINES - 1)) {
                return false;
            }
        }
        return true;
    }

    bool IsEmpty(int x, int y) const {
        if (x < 0 || x >= GRID_COLS || y < 0 || y >= GRID_LINES) {
            return false;
        }
        return grid_[x][y] == Puyo::None;
    }

    void ReinitPuyo();
    bool CanMoveRight() const;
    bool CanMoveLeft() const;
    bool CanMoveDown() const;

    int CountBlob(Grid<bool>& visited, Puyo cur_color, int x, int y) const;

    void Explode(Puyo cur_color, int x, int y);

    Grid<Puyo> grid_;

    int puyo_x_;
    int puyo_y_;

    Puyo puyo1_;
    Puyo puyo2_;
    PuyoConfig puyo_config_;
};
