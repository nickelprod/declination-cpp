#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <random>
#include <unistd.h>

#include "compartment.hpp"
#include "player.hpp"

using namespace std;

//NOTE: Many todos are for future plans. We (Nick (the writer) and I) plan to continue the project, so leftover todos aren't necessarily a result of poor timing for the deadline


// Global variables (compartments, player, etc)
Player player(0, {}, 0);

Compartment main_shut("maintenance shuttle", "An alert screen is flashing \"READ\" on a large monitor. The maintenance shuttle is in disarray - It seems everything is broken except the glowing pad you're standing on.", {"wrench", "code-book"}, {});

Compartment boiler("boiler room", "Scan data tells you  there are two civilians, a mother and a child, trapped in the corner of the compartment behind an erupting steam pipe. There also seems to be an exposed valve.", {}, {"mother", "child"});

Compartment kitchen("kitchen", "Two wires are exposed from a side panel, one yellow and one blue. The door to the next compartment is rapidly slamming open and shut.", {"wire-cutters"}, {});

Compartment oxy("oxygen recycling room", "Water poors from the CO2 reclaimer. A little boy is standing, crying, on top of a counter across the room. The little boy cries for you to save his dad, who is trapped by debris under the water.", {}, {"boy", "dad."});

Compartment engine("engine room", "Two large pilons crackling with electricity stand before you. The electricity arcing between them is trapping a dog in the main bay. Power overload caused cascade failure in primary engine nodes. A flashing red button labeled \"REBOOT\" glares at you.", {}, {"dog"});

Compartment cockpit("cockpit", "The Vice Admiral lies on the floor, unconscious. All the status lights have gone blank, but the console seems to be intact.", {}, {"va"});

vector<Compartment> compartments = {main_shut, boiler, kitchen, oxy, engine, cockpit};

bool steam_pipe_closed = false;  // Whether or not the steam pipe in the boiler room has been closed
bool kitchen_door_open = false;  // Whether or not the door in the kitchen is stuck open
bool oxy_dad_free = false;  // Whether or not the dad in the oxygen recycling room has been unpinned from debris
bool engine_reset = false;  // Whether or not the engine has been turned of and back on again

int time_left = 30;  // Time remaining until collision with earth
	// Issue with using actual time: need to measure program time rather than real time, so player can save and come back later

vector<string> all_pass = {"mother", "dad"};  // For determining terrorist at random. It's called all_pass, but it's really all the passengers that can be the terrorist (it would be weird for a child or dog to be the terrorist)
int num_pass = all_pass.size();
string terrorist;
bool hostage = false;  // Whether or not the terrorist is holding other passengers hostage in the maintenance shuttle

bool debugging = false;  // Whether or not to print debug messages
bool demo_mode = false;
// End globals


void make_terrorist()
{
	random_device rand;
	default_random_engine gen(rand());
	uniform_int_distribution<int> dist(0, num_pass-1);  // -1 becuase the range is inclusive, but index maxes out at size-1
	int terrorist_index = dist(gen);
	terrorist = all_pass[terrorist_index];
	if (debugging) cout << "The terrorist is " << terrorist << endl << endl;
}


void help()
{
	cout << "(h)elp: display this message" << endl;
	cout << "(f)orward: move to the next compartment" << endl;
	cout << "(b)ack: move to the previous compartment" << endl;
	cout << "scan: receive data on surroundings" << endl;
	cout << "take <item-1> <item-2>: take one or more items form the current room. Separate items with a space" << endl;
	cout << "take all: take all usefull items from the current room" << endl;
	cout << "drop <item-1> <item-2>: move one or more items from your inventory to the room. Separate items with a space" << endl;
	cout << "drop all: drop all items in your inventory" << endl;
	cout << "(inv)entory: check the items in your inventory" << endl;
	cout << "save <passenger-1> <passenger-2>: send one or more passengers from the current room to the maintenance shuttle to wait for you. Separate passengers with a space" << endl;
	cout << "save all: send all passengers in the current room to the maintenance shuttle" << endl;
	cout << "(q)uit: exit the game" << endl;
	cout << "Some areas may have commands specific to that area" << endl << endl;
}

