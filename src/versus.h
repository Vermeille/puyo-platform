#pragma once

#include <iostream>
#include <string>

#include <httpi/displayer.h>
#include <httpi/html/chart.h>
#include <httpi/html/form-gen.h>
#include <httpi/html/json.h>
#include <httpi/job.h>
#include <httpi/monitoring.h>
#include <httpi/rest-helpers.h>

#include "game.h"

class Versus {
   public:
    Versus& operator=(const Versus&) = default;

    bool HasEnded() const {
        bool g1_lost = g1_.HasLost();
        bool g2_lost = g2_.HasLost();
        if (g1_lost) {
            std::cerr << "p1 " << tok1_ << " lost in\n";
        }

        if (g1_lost) {
            std::cerr << "p1 " << tok1_ << " lost in\n";
        }
        return g1_lost || g2_lost;
    }

    bool IsPlayer(const std::string& tok) const {
        return tok == tok1_ || tok == tok2_;
    }

    bool HasWon(const std::string& tok) const {
        if (tok == tok1_) {
            return !g1_.HasLost() && g2_.HasLost();
        }
        if (tok == tok2_) {
            return g1_.HasLost() && !g2_.HasLost();
        }
        return false;
    }

    bool Play(const std::string& cmd, const std::string& tok) {
        if (tok == tok1_) {
            return Play1(cmd);
        } else if (tok == tok2_) {
            return Play2(cmd);
        }
        return false;
    }

    std::string PrintGameFor(const std::string& tok) const {
        if (tok == tok1_) {
            return g1_.PrintGame();
        } else if (tok == tok2_) {
            return g2_.PrintGame();
        }
        return "ERROR";
    }

    std::string PrintGameHTML() const {
        using namespace httpi::html;
        // clang-format off
        return (Html()
                << Table()
                    << Tr()
                        << Td() << g1_.PrintGameHTML() << Close()
                        << Td() << g2_.PrintGameHTML() << Close()
                    << Close()
                << Close()).Get();
        // clang-format on
    }

    bool AddPlayer(std::string tok) {
        if (tok1_.empty() && tok2_ != tok) {
            tok1_ = std::move(tok);
            return true;
        }
        if (tok2_.empty() && tok1_ != tok) {
            tok2_ = std::move(tok);
            return true;
        }
        return false;
    }

   private:
    bool Play1(std::string cmd) {
        if (waiting_ == 3) {
            waiting_ = 0;
        }

        if ((waiting_ & 1) == 0) {
            g2_.AddRocks(g1_.GameMakeTurn(cmd));
            waiting_ |= 1;
            return true;
        }
        g1_.RefreshBanTimeout();
        return false;
    }

    bool Play2(std::string cmd) {
        if (waiting_ == 3) {
            waiting_ = 0;
        }

        if ((waiting_ & 2) == 0) {
            g1_.AddRocks(g2_.GameMakeTurn(cmd));
            waiting_ |= 2;
            return true;
        }
        g2_.RefreshBanTimeout();
        return false;
    }

    Game g1_;
    Game g2_;
    int waiting_;
    std::string tok1_;
    std::string tok2_;
};
