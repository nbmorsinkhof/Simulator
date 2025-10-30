

cmake_minimum_required(VERSION 3.15)

project(simulator LANGUAGES CXX)

add_executable( simulator
    src/main.cpp
    src/system.cpp
    src/state_space.cpp
    src/simulator.cpp
    src/graphics.cpp
)

# C++ standard (pick one of these styles)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories(
    simulator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(Threads REQUIRED)

target_link_libraries(simulator PRIVATE
    Qt5::Widgets
    Threads::Threads
)