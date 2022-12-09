/**
 * @file test_interface.cpp
 * @author Peter Bence (ecneb2000@gmail.com)
 * @brief This software is testing the features of the interface implementation.
 * @version 0.1
 * @date 2022-10-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <gtest/gtest.h>
#include "sessionmanager.cpp"

class SessionManagerTest : public ::testing::Test {
protected:
    SessionManager* session_object;
    void SetUp() {
        session_object = new SessionManager("../test_story.xml");
    }
    void TearDown() {
        delete session_object;
    }
};

/**
 * @brief Try to test SessionManager's doMove(), that requires user input, because this will handle the user input that will move the player from one room to another. The doMove() method will prompt for a number, which room to advance to. For a successful test we will await that the user inputs 2, that will move the player to Room ID 2.
 * 
 */
TEST_F(SessionManagerTest, test_doMove) {
    std::cout << "To the test to be successful:\n";
    std::cout << "First tests for movement from one room to another: input number 1 then number 2.\n";
    session_object->doMove();
    EXPECT_EQ(1, session_object->getWorld()->getPlayer().getLocation()->getID()) << "The player should stay in the room, that he was in, because he had no key for room id 2.";
    session_object->doMove();
    EXPECT_EQ(3, session_object->getWorld()->getPlayer().getLocation()->getID()) << "To the test to be successful input 2 to advance to room id 3.";
}

TEST_F(SessionManagerTest, test_Search_Back) {
    std::cout << "Second tests choice search, then go back: input 0.\n";
    EXPECT_EQ(false, session_object->doSearch()) << "Press 0 to go back in selection.";
}

TEST_F(SessionManagerTest, test_Search_Items_Inspect_Back) {
    std::cout << "Third test for Item inspection: input 1. then inspect an item, then input 0.\n";
    EXPECT_EQ(false, session_object->doSearch()) << "Press 1 then inspect an item, then go back with 0.";
}

TEST_F(SessionManagerTest, test_Search_Items_Inspect_PickUp) {
    std::cout << "Fourth tests for picking up an item: input 1 then inspect an item, then choose pick up.\n";
    EXPECT_EQ(true, session_object->doSearch()) << "Press 1 then inspect an item, then pick up the item.";
    EXPECT_EQ(1, session_object->getWorld()->getPlayer().getInventory()[0]->getID());
}

TEST_F(SessionManagerTest, test_Search_Interact_Mission_Fail) {
    std::cout << "Fifth tests for NPC interaction, with failing to interact with the NPC, because of the lack of progress.\n";
    EXPECT_EQ(true, session_object->doSearch()) << "Choose Interact with NPC's, then choose NCP to interact with it.";
    EXPECT_EQ(active, session_object->getWorld()->getWorldRooms()[0]->getPopulation()[0]->getMissions()[0]->getStatus());
}

TEST_F(SessionManagerTest, test_Search_Interact_Mission_Success) {
    std::cout << "Sixth tests for NPC interaction, this time it is a success.\n";
    session_object->getWorld()->getPlayer().addItem(session_object->getWorld()->getWorldRooms()[0]->getItems()[0]);
    EXPECT_EQ(true, session_object->doSearch()) << "Choose Interact with NPC's, then choose NCP to interact with it.";
    EXPECT_EQ(finished, session_object->getWorld()->getWorldRooms()[0]->getPopulation()[0]->getMissions()[0]->getStatus());
}