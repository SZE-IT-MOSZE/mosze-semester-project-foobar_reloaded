#ifndef ENGINE
#define ENGINE
/* Game will be built like a linked list and a graph(tree). */
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <iostream>
#include "tinyxml2.h"

class World;
class Object;
class Room;
class Entity;
class Mission;
class Choice;

/**
 * @typedef Room wrapped in a shared_ptr to be able to connect it to other nodes.
 * 
 */
typedef std::unique_ptr<Room> node;
/**
 * @typedef Object wrapped in a unique_ptr, because it can be only owned by one Entity or Room.
 * 
 */
typedef std::unique_ptr<Object> item;
/**
 * @typedef Entity wrapped in shared_ptr, so World can keep track.
 * 
 */
typedef std::unique_ptr<Entity> ent;
/**
 * @typedef Vector of nodes.
 * 
 */
typedef std::vector<node> nodes;
/**
 * @typedef Vector of room id's. 
 * 
 */
typedef std::vector<int> neighbours;
/**
 * @typedef Vector of items.
 * 
 */
typedef std::vector<item> items;
/**
 * @typedef Vector of entities.
 * 
 */
typedef std::vector<ent> entities;
/**
 * @typedef Connection of two rooms represented with the ID's of the rooms.
 * 
 */
typedef std::pair<int, std::vector<int>> roomConnection;

enum missionStatus{finished, in_progress};

/**
 * @brief Base class for any object that can be owned by an Entity or Room.
 * 
 */
class Object {
	const std::string objectName;
	const std::string objectDescriptor;
	const int objID;
	
public:
	/**
	 * @brief Construct a new Object object
	 * 
	 * @param n (std::string&) Name of the Object
	 * @param d (std::string&): Description of the object
	 * @param id (int): Creation ID of the item
	 */
	Object(const std::string& n, const int id, const std::string& d) : objectName(n), objectDescriptor(d), objID(id)  {}
	/**
	 * @brief Get the Name object
	 * 
	 * @return objectName (std::string) 
	 */
	std::string getName() const {return objectName;}
	/**
	 * @brief Get the ID of the object
	 * 
	 * @return objID (int) 
	 */
	int const getID() {return objID;}
	/**
	 * @brief Get the Description of the object
	 * 
	 * @return objectDescription (std::string) 
	 */
	std::string getDescription() const {return objectDescriptor;}
	virtual ~Object() {}
};

/**
 * @brief An object that can open a room.
 * 
 */
class Key : public Object {
	const int keyID;
public:
	Key(const int kid, const std::string& n, const int id, const std::string& d) : Object(n, id, d), keyID(kid) {}
};

/**
 * @brief Base class for a NPC, USER or any other Entity living in the game world.
 * 
 */
class Entity {
	const std::string name;
	items inventory;
	int hp;
	int stamina;
	int intelligence;
	int agility;
	int strenght;
	int stealth;
	int carisma;
public:
	/**
	 * @brief Construct a new Entity object
	 * 
	 * @param n (std::string)
	 */
	Entity(const std::string& n) : name(n) {}
	/**
	 * @brief Get the Name of the Entity
	 * 
	 * @return std::string 
	 */
	std::string getName() const {return name;}
	/**
	 * @brief Add Item to Entity's inventory
	 * 
	 * @param i (item&): Item to move to Entity's inventory
	 * @return Entity&
	 */
	Entity& addItem(item &i) {
		inventory.push_back(std::move(i));
		return *this;
	}
	/**
	 * @brief Add multiple items from an item vector to the entity's inventory.
	 * 
	 * @param i 
	 * @return Entity& 
	 */
	Entity& addItems(items &i) {
		for (auto it = i.begin(); it != i.end(); it++) {
			this->addItem(*it);
		}
		return *this;
	}
	/**
	 * @brief Get the Inventory object
	 * 
	 * @return items& 
	 */
	items& getInventory() {
		return inventory;
	}
	virtual ~Entity() {
		for (items::iterator it = inventory.begin(); it != inventory.end(); it++) {
			it->reset();
		}
	}
};

/**
 * @brief  Enum class to make tracking room lockstatus easier.
 * 
 */
enum LockStatus{locked, unlocked};

/**
 * @brief Part of the World. A room that contains items, that can be collected, players or npc can move in and out of these rooms.
 * 
 * 
 */
