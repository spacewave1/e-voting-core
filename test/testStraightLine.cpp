//
// Created by wnabo on 30.12.2021.
//

#include "../src/straightLineDistributeThread.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockSocket.h"

using ::testing::Return;

TEST(distribution_invert_direction_up, DemonstrateGTestMacros) {
    mockSocket sub_mock_socket;
    mockSocket pub_mock_socket;

    straightLineDistributeThread testee = straightLineDistributeThread(((abstractSocket&) sub_mock_socket), ((abstractSocket&)pub_mock_socket));
    EXPECT_EQ(testee.invertDirection("up"), "down");
}

TEST(distribution_invert_direction_down, DemonstrateGTestMacros) {
    mockSocket sub_mock_socket;
    mockSocket pub_mock_socket;

    straightLineDistributeThread testee = straightLineDistributeThread(((abstractSocket&) sub_mock_socket), ((abstractSocket&)pub_mock_socket));
    EXPECT_EQ(testee.invertDirection("down"), "up");
}

TEST(distribution, receive_from_up) {
    mockSocket sub_mock_socket;
    mockSocket pub_mock_socket;


    EXPECT_CALL(sub_mock_socket, connect("127.0.0.1",5001));
    EXPECT_CALL(sub_mock_socket, recv())
            .Times(3)
            .WillOnce(Return("1"))
            .WillOnce(Return("4"))
            .WillOnce(Return("[{0:A},{1:B},{2:C}]"));

    straightLineDistributeThread testee = straightLineDistributeThread(((abstractSocket&) pub_mock_socket), ((abstractSocket&) sub_mock_socket));
    testee.setParams(nullptr,"127.0.0.1","<toIp>", 0, 3,election());
    testee.setSubscribePort(((size_t)5001));
    election result = testee.receiveFromUp();

    EXPECT_EQ(1, result.getPollId());
    EXPECT_EQ(4, result.getSequenceNumber());
    EXPECT_EQ("[{0:A},{1:B},{2:C}]", result.getJson());
}

TEST(distribution, receive_from_down) {
    mockSocket sub_mock_socket;
    mockSocket pub_mock_socket;

    EXPECT_CALL(sub_mock_socket, connect("127.0.0.1",5001));
    EXPECT_CALL(sub_mock_socket, recv())
            .Times(3)
            .WillOnce(Return("1"))
            .WillOnce(Return("4"))
            .WillOnce(Return("[{0:A},{1:B},{2:C}]"));

    straightLineDistributeThread testee = straightLineDistributeThread(((abstractSocket&) pub_mock_socket), ((abstractSocket&) sub_mock_socket));
    testee.setParams(nullptr,"irrelevant","127.0.0.1", 0, 3,election());
    testee.setSubscribePort(((size_t)5001));
    election result = testee.receiveFromDown();

    EXPECT_EQ(1, result.getPollId());
    EXPECT_EQ(4, result.getSequenceNumber());
    EXPECT_EQ("[{0:A},{1:B},{2:C}]", result.getJson());
}

TEST(distribution, request_from_up) {

}

TEST(distribution, request_from_down) {

}

TEST(distribution, init_request) {

}