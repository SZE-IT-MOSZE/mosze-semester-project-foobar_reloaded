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
#include "engine.hpp"
#include "interface.hpp"

Action::Action(const std::string& desc, World& gm) : description(desc), game_world(gm) {}

std::string Action::getDescription() {
    return description;
}

Move::Move(const std::string& desc, World& gm) : Action(desc, gm) {}

void Move::doAction(node& r) {
    game_world.enterRoom(r);
}

void Move::operator ()(node& r) {
    doAction(r);
}

Search::Search(const std::string& desc, World& gm) : Action(desc, gm) {}

search_results Search::doAction(Room* r) {
    search_results results = search_results(r->getItems(), r->getPopulation()); 
    return results;
}

search_results Search::operator ()(Room* r) {
    return doAction(r);
}

PickUp::PickUp(const std::string& desc, World& gm) : Action(desc, gm) {}

void PickUp::doAction(item& i) {
    game_world.getPlayer().addItem(i);
}

void PickUp::operator ()(item& i) {
    doAction(i);
}

Interact::Interact(const std::string& desc, World& gm) : Action(desc, gm) {}

missions Interact::doAction(npc& npc) {
    std::cout << npc->getDialog();
    return npc->getMissions();
}

AcceptMission::AcceptMission(const std::string& desc, World& gm) : Action(desc, gm) {}

void AcceptMission::doAction(Mission& m) {
    game_world.startMission(m);
}

void AcceptMission::operator ()(Mission& m) {
    doAction(m);
}