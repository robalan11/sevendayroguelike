#include <stdlib.h>
#include <math.h>
#include <curses.h>
#include "Agent.h"
#include "Player.h"
#include "Level.h"

Player::Player(Level *loc) : Agent(loc->get_upstair_x(), loc->get_upstair_y(), 0, loc) {
    
}

Player::Player(int x, int y, float f, Level *loc) : Agent(x, y, f, loc) {
    
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
    
    calculate_visibility();
    return input;
}

int Player::calculate_visibility() {
    location->clear_visibility();
	for (int i = -5; i <= 5; i++) {
		for (int j = -5; j <= 5; j++) {
			if (position.x+i > win_width || position.y+j > win_height ||
				position.x+i < 0 || position.y+j < 0) continue;

			float angle = atan2(float(j),float(i));
			if (angle < 0) angle += float(2*PI);
			float anglediff = angle - facing;
			if (anglediff > PI) anglediff -= float(2*PI);
			if (fabs(anglediff) > PI/3 && !(i==0 && j==0)) continue;

			if (location->is_visible(position.x, position.y, position.x+i, position.y+j))
				location->mark_visible(position.x+i, position.y+j);
		}
	}
    return 0;
}
