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
	strcpy(name, _name);
	strcpy(description, _desc);
}

void Weapon::use(Inventory *parent) {
	if (type == RANGED) parent->set_current_ranged_weapon(this);
	else if (type == MELEE) parent->set_current_melee_weapon(this);
}

int Weapon::damage() {
	return rand()%(max_damage - min_damage) + min_damage;
}


Armor::Armor(int def, int _type, char *_name, char *_desc) {
	init(def, _type, _name, _desc);
}
Armor::Armor(char *desc) {
	if (!strcmp(desc, "Jumpsuit"))
		init(0, SUIT, "Jumpsuit", "What you wore to jump out of the airplane.");
	else if (!strcmp(desc, "Business Suit"))
		init(2, SUIT, "Business Suit", "Mediocre defense, but looks good.");
	else if (!strcmp(desc, "Kevlar Suit"))
		init(6, SUIT, "Kevlar Suit", "Offers good defense from attacks.");
	else if (!strcmp(desc, "Tuxedo"))
		init(10, SUIT, "Tuxedo", "Very stylish, also excellent at stopping bullets.");

	else if (!strcmp(desc, "None"))
		init(0, HAT, "None", "No hat. Very sad.");
	else if (!strcmp(desc, "Ushanka"))
		init(1, HAT, "Ushanka", "While warm, this does not offer much defense.");
	else if (!strcmp(desc, "Bowler"))
		init(3, HAT, "Bowler", "Some use this as a weapon, but it's better on your head.");
	else if (!strcmp(desc, "Fedora"))
		init(5, HAT, "Fedora", "Essential to the wardrobe of any secret agent.");
}

void Armor::init(int def, int _type, char *_name, char *_desc) {
	if (type == RANGED) symbol = '{';
	if (type == MELEE) symbol = '\\';
	type = _type;
	quantity = 1;
	defense = def;
	strcpy_s(name, _name);
	strcpy_s(description, _desc);
}

void Armor::use(Inventory *parent) {
	if (type == SUIT) parent->set_current_suit(this);
	else if (type == HAT) parent->set_current_hat(this);
}