void read(vector<string> input)  //will be using for items that can be read from more than one place (eg books in inventory). Anything read within a compartment (eg the alert screen in the maintenance shuttle) will be handled in the cart's function
{
	if (input.size() < 2)
	{
		cout << "Please specify something to read" << endl;
		return;
	}

	string item = input[1];

	if (not player.has(item))
	{
		cout << "You can only read items in your inventory" << endl;
	}
	else if (item == "code-book")
	{
		cout << "The weathered cover titles the book: \"AotU: Acronyms of the URSC\"" << endl;
		cout << "You Open the cover and begin to read the lightly grease stained pages:" << endl;
		cout << " URSC: United republic Stellar Command - The space-specialized military for teh United Republic." << endl;
		cout << " CDRS: Carbon to Digital Retro-Sync - A technology used for transporting your consciousness elsewhere. Used to control an OTIS. Often called \"CD\" for short." << endl;
		cout << " OTIS: Organic tissue over Integrated Scaffolding - A synthetic body used to perform actions remotely. Equiped with sensors for each of the human senses. As a backup, an OTIS can be controlled using basic keyboard commands." << endl;
		cout << " VA: Vice Admiral - A rank high ranking official in the URSC." << endl;
		cout << " SOP: Standard Operating Procedure " << endl;
	}
	else
	{
		cout << "You can't read that" << endl;
	}
}


void take(Compartment &comp, vector<string> input)
{
	if (input.size() < 2)  // If no item was specified, say so
	{
		cout << "Please specify an item to take" << endl;
	}
	else if (input[1] != "all")  // If input isn't "take all"
	{

		for (unsigned int i = 1; i < input.size(); i++)
		{
			string item = input[i];
			if (comp.has(item))
			{
				comp.remove_item(item);
				player.add_item(item);
				cout << "Took " << item << endl;
			}
			else
			{
				cout << "There is no " << item << " here" << endl;
			}
		}
	}
	else  // Input is "take all"
	{
		vector<string> new_items = comp.get_inv();
		if (new_items.empty())
		{
			cout << "There is nothing to take" << endl;
		}
		else
		{
			comp.remove_all_items();
			player.add_n_items(new_items);
			cout << "Took " << new_items << endl;
		}
	}
}

void drop(Compartment &comp, vector<string> input)
{
	if (input.size() < 2)  // If no item was specified, say so
	{
		cout << "Please specify an item to drop" << endl;
	}
	else if (input[1] != "all")  // If input isn't "drop all"
	{
		for (unsigned int i = 1; i < input.size(); i++)
		{
			string item = input[i];
			if (player.has(item))
			{
				comp.add_item(item);
				player.remove_item(item);
				cout << "Dropped " << item << endl;
			}
			else
			{
				cout << "You don't have " << item << endl;
			}
		}
	}
	else  // Input is "drop all"
	{
		vector <string> dropped_items = player.get_inv();
		if (dropped_items.empty())
		{
			cout << "You have nothing to drop" << endl;
		}
		else
		{
			player.remove_all_items();
			comp.add_n_items(dropped_items);
			cout << "Took " << dropped_items << endl;
		}
	}
}

void save(Compartment &comp, vector<string> input)
{
	if (comp.get_name() == "maintenance shuttle")
	{
		cout << "Sending someone from the maintenance shuttle to the maintenance shuttle would be pointless" << endl;
	}
	else if (input.size() < 2)  // If no passenger was specified
	{
		cout << "Who would you like to save?" << endl;
	}
	else if (input[1] != "all")
	{
		for (unsigned int i = 1; i < input.size(); i++)
		{
			string pass = input[i];
			if (comp.has_pass(pass))
			{
				comp.remove_pass(pass);
				main_shut.add_pass(pass);
				player.inc_score(1);
				cout << "Sent " << pass << " to the maintenance shuttle" << endl;
			}
			else
			{
				cout << "No such passenger in this compartment" << endl;
			}
		}
	}
	else  // Input is "save all"
	{
		vector<string> saved_pass = comp.get_pass();
		if (saved_pass.empty())
		{
			cout << "There are no passengers in the room" << endl;
		}
		else
		{
			comp.remove_all_pass();
			main_shut.add_n_pass(saved_pass);
			player.inc_score(saved_pass.size());
			cout << "Sent " << saved_pass << " to the maintenance shuttle" << endl;
		}
	}

	if (main_shut.has_pass(terrorist))
	{
		hostage = true;
		if (debugging) cout << "Hostage situation started" << endl;
	}
}

