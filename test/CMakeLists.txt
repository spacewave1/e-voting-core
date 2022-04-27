cmake_minimum_required(VERSION 3.17)

set(SOURCE_FILES testStraightLine.cpp MockSocket.h testElectionLogic.cpp testStraightLine.h testElectionLogic.h testInprocElectionBox.cpp testPeer.cpp testEncryption.cpp testPeer.cpp)
set(name test_p2p_vote)

add_executable(${name} ${SOURCE_FILES})
target_link_libraries(${name} PUBLIC
        gtest
        gtest_main
        gmock
        gmock_main
        vote_p2p_lib
        cppzmq
        nlohmann_json::nlohmann_json
        Threads::Threads
        sodium
        )

add_test(
        NAME ${name}
        COMMAND ${name}
)
