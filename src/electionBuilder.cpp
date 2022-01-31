//
// Created by wnabo on 07.01.2022.
//

#include "electionBuilder.h"

electionBuilder& electionBuilder::withParticipants(std::set<std::string>& participants){
    el.participants = participants;
    return *this;
};

electionBuilder& electionBuilder::withParticipantsVotes(std::map<std::string, int>& participants_votes){
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
    std::cout << "Calling destructor" << std::endl;
}

electionBuilder &electionBuilder::withParticipantsFromParticipantVotesKeySet() {
    std::set<std::string> keys;
    std::for_each(el.participants_votes.begin(), el.participants_votes.end(),[&keys](std::pair<std::string, int> idToVoteOptions){
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

