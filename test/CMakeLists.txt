cmake_minimum_required(VERSION 3.17)

set(SOURCE_FILES testStraightLine.cpp)
set(name test_p2p_vote)

add_executable(${name} ${SOURCE_FILES})
target_link_libraries(test_p2p_vote PUBLIC
        gtest_main
        vote_p2p_lib
        cppzmq
        nlohmann_json::nlohmann_json
        Threads::Threads
        )

add_test(
        NAME ${name}
        COMMAND ${name}
)
