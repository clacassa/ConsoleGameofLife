/************************************************************************

*	cgol (Console Game of Life) -- run the game of life in the terminal
*	Copyright (C) 2022 Cyprien Lacassagne

*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.

*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.

*	You should have received a copy of the GNU General Public License
*	along with this program.  If not, see <https://www.gnu.org/licenses/>.

*************************************************************************/

#include <iostream>
#include "config.h"

// Display version information
void print_version(std::string prog_name) {

	std::cout << "\x1b[36m" << prog_name << " (Console Game of Life) ";
	std::cout << VERSION << "\n\x1b[0m";
	std::cout << "Copyright (C) 2022 " << AUTHOR << "\n";
	std::cout << "This is free software: you are free to change and redistribute it.\n";
	std::cout << "There is no WARRANTY, to the extent permitted by law.\n\n";

	// std::cout << "\n";
	// std::cout << "    _____  "  "\x1b[36m" << prog_name << " (Console Game of Life) "
	// 		  << VERSION << "\n\x1b[0m";
	// std::cout << " __|__   | Copyright (C) 2022 " << AUTHOR << "\n";
	// std::cout << "|__|  |  | This is free software: you are free to change and redistribute it.\n";
	// std::cout << "      |__| There is NO WARRANTY, to the extent permitted by law.\n\n";
}

// Display help
void print_help(std::string prog_name) {
	std::cout << "Usage: " << prog_name << " [options] [file]\n";
	std::cout << "\n";
	std::cout << "Options:\n";
	std::cout << "-V, --version     display version information and exit\n";
	std::cout << "-h, --help        display this help and exit\n\n";
	std::cout << "Pass a filename as argument to initialize the simulation.\n";
	std::cout << "The command can also be run with no argument.\n\n";
}
