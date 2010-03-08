#ifndef level_h
#define level_h

#include <curses.h>

#define win_width 80
#define win_height 30

struct Tile{
	bool revealed;
	bool visible;
	char symbol;
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

	void print();
	bool is_walkable(int x, int y);
	bool is_wall(int x, int y);
	bool is_closed_door(int x, int y);
	bool is_floor(int x, int y);
	bool is_upstair(int x, int y);
	bool is_downstair(int x, int y);
	int get_upstair_x();
	int get_upstair_y();

private:
	void generate();
	Room rect_room();
	bool room_intersect(Room a, Room b);
	bool point_in_room(int x, int y, Room a);

	Tile map[win_width][win_height];
	Room* rooms;
	int numrooms;
	struct {int x; int y;} upstair;
	struct {int x; int y;} downstair;
	
	WINDOW *level_win;
};

#endif
