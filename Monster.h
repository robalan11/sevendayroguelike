/* A Monster is an Agent controlled by AI.
 */

#ifndef Monster_h
#define Monster_h

#include "Agent.h"
#include "Level.h"

class Monster : public Agent {
    protected:
        
    public:
        Monster(int x, int y, float f, Level *loc, Game *parent);
        int take_turn();
};

#endif