vector<string> get_input() //Credit: jrok on stackoverflow for string to array. Gets player input, returns an array where each element is a word. This allows easily distinguishing between the command and the args (e.g. take wrench), as well as ignoring extra whitespace.
{
	vector<string> input;
	string s;
	cout << "> "; //TODO curr_location> like bash prompt? If so, remove current location line from scan and make enter() call scan. If allowing player to choose name, use name@location>
	getline(cin, s);
	transform(s.begin(), s.end(), s.begin(), ::tolower);

	istringstream iss(s);
	string word;

	while (iss >> word)
	{
		input.push_back(word);
	}

	if (input.empty())
	{
		input = {"INVALID"};
	}

	if (debugging) cout << "Input: " << input << endl;
	time_left --;
	if (time_left < 0)
	{
		cout << endl << "You try to perform one final action, but the Connie crashes into the earth's surface, killing everyone and destroying everything, including the VA's intel. You have just enough time to hear pained screams before your link is broken and you awake in your own body at control HQ." << endl;
		input = {"quit"};
	}
	return input;
}


vector<string> go_main_shut(bool scanned, bool entered)
{
	if ((not scanned) and (not entered))  // Only print scan data if player did not type "scan" in the intro (to prevent printing it twice)
	{
		main_shut.scan();
	}

	else if ((not scanned) and (entered))  // Only print enter text if player is actually entering the compartment (ie not at beginning, when player starts in this room)
	{
		main_shut.enter();
	}

	while (true)
	{
		vector<string> input = get_input();
		string command = input[0];

		if ((command == "q") or (command == "quit") or (command == "f") or (command == "forward") or (command == "b") or (command == "back"))  // If the command requires leaving the compartment, return to main
		{
			return input;
		}

		if (command == "scan")
		{
			main_shut.scan();
		}

		else if (command == "read")
		{
			if (input.size() < 2)
			{
				cout << "You read the large monitor:" << endl;
				cout << "- Mission: SOP for unknown threat protocal -" << endl;
				cout << "- U.S.R.C Connecticut unresponsive and rapidly losing orbit -" << endl;
				cout << "- Decay to termination estimate: " << time_left << " minutes -" << endl;
				cout << "- Investigate and remedy. If remedy infeasable, make way to bow for extraction -" << endl;
			}
			else  // If an item was specified, call the read function
			{
				read(input);
			}
		}

		else if (command == "take")
		{
			take(main_shut, input);
		}

		else if (command == "drop")
		{
			drop(main_shut, input);
		}

		else if ((command == "h") or (command == "help"))
		{
			help();
		}

		else if ((command == "inv") or (command == "inventory"))
		{
			player.check_inv();
		}

		else if (command == "save")
		{
			cout << "Sending someone from the maintenance shuttle to the maintenance shuttle would be pointless" << endl;
		}

		else
		{
			cout << "Please enter a valid command" << endl;
		}
	}
}


