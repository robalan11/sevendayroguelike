#include <string.h>
#include <stdlib.h>
#include "Inventory.h"
#include "Player.h"

Inventory::Inventory(WINDOW *inventory_window, Player *player) {
	window = inventory_window;
	parent = player;
	num_drinks = 0;
	num_ammos = 0;
	num_rangeds = 0;
	num_melees = 0;
	num_suits = 0;
	num_hats = 0;
	selected = 0;
	category = 0;
	categories[0] = "Drinks";
	categories[1] = "Ammo";
	categories[2] = "Melee";
	categories[3] = "Ranged";
	categories[4] = "Suits";
	categories[5] = "Hats";

	Drink *beer = new Drink("Beer");
	Drink *wine = new Drink("Wine");
	Drink *shot = new Drink("Shot");
	add_item(beer);
	add_item(beer);
	add_item(beer);
	add_item(wine);
	add_item(shot);

	Weapon *fists = new Weapon("Fists");
	Weapon *pocketknife = new Weapon("Pocketknife");
	add_item(fists);
	add_item(pocketknife);

	Weapon *derringer = new Weapon("Derringer");
	add_item(derringer);

	current_melee_weapon = melees[0];
	current_ranged_weapon = rangeds[0];
}

void Inventory::add_item(Item *new_item) {
	select_category(new_item->get_type());

	for (int i=0; i < *num_things; i++) {
		if (!strcmp(list[i]->get_name(), new_item->get_name())) {
			list[i]->add_more(new_item->get_quantity());
			return;
		}
	}	
	list[*num_things] = new_item;
	(*num_things)++;
}

void Inventory::open() {
	int input = 0;
	while (input != 's') {
		display();
		input = getch();

		if (input == ',') selected = (selected+1)%num_drinks;
		else if (input == 'i') {
			selected--;
			if (selected < 0) selected = num_drinks-1;
		}
		else if (input == 'l') {
			category = (category+1)%6;
			selected = 0;
		}
		else if (input == 'j') {
			category--;
			if (category < 0) category = 5;
			selected = 0;
		}
		else if (input == 'k') {
			select_category(category);
			list[selected]->use(this);
		}
	}
	wclear(window);
	wrefresh(window);
}

void Inventory::display() {
	wclear(window);
	// Draw a border
	for (int i = 0; i < 82; i++) {
		mvwaddch(window, 0, i, '*' | COLOR_PAIR(2));
		mvwaddch(window, 1, i, '*' | COLOR_PAIR(2));
		mvwaddch(window, 33, i, '*' | COLOR_PAIR(2));
		mvwaddch(window, 34, i, '*' | COLOR_PAIR(2));
	}
	for (int i = 2; i < 33; i++) {
		mvwaddch(window, i, 0, '*' | COLOR_PAIR(2));
		mvwaddch(window, i, 1, '*' | COLOR_PAIR(2));
		mvwaddch(window, i, 80, '*' | COLOR_PAIR(2));
		mvwaddch(window, i, 81, '*' | COLOR_PAIR(2));
	}

	// Draw the categories
	for (int i = 0; i < 6; i++) {
		if (i == category)
			wattron(window, A_REVERSE);
		mvwprintw(window, 3, 8 + 8*i, "%s", categories[i]);
		wattroff(window, A_REVERSE);
	}

	// Draw the active category
	select_category(category);
	char num[2];
	for (int i = 0; i < *num_things; i++) {
		_itoa(i+1, num, 10);
		if (i == selected) 
			wattron(window, A_REVERSE);
		mvwprintw(window, 5 + i, 5, "%2i", i+1);

		wattroff(window, A_REVERSE);
		mvwprintw(window, 5 + i, 8, "%i %s - %s",
			list[i]->get_quantity(), list[i]->get_name(), list[i]->get_description());
	}

	wrefresh(window);
}

void Inventory::select_category(int type) {
	switch (type) {
		case DRINK:
			num_things = &num_drinks;
			list = drinks;
			break;
		case AMMO:
			num_things = &num_ammos;
			list = ammos;
			break;
		case MELEE:
			num_things = &num_melees;
			list = melees;
			break;
		case RANGED:
			num_things = &num_rangeds;
			list = rangeds;
			break;
		case SUIT:
			num_things = &num_suits;
			list = suits;
			break;
		case HAT:
			num_things = &num_hats;
			list = hats;
			break;
		default:
			num_things = NULL;
			list = NULL;
			break;
	}
}
