#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Agent.h"
#include "Monster.h"
#include "Level.h"
#include "Item.h"
#include "minmax.h"
#include "Game.h"

class Game;

Level::Level(WINDOW *win, Game *parent) {
	game = parent;
	generate();
	agents = (Agent_List *)malloc(sizeof(Agent_List)); //need to free this and children in destructor
	agents->next = NULL;
	level_win = win;
	spawn_monster(DOG);
}

Level::~Level() {}

void Level::generate() {
	// Declare stuff and fill the map with spaces
	rooms = new Room[map_width];
	numrooms = 0;
	int roomfilled = 0;
	srand((unsigned int)time(NULL));
	for (int j = 0; j < map_height; j++) {
		for (int i = 0; i < map_width; i++) {
			map[i][j].symbol = ' ';
			map[i][j].revealed = false;
			map[i][j].visible = false;
			map[i][j].agent = NULL;
			map[i][j].stuff = NULL;
		}
	}

	// Add rooms until the map is 50% full of room tiles
	while(roomfilled < 0.5 * map_width * map_height) {
		Room temp = rect_room();
		
		bool valid = true;
		for (int i=0; i < numrooms; i++) {
			if (room_intersect(temp, rooms[i])) valid = false;
		}
		if (!valid) continue;
		
		for (int k = temp.top; k <= temp.bottom; k++) {
			for (int j = temp.left; j <= temp.right; j++) {
				if (map[j][k].symbol == ' ') {
					roomfilled ++;
					//if (j==temp.left || k==temp.top || j==temp.right || k==temp.bottom)
					//	map[j][k] = '#';
					//else
					map[j][k].symbol = '.';
				}
			}
		}

		rooms[numrooms] = temp;
		numrooms++;
	}

	// Add items
	add_items();

	// Connect rooms
	rooms[0].connected = true;
	bool allroomsconnected = false;
	while (!allroomsconnected) {
		Room source = rooms[rand()%numrooms];
		while (!source.connected) source = rooms[rand()%numrooms];

		Room target = rooms[rand()%numrooms];
		while (target.connected) target = rooms[rand()%numrooms];

		int startx = rand()%(source.right-source.left) + source.left;
		int starty = rand()%(source.bottom-source.top) + source.top;
		int endx = rand()%(target.right-target.left) + target.left;
		int endy = rand()%(target.bottom-target.top) + target.top;
		int dx = endx > startx ? 1 : endx < startx ? -1 : 0;
		int dy = endy > starty ? 1 : endy < starty ? -1 : 0;
		int x = startx, y = starty;

		while (!point_in_room(x, y, target)) {
			int dir = rand()%2;
			if (dir && x != endx) x += dx;
			else if (y != endy) y += dy;
			else x += dx;

			map[x][y].symbol = '.';
			for (int i = 0; i < numrooms; i++)
				if (point_in_room(x, y, rooms[i])) rooms[i].connected = true;
		}

		// Check to see if all rooms are connected
		allroomsconnected = true;
		for (int i = 0; i < numrooms; i++) {
			if (!rooms[i].connected) {
				allroomsconnected = false;
				break;
			}
		}
		// If all the rooms are connected, add stairs
		if (allroomsconnected) {
            upstair.x = rand() % (rooms[0].right - rooms[0].left) + rooms[0].left;
            upstair.y = rand() % (rooms[0].bottom - rooms[0].top) + rooms[0].top;
			map[upstair.x][upstair.y].symbol = '<';
			downstair.x = rand() % (target.right - target.left) + target.left;
			downstair.y = rand() % (target.bottom - target.top) + target.top;
			map[downstair.x][downstair.y].symbol = '>';
		}
	}
	
	// Fill walls
	for (int j = 0; j < map_height; j++) {
		for (int i = 0; i < map_width; i++) {
			if (map[i][j].symbol == ' ') map[i][j].symbol = '#';
		}
	}

	// Add doors
	for (int j = 0; j < map_height; j++) {
		for (int i = 0; i < map_width; i++) {
			bool good = false;
			if (map[i][j].symbol == '.' && map[i-1][j].symbol == '.' && map[i+1][j].symbol == '.' && map[i][j-1].symbol == '#' && map[i][j+1].symbol == '#') {
				for (int k = 0; k < numrooms; k++) {
					if (point_in_room(i-1, j, rooms[k]) || point_in_room(i+1, j, rooms[k])) {
						good = true;
						break;
					}
				}
			}
			else if (map[i][j].symbol == '.' && map[i][j-1].symbol == '.' && map[i][j+1].symbol == '.' && map[i-1][j].symbol == '#' && map[i+1][j].symbol == '#') {
				for (int k = 0; k < numrooms; k++) {
					if (point_in_room(i, j-1, rooms[k]) || point_in_room(i, j+1, rooms[k])) {
						good = true;
						break;
					}
				}
			}
			if (good) map[i][j].symbol = '+';
		}
	}
}

