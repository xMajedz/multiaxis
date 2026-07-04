cmake_minimum_required(VERSION 3.10)
project(multiaxis LANGUAGES CXX)

add_subdirectory(vendor/luau)
add_subdirectory(vendor/raylib)
add_subdirectory(vendor/box3d)

add_executable(multiaxis sources/Game.cpp sources/multiaxis.cpp)

target_link_libraries(multiaxis PRIVATE Luau.Compiler Luau.VM raylib box3d::box3d)
