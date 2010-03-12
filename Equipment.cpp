#include <stdlib.h>
#include <string.h>
#include "Equipment.h"
#include "Inventory.h"

Weapon::Weapon(int min, int max, int _type, char *_name, char *_desc) {
	init(min, max, _type, _name, _desc);
}
Weapon::Weapon(char *desc) {
	if (!strcmp(desc, "Fists"))
		init(0, 0, MELEE, "Fists", "Your fists. They're not very effective.");
	else if (!strcmp(desc, "Pocketknife"))
		init(1, 3, MELEE, "Pocketknife", "Your pocketknife.  It's sharp enough to do some damage.");
	else if (!strcmp(desc, "Switchblade"))
		init(4, 8, MELEE, "Switchblade", "More effective than a pocketknife.");
	else if (!strcmp(desc, "Butterfly Knife"))
		init(6, 18, MELEE, "Butterfly Knife", "A stylish weapon, excellent for backstabbing.");

	else if (!strcmp(desc, "Derringer"))
		init(2, 2, RANGED, "Derringer", "Easily concealed, this will help in a pinch.");
	else if (!strcmp(desc, "Luger"))
		init(3, 5, RANGED, "Luger", "A solid pistol, this will serve you well.");
	else if (!strcmp(desc, "Colt 45"))
		init(6, 10, RANGED, "Colt 45", "A classic weapon, accurate and effective.");
	else if (!strcmp(desc, "Walther PPK"))
		init(10, 20, RANGED, "Walther PPK", "The weapon of choice for secret agents.");
}

void Weapon::init(int min, int max, int _type, char *_name, char *_desc) {
	symbol = '{';
	type = _type;
	quantity = 1;
	min_damage = min;
	max_damage = max;
	strcpy_s(name, _name);
	strcpy_s(description, _desc);
}

void Weapon::use(Inventory *parent) {
	if (type == RANGED) parent->set_current_ranged_weapon(this);
	else if (type == MELEE) parent->set_current_melee_weapon(this);
}

int Weapon::damage() {
	return rand()%(max_damage - min_damage) + min_damage;
}
