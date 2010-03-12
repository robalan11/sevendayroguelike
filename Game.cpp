#include <stdlib.h>
#include <curses.h>
#include <string.h>
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
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);
	init_pair(3, COLOR_BLACK, COLOR_WHITE);
	init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    
    message_win = subwin(stdscr, 2, 80, 0, 1);
    wmove(message_win, 1, 0);
    message_buffer[0] = '\0';
    level_win = subwin(stdscr, 30, 80, 2, 1);
    stats_win = subwin(stdscr, 3, 80, 32, 1);
	inventory_win = newwin(0, 0, 0, 0);
    
    floors[0] = new Level(level_win, this);
	current_level = 0;
	num_levels = 1;
    player = new Player(floors[current_level], this);
    floors[current_level]->add_agent(player);
    
    deaders = (Agent_List *)malloc(sizeof(Agent_List)); //need to free this and children in destructor
    deaders->next = NULL;
    
    return TRUE;
}

//The main game loop.
void Game::play() {
    player->mutual_fov();
    int input;
    do {
        wclear(level_win);
        floors[current_level]->print();
        wrefresh(level_win);
        
        wclear(stats_win);
        mvwprintw(stats_win, 0, 0, "HP %2i/%2i    Floor %i", player->get_hp(), player->get_max_hp(), current_level);
		mvwprintw(stats_win, 1, 0, "Equipment:  Melee weapon: %-12s Suit: %s", player->get_inventory()->get_current_melee_weapon()->get_name(), player->get_inventory()->get_current_suit()->get_name());
		mvwprintw(stats_win, 2, 0, "           Ranged weapon: %-13s Hat: %s", player->get_inventory()->get_current_ranged_weapon()->get_name(), player->get_inventory()->get_current_hat()->get_name());
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

Agent *Game::get_player() {
    return player;
}

void Game::add_dead_agent(Agent *agent) {
    Agent_List *l = (Agent_List *)malloc(sizeof(Agent_List));
    l->agent = agent;
    l->next = deaders->next;
    deaders->next = l;
}

void Game::write_message(const char *msg) {
    int x, y, mx, my;
    getyx(message_win, y, x);
    getmaxyx(message_win, my, mx);
    if(strlen(msg) >= (unsigned int)(mx - x)) {
        wclear(message_win);
        mvwprintw(message_win, 0, 0, "%s", message_buffer);
        message_buffer[0] = '\0';
        wmove(message_win, 1, 0);
    }
    wprintw(message_win, "%s ", msg);
    strcat(message_buffer, msg);
    strcat(message_buffer, " ");
    wrefresh(message_win);
}
