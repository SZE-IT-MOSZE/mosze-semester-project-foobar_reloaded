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
#include "engine.hpp"
#include "interface.hpp"

class InterfaceTest : public ::testing::Test {
protected:
    World testWorld;
    void SetUp() override {
        testWorld.initWorld("../test_story.xml");
    }
    void TearDown() override {
        testWorld.destroyWorld();
    }
};

TEST_F(InterfaceTest, test_Move_LockSystem) {
    Action* test_action = new Move("Test move action.", testWorld);
    int spawnID = testWorld.getPlayer().getLocation()->getID();
    EXPECT_EQ(1, spawnID);
    Move* test_move = dynamic_cast<Move*>(test_action);
    if (test_move) {
        test_move->doAction(3);
    }
    EXPECT_EQ(3, testWorld.getPlayer().getLocation()->getID());
    if (test_move) {
        test_move->doAction(2);
    }
    EXPECT_EQ(3, testWorld.getPlayer().getLocation()->getID());
    item key_room2 = std::make_unique<Key>(2, "KeyTo2", 5, "This is a");
    testWorld.getPlayer().addItem(key_room2);
    if (test_move) {
        test_move->doAction(2);
    }
    EXPECT_EQ(2, testWorld.getPlayer().getLocation()->getID());
    EXPECT_EQ(nullptr, testWorld.getPlayer().getInventory()[0].get());
    delete test_action;
}