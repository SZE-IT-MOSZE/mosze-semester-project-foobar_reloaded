/**
 * @file interface.hpp
 * @author Peter Bence (ecneb2000@gmail.com)
 * @brief Implementation of interface that connects the game with game eingine.
 * @version 0.1
 * @date 2022-10-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef INTERFACE
#define INTERFACE

typedef std::pair<items&, const npcs&> search_results;

/**
 * @brief Abstract class representing a player action.
 * 
 */
class Action {
    std::string description;
protected:
    World& game_world;
public:
    /**
     * @brief Construct a new Action object
     * 
     * @param desc (std::string): Description of the action.
     * @param gm (World&): The game world, so the action class can access the ongoing games attributes.
     * 
     */
    Action(const std::string&, World&);
    /**
     * @brief Take action with the player character.
     * 
     */
    void doAction();
    /**
     * @brief Get the Description object
     * 
     * @return std::string 
     */
    std::string getDescription();
    /**
     * @brief The object created from the Action class, can be called like a function.
     * 
     */
    void operator ()();
    virtual ~Action() = default;
};

/**
 * @brief Realisation of the action class, representing movement from a room to another.
 * 
 */
class Move : public Action {
public:
    /**
     * @brief Construct a new Action object
     * 
     * @param desc (std::string): Description of the action.
     * @param gm (World&): The game world, so the action class can access the ongoing games attributes.
     * 
     */
    Move(const std::string&, World&);
    /**
     * @brief Take action with the player character and move to another room.
     * 
     * @param r (node&): The room that the player will be moved to.
     * 
     */
    void doAction(int); 
    /**
     * @brief The Move object is a callable object, and will call the doAction method of the object. 
     * 
     * @param r (node&): The room that the player will be moved to.
     * 
     */
    void operator ()(int);
};

/**
 * @brief Realisation of the action class, respresenting the search of a room.
 * 
 */
class Search : public Action {
public:
    /**
     * @brief Construct a new Action object
     * 
     * @param desc (std::string): Description of the action.
     * @param gm (World&): The game world, so the action class can access the ongoing games attributes.
     * 
     */
    Search(const std::string&, World&);
    /**
     * @brief Take action with player character and search through the room, that the player is in.
     * 
     * @return search_results 
     */
    search_results doAction();
    /**
     * @brief The Search object is a callable object, and will call the doAction method of the object. 
     * 
     * @return search_results 
     */
    search_results operator ()();
};

/**
 * @brief Realisation of the action class, the player picking up an item.
 * 
 */
class PickUp : public Action {
public:
    /**
     * @brief Construct a new Action object
     * 
     * @param desc (std::string): Description of the action.
     * @param gm (World&): The game world, so the action class can access the ongoing games attributes.
     * 
     */
    PickUp(const std::string&, World&);
    /**
     * @brief Take action with the player character and pick up the desired item and place it into the player's inventory.
     * 
     * @param i (item&): The smart pointer's reference of the item that will be picked up and placed into the player's inventory. 
     * 
     */
    void doAction(item&) ;
    /**
     * @brief The PickUp object is a callable object, and will call the doAction method of the object. 
     * 
     * @param i (item&): The smart pointer's reference of the item that will be picked up and placed into the player's inventory. 
     * 
     */
    void operator ()(item&) ;
};

/**
 * @brief Realisation of the action class, interaction with an NPC.
 * 
 */
class Interact : public Action {
public:
    /**
     * @brief Construct a new Action object
     * 
     * @param desc (std::string): Description of the action.
     * @param gm (World&): The game world, so the action class can access the ongoing games attributes.
     * 
     */
    Interact(const std::string&, World&);
    /**
     * @brief Interact with the npc.
     * 
     * @param npc (npc&): The smart pointer of the npc that will be talked to.
     * @return std::string 
     */
    std::string doAction(npc&) ;
    /**
     * @brief Accept NPC's mission.
     * 
     * @param npc (npc&): The smart pointer of the npc that will be talked to.
     * 
     */
    void acceptMission(npc&); 
    /**
     * @brief The Interact object is a callable object, and will call the doAction method of the object. 
     * 
     * @param npc (npc&): The smart pointer of the npc that will be talked to.
     * 
     */
    void operator ()(npc&) ;
};

class AcceptMission : public Action {
public:
    /**
     * @brief Construct a new Accept Mission object
     * 
     * @param desc (std::string): Description of the action.
     * @param gm (World&): The game world, so the action class can access the ongoing games attributes.
     * 
     */
    AcceptMission(const std::string&, World&);
    /**
     * @brief Accept mission given in args.
     * 
     * @param m (Mission&): Mission to accept.
     * 
     */
    void doAction(Mission&) ;
    /**
     * @brief The AcceptMission object is a callable object, and will call the doAction method of the object. 
     * 
     * @param m (Mission&): Mission to accept.
     * 
     */
    void operator ()(Mission&) ;
};

class OpenInventory : public Action {
public:
    /**
     * @brief Construct a new Open Inventory object
     * 
     */
    OpenInventory(const std::string&, World&);
    /**
     * @brief Interact with the players inventory. Returns the inventory of the player.
     * 
     * @return items& 
     */
    items& doAction();
    /**
     * @brief The OpenInventory object is a callable object, and will call the doAction method of the object.
     * 
     * @return items& 
     */
    items& operator ()();
};

class SessionManager {
//TODO Implement session manager class that will manage actions and handle user input.
    World game_world;
public:
    /**
     * @brief Construct a new Session Manager object
     * 
     * @param path2story (std::string)
     */
    SessionManager(const std::string&);
    /**
     * @brief Start game session. 
     * 
     */
    void startSession();
    /**
     * @brief Return possible actions, that can be performed in the current state of the game world and the player. 
     * 
     */
    std::vector<Action> possibleActions();
    /**
     * @brief Player interaction with Movement action. 
     * 
     */
    void doMove();
    /**
     * @brief Player interaction with Search action.
     * 
     */
    void doSearch();
    /**
     * @brief Method to handle interaction with items.
     * 
     */
    void doOpenInventory(items&);
    /**
     * @brief Method to handle interaction with NPCs.
     * 
     */
    void doInteract();
    /**
     * @brief Run an iteration of the game loop. This method handles the user input.
     * 
     */
    void doIteration();
};

#endif