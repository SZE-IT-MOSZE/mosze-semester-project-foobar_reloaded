# SpaceWalk_TheGame

A Roleplay Space adventure game. You can move room to room, collect items and open new paths to explore the game world. Meet and interact with NPCs.

## Project documentation

A projekt dokumentálása megtalálható a documentation/main.pdf dokumentumban.

## Build with cmake

CMake legenerálja a makefile-t a CMakeLists.txt alapján, majd a `make` parancs segítségével lehet lefordítani a programot.

### Spacewalk

```
mkdir build && cd build
cmake ../
```

### Test Room

```
mkdir build && cd build
cmake ../ -DTEST_ROOM=ON
```

### Test World 

```
mkdir build && cd build
cmake ../ -DTEST_WORLD=ON
```
### Test Object 

```
mkdir build && cd build
cmake ../ -DTEST_OBJECT=ON
```

### Test session manager 

```
mkdir build && cd build
cmake ../ -DTEST_SESSION=ON
```

### Test Interface 

```
mkdir build && cd build
cmake ../ -DTEST_INTERFACE=ON
```

### Test Unit 

```
mkdir build && cd build
cmake ../ -DTEST_UNIT=ON
```

### Make használata

```
make
```