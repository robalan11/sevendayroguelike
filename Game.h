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
		
		Agent *get_player();
		
		void write_message(const char *msg);
		WINDOW* get_inventory_win() { return inventory_win; }
		
    private:
        WINDOW *level_win, *message_win, *stats_win, *inventory_win;
                             // curses windows for output.
        char message_buffer[80];
        void setup_colors(); // set up colors! just like it says
        Agent *player;       // The player.
		Level* floors[16];   // The levels, with zero being the top.
        int current_level;   // Index into floors array.
		int num_levels;      // Number of levels already generated.
};

#endif
