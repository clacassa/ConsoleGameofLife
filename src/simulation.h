// Console Game of Life (cgol) -- Lightweight terminal integrated 2D simulation
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

#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <vector>
#include <string>

enum Error_reading { READING_OPENING, READING_END };
enum Mode { EXPERIMENTAL, NORMAL };
enum Init { RANDOM_INIT, GLIDERGUN_INIT, FILE_INIT };

constexpr unsigned world_size(40);
constexpr unsigned init_refresh(100);
constexpr unsigned max_time(150);
const std::string error_checker("dontforgetlastline");

class Simulation {
	struct Cell {
		unsigned x;
		unsigned y;
	};
    unsigned refresh_rate;
    typedef std::vector<std::vector<bool>> Grid;
    Grid grid;
    Grid updated_grid;
    std::vector<Cell> file_data;
    bool stab_end;
public:
    Simulation(int rfrsh_rate);
    ~Simulation();
    void read_file(std::string filename);
    void line_decoding(std::string line);
    void error(Error_reading code);

    unsigned display();
    void init();
    void set_refresh(unsigned ref);
    void toggle_stab_end();
    void start_sim(Init init = GLIDERGUN_INIT);
    bool update(Mode mode = NORMAL);

    bool get_stab_end();
    unsigned get_refrsh_rate();

    void draw_canon_planeur(unsigned x, unsigned y);

    unsigned neighbours(unsigned x, unsigned y);
    void birth_test(unsigned x, unsigned y);

    void new_birth(unsigned x, unsigned y);
    void new_death(unsigned x, unsigned y);
};

#endif
