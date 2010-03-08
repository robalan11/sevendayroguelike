/* A Player is an Agent controlled by the player
 */

#ifndef Player_h
#define Player_h

#include "Agent.h"
#include "Level.h"

class Player : public Agent {
    protected:
        
    public:
        Player(Level *loc);
        Player(int x, int y, float f, Level *loc);
        int take_turn();
        int calculate_visibility();
};

#endif
