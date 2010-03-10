#ifndef level_h
#define level_h

#include <curses.h>

#define map_width 80
#define map_height 30

struct Position {
    int x, y;
};

struct Tile{
	bool revealed; // The player has seen this tile.
	bool visible;  // The player can see this tile right now.
	char symbol;   // The symbol to display for this tile.
};

struct Room{
	int left, right, top, bottom;
	bool connected;
};

class Level{
public:
	Level(WINDOW *win);
	~Level();

    void mark_visible(int x, int y);
    void clear_visibility();
	void open_door(int x, int y);

	void print();
	bool is_visible(int x1, int y1, int x2, int y2);
	bool is_walkable(int x, int y);
	bool is_sight_blocking(int x, int y);
	bool is_wall(int x, int y);
	bool is_closed_door(int x, int y);
	bool is_floor(int x, int y);
	bool is_upstair(int x, int y);
	bool is_downstair(int x, int y);
	int get_upstair_x();
	int get_upstair_y();
	int get_downstair_x();
	int get_downstair_y();

private:
	void generate();
	Room rect_room();
	bool room_intersect(Room a, Room b);
	bool point_in_room(int x, int y, Room a);
	bool obstructed(int x1, int y1, int x2, int y2);
	bool corner_obstructed(int x, int y, int sgn_x, int sgn_y);

	Tile map[map_width][map_height];
	Room* rooms;
	int numrooms;
	Position upstair;
	Position downstair;
	
	WINDOW *level_win;
};

#endif
