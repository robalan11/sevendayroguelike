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
    
    current_level = new Level(level_win);
    player = new Player(current_level);
    
    return TRUE;
}

void Game::play() {
    player->calculate_visibility();
    int input;
    do {
        wclear(level_win);
        current_level->print();
        mvwaddch(level_win, player->get_y_pos(), player->get_x_pos(), '@');
        wrefresh(level_win);
        
        wclear(message_win);
        mvwprintw(message_win, 0, 0, "Player at x:%i, y:%i", player->get_x_pos(), player->get_y_pos());
        wrefresh(message_win);
        
        refresh();
        input = player->take_turn(); 
    } while(input != 'Q');
}
