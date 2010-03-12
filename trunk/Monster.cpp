#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Agent.h"
#include "Monster.h"
#include "Level.h"
#include "Game.h"

Monster::Monster(int x, int y, float f, Level *loc, Game *parent, int monster_type) : Agent(x, y, f, loc, parent) {
    is_player = false;
    type = monster_type;
    can_see_player = could_see_player = false;
    tenacity = 0; //for now
    switch(monster_type) {
        case DOG:
            symbol = 'd';
            hp = max_hp = 15;
            attack_strength = 5;
            ranged_accuracy = 0; //no ranged attack
            ranged_attack = 0;
            vision--;vision--; //why do it this way, just set this in player or monster instead
            break;
        default:
            symbol = 'g'; //default is a goblin; there are no goblins in this game.
            hp = max_hp = 15;
            attack_strength = 10;
            ranged_accuracy = 2; //throws rocks
            ranged_attack = 2;
            break;
    }
}

void Monster::die() {
    Agent::die();
    game->add_dead_agent(this);
    location->spawn_corpse(position.x, position.y, type);
    char message[80] = "";
    strcat(message, MONSTER_NAME(type));
    strcat(message, " dies.");
    message[0] -= 0x20; //capitalize
    game->write_message(message);
}

int Monster::take_turn() {
    mutual_fov();
    if(can_see_player) {
        //monster can see player right now
        //attack or approach
        Agent *p = game->get_player();
        int dx = p->get_x_pos() - position.x;
        int dy = p->get_y_pos() - position.y;
        if(abs(dx) <= 1 && abs(dy) <= 1) {
            attack(p);
        }
        else {
            //move toward
            //if facing nearly toward player, move forward.
            float dtheta = atan2(float(dy), float(dx)) - facing;
            while(dtheta <= (float)(-PI)) dtheta += (float)(2*PI);
            while(dtheta > (float)(PI)) dtheta -= (float)(2*PI);
            if(fabs(dtheta) < (float)PI/6) move_forward();
            else {
                //otherwise, move toward the player.
                float theta = atan2(float(dy), float(dx));
                int x = (int)(1.5*cos(theta));
                int y = (int)(1.5*sin(theta));
                walk(x, y);
            }
        }
    }
    else if(could_see_player) {
        //monster could see player a few turns ago
        //try to find player
    }
    else {
        //monster forgot about / never saw the player
        //random walk
        bool reroll;
        do {
            reroll = false;
            int action = rand() % 100;
            if(action < 60) {
                if(can_move_forward())
                    move_forward();
                else
                    reroll = true;
            }
            else if(action < 90) {
                int angle = rand() % 4;
                if(angle == 0) turn((float)(-PI/2));
                else if(angle == 1) turn((float)(-PI/4));
                else if(angle == 2) turn((float)(PI/4));
                else turn((float)(PI/2));
            }
            else {
                wait();
            }
        } while(reroll);
    }
    mutual_fov();
    mark_danger();
    time_since_player_seen++;
    return 0;
}

void Monster::mutual_fov() {
    Agent::mutual_fov();
    //determine what it can see, and what it means...
    if(can_see(game->get_player()->get_x_pos(), game->get_player()->get_y_pos())) {
        can_see_player = true;
        time_since_player_seen = 0;
        player_last_seen.x = game->get_player()->get_x_pos();
        player_last_seen.y = game->get_player()->get_y_pos();
    } else can_see_player = false;
    could_see_player = (can_see_player || (time_since_player_seen < tenacity));
}

void Monster::mark_danger() {
    Agent *p = game->get_player();
    //mark the map with visible tiles if player can see this agent
    if(p->can_see(position.x, position.y)) {
        for(int i = 0; i < n_visible_corners; i++) {
            if(p->can_see(visible_corners[i].x, visible_corners[i].y))
                location->mark_dangerous(visible_corners[i].x, visible_corners[i].y);
            if(p->can_see(visible_corners[i].x-1, visible_corners[i].y))
                location->mark_dangerous(visible_corners[i].x-1, visible_corners[i].y);
            if(p->can_see(visible_corners[i].x, visible_corners[i].y-1))
                location->mark_dangerous(visible_corners[i].x, visible_corners[i].y-1);
            if(p->can_see(visible_corners[i].x-1, visible_corners[i].y-1))
                location->mark_dangerous(visible_corners[i].x-1, visible_corners[i].y-1);
        }
    }
}

char *Monster::get_name() {
    return MONSTER_NAME(type);
}
