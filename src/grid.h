#pragma once

#include <algorithm>
#include <array>
#include <cassert>

static constexpr int GRID_LINES = 16;
static constexpr int GRID_COLS = 6;

enum class Puyo { Red, Green, Blue, Yellow, Rock, None };

template <class T>
using Grid = std::array<std::array<T, GRID_LINES>, GRID_COLS>;

char PuyoToChar(Puyo p) {
    switch (p) {
        case Puyo::Red:
            return 'R';
        case Puyo::Green:
            return 'G';
        case Puyo::Blue:
            return 'B';
        case Puyo::Yellow:
            return 'Y';
        case Puyo::Rock:
            return '#';
        case Puyo::None:
            return ' ';
        default:
            assert(false);
    }
}

class Game {
    Grid<Puyo> grid_;

    int puyo_x_;
    int puyo_y_;

    enum class PuyoConfig { Up, Right, Down, Left };
    Puyo puyo1_;
    Puyo puyo2_;
    PuyoConfig puyo_config_;

    int CountBlob(Grid<bool>& visited, Puyo cur_color, int x, int y) const {
        if (visited[x][y] || grid_[x][y] != cur_color ||
            grid_[x][y] == Puyo::None) {
            return 0;
        }
        visited[x][y] = true;

        int total = 1;
        if (x > 0) {
            total += CountBlob(visited, cur_color, x - 1, y);
        }

        if (x < GRID_COLS - 1) {
            total += CountBlob(visited, cur_color, x + 1, y);
        }

        if (y > 0) {
            total += CountBlob(visited, cur_color, x, y - 1);
        }

        if (y < GRID_LINES - 1) {
            total += CountBlob(visited, cur_color, x, y + 1);
        }

        return total;
    }

    void Explode(Puyo cur_color, int x, int y) {
        if (grid_[x][y] != cur_color || grid_[x][y] == Puyo::None) {
            return;
        }

        grid_[x][y] = Puyo::None;

        if (x > 0) {
            Explode(cur_color, x - 1, y);
        }

        if (x < GRID_COLS - 1) {
            Explode(cur_color, x + 1, y);
        }

        if (y > 0) {
            Explode(cur_color, x, y - 1);
        }

        if (y < GRID_LINES - 1) {
            Explode(cur_color, x, y + 1);
        }
    }

   public:
    void ReinitPuyo() {
        puyo1_ = Puyo::Red;
        puyo2_ = Puyo::Green;
        puyo_config_ = PuyoConfig::Left;
        puyo_x_ = 3;
        puyo_y_ = 15;
    }

    void RotateRight() {
        switch (puyo_config_) {
            case PuyoConfig::Up:
                if (IsEmpty(puyo_x_ + 1, puyo_y_)) {
                    puyo_config_ = PuyoConfig::Right;
                }
                break;
            case PuyoConfig::Right:
                if (IsEmpty(puyo_x_, puyo_y_ - 1)) {
                    puyo_config_ = PuyoConfig::Down;
                }
                break;
            case PuyoConfig::Down:
                if (IsEmpty(puyo_x_ - 1, puyo_y_)) {
                    puyo_config_ = PuyoConfig::Left;
                }
                break;
            case PuyoConfig::Left:
                if (IsEmpty(puyo_x_, puyo_y_ + 1)) {
                    puyo_config_ = PuyoConfig::Up;
                }
                break;
            default:
                assert(false);
        }
    }

    void RotateLeft() {
        switch (puyo_config_) {
            case PuyoConfig::Up:
                if (IsEmpty(puyo_x_ - 1, puyo_y_)) {
                    puyo_config_ = PuyoConfig::Left;
                }
                break;
            case PuyoConfig::Right:
                if (IsEmpty(puyo_x_, puyo_y_ + 1)) {
                    puyo_config_ = PuyoConfig::Up;
                }
                break;
            case PuyoConfig::Down:
                if (IsEmpty(puyo_x_ + 1, puyo_y_)) {
                    puyo_config_ = PuyoConfig::Right;
                }
                break;
            case PuyoConfig::Left:
                if (IsEmpty(puyo_x_, puyo_y_ - 1)) {
                    puyo_config_ = PuyoConfig::Down;
                }
                break;
            default:
                assert(false);
        }
    }

    Game() {
        for (auto& i : grid_) {
            for (auto& j : i) {
                j = Puyo::None;
            }
        }
        ReinitPuyo();
    }

    auto Print() const {
        Grid<char> val;
        for (size_t i = 0; i < grid_.size(); ++i) {
            std::transform(std::begin(grid_[i]),
                           std::end(grid_[i]),
                           std::begin(val[i]),
                           PuyoToChar);
        }
        val[puyo_x_][puyo_y_] = '0';
        return val;
    }

