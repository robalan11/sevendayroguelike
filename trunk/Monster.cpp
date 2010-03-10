#include "Agent.h"
#include "Monster.h"
#include "Level.h"

Monster::Monster(int x, int y, float f, Level *loc, Game *parent) : Agent(x, y, f, loc, parent) {
    
}

int Monster::take_turn() {
    return 0;
}
