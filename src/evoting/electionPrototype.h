//
// Created by wnabo on 07.01.2022.
//


#ifndef VOTE_P2P_ELECTIONPROTOTYPE_H
#define VOTE_P2P_ELECTIONPROTOTYPE_H

#include <ostream>

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

    friend std::ostream &operator<<(std::ostream &os, const electionPrototype &prototype) {
        os << "election_id: " << prototype.election_id << " sequence_number: " << prototype.sequence_number;
        return os;
    }
};

#endif //VOTE_P2P_ELECTIONPROTOTYPE_H
