cmake_minimum_required(VERSION 3.13...3.20)

set(CMAKE_CXX_STANDARD 17)

project(SevenSegmentProject C CXX ASM)

add_executable(${PROJECT_NAME} app/main.cpp)

add_library(srclib SHARED src/IStream.cpp src/Stream.cpp src/OStream.cpp src/SevenSegment.cpp src/gpio.cpp src/terminal.cpp)

target_include_directories(srclib PUBLIC include/)

target_link_libraries(${PROJECT_NAME} srclib)


