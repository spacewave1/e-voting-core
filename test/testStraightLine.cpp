//
// Created by wnabo on 30.12.2021.
//

#include "../src/straightLineDistributeThread.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockSocket.h"
#include "../src/electionBuilder.h"

using ::testing::Return;

nlohmann::json options;
/*
TEST(straightLineDistributeTest, distribution_invert_direction_up) {
    mockSocket sub_mock_socket;
    mockSocket pub_mock_socket;

    straightLineDistributeThread testee = straightLineDistributeThread(((abstractSocket &) sub_mock_socket),
                                                                       ((abstractSocket &) pub_mock_socket));
    EXPECT_EQ(testee.invertDirection("up"), "down");
}

TEST(straightLineDistributeTest, distribution_invert_direction_down) {
    mockSocket sub_mock_socket;
    mockSocket pub_mock_socket;

    straightLineDistributeThread testee = straightLineDistributeThread(((abstractSocket &) sub_mock_socket),
                                                                       ((abstractSocket &) pub_mock_socket));
    EXPECT_EQ(testee.invertDirection("down"), "up");
}

TEST(straightLineDistributeTest, receive_from_up) {
    mockSocket sub_mock_socket;
    mockSocket pub_mock_socket;


    EXPECT_CALL(sub_mock_socket, connect("tcp", "127.0.0.1", 5001));
    EXPECT_CALL(sub_mock_socket, recv())
            .Times(5)
            .WillOnce(Return("1"))
            .WillOnce(Return("4"))
            .WillOnce(Return(std::to_string(time(NULL))))
            .WillOnce(Return(("[\"A\",\"B\",\"C\"]")))
            .WillOnce(Return(("[[\"asd\",1],[\"yxc\",2],[\"qwe\",-1],[\"qwy\",1]]")));

    straightLineDistributeThread testee = straightLineDistributeThread(((abstractSocket &) pub_mock_socket),
                                                                       ((abstractSocket &) sub_mock_socket));
    const election &snapshot = election::create(0).withSetupDate(time(NULL));
    testee.setParams(nullptr, "127.0.0.1", "<toIp>", 0, 3, snapshot);
    testee.setSubscribePort(((size_t) 5001));
    election result = testee.receiveFrom("127.0.0.1");

    EXPECT_EQ(1, result.getPollId());
    EXPECT_EQ(4, result.getSequenceNumber());
    EXPECT_EQ(time(NULL),result.getSetupDate());
    EXPECT_EQ("[\"A\",\"B\",\"C\"]", result.getElectionOptionsJson().dump());
    EXPECT_EQ("[[\"asd\",1],[\"qwe\",-1],[\"qwy\",1],[\"yxc\",2]]", result.getVotesAsJson().dump());
}

TEST(straightLineDistributeTest, receive_from_down) {
    mockSocket sub_mock_socket;
    mockSocket pub_mock_socket;

    EXPECT_CALL(sub_mock_socket, connect("tcp", "127.0.0.1", 5001));
    EXPECT_CALL(sub_mock_socket, recv())
            .Times(5)
            .WillOnce(Return("1"))
            .WillOnce(Return("4"))
            .WillOnce(Return(std::to_string(time(NULL))) )
            .WillOnce(Return("[\"A\",\"B\",\"C\"]"))
            .WillOnce(Return(("[[\"aa\",1],[\"bb\",2],[\"cc\",-1],[\"dd\",1]]")));

    straightLineDistributeThread testee = straightLineDistributeThread(((abstractSocket &) pub_mock_socket),
                                                                       ((abstractSocket &) sub_mock_socket));
    const election &snapshot = election::create(1).withSetupDate(time(NULL));
    testee.setParams(nullptr, "irrelevant", "127.0.0.1", 0, 3, snapshot);
    testee.setSubscribePort(((size_t) 5001));
    election result = testee.receiveFrom("127.0.0.1");

    EXPECT_EQ(1, result.getPollId());
    EXPECT_EQ(4, result.getSequenceNumber());
    EXPECT_EQ(time(NULL), result.getSetupDate());
    EXPECT_EQ("[\"A\",\"B\",\"C\"]", result.getElectionOptionsJson().dump());
    EXPECT_EQ("[[\"aa\",1],[\"bb\",2],[\"cc\",-1],[\"dd\",1]]", result.getVotesAsJson().dump());
}
 */