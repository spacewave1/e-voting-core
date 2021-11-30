//
// Created by wnabo on 19.11.2021.
//

#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include "straightLineDistributeThread.h"

void straightLineDistributeThread::InternalThreadEntry() {

}

void straightLineDistributeThread::forwardUp() {
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t publish_socket(*context, zmq::socket_type::pub);
    publish_socket.bind("tcp://*:5151");

    publish_socket.send(zmq::message_t(election_snapshot_to_send.getPollId()));
    publish_socket.send(zmq::message_t(election_snapshot_to_send.getSequenceNumber() + 1));
    publish_socket.send(zmq::message_t(election_snapshot_to_send.getJson()));
    //publish_socket.send(zmq::message_t(electionSnapshot.getVotes().));
}

void straightLineDistributeThread::receiveFromUp() {
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t publish_socket(*context, zmq::socket_type::pub);
    publish_socket.bind("tcp://*:5151");

    publish_socket.send(zmq::message_t(election_snapshot_to_send.getPollId()));
    publish_socket.send(zmq::message_t(election_snapshot_to_send.getSequenceNumber() + 1));
    publish_socket.send(zmq::message_t(election_snapshot_to_send.getJson()));
    //publish_socket.send(zmq::message_t(electionSnapshot.getVotes()));
}

void straightLineDistributeThread::receiveFromDownForwardUp() {
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t subscriber(*context, zmq::socket_type::sub);

    subscriber.bind("tcp://" + address_down + ":5151");

    zmq::message_t message_election_id;
    zmq::message_t message_election_sequence;
    zmq::message_t message_election_json;

    subscriber.recv(message_election_id);
    subscriber.recv(message_election_sequence);
    subscriber.recv(message_election_json);

    subscriber.close();

    int election_id = std::stoi(message_election_id.to_string());
    int sequence_id = std::stoi(message_election_sequence.to_string());
    nlohmann::json electionOptionsJson = message_election_json.to_string();

    if (address_up.length() == 0) {
        // Publish down
        zmq::socket_t publish_socket(*context, zmq::socket_type::pub);
        publish_socket.bind("tcp://*:5152");

        publish_socket.send(zmq::message_t(election_id));
        publish_socket.send(zmq::message_t(sequence_id));
        publish_socket.send(zmq::message_t(electionOptionsJson.dump()));
    } else {
        zmq::socket_t publish_socket(*context, zmq::socket_type::pub);
        publish_socket.bind("tcp://*:5151");

        publish_socket.send(zmq::message_t(election_id));
        publish_socket.send(zmq::message_t(sequence_id));
        publish_socket.send(zmq::message_t(electionOptionsJson.dump()));
    }
}

void straightLineDistributeThread::receiveFromUpForwardDown() {
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t subscriber(*context, zmq::socket_type::sub);

    subscriber.bind("tcp://" + address_up + ":5152");

    zmq::message_t message_election_id;
    zmq::message_t message_election_sequence;
    zmq::message_t message_election_json;
    zmq::message_t message_votes_json;

    subscriber.recv(message_election_id);
    subscriber.recv(message_election_sequence);
    subscriber.recv(message_election_json);
    subscriber.recv(message_votes_json);

    subscriber.close();

    int election_id = std::stoi(message_election_id.to_string());
    int sequence_id = std::stoi(message_election_sequence.to_string());
    nlohmann::json election_options_json = message_election_json.to_string();
    nlohmann::json votes_json = message_votes_json.to_string();

    if (address_down.length() == 0) {
    } else {
        zmq::socket_t publish_socket(*context, zmq::socket_type::pub);
        publish_socket.bind("tcp://*:5152");

        publish_socket.send(zmq::message_t(election_id));
        publish_socket.send(zmq::message_t(sequence_id));
        publish_socket.send(zmq::message_t(election_options_json.dump()));
        publish_socket.send(zmq::message_t(votes_json.dump()));
    }
}

void straightLineDistributeThread::setParams(void *arg, std::string address_up, std::string address_down,
                                             const election &election_snapshot) {
    this->arg = arg;
    this->address_down = address_down;
    this->address_up = address_up;
    this->election_snapshot_to_send = election_snapshot;
}

election &straightLineDistributeThread::getElectionSnapshot() const {
    return election_snapshot_to_send;
}

straightLineDistributeThread::straightLineDistributeThread(election &election_snapshot)
        : election_snapshot_to_send(election_snapshot) {
    this->election_snapshot_to_send = election_snapshot;
}

straightLineDistributeThread::straightLineDistributeThread() {

}
