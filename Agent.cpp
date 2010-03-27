#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <curses.h>
#include "Agent.h"
#include "Level.h"
#include "Game.h"
#include "Inventory.h"
#include "minmax.h"

Agent::Agent(int x, int y, float f, Level *loc, Game *parent) {
    position.x = x;
    position.y = y;
    facing = f;
    location = loc;
	game = parent;
    
    speed = 1;
    fov_angle = float(PI/3);
}

Agent::~Agent() {
    
}

//The agent moves in the specified direction and keeps its facing.
bool Agent::walk(int x, int y) {
    if(x != 0 || y != 0) {
        if(location->is_walkable(position.x+x, position.y+y)) {
            if(location->contains_agent(position.x+x, position.y+y)) {
                if(can_see(position.x+x, position.y+y))
                    attack(location->agent_at(position.x+x, position.y+y));
            }
            else {
                location->move_agent(position.x, position.y,
                    position.x+x, position.y+y);
                set_position(position.x+x, position.y+y);
            }
            return true;
        }
        else if(location->is_closed_door(position.x+x, position.y+y)) {
            location->open_door(position.x+x, position.y+y);
            char msg[80] = "";
            strcat(msg, get_name());
            strcat(msg, " open");
            if(is_player) strcat(msg, " ");
            else strcat(msg, "s ");
            strcat(msg, "a door.");
            msg[0] -= 0x20; //capitalize
            game->write_message(msg);
            return true;
        }
        else {
            char msg[80] = "";
            strcat(msg, get_name());
            strcat(msg, " bump");
            if(is_player) strcat(msg, " ");
            else strcat(msg, "s ");
            strcat(msg, "into an obstacle.");
            msg[0] -= 0x20; //capitalize
            game->write_message(msg);
            return true;
        }
    }
    return true;
}

//The agent moves in the specified direction and faces in that direction.
bool Agent::walk_turn(int x, int y) {
    if(x != 0 || y != 0) {
    	face(atan2(float(y), float(x)));
    	mutual_fov();
        return walk(x, y);
    }
    return true;
}

void Agent::attack(Agent *enemy) {
    char message[80] = "";
    strcat(message, get_name());
    strcat(message, " attack");
    if(is_player)
        strcat(message, " ");
    else
        strcat(message, "s ");
    strcat(message, enemy->get_name());
    strcat(message, ".");
    message[0] -= 0x20; //capitalize
    game->write_message(message);
    enemy->lose_hp(get_melee_damage());
}

void Agent::ranged_attack(int x, int y) {
    Position hit = location->shoot_projectile(position.x, position.y, x, y, ranged_accuracy, 10);
    
    char msg[80] = "";
    strcat(msg, get_name());
    strcat(msg, " shoot");
    if(!is_player)
        strcat(msg, "s");
    strcat(msg, ".");
    msg[0] -= 0x20;
    game->write_message(msg);
    
    if((hit.x == position.x) && (hit.y == position.y)) {
        //miss...
    } else {
        if(location->contains_agent(hit.x, hit.y)) {
            //hit an agent
            msg[0] = '\0';
            strcat(msg, get_name());
            strcat(msg, " hit");
            if(is_player)
                strcat(msg, " ");
            else
                strcat(msg, "s ");
            strcat(msg, location->agent_at(hit.x, hit.y)->get_name());
            strcat(msg, "!");
            msg[0] -= 0x20;
            game->write_message(msg);
            location->agent_at(hit.x, hit.y)->lose_hp(get_ranged_damage());
        } else if(location->is_wall(hit.x, hit.y) || location->is_closed_door(hit.x, hit.y)) {
            //hit a wall
        } else {
            //wtf
        }
    }
}

void Agent::lose_hp(int hurt) {
    if(is_player)
        hurt = max(0, hurt - ((Player *)this)->get_armor());
    hp -= hurt;
    if(hp <= 0)
        die();
}

bool Agent::can_move_forward() {
    int x = (int)(1.5*cos(facing));
    int y = (int)(1.5*sin(facing));
    return (location->is_walkable(position.x+x, position.y+y) &&
            !location->contains_agent(position.x+x, position.y+y));
}

void Agent::move_forward() {
    int x = (int)(1.5*cos(facing));
    int y = (int)(1.5*sin(facing));
    walk(x, y);
}

void Agent::die() {
    location->remove_agent(this);
}

int Agent::get_melee_damage() {
    return 0;
}

int Agent::get_ranged_damage() {
    return 0;
}

void Agent::gain_hp(int heal) {
	hp = min(hp + heal, max_hp);
}

void Agent::set_position(int x, int y) {
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
            float anglediff = float(facing - atan2(y-0.5, x-0.5));
            while(anglediff < -PI) anglediff += float(2*PI);
            while(anglediff >= PI) anglediff -= float(2*PI);
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

const char *Agent::get_name() {
    return "Generic Name";
}

bool Agent::can_see(int x, int y) {
    for(int i = 0; i < n_visible_corners; i++) {
        if(((visible_corners[i].x == x) || (visible_corners[i].x-1 == x)) &&
                ((visible_corners[i].y == y) || (visible_corners[i].y-1 == y)))
            return true;
    }
    return false;
}
