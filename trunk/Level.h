#ifndef level_h
#define level_h

#include <curses.h>

#define map_width 80
#define map_height 30

class Agent;
class Game;
class Item;

struct Position {
    int x, y;
};

struct Tile{
	bool revealed; // The player has seen this tile.
	bool visible;  // The player can see this tile right now.
	bool dangerous;// A monster the player can see can see this tile now.
	int symbol;    // The symbol to display for this tile.
	Agent *agent;  // The agent at this tile, or NULL.
	Item *stuff;   // The item that is dropped in the tile, or NULL.
};

struct Room{
	int left, right, top, bottom;
	bool connected;
};

struct Agent_List {
    Agent *agent;
    Agent_List *next;
};

class Level{
public:
	Level(WINDOW *win, Game *parent);
	~Level();
    
    void mark_visible(int x, int y);
    void clear_visibility();
    void mark_dangerous(int x, int y);
    void clear_dangerousness();
    
	void open_door(int x, int y);
	bool close_door(int x, int y);
	void monsters_take_turns();
	void monsters_mutual_fov();
	
	void print_path(int x1, int y1, int x2, int y2);
	Position shoot_projectile(int x1, int y1, int x2, int y2, int accuracy, int range);
	Agent_List *get_targets(Agent *agent);
	
	void add_agent(Agent *agent);
	void remove_agent(Agent *agent);
	void move_agent(int x1, int y1, int x2, int y2);
	void spawn_monster(int monster_type);
	void spawn_corpse(int x, int y, int monster_type);
	void remove_item(int x, int y);
    
	void print();
	void print_location(int x, int y);
	bool contains_agent(int x, int y);
	bool contains_item(int x, int y);
	Item* get_item(int x, int y);
	Agent *agent_at(int x, int y);
	bool is_visible(int x1, int y1, int x2, int y2);
	bool is_walkable(int x, int y);
	bool is_sight_blocking(int x, int y);
	bool is_wall(int x, int y);
	bool is_closed_door(int x, int y);
	bool is_open_door(int x, int y);
	bool is_floor(int x, int y);
	bool is_upstair(int x, int y);
	bool is_downstair(int x, int y);
	int get_upstair_x();
	int get_upstair_y();
	int get_downstair_x();
	int get_downstair_y();

private:
	void generate();
	void add_items();
	void populate();
	Room rect_room();
	bool room_intersect(Room a, Room b);
	bool point_in_room(int x, int y, Room a);
	bool obstructed(int x1, int y1, int x2, int y2);
	bool corner_obstructed(int x, int y, int sgn_x, int sgn_y);

	Tile map[map_width][map_height];
	Game *game;
	Agent_List *agents;
	Room* rooms;
	int numrooms;
	Position upstair;
	Position downstair;
	
	WINDOW *level_win;
};

#endif
