#include <gtest/gtest.h>
#include "engine.hpp"

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

TEST_F(WorldTest, test_xml_module) {
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
    // test NextSiblingElement()
}

TEST_F(WorldTest, test_initWorld) {
    ASSERT_EQ(testWorld.getWorldRooms().size(), 3);
    for (int i = 0; i < int(testWorld.getWorldRooms().size()); i++) {
        EXPECT_EQ(testWorld.getWorldRooms()[i]->getID(), (i+1));
        for (int j = 0; j < int(testWorld.getWorldRooms()[i]->getItems().size()); j++) {
            EXPECT_EQ(testWorld.getWorldRooms()[i]->getItems()[j]->getID(), (i+1));
        }
    }
    EXPECT_EQ(testWorld.getWorldRooms()[0]->getNeighbours()[0], 2);
    EXPECT_EQ(testWorld.getWorldRooms()[1]->getNeighbours()[0], 1);
    EXPECT_EQ(testWorld.getWorldRooms()[1]->getNeighbours()[1], 3);
    EXPECT_EQ(testWorld.getWorldRooms()[2]->getNeighbours()[1], 2);
    std::string firstEntName = "TestEntity1";
    EXPECT_EQ(testWorld.getWorldRooms()[0]->getPopulation().size(), 1);
    EXPECT_STREQ(testWorld.getWorldRooms()[0]->getPopulation()[0]->getName().c_str(), firstEntName.c_str());
    EXPECT_EQ(testWorld.getWorldRooms()[0]->getPopulation()[0]->getInventory()[0]->getID(), 4);
}