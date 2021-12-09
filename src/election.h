//
// Created by wnabo on 19.11.2021.
//

#ifndef VOTE_P2P_ELECTION_H
#define VOTE_P2P_ELECTION_H

#include <nlohmann/json.hpp>
#include <string>
#include <iostream>

class election {
private:
    int election_id;
    size_t sequence_number;
    std::map<std::string, int> votes;
    std::string json;

public:
    election(int election_id, size_t sequence_number, const std::map<std::string, int> &votes, const std::string &json)
            : election_id(election_id), sequence_number(sequence_number), votes(votes), json(json) {}


    election() {}

    int getPollId() const {
        return election_id;
    }

    void setPollId(int poll_id) {
        election::election_id = poll_id;
    }

    size_t getSequenceNumber() const {
        return sequence_number;
    }

    void setSequenceNumber(size_t sequence_number) {
        election::sequence_number = sequence_number;
    }

    const std::string &getJson() const {
        return json;
    }

    void setJson(const std::string &json) {
        election::json = json;
    }

    const std::map<std::string, int> &getVotes() const {
        return votes;
    }

    void setVotes(const std::map<std::string, int> &votes) {
        election::votes = votes;
    }

    void placeVote(const std::string identity, int chosen_option){
        if(!votes.contains(identity)) {
           // std::cout << "Has placed vote on " << chosen_option << std::endl;
            votes[identity] = chosen_option;
        } else {
            std::cout << "" << std::endl;
        }

        nlohmann::json js = nlohmann::json::parse(json);
        std::for_each(votes.begin(), votes.end(), [&js](const std::pair<std::string, int> &id_option_pair) {
            std::cout << id_option_pair.first << ": " << id_option_pair.second << js[id_option_pair.second] << std::endl;
        });
    }

    void print() {
        std::cout << "id: " << election_id << std::endl;
        std::cout << "sequence: " << sequence_number << std::endl;
        std::cout << json << std::endl;
        std::for_each(votes.begin(), votes.end(), [](std::pair<std::string, int> id_choice_pair){
            std::cout << id_choice_pair.first << ": " << id_choice_pair.second << std::endl;
        });
    }
};


#endif //VOTE_P2P_ELECTION_H
