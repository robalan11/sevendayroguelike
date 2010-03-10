#include <stdlib.h>
#include <math.h>
#include <curses.h>
#include "Agent.h"
#include "Level.h"
#include "Game.h"

Agent::Agent(int x, int y, float f, Level *loc, Game *parent) {
    position.x = x;
    position.y = y;
    facing = f;
    location = loc;
	game = parent;
    
    speed = 1;
    vision = 8;
    
    visible_corners = (Position *)calloc((2*vision) * (2*vision), sizeof(Position));
}

//The agent moves in the specified direction and keeps its facing.
void Agent::walk(int x, int y) {
    if(location->is_walkable(position.x+x, position.y+y)) {
        position.x += x;
        position.y += y;
    }
    else if(location->is_closed_door(position.x+x, position.y+y)) {
        location->open_door(position.x+x, position.y+y);
    }
}

//The agent moves in the specified direction and faces in that direction.
void Agent::walk_turn(int x, int y) {
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
    while(facing < float(-PI)) facing += float(2*PI);
    while(facing >= float(PI)) facing -= float(2*PI);
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

void Agent::mutual_fov() {
    int i = 0;
    for(int y = -vision; y <= vision+1; y++) {
        for(int x = -vision; x <= vision+1; x++) {
            //don't test squares off the map
            if(position.x+x >= map_width || position.y+y >= map_height ||
				position.x+x < 1 || position.y+y < 1) continue;
			
			//don't test squares outside sight radius
			float xdist = float(x-0.5);
			float ydist = float(y-0.5);
            if(sqrt((xdist*xdist)+(ydist*ydist)) > vision) continue;
            
            //add corner to the visible list if it is visible
            if(location->is_visible(position.x, position.y,
                    position.x+x, position.y+y)) {
                visible_corners[i].x = position.x+x;
                visible_corners[i].y = position.y+y;
                i++;
            }
        }
    }
    n_visible_corners = i;
}