void Level::add_items() {
	// Drinks
	Item *beer = new Drink("Beer");
	for (int i = 0; i < 6 - game->get_current_level(); i++) {
		int x = rand()%map_width;
		int y = rand()%map_height;
		if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
			map[x][y].stuff = beer;
		else
			i--;
	}
	Item *wine = new Drink("Wine");
	for (int i = 0; i < 6 - abs(5 - game->get_current_level()); i++) {
		int x = rand()%map_width;
		int y = rand()%map_height;
		if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
			map[x][y].stuff = wine;
		else
			i--;
	}
	Item *shot = new Drink("Shot");
	for (int i = 0; i < 6 - abs(10 - game->get_current_level()); i++) {
		int x = rand()%map_width;
		int y = rand()%map_height;
		if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
			map[x][y].stuff = shot;
		else
			i--;
	}
	Item *martini = new Drink("Martini");
	for (int i = 0; i < game->get_current_level() - 9; i++) {
		int x = rand()%map_width;
		int y = rand()%map_height;
		if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
			map[x][y].stuff = martini;
		else
			i--;
	}

	// Ammunition
	Item *derringer_rounds = new Ammo("Derringer");
	for (int i = 0; i < max(4 - game->get_current_level(), 1); i++) {
		int x = rand()%map_width;
		int y = rand()%map_height;
		if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
			map[x][y].stuff = derringer_rounds;
		else
			i--;
	}
	if (game->get_current_level() >= 2) {
		Item *luger_rounds = new Ammo("Luger");
		for (int i = 0; i < max(6 - game->get_current_level(), 1); i++) {
			int x = rand()%map_width;
			int y = rand()%map_height;
			if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
				map[x][y].stuff = luger_rounds;
			else
				i--;
		}
	}
	if (game->get_current_level() >= 6) {
		Item *colt_rounds = new Ammo("Colt 45");
		for (int i = 0; i < max(10 - game->get_current_level(), 2); i++) {
			int x = rand()%map_width;
			int y = rand()%map_height;
			if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
				map[x][y].stuff = colt_rounds;
			else
				i--;
		}
	}
	if (game->get_current_level() >= 10) {
		Item *walther_rounds = new Ammo("Walther PPK");
		for (int i = 0; i < max(15 - game->get_current_level(), 4); i++) {
			int x = rand()%map_width;
			int y = rand()%map_height;
			if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
				map[x][y].stuff = walther_rounds;
			else
				i--;
		}
	}

	// Weapons
	if (game->get_current_level() == 2) {
		Item *luger = new Weapon("Luger");
		for (int i = 0; i < 1; i++) {
			int x = rand()%map_width;
			int y = rand()%map_height;
			if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
				map[x][y].stuff = luger;
			else
				i--;
		}
	}
	if (game->get_current_level() == 4) {
		Item *switchblade = new Weapon("Switchblade");
		for (int i = 0; i < 1; i++) {
			int x = rand()%map_width;
			int y = rand()%map_height;
			if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
				map[x][y].stuff = switchblade;
			else
				i--;
		}
	}
	if (game->get_current_level() == 6) {
		Item *colt = new Weapon("Colt 45");
		for (int i = 0; i < 1; i++) {
			int x = rand()%map_width;
			int y = rand()%map_height;
			if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
				map[x][y].stuff = colt;
			else
				i--;
		}
	}
	if (game->get_current_level() == 8) {
		Item *butterfly = new Weapon("Butterfly Knife");
		for (int i = 0; i < 1; i++) {
			int x = rand()%map_width;
			int y = rand()%map_height;
			if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
				map[x][y].stuff = butterfly;
			else
				i--;
		}
	}
	if (game->get_current_level() == 10) {
		Item *walther = new Weapon("Walther PPK");
		for (int i = 0; i < 1; i++) {
			int x = rand()%map_width;
			int y = rand()%map_height;
			if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
				map[x][y].stuff = walther;
			else
				i--;
		}
	}

	// Armor
	if (game->get_current_level() == 1) {
		Item *ushanka = new Armor("Ushanka");
		for (int i = 0; i < 1; i++) {
			int x = rand()%map_width;
			int y = rand()%map_height;
			if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
				map[x][y].stuff = ushanka;
			else
				i--;
		}
	}
	if (game->get_current_level() == 3) {
		Item *business = new Armor("Business Suit");
		for (int i = 0; i < 1; i++) {
			int x = rand()%map_width;
			int y = rand()%map_height;
			if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
				map[x][y].stuff = business;
			else
				i--;
		}
	}
	if (game->get_current_level() == 5) {
		Item *bowler = new Armor("Bowler");
		for (int i = 0; i < 1; i++) {
			int x = rand()%map_width;
			int y = rand()%map_height;
			if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
				map[x][y].stuff = bowler;
			else
				i--;
		}
	}
	if (game->get_current_level() == 7) {
		Item *kevlar = new Armor("Kevlar Suit");
		for (int i = 0; i < 1; i++) {
			int x = rand()%map_width;
			int y = rand()%map_height;
			if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
				map[x][y].stuff = kevlar;
			else
				i--;
		}
	}
	if (game->get_current_level() == 9) {
		Item *fedora = new Armor("Fedora");
		for (int i = 0; i < 1; i++) {
			int x = rand()%map_width;
			int y = rand()%map_height;
			if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
				map[x][y].stuff = fedora;
			else
				i--;
		}
	}
	if (game->get_current_level() == 11) {
		Item *tuxedo = new Armor("Tuxedo");
		for (int i = 0; i < 1; i++) {
			int x = rand()%map_width;
			int y = rand()%map_height;
			if (map[x][y].symbol == '.' && map[x][y].stuff == NULL)
				map[x][y].stuff = tuxedo;
			else
				i--;
		}
	}
}

