/* An Item is a thing that can be dropped by monsters
   and picked up by the player */

#ifndef Item_h
#define Item_h

class Item {
	protected:
		char symbol;
		char name[64];
		char description[256];
		int quantity;

	public:
		void add_more(int num) { quantity += num; }

		char get_symbol() { return symbol; }
		char* get_name() { return name; }
		char* get_description() { return description; }
		int get_quantity() { return quantity; }
};

#endif
