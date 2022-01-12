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

electionBuilder &electionBuilder::withSequenceNumber(int sequence_number) {
    el.prototype.sequence_number = sequence_number;
    return *this;
}

electionBuilder::~electionBuilder() {
    std::cout << "Calling destructor" << std::endl;
}