Room Level::rect_room() {
int x = rand() % (int)floor(map_width * 0.9) + 1;
		int width = rand() % (int)floor(map_width * 0.25) + int(map_width * 0.15);
		int y = rand() % (int)floor(map_height * 0.9) + 1;
		int height = rand() % (int)floor(map_height * 0.3) + int(map_height * 0.15);

		Room temp;
		temp.connected = false;
		temp.left = x;
		temp.right = min(x + width, map_width-2);
		temp.top = y;
		temp.bottom = min(y + height, map_height-2);
		
		return temp;
}

bool Level::room_intersect(Room a, Room b) {
	return (!(a.bottom < b.top || a.right < b.left || a.top > b.bottom || a.left > b.right));
}

bool Level::point_in_room(int x, int y, Room a) {
	return (x <= a.right && x >= a.left && y <= a.bottom && y >= a.top);
}

void Level::mark_visible(int x, int y) {
    map[x][y].visible = true;
    map[x][y].revealed = true;
}

void Level::clear_visibility() {
    for(int j = 0; j < map_height; j++)
        for(int i = 0; i < map_width; i++)
            map[i][j].visible = false;
}

void Level::mark_dangerous(int x, int y) {
    map[x][y].dangerous = true;
}

void Level::clear_dangerousness() {
    for(int j = 0; j < map_height; j++)
        for(int i = 0; i < map_width; i++)
            map[i][j].dangerous = false;
}

void Level::open_door(int x, int y) {
	if (map[x][y].symbol == '+')
		map[x][y].symbol = '/';
}

