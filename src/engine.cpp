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
#include "engine.hpp"

bool Room::unlock(item& k, node& r) {
    Key* k2r = dynamic_cast<Key*>(k.release());
    if (k2r != nullptr && k2r->getKeyID() == r->getID()) {
        r->setLock(unlocked);
        delete k2r;
        return true;
    }
    Object* o = dynamic_cast<Object*>(k2r);
    k = std::make_unique<Object>(*o);
    return false;
}

bool Mission::checkStatus(Player& player) {
    if (targetRoom == -1) {
        for (auto rit = player.getInventory().cbegin(); rit != player.getInventory().cend(); rit++) {
            if (targetItem == (*rit)->getID()) {
                this->complete();
                return true;
            }
        }
    }
    if (targetItem == -1) {
        if (player.getLocation()->getID() == targetRoom) {
            this->complete();
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
            this->complete();
            return true;
        }
    }
    return false;
}

void World::RoomFactory(const std::string& name, int id, const std::string& desc, neighbours nids) {
    auto newRoom = std::make_unique<Room>(name, id, desc, nids);
    worldRooms.emplace_back(std::move(newRoom));
}

items World::makeInventory(tinyxml2::XMLElement* invEle) {
    tinyxml2::XMLElement* firstObj = invEle->FirstChildElement("object");
    tinyxml2::XMLElement* actual = firstObj;
    items retItems;
    while(actual) {
        const std::string objName(actual->FirstChildElement("name")->GetText());
        const std::string desc(actual->FirstChildElement("description")->GetText());
        int id = 0;
        actual->FirstChildElement("id")->QueryIntText(&id);
        auto newObj = std::make_unique<Object>(objName, id, desc);
        retItems.push_back(std::move(newObj));
        actual = actual->NextSiblingElement("object");
    }
    return retItems;
}

neighbours World::parseConnections(tinyxml2::XMLElement* conns) {
    tinyxml2::XMLElement* firstConn = conns->FirstChildElement("id");
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
        const std::string roomName(actual->FirstChildElement("name")->GetText());
        const std::string roomDescription(actual->FirstChildElement("description")->GetText());
        int roomID = 0;
        actual->FirstChildElement("id")->QueryIntText(&roomID);
        tinyxml2::XMLElement* conns = actual->FirstChildElement("connections");
        RoomFactory(roomName, roomID, roomDescription, parseConnections(conns)); // Construct room with name, id and desc
        tinyxml2::XMLElement* invEle = actual->FirstChildElement("inventory");
        tinyxml2::XMLElement* actualInv = invEle->FirstChildElement("object");
        while(actualInv) {
            const std::string objName(actualInv->FirstChildElement("name")->GetText());
            const std::string desc(actualInv->FirstChildElement("description")->GetText());
            int objectID = 0;
            actualInv->FirstChildElement("id")->QueryIntText(&objectID);
            auto tmpItem = std::make_unique<Object>(objName, objectID, desc);
            worldRooms.back()->addItem(tmpItem);
            actualInv = actualInv->NextSiblingElement("object");
        }
        loadEntities(actual->FirstChildElement("entity"), worldRooms.back());
        actual = actual->NextSiblingElement(elementName);
    }
}

void World::loadEntities(tinyxml2::XMLElement* firstEle, node& r) {
    tinyxml2::XMLElement* actual = firstEle;
    while(actual) {
        const std::string name = actual->FirstChildElement("name")->GetText();
        const std::string dialog = actual->FirstChildElement("dialog")->GetText();
        tinyxml2::XMLElement* invele = actual->FirstChildElement("inventory");
        items inv = makeInventory(invele);
        tinyxml2::XMLElement* missionEle = actual->FirstChildElement("missions");
        missions npc_missions = loadNPCMissions(missionEle);
        npc newEnt = std::make_unique<NPC>(name, dialog, npc_missions);
        newEnt->addItems(inv);
        r->addEntity(newEnt);
        actual = actual->NextSiblingElement("entity");
    }
}

Mission World::makeMission(tinyxml2::XMLElement* missionEle) {
    std::string description = missionEle->FirstChildElement("description")->GetText();
    Mission retMission(description);
    int targetRoomID = -1;
    int targetItemID = -1;
    tinyxml2::XMLElement* targetRoomEle = missionEle->FirstChildElement("targetRoom");
    if (targetRoomEle != 0) {
        targetRoomEle->QueryIntText(&targetRoomID);
        if (targetRoomID != -1) {
            retMission.setTargetRoom(targetRoomID);
        }
    }
    tinyxml2::XMLElement* targetItemEle = missionEle->FirstChildElement("targetItem");
    if (targetItemEle != 0) {
        targetItemEle->QueryIntText(&targetItemID);
        if (targetItemID != -1) {
            retMission.setTargetItem(targetItemID);
        }
    }
    return retMission;
}

void World::loadWorldMissions(tinyxml2::XMLElement* missionsEle) {
    tinyxml2::XMLElement* firstMission = missionsEle->FirstChildElement("mission");
    tinyxml2::XMLElement* actual = firstMission;
    while (actual) {
        active_missions.push_back(makeMission(actual));
        actual = actual->NextSiblingElement("mission");
    }
}

missions World::loadNPCMissions(tinyxml2::XMLElement* missionsEle) {
    tinyxml2::XMLElement* firstMission = missionsEle->FirstChildElement("mission");
    tinyxml2::XMLElement* actual = firstMission;
    missions npc_missions;
    while (actual) {
        npc_missions.push_back(makeMission(actual));
        actual = actual->NextSiblingElement("mission");
    }
    return npc_missions;
}

void World::initWorld(const char* path2story) {
    story.LoadFile(path2story);
    tinyxml2::XMLElement* worldElement = story.FirstChildElement("world");
    loadRooms(worldElement->FirstChildElement("room"));
    loadWorldMissions(worldElement->FirstChildElement("missions"));
}