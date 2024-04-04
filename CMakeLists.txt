cmake_minimum_required(VERSION 3.20.0)
project(ZYCO)

set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SRC_LIST ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(app ${SRC_LIST})