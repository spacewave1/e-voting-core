//
// Created by wnabo on 25.04.2022.
//

#include <queue>
#include "gtest/gtest.h"
#include "../src/inprocElectionboxThread.h"
#include "../src/electionBuilder.h"
#include "../src/peer.h"

TEST(peer, generate_participant_keys) {

    std::map<size_t, std::string> options;
    options[1] = "A";
    options[2] = "B";
    options[3] = "C";

    std::vector<std::string> election_group;
    election_group.push_back("asd");
    election_group.push_back("qwe");
    election_group.push_back("yxc");
    election_group.push_back("qwy");

    std::vector<std::vector<std::string>> election_groups;
    election_groups.push_back(election_group);

    election election_before_update = election::create(1)
            .withSetupDate(time(NULL))
            .withVoteOptions(options)
            .withEvalGroups(election_groups);

    std::vector<election> electionBox;
    electionBox.push_back(election_before_update);

    unsigned char encryptedVote[64];

    peer testee;
    testee.pushBackElection(election_before_update);
    testee.setIdentity("asd");
    //testee.encryptVote(election_before_update,"1",encryptedVote);
    //testee.generate_keys(0);
}

TEST(peer, test_deque) {
    std::map<size_t, std::queue<std::string>> elMapDeq;

    std::queue<std::string> deq;
    deq.push("asd");
    deq.push("yxc");
    deq.push("wer");

    elMapDeq[1] = deq;

    std::cout << elMapDeq.at(1).front() << std::endl;
    elMapDeq[1].pop();
    std::cout << elMapDeq.at(1).front() << std::endl;
    elMapDeq[1].pop();
    std::cout << elMapDeq.at(1).front() << std::endl;

}