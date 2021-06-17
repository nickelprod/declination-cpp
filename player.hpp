#ifndef PLAYER_HPP_
#define PLAYER_HPP_
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "compartment.hpp"

using namespace std;

/*template<typename T>
ostream& operator<< (ostream& out, const vector<T>& v)
{
	size_t last = v.size() - 1;
	for (size_t i = 0; i < v.size(); i++)
	{
		out << v[i];
		if (i != last)
		{
			out << ", ";
		}
	}
	return out;
}*/


class Player
{
public:
	Player(int location, vector<string> items, int start_score)
	{
		loc = location;
		inventory = items;
		score = start_score;
	}

	void check_inv()
	{
		cout << "Your inventory: " << inventory << endl;
	}

	bool has(string item)
	{
		if (find(inventory.begin(), inventory.end(), item) != inventory.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	void set_loc(int new_loc)
	{
		loc = new_loc;
	}

	void set_score(int new_score)
	{
		score = new_score;
	}

	void inc_score(int inc_val)
	{
		score += inc_val;
	}

	void dec_score(int dec_val)
	{
		score -= dec_val;
	}

	void add_item(string new_item)
	{
		inventory.push_back(new_item);
	}

	void add_n_items(vector<string> new_items)
	{
		for(unsigned int i = 0; i < new_items.size(); i++)
		{
			inventory.push_back(new_items[i]);
		}
	}

	void remove_item(string old_item)
	{
		inventory.erase(remove(inventory.begin(), inventory.end(), old_item), inventory.end());
	}

	void remove_n_items(vector<string> old_items)
	{
		for(unsigned int i = 0; i < old_items.size(); i++)
		{
			inventory.erase(remove(inventory.begin(), inventory.end(), old_items[i]), inventory.end());
		}
	}

	void remove_all_items()
	{
		inventory.clear();
	}

	int get_loc()
	{
		return loc;
	}

	vector<string> get_inv()
	{
		return inventory;
	}

	int get_num_inv()
	{
		return inventory.size();
	}

	int get_score()
	{
		return score;
	}

private:
	unsigned int loc;  // compartment number (determined by order of compartment vector in main program)
	vector<string> inventory;  // items available to the player to use
	int score;  // the player's score. Based on things like puzzles solved, passengers saved, secrets found, etc
};


#endif /* PLAYER_HPP_ */
