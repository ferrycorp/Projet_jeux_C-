cmake_minimum_required(VERSION 3.18)

project(NOMDEVOTREPROJET LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23) # si nécessaire

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

file (GLOB_RECURSE sourceCode src/*.cpp src/*.h)

qt_standard_project_setup()

add_executable(NOMDEVOTREPROJET ${sourceCode}
        src/player.cpp)

target_link_libraries(NOMDEVOTREPROJET PRIVATE Qt6::Widgets)