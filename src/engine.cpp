/**
 * @file engine.cpp
 * @author Peter Bence (ecneb2000@gmail.com)
 * @brief Implementation of the game logic.
 * @version 0.1
 * @date 2022-10-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <memory>
#include <algorithm>
#include "tinyxml2.h"
#include "engine.hpp"

bool Room::unlock(item* k, Room* r) {
    Key* k2r = dynamic_cast<Key*>(k->release());
    if (k2r != nullptr && k2r->getKeyID() == r->getID()) {
        r->setLock(unlocked);
        delete k2r;
        return true;
    }
    Object* o = dynamic_cast<Object*>(k2r);
    *k = std::make_unique<Object>(*o);
    return false;
}

Room::~Room() {
		for (items::iterator iit = inventory.begin(); iit != inventory.end(); iit++) {
			iit->reset();
		}
		for (npcs::iterator eit = roomPopulation.begin(); eit != roomPopulation.end(); eit++) {
			eit->reset();
		}
	}

bool Mission::checkStatus(Player& player) {
    if (status == finished) return true;
    if (targetItem != -1 && targetRoom == -1) {
        for (auto rit = player.getInventory().cbegin(); rit != player.getInventory().cend(); rit++) {
            if (targetItem == (*rit)->getID()) {
                complete();
                return true;
            }
        }
    }
    if (targetRoom != -1 && targetItem == -1) {
        if (player.getLocation()->getID() == targetRoom) {
            complete();
            return true;
        }
    }
    if (targetItem != -1 && targetRoom != -1) {
        int completed = 0;
        for (auto rit = player.getInventory().cbegin(); rit != player.getInventory().cend(); rit++) {
            if (targetItem == (*rit)->getID()) {
                completed++;
                break;
            }
        }
        if (player.getLocation()->getID() == targetRoom) {
            completed++;
        }
        if (completed == 2) {
            complete();
            return true;
        }
    }
    return false;
}

NPC::~NPC() {
		for (auto mit = missions_to_give.begin(); mit != missions_to_give.end(); mit++) {
			delete *mit;
		}
	}

World::World(const char* path2story) {
    try {
        story.LoadFile(path2story);
    } catch(int errorID) {
        std::cerr << "Error: Cant load story file " << path2story << std::endl;
        exit(1);
    }
}

item* World::searchKey(int roomID) {
    for (items::iterator it = player.getInventory().begin(); it != player.getInventory().end(); it++) {
        Key* k = dynamic_cast<Key*>(it->get());
        if (k != nullptr) {
            if (k->getKeyID() == roomID) {
                return (&(*it));
            }
        }
    }
    return nullptr; 
} 

Room* World::lookUpRoom(int id) {
    nodes::iterator found_room = find_if(
        worldRooms.begin(), worldRooms.end(),
        [&id](node& element){
            if (element->getID() == id) {
                return true;
            } else return false;
        }
    );
    if (found_room == worldRooms.end()) return nullptr;
    return found_room->get();
}

bool World::enterRoom(int id) {
    Room* r = lookUpRoom(id);
    if (r) {
        if (r->isLocked()) {
            item* key = searchKey(id);
            if (key) {
                if (Room::unlock(key, r)) {
                    player.setLocation(r);
                    return true;
                }
            }
            return false;
        }
        player.setLocation(r);
        return true;
    }
    exit(1); //Exit failure, dont enter room id that is not a neighbour.
    return false;
}

void World::RoomFactory(const std::string& name, int id, const std::string& desc, const std::string& choice, LockStatus ls, neighbours nids) {
    auto newRoom = std::make_unique<Room>(name, id, desc, choice, ls, nids);
    worldRooms.emplace_back(std::move(newRoom));
}

tinyxml2::XMLElement* checkXMLElement(tinyxml2::XMLElement* parent, const std::string& element_name) {
    tinyxml2::XMLElement* e = parent->FirstChildElement(element_name.c_str());
    if (e == nullptr) {
        std::cout << "XML Error at line: " << parent->GetLineNum() << std::endl;
        exit(1);
    }
    return e;
}

items World::makeInventory(tinyxml2::XMLElement* invEle) {
    tinyxml2::XMLElement* firstObj = invEle->FirstChildElement("object");
    items retItems;
    if (firstObj) {
        tinyxml2::XMLElement* actual = firstObj;
        while(actual) {
            const char* name_c_string = checkXMLElement(actual, "name")->GetText();
            const char* desc_c_string = checkXMLElement(actual, "description")->GetText();
            if (name_c_string == nullptr || desc_c_string == nullptr) {
                std::cout << "XML Error: Bad object tags given at line: " << actual->GetLineNum() << std::endl;
                exit(1);
            }
            const std::string objName(name_c_string);
            const std::string desc(desc_c_string);
            int id = -1;
            checkXMLElement(actual, "id")->QueryIntText(&id);
            if (id == -1) {
                std::cout << "XML Error: Bad object tags given at line: " << actual->GetLineNum() << std::endl;
                exit(1);
            }
            auto newObj = std::make_unique<Object>(objName, id, desc);
            retItems.push_back(std::move(newObj));
            actual = actual->NextSiblingElement("object");
        }
    }
    tinyxml2::XMLElement* firstKey = invEle->FirstChildElement("key");
    if (firstKey) {
        tinyxml2::XMLElement* actualKey = firstKey;
        while(actualKey) {
            const char* key_name_c_string = actualKey->FirstChildElement("name")->GetText();
            const char* key_desc_c_string = actualKey->FirstChildElement("description")->GetText();
            if (key_name_c_string == nullptr || key_desc_c_string == nullptr) {
                std::cout << "XML Error: Bad key tags given at line: " << actualKey->GetLineNum() << std::endl;
                exit(1);
            }
            const std::string key_name(key_name_c_string);
            const std::string key_desc(key_desc_c_string);
            int id = -1;
            actualKey->FirstChildElement("id")->QueryIntText(&id);
            int keyID = -1; 
            actualKey->FirstChildElement("keyID")->QueryIntText(&keyID);
            if (id == -1 || keyID == -1 ) {
                std::cout << "XML Error: Bad key tags given at line: " << actualKey->GetLineNum() << std::endl;
                exit(1);
            }
            auto newKey= std::make_unique<Key>(keyID, key_name, id, key_desc);
            retItems.push_back(std::move(newKey));
            actualKey = actualKey->NextSiblingElement("key");
        }
    }
    return retItems;
}

neighbours World::parseConnections(tinyxml2::XMLElement* conns) {
    tinyxml2::XMLElement* firstConn = checkXMLElement(conns, "id");
    tinyxml2::XMLElement* actual = firstConn;
    std::vector<int> connections;
    while(actual) {
        int nid = 0;
        actual->QueryIntText(&nid);
        connections.push_back(nid); 
        actual = actual->NextSiblingElement("id");
    }
    return connections;
}

void World::loadRooms(tinyxml2::XMLElement* firstRoom) {
    const char* elementName = "room";
    tinyxml2::XMLElement* actual = firstRoom;
    while(actual) {
        tinyxml2::XMLElement* name = checkXMLElement(actual, "name");
        const char * roomName_cstring = name->GetText();
        tinyxml2::XMLElement* description = checkXMLElement(actual, "description");
        const char * roomDesc_cstring = description->GetText();
        tinyxml2::XMLElement* choice_description = actual->FirstChildElement("choice");
        if (choice_description == nullptr) {
            std::cout << "XML Error: Bad room choice tags given at line: " << actual->GetLineNum() << std::endl;
            exit(1);
        }
        const char * choiceDesc_cstring = choice_description->GetText();
        if (roomName_cstring == nullptr || roomDesc_cstring == nullptr || choiceDesc_cstring == nullptr) {
            std::cout << "XML Error: Bad room tags given at line: " << actual->GetLineNum() << std::endl;
            exit(1);
        }
        const std::string roomName(roomName_cstring);
        const std::string roomDescription(roomDesc_cstring);
        const std::string choiceDescription(choiceDesc_cstring);
        tinyxml2::XMLElement* id = actual->FirstChildElement("id");
        if (id == nullptr) {
            std::cout << "XML Error: Bad room id tags given at line: " << actual->GetLineNum() << std::endl;
            exit(1);
        }
        int roomID = 0;
        id->QueryIntText(&roomID);
        tinyxml2::XMLElement* lock = actual->FirstChildElement("lock");
        if (lock == nullptr) {
            std::cout << "XML Error: Bad room lock tags given at line: " << actual->GetLineNum() << std::endl;
            exit(1);
        }
        LockStatus ls;
        int lockInt = -1;
        lock->QueryIntText(&lockInt);
        if (lockInt != -1) {
            if (lockInt == 0) {
                ls = unlocked;
            } else ls = locked;
        } else {
            std::cout << "XML Error: Bad room lock tags given at line: " << actual->GetLineNum() << std::endl;
            exit(1); //Exit failure, because there is no value in lock, or no lock tag.
        }
        tinyxml2::XMLElement* conns = actual->FirstChildElement("connections");
        if (conns == nullptr) {
            std::cout << "XML Error: Bad room connections tags given at line: " << actual->GetLineNum() << std::endl;
            exit(1);
        }
        RoomFactory(roomName, roomID, roomDescription, choiceDescription, ls, parseConnections(conns));
        tinyxml2::XMLElement* invEle = actual->FirstChildElement("inventory");
        if (invEle) {
            items inv = makeInventory(invEle);
            worldRooms.back()->addItems(inv);
        }
        tinyxml2::XMLElement* entEle = actual->FirstChildElement("entity");
        if (entEle) {
            loadEntities(entEle, worldRooms.back());
        }
        actual = actual->NextSiblingElement(elementName);
    }
}

void World::loadEntities(tinyxml2::XMLElement* firstEle, node& r) {
    tinyxml2::XMLElement* actual = firstEle;
    while(actual) {
        tinyxml2::XMLElement* name = actual->FirstChildElement("name");
        if (name == nullptr) {
            std::cout << "XML Error: Bad entity name tags given at line: " << actual->GetLineNum() << std::endl;
            exit(1);
        }
        const char * name_cstring = name->GetText();
        tinyxml2::XMLElement* dialog = actual->FirstChildElement("dialog");
        if (dialog == nullptr) {
            std::cout << "XML Error: Bad entity dialog tags given at line: " << actual->GetLineNum() << std::endl;
            exit(1);
        }
        const char * dialog_cstring = dialog->GetText();
        if (name_cstring == nullptr || dialog_cstring == nullptr) {
            std::cout << "XML Error: Bad entity tags given at line: " << actual->GetLineNum() << std::endl;
            exit(1);
        }
        const std::string ent_name(name_cstring);
        const std::string ent_dialog(dialog_cstring);
        tinyxml2::XMLElement* invele = actual->FirstChildElement("inventory");
        tinyxml2::XMLElement* missionEle = actual->FirstChildElement("missions");
        if (missionEle) {
            missions npc_missions = loadNPCMissions(missionEle);
            npc newEnt = std::make_unique<NPC>(ent_name, ent_dialog, npc_missions);
            if (invele) {
                items inv = makeInventory(invele);
                newEnt->addItems(inv);
            }
            r->addEntity(newEnt);
        } else {
            std::cout << "XMl Error: Entity must have missions and inventory. Error at line: " << actual->GetLineNum() << std::endl;
            exit(0);
        }
        actual = actual->NextSiblingElement("entity");
    }
}

Mission* World::makeMission(tinyxml2::XMLElement* missionEle) {
    std::string description = checkXMLElement(missionEle, "description")->GetText();
    Mission* retMission = new Mission(description);
    int targetRoomID = -1;
    int targetItemID = -1;
    tinyxml2::XMLElement* targetRoomEle = missionEle->FirstChildElement("targetRoom");
    if (targetRoomEle) {
        targetRoomEle->QueryIntText(&targetRoomID);
        if (targetRoomID != -1) {
            retMission->setTargetRoom(targetRoomID);
        }
    } else retMission->setTargetRoom(targetRoomID);
    tinyxml2::XMLElement* targetItemEle = missionEle->FirstChildElement("targetItem");
    if (targetItemEle) {
        targetItemEle->QueryIntText(&targetItemID);
        if (targetItemID != -1) {
            retMission->setTargetItem(targetItemID);
        }
    } else retMission->setTargetItem(targetItemID);
    retMission->startMission(); // Set mission status to active.
    return retMission;
}

void World::loadWorldMissions(tinyxml2::XMLElement* missionsEle) {
    tinyxml2::XMLElement* firstMission = missionsEle->FirstChildElement("mission");
    if (firstMission) {
        tinyxml2::XMLElement* actual = firstMission;
        while (actual) {
            active_missions.emplace_back(makeMission(actual));
            actual = actual->NextSiblingElement("mission");
        }
    }
}

missions World::loadNPCMissions(tinyxml2::XMLElement* missionsEle) {
    tinyxml2::XMLElement* firstMission = missionsEle->FirstChildElement("mission");
    missions npc_missions;
    if (firstMission) {
        tinyxml2::XMLElement* actual = firstMission;
        while (actual) {
            npc_missions.push_back(makeMission(actual));
            actual = actual->NextSiblingElement("mission");
        }
    }
    return npc_missions;
}

bool World::setPlayerSpawn(tinyxml2::XMLElement* spawnEle) {
    int spawnID = -1;
    spawnEle->QueryIntText(&spawnID);
    for (nodes::iterator it = worldRooms.begin(); it != worldRooms.end(); it++) {
        if ((*it)->getID() == spawnID) {
            player.setLocation(it->get());
            return true;
        }
    }
    return false;
}

void World::cleanInventories() {
    for (auto r = this->worldRooms.begin(); r != this->worldRooms.end(); r++) {
        auto i = std::remove_if(
            (*r)->getItems().begin(),
            (*r)->getItems().end(),
            [](item& i) {
                if (i.get()) return false;
                return true;
            }
        );
        (*r)->getItems().erase(i, (*r)->getItems().end());
    }
    for (auto r = this->worldRooms.begin(); r != this->worldRooms.end(); r++) {
        for (auto e = (*r)->getPopulation().begin(); e != (*r)->getPopulation().end(); e++) {
            auto i = std::remove_if(
                (*e)->getInventory().begin(),
                (*e)->getInventory().end(),
                [](item& i) {
                    if (i.get()) return false;
                    return true;
                }
            );
            (*e)->getInventory().erase(i, (*e)->getInventory().end());
        }
    }
    auto iit = std::remove_if(
        this->player.getInventory().begin(),
        this->player.getInventory().end(),
        [](item& i) {
            if (i.get()) return false;
            return true;
        }
    );
    player.getInventory().erase(iit, player.getInventory().end());
}

void World::initWorld(const std::string& path2story) {
    try {
        story.LoadFile(path2story.c_str());
    } catch(int errorID) {
        std::cerr << "Error: Cant load story file: " << path2story << std::endl;
        exit(1);
    }
    tinyxml2::XMLElement* worldElement = story.FirstChildElement("world");
    if (worldElement==nullptr) {
        std::cout << "XMl Error: no world element." << std::endl;
        exit(1);
    }
    tinyxml2::XMLElement* firstRoom = checkXMLElement(worldElement, "room");
    if (firstRoom==nullptr) {
        std::cout << "XMl Error: no room element." << std::endl;
        exit(1);
    }
    loadRooms(firstRoom);
    tinyxml2::XMLElement* missionsElem = worldElement->FirstChildElement("missions");
    if (missionsElem!=nullptr) {
        loadWorldMissions(missionsElem);
    }
    if (setPlayerSpawn(worldElement->FirstChildElement("spawn")));
    else {
        std::cerr << "Error: no player spawn location given." << std::endl;
        // Exit failure
        exit(1);
    }
}

void World::destroyWorld() {
    for (nodes::iterator it = worldRooms.begin(); it != worldRooms.end(); it++) {
        it->reset();
    }
    for (auto mit = active_missions.begin(); mit != active_missions.end(); mit++) {
        delete *mit;
    }
}

World::~World() {
    this->destroyWorld();
}