cmake_minimum_required(VERSION 3.10)
project(box3d-example LANGUAGES CXX)

add_subdirectory(vendor/luau)
add_subdirectory(vendor/raylib)
add_subdirectory(vendor/box3d)

add_executable(multiaxis sources/multiaxis.cpp)

target_link_libraries(multiaxis PRIVATE box3d::box3d)
