#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Level.h"

#define min(x, y) (x>y)?y:x

Level::Level(WINDOW *win) {
	generate();
	level_win = win;
}

Level::~Level() {}

void Level::generate() {
	// Declare stuff and fill the map with spaces
	rooms = new Room[win_width];
	numrooms = 0;
	int roomfilled = 0;
	srand((unsigned int)time(NULL));
	for (int j = 0; j < win_height; j++) {
		for (int i = 0; i < win_width; i++) {
			map[i][j].symbol = ' ';
			map[i][j].revealed = FALSE;
			map[i][j].visible = FALSE;
		}
	}

	// Add rooms until the map is 50% full of room tiles
	while(roomfilled < 0.5 * win_width * win_height) {
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
	for (int j = 0; j < win_height; j++) {
		for (int i = 0; i < win_width; i++) {
			if (map[i][j].symbol == ' ') map[i][j].symbol = '#';
		}
	}

	// Add doors
	for (int j = 0; j < win_height; j++) {
		for (int i = 0; i < win_width; i++) {
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
int x = rand() % (int)floor(win_width * 0.9) + 1;
		int width = rand() % (int)floor(win_width * 0.25) + int(win_width * 0.15);
		int y = rand() % (int)floor(win_height * 0.9) + 1;
		int height = rand() % (int)floor(win_height * 0.3) + int(win_height * 0.15);

		Room temp;
		temp.connected = false;
		temp.left = x;
		temp.right = min(x + width, win_width-2);
		temp.top = y;
		temp.bottom = min(y + height, win_height-2);
		
		return temp;
}

bool Level::room_intersect(Room a, Room b) {
	return (!(a.bottom < b.top || a.right < b.left || a.top > b.bottom || a.left > b.right));
}

bool Level::point_in_room(int x, int y, Room a) {
	return (x <= a.right && x >= a.left && y <= a.bottom && y >= a.top);
}

void Level::mark_visible(int x, int y) {
    map[x][y].visible = TRUE;
    map[x][y].revealed = TRUE;
}

void Level::clear_visibility() {
    for(int j = 0; j < win_height; j++)
        for(int i = 0; i < win_width; i++)
            map[i][j].visible = FALSE;
}

void Level::open_door(int x, int y) {
	if (map[x][y].symbol == '+')
		map[x][y].symbol = '/';
}

void Level::print() {
	for (int j = 0; j < win_height; j++) {
		for (int i = 0; i < win_width; i++) {
            if(map[i][j].revealed) {
                if(map[i][j].visible)
                    mvwaddch(level_win, j, i, map[i][j].symbol | COLOR_PAIR(0));
                else
                    mvwaddch(level_win, j, i, map[i][j].symbol | COLOR_PAIR(1));
            }
		}
	}
}

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

bool Level::is_walkable(int x, int y) {
    return !(is_wall(x, y) || is_closed_door(x, y));
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
