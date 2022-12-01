cmake_minimum_required(VERSION 3.17)
project(vote_p2p)

set(nlohmann_json_DIR ~/json-3.10.4/build)
set(CMAKE_CXX_STANDARD 20)
set(cppzmq_DIR ~/.libs/cppzmq-4.9.0/build)
set(THREADS_PREFER_PTHREAD_FLAG ON)
include(FetchContent)

find_package(Threads REQUIRED)
find_package(nlohmann_json 3.11.0 REQUIRED)
find_package(cppzmq REQUIRED)

include_directories(${cppzmq_INCLUDE_DIR})

add_subdirectory(googletest)

set(SOURCE_FILES
        src/main.cpp
        src/evoting/peer.cpp
        src/evoting/peer.h
        src/evoting/abstractTopology.cpp
        src/evoting/abstractTopology.h
        src/evoting/networkPlan.cpp
        src/evoting/networkPlan.h
        src/evoting/straightLineTopology.cpp
        src/evoting/straightLineTopology.h
        src/evoting/abstractThread.h
        src/evoting/straightLineSyncThread.cpp
        src/evoting/straightLineSyncThread.h
        src/evoting/election.h
        src/evoting/election.cpp
        src/evoting/straightLineDistributeThread.cpp
        src/evoting/straightLineDistributeThread.h
        src/evoting/zmqSocketAdapter.h
        src/evoting/zmqSocketAdapter.cpp
        src/evoting/zmqSocketAdapter.h
        src/network/abstractSocket.h
        src/evoting/electionBuilder.cpp
        src/evoting/electionBuilder.h
        src/evoting/electionPrototype.h
        src/evoting/inprocElectionboxThread.cpp
        src/evoting/inprocElectionboxThread.h
        src/evoting/logger.h
        src/evoting/replyKeyThread.cpp
        src/evoting/replyKeyThread.h
        src/evoting/basicEncryptionService.cpp
        src/evoting/basicEncryptionService.h
        src/evoting/hillLengthException.h
        src/evoting/interruptException.h
        )

enable_testing()

add_executable(vote_app  ${SOURCE_FILES})
add_library(evote_core SHARED ${SOURCE_FILES})

add_subdirectory(test)

target_link_libraries(vote_app PRIVATE cppzmq nlohmann_json::nlohmann_json Threads::Threads)
target_link_libraries(evote_core PRIVATE cppzmq nlohmann_json::nlohmann_json Threads::Threads)