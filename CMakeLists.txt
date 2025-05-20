cmake_minimum_required(VERSION 3.18)

project(Projet_jeux_C- LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23) # si nécessaire

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

file (GLOB_RECURSE sourceCode src/*.cpp src/*.h)

qt_standard_project_setup()

add_executable(Projet_jeux_C- ${sourceCode}
        src/player.cpp
        src/mobmort.cpp
        src/mob.cpp
        src/projectile.cpp
)

target_link_libraries(Projet_jeux_C- PRIVATE Qt6::Widgets)