vector<string> go_boiler()
{
	boiler.enter();

	while (true)
	{
		vector<string> input = get_input();
		string command = input[0];

		if ((command == "q") or (command == "quit") or (command == "f") or (command == "forward") or (command == "b") or (command == "back"))  // If the command requires leaving the compartment, return to main
		{
			return input;
		}

		if (command == "scan")
		{
			boiler.scan();
		}

		else if (command == "read")
		{
			read(input);
		}

		else if (command == "take")
		{
			take(boiler, input);
		}

		else if (command == "drop")
		{
			drop(boiler, input);
		}

		else if ((command == "h") or (command == "help"))
		{
			help();
		}

		else if ((command == "inv") or (command == "inventory"))
		{
			player.check_inv();
		}

		else if ((command == "close") and (not steam_pipe_closed))
		{
			if (input.size() < 2)  // If the player didn't specify what to close
			{
				cout << "specify something to close" << endl;
			}
			else
			{
				string to_close = input [1];
				if ((to_close == "valve") or (to_close == "pipe") or (to_close == "steam"))  // ie "valve" "pipe" or "steam pipe"
				{
					if (player.has("wrench"))
					{
						cout << "You wrap the crescent wrench around the valve, tighten the wrench, and, with great effort, close the valve. The steam ceases to erupt, and the mother gives you a look as if to say \"Please be here to help us and not hurt us\"" << endl;
						steam_pipe_closed = true;
						boiler.set_desc("The steam pipe is no longer erupting, and thus the mother and child no longer trapped.");
					}
					else
					{
						cout << "You try to close the valve with your bare hands, but are not strong enough" << endl;
					}
				}
				else
				{
					cout << "You can't close that" << endl;
				}
			}
		}
		else if ((command == "close") and (steam_pipe_closed))
		{
			cout << "There is nothing left to close" << endl;
		}

		else if (command == "save")
		{
			if (steam_pipe_closed)
			{
				save(boiler, input);
			}
			else
			{
				cout << "The steam is hot, so valve must be closed first. You wouldn't want to burn a mother and her child, would you?" << endl;
			}
		}


		else
		{
			cout << "Please enter a valid command" << endl;
		}
	}

}


vector<string> go_kitchen()
{
	kitchen.enter();

	while (true)
	{
		vector<string> input = get_input();
		string command = input[0];

		if ((command == "q") or (command == "quit") or  (command == "b") or (command == "back"))  // If the command requires leaving the compartment, return to main
		{
			return input;
		}

		if ((command == "f") or (command == "forward"))
		{
			if (kitchen_door_open)
			{
				return input;
			}
			else
			{
				cout << "The door is moving too quickly" << endl;
			}
		}

		else if (command == "scan")
		{
			kitchen.scan();
		}

		else if (command == "read")
		{
			read(input);
		}

		else if (command == "take")
		{
			take(kitchen, input);
		}

		else if (command == "drop")
		{
			drop(kitchen, input);
		}

		else if ((command == "h") or (command == "help"))
		{
			help();
		}

		else if ((command == "inv") or (command == "inventory"))
		{
			player.check_inv();
		}

		else if (command == "save")
		{
			cout << "There is no one here to save" << endl;
		}

		else if (command == "cut")
		{
			if (not player.has("wire-cutters"))
			{
				cout << "You can't just cut with your hands" << endl;
			}
			else if (input.size() < 2)
			{
				cout << "Please specify something to cut" << endl;
			}
			else if ((input[1] == "wire") or (input[1] == "wires") or (input[1] == "blue") or (input[1] == "yellow"))
			{
				cout << "With expert timing, you make the cut and the door sticks open" << endl;
				kitchen_door_open = true;
				kitchen.set_desc("The wires fot the door mechanism have been cut, and the door is stuck open");
			}
			else
			{
				cout << "You can't cut that" << endl;
			}
		}

		else
		{
			cout << "Please enter a valid command" << endl;
		}
	}
}


vector<string> go_oxy()
{
	oxy.enter();

	while (true)
	{
		vector<string> input = get_input();
		string command = input[0];

		if ((command == "q") or (command == "quit") or (command == "f") or (command == "forward") or (command == "b") or (command == "back"))  // If the command requires leaving the compartment, return to main
		{
			return input;
		}

		else if (command == "scan")
		{
			oxy.scan();
		}

		else if (command == "read")
		{
			read(input);
		}

		else if (command == "take")
		{
			take(oxy, input);
		}

		else if (command == "drop")
		{
			drop(oxy, input);
		}

		else if ((command == "h") or (command == "help"))
		{
			help();
		}

		else if ((command == "inv") or (command == "inventory"))
		{
			player.check_inv();
		}

		else if (command == "save")
		{
			if (not oxy_dad_free)
			{
				cout << "The boy won't leave without his dad, but the dad is still strapped" << endl;
			}
			else
			{
				save(oxy, input);
			}
		}

		else if (command == "lift")
		{
			if (input.size() < 2)
			{
				cout << "Please specify something to lift" << endl;
			}
			else if (input[1] == "debris")
			{
				cout << "You manage to lift the debris off the boy's father, and you pull him out of the water. After some CPR, the dad is conscious." << endl;
				oxy_dad_free = true;
				oxy.set_desc("The dad is alive, and the boy is no longer crying. The CO2 reclaimer is still pouring water, but you can't do anything about that.");
			}
			else
			{
				cout << "You can't lift that" << endl;
			}
		}

		else
		{
			cout << "Please enter a valid command" << endl;
		}
	}
}


