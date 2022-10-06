#include <gtest/gtest.h>
#include "engine.hpp"

class RoomTest : public ::testing::Test {
    protected:
        items testItems;
        nodes testRooms;
        void SetUp() override {
            for (int i = 0; i < 10; i++) {
                testRooms.emplace_back(new Room("Room", i, "This is a test room."));
                testItems.emplace_back(new Object("Item No. " + std::to_string(i), i, "This is a test item."));
            }
        }
        void TearDown() override {
            for (int i = 0; i < 10; i++) {
                testRooms[i].reset();
            }
        }
};

TEST_F(RoomTest, constructor_test) {
    node Room1(new Room("Room1", 1, "This room is created to test the constructor."));
    EXPECT_EQ("Room1", Room1->getName());
    EXPECT_EQ(1, Room1->getID());
    EXPECT_EQ("This room is created to test the constructor.", Room1->getDescription());
}

/* Uncomment this if you want the test to fail. :D 
TEST(FailTest, tofail) {
    EXPECT_EQ(1, 2);
}
*/

TEST_F(RoomTest, test_Item_operation_methods) {
    // testItems should have 10 items now
    ASSERT_EQ(10, testItems.size());
    testRooms[0]->addItem(testItems[0]).addItem(testItems[1]);
    // 2 items from testItems were added to testRoom[0]'s inventory, so now testItems should have 8 items
    ASSERT_EQ(2, testRooms[0]->getItems().size());
    EXPECT_EQ(nullptr, testItems[0].get()); // This unique_ptr should contain a nullptr, because addItem moved it to the Room's inventory
    EXPECT_EQ(nullptr, testItems[1].get()); // Same as above
    EXPECT_EQ("Item No. 0", testRooms[0]->getItems()[0]->getName());
    EXPECT_EQ("Item No. 1", testRooms[0]->getItems()[1]->getName());
}

TEST_F(RoomTest, test_Neighbour_operation_methods) {
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(1, testRooms[i].use_count());
    }
    for (int i = 1; i < 10; i++) {
        testRooms[0]->addNeighbour(testRooms[i]);
        EXPECT_EQ(2, testRooms[i].use_count());
    }
    EXPECT_EQ(9, testRooms[0]->getNeighbours().size());
}