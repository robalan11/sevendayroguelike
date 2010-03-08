#ifndef Game_h
#define Game_h

#include <curses.h>
#include "Agent.h"
#include "Player.h"
#include "Level.h"

class Game {
    public:
        Game();
        ~Game();
        bool init_game();
        void play();
    private:
        WINDOW *level_win, *message_win, *stats_win;
        Agent *player;
        Level *current_level;
};

#endif
