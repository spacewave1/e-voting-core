//
// Created by wnabo on 07.01.2022.
//

#include "gtest/gtest.h"
#include "../src/election.h"
#include "../src/electionBuilder.h"

TEST(electionTest, place_first_vote) {
    std::set<std::string> participants = {"asd"};
    const electionPrototype &prototype = electionPrototype();

    election testee = election::create(0).withParticipants(participants);
    EXPECT_TRUE(testee.placeVote("asd", 1));
}

TEST(electionTest, place_vote_existing) {
    std::set<std::string> participants = {"asd"};
    const electionPrototype &prototype = electionPrototype();
    std::map<std::string, int> participant_votes;
    participant_votes["asd"] = 0;

    election testee = election::create(0).withParticipants(participants).withParticipantsVotes(participant_votes);
    EXPECT_FALSE(testee.placeVote("asd", 1));
}

TEST(electionTest, place_vote_unauthorized_participant) {
    std::set<std::string> participants = {"yxc"};
    const electionPrototype &prototype = electionPrototype();
    std::map<std::string, int> participant_votes;

    election testee = election::create(0).withParticipants(participants).withParticipantsVotes(participant_votes);
    EXPECT_FALSE(testee.placeVote("asd", 1));
}

TEST(electionTest, prepare_for_distribtion) {
    std::set<std::string> participants = {"yxc", "abc", "dfg"};
    const electionPrototype &prototype = electionPrototype();

    std::map<std::string, int> participant_votes;
    std::for_each(participants.begin(), participants.end(), [&participant_votes](std::string participant_identity) {
        participant_votes[participant_identity] = 0;
    });

    election testee = election::create(0).withParticipants(participants);
    testee.prepareForDistribtion(participants);

    EXPECT_EQ(testee.getParticipantsVotes(), participant_votes);
    EXPECT_EQ(testee.getSetupDate(), std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
}