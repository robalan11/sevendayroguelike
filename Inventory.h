/* The Inventory is the place where the Player's items
   are stored.  It also provides functionality for
   using and equipping these items. */

#ifndef Inventory_h
#define Inventory_h

#include <curses.h>
#include "Consumable.h"
#include "Equipment.h"

class Inventory {
	protected:
		WINDOW *window;
		Item items[64];
		int num_items;
		int selected;
		Equipment *current_weapon;
		Equipment *current_suit;
		Equipment *current_hat;
		void display();

	public:
		Inventory(int difficulty, WINDOW *inventory_window);

		void add_item(Item new_item);
		void open();

		Equipment* get_current_weapon() { return current_weapon; }
		Equipment* get_current_suit() { return current_suit; }
		Equipment* get_current_hat() { return current_hat; }
};

#endif
