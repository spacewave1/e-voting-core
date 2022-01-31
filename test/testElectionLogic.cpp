//
// Created by wnabo on 07.01.2022.
//

#include "gtest/gtest.h"
#include "../src/election.h"
#include "../src/electionBuilder.h"

TEST(electionTest, place_first_vote) {
    std::set<std::string> participants = {"asd", "qwe", "yxc"};
    electionPrototype prototype = electionPrototype();
    std::map<size_t, std::string> election_options;
    election_options[0] = "A";
    election_options[1] = "B";
    election_options[2] = "C";

    std::map<std::string, int> votes;
    votes["asd"] = -1;
    votes["qwe"] = -1;
    votes["yxc"] = -1;

    election testee = election::create(0).withSequenceNumber(0).withVoteOptions(election_options).withParticipants(
            participants).withParticipantsVotes(votes);

    EXPECT_TRUE(testee.placeVote("asd", 1));
    EXPECT_EQ(testee.getVotes().at("asd"),1);
    EXPECT_EQ(testee.getVotes().at("qwe"),-1);
    EXPECT_EQ(testee.getVotes().at("yxc"),-1);
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
        participant_votes[participant_identity] = -1;
    });

    election testee = election::create(0).withParticipants(participants);
    testee.prepareForDistribtion(participants);

    EXPECT_EQ(testee.getParticipantsVotes(), participant_votes);
    EXPECT_EQ(testee.getSetupDate(), std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
}

TEST(electionTest, getJsonVotesAfterPlacingTwoVotes) {
    std::set<std::string> participants = {"yxc", "abc", "dfg"};
    const electionPrototype &prototype = electionPrototype();

    std::map<std::string, size_t> participant_votes;
    std::for_each(participants.begin(), participants.end(), [&participant_votes](std::string participant_identity) {
        participant_votes[participant_identity] = 0;
    });

    nlohmann::json json_election_options;
    std::map<size_t, std::string> election_options;
    election_options[0] = "A";
    election_options[1] = "B";
    election_options[2] = "C";

    election testee = election::create(0)
            .withParticipants(participants)
            .withVoteOptions(election_options);
    testee.prepareForDistribtion(participants);

    testee.placeVote("yxc", 0);
    testee.placeVote("abc", 0);
    testee.placeVote("dfg", 1);

    std::string string = testee.getCurrentElectionStatisticAsJson().dump();
    EXPECT_EQ(string, "{\"count\":2,\"name\":\"A\"},{\"count\":1,\"name\":\"B\"},{\"count\":0,\"name\":\"C\"}");
}

TEST(electionTest, setVotesFromJson) {
    std::set<std::string> participants = {"yxc", "abc", "dfg"};
    std::map<size_t, std::string> election_options;
    election_options[0] = "A";
    election_options[1] = "B";
    election_options[2] = "C";

    nlohmann::json jsonVotes = nlohmann::json::parse(
            "{\"192.168.0.174\":2,\"192.168.0.19\":-1,\"192.168.0.3\":0,\"192.168.0.88\":-1}");

    election testee = election::create(0)
            .withParticipants(participants)
            .withVoteOptions(election_options);

    testee.prepareForDistribtion(participants);

    testee.setJsonVotesToVotes(jsonVotes);

    const std::map<std::string, int> &result = testee.getVotes();

    EXPECT_TRUE(result.contains("192.168.0.174"));
    EXPECT_TRUE(result.contains("192.168.0.19"));
    EXPECT_TRUE(result.contains("192.168.0.3"));
    EXPECT_TRUE(result.contains("192.168.0.88"));
    EXPECT_EQ(result.at("192.168.0.174"), 2);
    EXPECT_EQ(result.at("192.168.0.19"), -1);
    EXPECT_EQ(result.at("192.168.0.3"), 0);
    EXPECT_EQ(result.at("192.168.0.88"), -1);
}