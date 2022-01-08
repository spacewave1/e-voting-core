//
// Created by wnabo on 19.11.2021.
//

#include "straightLineDistributeThread.h"
#include "zmqSocketAdapter.h"
#include "peer.h"

void straightLineDistributeThread::InternalThreadEntry() {
    log("localhost", "Execute thread");
    if (is_initial_requester) {
        sendInitialRequest();
    } else {
        receiveInitialSetupRequest();
    }

    if (this->address_down.empty() && !this->address_up.empty()) {
        if (is_initial_requester) {
            sendDistributionRequest("up");
            forwardData("up");
        } else {
            //rec req
            std::string directionFrom = receiveDistributionRequest();
            receiveData(invertDirection(directionFrom));
        }
    } else if (!this->address_down.empty() && this->address_up.empty()) {
        if (is_initial_requester) {
            sendDistributionRequest("down");
            forwardData("down");
        } else {
            //rec req
            std::string directionFrom = receiveDistributionRequest();
            receiveData(invertDirection(directionFrom));
        }
    } else if (!this->address_down.empty() && !this->address_up.empty()) {
        if (is_initial_requester) {
            // Send to the closest extreme
            if (node_position < network_size / 2) {
                sendDistributionRequest("up");
                forwardData("up");
                receiveDistributionRequest();
                receiveData("up");
                sendDistributionRequest("down");
                forwardData("down");
            } else {
                sendDistributionRequest("down");
                forwardData("down");
                receiveDistributionRequest();
                receiveData("down");
                sendDistributionRequest("up");
                forwardData("up");
            }
        } else {
            std::string directionFrom = receiveDistributionRequest();
            receiveData(invertDirection(directionFrom));
            sendDistributionRequest(directionFrom);
            forwardData(directionFrom);
        }
        // Position check case
        // if initial request in 1 ... n/2 -> receive from down -> forward up -> receive from up -> forward down
        // else if initial request in n/2 ... n -> receive from up -> forward down -> receive from down -> forward up
    }
    log("localhost", "finished distribution process for this host");
}

//If Distribute -> close current receive and
//Else if Receive and lowest


void straightLineDistributeThread::sendInitialRequest() {
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t request_socket(*context, zmq::socket_type::req);

    nlohmann::json sendJson;
    sendJson["originPosition"] = node_position;

    // TODO: Fall nicht abgedeckt bei der sich der initial request in der Mitte befindet

    if (!address_up.empty()) {
        request_socket.connect("tcp://" + address_up + ":5049");
        sendJson["origin_publish_port"] = 5050;
        request_socket.send(zmq::buffer(sendJson.dump()));
        publish_port = 5050;

        zmq::message_t reply;
        request_socket.recv(reply);
        log(reply.gets("Peer-Address"), reply.to_string());
        request_socket.disconnect("tcp://" + address_up + ":5049");
    }

    if (!address_down.empty()) {
        request_socket = zmq::socket_t(*context, zmq::socket_type::req);
        request_socket.connect("tcp://" + address_down + ":5049");
        sendJson["origin_publish_port"] = 5051;
        publish_port = 5051;
        request_socket.send(zmq::buffer(sendJson.dump()));

        zmq::message_t reply;
        request_socket.recv(reply);
        log(reply.gets("Peer-Address"), reply.to_string());
        request_socket.disconnect("tcp://" + address_down + ":5049");
    }

    log("localhost", "Has setup from active Distribution");
}

void straightLineDistributeThread::receiveInitialSetupRequest() {
    auto *context = (zmq::context_t *) arg;
    zmq::message_t request;
    nlohmann::json receiveJson;
    {
        zmq::socket_t receive_request_socket(*context, zmq::socket_type::rep);

        receive_request_socket.bind("tcp://*:5049");

        receive_request_socket.recv(request);

        receiveJson = nlohmann::json::parse(request.to_string());
        log(request.gets("Peer-Address"), receiveJson.dump());

        receive_request_socket.send(zmq::buffer("accept"));
        receive_request_socket.close();

    }
    setupDistribution((zmq::message_t &) request, receiveJson);

    log("localhost", "Has setup from passive distribution");
}

