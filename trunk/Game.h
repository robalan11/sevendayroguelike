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
                             // curses windows for output.
        Agent *player;       // The player.
		Level* floors[16];   // The levels, with zero being the top.
        int current_level;   // Index into floors array.
		int num_levels;      // Number of levels already generated.
};

#endif
