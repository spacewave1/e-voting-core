//
// Created by wnabo on 15.01.2022.
//

#include "inprocElectionboxThread.h"
#include "electionBuilder.h"
#include <zmq.hpp>

void inprocElectionboxThread::InternalThreadEntry() {
    runElectionUpdate();
}

inprocElectionboxThread::inprocElectionboxThread(std::vector<election> &election_box, abstractSocket &abstract_socket)
        : election_box(election_box), abstract_socket(abstract_socket) {}

void inprocElectionboxThread::runElectionUpdate() {
    // Currently expecting pub-sub socket
    abstract_socket.connect("inproc", "update_elections");

    const std::string &received_id_string = abstract_socket.recv();
    const int received_id = std::stoi(received_id_string);

    const std::string &received_seq_string = abstract_socket.recv();
    int received_sequence_id = std::stoi(received_seq_string);

    const std::string &received_setup_time_string = abstract_socket.recv();
    time_t received_setup_time = (unsigned int) std::stoul(received_setup_time_string);

    const std::string &received_options_string = abstract_socket.recv();
    nlohmann::json received_election_options_json = nlohmann::json::parse(received_options_string);

    const std::string &received_votes_string = abstract_socket.recv();
    nlohmann::json received_election_votes_json = nlohmann::json::parse(received_votes_string);

    _logger.log("Received: " + received_id_string, "inproc");
    _logger.log("Received: " + received_seq_string, "inproc");
    _logger.log("Received: " + received_setup_time_string, "inproc");
    _logger.log("Received: " + received_options_string, "inproc");
    _logger.log("Received: " + received_votes_string, "inproc");

    auto p_function = [&received_id, &received_setup_time](const election &_election) {
        return _election.getPollId() == received_id && _election.getSetupDate() == received_setup_time;
    };
    auto result = std::find_if(election_box.begin(), election_box.end(), p_function);
    if (result != election_box.end()) {
        result->setJsonOptionsToOptions(received_election_options_json);
        result->setSequenceNumber(received_sequence_id);
        result->setSetupDate(received_setup_time);
        result->setJsonVotesToVotes(received_election_votes_json);
    } else {
        election new_election = election::create(received_id)
                .withVoteOptionsFromJson(received_election_options_json)
                .withParticipantsVotesFromJson(received_election_votes_json)
                .withSetupDate(received_setup_time)
                .withParticipantsFromParticipantVotesKeySet()
                .withSequenceNumber(received_sequence_id)
                .withPreparedForDistribution(true);
        election_box.push_back(new_election);
    }
}

inprocElectionboxThread::~inprocElectionboxThread() {

}


