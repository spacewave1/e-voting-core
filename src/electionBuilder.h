//
// Created by wnabo on 07.01.2022.
//

#ifndef VOTE_P2P_ELECTIONBUILDER_H
#define VOTE_P2P_ELECTIONBUILDER_H

#pragma once

#include "election.h"
#include <nlohmann/json.hpp>

class electionBuilder {
private:
    election el;
public:
    electionBuilder(int id) : el(id) {}
    electionBuilder &withParticipants(std::set<std::string>& participants);
    electionBuilder &withParticipantsVotes(std::map<std::string, std::string>& participants_votes);
    electionBuilder &withVoteOptions(std::map<size_t, std::string> options);
    electionBuilder &withSequenceNumber(int sequence_number);
    electionBuilder &withParticipantsVotesFromJson(nlohmann::json jsonVotes);
    electionBuilder &withVoteOptionsFromJson(nlohmann::json jsonOptions);
    electionBuilder &withParticipantsFromParticipantVotesKeySet();
    electionBuilder &withSetupDate(time_t setupDate);
    electionBuilder &withPreparedForDistribution(bool isPrepared);
    electionBuilder &withElectionResult(std::map<size_t, size_t> result);
    electionBuilder &withElectionGroupsFromJson(nlohmann::json json);
    electionBuilder &withElectionResultFromJson(nlohmann::json json);
    electionBuilder &withEvalGroups(std::vector<std::vector<std::string>> eval_groups);


    virtual ~electionBuilder();

    operator election() const { return std::move(el); }

};

#endif //VOTE_P2P_ELECTIONBUILDER_H
