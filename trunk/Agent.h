/* An Agent is a creature in the world that can move around.
 * Agents have inventories.
 * Some agents are controlled by the player.
 */

#ifndef Agent_h
#define Agent_h

#include "Level.h"

#define PI 3.14159265

class Game;

class Agent {
    protected:
        Position position; // Position in the level.
        float facing;      // Facing direction: 0=North.
        int speed;         // Not sure what this means yet.
        int vision;        // Distance this agent can see.
                           // If this value changes, fov must be reallocated.
        Level *location;   // The level this agent currently inhabits.
		Game *game;        // The game from which this agent was spawned
        int n_visible_corners;
                           // The number of corners the agent can see.
        Position *visible_corners;
                           // Array of points the agent can see.
                           // Must be reallocated if vision range changes.
        
    public:
        Agent(int x, int y, float f, Level *loc, Game *parent);
        void walk(int x, int y);
        void walk_turn(int x, int y);
        void turn(float angle);
		void face(float angle);
        void set_location(Level *loc);
        
        int get_x_pos();
        int get_y_pos();
        float get_facing();
        
        virtual int take_turn();
        virtual void mutual_fov();
};

#endif
