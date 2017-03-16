cmake_minimum_required(VERSION 3.6)
project(FIRST_CameraControl)

set(CMAKE_CXX_STANDARD 14)

# SDL2
include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
include_directories(SYSTEM ${SDL2_INCLUDE_DIRS})

set(SOURCE_FILES
        src/stdSocketTools.cpp
        src/main.cpp
        src/Joystick.cpp
        src/Controller.cpp
        src/Camera.cpp)

add_executable(FIRST_CameraControl ${SOURCE_FILES})
target_link_libraries(FIRST_CameraControl ${SDL2_LIBRARIES})
