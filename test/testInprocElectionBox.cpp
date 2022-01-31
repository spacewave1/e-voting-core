//
// Created by wnabo on 15.01.2022.
//

#include "gtest/gtest.h"
#include "../src/inprocElectionboxThread.h"
#include "../src/electionBuilder.h"
#include "MockSocket.h"

using ::testing::Return;

TEST(InprocElectionBox, UpdateTest) {

    std::map<size_t, std::string> options;
    options[1] = "A";
    options[2] = "B";
    options[3] = "C";

    std::map<std::string, int> votes;
    votes["asd"] = -1;
    votes["qwe"] = -1;
    votes["yxc"] = -1;
    votes["qwy"] = -1;

    election election_before_update = election::create(1)
            .withSetupDate(time(NULL))
            .withVoteOptions(options)
            .withParticipantsVotes(votes);

    std::vector<election> electionBox;
    electionBox.push_back(election_before_update);
    mockSocket mock_inproc_subscribe_socket;

    EXPECT_CALL(mock_inproc_subscribe_socket, connect("inproc", "update_elections", 0));
    EXPECT_CALL(mock_inproc_subscribe_socket, recv())
            .Times(5)
            .WillOnce(Return("1"))
            .WillOnce(Return("4"))
            .WillOnce(Return(std::to_string(time(NULL))))
            .WillOnce(Return(("[\"A\",\"B\",\"C\"]")))
            .WillOnce(Return(("[[\"asd\",1],[\"yxc\",-1],[\"qwe\",-1],[\"qwy\",-1]]")));

    inprocElectionboxThread testee = inprocElectionboxThread(electionBox, (abstractSocket &) mock_inproc_subscribe_socket);
    testee.runElectionUpdate();

    ASSERT_EQ(electionBox.size(), 1);
    ASSERT_EQ(electionBox[0].getVotes().at("asd"), 1);
    ASSERT_EQ(electionBox[0].getVotes().at("yxc"), -1);
    ASSERT_EQ(electionBox[0].getVotes().at("qwe"), -1);
    ASSERT_EQ(electionBox[0].getVotes().at("qwe"), -1);
}

TEST(InprocElectionBox, AddNewElectionThatHasDifferentSetupDate) {

    std::map<size_t, std::string> options;
    options[1] = "A";
    options[2] = "B";
    options[3] = "C";

    std::map<std::string, int> votes;
    votes["asd"] = -1;
    votes["qwe"] = -1;
    votes["yxc"] = -1;
    votes["qwy"] = -1;

    election election_before_update = election::create(1)
            .withSetupDate(time(NULL))
            .withVoteOptions(options)
            .withParticipantsVotes(votes);

    std::vector<election> electionBox;
    electionBox.push_back(election_before_update);
    mockSocket mock_inproc_subscribe_socket;

    EXPECT_CALL(mock_inproc_subscribe_socket, connect("inproc", "update_elections", 0));
    EXPECT_CALL(mock_inproc_subscribe_socket, recv())
            .Times(5)
            .WillOnce(Return("1"))
            .WillOnce(Return("4"))
            .WillOnce(Return(std::to_string(time(NULL) + 1)))
            .WillOnce(Return(("[\"A\",\"B\",\"C\"]")))
            .WillOnce(Return(("[[\"asd\",1],[\"yxc\",-1],[\"qwe\",-1],[\"qwy\",-1]]")));

    inprocElectionboxThread testee = inprocElectionboxThread(electionBox, (abstractSocket &) mock_inproc_subscribe_socket);
    testee.runElectionUpdate();

    ASSERT_EQ(electionBox.size(), 2);

    ASSERT_EQ(electionBox[0].getVotes().at("asd"), -1);
    ASSERT_EQ(electionBox[0].getVotes().at("yxc"), -1);
    ASSERT_EQ(electionBox[0].getVotes().at("qwe"), -1);
    ASSERT_EQ(electionBox[0].getVotes().at("qwe"), -1);

    ASSERT_EQ(electionBox[1].getVotes().at("asd"), 1);
    ASSERT_EQ(electionBox[1].getVotes().at("yxc"), -1);
    ASSERT_EQ(electionBox[1].getVotes().at("qwe"), -1);
    ASSERT_EQ(electionBox[1].getVotes().at("qwe"), -1);
}

TEST(InprocElectionBox, AddNewElection) {

    std::map<size_t, std::string> options;
    options[1] = "A";
    options[2] = "B";
    options[3] = "C";

    std::map<std::string, int> votes;
    votes["asd"] = -1;
    votes["qwe"] = -1;
    votes["yxc"] = -1;
    votes["qwy"] = -1;

    election
            election_before_update = election::create(1)
            .withVoteOptions(options)
            .withParticipantsVotes(votes);

    std::vector<election> electionBox;
    electionBox.push_back(election_before_update);
    mockSocket mock_inproc_subscribe_socket;

    EXPECT_CALL(mock_inproc_subscribe_socket, connect("inproc", "update_elections", 0));
    EXPECT_CALL(mock_inproc_subscribe_socket, recv())
            .Times(4)
            .WillOnce(Return("2"))
            .WillOnce(Return("4"))
            .WillOnce(Return(("[\"A\",\"B\",\"C\",\"D\"]")))
            .WillOnce(Return(("[[\"yyy\",1],[\"aaa\",-1],[\"eee\",-1],[\"bbb\",-1]]")));

    inprocElectionboxThread testee = inprocElectionboxThread(electionBox, (abstractSocket &) mock_inproc_subscribe_socket);
    testee.runElectionUpdate();

    // TODO: Test the rest of the election member vars
    ASSERT_EQ(electionBox.size(), 2);
    ASSERT_EQ(electionBox[1].getVotes().at("yyy"), 1);
    ASSERT_EQ(electionBox[1].getVotes().at("aaa"), -1);
    ASSERT_EQ(electionBox[1].getVotes().at("eee"), -1);
    ASSERT_EQ(electionBox[1].getVotes().at("bbb"), -1);
}

