//
// Created by wnabo on 07.01.2022.
//

#include "electionBuilder.h"

electionBuilder& electionBuilder::withParticipants(std::set<std::string> participants){
    el.participants = participants;
    return *this;
};

electionBuilder& electionBuilder::withParticipantsVotes(std::map<std::string, int> participants_votes){
    el.participants_votes = participants_votes;
    return *this;
}

