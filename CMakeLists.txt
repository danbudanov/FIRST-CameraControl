cmake_minimum_required(VERSION 3.6)
project(FIRST_CameraControl)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/stdSocketTools.cpp
        src/stdSocketTools.h
        src/cameraControl.h
        src/detectJoystick.cpp
        src/joystick.h
        src/main.cpp)

add_executable(FIRST_CameraControl ${SOURCE_FILES})