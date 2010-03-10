/* A Player is an Agent controlled by the player
 */

#ifndef Player_h
#define Player_h

#include "Agent.h"
#include "Level.h"

class Player : public Agent {
    protected:
        
    public:
        Player(Level *loc, Game *parent);
        Player(int x, int y, float f, Level *loc, Game *parent);
		void use();
        int take_turn();
        void mutual_fov();
};

#endif
