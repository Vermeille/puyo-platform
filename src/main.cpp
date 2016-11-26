#include "grid.h"

#include <iostream>

void PrintGame(const Game& g) {
    auto grid = g.Print();
    for (int i = 0; i < GRID_LINES; ++i) {
        std::cout << "|";
        for (int j = 0; j < GRID_COLS; ++j) {
            std::cout << grid[j][GRID_LINES - i - 1];
        }
        std::cout << "|\n";
    }

    for (int j = 0; j < GRID_COLS + 2; ++j) {
        std::cout << "=";
    }
    std::cout << "\n";
}

int main() {
    Game g;

    while (!g.HasLost()) {
        PrintGame(g);
        std::string cmd;
        std::cin >> cmd;

        Game::State state = Game::State::PlayerMove;
        if (cmd == "q") {
            state = g.Move(Game::Direction::Left);
        } else if (cmd == "d") {
            state = g.Move(Game::Direction::Right);
        } else if (cmd == "s") {
            state = g.Move(Game::Direction::Down);
        } else if (cmd == "a") {
            g.RotateLeft();
        } else if (cmd == "e") {
            g.RotateRight();
        }

        if (state == Game::State::ProcessCollisions) {
            g.AddPuyos();
            g.ProcessFalls();
            int total_combo = 0;
            int explosions = 0;
            while ((explosions = g.Explode()) != 0) {
                total_combo += explosions;
                g.ProcessFalls();
            }
            g.ReinitPuyo();
        }
    }

    return 0;
}
