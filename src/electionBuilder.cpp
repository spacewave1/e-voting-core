//
// Created by wnabo on 07.01.2022.
//

#include "electionBuilder.h"

electionBuilder& electionBuilder::withParticipants(std::set<std::string>& participants){
    el.participants = participants;
    return *this;
};

electionBuilder& electionBuilder::withParticipantsVotes(std::map<std::string, std::string>& participants_votes){
    el.participants_votes = participants_votes;
    return *this;
}

electionBuilder &electionBuilder::withVoteOptions(std::map<size_t, std::string> options) {
    el.prototype.options = options;
    return *this;
}

electionBuilder &electionBuilder::withVoteOptionsFromJson(nlohmann::json jsonOptions) {
    el.setJsonOptionsToOptions(jsonOptions);
    return *this;
}

electionBuilder &electionBuilder::withParticipantsVotesFromJson(nlohmann::json jsonVotes) {
    el.setJsonVotesToVotes(jsonVotes);
    return *this;
}

electionBuilder &electionBuilder::withSequenceNumber(int sequence_number) {
    el.prototype.sequence_number = sequence_number;
    return *this;
}

electionBuilder::~electionBuilder() {
}

electionBuilder &electionBuilder::withParticipantsFromParticipantVotesKeySet() {
    std::set<std::string> keys;
    std::for_each(el.participants_votes.begin(), el.participants_votes.end(),[&keys](std::pair<std::string, std::string> idToVoteOptions){
        keys.insert(idToVoteOptions.first);
    });
    el.participants = keys;
    return *this;
}

electionBuilder &electionBuilder::withPreparedForDistribution(bool isPrepared) {
    el.is_prepared_for_distribution = isPrepared;
    return *this;
}

electionBuilder &electionBuilder::withSetupDate(time_t setupDate) {
    el.setup_date = setupDate;
    return *this;
}

electionBuilder &electionBuilder::withElectionResult(std::map <size_t, size_t> result) {
    el.election_result = result;
    return *this;
}

electionBuilder &electionBuilder::withElectionGroupsFromJson(nlohmann::json json) {
    el.setJsonElectionGroupToGroups(json);
    return *this;
}

electionBuilder &electionBuilder::withElectionResultFromJson(nlohmann::json json) {
    el.setJsonResultToResult(json);
    return *this;
}

electionBuilder &electionBuilder::withEvalGroups(std::vector<std::vector<std::string>> eval_groups) {
    el.setEvaluationGroups(eval_groups);
    return *this;
}

