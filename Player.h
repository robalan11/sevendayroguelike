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
        Player(int x, int y, int f, Level *loc);
        int take_turn();
        void calculate_visibility();
};

#endif
