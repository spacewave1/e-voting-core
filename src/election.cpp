//
// Created by wnabo on 07.01.2022.
//

#include "election.h"
#include "electionBuilder.h"

election::election(int election_id, size_t sequence_number, const std::map<std::string, int> &votes,
                   const std::set<std::string> &participants, const std::string &election_options_json)
        : prototype(electionPrototype(election_id, sequence_number, election_options_json)),
          participants_votes(votes), participants(participants) {}

election::election(int id, size_t sequence_id, nlohmann::json options) : prototype(
        electionPrototype(id, sequence_id, options)) {

}

election::election() {

}

electionBuilder election::create(int id) {
    return electionBuilder(id);
}

const std::map<std::string, int> &election::getParticipantsVotes() const {
    return participants_votes;
}

bool election::placeVote(const std::string identity, int chosen_option) {
    if (participants_votes.contains(identity)) {
        std::cout << "Vote for the identity (" << identity << ") has already been placed." << std::endl;
        return false;
    } else if (!participants.contains(identity)) {
        std::cout << "The requesting identity (" << identity << ") is not on the list of participants" << std::endl;
        return false;
    } else {
        // std::cout << "Has placed vote on " << chosen_option << std::endl;
        participants_votes[identity] = chosen_option;
        return true;
    }
}


nlohmann::json election::participantVotesAsJson() {
    nlohmann::json js = nlohmann::json::parse(prototype.election_options_json);
    std::for_each(participants_votes.begin(), participants_votes.end(),
                  [&js](const std::pair<std::string, int> &id_option_pair) {
                      std::cout << id_option_pair.first << ": " << id_option_pair.second
                                << js[id_option_pair.second] << std::endl;
                  });

    return js;
}

void election::print() {
    std::cout << "id: " << prototype.election_id << std::endl;
    std::cout << "sequence: " << prototype.sequence_number << std::endl;
    std::cout << prototype.election_options_json << std::endl;
    std::for_each(participants_votes.begin(), participants_votes.end(),
                  [](std::pair<std::string, int> id_choice_pair) {
                      std::cout << id_choice_pair.first << ": " << id_choice_pair.second << std::endl;
                  });
    std::cout << std::endl;
}

void election::prepareForDistribtion(std::set<std::string> peer_identities) {
    this->participants = peer_identities;
    std::for_each(peer_identities.begin(), peer_identities.end(), [this](std::string peer_identity) {
        this->participants_votes[peer_identity] = 0;
    });
    time_t timer = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    this->setupDate = timer;
}

void election::setVotes(const std::map<std::string, int> &votes) {
    election::participants_votes = votes;
}

const std::map<std::string, int> &election::getVotes() const {
    return participants_votes;
}

const std::string &election::getElectionOptionsJson() const {
    return prototype.election_options_json;
}

void election::setSequenceNumber(size_t sequence_number) {
    election::prototype.sequence_number = sequence_number;
}

size_t election::getSequenceNumber() const {
    return prototype.sequence_number;
}

void election::setPollId(int poll_id) {
    election::prototype.election_id = poll_id;
}

int election::getPollId() const {
    return prototype.election_id;
}

void election::setJson(const std::string &json) {
    election::prototype.election_options_json = json;
}

time_t election::getSetupDate() const {
    return setupDate;
}

