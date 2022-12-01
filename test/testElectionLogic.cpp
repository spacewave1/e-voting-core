//
// Created by wnabo on 07.01.2022.
//

#include "gtest/gtest.h"
#include "../src/evoting/election.h"
#include "../src/evoting/electionBuilder.h"
#include "../src/evoting/peer.h"

TEST(electionTest, place_first_vote) {
    std::set<std::string> participants = {"asd", "qwe", "yxc"};
    electionPrototype prototype = electionPrototype();
    std::map<size_t, std::string> election_options;
    election_options[0] = "A";
    election_options[1] = "B";
    election_options[2] = "C";

    std::map<std::string, std::string> votes;
    votes["asd"] = "-1";
    votes["qwe"] = "-1";
    votes["yxc"] = "-1";

    election testee = election::create(0).withSequenceNumber(0).withVoteOptions(election_options).withParticipants(
            participants).withParticipantsVotes(votes);

    EXPECT_TRUE(testee.placeVote("asd", "1"));
    EXPECT_EQ(testee.getVotes().at("asd"),"1");
    EXPECT_EQ(testee.getVotes().at("qwe"),"-1");
    EXPECT_EQ(testee.getVotes().at("yxc"),"-1");
}

TEST(electionTest, place_vote_existing) {
    std::set<std::string> participants = {"asd"};
    const electionPrototype &prototype = electionPrototype();
    std::map<std::string, std::string> participant_votes;
    participant_votes["asd"] = "0";

    election testee = election::create(0).withParticipants(participants).withParticipantsVotes(participant_votes);
    EXPECT_FALSE(testee.placeVote("asd", "1"));
}

TEST(electionTest, place_vote_unauthorized_participant) {
    std::set<std::string> participants = {"yxc"};
    const electionPrototype &prototype = electionPrototype();
    std::map<std::string, std::string> participant_votes;

    election testee = election::create(0).withParticipants(participants).withParticipantsVotes(participant_votes);
    EXPECT_FALSE(testee.placeVote("asd", "1"));
}

TEST(electionTest, prepare_for_distribtion) {
    std::set<std::string> participants = {"yxc", "abc", "dfg"};
    const electionPrototype &prototype = electionPrototype();

    std::map<std::string, std::string> participant_votes;
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

    testee.placeVote("yxc", "0");
    testee.placeVote("abc", "0");
    testee.placeVote("dfg", "1");

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

    const std::map<std::string, std::string> &result = testee.getVotes();

    EXPECT_TRUE(result.contains("192.168.0.174"));
    EXPECT_TRUE(result.contains("192.168.0.19"));
    EXPECT_TRUE(result.contains("192.168.0.3"));
    EXPECT_TRUE(result.contains("192.168.0.88"));
    EXPECT_EQ(result.at("192.168.0.174"), "2");
    EXPECT_EQ(result.at("192.168.0.19"), "-1");
    EXPECT_EQ(result.at("192.168.0.3"), "0");
    EXPECT_EQ(result.at("192.168.0.88"), "-1");
}

TEST(electionTest, test_hasFreeEvaluationGroups_with_evaluatedResult) {
    std::set<std::string> participants = {"asd", "qwe", "yxc"};
    electionPrototype prototype = electionPrototype();
    std::map<size_t, std::string> election_options;
    election_options[0] = "A";
    election_options[1] = "B";
    election_options[2] = "C";

    std::map<std::string, std::string> votes;
    votes["asd"] = "1";
    votes["qwe"] = "0";
    votes["yxc"] = "1";
    votes["ccc"] = "2";

    std::map<size_t, size_t> resultMap;
    resultMap[0] = 1;
    resultMap[1] = 2;
    resultMap[2] = 1;

    election testee = election::create(0).withSequenceNumber(0).withVoteOptions(election_options).withParticipants(
            participants).withParticipantsVotes(votes).withElectionResult(resultMap);

    bool result = testee.hasFreeEvaluationGroups();
    EXPECT_FALSE(result);
}

TEST(electionTest, test_hasFreeEvaluationGroups_withoutEvaluatedResult) {
    std::set<std::string> participants = {"asd", "qwe", "yxc"};
    electionPrototype prototype = electionPrototype();
    std::map<size_t, std::string> election_options;
    election_options[0] = "A";
    election_options[1] = "B";
    election_options[2] = "C";

    std::map<std::string, std::string> votes;
    votes["asd"] = "1";
    votes["qwe"] = "0";
    votes["yxc"] = "1";
    votes["ccc"] = "2";

    std::map<size_t, size_t> resultMap;
    resultMap[0] = 0;
    resultMap[1] = 0;
    resultMap[2] = 0;

    election testee = election::create(0).withSequenceNumber(0).withVoteOptions(election_options).withParticipants(
            participants).withParticipantsVotes(votes).withElectionResult(resultMap);

    bool result = testee.hasFreeEvaluationGroups();
    EXPECT_TRUE(result);
}

TEST(electionTest, test_getNumberOfPlaceVotes_three) {

    std::map<std::string, std::string> votes;
    votes["asd"] = "1";
    votes["qwe"] = "-1";
    votes["yxc"] = "1";
    votes["ccc"] = "2";

    election testee = election::create(0).withSequenceNumber(0).withParticipantsVotes(votes);

    size_t result = testee.getNumberOfPlacedVotes();
    EXPECT_EQ(result, 3);
}

