#include <stdlib.h>
#include <math.h>
#include <curses.h>
#include "Agent.h"
#include "Player.h"
#include "Level.h"
#include "Game.h"

Player::Player(Level *loc, Game *parent) : Agent(loc->get_upstair_x(), loc->get_upstair_y(), 0, loc, parent) {
    walk_mode = TURNING;
    init_keys();
}

void Player::init_keys() {
    default_keys();
}

void Player::default_keys() {
    keys.walk_west = 'j';
    keys.walk_east = 'l';
    keys.walk_north = 'i';
    keys.walk_south = ',';
    keys.walk_nw = 'u';
    keys.walk_ne = 'o';
    keys.walk_sw = 'm';
    keys.walk_se = '.';
    keys.turn_left = 'h';
    keys.turn_right = ';';
    keys.use = 'k';
    keys.change_walk_mode = '/';
}

//Get input from the keyboard and act on input.  Then recalculate player's fov.
int Player::take_turn() {
    int input;
    input = getch();
    if(input == keys.walk_west) 
        walk(-1, 0);
    else if(input == keys.walk_east)
        walk(1, 0);
    else if(input == keys.walk_north)
        walk(0, -1);
    else if(input == keys.walk_south)
        walk(0, 1);
    else if(input == keys.walk_nw)
        walk(-1, -1);
    else if(input == keys.walk_sw)
        walk(-1, 1);
    else if(input == keys.walk_ne)
        walk(1, -1);
    else if(input == keys.walk_se)
        walk(1, 1);
    else if(input ==  keys.turn_left)
        turn(float(-PI/4));
    else if(input ==  keys.turn_right)
        turn(float(PI/4));
	else if(input ==  keys.use)
		use();
	else if(input ==  keys.change_walk_mode)
        toggle_walk_mode();
    else {
        
    }
    
    mutual_fov();
    return input;
}

void Player::walk(int x, int y) {
    if(walk_mode == TURNING)
        Agent::walk_turn(x, y);
    else if(walk_mode == STRAFING)
        Agent::walk(x, y);
}

void Player::toggle_walk_mode() {
    if(walk_mode == TURNING)
        walk_mode = STRAFING;
    else if(walk_mode == STRAFING)
        walk_mode = TURNING;
}

//Calculate FOV as for any agent, then update the map with this information.
void Player::mutual_fov() {
    location->clear_visibility();
    Agent::mutual_fov();
    for(int i = 0; i < n_visible_corners; i++) {
        location->mark_visible(visible_corners[i].x, visible_corners[i].y);
        location->mark_visible(visible_corners[i].x-1, visible_corners[i].y);
        location->mark_visible(visible_corners[i].x, visible_corners[i].y-1);
        location->mark_visible(visible_corners[i].x-1, visible_corners[i].y-1);
    }
}

//Use the thing where the player is standing: stairs...
void Player::use() {
	if (location->is_upstair(position.x, position.y)) {
		if (game->ascend()) {
			position.x = location->get_downstair_x();
			position.y = location->get_downstair_y();
		}
	}
	else if (location->is_downstair(position.x, position.y)) {
		if (game->descend()) {
			position.x = location->get_upstair_x();
			position.y = location->get_upstair_y();
		}
	}
}
