//
// Created by wnabo on 25.04.2022.
//

#include "gtest/gtest.h"
#include "../src/inprocElectionboxThread.h"
#include "../src/electionBuilder.h"
#include "../src/peer.h"

TEST(peer, evaluateVote) {

    std::map<size_t, std::string> options;
    options[1] = "A";
    options[2] = "B";
    options[3] = "C";

    std::map<std::string, std::string> votes;
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

    peer testee;
    testee.eval_votes();
}