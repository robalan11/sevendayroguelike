#include <curses.h>
#include "Agent.h"
#include "Player.h"
#include "Monster.h"

int main() {
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    raw();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    attron(COLOR_PAIR(1));
    
    Agent *player = new Player(10, 10, 0);
    
    int input;
    do {
        clear();
        mvaddch(player->get_y_pos(), player->get_x_pos(), '@');
        refresh();
        input = player->take_turn(); 
    } while(input != 'Q');
    
    attroff(COLOR_PAIR(1));
    endwin();
    return 0;
}
