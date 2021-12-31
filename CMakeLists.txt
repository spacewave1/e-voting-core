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

add_subdirectory(googletest)

set(SOURCE_FILES src/main.cpp src/peer.cpp src/peer.h src/abstractTopology.cpp src/abstractTopology.h src/networkPlan.cpp src/networkPlan.h src/straightLineTopology.cpp src/straightLineTopology.h src/abstractThread.h src/straightLineSyncThread.cpp src/straightLineSyncThread.h src/election.h src/straightLineDistributeThread.cpp src/straightLineDistributeThread.h src/straightLineTopDistributeThread.cpp src/straightLineTopDistributeThread.h)

enable_testing()

add_executable(vote_p2p ${SOURCE_FILES})
add_library(vote_p2p_lib STATIC ${SOURCE_FILES})

add_subdirectory(test)

target_link_libraries(vote_p2p PRIVATE cppzmq nlohmann_json::nlohmann_json Threads::Threads)
target_link_libraries(vote_p2p_lib PRIVATE cppzmq nlohmann_json::nlohmann_json Threads::Threads)