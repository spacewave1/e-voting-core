//
// Created by wnabo on 07.01.2022.
//


#ifndef VOTE_P2P_ELECTIONPROTOTYPE_H
#define VOTE_P2P_ELECTIONPROTOTYPE_H

struct electionPrototype {
    electionPrototype() {};

    electionPrototype(int id) {
        this->election_id = id;
        this->sequence_number = 0;
        this->options = std::map<size_t, std::string>();
    }

    int election_id;
    size_t sequence_number{};
    std::map<size_t, std::string> options{};
};

#endif //VOTE_P2P_ELECTIONPROTOTYPE_H
