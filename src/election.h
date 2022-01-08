//
// Created by wnabo on 19.11.2021.
//

#ifndef VOTE_P2P_ELECTION_H
#define VOTE_P2P_ELECTION_H

#include <nlohmann/json.hpp>
#include <string>
#include <set>
#include <iostream>
#include <chrono>
#include <utility>
#include "electionPrototype.h"

class electionBuilder;

class election {
    election(int id) : prototype(id,0,"") {}

private:
    electionPrototype prototype;
    std::time_t setupDate;
    std::set<std::string> participants;
    std::map<std::string, int> participants_votes;
    election(int id, size_t sequence_id, nlohmann::json options);

public:
    friend class electionBuilder;
    static electionBuilder create(int id);

    election(int election_id, size_t sequence_number, const std::map<std::string, int> &votes,
             const std::set<std::string> &participants, const std::string &election_options_json);
    election();

    const std::map<std::string, int> &getParticipantsVotes() const;
    int getPollId() const;
    void setPollId(int poll_id);

    time_t getSetupDate() const;
    size_t getSequenceNumber() const;
    void setSequenceNumber(size_t sequence_number);
    const std::string &getElectionOptionsJson() const;
    void setJson(const std::string &json);
    const std::map<std::string, int> &getVotes() const;
    void setVotes(const std::map<std::string, int> &votes);
    bool placeVote(const std::string identity, int chosen_option);
    nlohmann::json participantVotesAsJson();
    void prepareForDistribtion(std::set<std::string> peer_identities);
    void print();
};


#endif //VOTE_P2P_ELECTION_H
