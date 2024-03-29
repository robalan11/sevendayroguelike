/* A Monster is an Agent controlled by AI.
 */

#ifndef Monster_h
#define Monster_h

#include "Agent.h"
#include "Level.h"

//monster types
#define DOG 0
#define MOOK 1
#define GUARD 2
#define AGENT 3

class Monster : public Agent {
    protected:
        int type;
        int ranged_damage;
        
        bool can_see_player, could_see_player;
        Position player_last_seen;
        int time_since_player_seen;
        int tenacity;  //number of turns the monster will search for the player after losing sight.
        
        void die();
        
        int get_melee_damage() { return attack_strength; }
        int get_ranged_damage() { return ranged_damage; }
        
    public:
        Monster(int x, int y, float f, Level *loc, Game *parent, int monster_type);
        ~Monster();
        int take_turn();
        void mutual_fov();
        void mark_danger();
        const char *get_name();
        const char *MONSTER_NAME(int x);
};

#endif
