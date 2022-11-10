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
    session_object->doMove();
    EXPECT_EQ(1, session_object->getWorld()->getPlayer().getLocation()->getID()) << "The player should stay in the room, that he was in, because he had no key for room id 2.";
    session_object->doMove();
    EXPECT_EQ(3, session_object->getWorld()->getPlayer().getLocation()->getID()) << "To the test to be successful input 2 to advance to room id 3.";
}

TEST_F(SessionManagerTest, test_Search_Back) {
    EXPECT_EQ(false, session_object->doSearch()) << "Press 0 to go back in selection.";
}

TEST_F(SessionManagerTest, test_Search_Items_Inspect_Back) {
    EXPECT_EQ(false, session_object->doSearch()) << "Press 1 then inspect an item, then go back with 0.";
}

TEST_F(SessionManagerTest, test_Search_Items_Inspect_PickUp) {
    EXPECT_EQ(true, session_object->doSearch()) << "Press 1 then inspect an item, then go back with 0.";
    EXPECT_EQ(1, session_object->getWorld()->getPlayer().getInventory()[0]->getID());
}

//TODO test interaction with npc's