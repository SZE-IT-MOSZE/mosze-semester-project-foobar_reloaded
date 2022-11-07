/**
 * @file test_room.cpp
 * @author Peter Bence (ecneb2000@gmail.com)
 * @brief Testing software of the class Room.
 * @version 0.1
 * @date 2022-10-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <gtest/gtest.h>
#include "engine.cpp"

/**
 * @brief This cpp file is intended to test the methods of class Room 
 * 
 */

class RoomTest : public ::testing::Test {
    protected:
        items testItems;
        nodes testRooms;
        items testKeys;
        npcs testEntities;
        void SetUp() override {
            for (int i = 0; i < 10; i++) {
                testRooms.emplace_back(new Room("Room", i, "This is a test room.", "Room", unlocked));
                testItems.emplace_back(new Object("Item No. " + std::to_string(i), i, "This is a test item."));
                testKeys.emplace_back(new Key(i, "TestKey No." + std::to_string(i), i, "This is a testKey."));
                testEntities.emplace_back(new NPC("TestEntity No." + std::to_string(i), "TestDialog", missions{}));
            }
        }
        void TearDown() override {
            for (int i = 0; i < 10; i++) {
                testRooms[i].reset();
            }
        }
};

TEST_F(RoomTest, constructor_test) {
    node Room1(new Room("Room1", 1, "This room is created to test the constructor.", "Room1", unlocked));
    EXPECT_EQ("Room1", Room1->getName());
    EXPECT_EQ(1, Room1->getID());
    EXPECT_EQ("This room is created to test the constructor.", Room1->getDescription());
}

TEST_F(RoomTest, test_Item_operation_methods) {
    // testItems should have 10 items now
    ASSERT_EQ(10, testItems.size());
    testRooms[0]->addItem(testItems[0]).addItem(testItems[1]);
    // 2 items from testItems were added to testRoom[0]'s inventory, so now testItems should have 8 items
    ASSERT_EQ(2, testRooms[0]->getItems().size());
    EXPECT_EQ(nullptr, testItems[0].get()); // This unique_ptr should contain a nullptr, because addItem moved it to the Room's inventory
    EXPECT_EQ(nullptr, testItems[1].get()); // Same as above
    EXPECT_STREQ(std::string("Item No. 0").c_str(), testRooms[0]->getItems()[0]->getName().c_str());
    EXPECT_STREQ(std::string("Item No. 1").c_str(), testRooms[0]->getItems()[1]->getName().c_str());
}

TEST_F(RoomTest, test_lock_feature) {
    for (node &r : testRooms) {
        EXPECT_EQ(true, r->isLocked());
    }
    for ( int i = 0 ; i < int(testRooms.size()) ; i++ ) {
        Room::unlock(&testKeys[i], testRooms[i].get());
    }
    for (node &r : testRooms) {
        EXPECT_EQ(false, r->isLocked()) << "After using the static unlock() method of the Room class, then each room should be opened";
    }
    for (item &k : testKeys) {
        EXPECT_EQ(nullptr, k.get()) << "After unlocking a room with a key, the key should be destroyed and the unique_ptr should point to a nullptr";
    }
}

TEST_F(RoomTest, test_entity_operation_methods) {
    ASSERT_EQ(10, testEntities.size()) << "testEntities should have 10 entities";
    for ( int i = 0 ; i < int(testRooms.size()) ; i++ ) {
        // Add each entity to each room 
        testRooms[i]->addEntity(testEntities[i]);
    }
    for ( int i = 0 ; i < int(testRooms.size()) ; i++ ) {
        ASSERT_EQ(nullptr, testEntities[i].get()) << "If entities moved right from testEntities to the room's population, then each unique_ptr in testEntities should be a nullptr";
        EXPECT_STREQ(std::string("TestEntity No." + std::to_string(i)).c_str(), testRooms[i]->getPopulation()[0]->getName().c_str()) << "The name of the entities should match the moved entities names";
    } 
}