//
// Created by wnabo on 19.11.2021.
//

#include <nlohmann/json.hpp>
#include "straightLineDistributeThread.h"

void straightLineDistributeThread::InternalThreadEntry() {
    std::cout << "Execute thread" << std::endl;
    if (is_initial_requester) {
        sendInitialRequest();
    } else {
        receiveRequest();
    }


    if (this->address_down.empty() && !this->address_up.empty()) {
        forwardUp();
    } else if (!this->address_down.empty() && this->address_up.empty()) {
        forwardDown();
    } else if (!this->address_down.empty() && !this->address_up.empty()) {
        receiveFromDownForwardUp();
        receiveFromUpForwardDown();
    }

    //If Distribute -> close current receive and
    forwardUp();
    //Else if Receive and lowest
}

void straightLineDistributeThread::sendInitialRequest() {
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t request_socket(*context, zmq::socket_type::req);

    nlohmann::json sendJson;
    sendJson["originPosition"] = nodePosition;
    sendJson["subscribePort"] = 5050;

    if (!address_up.empty()) {
        request_socket.connect("tcp://" + address_up + ":5049");
        request_socket.send(zmq::buffer(sendJson.dump()));

        zmq::message_t reply;
        request_socket.recv(reply);
    }

    if (!address_down.empty()) {
        request_socket.connect("tcp://" + address_down + ":5049");
        request_socket.send(zmq::buffer(sendJson.dump()));

        zmq::message_t reply;
        request_socket.recv(reply);
        std::cout << reply.to_string() << std::endl;
        request_socket.close();
    }

    publish_socket = zmq::socket_t(*context, zmq::socket_type::pub);
    publish_socket.bind("tcp://*:5050");

    std::cout << "Has setup Distribution" << std::endl;
}

void straightLineDistributeThread::receiveRequest() {
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t receive_request_socket(*context, zmq::socket_type::rep);

    receive_request_socket.bind("tcp://*:5049");

    zmq::message_t request;
    receive_request_socket.recv(request);
    nlohmann::json receiveJson = nlohmann::json::parse(request.to_string());

    std::cout << "Received From: " << std::string(request.gets("Peer-Address")) << std::endl;
    std::cout << "Received Json: " << receiveJson.dump() << std::endl;

    receive_request_socket.send(zmq::buffer("accept"));
    receive_request_socket.close();

    setupDistribution((zmq::message_t &) request, receiveJson);
    std::cout << "Has setup Distribution" << std::endl;
}

void straightLineDistributeThread::setupDistribution(zmq::message_t &request, nlohmann::json receiveJson) {
    auto *context = (zmq::context_t *) arg;
    size_t originPosition = std::stoi(receiveJson["originPosition"].dump());
    size_t subscribePort = std::stoi(receiveJson["subscribePort"].dump());

    if (originPosition < nodePosition) {
        std::string peerAddress = std::string(request.gets("Peer-Address"));

        subscribe_socket = zmq::socket_t(*context, zmq::socket_type::sub);
        subscribe_socket.connect("tcp://" + peerAddress + ":" + std::to_string(subscribePort));

        if (!address_down.empty()) {
            zmq::socket_t forward_request(*context, zmq::socket_type::req);
            size_t publishPort = (subscribePort - 5050 + 1) % 2 + 5050;
            forward_request.connect("tcp://" + address_down + ":5049");
            forward_request.send(zmq::buffer(receiveJson.dump()));

            zmq::message_t reply;
            forward_request.recv(reply);
            std::cout << reply.to_string() << std::endl;
            forward_request.close();

            publish_socket.bind("tcp://*:" + std::to_string(publishPort));
        }
    } else if (originPosition > nodePosition) {
        std::string peerAddress = std::string(request.gets("Peer-Address"));

        subscribe_socket = zmq::socket_t(*context, zmq::socket_type::sub);
        subscribe_socket.connect("tcp://" + peerAddress + ":" + std::to_string(subscribePort));

        if (!address_up.empty()) {
            zmq::socket_t forward_request(*context, zmq::socket_type::req);
            size_t publishPort = (subscribePort - 5050 + 1) % 2 + 5050;
            forward_request.connect("tcp://" + address_up + ":5049");
            forward_request.send(zmq::buffer(receiveJson.dump()));

            zmq::message_t reply;
            forward_request.recv(reply);
            std::cout << reply.to_string() << std::endl;
            forward_request.close();

            publish_socket.bind("tcp://*:" + std::to_string(publishPort));
        }
    }
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
    publish_socket.bind("tcp://*:5152");

    publish_socket.send(zmq::message_t(election_snapshot_to_send.getPollId()));
    publish_socket.send(zmq::message_t(election_snapshot_to_send.getSequenceNumber() + 1));
    publish_socket.send(zmq::message_t(election_snapshot_to_send.getJson()));
    //publish_socket.send(zmq::message_t(electionSnapshot.getVotes()));
}

