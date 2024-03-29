/**
 * @file interface.cpp
 * @author Peter Bence (ecneb2000@gmail.com)
 * @brief Implementation of the interface that connects the game and the game engine.
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

bool Move::doAction(int id) {
    return game_world.enterRoom(id);
}

bool Move::operator ()(int id) {
    return doAction(id);
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

std::string Interact::doAction(npc& n) {
    missions npc_missions = n->getMissions();
    if (npc_missions.size() == 0) return n->getDialog();
    for (auto it = npc_missions.begin(); it != npc_missions.end(); it++) {
        if ((*it)->getStatus() == finished) return n->getDialog();
        if ((*it)->checkStatus(game_world.getPlayer())) {
            //move npc items to players inventory
            if (n->getInventory().size() > 0) {
                game_world.getPlayer().addItem(n->getInventory()[0]);
                //remove target item from players inventory
                auto rm_it = remove_if(
                    game_world.getPlayer().getInventory().begin(),
                    game_world.getPlayer().getInventory().end(),
                    [&it](item& i) {
                        if (i->getID() == (*it)->getTargetItem()) return true;
                        return false;
                    }
                );
                game_world.getPlayer().getInventory().erase(rm_it, game_world.getPlayer().getInventory().end());
            }
            //return npc's dialog
            return n->getDialog();
        }
    }
    // if mission is not finished then npc's dialog cannot be accessed
    return n->getDialogNoAcess();
}

std::string Interact::operator()(npc& n) {
    return doAction(n);
}

AcceptMission::AcceptMission(const std::string& desc, World& gm) : Action(desc, gm) {}

void AcceptMission::doAction(Mission* m) {
    game_world.startMission(m);
}

void AcceptMission::operator ()(Mission* m) {
    doAction(m);
}

OpenInventory::OpenInventory(const std::string& desc, World& gm) : Action(desc, gm) {}

items& OpenInventory::doAction() {
    return game_world.getPlayer().getInventory();
}

items& OpenInventory::operator()() {
    return doAction();
}