#include <stdlib.h>
#include "Agent.h"
#include "Monster.h"
#include "Level.h"

Monster::Monster(int x, int y, float f, Level *loc, Game *parent, int monster_type) : Agent(x, y, f, loc, parent) {
    is_player = false;
    type = monster_type;
    switch(monster_type) {
        case DOG:
            symbol = 'd';
            hp = max_hp = 15;
            attack_strength = 5;
            ranged_accuracy = 0; //no ranged attack
            break;
        default:
            symbol = 'g'; //default is a goblin; there are no goblins in this game.
            hp = max_hp = 15;
            attack_strength = 10;
            ranged_accuracy = 2; //throws rocks
            break;
    }
}

int Monster::take_turn() {
    walk_turn(rand()%3-1, rand()%3-1);
    return 0;
}

void Monster::mutual_fov() {
    Agent::mutual_fov();
    //determine what it can see, and what it means...
}
