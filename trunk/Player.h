/* A Player is an Agent controlled by the player
 */

#ifndef Player_h
#define Player_h

#include "Agent.h"
#include "Level.h"

#define TURNING 0
#define STRAFING 1

class Player : public Agent {
    protected:
        int walk_mode;
        
    public:
        Player(Level *loc, Game *parent);
		void use();
        int take_turn();
        int walk(int x, int y);
        void toggle_walk_mode();
        void mutual_fov();
};

#endif
