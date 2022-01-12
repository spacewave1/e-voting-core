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
#include "electionPrototype.h"

class electionBuilder;

class election {

private:
    electionPrototype prototype;
    std::time_t setupDate;
    std::set<std::string> participants;
    std::map<std::string, int> participants_votes;
    election(const int id);

public:
    friend class electionBuilder;
    static electionBuilder create(int id);

    election(const election& el);
    election();

    const std::map<std::string, int> &getParticipantsVotes() const;
    int getPollId() const;
    void setPollId(int poll_id);

    time_t getSetupDate() const;
    size_t getSequenceNumber() const;
    void setSequenceNumber(size_t sequence_number);
    nlohmann::json getElectionOptionsJson() const;
    void setOptions(const std::map<size_t, std::string> &options);
    const std::map<std::string, int> &getVotes() const;
    void setVotes(const std::map<std::string, int> &votes);
    bool placeVote(const std::string identity, int chosen_option);
    nlohmann::json participantVotesAsJson();
    void prepareForDistribtion(std::set<std::string> peer_identities);
    void print();

    void setJsonOptionsToOptions(nlohmann::json json);

    void setJsonVotesToVotes(nlohmann::json json);

    nlohmann::json getCurrentElectionStatisticAsJson();

    nlohmann::json getVotesAsJson() const;
};


#endif //VOTE_P2P_ELECTION_H
