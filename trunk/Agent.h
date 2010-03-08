/* An Agent is a creature in the world that can move around.
 * Agents have inventories.
 * Some agents are controlled by the player.
 */

#ifndef Agent_h
#define Agent_h

#include "Level.h"

class Agent {
    protected:
        struct {
            int x, y;
        } position;      // Position in the level.
        int facing;      // Facing direction: 0=North.
        int speed;       // Not sure what this means yet.
        int vision;      // Distance this agent can see.
        Level *location; // The level this agent currently inhabits.
        
    public:
        Agent(int x, int y, int f, Level *loc);
        void walk(int x, int y);
        void turn(int angle);
        void set_location(Level *loc);
        
        int get_x_pos();
        int get_y_pos();
        int get_facing();
        
        virtual int take_turn();
        virtual int calculate_visibility();
};

#endif