bool Level::close_door(int x, int y) {
    if(map[x][y].symbol == '/')
        if(!contains_agent(x, y)&& !contains_item(x, y)) {
            map[x][y].symbol = '+';
            return true;
        }
        else return false;
    else return false;
}

void Level::monsters_take_turns() {
    Agent_List *l = agents;
    while(l->next) {
        if(!l->next->agent->is_player) {
            l->next->agent->take_turn();
        }
        l = l->next;
    }
}

void Level::add_agent(Agent *agent) {
    map[agent->get_x_pos()][agent->get_y_pos()].agent = agent;
    Agent_List *l = agents;
    while(l->next) l = l->next;
    Agent_List *new_agent = (Agent_List *)malloc(sizeof(Agent_List));
    new_agent->agent = agent;
    new_agent->next = NULL;
    l->next = new_agent;
}

void Level::remove_agent(Agent *agent) {
    Agent_List *l = agents;
    while(l != NULL) {
        if(l->next->agent == agent) {
            l->next = l->next->next;
            break;
        }
        l = l->next;
    }
    map[agent->get_x_pos()][agent->get_y_pos()].agent = NULL;
}

void Level::move_agent(int x1, int y1, int x2, int y2) {
    if((map[x1][y1].agent != NULL) && (map[x2][y2].agent == NULL)) {
        map[x2][y2].agent = map[x1][y1].agent;
        map[x1][y1].agent = NULL;
    }
}

void Level::spawn_monster(int monster_type) {
    bool placed = false;
    while(!placed) {
        int x = rand() % map_width;
        int y = rand() % map_height;
        if(!is_walkable(x, y))
            continue;
        if(contains_agent(x, y))
            continue;
        Agent *new_monster = new Monster(x, y, 0, this, game, monster_type);
        add_agent(new_monster);
        placed = true;
    }
}

void Level::spawn_corpse(int x, int y, int monster_type) {
    map[x][y].symbol = '%';
}

void Level::print() {
	for (int j = 0; j < map_height; j++) {
		for (int i = 0; i < map_width; i++) {
            if(map[i][j].revealed) {
                if(map[i][j].visible) {
                    chtype c = COLOR_PAIR(0);
                    if(map[i][j].dangerous)
                        c = COLOR_PAIR(1);
                    if(map[i][j].agent == NULL)
                        if(map[i][j].stuff == NULL)
                            mvwaddch(level_win, j, i,
                                map[i][j].symbol | c);
                        else
                            mvwaddch(level_win, j, i,
                                map[i][j].stuff->get_symbol() | c);
                    else
                        mvwaddch(level_win, j, i,
                            map[i][j].agent->get_symbol() | c);
                }
                else {
                    mvwaddch(level_win, j, i,
                        map[i][j].symbol | COLOR_PAIR(2));
                }
            }
		}
	}
}

bool Level::contains_agent(int x, int y) {
    return (map[x][y].agent != NULL);
}

bool Level::contains_item(int x, int y) {
	return (map[x][y].stuff != NULL);
}

Item* Level::get_item(int x, int y) {
	return map[x][y].stuff;
}

void Level::remove_item(int x, int y) {
	map[x][y].stuff = NULL;
}

Agent *Level::agent_at(int x, int y) {
    return map[x][y].agent;
}

bool Level::is_visible(int x1, int y1, int x2, int y2) {
    return !(obstructed(x1, y1, x2, y2) &&
                obstructed(x1+1, y1, x2, y2) &&
                obstructed(x1, y1+1, x2, y2) &&
                obstructed(x1+1, y1+1, x2, y2));
}

