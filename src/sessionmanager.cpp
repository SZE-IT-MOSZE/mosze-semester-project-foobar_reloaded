/**
 * @file sessionmanager.hpp
 * @author Peter Bence (ecneb2000@gmail.com)
 * @brief This is the implementation of the SessionManager module. The session manager handles the user input.
 * @version 0.1
 * @date 2022-11-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "interface.cpp"
#include "sessionmanager.hpp"
#include <stdexcept>

int promptChoice(int max) {
    std::string input;
    int choice;
    do {
        std::cout << "Választás sorszáma: "; getline(std::cin, input); 
        if (std::strcmp("Close", input.c_str()) == 0) exit(0);
        try {
            choice = std::stoi(input, nullptr);
        } catch (const std::invalid_argument& ia) {
            choice = -1;
        }
        if (choice < 0 || choice > max) {
            std::cout << "Rossz parancs, próbáld újra. (Kilépéshez írd be a - Close - parancsot.)" << std::endl;
        }
    } while (choice < 0 || choice > max);
    return choice;
}

SessionManager::SessionManager(const std::string& path) {
    game_world.initWorld(path);
}

bool SessionManager::startSession() {
    while (!doIteration()) game_world.cleanInventories(); // Run inventory thrash cleaner method.
    game_world.cleanInventories();
    return true;
}

bool SessionManager::doMove() {
    //TODO can implement neighbours better, not as integers in a vector rather than pointers. 
    auto neighbours = game_world.getPlayer().getLocation()->getNeighbours();
    nodes& world_rooms = game_world.getWorldRooms();
    std::vector<Room*> choices;
    std::cout << "0. Vissza" << std::endl;
    for (size_t i = 0; i < world_rooms.size(); i++) {
        for (size_t j = 0; j < neighbours.size(); j++) {
            if (world_rooms[i]->getID() == neighbours[j]) {
                std::cout << j+1 << ". " << world_rooms[i]->getChoiceDescription() << std::endl; 
                choices.push_back(world_rooms[i].get());
            }
        }
    }
    int choice = promptChoice(int(neighbours.size()));
    if (choice == 0) return false;
    Move move_action = Move("Mozgás a kiválasztott szobába.", game_world);
    if (move_action(choices[choice-1]->getID())) {
        std::cout << move_action.getDescription() << std::endl;
        return true;
    }
    std::cout << "Ebbe a szobába sajnos, most nem tudsz bemenni, mivel csak kulccsal nyílik. Találd meg a kulcsot!" << std::endl;
    return false;
}

bool SessionManager::doSearch() {
    Search search_action = Search("Átkutatod a szobát.", game_world);
    search_results results = search_action();
    std::cout << "0. Vissza" << std::endl;
    std::cout << "1. Tárgyak" << std::endl; 
    std::cout << "2. NPC-k" << std::endl;
    int choice = promptChoice(2);
    switch (choice)
    {
    case 0:
        return false;
    case 1:
        return doInspectFoundItems(results.first);
    case 2:
        return doInteract(results.second);
    }
    return false;
}

bool SessionManager::doPickUp(item& selected_item) {
    PickUp pickup_action = PickUp("Felveszed a targyat.", game_world);
    pickup_action(selected_item);
    return true;
}

bool SessionManager::doInspectFoundItem(item& selected_item) {
    std::cout << selected_item->getName() << ": " << selected_item->getDescription() << std::endl;
    std::cout << "0. Vissza" << std::endl;
    std::cout << "1. Eltesz" << std::endl;
    int choice = promptChoice(2);
    switch (choice)
    {
    case 0:
        return false; 
    case 1:
        return doPickUp(selected_item);
    }
    return false;
}

bool SessionManager::doInspectFoundItems(items& found_items) {
    std::cout << "0. Vissza" << std::endl;
    for (int i = 0; i < int(found_items.size()); i++) {
        std::cout << i+1 << ". " << found_items[i]->getName() << std::endl;
    }
    int choice = promptChoice(int(found_items.size()));
    if (choice == 0) return false;
    return doInspectFoundItem(found_items[choice-1]);
}

bool SessionManager::doInteract(npcs& found_npcs) {
    std::cout << "Talált NPC-k a szobában:" << std::endl;
    std::cout << "0. Vissza" << std::endl;
    for (int i = 0; i < int(found_npcs.size()); i++) {
        std::cout << i+1 << ". " << found_npcs[i]->getName() << std::endl;
    }
    int choice = promptChoice(int(found_npcs.size()));
    if (choice == 0) return false;
    Interact interact_action = Interact("Beszéd az NPC-vel.", game_world);
    std::cout << interact_action(found_npcs[choice-1]) << std::endl;
    return true;
}

bool SessionManager::doInspectPlayerInventory() {
    items& player_inventory = game_world.getPlayer().getInventory();
    std::cout << "0. Vissza" << std::endl;
    for (int i = 0; i < int(player_inventory.size()); i++) {
        std::cout << i+1 << ". " << player_inventory[i]->getName() << std::endl;
    }
    int choice = promptChoice(int(player_inventory.size()));
    if (choice == 0) return false;
    return doInspectInventoryItem(player_inventory[choice-1]);
}

bool SessionManager::doInspectInventoryItem(item& i) {
    std::cout << i->getDescription() << std::endl;
    std::cout << "0. Vissza" << std::endl;
    std::cout << "1. Töröl" << std::endl;
    int choice = promptChoice(1);
    if (choice == 0) return false;
    return false;
}

bool SessionManager::doIteration() {
    std::cout << "\n";
    // Print out the room's desciption, that the player is in.
    std::cout << game_world.getPlayer().getLocation()->getDescription() << std::endl;
    std::cout << "\n";

    // List out possible choices.
    std::cout << "1. Mozgás kiválasztott szobába." << std::endl;
    std::cout << "2. Kutasd át a szobát." << std::endl;
    std::cout << "3. Saját tárgyak." << std::endl;

    // Prompt for user input.
    int choice = promptChoice(3);
    switch (choice)
    {
    case 1:
        doMove();
        break;
    case 2:
        doSearch();
        break;
    case 3:
        doInspectPlayerInventory();
        break;
    default:
        break;
    }

    // Check for game finish.
    bool game_finished = true;
    for (auto it = game_world.getActiveMissions().begin(); it != game_world.getActiveMissions().end(); it++) {
        if ((*it)->getStatus() == active) game_finished = false;
    }
    for (auto it = game_world.getWorldRooms().begin(); it != game_world.getWorldRooms().end(); it++) {
        for (auto npc_it = (*it)->getPopulation().begin(); npc_it != (*it)->getPopulation().end(); npc_it++) {
            for (auto mission_it = (*npc_it)->getMissions().begin(); mission_it != (*npc_it)->getMissions().end(); mission_it++) {
                if ((*mission_it)->getStatus() == active) game_finished = false;
            }
        }
    }
    return game_finished;
}