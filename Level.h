#ifndef level_h
#define level_h

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
	Level();
	~Level();

	void print();
	bool is_wall(int x, int y);

private:
	void generate();
	Room rect_room();
	bool room_intersect(Room a, Room b);
	bool point_in_room(int x, int y, Room a);

	Tile map[win_width][win_height];
	Room* rooms;
	int numrooms;
};

#endif
