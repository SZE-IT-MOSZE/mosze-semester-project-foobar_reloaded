#ifndef INTERFACE
#define INTERFACE
#include "engine.hpp"

/**
 * @brief Abstract class representing a player action.
 * 
 */
class Action {

};

/**
 * @brief Realisation of the action class, representing movement from a room to another.
 * 
 */
class Move : public Action {

};

/**
 * @brief Realisation of the action class, respresenting the search of a room.
 * 
 */
class Search : public Action {

};

/**
 * @brief Realisation of the action class, the player picking up an item.
 * 
 */
class PickUp : public Action {

};

/**
 * @brief Realisation of the action class, interaction with an NPC.
 * 
 */
class Interact : public Action {

};

#endif