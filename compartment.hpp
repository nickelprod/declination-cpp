#ifndef COMPARTMENT_HPP_
#define COMPARTMENT_HPP_
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

template<typename T>
ostream& operator<< (ostream& out, const vector<T>& v)  // Credit: Chris Redford on stackoverflow. Defines how cout handles vectors
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
}


class Compartment
{
public:
	Compartment(string title, string description, vector<string> items, vector<string> passengers)
	{
		name = title;
		desc = description;
		inventory = items;
		pass = passengers;
	}


	void scan()
	{
		cout << endl << "You are in the " << name << endl << endl;
		cout << desc << endl;

		if (inventory.size() != 0)
		{
			cout << "Useful items: " << inventory << endl;
		}

		if (pass.size() != 0)
		{
			cout << "Passengers: " << pass << endl;
		}
		cout << endl;
	}

	void enter()
	{
		cout << endl << "You entered the " << name << endl << endl;
		cout << desc << endl;

		if (inventory.size() != 0)
		{
			cout << "Useful items: " << inventory << endl;
		}

		if (pass.size() != 0)
		{
			cout << "Passengers: " << pass << endl;
		}
		cout << endl;
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

	bool has_pass(string passenger)
	{
		if (find(pass.begin(), pass.end(), passenger) != pass.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	void set_name(string new_name)
	{
		name = new_name;
	}

	void set_desc(string new_desc)
	{
		desc = new_desc;
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

	void add_pass(string new_pass)
	{
		pass.push_back(new_pass);
	}

	void add_n_pass(vector<string> new_pass)
	{
		for(unsigned int i = 0; i < new_pass.size(); i++)
		{
			pass.push_back(new_pass[i]);
		}
	}


	void remove_item(string old_item)
	{
		inventory.erase(remove(inventory.begin(), inventory.end(), old_item), inventory.end());
	}

	void remove_n_items(vector<string> old_items)
	{
		for (unsigned int i = 0; i < old_items.size(); i++)
		{
			inventory.erase(remove(inventory.begin(), inventory.end(), old_items[i]), inventory.end());
		}
	}

	void remove_all_items()
	{
		inventory.clear();
	}

	void remove_pass(string old_pass)
	{
		pass.erase(remove(pass.begin(), pass.end(), old_pass), pass.end());
	}

	void remove_n_pass(vector<string> old_pass)
	{
		for (unsigned int i = 0; i < old_pass.size(); i++)
		{
			pass.erase(remove(pass.begin(), pass.end(), old_pass[i]), pass.end());
		}
	}

	void remove_all_pass()
	{
		pass.clear();
	}


	string get_name()
	{
		return name;
	}

	string get_desc()
	{
		return desc;
	}

	vector<string> get_inv()
	{
		return inventory;
	}

	int get_num_inv()
	{
		return inventory.size();
	}

	vector<string> get_pass()
	{
		return pass;
	}

	int get_num_pass()
	{
		return pass.size();
	}

private:
	string name;  // the name of the compartment (independent from number)
	string desc;  // the description displayed upon entering
	vector<string> inventory;  // items in the compartment that the player may pick up
	vector<string> pass;  // the passengers in the compartment
};


#endif /* COMPARTMENT_HPP_*/
