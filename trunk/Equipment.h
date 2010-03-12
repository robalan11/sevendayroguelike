/* An Item is a thing that can be dropped by monsters
   and picked up by the player */

#ifndef Equipment_h
#define Equipment_h

#include "Item.h"

class Weapon : public Item {
	protected:
		int min_damage;
		int max_damage;

	public:
		Weapon(int min, int max, int _type, char *_name, char *_desc);
		Weapon(char *description);
		void init(int min, int max, int _type, char *_name, char *_desc);
		int damage();
		void use(Inventory *parent);
};

class Armor : public Item {

};

#endif