void straightLineDistributeThread::sendDistributionRequest(std::string direction) {
    log("localhost", "send distribution request");
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t send_request_socket(*context, zmq::socket_type::req);

    if (direction == "up") {
        send_request_socket.connect("tcp://" + address_up + ":5049");
    } else {
        send_request_socket.connect("tcp://" + address_down + ":5049");
    }

    zmq::message_t reply;
    send_request_socket.send(zmq::buffer(direction));
    send_request_socket.recv(reply);
    //nlohmann::json receiveJson = nlohmann::json::parse(request.to_string());
    log(reply.gets("Peer-Address"), reply.to_string());

    send_request_socket.close();
}

void straightLineDistributeThread::receiveData(std::string direction) {
    log("localhost", "Receive data");
    if (direction == "down") {
        election_snapshot_to_send = receiveFromDown();
    } else {
        election_snapshot_to_send = receiveFromUp();
    }
}

std::string straightLineDistributeThread::receiveDistributionRequest() {
    log("localhost", "Wait for distribution request");
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t receive_request_socket(*context, zmq::socket_type::rep);

    bool isSocketBind = false;

    while (!isSocketBind) {
        try {
            receive_request_socket.bind("tcp://*:5049");
            isSocketBind = true;
        }
        catch (zmq::error_t &e) {
            log("localhost", ("couldn't bind to socket: " + std::string(e.what())));
            zmq_sleep(1);
        }
    }

    zmq::message_t request;
    receive_request_socket.recv(request);
    receive_request_socket.send(zmq::buffer("accepted"));
    receive_request_socket.close();

    log(std::string(request.gets("Peer-Address")), request.to_string());
    return request.to_string();
}

void straightLineDistributeThread::setupDistribution(zmq::message_t &request, nlohmann::json receiveJson) {
    auto *context = (zmq::context_t *) arg;
    size_t originPosition = std::stoi(receiveJson["originPosition"].dump());
    subscribe_port = std::stoi(receiveJson["origin_publish_port"].dump());

    nlohmann::json sendJson;
    sendJson["originPosition"] = node_position;

    if (originPosition < node_position) {
        log("localhost", "Set subscribe socket for upwards");

        if (!address_down.empty()) {
            log("localhost", "Forward port number to subscribers");
            zmq::socket_t forward_request(*context, zmq::socket_type::req);
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;

            sendJson["origin_publish_port"] = publish_port;
            forward_request.connect("tcp://" + address_down + ":5049");
            forward_request.send(zmq::buffer(sendJson.dump()));

            zmq::message_t reply;
            forward_request.recv(reply);
            log(address_down, reply.to_string());
            forward_request.disconnect("tcp://" + address_down + ":5049");
            forward_request.close();


            zmq::socket_t socket = zmq::socket_t(*context, zmq::socket_type::sub);
            socket.set(zmq::sockopt::subscribe, "");
            //((zmqSocketAdapter &) subscribe_socket).setSocket(socket);
        }
    } else if (originPosition > node_position) {
        log("localhost", "Set subscribe socket for downwards");

        if (!address_up.empty()) {
            log("localhost", "Forward port number to subscribers");
            zmq::socket_t forward_request(*context, zmq::socket_type::req);
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;


            sendJson["origin_publish_port"] = publish_port;
            forward_request.connect("tcp://" + address_up + ":5049");
            forward_request.send(zmq::buffer(sendJson.dump()));

            zmq::message_t reply;
            forward_request.recv(reply);
            log(address_up, reply.to_string());
            forward_request.disconnect("tcp://" + address_up + ":5049");
            forward_request.close();

        }
    }
}

void straightLineDistributeThread::forwardUp() {
    log("localhost", "forward up");


    publish_socket.bind("*", publish_port);

    publish_socket.send(std::to_string(election_snapshot_to_send.getPollId()));
    publish_socket.send(std::to_string(election_snapshot_to_send.getSequenceNumber() + 1));
    publish_socket.send(election_snapshot_to_send.getElectionOptionsJson());

    log("localhost", "finished broadcasting");
    //publish_socket.send(zmq::message_t(electionSnapshot.getVotes().));
}

