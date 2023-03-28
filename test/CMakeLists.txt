cmake_minimum_required(VERSION 3.17)

add_executable(shatest shatest.c)

set(SOURCE_FILES
        testStraightLine.cpp
        MockSocket.h
        testElectionLogic.cpp
        testInprocElectionBox.cpp
        testPeer.cpp
        testHill.cpp
        testDecentralNode.cpp
        testHashService.cpp
        testStorage.cpp testIdentityService.cpp)

set(name test_p2p_vote)

add_executable(${name} ${SOURCE_FILES})

target_link_libraries(shatest PUBLIC crypt)

target_link_libraries(${name} PUBLIC
        gtest
        gtest_main
        gmock
        gmock_main
        vote_core
        cppzmq
        nlohmann_json::nlohmann_json
        Threads::Threads
        )

add_test(
        NAME ${name}
        COMMAND ${name}
)
