#include <curses.h>
#include "Agent.h"
#include "Player.h"
#include "Level.h"

Player::Player(int x, int y, int f, Level *loc) : Agent(x, y, f, loc) {
    
}

int Player::take_turn() {
    int input;
    input = getch();
    switch(input) {
        case 'j':
            walk(-1, 0);
            break;
        case 'l':
            walk(1, 0);
            break;
        case 'i':
            walk(0, -1);
            break;
        case ',':
            walk(0, 1);
            break;
        case 'u':
            walk(-1, -1);
            break;
        case 'm':
            walk(-1, 1);
            break;
        case 'o':
            walk(1, -1);
            break;
        case '.':
            walk(1, 1);
            break;
        default:
            break;
    }
    return input;
}