vector<string> go_engine()
{
	engine.enter();

	while (true)
	{
		vector<string> input = get_input();
		string command = input[0];

		if ((command == "q") or (command == "quit") or (command == "f") or (command == "forward") or (command == "b") or (command == "back"))  // If the command requires leaving the compartment, return to main
		{
			return input;
		}

		else if (command == "scan")
		{
			engine.scan();
		}

		else if (command == "read")
		{
			read(input);
		}

		else if (command == "take")
		{
			take(engine, input);
		}

		else if (command == "drop")
		{
			drop(engine, input);
		}

		else if ((command == "h") or (command == "help"))
		{
			help();
		}

		else if ((command == "inv") or (command == "inventory"))
		{
			player.check_inv();
		}

		else if (command == "save")
		{
			if (not engine_reset)
			{
				cout << "If the dog tried to come to you, it would be electricuted to death" << endl;
			}
			else
			{
				save(engine, input);
			}
		}

		else if ((command == "push") or (command == "press"))
		{
			if (input.size() < 2)
			{
				cout << "Please specify something to " << command << endl;
			}
			else if (input[1] == "button")
			{
				cout << "The pylons shut off briefly before coming back on, electricity no longer arcing between them" << endl;
				engine_reset = true;
				oxy.set_desc("The engine room: now functioning normally (though at low power), and no longer trapping any dogs.");
			}
			else
			{
				cout << "You can't " << command << " that" << endl;
			}
		}

		else
		{
			cout << "Please enter a valid command" << endl;
		}
	}
}


