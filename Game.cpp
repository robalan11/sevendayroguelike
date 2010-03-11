#include <curses.h>
#include "Agent.h"
#include "Player.h"
#include "Level.h"
#include "Game.h"

Game::Game() {
    init_game();
}

Game::~Game() {
    delwin(message_win);
    delwin(level_win);
    delwin(stats_win);
    endwin();
}

//Initialize curses, colors, windows, and the game objects.
bool Game::init_game() {
    initscr();
    start_color();
    resize_term(35, 82);
    raw();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    
    message_win = subwin(stdscr, 2, 80, 0, 1);
    level_win = subwin(stdscr, 30, 80, 2, 1);
    stats_win = subwin(stdscr, 3, 80, 32, 1);
    
    floors[0] = new Level(level_win, this);
	current_level = 0;
	num_levels = 1;
    player = new Player(floors[current_level], this);
    floors[current_level]->add_agent(player);
    
    return TRUE;
}

//The main game loop.
void Game::play() {
    player->mutual_fov();
    int input;
    do {
        wclear(level_win);
        floors[current_level]->print();
        //mvwaddch(level_win, player->get_y_pos(), player->get_x_pos(), '@');
        wrefresh(level_win);
        
        wclear(message_win);
        wrefresh(message_win);
        
        wclear(stats_win);
        mvwprintw(stats_win, 0, 0, "Player at x:%i, y:%i, facing:%f.  Floor %i",
            player->get_x_pos(), player->get_y_pos(), player->get_facing(), current_level);
        mvwprintw(stats_win, 1, 0, "HP %2i/%2i", player->get_hp(), player->get_max_hp());
        wrefresh(stats_win);
        
        refresh();
        input = player->take_turn();
        floors[current_level]->monsters_take_turns();
    } while(input != 'Q');
}

//Go down a stair.  Create a new level if there is none.
bool Game::descend() {
	if (current_level < 15) {
        floors[current_level]->remove_agent(player);
		current_level++;
		if (current_level >= num_levels) {
			floors[current_level] = new Level(level_win, this);
			num_levels++;
		}
		player->set_location(floors[current_level]);
		return true;
	} else {
		return false;
	}
}

//Go up a stair.
bool Game::ascend() {
	if (current_level > 0) {
        floors[current_level]->remove_agent(player);
		current_level--;
		player->set_location(floors[current_level]);
		return true;
	} else {
		return false;
	}
}

void Game::write_message(const char *msg) {
    wprintw(message_win, "%s", msg);
}