bool Level::obstructed(int x1, int y1, int x2, int y2) {
    int sgn_x = sgn(x2-x1);
    int sgn_y = sgn(y2-y1);
    
    if(sgn_x == 1 && sgn_y == 1 && is_sight_blocking(x1, y1)) return true;
    if(sgn_x == -1 && sgn_y == 1 && is_sight_blocking(x1-1, y1)) return true;
    if(sgn_x == -1 && sgn_y == -1 && is_sight_blocking(x1-1, y1-1)) return true;
    if(sgn_x == 1 && sgn_y == -1 && is_sight_blocking(x1, y1-1)) return true;
    
    int delta_x = abs(x2 - x1);
    int delta_y = abs(y2 - y1);
    int x, y, qx, qy, rx, ry;
    
    if(delta_x != 0) {
        for(int dx = 1; dx < delta_x; dx++) {
            qy = dx * delta_y / delta_x;
            ry = dx * delta_y % delta_x;
            x = x1 + dx * sgn_x;
            y = y1 + qy * sgn_y;
            if(ry == 0) {
                if(corner_obstructed(x, y, sgn_x, sgn_y))
                    return true;
            } else {
                if(sgn_y == -1)
                    y--;
                if(is_sight_blocking(x, y) || is_sight_blocking(x-1, y))
                    return true;
            }
        }
    } else {
        if(sgn_y*delta_y == 1)
            if(is_sight_blocking(x1, y1) || is_sight_blocking(x1-1, y1))
                return true;
        if(sgn_y*delta_y == -1)
            if(is_sight_blocking(x1, y1-1) || is_sight_blocking(x1-1, y1-1))
                return true;
    }
    
    if(delta_y != 0) {
        for(int dy = 1; dy < delta_y; dy++) {
            qx = dy * delta_x / delta_y;
            rx = dy * delta_x % delta_y;
            y = y1 + dy * sgn_y;
            x = x1 + qx * sgn_x;
            if(rx == 0) {
                if(corner_obstructed(x, y, sgn_x, sgn_y))
                    return true;
            } else {
                if(sgn_x == -1)
                    x--;
                if(is_sight_blocking(x, y) || is_sight_blocking(x, y-1))
                    return true;
            }
        }
    } else {
        if(sgn_x*delta_x == 1)
            if(is_sight_blocking(x1, y1) || is_sight_blocking(x1, y1-1))
                return true;
        if(sgn_x*delta_x == -1)
            if(is_sight_blocking(x1-1, y1) || is_sight_blocking(x1-1, y1-1))
                return true;
    }
    
    return false;
}

bool Level::corner_obstructed(int x, int y, int sgn_x, int sgn_y) {
    if(is_sight_blocking(x, y) && is_sight_blocking(x, y-1)) return true;
    if(is_sight_blocking(x, y) && is_sight_blocking(x-1, y)) return true;
    if(is_sight_blocking(x-1, y-1) && is_sight_blocking(x, y-1)) return true;
    if(is_sight_blocking(x-1, y-1) && is_sight_blocking(x-1, y)) return true;
    
    if(sgn_x*sgn_y == 1) {
        if(is_sight_blocking(x, y) || is_sight_blocking(x-1, y-1))
            return true;
    }
    else if(sgn_x*sgn_y == -1) {
        if(is_sight_blocking(x, y-1) || is_sight_blocking(x-1, y))
            return true;
    }
    else {
        if(is_sight_blocking(x, y) || is_sight_blocking(x, y-1) ||
                is_sight_blocking(x-1, y-1) || is_sight_blocking(x-1, y))
            return true;
    }
    
    return false;    
}

bool Level::is_walkable(int x, int y) {
    return !(is_wall(x, y) || is_closed_door(x, y));
}

bool Level::is_sight_blocking(int x, int y) {
    return (is_wall(x, y) || is_closed_door(x, y));
}

bool Level::is_wall(int x, int y) {
    return (map[x][y].symbol == '#');
}

bool Level::is_closed_door(int x, int y) {
    return (map[x][y].symbol == '+');
}

bool Level::is_open_door(int x, int y) {
    return (map[x][y].symbol == '/');
}

bool Level::is_floor(int x, int y) {
    return (map[x][y].symbol == '.');
}

bool Level::is_upstair(int x, int y) {
    return (map[x][y].symbol == '<');
}

bool Level::is_downstair(int x, int y) {
    return (map[x][y].symbol == '>');
}

int Level::get_upstair_x() {
    return upstair.x;
}

int Level::get_upstair_y() {
    return upstair.y;
}

int Level::get_downstair_x() {
    return downstair.x;
}

int Level::get_downstair_y() {
    return downstair.y;
}
