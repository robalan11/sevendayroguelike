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
    
    game_over = game_won = game_lost = false;
    
    return true;
}

void Game::show_title() {
    mvprintw(1,  30, "The Spy Who Rogued Me");
    mvprintw(3,  30, "A Seven Day Roguelike");
    mvprintw(5,  30, "by robalan and jjonir");
    mvprintw(10, 37, "#######");
    mvprintw(11, 37, "#.....#");
    mvprintw(12, 37, "#..@..#");
    mvprintw(13, 37, "#.....#");
    mvprintw(14, 37, "#######");
    getch();
    clear();
    
    char *text1 =
    "The year is 1952. The place, a tiny village in Northern Sibera.\n"
    "Buried nearby is a secret government installation containing weapons\n"
    "that could destroy the world as we know it.  Only you have the daring,\n"
    "derring-do, and derringers to stop these dastardly deviants in their\n"
    "tracks.  Your mission, should you choose to accept it, is to fight or\n"
    "sneak your way through the labyrinthine passages carved into the hills\n"
    "and find the central control room to thwart this plot once and for all.\n";
    mvprintw(0, 0, "%s", text1);
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
    int input;
    do {
        floors[current_level]->monsters_mutual_fov();
        wclear(level_win);
        floors[current_level]->print();
        wrefresh(level_win);
        
        show_messages();
        end_message_turn();
        
        wclear(stats_win);
        mvwprintw(stats_win, 0, 0, "HP %2i/%2i    Floor %i", player->get_hp(), player->get_max_hp(), current_level);
		if (floors[current_level]->contains_item(player->get_x_pos(), player->get_y_pos()))
			mvwprintw(stats_win, 0, 40, "There is a %s on the floor.", floors[current_level]->get_item(player->get_x_pos(), player->get_y_pos())->get_name());
		mvwprintw(stats_win, 1, 0, "Equipment:  Melee weapon: %-12s Suit: %s", player->get_inventory()->get_current_melee_weapon()->get_name(), player->get_inventory()->get_current_suit()->get_name());
		mvwprintw(stats_win, 2, 0, "           Ranged weapon: %-13s Hat: %s", player->get_inventory()->get_current_ranged_weapon()->get_name(), player->get_inventory()->get_current_hat()->get_name());
        wrefresh(stats_win);
        
        refresh();
        player_acted = false;
        input = player->take_turn();
        if(!game_over && player_acted)
            floors[current_level]->monsters_take_turns();
    } while(input != 'Q' && !game_over);
    if(game_lost) {
        show_messages();
        while(getch() != 'Q');
    }
}

void Game::end() {
    if(game_lost) {
        clear();
        mvprintw(0, 0, "Mission Report.\n%s ", name);
        char *text1 =
        "was sent on a mission three days ago.\n"
        "We have not had word since then. ";
        printw("%s", text1);
        mvprintw(3, 0, "%s", name);
        printw(" is assumed deceased.");
        mvprintw(5, 1, "Press any key to file this report. ");
        curs_set(2);
        getch();
        curs_set(0);
        mvprintw(6, 1, "Report filed.");
        getch();
    } else if(game_won) {
        clear();
        char *text1 =
        "After you descend the stairs, you find yourself in a control room.\n"
        "There seems to be nobody else here; All of the guards were on the\n"
        "upper floors of the installation.  You find the controls to shut down\n"
        "the base entirely, conveniently placed where anyone who manages to\n"
        "infiltrate the lower floors can activate them.  You swipe any papers\n"
        "that look like they would be useful for your organization.  Now all\n"
        "that remains is to wipe their files.\n";
        mvprintw(0, 0, "%s", text1);
        mvprintw(8, 1, "Press any key to delete the plans for the weapon system. ");
        curs_set(2);
        getch();
        curs_set(0);
        mvprintw(9, 1, "The files have been removed.\n\n"
        "Now all that remains is to escape this installation and meet with\n"
        "your contact outside, then to get back home.");
        getch();
        clear();
        mvprintw(0, 0, "Congratulations, %s!\n\nYou have won the game!\n\n\n\n"
        "This game was made for the 2010 seven day roguelike competition.\n\n"
        "Programming and Design by robalan and jjonir.\n\n"
        "Input and output use the public domain curses library.\n\n"
        "Thank you for playing!", name);
        getch();
    } else {
        //wtf
    }
}

//Go down a stair.  Create a new level if there is none.
//If this is the last level, win the game.
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
        win();
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

void Game::player_act() {
    player_acted = true;
}

void Game::end_message_turn() {
    if(message_line_1[0]) {
        strcpy(message_line_0, message_line_1);
        message_line_1[0] = '\0';
    }
}

void Game::win() {
    game_over = true;
    game_won = true;
}

void Game::lose() {
    game_over = true;
    game_lost = true;
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
    unsigned int mx = getmaxx(message_win);
    if(strlen(msg) + strlen(message_line_1) >= mx) {
        strcpy(message_line_0, message_line_1);
        message_line_1[0] = '\0';
    }
    strcat(message_line_1, msg);
    strcat(message_line_1, " ");
    show_messages();
}

//messages shown in aimmode or lookmode, don't keep them around.
void Game::write_temp_message(const char *msg) {
    wclear(message_win);
    wattron(message_win, COLOR_PAIR(2));
    mvwprintw(message_win, 0, 0, "%s", message_line_0);
    wattroff(message_win, COLOR_PAIR(2));
    mvwprintw(message_win, 1, 0, "%s", msg);
    wrefresh(message_win);
}

void Game::show_messages() {
    wclear(message_win);
    wattron(message_win, COLOR_PAIR(2));
    mvwprintw(message_win, 0, 0, "%s", message_line_0);
    wattroff(message_win, COLOR_PAIR(2));
    mvwprintw(message_win, 1, 0, "%s", message_line_1);
    wrefresh(message_win);
}
