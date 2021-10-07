cmake_minimum_required(VERSION 3.17)
project(vote_p2p)

set(CMAKE_CXX_STANDARD 20)

set(cppzmq_DIR ~/cppzmq/build)

find_package(cppzmq REQUIRED)
include_directories(${cppzmq_INCLUDE_DIR})

set(SOURCE_FILES main.cpp)

add_executable(vote_p2p ${SOURCE_FILES})

target_link_libraries(vote_p2p cppzmq)