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
    message_line_1[0] = message_line_0[0] = '\0';
    level_win = subwin(stdscr, 30, 80, 2, 1);
    stats_win = subwin(stdscr, 3, 80, 32, 1);
	inventory_win = newwin(0, 0, 0, 0);
    
	current_level = 0;
    floors[current_level] = new Level(level_win, this);
	num_levels = 1;
    player = new Player(floors[current_level], this);
    floors[current_level]->add_agent(player);
    
    deaders = (Agent_List *)malloc(sizeof(Agent_List)); //need to free this and children in destructor
    deaders->next = NULL;
    
    return TRUE;
}

void Game::show_title() {
    char *text1 =
    "The year is 1952. The place, a tiny village in Northern Sibera.\n"
    "Buried nearby is a secret government installation containing weapons\n"
    "that could destroy the world as we know it.  Only you have the daring,\n"
    "derring-do, and derringers to stop these dastardly deviants in their\n"
    "tracks.  Your mission, should you choose to accept it, is to fight or\n"
    "sneak your way through the labyrinthine passages carved into the hills\n"
    "and find the central control room to thwart this plot once and for all.\n";
    printw("%s", text1);
    printw("%s", "\nPress a key to accept this mission.\n");
    curs_set(2);
    getch();
    clear();
    echo();
    mvprintw(1, 1, "Enter your name: ");
    name[0] = 'M';
    name[1] = '\0';
    scanw("%31s", name);
    noecho();
    curs_set(0);
    clear();
    mvprintw(0, 0, "Mission Briefing.\n%s:", name);
    char *text2 =
    " You will parachute directly into the installation. You will carry as little\n"
    "as possible to minimize the danger of detection.  You will be equipped with\n"
    "a derringer, some ammunition, a pocketknife, and a your jumpsuit.  We trust\n"
    "that you will be able to find any other tools you need in the base.\n";
    mvprintw(2, 0, "%s", text2);
    getch();
    clear();
}

//The main game loop.
void Game::play() {
    player->mutual_fov();
    floors[current_level]->monsters_mutual_fov();
    int input;
    do {
        wclear(level_win);
        floors[current_level]->print();
        wrefresh(level_win);
        
        int msg_x, msg_y;
        getyx(message_win, msg_y, msg_x);
        wclear(message_win);
        mvwprintw(message_win, 0, 0, "%s", message_line_0);
        mvwprintw(message_win, 1, 0, "%s", message_line_1);
        wrefresh(message_win);
        wmove(message_win, msg_y, msg_x);
        
        wclear(stats_win);
        mvwprintw(stats_win, 0, 0, "HP %2i/%2i    Floor %i", player->get_hp(), player->get_max_hp(), current_level);
		if (floors[current_level]->contains_item(player->get_x_pos(), player->get_y_pos()))
			mvwprintw(stats_win, 0, 40, "There is a %s on the floor.", floors[current_level]->get_item(player->get_x_pos(), player->get_y_pos())->get_name());
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
        strcpy(message_line_0, message_line_1);
        mvwprintw(message_win, 0, 0, "%s", message_line_0);
        message_line_1[0] = '\0';
        wmove(message_win, 1, 0);
    }
    wprintw(message_win, "%s ", msg);
    strcat(message_line_1, msg);
    strcat(message_line_1, " ");
    wrefresh(message_win);
}
