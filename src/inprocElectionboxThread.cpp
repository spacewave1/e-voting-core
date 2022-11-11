//
// Created by wnabo on 15.01.2022.
//

#include "inprocElectionboxThread.h"
#include "electionBuilder.h"
#include "interruptException.h"
#include <zmq.hpp>

void inprocElectionboxThread::InternalThreadEntry() {
    runElectionUpdate();
}

inprocElectionboxThread::inprocElectionboxThread(std::vector<election> &election_box, abstractSocket &abstract_socket)
        : election_box(election_box), abstract_socket(abstract_socket) {}

void inprocElectionboxThread::runElectionUpdate() {
    // Currently expecting pub-sub socket

    try {
        zmq_sleep(1);
        abstract_socket.connect("inproc", "update_elections");

        _logger.log("wait for election", "localhost", "inproc");
        const std::string &received_id_string = abstract_socket.interruptableRecv(is_interrupted);
        const int received_id = std::stoi(received_id_string);

        const std::string &received_seq_string = abstract_socket.interruptableRecv(is_interrupted);
        int received_sequence_id = std::stoi(received_seq_string);

        const std::string &received_setup_time_string = abstract_socket.interruptableRecv(is_interrupted);
        time_t received_setup_time = (unsigned int) std::stoul(received_setup_time_string);

        const std::string &received_options_string = abstract_socket.interruptableRecv(is_interrupted);
        nlohmann::json received_election_options_json = nlohmann::json::parse(received_options_string);

        const std::string &received_votes_string = abstract_socket.interruptableRecv(is_interrupted);
        nlohmann::json received_election_votes_json = nlohmann::json::parse(received_votes_string);

        const std::string &received_election_groups = abstract_socket.interruptableRecv(is_interrupted);
        nlohmann::json received_election_groups_json = nlohmann::json::parse(received_election_groups);

        const std::string &received_election_result = abstract_socket.interruptableRecv(is_interrupted);
        nlohmann::json received_election_result_json = nlohmann::json::parse(received_election_result);

        _logger.log("Received: " + received_id_string, "localhost","inproc");
        _logger.log("Received: " + received_seq_string, "localhost","inproc");
        _logger.log("Received: " + received_setup_time_string, "localhost","inproc");
        _logger.log("Received: " + received_options_string, "localhost","inproc");
        _logger.log("Received: " + received_votes_string, "localhost","inproc");
        _logger.log("Received: " + received_election_groups, "localhost","inproc");
        _logger.log("Received: " + received_election_result, "localhost","inproc");

        auto p_function = [&received_id, &received_setup_time](const election &_election) {
            return _election.getPollId() == received_id && _election.getSetupDate() == received_setup_time;
        };
        auto result = std::find_if(election_box.begin(), election_box.end(), p_function);
        if (result != election_box.end()) {
            result->setJsonOptionsToOptions(received_election_options_json);
            result->setSequenceNumber(received_sequence_id);
            result->setSetupDate(received_setup_time);
            result->setJsonVotesToVotes(received_election_votes_json);
            result->setJsonElectionGroupToGroups(received_election_groups_json);
            result->setJsonResultToResult(received_election_result_json);
        } else {
            election new_election = election::create(received_id)
                    .withVoteOptionsFromJson(received_election_options_json)
                    .withParticipantsVotesFromJson(received_election_votes_json)
                    .withSetupDate(received_setup_time)
                    .withParticipantsFromParticipantVotesKeySet()
                    .withSequenceNumber(received_sequence_id)
                    .withPreparedForDistribution(true)
                    .withElectionGroupsFromJson(received_election_groups_json)
                    .withElectionResultFromJson(received_election_result_json);

            election_box.push_back(new_election);
        }

        abstract_socket.disconnect("inproc", "update_elections");
    } catch (interruptException ex) {
        _logger.log("interrupted","localhost","inproc");
    }
}

inprocElectionboxThread::~inprocElectionboxThread() {

}

void inprocElectionboxThread::interrupt() {
    is_interrupted = true;
}


