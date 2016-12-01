#include "src/game.h"

#include <sstream>

#include <httpi/displayer.h>
#include <httpi/html/chart.h>
#include <httpi/html/form-gen.h>
#include <httpi/html/json.h>
#include <httpi/job.h>
#include <httpi/monitoring.h>
#include <httpi/rest-helpers.h>

PuyoGrid::State Game::ApplyMove(const std::string& cmd) {
    PuyoGrid::State state = PuyoGrid::State::PlayerMove;
    if (cmd == "LEFT") {
        state = g_.Move(PuyoGrid::Direction::Left);
    } else if (cmd == "RIGHT") {
        state = g_.Move(PuyoGrid::Direction::Right);
    } else if (cmd == "DOWN") {
        state = g_.Move(PuyoGrid::Direction::Down);
    } else if (cmd == "ROTL") {
        g_.RotateLeft();
    } else if (cmd == "ROTR") {
        g_.RotateRight();
    }
    return state;
}

int Game::ProcessRocksAndPhysics() {
    int score = g_.ProcessCollisions();
    if (pending_rocks_ > 30) {
        g_.AddRocks(30);
        pending_rocks_ -= 30;
    } else {
        g_.AddRocks(pending_rocks_);
        pending_rocks_ = 0;
    }
    return score;
}

std::string Game::PrintGame() const {
    if (g_.HasLost()) {
        return "YOU_LOST";
    }

    std::ostringstream oss;
    oss << g_.puyo_x() << " " << g_.puyo_y() << " " << g_.puyo_config_as_str()
        << "\n";
    oss << PuyoToChar(g_.puyo1()) << " " << PuyoToChar(g_.puyo2()) << "\n";

    oss << g_.PrintStr();
    return oss.str();
}

std::string Game::PrintGameHTML() const {
    if (g_.HasLost()) {
        return "YOU_LOST";
    }

    auto grid = g_.Print();
    grid[g_.puyo_x()][g_.puyo_y()] = PuyoToChar(g_.puyo1());
    if (g_.puyo_config() == PuyoGrid::PuyoConfig::Up) {
        grid[g_.puyo_x()][g_.puyo_y() + 1] = PuyoToChar(g_.puyo2());
    } else if (g_.puyo_config() == PuyoGrid::PuyoConfig::Down) {
        grid[g_.puyo_x()][g_.puyo_y() - 1] = PuyoToChar(g_.puyo2());
    } else if (g_.puyo_config() == PuyoGrid::PuyoConfig::Left) {
        grid[g_.puyo_x() - 1][g_.puyo_y()] = PuyoToChar(g_.puyo2());
    } else {
        grid[g_.puyo_x() + 1][g_.puyo_y()] = PuyoToChar(g_.puyo2());
    }

    using namespace httpi::html;
    Html html;
    html << Table();

    for (int i = 0; i < GRID_LINES; ++i) {
        html << Tr() << Td() << "<img "
                                "src=\"http://files.vermeille.fr/puyo/"
                                "puyoWall.png\"/>";
        html << Close();
        for (int j = 0; j < GRID_COLS; ++j) {
            html << Td();
            std::string url = "<img src=\"http://files.vermeille.fr/puyo/puyo";
            url.push_back(grid[j][GRID_LINES - i - 1]);
            url += ".png\"/>";

            html << url;
            html << Close();
        }
        html << Td();
        html << "<img src=\"http://files.vermeille.fr/puyo/"
                "puyoWall.png\"/>";
        html << Close();
        html << Close();
    }

    html << Close();
    return html.Get();
}
