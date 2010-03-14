#include "Game.h"

int main() {
    Game *game = new Game();
    
    game->show_title();
    game->play();
    
    delete game;
    
    return 0;
}