vector<string> go_cockpit()
{
	cockpit.enter();

	if (hostage)  // If hostage, the terrorist will kill the passengers if you save the VA, so you can only save VA or passengers
	{
		cout << "After you look around the room, you hear the sound of an incoming intercom transmission and a devious voice" << endl;
		//TODO if allowing player to choose name, replace "hero" with player's name
		cout << "\"Howdy there, 'hero,' if that even is your real name. I have a probem, would you like to hear about it? Good. You see, that VA in there, Ol' Mister Mecker, he has some very inportant intel that I don't want to get to the URSC higher-ups. You, however, have been sent here for the sole purpose of retrieving that info and giving it to the URSC, so it seems we have reached an impasse. What is my solution to my problem you ask? Well, simple. You give me an impasse, I give you an ultimatum. If you save the VA, either by bringing him back to the maintenance shuttle, or by detaching the cockpit, I will kill all of the passengers in this shuttle; however, if you divert remaining power to the maintenance shuttle, leave Mecker where he is, and come fly us out of here, all the passengers you worked so hard to save can live. You get to save people, and I don't get my intel leaked. It's a win-win really. So, what'll it be?\"" << endl;
		cout << "The transmission ends, and it seems you are left with a choice. Are you duty bound to save the VA and his intel, intel that undoubtedly cost lives to get, or are you honor bound to save the civilians caught up in this mess?" << endl;
		while (true)
		{
			vector<string> input = get_input();
			string command = input[0];

			if ((command == "q") or (command == "quit") or (command == "f") or (command == "forward") or (command == "b") or (command == "back"))  // If the command requires leaving the compartment, return to main
			{
				cout << "There is no way out, you must make a decision" << endl;
			}

			if (command == "scan")
			{
				cockpit.scan();
			}

			else if (command == "read")
			{
				cout << "Reading won't help you decide" << endl;
			}

			else if (command == "take")
			{
				cout << "There is nothing of use" <<endl;
			}

			else if (command == "drop")
			{
				cout << "There is no reason to drop anything here" << endl;
			}

			else if ((command == "h") or (command == "help"))
			{
				cout << "There is no help. You are either `duty bound` or `honor bound`" << endl;
			}

			else if ((command == "inv") or (command == "inventory"))
			{
				cout << "You have nothing that can help you make the choice" << endl;
			}

			else if (command == "save")
			{
				cout << "The terrorist will kill everyone if you try to bring the VA to the maintenance room." << endl;
			}

			else if (command == "duty")
			{
				cout << "Figuring that the intel will save more lives than this choice will cost, you decide to save the VA. Realizing it's too dangerous to take him back to the maintenance shuttle, you try the fire up the main engines. At first they sputter weakly, but within seconds they sound like real engines. The lights on the console flicker to life, and status gauges tell you that power is low and fuel is almost non existant. Nevertheless, you attempt to pull the Connie put of her dive, but to no avail; the engines are too weak. As the last remaining option, you detatch the cockpit from the rest of the ship. The engines seem powerful enough to at least keep the small cockpit from crashing into Earth's surface. As you decend slowly, you see the rest of the ship fall below you. You cannot know what the passengers are thinking or the fear they are feeling, but you pray for their deaths to be quick and painless. You doubt the terrorist will be that kind." << endl;

				player.inc_score(10);
				return {"DONE"};  // Becuse player input is converted to lowercase, the player cannot cheat by typing "DONE"
			}

			else if (command == "honor")
			{
				cout << "Deciding that one life and some intel is not worth the cost of many lives, you turn on the console, lights flickering to life, and divert power and fuel to the maintenance shuttle. You leave Vice Admiral Mecker where he is and walk back the the shuttle. Upon opening the door, you see the terrorist is " << terrorist << ", who is pointing a gun in your face. After proving you came alone, you are permitted to enter the shuttle. You detach the shuttle from the main ship and begin the slow, uncomfortable decent toward Earth. You wonder if  Mecker will regain consciousness before his collision, but you pray that he won't." << endl;

				player.inc_score(10);
				return {"DONE"};
			}

			else
			{
				cout << "Please enter a valid command (hint, there are two)" << endl;
			}
		}
	}

	else
	{
		while (true)
		{
			vector<string> input = get_input();
			string command = input[0];

			if ((command == "q") or (command == "quit") or (command == "f") or (command == "forward") or (command == "b") or (command == "back"))  // If the command requires leaving the compartment, return to main
			{
				return input;
			}

			if (command == "scan")
			{
				cockpit.scan();
			}

			else if (command == "read")
			{
				read(input);
			}

			else if (command == "take")
			{
				take(cockpit, input);
			}

			else if (command == "drop")
			{
				drop(cockpit, input);
			}

			else if ((command == "h") or (command == "help"))
			{
				help();
			}

			else if ((command == "inv") or (command == "inventory"))
			{
				player.check_inv();
			}

			else if (command == "save")
			{
				save(cockpit, input);
				if (not cockpit.has("va"))  // If the VA was saved
				{
					player.inc_score(25);
					cout << "Knowing that the maintenance shuttle won't have enough fuel for a descent all the way to Earth, you divert power and fuel to the shuttle. The VA is too heavy to carry, but you manage to drag him back the the maintenance shuttle, bumping his head along the way for that time at the holiday party. You shut the door behind you and detach the shuttle from the ship. As you do so, you realize that you have yet to be shot in the back, and are relieved that the terrorist isn't in the shuttle. You descend toward Earth, wondering what would have happend if the terrorist had been sent to the shuttle." << endl;
					return {"DONE"};
				}
			}

			else
			{
				cout << "Please enter a valid command" << endl;
			}
		}
	}
}


vector<string> go_next_cart()
{
	vector<string> input;

	unsigned int loc = player.get_loc();
	if (loc == compartments.size()-1)  // If the player cannot go forward, tell them and get new input
	{
		cout << "You are already at the front of the ship" << endl;
		input = get_input();
		return input;
	}
	loc += 1;
	player.set_loc(loc);

	string next_room = compartments[loc].get_name();

	if (next_room == "maintenance shuttle")
	{
		input = go_main_shut(false, true);
	}
	else if (next_room == "boiler room")
	{
		input = go_boiler();
	}
	else if (next_room == "kitchen")
	{
		input = go_kitchen();
	}
	else if (next_room == "oxygen recycling room")
	{
		input = go_oxy();
	}
	else if (next_room == "engine room")
	{
		input = go_engine();
	}
	else if (next_room == "cockpit")
	{
		input = go_cockpit();
	}

	return input;
}