void straightLineDistributeThread::receiveFromDownForwardUp() {
    auto *context = (zmq::context_t *) arg;

    zmq::message_t message_election_id;
    zmq::message_t message_election_sequence;
    zmq::message_t message_election_json;

    subscribe_socket.recv(message_election_id);
    subscribe_socket.recv(message_election_sequence);
    subscribe_socket.recv(message_election_json);

    subscribe_socket.close();

    int election_id = std::stoi(message_election_id.to_string());
    int sequence_id = std::stoi(message_election_sequence.to_string());
    nlohmann::json electionOptionsJson = message_election_json.to_string();

    if (address_up.length() == 0) {
    } else {
        publish_socket.send(zmq::message_t(election_id));
        publish_socket.send(zmq::message_t(sequence_id));
        publish_socket.send(zmq::message_t(electionOptionsJson.dump()));
    }

    // Start receiving from Up
}

void straightLineDistributeThread::forwardDown() {
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t publish_socket(*context, zmq::socket_type::pub);
    publish_socket.bind("tcp://*:5152");

    publish_socket.send(zmq::message_t(election_snapshot_to_send.getPollId()));
    publish_socket.send(zmq::message_t(election_snapshot_to_send.getSequenceNumber() + 1));
    publish_socket.send(zmq::message_t(election_snapshot_to_send.getJson()));
    //publish_socket.send(zmq::message_t(electionSnapshot.getVotes().));
}

void straightLineDistributeThread::receiveFromUpForwardDown() {
    auto *context = (zmq::context_t *) arg;

    zmq::message_t message_election_id;
    zmq::message_t message_election_sequence;
    zmq::message_t message_election_json;
    zmq::message_t message_votes_json;

    subscribe_socket.recv(message_election_id);
    subscribe_socket.recv(message_election_sequence);
    subscribe_socket.recv(message_election_json);
    subscribe_socket.recv(message_votes_json);

    int election_id = std::stoi(message_election_id.to_string());
    int sequence_id = std::stoi(message_election_sequence.to_string());
    nlohmann::json election_options_json = message_election_json.to_string();
    nlohmann::json votes_json = message_votes_json.to_string();

    if (address_down.length() == 0) {
    } else {
        publish_socket.send(zmq::message_t(election_id));
        publish_socket.send(zmq::message_t(sequence_id));
        publish_socket.send(zmq::message_t(election_options_json.dump()));
        publish_socket.send(zmq::message_t(votes_json.dump()));
    }
}

void straightLineDistributeThread::setParams(void *arg, std::string address_up, std::string address_down,
                                             const size_t nodePosition, const election &election_snapshot) {
    this->arg = arg;
    this->address_down = address_down;
    this->address_up = address_up;
    this->election_snapshot_to_send = election_snapshot;
    this->nodePosition = nodePosition;
}

election straightLineDistributeThread::getElectionSnapshot() const {
    return election_snapshot_to_send;
}

straightLineDistributeThread::straightLineDistributeThread(election &election_snapshot)
        : election_snapshot_to_send(election_snapshot) {
    this->election_snapshot_to_send = election_snapshot;
}

straightLineDistributeThread::straightLineDistributeThread() {
}

void straightLineDistributeThread::setInitialDistributer(const bool _is_initial_requester) {
    this->is_initial_requester = _is_initial_requester;
}
