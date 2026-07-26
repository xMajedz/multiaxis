cmake_minimum_required(VERSION 3.10)
project(multiaxis LANGUAGES CXX)

add_subdirectory(vendor/luau EXCLUDE_FROM_ALL)
add_subdirectory(vendor/raylib)
add_subdirectory(vendor/box3d)

add_executable(multiaxis
sources/ApiRaylib.cpp
sources/ApiRaygui.cpp
sources/ApiRaymath.cpp
sources/Api.cpp
sources/ApiGame.cpp
sources/ApiRenderer.cpp
sources/Game.cpp
sources/Renderer.cpp
sources/uielement.cpp
sources/multiaxis.cpp)

target_include_directories(multiaxis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/raygui/src)

target_link_libraries(multiaxis PRIVATE Luau.Compiler Luau.VM raylib box3d::box3d)
