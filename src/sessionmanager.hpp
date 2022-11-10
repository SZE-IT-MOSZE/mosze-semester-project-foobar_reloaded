/**
 * @file sessionmanager.hpp
 * @author Peter Bence (ecneb2000@gmail.com)
 * @brief This is the declaration of the SessionManager module. The session manager handles the user input.
 * @version 0.1
 * @date 2022-11-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef SESSIONMANAGER
#define SESSIONMANAGER

/**
 * @brief Promp user choice. Max choice number can be given with arg.
 * 
 * @param max (int): Max number of choices.
 * @return int 
 */
int promptChoice(int);

class SessionManager {
//TODO Implement session manager class that will manage actions and handle user input.
    std::string path2story;
    World game_world;
public:
    /**
     * @brief Construct a new Session Manager object
     * 
     * @param path2story (std::string)
     */
    SessionManager(const std::string&);
    World* getWorld() {return &game_world;}
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
     * @return true
     * @return false
     */
    bool doMove();
    /**
     * @brief Player interaction with Search action.
     * 
     * @return true
     * @return false  
     */
    bool doSearch();
    /**
     * @brief Method to handle interaction with objects found in room.
     * 
     * @return true
     * @return false 
     */
    bool doOpenInventory(items&);
    /**
     * @brief Method to handle interaction with NPCs.
     * 
     * @return true
     * @return false
     */
    bool doInteract(npcs&);
    /**
     * @brief Pick up selected item.
     * 
     * @return true 
     * @return false 
     */
    bool doPickUp(item&);
    /**
     * @brief Inspect found item. Print out name and description of the item.
     * 
     * @return true 
     * @return false 
     */
    bool doInspect(item&);
    /**
     * @brief Run an iteration of the game loop. This method handles the user input.
     *
     * @return Returns a bool if all missions are finished. 
     */
    bool doIteration();
};
#endif