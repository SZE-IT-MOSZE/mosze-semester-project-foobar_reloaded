/**
 * @file interface.cpp
 * @author Peter Bence (ecneb2000@gmail.com)
 * @brief Implementation of the interface that connects the game and the game eninge.
 * @version 0.1
 * @date 2022-10-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <sstream>
#include "engine.cpp"
#include "interface.hpp"

Action::Action(const std::string& desc, World& gm) : description(desc), game_world(gm) {}

std::string Action::getDescription() {
    return description;
}

Move::Move(const std::string& desc, World& gm) : Action(desc, gm) {}

void Move::doAction(int id) {
    game_world.enterRoom(id);
}

void Move::operator ()(int id) {
    doAction(id);
}

Search::Search(const std::string& desc, World& gm) : Action(desc, gm) {}

search_results Search::doAction() {
    Room* r = game_world.getPlayer().getLocation();
    search_results results = search_results(r->getItems(), r->getPopulation()); 
    return results;
}

search_results Search::operator ()() {
    return doAction();
}

PickUp::PickUp(const std::string& desc, World& gm) : Action(desc, gm) {}

void PickUp::doAction(item& i) {
    game_world.getPlayer().addItem(i);
}

void PickUp::operator ()(item& i) {
    doAction(i);
}

Interact::Interact(const std::string& desc, World& gm) : Action(desc, gm) {}

std::string Interact::doAction(npc& npc) {
    missions npc_missions = npc->getMissions();
    for (missions::iterator it = npc_missions.begin(); it != npc_missions.end(); it++) {
        if (it->checkStatus(game_world.getPlayer())) {
            //move npc items to players inventory
            game_world.getPlayer().addItem(npc->getInventory()[0]);
            //remove target item from players inventory
            auto rm_it = remove_if(
                game_world.getPlayer().getInventory().begin(),
                game_world.getPlayer().getInventory().end(),
                [&it](item& i) {
                    if (i->getID() == it->getTargetItem()) return true;
                    return false;
                }
            );
            game_world.getPlayer().getInventory().erase(rm_it, game_world.getPlayer().getInventory().end());
            //return npc's dialog
            return npc->getDialog();
        }
    }
    // if mission is not finished then npc's dialog cannot be accessed
    return npc->getDialogNoAcess();
}

AcceptMission::AcceptMission(const std::string& desc, World& gm) : Action(desc, gm) {}

void AcceptMission::doAction(Mission& m) {
    game_world.startMission(m);
}

void AcceptMission::operator ()(Mission& m) {
    doAction(m);
}

OpenInventory::OpenInventory(const std::string& desc, World& gm) : Action(desc, gm) {}

items& OpenInventory::doAction() {
    return game_world.getPlayer().getInventory();
}

items& OpenInventory::operator()() {
    return doAction();
}

SessionManager::SessionManager(const std::string& path) {
    game_world.initWorld(path);
}

void SessionManager::startSession() {
    while (doIteration());
}

bool SessionManager::doMove() {
    //TODO can implement neighbours better, not as integers in a vector rather than pointers. 
    auto neighbours = game_world.getPlayer().getLocation()->getNeighbours();
    nodes& world_rooms = game_world.getWorldRooms();
    std::vector<Room*> choices;
    for (size_t i = 0; i < world_rooms.size(); i++) {
        for (size_t j = 0; j < neighbours.size(); j++) {
            if (world_rooms[i]->getID() == neighbours[j]) {
                std::cout << j+1 << ". " << world_rooms[i]->getChoiceDescription() << std::endl; 
                choices.push_back(world_rooms[i].get());
            }
        }
    }
    std::string input;
    int choice;
    do {
        std::cout << "Szoba sorszáma: "; getline(std::cin, input); 
        std::stringstream(input) >> choice;
        if (choice < 0 || choice > int(neighbours.size())) {
            std::cout << "Rossz parancs, próbáld újra." << std::endl;
        }
    } while (choice < 0 || choice > int(neighbours.size()));
    if (choice == 0) return false;
    Move move_action = Move("Mozgás a kiválasztott szobába.", game_world);
    move_action(choices[choice-1]->getID());
    return true;
}

bool SessionManager::doIteration() {
    //TODO implement iteration
    return true;
}