class Room {
	const std::string roomName; // Name of the room.
	const int roomID; // ID of the room, that connects a key to this room.
	const std::string description; // Description of the room.
	LockStatus lock;
	entities roomPopulation;
	neighbours connectedRooms; // Neighbouring rooms.
	items inventory; // Items, that can be found in the room.
public:
	/**
	 * @brief Construct a new Room object
	 * 
	 * @param n (std::string&): Name of the Room that cant be changed later. 
	 * @param id (int): This have to be a unique ID to be able to connect keys with rooms. 
	 * @param desc (std::string&): Description of the room. Cant be changed later.
	 */
	Room(const std::string& n, int id, const std::string& desc, neighbours cn) : roomName(n), roomID(id), description(desc), lock(locked), connectedRooms(cn) {}
	/**
	 * @brief Get the Name of the Room
	 * 
	 * @return roomName (std::string) 
	 */
	std::string getName() const {return roomName;}
	/**
	 * @brief Get the roomID of the Room 
	 * 
	 * @return roomID (int) 
	 */
	int getID() const {return roomID;}
	/**
	 * @brief Get the Description of the Room
	 * 
	 * @return description (std::string) 
	 */
	std::string getDescription() const {return description;}
	/**
	 * @brief Get the Neighbours object
	 * 
	 * @return neighbours (const nodes) 
	 */
	neighbours getNeighbours() const {return connectedRooms;}	
	/**
	 * @brief Add a neighbour to the Neighours object
	 * 
	 * @param nn (node&) A new Room, that will be added to the Neighbours object.
	 * @return Room&
	 */
	Room& addNeighbour(int rid) {
		connectedRooms.push_back(rid);
		return *this;
	}
	/**
	 * @brief Add multiple neighbours to the Neighbours object 
	 * 
	 * @param ns (nodes&) Vector of Neighbours object 
	 * @return Room& 
	 */
	Room& addNeighbours(neighbours rids) {
		for (auto rid : rids) {
			this->addNeighbour(rid);
		}
		return *this;
	}
	/**
	 * @brief Get the Items object
	 * 
	 * @return items const& 
	 */
	const items& getItems() {return inventory;}	
	/**
	 * @brief Add new item to the Inventory of the Room 
	 * 
	 * @param i (item&) new Item
	 * @return Room& 
	 */
	Room& addItem(item& i) {
		inventory.emplace_back(std::move(i));
		return *this;
	}
	/**
	 * @brief Add new Items to the Inventory of the Room 
	 * 
	 * @param inv (items&) Vector of Items to be added to the Inventory of the Room
	 * @return Room& 
	 */
	Room& addItems(items& inv) {
		for (items::iterator it = inv.begin(); it != inv.end(); it++) {
			this->addItem(*it);
		}
		return *this;
	}
	/**
	 * @brief Add entity to the population of the room.
	 * 
	 * @param e 
	 * @return Room& 
	 */
	Room& addEntity(ent& e) {
		roomPopulation.emplace_back(std::move(e));
		return *this;
	}
	/**
	 * @brief Add a bunch of entities to the population of the room.
	 * 
	 * @param ents 
	 * @return Room& 
	 */
	Room& addEntities(entities& ents) {
		for (entities::iterator it = ents.begin(); it != ents.end(); it++) {
			this->addEntity(*it);
		}
		return *this;
	}
	/**
	 * @brief Set the Lock object
	 * 
	 * @param stat 
	 * @return Room& 
	 */
	Room& setLock(LockStatus stat) {
		lock = stat;
		return *this;
	}
	/**
	 * @brief With the right key the room given in the argument can be unlocked.
	 * 
	 * @param k key to unlock the room
	 * @param r room to be unlocked
	 * @return true 
	 * @return false 
	 */
	bool static unlock(item& k, node& r) {
		Key* k2r = dynamic_cast<Key*>(k.release());
		if (k2r != nullptr && k->getID() == r->getID()) {
			r->setLock(unlocked);
			delete k2r;
			return true;
		}
		Object* o = dynamic_cast<Object*>(k2r);
		k = std::make_unique<Object>(*o);
		return false;
	}
	const entities& getPopulation() {return roomPopulation;}
	/**
	 * @brief Destroy the Room object. Reset all nodes in the neighours vector and all items in inventory.
	 *
	 * 
	 */
	~Room() {
		for (items::iterator iit = inventory.begin(); iit != inventory.end(); iit++) {
			iit->reset();
		}
		for (entities::iterator eit = roomPopulation.begin(); eit != roomPopulation.end(); eit++) {
			eit->reset();
		}
	}
};

