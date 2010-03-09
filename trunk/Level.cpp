#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Level.h"

#define min(x, y) ((x>y)?y:x)
#define max(x, y) ((x>y)?x:y)
#define sgn(x) ((x==0)?0:((x>0)?1:-1))

Level::Level(WINDOW *win) {
	generate();
	level_win = win;
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

void Level::open_door(int x, int y) {
	if (map[x][y].symbol == '+')
		map[x][y].symbol = '/';
}

void Level::print() {
	for (int j = 0; j < map_height; j++) {
		for (int i = 0; i < map_width; i++) {
            if(map[i][j].revealed) {
                if(map[i][j].visible)
                    mvwaddch(level_win, j, i, map[i][j].symbol | COLOR_PAIR(0));
                else
                    mvwaddch(level_win, j, i, map[i][j].symbol | COLOR_PAIR(1));
            }
		}
	}
}

/*
bool Level::is_visible(int x1, int y1, int x2, int y2) {
	bool steep = abs(y2-y1) > abs(x2-x1);
	float error = 0;
	if (!steep) {
		float derror = (float)(y2-y1)/(float)(x2-x1);
		int y = y1;
		int dx = x1 < x2 ? 1 : -1;
		int dy = y1 < y2 ? 1 : -1;
		for (int x = x1; abs(x-x2) > 0; x += dx) {
			if (!is_walkable(x, y)) return false;
			error += derror;
			if (abs(error) > 0.5) {
				y += dy;
				error -= 1.0;
			}
		}
	} else {
		float derror = (float)(x2-x1)/(float)(y2-y1);
		int x = x1;
		int dx = x1 < x2 ? 1 : -1;
		int dy = y1 < y2 ? 1 : -1;
		for (int y = y1; abs(y-y2) > 0; y += dy) {
			if (!is_walkable(x, y)) return false;
			error += derror;
			if (abs(error) > 0.5) {
				x += dx;
				error -= 1.0;
			}
		}
	}
	return true;
}
*/

/*
//Simplistic center-to-nearest-corner line of sight
bool Level::is_visible(int x1, int y1, int x2, int y2) {
    int delta_x = x2 - x1;
    int delta_y = y2 - y1;
    int sgn_x = delta_x > 0 ? 1 : -1;
    int sgn_y = delta_y > 0 ? 1 : -1;
    
    if(delta_x == 0) {
        for(int y = y1 + sgn_y; y != y2; y += sgn_y) {
            if(is_sight_blocking(x1, y))
                return false;
        }
    }
    else if(delta_y == 0) {
        for(int x = x1 + sgn_x; x != x2; x += sgn_x) {
            if(is_sight_blocking(x, y1))
                return false;
        }
    }
    else {
        float x = x1;
        float y = y1;
        x += 0.5;
        y += 0.5;
        float slope = (float)(y2-y) / (float)(x2-x);
        float diff_x = sgn_x*floor(sgn_x*x + 1)-x;
        float diff_y = sgn_y*floor(sgn_y*y + 1)-y;
        while(sgn_x*x < sgn_x*(float)x2) {
            if(fabs(diff_y / diff_x) > fabs(slope)) {
                x += diff_x;
                y += slope * diff_x;
            } else {
                y += diff_y;
                x += diff_y / slope;
            }
            if(is_sight_blocking((int)floor(x), (int)floor(y)))
                return false;
            float diff_x = sgn_x*floor(sgn_x*x + 1)-x;
            float diff_y = sgn_y*floor(sgn_y*y + 1)-y;
        }
    }
    
    return true;
}
*/

bool Level::is_visible(int x1, int y1, int x2, int y2) {
    if(x1==x2 && y1==y2) return true;
    else return !(obstructed(x1, y1, x2, y2) &&
                obstructed(x1+1, y1, x2, y2) &&
                obstructed(x1, y1+1, x2, y2) &&
                obstructed(x1+1, y1+1, x2, y2));
}

bool Level::obstructed(int x1, int y1, int x2, int y2) {
    int sgn_x = sgn(x2-x1);
    int sgn_y = sgn(y2-y1);
    
    if(sgn_x == 1 && sgn_y == 1 && is_sight_blocking(x1+1, y1+1)) return true;
    if(sgn_x == -1 && sgn_y == 1 && is_sight_blocking(x1-1, y1+1)) return true;
    if(sgn_x == -1 && sgn_y == -1 && is_sight_blocking(x1-1, y1-1)) return true;
    if(sgn_x == 1 && sgn_y == -1 && is_sight_blocking(x1+1, y1-1)) return true;
    
    int delta_x = abs(x2 - x1);
    int delta_y = abs(y2 - y1);
    int x, y, qx, qy, rx, ry;
    
    if(delta_x != 0) {
        for(int dx = 1; dx < delta_x; dx++) {
            qy = dx * delta_y / delta_x;
            ry = dx * delta_y % delta_x;
            x = x1 + dx * sgn_x;
            y = y1 + qy * sgn_y;
            if(rx == 0) {
                if(corner_obstructed(x, y, sgn_x, sgn_y))
                    return true;
            } else {
                if(sgn_y == -1)
                    y--;
                if(is_sight_blocking(x, y) || is_sight_blocking(x-1, y))
                    return true;
            }
        }
    }
    
    if(delta_y != 0) {
        for(int dy = 1; dy < delta_y; dy++) {
            qx = dy * delta_x / delta_y;
            rx = dy * delta_x % delta_y;
            y = y1 + dy * sgn_y;
            x = x1 + qx * sgn_x;
            if(ry == 0) {
                if(corner_obstructed(x, y, sgn_x, sgn_y))
                    return true;
            } else {
                if(sgn_x == -1)
                    x--;
                if(is_sight_blocking(x, y) || is_sight_blocking(x, y-1))
                    return true;
            }
        }
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
