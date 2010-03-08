#include <curses.h>
#include "Agent.h"
#include "Level.h"

Agent::Agent(int x, int y, int f, Level *loc) {
    position.x = x;
    position.y = y;
    facing = f;
    location = loc;
}

void Agent::walk(int x, int y) {
    if(!(location->is_wall(position.x+x, position.y+y))) {
        position.x += x;
        position.y += y;
    }
}

void Agent::turn(int angle) {
    facing += angle;
    while(facing < 0) facing += 360;
    while(facing >= 360) facing -= 360;
}

void Agent::set_location(Level *loc) {
    location = loc;
}

int Agent::get_x_pos() {
    return position.x;
}

int Agent::get_y_pos() {
    return position.y;
}

int Agent::get_facing() {
    return facing;
}

int Agent::take_turn() {
    printw("Generic Player take_turn");
    return 0;
}
