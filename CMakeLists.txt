cmake_minimum_required(VERSION 3.16)

project(OctreeTest
        VERSION 1.0
        LANGUAGES CXX)


# Use modern C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Header files
include_directories(
    include
)


# Octree library
add_library(octree
    src/octree.cpp
    src/vector3d.cpp
)


# Test executable
add_executable(test_octree
    tests/test_octree.cpp
)


# Link octree library
target_link_libraries(test_octree
    PRIVATE octree
)