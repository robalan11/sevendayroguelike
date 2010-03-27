#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <curses.h>
#include "Agent.h"
#include "Player.h"
#include "Level.h"
#include "Game.h"
#include "Item.h"

Player::Player(Level *loc, Game *parent) : Agent(loc->get_upstair_x(), loc->get_upstair_y(), 0, loc, parent) {
    is_player = true;
    walk_mode = TURNING;
    symbol = '@';
    hp = max_hp = 20;
    attack_strength = 2;
    ranged_accuracy = 9;
    vision = 10;
    init_keys();
    last_targeted = NULL;
	inventory = new Inventory(game->get_inventory_win(), this);
	visible_corners = (Position *)calloc((2*vision) * (2*vision), sizeof(Position));
}

Player::~Player() {
    free(visible_corners);
    delete inventory;
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
	keys.inventory = 's';
	keys.close = 'c';
	keys.fire = 'f';
	keys.look = 'v';
	keys.wait = '-';
}

void Player::die() {
    Agent::die();
    game->write_message("You die!  Press 'Q' to end the game.");
    game->lose();
}

void Player::wait() {
    Agent::wait();
    game->player_act();
}

void Player::close_door() {
    int n_open_doors = 0;
    Position door;
    for(int y = -1; y <= 1; y++)
        for(int x = -1; x <= 1; x++)
            if(x != 0 || y != 0)
                if(location->is_open_door(position.x+x, position.y+y)) {
                    n_open_doors++;
                    door.x = position.x+x;
                    door.y = position.y+y;
                }
    int result;
    #define NO_DOORS 0
    #define NO_DOOR_THAT_DIRECTION 1
    #define DOOR_OBSTRUCTED 2
    #define DOOR_CLOSED_ALREADY 3
    #define DOOR_CLOSE_SUCCESS 4
    if(n_open_doors == 0) {
        result = NO_DOORS;
    }
    else if(n_open_doors == 1) {
        if(location->close_door(door.x, door.y))
            result = DOOR_CLOSE_SUCCESS;
        else
            result = DOOR_OBSTRUCTED;
    } else {
        bool input_ok;
        game->write_message("Close a door in what direction?");
        do {
            input_ok = true;
            int input = getch();
            if(key2pos_rel(input, &door)) {
                input_ok = false;
                game->write_message("I didn't catch that.  What direction?");
            }
        } while(!input_ok);
        if(location->close_door(door.x, door.y))
            result = DOOR_CLOSE_SUCCESS;
        else if(location->is_closed_door(door.x, door.y))
            result = DOOR_CLOSED_ALREADY;
        else if(location->is_open_door(door.x, door.y))
            result = DOOR_OBSTRUCTED;
        else
            result = NO_DOOR_THAT_DIRECTION;
    }
    char msg[80] = "";
    switch(result) {
        case NO_DOORS:
            strcat(msg, "There are no doors you can close.");
            break;
        case NO_DOOR_THAT_DIRECTION:
            strcat(msg, "There is no door in that direction.");
            break;
        case DOOR_OBSTRUCTED:
            strcat(msg, "You try to close a door, but there something in the way.");
            break;
        case DOOR_CLOSED_ALREADY:
            strcat(msg, "That door is already closed.");
            break;
        case DOOR_CLOSE_SUCCESS:
            strcat(msg, "You close a door.");
            game->player_act();
            break;
        default:
            strcat(msg, "yay?");
            break;
    }
    game->write_message(msg);
}

int Player::get_melee_damage() {
    return attack_strength +
        ((Weapon *)(inventory->get_current_melee_weapon()))->damage();
}

int Player::get_ranged_damage() {
    return ((Weapon *)(inventory->get_current_ranged_weapon()))->damage();
}

int Player::get_armor() {
    return ((Armor *)(inventory->get_current_suit()))->get_defense() +
            ((Armor *)(inventory->get_current_hat()))->get_defense();
}

bool Player::key2pos_abs(int c, Position *result) {
    Position rel;
    bool ret = key2pos_rel(c, &rel);
    result->x = position.x + rel.x;
    result->y = position.y + rel.y;
    return ret;
}

bool Player::key2pos_rel(int c, Position *result) {
    bool ret = true;
    if(c == keys.walk_west) {
        result->x = -1;
        result->y = 0;
    } else if(c == keys.walk_east) {
        result->x = 1;
        result->y = 0;
    } else if(c == keys.walk_north) {
        result->x = 0;
        result->y = -1;
    } else if(c == keys.walk_south) {
        result->x = 0;
        result->y = 1;
    } else if(c == keys.walk_nw) {
        result->x = -1;
        result->y = -1;
    } else if(c == keys.walk_sw) {
        result->x = -1;
        result->y = 1;
    } else if(c == keys.walk_ne) {
        result->x = 1;
        result->y = -1;
    } else if(c == keys.walk_se) {
        result->x = 1;
        result->y = 1;
    } else {
        ret = false;
    }
    return ret;
}

//Get input from the keyboard and act on input.  Then recalculate player's fov.
int Player::take_turn() {
    int input;
    input = getch();
    Position destination;
    if(key2pos_rel(input, &destination))
        walk(destination.x, destination.y);
    else if(input == keys.turn_left)
        turn(float(-PI/4));
    else if(input == keys.turn_right)
        turn(float(PI/4));
	else if(input == keys.use)
		use();
	else if(input == keys.change_walk_mode)
        toggle_walk_mode();
	else if(input == keys.inventory)
		inventory->open();
	else if(input == keys.close)
        close_door();
    else if(input == keys.fire)
        fire();
    else if(input == keys.look)
        look_mode();
    else if(input == keys.wait)
        wait();
    else {
        
    }
    
    mutual_fov();
    return input;
}

