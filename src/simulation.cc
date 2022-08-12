/************************************************************************

*   cgol (Console Game of Life) -- run the game of life in the terminal
*   Copyright (C) 2022 Cyprien Lacassagne

*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.

*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.

*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <https://www.gnu.org/licenses/>.

*************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <thread>
#include <chrono>
#include "simulation.h"
#include "config.h"

static int nb_alive(0);
static int past_alive(0);
static int past_2_alive(0);
static int past_3_alive(0);
static int past_4_alive(0);
static int past_5_alive(0);
static int nb_dead(0);
static bool stable(false);
static bool past_stable(false);

enum reading_State { NB_CELLS, COORDINATES, END, OK };

static int state(NB_CELLS);
static unsigned i(0), total(0), line_nb(0), x(0), y(0);

Simulation::Simulation(int rfrsh_rate)
: refresh_rate(rfrsh_rate) {
    std::vector<bool> line(world_size, false);
    for (unsigned i(0); i < world_size; ++i) {
        grid.push_back(line);
        updated_grid.push_back(line);
    }
    stab_end = true;
}

Simulation::~Simulation() {
    system(CLEAR_SCREEN);
}

void Simulation::read_file(std::string filename) {

    state = NB_CELLS;
    i = 0;
    total = 0;
    line_nb = 0;
    x = 0;
    y = 0;

    std::string line;
    std::ifstream file(filename);

    if (!file.fail()) {
        // The file must be a text file
        unsigned pos(filename.find_last_of("."));
        std::string extension(filename.substr(pos + 1));
        if (extension != "txt") {
            std::cout << "\x1b[91m" "error: \x1b[0m" "expected a .txt file\n" "\x1b[0m";
            exit(EXIT_FAILURE);
        }
    	// Read the file line by line, ignoring those starting with '#'
        while (getline(file >> std::ws, line)) {
            ++line_nb;
            if (line[0] == '#') continue;
            line_decoding(line, filename);
        }
        line_decoding("error_checker", filename);
    }else {
    	error(READING_OPENING);
    }
}

void Simulation::line_decoding(std::string line, std::string filename) {
    std::istringstream data(line);

    switch(state) {
    case NB_CELLS:
        if (!(data >> total)) {
        	state = END;
        	break;
        }
        if (total < 0 || total > world_size*world_size) {
            std::cout << filename << ": ";
            std::cout << "The specified # of cells is out of range\n";
            state = END;
        }
        else state = COORDINATES;
        break;
    case COORDINATES:
        data >> x >> y;
        if (x < 0 || y < 0 || x >= world_size || y >= world_size) {
            std::cout << filename << ": ";
			std::cout << "Some coordinates are out of range [0, 39]\n";
            std::cout << "  " << line_nb << "  " << "|  ";
            if (x < 0 || x >= world_size) {
                std::cout << "\x1b[91m" << x << " \x1b[0m" << y << "\n";
            }
            else {
                std::cout << x << " " << "\x1b[91m" << y << "\n\x1b[0m";
            }
            state = END;
        }else {
			file_data.push_back({x, y});
        	++i;
        	if (i == total + 1) state = OK;
        	else state = COORDINATES;
        }
        break;
    case END:
        error(READING_END);
        break;
    case OK:
        break;
    default:
        std::cout << "Default\n";
    }
}

void Simulation::error(Error_reading code) {
    switch(code) {
    case READING_OPENING:
        std::cout << "\x1b[91m" "error: \x1b[0m" "failed opening the file: make sure the file exists\n";
        exit(EXIT_FAILURE);
    case READING_END:
        std::cout << "\x1b[91m" "error: \x1b[0m" "failed configuring the simulation: invalid values\n";
     	exit(EXIT_FAILURE);
    default:
        std::cout << "An unknown error occured\n";
       	exit(EXIT_FAILURE);
    }
}

void Simulation::start_sim(Init init) {
	// Read any remaining '\n' character to avoid wrong behavior
	std::cin.ignore(10000, '\n');

	// Store the time when the simulation is about to start in the "start" variable
	clock_t start(clock());
	clock_t end;
	unsigned count(0);
    unsigned nb_start, nb_end;
	if (init == GLIDERGUN_INIT) {
		this->init();
		draw_canon_planeur(0, 0);
    	while(true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(refresh_rate));
			std::cout << "\x1b[2J\x1b[H";
    		update();
            display();
            ++count;
            end = clock();
            if (((float) end - start)/CLOCKS_PER_SEC >= max_time) {
            	break;
            }
		}
	}else {
        if (init == RANDOM_INIT) {
        	this->init();
            unsigned rand_x, rand_y;
            // Initialize the simulation with alive cells of random coordinates
            for (unsigned index(0); index < (world_size*world_size)/2; ++index) {
                rand_x = rand() % world_size;
                rand_y = rand() % world_size;
                new_birth(rand_x, rand_y);
            }
            // Store the initial number of alive cells
            nb_start = (world_size*world_size)/2;
        }else {
        	// This is executed if the simulation is started from a file
        	for (unsigned i(0); i < file_data.size(); ++i) {
        	    new_birth(file_data[i].x, file_data[i].y);
        	}
        	// Store the initial number of alive cells
            nb_start = file_data.size();
        }
        if (stab_end) {
        	// This is executed if the option "end when stabilized" is "On"
    		while (!update(EXPERIMENTAL)) {
        	    std::this_thread::sleep_for(std::chrono::milliseconds(refresh_rate));
			    std::cout << "\x1b[2J\x1b[H";
        	    nb_end = display();
        	    ++count;
                if (nb_end == 0) {
                    std::cout << "Every cell have died\n";
                    end_sim(nb_start, nb_end);
                    return;
                }
    		}
    	}else {
    		while (true) {
    			std::this_thread::sleep_for(std::chrono::milliseconds(refresh_rate));
    			std::cout << "\x1b[2J\x1b[H";
    			update();
    			nb_end = display();
    			++count;
    			end = clock();
    			if (((float) end - start)/CLOCKS_PER_SEC >= max_time) {
    				break;
    			}
                if (nb_end == 0) {
                    std::cout << "\n\x1b[0m" "Every cell have died\n";
                    end_sim(nb_start, nb_end);
                    return;
                }
    		}
    	}
        if (stab_end) {
    		std::cout << "\nStability reached after " << count - oscillation_period;
            if (count - oscillation_period >= 2) {
                std::cout << " steps!\n";
            }else {
                std::cout << " step!\n";
            }
    	}
    }
    if (!stab_end) {
    	std::cout << "\n\nAuto stop after " << max_time << " seconds\n";
	}
    end_sim(nb_start, nb_end);
}

void Simulation::end_sim(unsigned nb_start, unsigned nb_end) {
    // Emit a bell sound
    std::cout << "\a";
    this->init();
    std::cout << "Alive cells\n";
    std::cout << "  Start: " << nb_start << "\n";
    std::cout << "  End: " << nb_end << "\n";
    std::cout << "\x1b[36m" "\33[6m" "Press Enter to continue..." "\x1b[0m" "\33[0m";
    std::cin.get();
}

void Simulation::toggle_stab_end() {
	stab_end = 1 - stab_end;
}

bool Simulation::get_stab_end() {
	return stab_end;
}

unsigned Simulation::get_refrsh_rate() {
	return refresh_rate;
}

void Simulation::new_birth(unsigned x, unsigned y) {
    updated_grid[world_size -1 - y][x] = true;
    ++nb_alive;
}

void Simulation::new_death(unsigned x, unsigned y) {
    updated_grid[world_size - 1 - y][x] = false;
}

void Simulation::birth_test(unsigned x, unsigned y) {
    if (!grid[world_size - 1 - y][x]) {
        if (neighbours(x, y) == 3) {
            new_birth(x, y);
        }
    }else {
        if (neighbours(x, y) == 2 || neighbours(x, y) == 3) {
            new_birth(x, y);
            --nb_alive;
        }else {
            ++nb_dead;
        }
    }
}

unsigned Simulation::neighbours(unsigned x, unsigned y) {
    unsigned n(0);

    if (x == 0 && y == 0) {
        if (grid[world_size-2][y]) ++n;
        if (grid[world_size-2][y+1]) ++n;
        if (grid[world_size-1][y+1]) ++n;
        return n;
    }
    if (x == world_size-1 && y == 0) {
        if (grid[world_size-2][world_size-1]) ++n;
        if (grid[world_size-2][world_size-2]) ++n;
        if (grid[world_size-1][world_size-2]) ++n;
        return n;
    }
    if (x == world_size-1 && y == world_size-1) {
        if (grid[0][world_size-2]) ++n;
        if (grid[1][world_size-2]) ++n;
        if (grid[1][world_size-1]) ++n;
        return n;
    }
    if (x == 0 && y == world_size-1) {
        if (grid[1][0]) ++n;
        if (grid[1][1]) ++n;
        if (grid[0][1]) ++n;
        return n;
    }
    if (x == 0 && y != 0 && y != world_size-1) {
        if (grid[world_size-y][x]) ++n;
        if (grid[world_size-y-2][x]) ++n;
        if (grid[world_size-y][x+1]) ++n;
        if (grid[world_size-y-1][x+1]) ++n;
        if (grid[world_size-y-2][x+1]) ++n;
        return n;
    }
    if (y == 0 && x != 0 && x != world_size-1) {
        if (grid[world_size-1][x-1]) ++n;
        if (grid[world_size-1][x+1]) ++n;
        if (grid[world_size-2][x-1]) ++n;
        if (grid[world_size-2][x]) ++n;
        if (grid[world_size-2][x+1]) ++n;
        return n;
    }
    if (x == world_size-1 && y != 0 && y != world_size-1) {
        if (grid[world_size-y][x]) ++n;
        if (grid[world_size-y-2][x]) ++n;
        if (grid[world_size-y][x-1]) ++n;
        if (grid[world_size-y-1][x-1]) ++n;
        if (grid[world_size-y-2][x-1]) ++n;
        return n;
    }
    if (y == world_size-1 && x != 0 && x != world_size-1) {
        if (grid[0][x-1]) ++n;
        if (grid[0][x+1]) ++n;
        if (grid[1][x-1]) ++n;
        if (grid[1][x]) ++n;
        if (grid[1][x+1]) ++n;
        return n;
    }
    if (grid[world_size - 1 - y - 1][x - 1]) ++n;
    if (grid[world_size - 1 - y - 1][x]) ++n;
    if (grid[world_size - 1 - y - 1][x + 1]) ++n;
    if (grid[world_size - 1 - y][x - 1]) ++n;
    if (grid[world_size - 1 - y][x + 1]) ++n;
    if (grid[world_size - 1 - y + 1][x - 1]) ++n;
    if (grid[world_size - 1 - y + 1][x]) ++n;
    if (grid[world_size - 1 - y + 1][x + 1]) ++n;
    return n;
}

void Simulation::set_refresh(unsigned ref) {
    refresh_rate = ref;
}

bool Simulation::update(Mode mode) {

    past_5_alive = past_4_alive;
    past_4_alive = past_3_alive;
    past_3_alive = past_2_alive;
    past_2_alive = past_alive;
    past_alive = nb_alive;
    nb_alive = 0;
    nb_dead = 0;
    past_stable = stable;
    stable = false;

    for (unsigned i(0); i < grid.size(); ++i) {
        for (unsigned j(0); j < grid[i].size(); ++j) {
            grid[i][j] = updated_grid[i][j];
        }
    }
    for (unsigned i(0); i < updated_grid.size(); ++i) {
        for (unsigned j(0); j < updated_grid[i].size(); ++j) {
            updated_grid[i][j] = false;
        }
    }
    for (unsigned i(0); i < grid.size(); ++i) {
        for (unsigned j(0); j < grid[i].size(); ++j) {
            birth_test(j, world_size - 1 - i);
        }
    }
    // Check for any perdiodic pattern to determine if the state of the simulation is stable
    // Fot the moment, this works for oscillations of a period of 5 and less
    if (mode == EXPERIMENTAL) {
        //std::cout << nb_alive << " " << past_alive << " " << past_2_alive << " " << past_3_alive << "\n";
        if (nb_alive == past_alive && past_alive == past_2_alive && past_2_alive == past_3_alive) {
            stable = true;
        }
        if (nb_alive == past_2_alive && past_alive == past_3_alive && past_2_alive == past_4_alive) {
            stable = true;
        }
        if (nb_alive == past_3_alive && past_alive == past_4_alive && past_2_alive == past_5_alive) {
            stable = true;
        }
        if (stable && past_stable) {
            return true;
        }
    }
    return false;
}

unsigned Simulation::display() {
    unsigned alive(0);
    char square(254);
    for (unsigned i(0); i < grid.size(); ++i) {
        std::cout << "\n";
        for (unsigned j(0); j < grid[i].size(); ++j) {
            if (grid[i][j]) {
                std::cout << square;
                ++alive;
            }else {
                std::cout << " ";
            }
            std::cout << " ";
        }
    }
    return alive;
}

void Simulation::init() {
	for (unsigned i(0); i < grid.size(); ++i) {
		for (unsigned j(0); j < grid[i].size(); ++j) {
			updated_grid[i][j] = false;
		}
	}
    nb_alive = 0;
    nb_dead = 0;
    past_alive = 0;
    past_2_alive = 0;
    past_3_alive = 0;
    past_4_alive = 0;
    past_5_alive = 0;
    stable = 0;
    past_stable = false;
}

void Simulation::draw_canon_planeur(unsigned x, unsigned y) {
   new_birth(x, y + 4);
   new_birth(x, y + 5);
   new_birth(x + 1, y + 4);
   new_birth(x + 1, y + 5);
   new_birth(x + 10, y + 4);
   new_birth(x + 10, y + 5);
   new_birth(x + 10, y + 6);
   new_birth(x + 11, y + 3);
   new_birth(x + 11, y + 7);
   new_birth(x + 12, y + 2);
   new_birth(x + 12, y + 8);
   new_birth(x + 13, y + 2);
   new_birth(x + 13, y + 8);
   new_birth(x + 14, y + 5);
   new_birth(x + 15, y + 3);
   new_birth(x + 15, y + 7);
   new_birth(x + 16, y + 4);
   new_birth(x + 16, y + 5);
   new_birth(x + 16, y + 6);
   new_birth(x + 17, y + 5);
   new_birth(x + 20, y + 2);
   new_birth(x + 20, y + 3);
   new_birth(x + 20, y + 4);
   new_birth(x + 21, y + 2);
   new_birth(x + 21, y + 3);
   new_birth(x + 21, y + 4);
   new_birth(x + 22, y + 1);
   new_birth(x + 22, y + 5);
   new_birth(x + 24, y);
   new_birth(x + 24, y + 1);
   new_birth(x + 24, y + 5);
   new_birth(x + 24, y + 6);
   new_birth(x + 34, y + 2);
   new_birth(x + 34, y + 3);
   new_birth(x + 35, y + 2);
   new_birth(x + 35, y + 3);
}
