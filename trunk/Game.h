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
        void show_title();
        void play();
		bool descend();
		bool ascend();
		void player_act();
		
		void win();
		void lose();
		void end();
		
		Agent *get_player();
		void add_dead_agent(Agent *agent);
		int get_current_level() { return current_level; }
		
		void write_message(const char *msg);
		WINDOW* get_inventory_win() { return inventory_win; }
		
    private:
        WINDOW *level_win, *message_win, *stats_win, *inventory_win;
                             // curses windows for output.
        char message_line_0[80];
        char message_line_1[80];
        char name[32];
        Agent *player;       // The player.
        Agent_List *deaders; // List of things that have died and need cleaning.
		Level* floors[16];   // The levels, with zero being the top.
        int current_level;   // Index into floors array.
		int num_levels;      // Number of levels already generated.
		bool game_over;      // Flag indicating whether the game is over.
		bool game_won, game_lost; //Flags indicating the result of the game.
		bool player_acted;   // Flag indicating whether the player has acted this turn.
};

#endif
