#include <stdlib.h>
#include <math.h>
#include <curses.h>
#include "Agent.h"
#include "Player.h"
#include "Level.h"
#include "Game.h"

Player::Player(Level *loc, Game *parent) : Agent(loc->get_upstair_x(), loc->get_upstair_y(), 0, loc, parent) {
    walk_mode = TURNING;
}

//Get input from the keyboard and act on input.  Then recalculate player's fov.
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
        case 'h':
            turn(float(-PI/4));
            break;
        case ';':
            turn(float(PI/4));
            break;
		case 'k':
			use();
			break;
		case '/':
            toggle_walk_mode();
            break;
        default:
            break;
    }
    
    mutual_fov();
    return input;
}

int Player::walk(int x, int y) {
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
