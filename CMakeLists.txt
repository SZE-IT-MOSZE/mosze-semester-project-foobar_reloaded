cmake_minimum_required(VERSION 3.14)

# set the project name
project(SpaceWalk VERSION 2.0)

# common comp flags
add_compile_options(-g -Wall)

# set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# version control config header file
configure_file(${PROJECT_SOURCE_DIR}/src/Config.h.in Config.h)

# set up options to build tests
option(TEST_ROOM "Test room class of engine.h" OFF)
option(TEST_OBJECT "Test object class of engine.h" OFF)
option(TEST_WORLD "Test world class of engine.h" OFF)
option(TEST_SESSION "Build unittest, that tries to cover the core functionalities of the game." OFF)
option(TEST_INTERFACE "Test interface implementation, that connects the game with the game engine." OFF)
option(TEST_UNIT "Build unit test." OFF)

# add include directories, where header files can be found
include_directories("${PROJECT_SOURCE_DIR}/src")
add_library(tinyxml2 src/tinyxml2.cpp)
add_library(engine src/engine.cpp)
add_library(interface src/interface.cpp)

macro(runtest testname)
	include(FetchContent)
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG release-1.12.1
	)
	# For Windows: Prevent overridin the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
	enable_testing()
	add_executable(test_program ${testname})
	target_link_libraries(
		test_program
		GTest::gtest_main
		tinyxml2
		engine
		interface
	)
	include(GoogleTest)
	gtest_discover_tests(test_program)
endmacro()

macro(buildworld)
	add_executable(spacewalk src/main.cpp)
	target_link_libraries(spacewalk PUBLIC tinyxml2 engine interface)
	target_include_directories(spacewalk PUBLIC
		"${PROJECT_BINARY_DIR}"
		"${PROJECT_SOURCE_DIR}"
		"${PROJECT_SOURCE_DIR}/src"
		)
endmacro()

if(TEST_ROOM)
	runtest("tests/test_room.cpp")
elseif(TEST_OBJECT)
	runtest("tests/test_object.cpp")
elseif(TEST_SESSION)
	runtest("tests/test_sessionmanager.cpp")
elseif(TEST_WORLD)
	runtest("tests/test_world.cpp")
elseif(TEST_INTERFACE)
	runtest("tests/test_interface.cpp")
elseif(TEST_UNIT)
	runtest("tests/test_unit.cpp")
else()
	buildworld()
endif()
