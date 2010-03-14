#include <stdlib.h>
#include <string.h>
#include "Consumable.h"
#include "Inventory.h"
#include "Player.h"

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
	strcpy(name, _name);
	strcpy(description, _desc);
}

void Drink::use(Inventory *parent) {
	if (quantity > 0) {
		parent->get_player()->gain_hp(rand()%(max_heal-min_heal) + min_heal);
		quantity--;
	}
}

Ammo::Ammo(char *_name, char *_desc) {
	init(_name, _desc);
}

Ammo::Ammo(char *desc) {
	if (!strcmp(desc, "Derringer"))
		init("Derringer Ammo", ".40 caliber pinfire bullets");
	else if(!strcmp(desc, "Luger"))
		init("Luger Ammo", "7.65x21mm Parabellum rounds");
	else if(!strcmp(desc, "Colt 45"))
		init("Colt 45 Ammo", ".45 caliber revolver rounds");
	else if(!strcmp(desc, "Walther PPK"))
		init("Walther PPK Ammo", ".32 ACP cartridges");
}

void Ammo::init(char *_name, char *_desc) {
	symbol = '|';
	type = AMMO;
	quantity = 10;
	strcpy(name, _name);
	strcpy(description, _desc);
}

void Ammo::use(Inventory *parent) {
    if(quantity > 0)
        quantity--;
}
