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
            move(-1, 0);
            break;
        case 'l':
            move(1, 0);
            break;
        case 'i':
            move(0, -1);
            break;
        case ',':
            move(0, 1);
            break;
        case 'u':
            move(-1, -1);
            break;
        case 'm':
            move(-1, 1);
            break;
        case 'o':
            move(1, -1);
            break;
        case '.':
            move(1, 1);
            break;
        default:
            break;
    }
    return input;
}