election straightLineDistributeThread::receiveFromUp() {
    log("localhost", "receive from up");

    subscribe_socket.connect(address_up, subscribe_port);

    log("localhost", "subscribe to " + address_up + ":" + std::to_string(subscribe_port));

    zmq::message_t message_election_id;
    zmq::message_t message_election_sequence;
    zmq::message_t message_election_json;


    const std::string &election_id_string = subscribe_socket.recv();
    int election_id = std::stoi(election_id_string);

    const std::string &sequence_id_string = subscribe_socket.recv();
    int sequence_id = std::stoi(sequence_id_string);

    const std::string &json = subscribe_socket.recv();
    nlohmann::json electionOptionsJson = json;

    //publish_socket.send(zmq::message_t(electionSnapshot.getVotes()));

    log("localhost", "received " + std::to_string(election_id));
    log("localhost", "received " + std::to_string(sequence_id));
    log("localhost", "received " + electionOptionsJson.dump());

    election receivedElection;

    receivedElection.setPollId(election_id);
    receivedElection.setSequenceNumber(sequence_id);
    receivedElection.setJson(electionOptionsJson);

    return receivedElection;
}

election straightLineDistributeThread::receiveFromDown() {
    log("localhost", "receive from down");

    subscribe_socket.connect(address_down, subscribe_port);

    log("localhost", "subscribed to " + address_down + ":" + std::to_string(subscribe_port));

    const std::string &election_id_string = subscribe_socket.recv();
    int election_id = std::stoi(election_id_string);

    const std::string &sequence_id_string = subscribe_socket.recv();
    int sequence_id = std::stoi(sequence_id_string);

    const std::string &json = subscribe_socket.recv();
    nlohmann::json electionOptionsJson = json;

    election receivedElection;

    receivedElection.setPollId(election_id);
    receivedElection.setSequenceNumber(sequence_id);
    receivedElection.setJson(electionOptionsJson);

    log("localhost", "received " + std::to_string(election_id));
    log("localhost", "received " + std::to_string(sequence_id));
    log("localhost", "received " + electionOptionsJson.dump());

    return receivedElection;
    // Set variables
}

void straightLineDistributeThread::forwardDown() {
    log("localhost", "forward down");
    publish_socket.bind("*", publish_port);

    zmq_sleep(1);

    log("localhost", "send on port: " + std::to_string(publish_port));

    publish_socket.send(std::to_string(election_snapshot_to_send.getPollId()));
    publish_socket.send(std::to_string(election_snapshot_to_send.getSequenceNumber() + 1));
    publish_socket.send(election_snapshot_to_send.getElectionOptionsJson());

    log("localhost", "finished broadcasting");

    log("localhost", "send: " + std::to_string(election_snapshot_to_send.getPollId()));
    log("localhost", "send: " + std::to_string(election_snapshot_to_send.getSequenceNumber() + 1));
    log("localhost", "send: " + election_snapshot_to_send.getElectionOptionsJson());
    //publish_socket.send(zmq::message_t(electionSnapshot.getVotes().));
}

void straightLineDistributeThread::setParams(void *arg, std::string address_up, std::string address_down,
                                             size_t node_position, size_t network_size,
                                             const election &election_snapshot) {
    this->arg = arg;
    this->address_down = address_down;
    this->address_up = address_up;
    this->election_snapshot_to_send = election_snapshot;
    this->node_position = node_position;
    auto *context = (zmq::context_t *) arg;
}

election straightLineDistributeThread::getElectionSnapshot() const {
    return election_snapshot_to_send;
}

void straightLineDistributeThread::setInitialDistributer(bool _is_initial_requester) {
    this->is_initial_requester = _is_initial_requester;
}

// TODO: Migrate to enums
std::string straightLineDistributeThread::invertDirection(std::string direction) {
    if (direction == "up") {
        return "down";
    } else {
        return "up";
    }
}

void straightLineDistributeThread::forwardData(std::string direction) {
    if (direction == "up") {
        forwardUp();
    } else {
        forwardDown();
    }
}

void straightLineDistributeThread::log(std::string address, std::string content) {
    std::cout << "[" << address << "]" << (address.length() < 11 ? "\t\t" : "\t") << content << std::endl;
}

void straightLineDistributeThread::setSubscribeSocket(abstractSocket &socket) {
    this->subscribe_socket = std::move(socket);
}

void straightLineDistributeThread::setPublishSocket(abstractSocket &socket) {
    this->publish_socket = std::move(socket);
}

straightLineDistributeThread::straightLineDistributeThread(abstractSocket &publish_socket,
                                                           abstractSocket &subscribe_socket)
        : publish_socket(publish_socket), subscribe_socket(subscribe_socket) {}

void straightLineDistributeThread::setPublishPort(size_t port_number) {
    this->publish_port = port_number;
}

void straightLineDistributeThread::setSubscribePort(size_t port_number){
    this->subscribe_port = port_number;
}
