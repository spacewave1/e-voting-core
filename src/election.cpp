//
// Created by wnabo on 07.01.2022.
//

#include "election.h"
#include "electionBuilder.h"

election::election(const int id) : prototype(
        electionPrototype(id)) {
    //std::cout << "Constructor with int called" << std::endl;
}

election::election() {
    //std::cout << "Empty constructor called" << std::endl;
}

electionBuilder election::create(int id) {
    return electionBuilder(id);
}

const std::map<std::string, int> &election::getParticipantsVotes() const {
    return participants_votes;
}

bool election::placeVote(const std::string identity, int chosen_option) {
    if (participants_votes.contains(identity) && participants_votes[identity] != -1) {
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

nlohmann::json election::getCurrentElectionStatisticAsJson() {
    nlohmann::json js;
    std::map<size_t, size_t> optionIdToVoteCounts;

    std::for_each(prototype.options.begin(), prototype.options.end(),
                  [&optionIdToVoteCounts](const std::pair<size_t, std::string> &idToOptionName) {
                      optionIdToVoteCounts[idToOptionName.first] = 0;
                  });

    std::for_each(participants_votes.begin(), participants_votes.end(),
                  [&optionIdToVoteCounts](const std::pair<std::string, int> &identityToVoteOptionId) {
                      if (identityToVoteOptionId.second != -1) {
                          optionIdToVoteCounts[identityToVoteOptionId.second] += 1;
                      }
                  });
    std::map<size_t, std::string> &p = prototype.options;

    std::for_each(optionIdToVoteCounts.begin(), optionIdToVoteCounts.end(),
                  [&js, &p](const std::pair<size_t, size_t> &idToCount) {
                      js[idToCount.first]["name"] = p[idToCount.first];
                      js[idToCount.first]["count"] = idToCount.second;
                  });

    return js;
}


nlohmann::json election::participantVotesAsJson() {
    nlohmann::json js = nlohmann::ordered_json(participants_votes);
    return js;
}

void election::print() {
    std::cout << std::endl;
    std::cout << "\tid: " << prototype.election_id << std::endl;
    std::cout << "\tsequence: " << prototype.sequence_number << std::endl;
    std::cout << "\tsetup_date: " << setup_date << std::endl;
    std::for_each(prototype.options.begin(), prototype.options.end(),
                  [](std::pair<size_t, std::string> id_option_pair) {
                      std::cout << "\t" << id_option_pair.first << ": " << id_option_pair.second << std::endl;
                  });
    std::for_each(participants_votes.begin(), participants_votes.end(),
                  [](std::pair<std::string, int> id_choice_pair) {
                      std::cout << "\t" << id_choice_pair.first << ": " << id_choice_pair.second << std::endl;
                  });
    std::cout << std::endl;
}

void election::prepareForDistribtion(std::set<std::string> peer_identities) {
    this->participants = peer_identities;
    std::for_each(peer_identities.begin(), peer_identities.end(), [this](std::string peer_identity) {
        this->participants_votes[peer_identity] = -1;
    });
    this->setup_date = time(NULL);
    this->is_prepared_for_distribution = true;
}

void election::setVotes(const std::map<std::string, int> &votes) {
    participants_votes = votes;
}

const std::map<std::string, int> &election::getVotes() const {
    return participants_votes;
}

nlohmann::json election::getVotesAsJson() const {
    nlohmann::json json = nlohmann::json();
    size_t index = 0;
    std::for_each(participants_votes.begin(), participants_votes.end(), [&json, &index](std::pair<std::string, int> identityToVote){
        json[index] = {identityToVote.first, identityToVote.second};
        index++;
    });
    return json;
}

nlohmann::json election::getElectionOptionsJson() const {
    nlohmann::json json = nlohmann::json::array();
    std::for_each(prototype.options.begin(), prototype.options.end(), [&json](std::pair<size_t, std::string> idToOption){
        json[idToOption.first] = idToOption.second;
    });
    // TODO: Do something about this
    return json;
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

void election::setOptions(const std::map<size_t, std::string> &options) {
    election::prototype.options = options;
}

time_t election::getSetupDate() const {
    return setup_date;
}

void election::setJsonOptionsToOptions(nlohmann::json json) {
    std::map<size_t, std::string> options;
    size_t index = 0;

    if (json.is_array()) {
        std::for_each(json.begin(), json.end(), [&options, &index](const nlohmann::json& option) {
            options[index] = option;
            index++;
        });
    } else if(json.is_object()) {
        for (nlohmann::json::iterator it = json.begin(); it != json.end(); ++it) {
        }
    }
    prototype.options = options;
}

void election::setJsonVotesToVotes(nlohmann::json json) {
    std::map<std::string, int> votes;
    if (json.is_array()) {
        std::for_each(json.begin(), json.end(), [&votes](nlohmann::json option) {
            votes[option[0]] = option[1].get<int>();
        });
    } else if(json.is_object()) {
        for (nlohmann::json::iterator it = json.begin(); it != json.end(); ++it) {
            votes[it.key()] = it.value();
        }
    }
    participants_votes = votes;
}

election::election(const election& el) : prototype(el.prototype), setup_date(el.setup_date), participants(el.participants), participants_votes(el.participants_votes), is_prepared_for_distribution(el.is_prepared_for_distribution){
}

const std::map<size_t, std::string> &election::getOptions() const {
    return prototype.options;
}

bool election::isPreparedForDistribution() const {
    return is_prepared_for_distribution;
}

void election::setSetupDate(time_t setupDate) {
    setup_date = setupDate;
}
