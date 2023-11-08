cmake_minimum_required(VERSION 3.22)
 
project(dadgammon)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(dadgammon main.cpp)

add_subdirectory(GammonLib)

target_link_libraries(dadgammon PUBLIC GammonLib)

target_include_directories(dadgammon PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/GammonLib"
                          )