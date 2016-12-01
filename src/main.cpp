#include <cstdlib>
#include <iostream>

#include <httpi/displayer.h>
#include <httpi/html/chart.h>
#include <httpi/html/form-gen.h>
#include <httpi/html/json.h>
#include <httpi/job.h>
#include <httpi/monitoring.h>
#include <httpi/rest-helpers.h>

#include "game.h"
#include "versus.h"

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
                    "<h1>Puyo-Platform</h1>" <<
                    Div().Attr("class", "col-md-9") <<
                        content <<
                    Close() <<
                    Div().Attr("class", "col-md-3") <<
                        Ul() <<
                            Li() <<
                                A().Attr("href", "/") <<
                                    "How to / manual" <<
                                Close() <<
                            Close() <<
                            Li() <<
                                A().Attr("href", "/new") <<
                                    "Create a new solo game" <<
                                Close() <<
                            Close() <<
                            Li() <<
                                A().Attr("href", "/turn") <<
                                    "Play a turn in a solo game" <<
                                Close() <<
                            Close() <<
                            Li() <<
                                A().Attr("href", "/newvs") <<
                                    "Create/Join a Versus game" <<
                                Close() <<
                            Close() <<
                            Li() <<
                                A().Attr("href", "/turnvs") <<
                                    "Play a turn in a versus game" <<
                                Close() <<
                            Close() <<
                        Close() <<
                    Close() <<
                    Div().Attr("class", "col-md-12 text-right") <<
                        Tag("footer") <<
                            A().Attr("href", "http://github.com/Vermeille") <<
                                "Vermeille" <<
                            Close() <<
                            " saved the world once again!" <<
                        Close() <<
                    Close() <<
                "</div>"
            "</body>"
        "</html>").Get();
    // clang-format on
}

template <class Container, class Pred>
void map_erase_if(Container& c, Pred&& p) {
    for (auto it = c.begin(), end = c.end(); it != end;) {
        if (p(*it)) {
            it = c.erase(it);
        } else {
            ++it;
        }
    }
}

struct GamesGarbageCollector {
    GamesGarbageCollector(std::map<std::string, Game>& games,
                          std::map<std::string, Versus>& versus)
        : games_(games), versus_(versus), nb_request_since_last_gc_(0) {}

    void Tick() {
        if (++nb_request_since_last_gc_ < 10000) {
            return;
        }
        std::cerr << "GARBAGE COLLECTION\n";
        map_erase_if(games_, [](const auto& g) { return g.second.HasLost(); });
        map_erase_if(versus_,
                     [](const auto& g) { return g.second.HasEnded(); });
        nb_request_since_last_gc_ = 0;
    }

   private:
    std::map<std::string, Game>& games_;
    std::map<std::string, Versus>& versus_;
    int nb_request_since_last_gc_;
};

