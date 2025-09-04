#ifndef GAME_H
#define GAME_H

#include "board.h"
#include <string>

class Game {
public:
    Game();
    void run();
private:
    Board board;
    bool parseMoveStr(const std::string &s, Move &m) const;
};

#endif
