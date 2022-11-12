/**
 * @file test_unit.cpp
 * @author Peter Bence (ecneb2000@gmail.com)
 * @brief This is the unit test of the game. In this test the real soty file will be tested, and the test counts successful if all missions are finished in the game.
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <gtest/gtest.h>
#include "sessionmanager.cpp"

TEST(test_unit, unit) {
    //TODO Segfault when loading in real story. Go fix it.
    SessionManager test_game("../story.xml");
    EXPECT_EQ(true, test_game.startSession());
}