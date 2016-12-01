#include <iostream>

#include "src/grid.h"

PuyoGrid MakeGrid(const std::vector<std::string>& map) {
    Grid<char> g;
    for (size_t i = 0; i < g.size(); ++i) {
        std::copy(std::begin(map[i]), std::end(map[i]), g[i].begin());
    }
    return PuyoGrid(g);
}

int main() {
    PuyoGrid pg = MakeGrid({"            ",
                            "YG          ",
                            "GGG         ",
                            "RG          ",
                            "            ",
                            "            "});
    assert(pg.ProcessCollisions() == 0);
    pg = MakeGrid({"            ",
                   "YG          ",
                   "GGGG        ",
                   "RG          ",
                   "            ",
                   "            "});
    assert(pg.ProcessCollisions() == 1);
    pg = MakeGrid({"            ",
                   "YG          ",
                   "GGG         ",
                   "RGRRR       ",
                   "            ",
                   "            "});
    assert(pg.ProcessCollisions() == 5);
    pg = MakeGrid({"            ",
                   "YG          ",
                   "GGG         ",
                   "RGRRRY      ",
                   "YYY         ",
                   "            "});
    assert(pg.ProcessCollisions() == 14);
    pg = MakeGrid({"            ",
                   "YG          ",
                   "GGG         ",
                   "RGRRRY      ",
                   "YYYB        ",
                   "BBB         "});
    assert(pg.ProcessCollisions() == 32);
    return 0;
}
