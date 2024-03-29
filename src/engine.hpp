/**
 * @file engine.hpp
 * @author Peter Bence (ecneb2000@gmail.com)
 * @brief Definition and part implementation of game logic.
 * @version 0.1
 * @date 2022-10-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef ENGINE
#define ENGINE

class World;
class Object;
class Room;
class Entity;
class Player;
class NPC;
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
typedef std::unique_ptr<NPC> npc;
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
typedef std::vector<npc> npcs;
/**
 * @typedef Connection of two rooms represented with the ID's of the rooms.
 * 
 */
typedef std::pair<int, std::vector<int>> roomConnection;
/**
 * @typedef Vector of missions. 
 * 
 */
typedef std::vector<Mission*> missions;

enum missionStatus{finished, active, inactive};

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
	/**
	 * @brief Construct a new Key object
	 * 
	 * @param kid The key id should be identical to the room's it opens
	 * @param n The name of the key
	 * @param id The uniqe item id of the key
	 * @param d A description that describes the keys look, use, etc.
	 */
	Key(const int kid, const std::string& n, const int id, const std::string& d) : Object(n, id, d), keyID(kid) {}
	/**
	 * @brief Get the KeyID of the key 
	 * 
	 * @return int 
	 */
	int getKeyID() {
		return keyID;
	}
	};

/**
 * @brief Base class for a NPC, USER or any other Entity living in the game world.
 * 
 */
