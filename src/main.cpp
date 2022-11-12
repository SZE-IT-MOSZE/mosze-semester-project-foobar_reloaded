/**
 * @file main.cpp
 * @author Peter Bence (ecneb2000@gmail.com)
 * @brief Main loop implementation of "SpaceWalk TheGame"
 * @version 0.1
 * @date 2022-10-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "sessionmanager.cpp"
#include "Config.h"
#include <string>
#include <iostream>
#include <cstring>

int main(int argc, char** argv) {
    char flag[] = "--story";
    if (argc != 3 || std::strcmp(argv[1], flag) != 0) {
        std::cout << "Usage: program --story [PATH]" << std::endl;
        return 1; 
    }
    const std::string path(argv[2]);
    SessionManager game_session(path);
    game_session.startSession();
    return 0;
}