void Player::fire() {
    if(inventory->get_ammo() == 0) {
        char msg[80] = "";
        strcat(msg, "You have no ammunition for the ");
        strcat(msg, inventory->get_current_ranged_weapon()->get_name());
        strcat(msg, ".");
        game->write_message(msg);
    } else {
        Position aim;
        Agent_List *visible_agents = location->get_targets(this);
        //try to find the last targeted
        if(last_targeted) {
            Agent_List *ref = visible_agents;
            visible_agents = visible_agents->next;
            while(visible_agents != ref) {
                if(visible_agents->agent == last_targeted)
                    break;
            }
            if(visible_agents->agent != last_targeted)
                visible_agents = ref;
        }
        
        aim.x = visible_agents->agent->get_x_pos();
        aim.y = visible_agents->agent->get_y_pos();
        int input;
        do {
            location->print();
            location->print_path(position.x, position.y, aim.x, aim.y);
            look_at(aim.x, aim.y);
            input = getch();
            Position dtarget;
            if(key2pos_rel(input, &dtarget)) {
                aim.x += dtarget.x;
                aim.y += dtarget.y;
            }
            else if(input == '\t') {
                visible_agents = visible_agents->next;
                aim.x = visible_agents->agent->get_x_pos();
                aim.y = visible_agents->agent->get_y_pos();
            }
        } while((input != keys.fire) && (input != keys.use));
        location->print();
        if(input == keys.fire) {
            if((aim.x == position.x) && (aim.y == position.y)) {
                game->write_message("If you want to die, try standing around near some dogs.");
            } else {
                ranged_attack(aim.x, aim.y);
                inventory->use_ammo();
                game->player_act();
            }
        }
        Agent_List *start = visible_agents;
        Agent_List *prev = visible_agents->next;
        visible_agents = prev;
        while(visible_agents != start->next) {
            visible_agents = visible_agents->next;
            free(prev);
            prev = visible_agents;
        }
    }
}

void Player::turn(float angle) {
    Agent::turn(angle);
    game->player_act();
}

void Player::walk(int x, int y) {
    if(walk_mode == TURNING) {
        if(Agent::walk_turn(x, y))
            game->player_act();
    }
    else if(walk_mode == STRAFING) {
        if(Agent::walk(x, y))
            game->player_act();
    }
}

void Player::toggle_walk_mode() {
    if(walk_mode == TURNING)
        walk_mode = STRAFING;
    else if(walk_mode == STRAFING)
        walk_mode = TURNING;
}

void Player::look_mode() {
    curs_set(1);
    Position look_pos;
    look_pos.x = position.x;
    look_pos.y = position.y;
    int input;
    do {
        look_at(look_pos.x, look_pos.y);
        
        move(look_pos.y+2, look_pos.x+1); //kludge
        refresh();
        input = getch();
        Position rel;
        if(key2pos_rel(input, &rel)) {
            look_pos.x += rel.x;
            look_pos.y += rel.y;
        }
        //show more if requested
    } while(input != keys.use);
    curs_set(0);
}

void Player::look_at(int x, int y) {
    if(location->revealed(x, y)) {
        char msg[80] = "You ";
        if(location->visible(x, y)) {
            strcat(msg, "see : ");
            if(location->contains_agent(x, y)) {
                strcat(msg, location->agent_at(x, y)->get_name());
                strcat(msg, ", ");
            }
            if(location->contains_item(x, y)) {
                strcat(msg, location->get_item(x, y)->get_name());
                strcat(msg, ", ");
            }
        }
        else {
            strcat(msg, "recall : ");
        }
        strcat(msg, location->get_tile_name(x, y));
        game->write_temp_message(msg);
    }
    else {
        game->write_temp_message("You know nothing about this location.");
    }
}

//Calculate FOV as for any agent, then update the map with this information.
void Player::mutual_fov() {
    location->clear_visibility();
    location->clear_dangerousness();
    Agent::mutual_fov();
    for(int i = 0; i < n_visible_corners; i++) {
        location->mark_visible(visible_corners[i].x, visible_corners[i].y);
        location->mark_visible(visible_corners[i].x-1, visible_corners[i].y);
        location->mark_visible(visible_corners[i].x, visible_corners[i].y-1);
        location->mark_visible(visible_corners[i].x-1, visible_corners[i].y-1);
    }
}

const char *Player::get_name() {
    return "you";
}

//Use the thing where the player is standing: stairs...
void Player::use() {
	if (location->is_upstair(position.x, position.y)) {
		if (game->ascend()) {
            set_position(location->get_downstair_x(), location->get_downstair_y());
            location->add_agent(this);
		}
	}
	else if (location->is_downstair(position.x, position.y)) {
		if (game->descend()) {
            set_position(location->get_upstair_x(), location->get_upstair_y());
            location->add_agent(this);
		}
	}
	else if (location->contains_item(position.x, position.y)) {
        Item *i = location->get_item(position.x, position.y);
		inventory->add_item(i);
		location->remove_item(position.x, position.y);
		char msg[80] = "You pick up a ";
		strcat(msg, i->get_name());
		strcat(msg, ".");
		game->write_message(msg);
		game->player_act();
	}
}
