cmake_minimum_required(VERSION 3.17)
project(vote_p2p)


set(nlohmann_json_DIR ~/json-3.10.4/build)
set(CMAKE_CXX_STANDARD 20)
set(cppzmq_DIR ~/cppzmq/build)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(cppzmq REQUIRED)

include_directories(${cppzmq_INCLUDE_DIR})

set(SOURCE_FILES main.cpp)

add_executable(vote_p2p ${SOURCE_FILES})

target_link_libraries(vote_p2p PRIVATE cppzmq nlohmann_json::nlohmann_json Threads::Threads)