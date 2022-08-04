#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string>

#if _WIN32
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif // _WIN32

#define AUTHOR "Cyprien Lacassagne"
#define VERSION "1.0"

void print_version(std::string prog_name);
void print_help(std::string prog_name);

#endif // CONFIG_H
