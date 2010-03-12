/* A Consumable is an Item that can be consumed */

#ifndef Consumable_h
#define Consumable_h

#include "Item.h"

class Drink : public Item {
	protected:
		int min_heal, max_heal;
		void init(int min, int max, char *_name, char *_desc);

	public:
		Drink(int min, int max, char *_name, char *_desc);
		Drink(char *desc);
		void use(Inventory *parent);
};

class Ammo : public Item {
	protected:

	public:
		
};

#endif