/**
 * @brief class Mission helps to create a mission system, that will give objectives, to accomplish, for the player.
 * This will give the player a direction, how to finish the story.
 * 
 */
class Mission {
	std::string description;
	int targetRoom;
	int targetItem;
	missionStatus status;
public:
	Mission(node& tr) : targetRoom(tr->getID()), status(in_progress) {}
	Mission(item& ti) : targetItem(ti->getID()), status(in_progress) {}
	Mission(node& tr,item& ti) : targetRoom(tr->getID()), targetItem(ti->getID()), status(in_progress) {}
	virtual void complete() {}	
};

/**
 * @brief The world that contains and manages all the rooms, entities. 
 * A World object will able to parse the story file and initialize the game and run it.
 * 
 */
class World {
	std::string title;
	nodes worldRooms;
	tinyxml2::XMLDocument story;
	std::map<int, std::vector<int>> roomConnectionMap;
public:
	/**
	 * @brief Construct a new World object
	 * 
	 */
	World() {}
	/**
	 * @brief Construct a new World object
	 * 
	 * @param path2story (const char*) The path to the story xml file.
	 */
	World(const char* path2story) {
		story.LoadFile(path2story);
	}
	/**
	 * @brief Get the World Rooms object
	 * 
	 * @return nodes 
	 */
	nodes& getWorldRooms() {
		return worldRooms;
	}
	/**
	 * @brief Create Room with initial params and add it to worldRooms.
	 * 
	 * @param title (const std::string&): Name of the Room
	 * @param id (const std::string&): ID of the Room
	 * @param desc (const std::string&): Description of the Room
	 */
	void RoomFactory(const std::string& name, int id, const std::string& desc, neighbours nids) {
		auto newRoom = std::make_unique<Room>(name, id, desc, nids);
		worldRooms.emplace_back(std::move(newRoom));
	}
	/**
	 * @brief Get the Story object
	 * 
	 * @return tinyxml2::XMLDocument& 
	 */
	tinyxml2::XMLDocument& getStory() {return story;}
	/**
	 * @brief create inventory
	 * 
	 * @param invEle (tinyxml2::XMLElement*)
	 * @return items 
	 */
	items makeInventory(tinyxml2::XMLElement* invEle) {
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
	/**
	 * @brief Load neigbouring rooms id's to connection map
	 * 
	 * @param conns 
	 * @return std::vector<int> 
	 */
	neighbours parseConnections(tinyxml2::XMLElement* conns) {
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
	/**
	 * @brief This function iterates through the room elements of the world element in the xml file and constructs the rooms of the world.
	 * 
	 * @param firstRoom 
	 */
	void loadRooms(tinyxml2::XMLElement* firstRoom) {
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
	/**
	 * @brief Load entities from xml doc.
	 * 
	 * @param firstEle 
	 */
	int loadEntities(tinyxml2::XMLElement* firstEle, node& r) {
		tinyxml2::XMLElement* actual = firstEle;
		int counter = 0;
		while(actual) {
			const std::string name = actual->FirstChildElement("name")->GetText();
			tinyxml2::XMLElement* invele = actual->FirstChildElement("inventory");
			items inv = makeInventory(invele);
			ent newEnt = std::make_unique<Entity>(name);
			newEnt->addItems(inv);
			r->addEntity(newEnt);
			actual = actual->NextSiblingElement("entity");
			counter++;
		}
		return counter;
	}
	/**
	 * @brief Initialize world with the xml story file.
	 * 
	 * @param path2story 
	 */
	void initWorld(const char* path2story) {
		story.LoadFile(path2story);
		tinyxml2::XMLElement* worldElement = story.FirstChildElement("world");
		loadRooms(worldElement->FirstChildElement("room"));
	}
	/**
	 * @brief Free resources used by world.
	 * 
	 */
	void destroyWorld() {
		for (nodes::iterator it = worldRooms.begin(); it != worldRooms.end(); it++) {
			it->reset();
		}
	}
	~World() {
		this->destroyWorld();
	}
};
#endif