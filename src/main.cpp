#include "grid.h"

#include <sstream>

#include <httpi/displayer.h>
#include <httpi/html/chart.h>
#include <httpi/html/form-gen.h>
#include <httpi/html/json.h>
#include <httpi/job.h>
#include <httpi/monitoring.h>
#include <httpi/rest-helpers.h>

std::string MakePage(const std::string& content) {
    // clang-format off
    using namespace httpi::html;
    return (Html() <<
        "<!DOCTYPE html>"
        "<html>"
           "<head>"
                R"(<meta charset="utf-8">)"
                R"(<meta http-equiv="X-UA-Compatible" content="IE=edge">)"
                R"(<meta name="viewport" content="width=device-width, initial-scale=1">)"
                R"(<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css">)"
                R"(<link rel="stylesheet" href="//cdn.jsdelivr.net/chartist.js/latest/chartist.min.css">)"
                R"(<script src="//cdn.jsdelivr.net/chartist.js/latest/chartist.min.js"></script>)"
            "</head>"
            "<body lang=\"en\">"
                "<div class=\"container\">"
                    "<div class=\"col-md-9\">" <<
                        content <<
                    "</div>"
                "</div>"
            "</body>"
        "</html>").Get();
    // clang-format on
}

class GameFull {
    Game g_;
    int down_cooldown_;

   public:
    GameFull() : down_cooldown_(3) {}

    void GameMakeTurn(std::string cmd) {
        Game::State state = Game::State::PlayerMove;
        if (cmd == "LEFT") {
            state = g_.Move(Game::Direction::Left);
        } else if (cmd == "RIGHT") {
            state = g_.Move(Game::Direction::Right);
        } else if (cmd == "DOWN") {
            state = g_.Move(Game::Direction::Down);
        } else if (cmd == "ROTL") {
            g_.RotateLeft();
        } else if (cmd == "ROTR") {
            g_.RotateRight();
        }

        --down_cooldown_;
        if (state == Game::State::PlayerMove && down_cooldown_ == 0) {
            state = g_.Move(Game::Direction::Down);
            down_cooldown_ = 3;
        }

        if (state == Game::State::ProcessCollisions) {
            g_.ProcessCollisions();
            down_cooldown_ = 3;
        }
    }

    bool HasLost() const { return g_.HasLost(); }

    std::string PrintGame() const {
        if (g_.HasLost()) {
            return "YOU_LOST";
        }

        std::ostringstream oss;
        oss << g_.puyo_x() << " " << g_.puyo_y() << " "
            << g_.puyo_config_as_str() << "\n";
        oss << PuyoToChar(g_.puyo1()) << " " << PuyoToChar(g_.puyo2()) << "\n";

        auto grid = g_.Print();
        for (int i = 0; i < GRID_LINES; ++i) {
            oss << "|";
            for (int j = 0; j < GRID_COLS; ++j) {
                oss << grid[j][GRID_LINES - i - 1];
            }
            oss << "|\n";
        }

        for (int j = 0; j < GRID_COLS + 2; ++j) {
            oss << "=";
        }
        oss << "\n";
        return oss.str();
    }
};

int main() {
    InitHttpInterface();
    GameFull g;

    RegisterUrl("/turn",
                httpi::RestPageMaker(MakePage).AddResource(
                    "GET",
                    httpi::RestResource(
                        httpi::html::FormDescriptor<std::string>{
                            "GET",
                            "/turn",
                            "Make your next move",  // name
                            "Make your next move. LEFT, DOWN, RIGHT, ROTL or "
                            "ROTR",  // longer description
                            {{"move", "text", "your move choice"}}},
                        [&](std::string cmd) {
                            if (g.HasLost()) {
                                return 0;
                            }
                            g.GameMakeTurn(cmd);
                            return 0;
                        },
                        [&](int) -> std::string {
                            using namespace httpi::html;
                            if (g.HasLost()) {
                                return "LOST";
                            }
                            return (Html() << Tag("pre") << g.PrintGame()
                                           << Close())
                                .Get();
                        },
                        [&](int) { return g.PrintGame(); })));

    ServiceLoopForever();  // infinite loop ending only on SIGINT / SIGTERM /
                           // SIGKILL

    StopHttpInterface();  // clear resources<Paste>
    return 0;
}
