//
// Created by wnabo on 07.01.2022.
//

#include "election.h"
#include "electionBuilder.h"
#include "sodium/utils.h"
#include "hillEncryptionService.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

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

const std::map<std::string, std::string> &election::getParticipantsVotes() const {
    return participants_votes;
}

bool election::placeVote(const std::string identity, std::string chosen_option) {
    if (participants_votes.contains(identity) && participants_votes[identity] != "-1") {
        std::cout << "Vote for the identity (" << identity << ")  has already been placed." << std::endl;
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
                  [&optionIdToVoteCounts](const std::pair<std::string, std::string> &identityToVoteOptionId) {
                      if (identityToVoteOptionId.second != "-1") {
                          //optionIdToVoteCounts[identityToVoteOptionId.second] += 1;
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
                  [](std::pair<std::string, std::string> id_choice_pair) {
                      std::cout << "\t" << id_choice_pair.first << ": " << id_choice_pair.second << std::endl;
                  });

    std::cout << "\tGroups: ";
    std::for_each(evaluation_groups.begin(), evaluation_groups.end(), [](std::vector<std::string> group) {
        std::cout << "[";
        std::for_each(group.begin(), group.end(), [](const std::string& peer){
            std::cout << peer << " ";
        });
        std::cout << "]";
    });
    std::cout << std::endl;

    std::cout << "\tResult: ";
    std::for_each(election_result.begin(), election_result.end(), [](std::pair<size_t, size_t> id_result) {
        std::cout << "[" << id_result.first << ": " << id_result.second << "]";
    });
    std::cout << std::endl;
}

void election::prepareForDistribtion(std::set<std::string> peer_identities) {
    this->participants = peer_identities;
    std::for_each(peer_identities.begin(), peer_identities.end(), [this](std::string peer_identity) {
        this->participants_votes[peer_identity] = "-1";
    });
    std::for_each(prototype.options.begin(), prototype.options.end(), [this](std::pair<size_t, std::string> vote_options) {
        this->election_result[vote_options.first] = 0;
    });
    this->setup_date = time(NULL);
    this->is_prepared_for_distribution = true;
}

void election::setVotes(const std::map<std::string, std::string> &votes) {
    participants_votes = votes;
}

const std::map<std::string, std::string> &election::getVotes() const {
    return participants_votes;
}

nlohmann::json election::getVotesAsJson() const {
    nlohmann::json json = nlohmann::json();
    size_t index = 0;
    std::for_each(participants_votes.begin(), participants_votes.end(),
                  [&json, &index](std::pair<std::string, std::string> identityToVote) {
                      json[index] = {identityToVote.first, identityToVote.second};
                      index++;
                  });
    return json;
}

nlohmann::json election::getElectionOptionsJson() const {
    nlohmann::json json = nlohmann::json::array();
    std::for_each(prototype.options.begin(), prototype.options.end(),
                  [&json](std::pair<size_t, std::string> idToOption) {
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

int election::getId() const {
    return prototype.election_id;
}

void election::setOptions(const std::map<size_t, std::string> &options) {
    election::prototype.options = options;
}

time_t election::getSetupDate() const {
    return setup_date;
}

std::string election::getSetupDateAsString() const {
    char mbstr[100];
    if (std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(&setup_date))) {
        return mbstr;
    }
    return "";
}

void election::setJsonOptionsToOptions(nlohmann::json json) {
    std::map<size_t, std::string> options;
    size_t index = 0;

    if (json.is_array()) {
        std::for_each(json.begin(), json.end(), [&options, &index](const nlohmann::json &option) {
            options[index] = option;
            index++;
        });
    } else if (json.is_object()) {
        for (nlohmann::json::iterator it = json.begin(); it != json.end(); ++it) {
        }
    }
    prototype.options = options;
}

void election::setJsonVotesToVotes(nlohmann::json json) {
    std::map<std::string, std::string> votes;
    if (json.is_array()) {
        std::for_each(json.begin(), json.end(), [&votes](nlohmann::json option) {
            votes[option[0]] = option[1].get<std::string>();
        });
    } else if (json.is_object()) {
        for (nlohmann::json::iterator it = json.begin(); it != json.end(); ++it) {
            votes[it.key()] = it.value();
        }
    }
    participants_votes = votes;
}

election::election(const election &el) : prototype(el.prototype), setup_date(el.setup_date),
                                         participants(el.participants), participants_votes(el.participants_votes),
                                         is_prepared_for_distribution(el.is_prepared_for_distribution), election_result(el.election_result), evaluation_groups(el.evaluation_groups) {
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

size_t election::getNumberOfPlacedVotes() {
    size_t counter = 0;
    std::for_each(participants_votes.begin(), participants_votes.end(),
                  [&counter](
                          const std::pair<std::string, std::string>& optionsToVotes) {
                      if (optionsToVotes.second != "-1") {
                          counter++;
                      }
                  });
    return counter;
}


size_t election::getVotesEvaluatedTotal() {
    std::vector<size_t> election_values;
    std::for_each(election_result.begin(), election_result.end(),
                  [&election_values](std::pair<size_t, size_t> optionVoteCountPair) { election_values.push_back(optionVoteCountPair.second); });

    size_t votesEvaluatedTotal = std::accumulate(election_values.begin(), election_values.end(), 0L);
    return votesEvaluatedTotal;
}

bool election::hasFreeEvaluationGroups() {
    return getNumberOfPlacedVotes() / groupSize - getVotesEvaluatedTotal() / groupSize > 0;
}

const std::map <size_t, size_t> &election::getElectionResult() const {
    return election_result;
}

void election::setElectionResult(const std::map <size_t, size_t> &election_result) {
    election::election_result = election_result;
}

void election::countInVotesWithKeys(std::vector<std::string> keys, hillEncryptionService &encryption_service) {
    std::vector<std::string> votes;
    std::transform(participants_votes.begin(), participants_votes.end(),  std::back_inserter(votes),[](const std::pair<std::string, std::string>& idToVote) -> std::string {
        std::cout << idToVote.first << std::endl;
        std::cout << idToVote.second << std::endl;
        return idToVote.second;
    });
    size_t index = 0;
    std::for_each(votes.begin(), votes.end(),[&keys, &index, encryption_service, this](std::string vote) {
        index++;
        std::for_each(keys.begin(), keys.end(),[vote, encryption_service, this](std::string key) {
            std::cout << "Encrypted vote: " << vote << std::endl;
            std::cout << "key: " << key << std::endl;

            std::string decrypted_vote =  encryption_service.decrypt(vote, key);

            std::cout << "Decrypted: " << decrypted_vote << std::endl;
            int ascii_decimal = encryption_service.mapCharToInt(decrypted_vote[0]);
            if(ascii_decimal < getOptions().size() && ascii_decimal >= 0){
                std::cout << "Count in the vote: " << decrypted_vote.substr(0, log(getOptions().size())) << std::endl;
                std::vector<int> integers = encryption_service.mapStringToNumberSequence(decrypted_vote.substr(0, log(getOptions().size())));
                std::string vote_at_id_str;
                std::transform(integers.begin(), integers.end(), std::back_inserter(vote_at_id_str),[](int number){
                    return (char) number + 48;
                });
                std::cout << "Increment one  for: " << vote_at_id_str << std::endl;
                int vote_at_id = std::stoi(vote_at_id_str);
                election_result.at(vote_at_id) += 1;
            }
        });
    });



    // If vote encrypted with key valid then add to result
    //
}

void election::addToNextEvaluationGroup(std::string identity) {
    // If evaluation groups exists check them first
    size_t index = -1;
    size_t counter = 0;
    std::for_each(evaluation_groups.begin(), evaluation_groups.end(), [this, &index, &counter](const std::vector<std::string> group){
        if(group.size() < groupSize){
            index = counter;
        }
        counter++;
    });

    if(index != -1){
        evaluation_groups[index].push_back(identity);
    } else {
        std::vector<std::string> newGroup;
        newGroup.push_back(identity);
        evaluation_groups.push_back(newGroup);
    }
}

const std::vector<std::vector<std::string>> &election::getEvaluationGroups() const {
    return evaluation_groups;
}

nlohmann::json election::getEvaluationGroupsAsJson() {
    nlohmann::json js = nlohmann::ordered_json(evaluation_groups);
    return js;
}

nlohmann::json election::getElectionResultAsJson() {
    nlohmann::json js = nlohmann::ordered_json(election_result);
    return js;
}

void election::setJsonResultToResult(nlohmann::json jsonResult) {
    std::map<size_t, size_t> votes;
    if (jsonResult.is_array()) {
        std::for_each(jsonResult.begin(), jsonResult.end(), [&votes](nlohmann::json option) {
            votes[option[0]] = option[1].get<size_t>();
        });
    } else {
        std::cout << "could not set result json" << std::endl;
    }
    election_result = votes;
}

void election::setJsonElectionGroupToGroups(nlohmann::json jsonGroups) {
    std::vector<std::vector<std::string>> groups;
    if (jsonGroups.is_array()) {
        std::for_each(jsonGroups.begin(), jsonGroups.end(), [&groups](nlohmann::json groupJson) {
            std::vector<std::string> group = groupJson.get<std::vector<std::string>>();
            groups.push_back(group);
        });
    } else {
        std::cout << "could not set result json" << std::endl;
    }
    evaluation_groups = groups;
}

void election::setEvaluationGroups(const std::vector<std::vector<std::string>> &evaluation_groups) {
    election::evaluation_groups = evaluation_groups;
}

nlohmann::json election::participantsAsJson() {
    nlohmann::json json = nlohmann::json::array();
    size_t index = 0;
    std::for_each(participants.begin(), participants.end(),
                  [&json, &index](const std::string participant) {
                      json[index] = participant;
                      index++;
                  });
    return json;
}

std::ostream &operator<<(std::ostream &os, election &election) {
    os << "prototype: " << election.prototype << " participants: " << election.participantsAsJson().dump() << " options: "
       << election.getElectionOptionsJson().dump()  << " participants_votes: "
       << election.getVotesAsJson().dump() << " groupSize: "
       << election.groupSize << " is_prepared_for_distribution: " << election.is_prepared_for_distribution;
    os << std::endl;
    os << "\tgroups: ";
    std::for_each(election.evaluation_groups.begin(), election.evaluation_groups.end(), [&os](std::vector<std::string> group) {
        os << "[";
        std::for_each(group.begin(), group.end(), [&os](const std::string& peer){
            os << peer << " ";
        });
        os << "]";
    });

    os << std::endl;
    os << "\tResult: ";
    std::for_each(election.election_result.begin(), election.election_result.end(), [&os](std::pair<size_t, size_t> id_result) {
        os << "[" << id_result.first << ": " << id_result.second << "]";
    });
    os << std::endl;
    return os;
}
