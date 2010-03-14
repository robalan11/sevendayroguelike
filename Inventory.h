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
		Item *current_melee_weapon;
		Item *current_ranged_weapon;
		Item *current_suit;
		Item *current_hat;
		Player *parent;
		void display();
		void select_category(int type);
		Item* which_ammo(Item *weapon);

	public:
		Inventory(WINDOW *inventory_window, Player *player);

		void add_item(Item *new_item);
		void open();

		void set_current_melee_weapon(Weapon *weapon) { current_melee_weapon = weapon; }
		Item* get_current_melee_weapon() { return current_melee_weapon; }
		void set_current_ranged_weapon(Weapon *weapon) { current_ranged_weapon = weapon; }
		Item* get_current_ranged_weapon() { return current_ranged_weapon; }
		void set_current_suit(Armor *armor) { current_suit = armor; }
		Item* get_current_suit() { return current_suit; }
		void set_current_hat(Armor *armor) { current_hat = armor; }
		Item* get_current_hat() { return current_hat; }
		int get_ammo();
		void use_ammo();

		Player* get_player() { return parent; }
};

#endif
