cmake_minimum_required(VERSION 3.16)
project(sfml_phs_simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(sfml_phs_simulator src/main.cpp)

target_link_libraries(sfml_phs_simulator PRIVATE sfml-graphics sfml-window sfml-system)