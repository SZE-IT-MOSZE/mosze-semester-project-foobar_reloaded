/**
 * @file test_world.cpp
 * @author Peter Bence (ecneb2000@gmail.com)
 * @brief Testing software of class World, that is the main module of the game engine.
 * @version 0.1
 * @date 2022-10-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <gtest/gtest.h>
#include "engine.cpp"

/**
 * @brief GTest Class initializes world, from test story file.
 * 
 */
class WorldTest : public ::testing::Test {
    protected:
        World testWorld;
        void SetUp() override {
            testWorld.initWorld("../test_story.xml");
        }
        void TearDown() override {
            testWorld.destroyWorld();
        }
};

/**
 * @brief Test if tinyxml2 work correctly.
 * 
 */
TEST_F(WorldTest, test_tinyXML) {
    // load world element of xml doc
    tinyxml2::XMLElement* worldEle = testWorld.getStory().FirstChildElement("world");
    // get title of world  
    const char* title = worldEle->FirstChildElement("title")->GetText();
    std::string expectedTitle("SpaceWalk");
    EXPECT_STREQ(expectedTitle.c_str(), title);
    // get xml element name
    const char* eleTitle = worldEle->Value();
    std::string eleTitleExpected = "world";
    EXPECT_STREQ(eleTitle, eleTitleExpected.c_str());
    // get id of first room
    int firstRoomID = 0;
    worldEle->FirstChildElement("room")->FirstChildElement("id")->QueryIntText(&firstRoomID);
    EXPECT_EQ(firstRoomID, 1);
}

TEST_F(WorldTest, test_initWorld_room_number) {
    ASSERT_EQ(testWorld.getWorldRooms().size(), 3);
}

TEST_F(WorldTest, test_initWorld_items) {
    // Test if items did get loaded to the room.
    for (int i = 0; i < int(testWorld.getWorldRooms().size()); i++) {
        EXPECT_EQ(testWorld.getWorldRooms()[i]->getID(), (i+1));
        for (int j = 0; j < int(testWorld.getWorldRooms()[i]->getItems().size()); j++) {
            EXPECT_EQ(testWorld.getWorldRooms()[i]->getItems()[j]->getID(), (i+1));
        }
    }
}

TEST_F(WorldTest, test_initWorld_neighbours) {
    // Test if neighbours were correctly loaded.
    EXPECT_EQ(testWorld.getWorldRooms()[0]->getNeighbours()[0], 2);
    EXPECT_EQ(testWorld.getWorldRooms()[1]->getNeighbours()[0], 1);
    EXPECT_EQ(testWorld.getWorldRooms()[1]->getNeighbours()[1], 3);
    EXPECT_EQ(testWorld.getWorldRooms()[2]->getNeighbours()[1], 2);
}

TEST_F(WorldTest, test_initWorld_npcs) {
    std::string firstEntName = "TestEntity1";
    ASSERT_EQ(testWorld.getWorldRooms()[0]->getPopulation().size(), 1);
    EXPECT_STREQ(testWorld.getWorldRooms()[0]->getPopulation()[0]->getName().c_str(), firstEntName.c_str());
    EXPECT_EQ(testWorld.getWorldRooms()[0]->getPopulation()[0]->getInventory()[0]->getID(), 4);
    EXPECT_STREQ(std::string("This is a test dialog.").c_str(), testWorld.getWorldRooms()[0]->getPopulation()[0]->getDialog().c_str());
}

TEST_F(WorldTest, test_initWorld_world_missions) {
    // Test if world missions are loaded correctly.
    ASSERT_EQ(testWorld.getWorldMission().size(), 1);
    EXPECT_EQ(testWorld.getWorldMission()[0].getTargetRoom(), 1);
}

TEST_F(WorldTest, test_initWorld_room_choiceDescription) {
    EXPECT_STREQ(std::string("Choice Description of room 1").c_str(), testWorld.getWorldRooms()[0]->getChoiceDescription().c_str());
    EXPECT_STREQ(std::string("Choice Description of room 2").c_str(), testWorld.getWorldRooms()[1]->getChoiceDescription().c_str());
    EXPECT_STREQ(std::string("Choice Description of room 3").c_str(), testWorld.getWorldRooms()[2]->getChoiceDescription().c_str());
}

TEST_F(WorldTest, test_initWorld_room_initial_lock_status) {
    EXPECT_EQ(false, testWorld.getWorldRooms()[0]->isLocked());
    EXPECT_EQ(true, testWorld.getWorldRooms()[1]->isLocked());
    EXPECT_EQ(false, testWorld.getWorldRooms()[2]->isLocked());
}