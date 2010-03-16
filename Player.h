/* A Player is an Agent controlled by the player
 */

#ifndef Player_h
#define Player_h

#include "Agent.h"
#include "Level.h"
#include "Inventory.h"

//walk modes
#define TURNING 0
#define STRAFING 1

class Player : public Agent {
    protected:
        int walk_mode;
        struct {
            int walk_west, walk_east, walk_north, walk_south,
                    walk_ne, walk_nw, walk_se, walk_sw,
                    turn_left, turn_right, use, change_walk_mode,
					inventory, close, fire, look, wait;
        } keys;
        void init_keys();
        void default_keys();
		void die();
		void wait();
		void close_door();
		
		int get_melee_damage();
		int get_ranged_damage();
		bool key2pos_abs(int c, Position *result);
		bool key2pos_rel(int c, Position *result);
        
    public:
        Player(Level *loc, Game *parent);
        ~Player();
		void use();
        int take_turn();
        void fire();
        void turn(float angle);
        void walk(int x, int y);
        void toggle_walk_mode();
		void look_mode();
		Inventory* get_inventory() { return inventory; }
		int get_armor();
        void mutual_fov();
        const char *get_name();
};

#endif
