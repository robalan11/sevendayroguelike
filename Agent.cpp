#include <math.h>
#include <curses.h>
#include "Agent.h"
#include "Level.h"

Agent::Agent(int x, int y, float f, Level *loc) {
    position.x = x;
    position.y = y;
    facing = f;
    location = loc;
}

void Agent::walk(int x, int y) {
	face(atan2(float(y), float(x)));
    if(location->is_walkable(position.x+x, position.y+y)) {
        position.x += x;
        position.y += y;
    }
	else if(location->is_closed_door(position.x+x, position.y+y)) {
		location->open_door(position.x+x, position.y+y);
	}
}

void Agent::turn(float angle) {
    facing += angle;
    while(facing < 0) facing += 360;
    while(facing >= 360) facing -= 360;
}

void Agent::face(float angle) {
	if (angle < 0) angle += float(2*PI);
	facing = angle;
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

float Agent::get_facing() {
    return facing;
}

int Agent::take_turn() {
    printw("Generic Player take_turn");
    return 0;
}

int Agent::calculate_visibility() {
    printw("Generic Player calculate_visibility");
    return 0;
}