vector<string> go_prev_cart()
{
	vector<string> input;

	unsigned int loc = player.get_loc();
	if (loc == 0)  // If the player cannot go back, tell them and get new input
	{
		cout << "You are already at the back of the ship" << endl;
		input = get_input();
		return input;
	}
	loc -= 1;
	player.set_loc(loc);

	string next_room = compartments[loc].get_name();

	if (next_room == "maintenance shuttle")
	{
		input = go_main_shut(false, true);
	}
	else if (next_room == "boiler room")
	{
		input = go_boiler();
	}
	else if (next_room == "kitchen")
	{
		input = go_kitchen();
	}
	else if (next_room == "oxygen recycling room")
	{
		input = go_oxy();
	}
	else if (next_room == "engine room")
	{
		input = go_engine();
	}
	else if (next_room == "cockpit")
	{
		input = go_cockpit();
	}

	return input;
	vector<string> temp = {"h"};
	return temp;
}


int main()
{
	if (debugging) cout << "Welcome to declination, please enter a test command: ";
	if (debugging) vector<string> input = get_input();
	if (demo_mode)
	{
		compartments = {main_shut, boiler, cockpit};
		all_pass = {"mother"};
		num_pass = 1;
	}

	make_terrorist();

	cout << endl << "|DECLINATION|" << endl << endl;

	sleep(3);

	cout << "Ground Comms: \"#### pulling your orbit. Your line doesn't look good ##.\"" << endl;
	sleep(2);
	cout << "Orbital Comms: ..." << endl;
	sleep(2);
	cout << "G.C.: \"Comms are registering interf-#### come back.\"" << endl;
	sleep(2);
	cout << "O.C.: ..." << endl;
	sleep(2);
	cout << "G.C.: \"### unknown threat protocal ## active link esta#\"" << endl << endl;

	sleep(3);

	cout << "Attempting Carbon to Digital Retro-Sync" << endl;
	sleep(1);
	cout << "Successfully CD'd into Organic Tissue Over Integrated Scaffolding" << endl;
	sleep(1);
	cout << "Visual and motor funtcion inoperable, default to keyboard commands" << endl;
	sleep(1);
	cout << "O.T.I.S. uplink stable" << endl;
	sleep(1);
	cout << "Basic Commands:" << endl << " (f)orward: next compartment" << endl << " (b)ack: previous compartment" << endl << " (h)elp: display more commands" << endl << endl;

	sleep(1);

	cout << "Type \"SCAN\" to receive data on surroundings" << endl;
	vector<string> input = get_input();
	string command = input[0];  // The first word of the input is the command
	bool scanned;
	if (command == "scan")
	{
		main_shut.scan();
		scanned = true;
	}
	else
	{
		cout << "That's okay, you can try again later" << endl << endl;
		scanned = false;
	}

	sleep(1);
	cout << "Initialization successful. SCAN will automatically be performed upon entering a compartment" << endl << endl;

	input = go_main_shut(scanned, false);
	command = input[0];
	while (true)
	{
		if ((command == "quit") or (command == "q"))  // If the command is quit, terminate
		{
			return 0;
		}

		if ((command == "f") or (command == "forward"))
		{
			input = go_next_cart();
		}

		else if ((command == "b") or (command == "back"))
		{
			input = go_prev_cart();
		}

		else if ((command == "h") or (command == "help"))
		{
			help();
			input = get_input();
		}

		else
		{
			cout << "Please enter a valid command" << endl;
			input = get_input();
		}

		command = input[0];

		if (command == "DONE")
		{
			cout << endl << "Conratulations! You beat the game. Your score was " << player.get_score() << ". Thanks for playing." << endl;
			return 0;
		}
	}

	return 0;
}