class Entity {
protected:
	const std::string name;
	items inventory;
	int hp;
	int stamina;
	int intelligence;
	int strenght;
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

class Player : public Entity {
	Room* location;
public:
	/**
	 * @brief Construct a new Player object with Default values.
	 * 
	 */
	Player() : Entity("Default") {}
	/**
	 * @brief Construct a new Player object, giving it an initial location.
	 * 
	 * @param n 
	 * @param init_location 
	 */
	Player(const std::string& n, Room* init_location) : Entity(n), location(init_location) {}
	/**
	 * @brief Get the Location of the Entity by returning the pointer to the Room. 
	 * 
	 * @return Room* 
	 */
	Room* getLocation() {
		return location;
	}
	/**
	 * @brief Set the Location of the Entity by giving the pointer of the Room. 
	 * 
	 * @param room_ptr 
	 * @return Entity& 
	 */
	Entity& setLocation(Room* room_ptr) {
		this->location = room_ptr;
		return *this;
	}
};

class NPC : public Entity {
	std::string dialog;
	std::string dialog_no_access = "Ezzel az NPCvel most nem tudsz kommunikálni, folytasd a felfedezést, és gyere vissza kicsit később.";
	missions missions_to_give;
public:
	/**
	 * @brief Construct a new NPC object, giving it a dialog and missions that can be accepted by the player.
	 * 
	 * @param n 
	 * @param d 
	 * @param m 
	 */
	NPC(const std::string& n,  const std::string& d, missions m) : Entity(n), dialog(d), missions_to_give(m) {}
	missions& getMissions() {
		return missions_to_give;
	}
	/**
	 * @brief Get the Dialog object
	 * 
	 * @return std::string 
	 */
	std::string getDialog() {return dialog;}
	/**
	 * @brief Get the Dialog No Acess object
	 * 
	 * @return std::string 
	 */
	std::string getDialogNoAcess() {return dialog_no_access;}
	~NPC();
};

/**
 * @brief  Enum class to make tracking room lockstatus easier.
 * 
 */
enum LockStatus{unlocked, locked};

/**
 * @brief Part of the World. A room that contains items, that can be collected, players or npc can move in and out of these rooms.
 * 
 * 
 */
class Room {
	const std::string roomName; // Name of the room.
	const int roomID; // ID of the room, that connects a key to this room.
	const std::string description; // Description of the room.
	const std::string choiceDescription;
	LockStatus lock; 
	npcs roomPopulation;
	neighbours connectedRooms; // Neighbouring rooms.
	items inventory; // Items, that can be found in the room.
public:
	/**
	 * @brief Construct a new Room object
	 * 
	 * @param n (std::string&): Name of the Room that cant be changed later. 
	 * @param id (int): This have to be a unique ID to be able to connect keys with rooms. 
	 * @param desc (std::string&): Description of the room. Cant be changed later.
	 * @param choice (std::string&): Desciption for choice generation.
	 */
	Room(const std::string& n, int id, const std::string& desc, const std::string& choice, LockStatus ls) : roomName(n), roomID(id), description(desc), choiceDescription(choice), lock(ls) {}
	/**
	 * @brief Construct a new Room object
	 * 
	 * @param n (std::string&): Name of the Room that cant be changed later. 
	 * @param id (int): This have to be a unique ID to be able to connect keys with rooms. 
	 * @param desc (std::string&): Description of the room. Cant be changed later.
	 * @param choice (std::string&): Desciption for choice generation.
	 * @param cn Connected neighbours
	 */
	Room(const std::string& n, int id, const std::string& desc, const std::string& choice, LockStatus ls, neighbours cn) : roomName(n), roomID(id), description(desc), choiceDescription(choice), lock(ls), connectedRooms(cn) {}
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
	 * @brief Get the Choice Description of the Room 
	 * 
	 * @return std::string 
	 */
	std::string getChoiceDescription() const {return choiceDescription;}
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
	items& getItems() {return inventory;}	
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
	Room& addEntity(npc& e) {
		roomPopulation.emplace_back(std::move(e));
		return *this;
	}
	/**
	 * @brief Add a bunch of entities to the population of the room.
	 * 
	 * @param ents 
	 * @return Room& 
	 */
	Room& addEntities(npcs& ents) {
		for (npcs::iterator it = ents.begin(); it != ents.end(); it++) {
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
	 * @brief With the right key the room given in the argument can be unlocked. If the key fits the door, after it unlocked the room, it cant be used anymore.
	 * 
	 * @param k key to unlock the room
	 * @param r room to be unlocked
	 * @return true 
	 * @return false 
	 */
	bool static unlock(item*, Room*);
	/**
	 * @brief Check wheter room is locked or not. Returns true if locked, false otherwise.
	 * 
	 * @return true 
	 * @return false 
	 */
	bool isLocked() {
		if ( lock == locked ) {
			return true;
		}
		return false;
	}
	/**
	 * @brief Get the Population object
	 * 
	 * @return const entities& 
	 */
	npcs& getPopulation() {return roomPopulation;}
	/**
	 * @brief Destroy the Room object. Reset all nodes in the neighours vector and all items in inventory.
	 *
	 * 
	 */
	~Room();
};

/**
 * @brief class Mission helps to create a mission system, that will give objectives, to accomplish, for the player.
 * This will give the player a direction, how to finish the story.
 * 
 */
class Mission {
	std::string description;
	missionStatus status;
	int targetRoom;
	int targetItem;
public:
	/**
	 * @brief Construct a new Mission object.
	 * 
	 * @param desc Description of the mission.
	 */
	Mission(const std::string& desc) : description(desc) {}	
	/**
	 * @brief Get the Target Room ID 
	 * 
	 * @return int 
	 */
	int getTargetRoom() {return targetRoom;}
	/**
	 * @brief Set the Target Room ID 
	 * 
	 * @param targetRoomID 
	 * @return Mission& 
	 */
	Mission& setTargetRoom(int targetRoomID) {
		targetRoom = targetRoomID;
		return *this;
	}
	/**
	 * @brief Get the Target Item ID 
	 * 
	 * @return int 
	 */
	int getTargetItem() {return targetItem;}
	/**
	 * @brief Set the Target Item ID 
	 * 
	 * @param targetItemID 
	 * @return Mission& 
	 */
	Mission& setTargetItem(int targetItemID) {
		targetItem = targetItemID;
		return *this;
	}
	/**
	 * @brief Set missionStatus to finished. 
	 * 
	 */
	void complete() {
		status = finished;
	}
	/**
	 * @brief Check if mission is accomplished
	 * 
	 * @param player 
	 * @return true 
	 * @return false 
	 */
	bool checkStatus(Player&);
	/**
	 * @brief Get the Status object
	 * 
	 * @return missionStatus 
	 */
	missionStatus& getStatus() {return status;}	
	/**
	 * @brief Get the Description object
	 * 
	 * @return std::string 
	 */
	std::string getDescription() {return description;}
	/**
	 * @brief Change mission status to active.
	 * 
	 * @return Mission& 
	 */
	Mission& startMission() {
		status = active;
		return *this;
	}
};

/**
 * @brief Check if xml tag is given in the story file. exit(1) if not.
 * 
 * @return tinyxml2::XMLElement* 
 */
tinyxml2::XMLElement* checkXMLElement(tinyxml2::XMLElement*, const std::string&);

/**
 * @brief The world that contains and manages all the rooms, entities. 
 * A World object will able to parse the story file and initialize the game and run it.
 * 
 */
class World {
	std::string title;
	nodes worldRooms;
	tinyxml2::XMLDocument story;
	Player player;
	missions active_missions;	
	/**
	 * @brief Construct a new World object. This is a private constructor implemented for testing reasons. Should not be used in production.
	 * 
	 * @param path2story (const char*) The path to the story xml file.
	 */
	World(const char* path2story);
	/**
	 * @brief Free resources used by world. This is not public, because class World's desctructor will call it.
	 * 
	 */
	void destroyWorld();
public:
	/**
	 * @brief Construct a new World object. Default constructor. To config and setup world object use initWorld() method.
	 * 
	 */
	World() {}
	/**
	 * @brief Get the World Rooms object
	 * 
	 * @return nodes 
	 */
	nodes& getWorldRooms() {
		return worldRooms;
	}
	/**
	 * @brief Get the World Mission object
	 * 
	 * @return missions& 
	 */
	missions& getWorldMission() {
		return active_missions;
	}
	/**
	 * @brief Create Room with initial params and add it to worldRooms.
	 * 
	 * @param title (const std::string&): Name of the Room
	 * @param id (const std::string&): ID of the Room
	 * @param desc (const std::string&): Description of the Room
	 * @param choice (const std::string&): Choice description of the Room
	 * @param ls (LockStatus): Status of the lock of the Room.
	 */
	void RoomFactory(const std::string&, int, const std::string&, const std::string&, LockStatus, neighbours);
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
	items makeInventory(tinyxml2::XMLElement*);
	/**
	 * @brief Load neigbouring rooms id's to connection map
	 * 
	 * @param conns 
	 * @return std::vector<int> 
	 */
	neighbours parseConnections(tinyxml2::XMLElement*);
	/**
	 * @brief This function iterates through the room elements of the world element in the xml file and constructs the rooms of the world.
	 * 
	 * @param firstRoom 
	 */
	void loadRooms(tinyxml2::XMLElement*);
	/**
	 * @brief Load entities from xml doc.
	 * 
	 * @param firstEle 
	 */
	void loadEntities(tinyxml2::XMLElement*, node&);
	/**
	 * @brief Construct a new mission with a description, then set mission targets.
	 * 
	 * @param missionEle 
	 * @return Mission 
	 */
	Mission* makeMission(tinyxml2::XMLElement*);
	/**
	 * @brief Load world story missions from xml doc.
	 * 
	 * @param firstEle first mission element in xml doc.
	 */
	void loadWorldMissions(tinyxml2::XMLElement*);
	/**
	 * @brief Load missions of an NPC. 
	 * 
	 * @param missionsEle 
	 * @return missions 
	 */
	missions loadNPCMissions(tinyxml2::XMLElement*);
	/**
	 * @brief Set the Player Spawn Room. The XML element contains the id of the spawn room. 
	 * 
	 * @param spawnEle
	 * @return true if spawn location is given in XML file, false otherwise
	 */
	bool setPlayerSpawn(tinyxml2::XMLElement*);
	/**
	 * @brief Initialize world with the xml story file.
	 * 
	 * @param path2story 
	 */
	void initWorld(const std::string&);
	/**
	 * @brief Search key for room ID given in args.
	 * 
	 * @param roomID (int): ID of the room which's key should be searched for.
	 * @return Object* 
	 */
	item* searchKey(int);	
	/**
	 * @brief Searches for the room which's id was given in args, if it finds the room, then returns the pointer of that room, nullptr otherwise.
	 * 
	 * @param id (int) Room's id.
	 * @return Room* 
	 */
	Room* lookUpRoom(int);
	/**
	 * @brief Sets player's location to the room, which's id was given in the args. 
	 * 
	 * @param id (int) The room's id.
	 */
	bool enterRoom(int);
	/**
	 * @brief Get the Player object
	 * 
	 * @return Player& 
	 */
	Player& getPlayer() {
		return player;
	}
	/**
	 * @brief Start mission by changing mission's status to in progress and placing it to the active_missions vector. 
	 * 
	 * @param m 
	 */
	void startMission(Mission* m) {
		m->startMission();
		active_missions.push_back(m);
	}
	/**
	 * @brief Get the Active Missions object
	 * 
	 * @return missions& 
	 */
	missions& getActiveMissions() {return active_missions;}
	/**
	 * @brief Iterates through every room, entity inventory in the world and removes item smart pointers that point to nullptr. This is neccessary, because if an item is moved from one place to another, then it leaves a smart pointer pointing to a nullptr.
	 * 
	 */
	void cleanInventories();
	/**
	 * @brief Destroy the World object
	 * 
	 */
	~World();
};
#endif