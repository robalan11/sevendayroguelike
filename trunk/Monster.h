/* A Monster is an Agent controlled by AI.
 */

#ifndef Monster_h
#define Monster_h

#include "Agent.h"
#include "Level.h"

//monster types
#define DOG 0

class Monster : public Agent {
    protected:
        int type;
        
    public:
        Monster(int x, int y, float f, Level *loc, Game *parent, int monster_type);
        int take_turn();
        void mutual_fov();
};

#endif
