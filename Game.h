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
		bool descend();
		bool ascend();
    private:
        WINDOW *level_win, *message_win, *stats_win;
        Agent *player;
		Level* floors[16];
        int current_level;
		int num_levels;
};

#endif
