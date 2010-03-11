/* The Inventory is the place where the Player's items
   are stored.  It also provides functionality for
   using and equipping these items. */

#ifndef Inventory_h
#define Inventory_h

#include <curses.h>
#include "Consumable.h"
#include "Equipment.h"

class Player;

class Inventory {
	protected:
		WINDOW *window;
		Item* drinks[8];
		Item* ammos[16];
		Item* rangeds[16];
		Item* melees[16];
		Item* suits[16];
		Item* hats[16];
		Item* *list;
		int num_drinks, num_ammos, num_rangeds, num_melees, num_suits, num_hats;
		int *num_things;
		int selected;
		char* categories[6];
		int category;
		Equipment *current_weapon;
		Equipment *current_suit;
		Equipment *current_hat;
		Player *parent;
		void display();
		void select_category(int type);

	public:
		Inventory(int difficulty, WINDOW *inventory_window, Player *player);

		void add_item(Item *new_item);
		void open();

		Equipment* get_current_weapon() { return current_weapon; }
		Equipment* get_current_suit() { return current_suit; }
		Equipment* get_current_hat() { return current_hat; }

		Player* get_player() { return parent; }
};

#endif
