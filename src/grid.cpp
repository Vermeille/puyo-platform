#include "grid.h"

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
            return 'X';
        case Puyo::None:
            return ' ';
        default:
            assert(false);
    }
}

PuyoGrid::PuyoGrid() {
    for (auto& i : grid_) {
        for (auto& j : i) {
            j = Puyo::None;
        }
    }
    ReinitPuyo();
}

void PuyoGrid::ReinitPuyo() {
    puyo1_ = static_cast<Puyo>(rand() % 4);
    puyo2_ = static_cast<Puyo>(rand() % 4);
    puyo_config_ = PuyoConfig::Up;
    puyo_x_ = GRID_COLS / 2;
    puyo_y_ = GRID_LINES - 2;
}

void PuyoGrid::RotateRight() {
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

void PuyoGrid::RotateLeft() {
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

Grid<char> PuyoGrid::Print() const {
    Grid<char> val;
    for (size_t i = 0; i < grid_.size(); ++i) {
        std::transform(std::begin(grid_[i]),
                       std::end(grid_[i]),
                       std::begin(val[i]),
                       PuyoToChar);
    }
    return val;
}

bool PuyoGrid::CanMoveRight() const {
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

bool PuyoGrid::CanMoveLeft() const {
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

bool PuyoGrid::CanMoveDown() const {
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

PuyoGrid::State PuyoGrid::Move(Direction dir) {
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

void PuyoGrid::AddPuyos() {
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

void PuyoGrid::ProcessFalls() {
    for (auto& col : grid_) {
        std::stable_partition(std::begin(col), std::end(col), [](Puyo p) {
            return p != Puyo::None;
        });
    }
}

int PuyoGrid::Explode() {
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

int PuyoGrid::chain_power(int chain) const {
    static constexpr int chain_power[] = {
        0,   8,   16,  32,  64,  96,  128, 160, 192, 224, 256, 288,
        320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672};
    if (chain >= 24) {
        return chain_power[23];
    }
    return chain_power[chain];
}

int PuyoGrid::group_bonus(int explosions) const {
    static constexpr int bonus[] = {0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 8};
    if (explosions >= 11) {
        return bonus[11];
    }
    return bonus[explosions];
}

int PuyoGrid::ProcessCollisions() {
    AddPuyos();
    ProcessFalls();
    int total_score = 0;
    int chain = 0;
    int explosions = 0;
    while ((explosions = Explode()) != 0) {
        total_score +=
            10 * explosions * (chain_power(chain) + group_bonus(explosions));
        ProcessFalls();
    }
    ReinitPuyo();
    return total_score;
}

void PuyoGrid::AddRocks(int amount) {
    while (amount > 0 && !IsTopRowFull()) {
        for (int c = (amount >= GRID_COLS ? 0 : rand() % GRID_COLS);
             c < GRID_COLS && amount > 0;
             ++c) {
            if (IsEmpty(c, GRID_LINES - 1)) {
                grid_[c][GRID_LINES - 1] = Puyo::Rock;
                --amount;
            }
        }
        ProcessFalls();
    }
}

std::string PuyoGrid::puyo_config_as_str() const {
    switch (puyo_config_) {
        case PuyoConfig::Up:
            return "UP";
        case PuyoConfig::Right:
            return "RIGHT";
        case PuyoConfig::Down:
            return "DOWN";
        case PuyoConfig::Left:
            return "LEFT";
        default:
            assert(false);
    }
}

int PuyoGrid::CountBlob(Grid<bool>& visited,
                        Puyo cur_color,
                        int x,
                        int y) const {
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

void PuyoGrid::Explode(Puyo cur_color, int x, int y) {
    if (grid_[x][y] == Puyo::Rock) {
        grid_[x][y] = Puyo::None;
        return;
    }

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