int main() {
    InitHttpInterface();
    srand(time(nullptr));

    std::map<std::string, Game> games;
    std::map<std::string, Versus> vs_games;
    GamesGarbageCollector gc(games, vs_games);
    RegisterUrl("/turn",
                httpi::RestPageMaker(MakePage).AddResource(
                    "GET",
                    httpi::RestResource(
                        httpi::html::FormDescriptor<std::string, std::string>{
                            "GET",
                            "/turn",
                            "Make your next move",  // name
                            "Make your next move. LEFT, DOWN, RIGHT, ROTL or "
                            "ROTR",  // longer description
                            {{"move", "text", "Your move choice"},
                             {"name", "text", "Your game's name"}}},
                        [&](std::string cmd, std::string name) -> Game* {
                            gc.Tick();
                            std::cerr << "/turn " << cmd << " " << name << "\n";
                            auto found = games.find(name);
                            if (found == games.end()) {
                                return nullptr;
                            }
                            Game* g = &found->second;
                            if (g->HasLost()) {
                                return g;
                            }
                            g->GameMakeTurn(cmd);
                            return g;
                        },
                        [&](const Game* g) -> std::string {
                            if (!g) {
                                std::cerr << "Your game wasn't found\n";
                                return "Your game wasn't found";
                            }
                            if (g->HasLost()) {
                                std::cerr << "LOST\n";
                                return "LOST";
                            }
                            std::cerr << g->PrintGame() << "\n";
                            return g->PrintGameHTML();
                        },
                        [&](const Game* g) -> std::string {
                            if (!g) {
                                std::cerr << "ERROR\n";
                                return "ERROR";
                            }
                            if (g->HasLost()) {
                                std::cerr << "LOST\n";
                                return "LOST";
                            }
                            std::cerr << g->PrintGame() << "\n";
                            return g->PrintGame();
                        })));

    RegisterUrl(
        "/new",
        httpi::RestPageMaker(MakePage).AddResource(
            "GET",
            httpi::RestResource(
                httpi::html::FormDescriptor<std::string>{
                    "GET",
                    "/new",
                    "Create a new Game",
                    "Name your game with a unique single-word token",
                    {{"name", "text", "Your game's name"}}},
                [&](std::string name) -> Game* {
                    gc.Tick();
                    std::cerr << "/new " << name << "\n";
                    auto found = games.find(name);
                    if (found != games.end() && !found->second.HasLost()) {
                        return nullptr;
                    }
                    Game* g = &(games[name] = Game());
                    return g;
                },
                [](const Game* g) {
                    if (g) {
                        std::cerr << g->PrintGame() << "\n";
                        return g->PrintGameHTML();
                    } else {
                        std::cerr << "invalid name\n";
                        return (httpi::html::Html() << "This name is invalid")
                            .Get();
                    }
                },
                [](const Game* g) -> std::string {
                    if (g) {
                        std::cerr << g->PrintGame() << "\n";
                        return g->PrintGame();
                    } else {
                        std::cerr << "ERROR\n";
                        return "ERROR";
                    }
                })));

    RegisterUrl(
        "/turnvs",
        httpi::RestPageMaker(MakePage).AddResource(
            "GET",
            httpi::RestResource(
                httpi::html::FormDescriptor<std::string,
                                            std::string,
                                            std::string>{
                    "GET",
                    "/turnvs",
                    "Make your next move",  // name
                    "Make your next move. LEFT, DOWN, RIGHT, ROTL or "
                    "ROTR",  // longer description
                    {{"move", "text", "Your move choice"},
                     {"game_name", "text", "Your game's name"},
                     {"player_name", "text", "Your player's name"}}},
                [&](std::string cmd,
                    std::string game_name,
                    std::string player_name)
                    -> std::tuple<const Versus*, std::string, bool> {
                        gc.Tick();

                        std::cerr << "/turn " << cmd << " " << game_name << " " << player_name << "\n";
                        auto found = vs_games.find(game_name);
                        if (found == vs_games.end()) {
                            return std::make_tuple(nullptr, player_name, false);
                        }
                        Versus* g = &found->second;
                        if (g->HasEnded()) {
                            return std::make_tuple(g, player_name, false);
                        }
                        bool has_played = g->Play(cmd, player_name);
                        return std::make_tuple(g, player_name, has_played);
                    },
                [](const Versus* g, std::string player, bool has_played)
                    -> std::string {
                        if (!g) {
                            std::cerr << "NOT FOUND\n";
                            return "Your game wasn't found";
                        }

                        if (!g->IsPlayer(player)) {
                            std::cerr << "NOT_PLAYER\n";
                            return "NOT_PLAYER";
                        }

                        if (g->HasEnded()) {
                            if (g->HasWon(player)) {
                                std::cerr << "WON\n";
                                return "WON";
                            }
                            std::cerr << "LOST\n";
                            return "LOST";
                        }
                        if (!has_played) {
                            std::cerr << "WAITING\n";
                            return "WAITING";
                        }
                        std::cerr << g->PrintGameFor(player) << "\n";
                        return g->PrintGameHTML();
                    },
                [](const Versus* g, std::string player, bool has_played)
                    -> std::string {
                        if (!g) {
                            std::cerr << "ERROR\n";
                            return "ERROR";
                        }

                        if (!g->IsPlayer(player)) {
                            std::cerr << "NOT_PLAYER\n";
                            return "NOT_PLAYER";
                        }

                        if (g->HasEnded()) {
                            if (g->HasWon(player)) {
                                std::cerr << "WON\n";
                                return "WON";
                            }
                            std::cerr << "LOST\n";
                            return "LOST";
                        }
                        if (!has_played) {
                            return "WAITING";
                        }
                        std::cerr << g->PrintGameFor(player) << "\n";
                        return g->PrintGameFor(player);
                    })));

    RegisterUrl(
        "/newvs",
        httpi::RestPageMaker(MakePage).AddResource(
            "GET",
            httpi::RestResource(
                httpi::html::FormDescriptor<std::string, std::string>{
                    "GET",
                    "/newvs",
                    "Create a new Game",
                    "Name your game with a unique single-word token",
                    {{"game_name", "text", "Your game's name"},
                     {"player_name", "text", "Player's name"}}},
                [&](std::string game_name, std::string player_name)
                    -> std::tuple<const Versus*, std::string> {
                        gc.Tick();
                        std::cerr << "/newvs " << game_name << " " << player_name << "\n";
                        auto found = vs_games.find(game_name);
                        if (found != vs_games.end()) {
                            if (!found->second.AddPlayer(player_name)) {
                                return std::make_pair(nullptr, player_name);
                            }
                            return std::make_pair(&found->second, player_name);
                        }
                        Versus* g = &(vs_games[game_name] = Versus());
                        g->AddPlayer(player_name);
                        return std::make_pair(g, player_name);
                    },
                [](const Versus* g, std::string name) {
                    if (g) {
                        std::cerr << g->PrintGameFor(name) << "\n";
                        return g->PrintGameHTML();
                    } else {
                        std::cerr << "INVALID NAME\n";
                        return (httpi::html::Html() << "This name is invalid")
                            .Get();
                    }
                },
                [](const Versus* g, std::string player_name) -> std::string {
                    if (g) {
                        std::cerr << g->PrintGameFor(player_name) << "\n";
                        return g->PrintGameFor(player_name);
                    } else {
                        std::cerr << "ERROR\n";
                        return "ERROR";
                    }
                })));

    RegisterUrl("/", [](const std::string&, const POSTValues&) {
        return MakePage((httpi::html::Html() <<
#include "landing_page_content.txt"
                         ).Get());
    });

    ServiceLoopForever();

    StopHttpInterface();  // clear resources<Paste>
    return 0;
}
