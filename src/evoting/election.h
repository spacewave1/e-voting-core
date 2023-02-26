//
// Created by wnabo on 19.11.2021.
//

#ifndef VOTE_P2P_ELECTION_H
#define VOTE_P2P_ELECTION_H

#include <nlohmann/json.hpp>
#include <string>
#include <set>
#include <iostream>
#include "electionPrototype.h"
#include "hillEncryptionService.h"

class electionBuilder;

class election {

private:
    electionPrototype prototype;
    std::time_t setup_date;
    std::set<std::string> participants;
    std::vector<std::vector<std::string>> evaluation_groups;
    std::map<std::string, std::string> participants_votes;
    std::map<size_t, size_t> election_result;
    size_t groupSize = 4;
    bool is_prepared_for_distribution = false;
    election(const int id);

public:
    friend class electionBuilder;
    static electionBuilder create(int id);
    election(const election& el);
    election();
    const std::map<std::string, std::string> &getParticipantsVotes() const;
    const std::map<size_t, std::string> &getOptions() const;
    int getId() const;
    void setPollId(int poll_id);
    time_t getSetupDate() const;
    size_t getSequenceNumber() const;
    void setSequenceNumber(size_t sequence_number);
    nlohmann::json getElectionOptionsJson() const;
    void setOptions(const std::map<size_t, std::string> &options);
    const std::map<std::string, std::string> &getVotes() const;
    void setVotes(const std::map<std::string, std::string> &votes);
    bool placeVote(const std::string identity, std::string chosen_option);
    nlohmann::json participantVotesAsJson();
    nlohmann::json participantsAsJson();
    void prepareForDistribtion(std::set<std::string> peer_identities);
    void print();
    void setJsonOptionsToOptions(nlohmann::json json);
    void setJsonVotesToVotes(nlohmann::json json);
    nlohmann::json getCurrentElectionStatisticAsJson();
    bool isPreparedForDistribution() const;
    nlohmann::json getVotesAsJson() const;
    void setSetupDate(time_t setupDate);

    const std::vector<std::vector<std::string>> &getEvaluationGroups() const;

    const std::map <size_t, size_t> &getElectionResult() const;
    void setElectionResult(const std::map <size_t, size_t> &election_result);
    std::string getSetupDateAsString() const;
    bool hasFreeEvaluationGroups();
    void addToNextEvaluationGroup(std::string identity);

    size_t getVotesEvaluatedTotal();
    size_t getNumberOfPlacedVotes();

    nlohmann::json getEvaluationGroupsAsJson();

    nlohmann::json getElectionResultAsJson();

    void setJsonResultToResult(nlohmann::json jsonResult);

    void setJsonElectionGroupToGroups(nlohmann::json jsonGroups);

    void setEvaluationGroups(const std::vector<std::vector<std::string>> &evaluation_groups);

    void countInVotesWithKeys(std::vector<std::string> keys, hillEncryptionService &encryption_service);

    friend std::ostream &operator<<(std::ostream &os, election &election);
};


#endif //VOTE_P2P_ELECTION_H
