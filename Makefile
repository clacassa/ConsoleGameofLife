# cgol (Console Game of Life) -- run the game of life in the terminal
# Copyright (C) 2022 Cyprien Lacassagne

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

OUT = cgol
CXX = g++
CXXFLAGS = -Wall -g -std=c++11
CXXFILES = main.cc simulation.cc config.cc
OFILES = $(CXXFILES:.cc=.o)
EXEDIR = ./bin
SRCDIR = ./src
VPATH = $(SRCDIR):$(EXEDIR)

ifeq ($(OS),Windows_NT)
OFILES += ./res/my_res
endif

all: $(EXEDIR)/$(OUT)

$(EXEDIR)/$(OUT): $(SRCDIR)/main.o $(SRCDIR)/simulation.o $(SRCDIR)/config.o
	$(CXX) $(SRCDIR)/main.o $(SRCDIR)/simulation.o $(SRCDIR)/config.o -o $@

$(SRCDIR)/main.o: main.cc simulation.h config.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SRCDIR)/simulation.o: simulation.cc simulation.h config.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SRCDIR)/config.o: config.cc config.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -f $(SRCDIR)/*.o $(EXEDIR)/$(OUT) *.cc~ *.h~
