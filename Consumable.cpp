#include <stdlib.h>
#include <string.h>
#include "Consumable.h"

Drink::Drink(int min, int max, char *_name, char *_desc) {
	init(min, max, _name, _desc);
}

Drink::Drink(char *desc) {
	if (!strcmp(desc, "Beer"))
		init(3, 5, "Beer", "Simple barley-based beverage");
	else if (!strcmp(desc, "Wine"))
		init(5, 10, "Wine", "Beverage made from fermented grapes");
	else if (!strcmp(desc, "Shot"))
		init(12, 20, "Shot", "Hard liquor, straight up");
	else if (!strcmp(desc, "Martini"))
		init(25, 40, "Martini", "The drink of choice for secret agents");
}

void Drink::init(int min, int max, char *_name, char *_desc) {
	symbol = '!';
	type = DRINK;
	quantity = 1;
	min_heal = min;
	max_heal = max;
	strcpy_s(name, _name);
	strcpy_s(description, _desc);
}

int Drink::drink() {
	return rand()%(max_heal-min_heal) + min_heal;
}