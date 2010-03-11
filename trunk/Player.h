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
					inventory;
        } keys;
        void init_keys();
        void default_keys();
		Inventory *inventory;
        
    public:
        Player(Level *loc, Game *parent);
		void use();
        int take_turn();
        void walk(int x, int y);
        void toggle_walk_mode();
		void toggle_inventory();
        void mutual_fov();
};

#endif
