//
// Created by wnabo on 07.01.2022.
//

#ifndef VOTE_P2P_ELECTIONPROTOTYPE_H
#define VOTE_P2P_ELECTIONPROTOTYPE_H

struct electionPrototype {
    electionPrototype() {};

    electionPrototype(int id, std::size_t sequence_number, const std::string options_json) {
        this->election_id = id;
        this->sequence_number = sequence_number;
        this->election_options_json = options_json;
    }

    int election_id;
    size_t sequence_number;
    std::string election_options_json;
};

#endif //VOTE_P2P_ELECTIONPROTOTYPE_H