    enum class State { PlayerMove, ProcessCollisions };
    enum class Direction { Left, Right, Down };

    bool IsEmpty(int x, int y) const {
        if (x < 0 || x >= GRID_COLS || y < 0 || y >= GRID_LINES) {
            return false;
        }
        return grid_[x][y] == Puyo::None;
    }

    bool CanMoveRight() const {
        switch (puyo_config_) {
            case PuyoConfig::Up:
                return IsEmpty(puyo_x_ + 1, puyo_y_) &&
                       IsEmpty(puyo_x_ + 1, puyo_y_ + 1);
            case PuyoConfig::Down:
                return IsEmpty(puyo_x_ + 1, puyo_y_) &&
                       IsEmpty(puyo_x_ + 1, puyo_y_ - 1);
            case PuyoConfig::Left:
                return IsEmpty(puyo_x_ + 1, puyo_y_);
            case PuyoConfig::Right:
                return IsEmpty(puyo_x_ + 2, puyo_y_);
            default:
                assert(false);
        }
    }

    bool CanMoveLeft() const {
        switch (puyo_config_) {
            case PuyoConfig::Up:
                return IsEmpty(puyo_x_ - 1, puyo_y_) &&
                       IsEmpty(puyo_x_ - 1, puyo_y_ + 1);
            case PuyoConfig::Down:
                return IsEmpty(puyo_x_ - 1, puyo_y_) &&
                       IsEmpty(puyo_x_ - 1, puyo_y_ - 1);
            case PuyoConfig::Right:
                return IsEmpty(puyo_x_ - 1, puyo_y_);
            case PuyoConfig::Left:
                return IsEmpty(puyo_x_ - 2, puyo_y_);
            default:
                assert(false);
        }
    }

    bool CanMoveDown() const {
        if (puyo_y_ <= 0) {
            return false;
        }

        switch (puyo_config_) {
            case PuyoConfig::Up:
                return IsEmpty(puyo_x_, puyo_y_ - 1);
            case PuyoConfig::Down:
                return IsEmpty(puyo_x_, puyo_y_ - 2);
            case PuyoConfig::Right:
                return IsEmpty(puyo_x_, puyo_y_ - 1) &&
                       IsEmpty(puyo_x_ + 1, puyo_y_ - 1);
            case PuyoConfig::Left:
                return IsEmpty(puyo_x_, puyo_y_ - 1) &&
                       IsEmpty(puyo_x_ - 1, puyo_y_ - 1);
            default:
                assert(false);
        }
    }

    State Move(Direction dir) {
        switch (dir) {
            case Direction::Left:
                if (CanMoveLeft()) {
                    --puyo_x_;
                }
                break;
            case Direction::Right:
                if (CanMoveRight()) {
                    ++puyo_x_;
                }
                break;
            case Direction::Down:
                if (CanMoveDown()) {
                    --puyo_y_;
                } else {
                    return State::ProcessCollisions;
                }
                break;
        }
        return State::PlayerMove;
    }

    void AddPuyos() {
        assert(IsEmpty(puyo_x_, puyo_y_));
        grid_[puyo_x_][puyo_y_] = puyo1_;
        switch (puyo_config_) {
            case PuyoConfig::Up:
                assert(IsEmpty(puyo_x_, puyo_y_ + 1));
                grid_[puyo_x_][puyo_y_ + 1] = puyo2_;
                return;
            case PuyoConfig::Down:
                assert(IsEmpty(puyo_x_, puyo_y_ - 1));
                grid_[puyo_x_][puyo_y_ - 1] = puyo2_;
                return;
            case PuyoConfig::Left:
                assert(IsEmpty(puyo_x_ - 1, puyo_y_));
                grid_[puyo_x_ - 1][puyo_y_] = puyo2_;
                return;
            case PuyoConfig::Right:
                assert(IsEmpty(puyo_x_ + 1, puyo_y_));
                grid_[puyo_x_ + 1][puyo_y_] = puyo2_;
                return;
        }
    }

    void ProcessFalls() {
        for (auto& col : grid_) {
            std::stable_partition(std::begin(col), std::end(col), [](Puyo p) {
                return p != Puyo::None;
            });
        }
    }

    bool HasLost() const {
        return grid_[GRID_COLS / 2][GRID_LINES - 1] != Puyo::None;
    }

    int Explode() {
        Grid<bool> visited{};

        int exploded = 0;
        for (int i = 0; i < GRID_LINES; ++i) {
            for (int j = 0; j < GRID_COLS; ++j) {
                if (CountBlob(visited, grid_[j][i], j, i) >= 4) {
                    Explode(grid_[j][i], j, i);
                    ++exploded;
                }
            }
        }
        return exploded;
    }
};
