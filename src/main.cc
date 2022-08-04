// cgol (Console Game of Life) -- run the game of life in the terminal
// Copyright (C) 2022 Cyprien Lacassagne

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <thread>
#include "simulation.h"
#include "config.h"

void go_to_menu(std::string filename, unsigned refresh);
void clear();
void shell();

static bool __stab_end;

int main(int argc, char* argv[]) {

	srand((unsigned) time(0));
	std::string filename("");

	// Define the programm name
	#ifdef _WIN32
		std::string EXEC_PATH = argv[0];
		if (EXEC_PATH.find_last_of("\\") != std::string::npos) {
			unsigned pos(EXEC_PATH.find_last_of("\\"));
			EXEC_PATH = EXEC_PATH.substr(pos + 1);
			pos = EXEC_PATH.find_first_of(".");
			EXEC_PATH = EXEC_PATH.replace(pos, EXEC_PATH.length(), "");
		}
		const std::string PROGRAM_NAME = EXEC_PATH;
	#else
		const std::string PROGRAM_NAME = argv[0];
	#endif

	// Option parsing
	if (argc == 2) {
		if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-V") == 0) {
			print_version(PROGRAM_NAME);
			exit(EXIT_SUCCESS);
		}
		if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
			print_help(PROGRAM_NAME);
			exit(EXIT_SUCCESS);
		}
		if (std::string(argv[1]).find_first_of("-") == 0) {
			std::cout << PROGRAM_NAME << " : the option \"" << argv[1] << "\" is not recognized.\n";
			std::cout << "use \"" << PROGRAM_NAME << " -h\" for a complete list of options\n";
			exit(EXIT_FAILURE);
		}
		filename = argv[1];
	}
	if (argc >= 3) {
		std::cout << "cgol: \x1b[91merror: \x1b[0m1 argument expected, " << argc - 1
				  << " provided.\n";
		exit(EXIT_FAILURE);
	}

	// Initialize variables and Simulation instance
	Simulation sim(init_refresh);
	if (filename != "") {
		sim.read_file(filename);
	}
	__stab_end = sim.get_stab_end();
	unsigned refresh(init_refresh);
	go_to_menu(filename, refresh);
	std::string input;

	// Interaction loop (I/O)
	while (true)
	{
		std::cin >> input;

		if (input == "q") {
			return 0;
		}

		if (input == "t") {
			sim.toggle_stab_end();
			__stab_end = sim.get_stab_end();
			go_to_menu(filename, refresh);
		}
		else if (input == "s"){
			do {
				std::cout << "simulation speed [10, 200]: ";
				// Exit the loop if the input is not an integer
				 if (!(std::cin >> refresh)) {
					std::cin.clear();
					std::cin.ignore(10000, '\n');
					std::cout << "\x1b[38;5;3mInvalid input.\n\x1b[0m";
					refresh = sim.get_refrsh_rate();
					std::this_thread::sleep_for(std::chrono::milliseconds(1500));
					break;
				}
			}while (refresh > 200 || refresh < 10);
			sim.set_refresh(refresh);
			go_to_menu(filename, refresh);
		}

		// Initialization option parsing
		else if (input == "r") {
			sim.start_sim(RANDOM_INIT);
			go_to_menu(filename, refresh);
		}
		else if (input == "g") {
			sim.start_sim();
			go_to_menu(filename, refresh);
		}
		else if (input == "f") {
			if (filename != "") {
				sim.start_sim(FILE_INIT);
				go_to_menu(filename, refresh);
			}else {
				std::cin.clear();
				std::cin.ignore(10000, '\n');
				shell();
			}
		}
		else {
			std::cin.clear();
			std::cin.ignore(10000, '\n');
			shell();
		}
	}
	return 0;
}

// Display context menu
void go_to_menu(std::string filename, unsigned refresh) {
	clear();
	std::cout << "\x1b[36m" "\33[52m" \
	             "----------- Console Game of Life -----------\n" \
 	             "\x1b[0m" "\33[m" \
				 "The map has a fixed size of " << world_size << " by " << world_size << " cells.\n" \
				 "By default, the speed is set to " << init_refresh << " ms.\n" \
				 "The simulation automatically stops when it\n" \
				 "reaches a state of stability";
	if (__stab_end) {
		std::cout << " (\x1b[36mOn\x1b[0m)\n";
	}else {
		std::cout << " (\x1b[36mOff\x1b[0m)\n";
	}
	std::cout << "You can toggle this option by pressing 't'.\n\n" \
				 "Press 's' to change the speed (\x1b[36m" << refresh
			  << "\x1b[0m ms/frame)\n" \
				 "Press 'r' to start the simulation from random.\n";
	if (filename != "") {
		std::cout << "\x1b[35mPress 'f' to start from your text file.\n\x1b[0m";
	}
	std::cout << "Press 'g' to start with the glider gun\n" \
				 "Press 'q' to quit.\n";
	shell();
	return;
}

// Clear the screen and the scrollback buffer
void clear() {
	system(CLEAR_SCREEN);
}
// void clear() {
// 	std::cout << "\x1b[3J\x1b[H";
// }

// Display the '>' character to make it look like a shell entry
void shell() {
	std::cout << "\x1b[36m> \x1b[0m";
}