TEST(electionTest, test_getEvaluatedVotes_zero) {
    electionPrototype prototype = electionPrototype();

    std::map<size_t, size_t> resultMap;
    resultMap[0] = 0;
    resultMap[1] = 0;
    resultMap[2] = 0;

    election testee = election::create(0).withSequenceNumber(0).withElectionResult(resultMap);

    size_t result = testee.getVotesEvaluatedTotal();
    EXPECT_EQ(result, 0);
}

TEST(electionTest, test_getEvaluatedVotes_five) {
    electionPrototype prototype = electionPrototype();

    std::map<size_t, size_t> resultMap;
    resultMap[0] = 2;
    resultMap[1] = 0;
    resultMap[2] = 3;

    election testee = election::create(0).withSequenceNumber(0).withElectionResult(resultMap);

    size_t result = testee.getVotesEvaluatedTotal();
    EXPECT_EQ(result, 5);
    EXPECT_EQ(testee.getElectionResultAsJson().dump(), "[[0,2],[1,0],[2,3]]");
}

TEST(electionTest, test_addToEvalGroup_first) {
    electionPrototype prototype = electionPrototype();

    std::map<std::string, std::string> votes;
    votes["asd"] = "1";
    votes["qwe"] = "-1";
    votes["yxc"] = "1";
    votes["ccc"] = "2";

    election testee = election::create(0).withSequenceNumber(0);

    testee.addToNextEvaluationGroup("asd");
    std::vector<std::vector<std::string>> result = testee.getEvaluationGroups();
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].size(), 1);
    EXPECT_EQ(result[0][0], "asd");
}

TEST(electionTest, test_addToEvalGroup_add_five) {
    electionPrototype prototype = electionPrototype();

    std::map<std::string, std::string> votes;
    votes["asd"] = "1";
    votes["qwe"] = "-1";
    votes["yxc"] = "1";
    votes["ccc"] = "2";

    election testee = election::create(0).withSequenceNumber(0);

    testee.addToNextEvaluationGroup("asd");
    testee.addToNextEvaluationGroup("yxc");
    testee.addToNextEvaluationGroup("qwe");
    testee.addToNextEvaluationGroup("cvb");
    testee.addToNextEvaluationGroup("tzu");

    std::vector<std::vector<std::string>> result = testee.getEvaluationGroups();

    std::cout << testee.getEvaluationGroupsAsJson().dump() << std::endl;

    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0].size(), 4);
    EXPECT_EQ(result[1].size(), 1);
    EXPECT_EQ(result[0][0], "asd");
    EXPECT_EQ(result[0][1], "yxc");
    EXPECT_EQ(result[0][2], "qwe");
    EXPECT_EQ(result[0][3], "cvb");
    EXPECT_EQ(result[1][0], "tzu");
    EXPECT_EQ(testee.getEvaluationGroupsAsJson().dump(), "[[\"asd\",\"yxc\",\"qwe\",\"cvb\"],[\"tzu\"]]");
}

TEST(electionTest, test_setResultFromJson) {
    election testee = election::create(0);

    EXPECT_EQ(testee.getElectionResultAsJson().dump(), "[]");
    nlohmann::json resultJson = nlohmann::json::parse("[[0,2],[1,0],[2,3]]");
    testee.setJsonResultToResult(resultJson);

    EXPECT_EQ(testee.getElectionResultAsJson().dump(), "[[0,2],[1,0],[2,3]]");
}

TEST(electionTest, test_setGroupsFromJson) {
    election testee = election::create(0);

    EXPECT_EQ(testee.getEvaluationGroupsAsJson().dump(), "[]");
    nlohmann::json groupsJson = nlohmann::json::parse(R"([["asd","yxc","qwe","cvb"],["tzu"]])");
    testee.setJsonElectionGroupToGroups(groupsJson);

    EXPECT_EQ(testee.getEvaluationGroupsAsJson().dump(), "[[\"asd\",\"yxc\",\"qwe\",\"cvb\"],[\"tzu\"]]");
}

TEST(electionTest, test_count_in_votes) {
    std::map<size_t, std::string> voteOptions;
    voteOptions[0] = "a";
    voteOptions[1] = "b";
    voteOptions[2] = "c";

    election testee = election::create(0).withVoteOptions(voteOptions);
    peer p ;

    std::map<std::string, std::string> identityToVote;

    unsigned char encryptedVoteA[64];
    unsigned char encryptedVoteB[64];

    //p.encryptVote(testee, "1", encryptedVoteA);
    //p.encryptVote(testee, "2", encryptedVoteB);

    identityToVote["asd"] = reinterpret_cast<const char *>(encryptedVoteA);
    identityToVote["yxc"] = reinterpret_cast<const char *>(encryptedVoteB);

    testee.setVotes(identityToVote);

    std::vector<std::string> keys;

    keys.push_back("jKGnnXi/A65qB6qku1xk9Ak=");
    keys.push_back("mj1TsIMQuySKa8ne8YOAaI0=");

    //testee.countInVotesWithKeys(keys);
}