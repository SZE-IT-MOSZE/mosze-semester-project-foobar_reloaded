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
#include "interface.cpp"

class InterfaceTest : public ::testing::Test {
protected:
    World testWorld;
    void SetUp() override {
        testWorld.initWorld("../test_story.xml");
    }
    void TearDown() override {
    }
};

TEST_F(InterfaceTest, test_Move_LockSystem) {
    Action* test_action = new Move("Test move action.", testWorld);
    int spawnID = testWorld.getPlayer().getLocation()->getID();
    EXPECT_EQ(1, spawnID);
    Move* test_move = dynamic_cast<Move*>(test_action);
    ASSERT_EQ(typeid(Move), typeid(*test_action));
    if (test_move) {
        (*test_move)(3);
    }
    EXPECT_EQ(3, testWorld.getPlayer().getLocation()->getID());
    if (test_move) {
        (*test_move)(2);
    }
    EXPECT_EQ(3, testWorld.getPlayer().getLocation()->getID());
    item key_room2 = std::make_unique<Key>(2, "KeyTo2", 5, "This is a");
    testWorld.getPlayer().addItem(key_room2);
    if (test_move) {
        (*test_move)(2);
    }
    EXPECT_EQ(2, testWorld.getPlayer().getLocation()->getID());
    EXPECT_EQ(nullptr, testWorld.getPlayer().getInventory()[0].get());
    delete test_action;
}

TEST_F(InterfaceTest, test_Search) {
    Action* test_action = new Search("Test room search action.", testWorld);
    Search* test_search = dynamic_cast<Search*>(test_action);
    ASSERT_EQ(typeid(Search), typeid(*test_action));
    search_results results = (*test_search)(); 
    ASSERT_EQ(1, results.first.size());
    ASSERT_EQ(1, results.second.size());
    EXPECT_STREQ(std::string("TestObject1").c_str(), results.first[0]->getName().c_str());
    EXPECT_STREQ(std::string("TestEntity1").c_str(), results.second[0]->getName().c_str());
    delete test_action;
}

TEST_F(InterfaceTest, test_PickUp) {
    /* This is the same as above, because to test PickUp search_results are needed. With the ASSERT_EQ, there is no place for error in the Search. */
    Action* test_action = new Search("Test room search action.", testWorld);
    Search* test_search = dynamic_cast<Search*>(test_action);
    ASSERT_EQ(typeid(Search), typeid(*test_action));
    search_results results = (*test_search)();
    ASSERT_EQ(1, results.first.size());
    ASSERT_EQ(1, results.second.size());
    delete test_action;

    test_action = new PickUp("Test pick up action", testWorld);
    PickUp* test_pickup = dynamic_cast<PickUp*>(test_action);
    ASSERT_EQ(typeid(PickUp), typeid(*test_action));
    (*test_pickup)(results.first[0]);
    EXPECT_EQ(1, testWorld.getPlayer().getInventory()[0]->getID());
    EXPECT_STREQ(std::string("TestObject1").c_str(), testWorld.getPlayer().getInventory()[0]->getName().c_str());
    delete test_action;
}

TEST_F(InterfaceTest, test_Interact) {
    /* This is the same as above, because to test Interact there need to be search_results from Search. No worry, if Search is not working, then ASSERT_EQ stops execution anyways. */
    Action* test_action = new Search("Test room search action.", testWorld);
    Search* test_search = dynamic_cast<Search*>(test_action);
    ASSERT_EQ(typeid(Search), typeid(*test_action));
    search_results results = (*test_search)();
    ASSERT_EQ(1, results.first.size());
    ASSERT_EQ(1, results.second.size());
    delete test_action;
    
    /* This tests when the player dont have the target item for the quest. */
    test_action = new Interact("Test interaction with test npc.", testWorld);
    Interact* test_interaction = dynamic_cast<Interact*>(test_action);
    ASSERT_EQ(typeid(Interact), typeid(*test_action));
    EXPECT_STREQ(results.second[0]->getDialogNoAcess().c_str(), (*test_interaction)(results.second[0]).c_str());
    EXPECT_EQ(active, results.second[0]->getMissions()[0]->getStatus());
    delete test_action;
}

TEST_F(InterfaceTest, test_Interact_Success) {
    /* This is the same as above, because to test Interact there need to be search_results from Search. No worry, if Search is not working, then ASSERT_EQ stops execution anyways. */
    Action* test_action = new Search("Test room search action.", testWorld);
    Search* test_search = dynamic_cast<Search*>(test_action);
    ASSERT_EQ(typeid(Search), typeid(*test_action));
    search_results results = (*test_search)();
    ASSERT_EQ(1, results.first.size());
    ASSERT_EQ(1, results.second.size());
    delete test_action;

    /* This tests when the player has the target item for the quest. */
    testWorld.getPlayer().addItem(testWorld.getWorldRooms()[0]->getItems()[0]); // This adds the Object ID 1 to the players inventory.
    test_action = new Interact("Test interaction with test npc.", testWorld);
    Interact* test_interaction = dynamic_cast<Interact*>(test_action);
    ASSERT_EQ(typeid(Interact), typeid(*test_action));
    EXPECT_STREQ(results.second[0]->getDialog().c_str(), (*test_interaction)(results.second[0]).c_str());
    EXPECT_EQ(finished, results.second[0]->getMissions()[0]->getStatus());
    EXPECT_EQ(7, testWorld.getPlayer().getInventory()[0]->getID());
    EXPECT_EQ(1, testWorld.getPlayer().getInventory().size());
    delete test_action;  
}