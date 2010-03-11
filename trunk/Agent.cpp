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
    location->add_agent(this, position.x, position.y);
	game = parent;
    
    speed = 1;
    vision = 8;
    fov_angle = PI/3;
    
    visible_corners = (Position *)calloc((2*vision) * (2*vision), sizeof(Position));
}

//The agent moves in the specified direction and keeps its facing.
void Agent::walk(int x, int y) {
    if(location->is_walkable(position.x+x, position.y+y)) {
        if(location->contains_agent(position.x+x, position.y+y)) {
            //attack it if visible
        }
        else {
            set_position(position.x+x, position.y+y);
        }
    }
    else if(location->is_closed_door(position.x+x, position.y+y)) {
        location->open_door(position.x+x, position.y+y);
    }
}

//The agent moves in the specified direction and faces in that direction.
void Agent::walk_turn(int x, int y) {
	face(atan2(float(y), float(x)));
    if(location->is_walkable(position.x+x, position.y+y)) {
        if(location->contains_agent(position.x+x, position.y+y)) {
            //attack it if visible?
        }
        else {
            set_position(position.x+x, position.y+y);
        }
    }
	else if(location->is_closed_door(position.x+x, position.y+y)) {
		location->open_door(position.x+x, position.y+y);
	}
}

void Agent::set_position(int x, int y) {
    location->move_agent(position.x, position.y, x, y);
    position.x = x;
    position.y = y;
}

//The agent turns counterclockwise by an angle.
void Agent::turn(float angle) {
    facing += angle;
    while(facing < float(-PI)) facing += float(2*PI);
    while(facing >= float(PI)) facing -= float(2*PI);
}

//The agent faces an angle, with 0 = North.
void Agent::face(float angle) {
	if (angle < 0) angle += float(2*PI);
	facing = angle;
}

//Set the level the agent inhabits and set the agent's position in the level.
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

//Determine which squares this agent can see.
void Agent::mutual_fov() {
    int i = 0;
    for(int y = -vision; y <= vision+1; y++) {
        for(int x = -vision; x <= vision+1; x++) {
            //don't test points off the map
            if(position.x+x >= map_width || position.y+y >= map_height ||
				position.x+x < 1 || position.y+y < 1) continue;
			
			//don't test points outside sight radius
			float xdist = float(x-0.5);
			float ydist = float(y-0.5);
            if(sqrt((xdist*xdist)+(ydist*ydist)) > vision) continue;
            
            //don't test points outside sight cone
            float anglediff = facing - atan2(y-0.5, x-0.5);
            while(anglediff < -PI) anglediff += 2*PI;
            while(anglediff >= PI) anglediff -= 2*PI;
            if(fabs(anglediff) > fov_angle) continue;
            
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
