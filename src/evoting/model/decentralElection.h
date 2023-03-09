//
// Created by wld on 09.03.23.
//

#ifndef E_VOTING_DECENTRALELECTION_H
#define E_VOTING_DECENTRALELECTION_H

#include "election.h"

class decentralElection {

private:
    electionPrototype prototype;
    std::time_t setup_date;
    std::set<std::string> participants;
    std::vector<std::vector<std::string>> evaluation_groups;
    std::map<std::string, std::string> participants_votes;
    std::map<size_t, size_t> election_result;
    bool is_prepared_for_distribution = false;
    decentralElection(const std::string nonce);
};


#endif //E_VOTING_DECENTRALELECTION_H
