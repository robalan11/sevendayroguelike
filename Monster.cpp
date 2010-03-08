#include "Agent.h"
#include "Monster.h"
#include "Level.h"

Monster::Monster(int x, int y, float f, Level *loc) : Agent(x, y, f, loc) {
    
}

int Monster::take_turn() {
    return 0;
}
