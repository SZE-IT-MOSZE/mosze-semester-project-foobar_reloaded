#include "engine.hpp"

/* class Entity implementation */
/**
 * @brief Add Item to Entity's inventory
 * 
 * @param i (item&): Item to move to Entity's inventory
 * @return Entity&
 */
Entity& Entity::addItem(Object* i) {
    inventory.emplace_back(i);
    return *this;
}

~Entity() {
    for (items::iterator it = inventory.begin(); it != inventory.end(); it++) {
        it->reset();
    }
}

/* class Room implementation */
/**
 * @brief Add a neighbour to the Neighours object
 * 
 * @param nn (node&) A new Room, that will be added to the Neighbours object.
 * @return Room&
 */
Room& Room::addNeighbour(node& nn) {
    neighbours.push_back(node(nn));
    return *this;
}
/**
 * @brief Add multiple neighbours to the Neighbours object 
 * 
 * @param ns (nodes&) Vector of Neighbours object 
 * @return Room& 
 */
Room& Room::addNeighbours(nodes& ns) {
    for (nodes::const_iterator it = ns.cbegin(); it != ns.cend(); it++) {
        neighbours.push_back(node(*it));
    }
    return *this;
}
/**
 * @brief Add new item to the Inventory of the Room 
 * 
 * @param i (item&) new Item
 * @return Room& 
 */
Room& Room::addItem(item& i) {
    inventory.emplace_back(std::move(i));
    return *this;
}
/**
 * @brief Add new Items to the Inventory of the Room 
 * 
 * @param inv (items&) Vector of Items to be added to the Inventory of the Room
 * @return Room& 
 */
Room& Room::addItems(items& inv) {
    for (items::iterator it = inv.begin(); it != inv.end(); it++) {
        this->addItem(*it);
    }
    return *this;
}
/**
 * @brief Destroy the Room object. Reset all nodes in the neighours vector and all items in inventory.
 *
 * 
 */
~Room() {
    for (nodes::iterator nit = neighbours.begin(); nit != neighbours.end(); nit++) {
        nit->reset();
    }
    for (items::iterator iit = inventory.begin(); iit != inventory.end(); iit++) {
        iit->reset();
    }
}

/* class World implementation */
void World::RoomFactory(const std::string& name, int id, const std::string& desc) {
    Room* newRoom = new Room(name, id, desc);
    worldRooms.emplace_back(std::move(newRoom));
}
/**
 * @brief Create entity and add it to the population.
 * 
 * @param name 
 */
void World::EntityFactory(const std::string& name) {
    population.emplace_back(new Entity(name));
}
/**
 * @brief create inventory
 * 
 * @param invEle (tinyxml2::XMLElement*)
 * @return items 
 */
items World::makeInventory(tinyxml2::XMLElement* invEle) {
    tinyxml2::XMLElement* firstObj = invEle->FirstChildElement("object");
    tinyxml2::XMLElement* actual = firstObj;
    items retItems;
    while(actual) {
        const std::string objName(actual->FirstChildElement("name")->GetText());
        const std::string desc(actual->FirstChildElement("description")->GetText());
        int id = 0;
        actual->FirstChildElement("id")->QueryIntText(&id);
        std::cout << id << std::endl;
        retItems.emplace_back(new Object(objName, id, desc));
        actual = actual->NextSiblingElement("object");
    }
    return retItems;
}
/**
 * @brief Load neigbouring rooms id's to connection map
 * 
 * @param conns 
 * @return std::vector<int> 
 */
std::vector<int> World::trackConnections(tinyxml2::XMLElement* conns) {
    tinyxml2::XMLElement* firstConn = conns->FirstChildElement("id");
    tinyxml2::XMLElement* actual = firstConn;
    std::vector<int> neighbours;
    while(actual) {
        int nid = 0;
        actual->QueryIntText(&nid);
        neighbours.push_back(nid); 
        actual = actual->NextSiblingElement("id");
    }
    return neighbours;
}
/**
 * @brief This function iterates through the room elements of the world element in the xml file and constructs the rooms of the world.
 * 
 * @param firstRoom 
 */
void World::loadRooms(tinyxml2::XMLElement* firstRoom) {
    const char* elementName = "room";
    tinyxml2::XMLElement* actual = firstRoom;
    while(actual) {
        const std::string roomName(actual->FirstChildElement("name")->GetText());
        const std::string roomDescription(actual->FirstChildElement("description")->GetText());
        int roomID = 0;
        actual->FirstChildElement("id")->QueryIntText(&roomID);
        RoomFactory(roomName, roomID, roomDescription); // Construct room with name, id and desc
        tinyxml2::XMLElement* invEle = actual->FirstChildElement("inventory");
        tinyxml2::XMLElement* actualInv = invEle->FirstChildElement("object");
        while(actualInv) {
            const std::string objName(actualInv->FirstChildElement("name")->GetText());
            const std::string desc(actualInv->FirstChildElement("description")->GetText());
            int objectID = 0;
            actualInv->FirstChildElement("id")->QueryIntText(&objectID);
            item tmpItem = item(new Object(objName, objectID, desc));
            worldRooms.back()->addItem(tmpItem);
            actualInv = actualInv->NextSiblingElement("object");
        }
        tinyxml2::XMLElement* conns = actual->FirstChildElement("connections");
        roomConnectionMap.insert(std::make_pair(roomID, trackConnections(conns)));
        actual = actual->NextSiblingElement(elementName);
    }
}
/**
 * @brief This function is called only after loadRooms() function to create connection between rooms based on the connection map.
 * 
 */
void World::connectRooms() {
    for (std::map<int, std::vector<int>>::iterator it = roomConnectionMap.begin(); it != roomConnectionMap.end(); it++) {
        int parentID = it->first;	
        nodes::iterator parentRoom = std::find_if(worldRooms.begin(), worldRooms.end(), 
                [&parentID](node& r) {if (r->getID() == parentID) return true; return false;});
        if (parentRoom != worldRooms.end()) {
            for (std::vector<int>::iterator sit = it->second.begin(); sit != it->second.end(); sit++) {
                int id2find = *sit;
                nodes::iterator childRoom = std::find_if(worldRooms.begin(), worldRooms.end(), 
                    [&id2find](node& r) {if (r->getID() == id2find) return true; return false;});
                if (childRoom != worldRooms.end()) {
                    parentRoom->get()->addNeighbour(*childRoom);
                }
            }	
        }
    }
}
void World::initWorld(const char* path2story) {
    story.LoadFile(path2story);
    tinyxml2::XMLElement* worldElement = story.FirstChildElement("world");
    loadRooms(worldElement->FirstChildElement("room"));
    //connectRooms();
}
void World::destroyWorld() {
    for (nodes::iterator it = worldRooms.begin(); it != worldRooms.end(); it++) {
        it->reset();
        std::cout << it->use_count() << std::endl;
    }
    for (entities::iterator it = population.begin(); it != population.end(); it++) {
        it->reset();
    }
}