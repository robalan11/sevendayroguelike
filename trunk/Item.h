/* An Item is a thing that can be dropped by monsters
   and picked up by the player */

#ifndef Item_h
#define Item_h

#define DRINK 0
#define AMMO 1
#define MELEE 2
#define RANGED 3
#define SUIT 4
#define HAT 5

class Inventory;

class Item {
	protected:
		char symbol;
		int type;
		char name[64];
		char description[256];
		int quantity;

	public:
		void add_more(int num) { quantity += num; }

		char get_symbol() { return symbol; }
		int get_type() { return type; }
		char* get_name() { return name; }
		char* get_description() { return description; }
		int get_quantity() { return quantity; }

		virtual void use(Inventory *parent) { }
};

#endif
