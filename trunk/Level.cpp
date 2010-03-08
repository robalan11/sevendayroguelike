#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Level.h"

#define min(x, y) (x>y)?y:x

Level::Level() {
	generate();
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
			map[i][j] = ' ';
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
				if (map[j][k] == ' ') {
					roomfilled ++;
					//if (j==temp.left || k==temp.top || j==temp.right || k==temp.bottom)
					//	map[j][k] = '#';
					//else
					map[j][k] = '.';
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

			/*if (map[x][y] == '#' && ((map[x-1][y] == '.' && map[x+1][y] == '.') || (map[x][y-1] == '.' && map[x][y+1] == '.'))) {
				map[x][y] = '+';
				for (int i = 0; i < numrooms; i++)
					if (point_in_room(x, y, rooms[i])) rooms[i].connected = true;
			} else*/
			map[x][y] = '.';
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
			map[rand()%(rooms[0].right-rooms[0].left) + rooms[0].left][rand()%(rooms[0].bottom-rooms[0].top) + rooms[0].top] = '<';
			map[rand()%(target.right-target.left) + target.left][rand()%(target.bottom-target.top) + target.top] = '>';
		}
	}
	
	// Fill walls
	for (int j = 0; j < win_height; j++) {
		for (int i = 0; i < win_width; i++) {
			if (map[i][j] == ' ') map[i][j] = '#';
		}
	}

	// Add doors
	for (int j = 0; j < win_height; j++) {
		for (int i = 0; i < win_width; i++) {
			bool good = false;
			if (map[i][j] == '.' && map[i-1][j] == '.' && map[i+1][j] == '.' && map[i][j-1] == '#' && map[i][j+1] == '#') {
				for (int k = 0; k < numrooms; k++) {
					if (point_in_room(i-1, j, rooms[k]) || point_in_room(i+1, j, rooms[k])) {
						good = true;
						break;
					}
				}
			}
			else if (map[i][j] == '.' && map[i][j-1] == '.' && map[i][j+1] == '.' && map[i-1][j] == '#' && map[i+1][j] == '#') {
				for (int k = 0; k < numrooms; k++) {
					if (point_in_room(i, j-1, rooms[k]) || point_in_room(i, j+1, rooms[k])) {
						good = true;
						break;
					}
				}
			}
			if (good) map[i][j] = '+';
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

void Level::print() {
	for (int j = 0; j < win_height; j++) {
		for (int i = 0; i < win_width; i++) {
			mvaddch(j, i, map[i][j]);
		}
	}
}

bool Level::is_wall(int x, int y) {
    return (map[x][y] == '#');
}
