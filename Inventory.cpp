#include <string.h>
#include <stdlib.h>
#include "Inventory.h"

Inventory::Inventory(int difficulty, WINDOW *inventory_window) {
	window = inventory_window;
	num_items = 0;
	selected = 0;
	if (difficulty == 0) {
		Drink *beer = new Drink("Beer");
		Drink *wine = new Drink("Wine");
		Drink *shot = new Drink("Shot");
		Drink *martini = new Drink("Martini");
		add_item(*beer);
		add_item(*beer);
		add_item(*beer);
		add_item(*wine);
		add_item(*martini);
		add_item(*shot);
		add_item(*wine);
	}
}

void Inventory::add_item(Item new_item) {
	for (int i=0; i < num_items; i++) {
		if (!strcmp(items[i].get_name(), new_item.get_name())) {
			items[i].add_more(new_item.get_quantity());
			return;
		}
	}
	
	items[num_items] = new_item;
	num_items++;
}

void Inventory::open() {
	int input = 0;
	while (input != 's') {
		display();
		input = getch();

		if (input == ',') selected = (selected+1)%num_items;
		else if (input == 'i') {
			selected--;
			if (selected < 0) selected = num_items-1;
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
	
	char num[2];
	for (int i = 0; i < num_items; i++) {
		itoa(i+1, num, 10);
		if (i == selected) {
			if (i < 9) {
				mvwaddch(window, 4 + 2*i, 5, ' ' | COLOR_PAIR(3));
				mvwaddch(window, 4 + 2*i, 6, num[0] | COLOR_PAIR(3));
			} else {
				mvwaddch(window, 4 + 2*i, 5, num[0] | COLOR_PAIR(3));
				mvwaddch(window, 4 + 2*i, 6, num[1] | COLOR_PAIR(3));
			}
		} else {
			if (i < 9) {
				mvwaddch(window, 4 + 2*i, 5, ' ' | COLOR_PAIR(0));
				mvwaddch(window, 4 + 2*i, 6, num[0] | COLOR_PAIR(0));
			} else {
				mvwaddch(window, 4 + 2*i, 5, num[0] | COLOR_PAIR(0));
				mvwaddch(window, 4 + 2*i, 6, num[1] | COLOR_PAIR(0));
			}
		}
		mvwprintw(window, 4 + 2*i, 8, "%i %s - %s",
			items[i].get_quantity(), items[i].get_name(), items[i].get_description());
	}

	wrefresh(window);
}
