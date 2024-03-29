/* An Agent is a creature in the world that can move around.
 * Agents have inventories.
 * Some agents are controlled by the player.
 */

#ifndef Agent_h
#define Agent_h

#include "Level.h"

#define PI 3.14159265

class Game;
class Inventory;

class Agent {
    protected:
        unsigned int symbol;
        
        int hp, max_hp;
        int attack_strength, ranged_accuracy;
        
        Position position; // Position in the level.
        float facing;      // Facing direction: 0=East.
        int speed;         // Not sure what this means yet.
        int vision;        // Distance this agent can see.
                           // If this value changes, fov must be reallocated.
        float fov_angle;   // How wide an angle the agent can see.
        
        Level *location;   // The level this agent currently inhabits.
		Game *game;        // The game from which this agent was spawned
		Inventory *inventory;
        int n_visible_corners;
                           // The number of corners the agent can see.
        Position *visible_corners;
                           // Array of points the agent can see.
                           // Must be reallocated if vision range changes.
        
        void wait() { /* do nothing. */ }
        bool can_move_forward();
        void move_forward();
        void set_position(int x, int y);
        virtual void die();
        
        virtual int get_melee_damage();
        virtual int get_ranged_damage();
        
    public:
        Agent(int x, int y, float f, Level *loc, Game *parent);
        virtual ~Agent();
        bool walk(int x, int y);
        bool walk_turn(int x, int y);
        void attack(Agent *enemy);
        void ranged_attack(int x, int y);
        void lose_hp(int hurt);
		void gain_hp(int heal);
        void turn(float angle);
		void face(float angle);
        void set_location(Level *loc) { location = loc; }
        
        int get_x_pos() { return position.x; }
        int get_y_pos() { return position.y; }
        float get_facing() { return facing; }
        int get_symbol() { return symbol; }
		Inventory* get_inventory() { return inventory; }
        
        int get_hp() { return hp; }
        int get_max_hp() { return max_hp; }
        
        virtual int take_turn();
        virtual void mutual_fov();
        virtual const char *get_name();
        
        bool can_see(int x, int y);
        bool is_player;
};

#endif
