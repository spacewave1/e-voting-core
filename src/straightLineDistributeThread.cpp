//
// Created by wnabo on 19.11.2021.
//

#include "straightLineDistributeThread.h"
#include "peer.h"
#include "zmqSocketAdapter.h"

void straightLineDistributeThread::InternalThreadEntry() {
    _logger.log("Start thread");
    is_interrupted = false;
    is_running = true;

    while (!is_interrupted && is_running) {
        resetHops();
        _logger.log("Execute thread", "localhost", "distribute");
        if (is_initial_requester) {
            sendInitialRequest();
        } else {
            receiveInitialSetupRequest();
        }

        if (is_interrupted) {
            _logger.log("interrupted thread", "localhost", "distribute");
            break;
        } else {
            if (this->address_down.empty() && !this->address_up.empty()) {
                if (is_initial_requester) {
                    sendDistributionRequest("up");
                    forward();
                } else {
                    //rec req
                    std::string directionFrom = receiveDistributionRequest();
                    receiveData(invertDirection(directionFrom));

                    if (current_number_of_hops < network_size - 1) {
                        sendDistributionRequest("up");
                        forward();
                    }

                    // if is not last requester continue to distribute
                }
            } else if (!this->address_down.empty() && this->address_up.empty()) {
                if (is_initial_requester) {
                    sendDistributionRequest("down");
                    forward();
                } else {
                    //rec req
                    std::string directionFrom = receiveDistributionRequest();
                    receiveData(invertDirection(directionFrom));

                    if (current_number_of_hops < network_size - 1) {
                        sendDistributionRequest("down");
                        forward();
                    }
                }
            } else if (!this->address_down.empty() && !this->address_up.empty()) {
                if (is_initial_requester) {
                    // Send to the closest extreme
                    if (node_position < network_size / 2) {
                        sendDistributionRequest("up");
                        forward();
                        receiveDistributionRequest();
                        receiveData("up");
                        sendDistributionRequest("down");
                        forward();
                    } else {
                        sendDistributionRequest("down");
                        forward();
                        receiveDistributionRequest();
                        receiveData("down");
                        sendDistributionRequest("up");
                        forward();
                    }
                } else {
                    std::string directionFrom = receiveDistributionRequest();
                    receiveData(invertDirection(directionFrom));
                    sendDistributionRequest(directionFrom);
                    forward();
                }
                // Position check case
                // if initial request in 1 ... n/2 -> receive from down -> forward up -> receive from up -> forward down
                // else if initial request in n/2 ... n -> receive from up -> forward down -> receive from down -> forward up
            }
            _logger.log("finished distribution process for this host");
            if (!is_initial_requester) {
                updateElectionBox();
            }
            if (is_initial_requester) {
                is_running = false;
            }
        }
    }
    _logger.log("Thread now stops running", "localhost", "distribute");
}

//If Distribute -> close current receive and
//Else if Receive and lowest


void straightLineDistributeThread::sendInitialRequest() {
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t request_socket(*context, zmq::socket_type::req);

    subscribe_port = 5051;
    publish_port = 5050;

    nlohmann::json sendJson;
    sendJson["originPosition"] = node_position;
    sendJson["origin_publish_port"] = publish_port;

    if (!address_up.empty()) {
        request_socket.connect("tcp://" + address_up + ":5049");
        request_socket.send(zmq::buffer(sendJson.dump()));

        zmq::message_t reply;
        request_socket.recv(reply);

        _logger.log(reply.to_string(), reply.gets("Peer-Address"), "distribute");
        request_socket.disconnect("tcp://" + address_up + ":5049");
    }

    if (!address_down.empty()) {
        request_socket = zmq::socket_t(*context, zmq::socket_type::req);
        request_socket.connect("tcp://" + address_down + ":5049");
        request_socket.send(zmq::buffer(sendJson.dump()));

        zmq::message_t reply;
        request_socket.recv(reply);

        _logger.log(reply.to_string(), reply.gets("Peer-Address"), "distribute");
        request_socket.disconnect("tcp://" + address_down + ":5049");
    }

    _logger.log("Has setup from active Distribution");
}

void straightLineDistributeThread::receiveInitialSetupRequest() {
    auto *context = (zmq::context_t *) arg;
    zmq::message_t request;
    nlohmann::json receiveJson;

    zmq::socket_t receive_request_socket(*context, zmq::socket_type::rep);

    receive_request_socket.bind("tcp://*:5049");

    while (!is_interrupted) {
        receive_request_socket.recv(request, zmq::recv_flags::dontwait);

        if (!request.empty()) {
            receiveJson = nlohmann::json::parse(request.to_string());
            _logger.log(receiveJson.dump(), request.gets("Peer-Address"), "distribute");

            receive_request_socket.send(zmq::buffer("accept"));
            receive_request_socket.close();

            setupDistribution((zmq::message_t &) request, receiveJson);

            _logger.log("Has setup from passive distribution", "distribute");
            break;
        }
    }
}

void straightLineDistributeThread::sendDistributionRequest(std::string direction) {
    _logger.log("send distribution request towards " + direction);
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
    _logger.log(reply.to_string(), reply.gets("Peer-Address"), "distribute");
    send_request_socket.send(zmq::buffer(std::to_string(current_number_of_hops)));

    send_request_socket.recv(reply);

    //nlohmann::json receiveJson = nlohmann::json::parse(request.to_string());
    _logger.log(reply.to_string(), reply.gets("Peer-Address"), "distribute");

    send_request_socket.close();
}

std::string straightLineDistributeThread::receiveDistributionRequest() {
    _logger.log("Wait for distribution request", "localhost", "distribute");
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t receive_request_socket(*context, zmq::socket_type::rep);

    bool isSocketBind = false;

    while (!isSocketBind) {
        try {
            receive_request_socket.bind("tcp://*:5049");
            isSocketBind = true;
        }
        catch (zmq::error_t &e) {
            _logger.log(("couldn't bind to socket: " + std::string(e.what())), "localhost", "distribute");
            zmq_sleep(1);
        }
    }

    zmq::message_t request_direction;
    zmq::message_t message_number_of_hops;

    receive_request_socket.recv(request_direction);
    receive_request_socket.send(zmq::buffer("accepted"));
    receive_request_socket.recv(message_number_of_hops);

    current_number_of_hops = std::stoi(message_number_of_hops.to_string()) + 1;

    receive_request_socket.send(zmq::buffer("accepted"));
    receive_request_socket.close();

    _logger.log(request_direction.to_string(), std::string(request_direction.gets("Peer-Address")), "distribute");
    _logger.log("received hop number = " + message_number_of_hops.to_string(),
                std::string(request_direction.gets("Peer-Address")), "distribute");
    _logger.log("current hop number = " + std::to_string(current_number_of_hops), "distribute");
    return request_direction.to_string();
}

void straightLineDistributeThread::receiveData(std::string direction) {
    _logger.log("Receive data", "localhost", "distribute");
    if (direction == "down") {
        election_snapshot_to_send = receiveFrom(address_down);
    } else {
        election_snapshot_to_send = receiveFrom(address_up);
    }
}

void straightLineDistributeThread::setupDistribution(zmq::message_t &request, nlohmann::json receiveJson) {
    auto *context = (zmq::context_t *) arg;
    size_t originPosition = std::stoi(receiveJson["originPosition"].dump());
    subscribe_port = std::stoi(receiveJson["origin_publish_port"].dump());

    nlohmann::json sendJson;
    sendJson["originPosition"] = node_position;

    if (originPosition < node_position) {
        _logger.log("Set subscribe socket for upwards", "localhost", "distribute");

        if (!address_down.empty()) {
            _logger.log("Forward port number to subscribers", "localhost", "distribute");
            zmq::socket_t forward_request(*context, zmq::socket_type::req);
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;

            sendJson["origin_publish_port"] = publish_port;
            forward_request.connect("tcp://" + address_down + ":5049");
            forward_request.send(zmq::buffer(sendJson.dump()));

            zmq::message_t reply;
            forward_request.recv(reply);
            _logger.log(reply.to_string(), address_down, "distribute");
            forward_request.disconnect("tcp://" + address_down + ":5049");
            forward_request.close();


            zmq::socket_t socket = zmq::socket_t(*context, zmq::socket_type::sub);
            socket.set(zmq::sockopt::subscribe, "");
        } else {
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;
        }
    } else if (originPosition > node_position) {
        _logger.log("Set subscribe socket for downwards", "localhost", "distribute");

        if (!address_up.empty()) {
            _logger.log("Forward port number to subscribers", "localhost", "distribute");
            zmq::socket_t forward_request(*context, zmq::socket_type::req);
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;


            sendJson["origin_publish_port"] = publish_port;
            forward_request.connect("tcp://" + address_up + ":5049");
            forward_request.send(zmq::buffer(sendJson.dump()));

            zmq::message_t reply;
            forward_request.recv(reply);
            _logger.log(reply.to_string(), address_up, "distribute");
            forward_request.disconnect("tcp://" + address_up + ":5049");
            forward_request.close();
        } else {
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;
        }
    }
}

election straightLineDistributeThread::receiveFrom(std::string address) {
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t sub_socket = zmq::socket_t(*context, zmq::socket_type::sub);
    sub_socket.set(zmq::sockopt::subscribe, "");

    zmqSocketAdapter sub_socket_adapter(sub_socket);
    _logger.log("receive from up", "localhost", "distribute");

    sub_socket_adapter.connect("tcp", address, subscribe_port);

    _logger.log("subscribe to " + address + ":" + std::to_string(subscribe_port), "localhost", "distribute");

    const std::string &election_id_string = sub_socket_adapter.recv();
    int election_id = std::stoi(election_id_string);
    _logger.log("received " + std::to_string(election_id), "localhost", "distribute");


    const std::string &sequence_id_string = sub_socket_adapter.recv();
    int sequence_id = std::stoi(sequence_id_string);
    _logger.log("received " + std::to_string(sequence_id), "localhost", "distribute");

    const std::string &setup_date_string = sub_socket_adapter.recv();
    time_t setup_date = (unsigned int) std::stoul(setup_date_string);
    _logger.log("received " + std::to_string(setup_date), "localhost", "distribute");

    const std::string &json = sub_socket_adapter.recv();
    nlohmann::json electionOptionsJson = nlohmann::json::parse(json);
    _logger.log("received " + electionOptionsJson.dump(), "localhost", "distribute");

    const std::string &jsonVotes = sub_socket_adapter.recv();
    nlohmann::json election_votes_json = nlohmann::json::parse(jsonVotes);
    _logger.log("received " + election_votes_json.dump(), "localhost", "distribute");

    election receivedElection;

    receivedElection.setPollId(election_id);
    receivedElection.setSequenceNumber(sequence_id);
    receivedElection.setSetupDate(setup_date);
    receivedElection.setJsonOptionsToOptions(electionOptionsJson);
    receivedElection.setJsonVotesToVotes(election_votes_json);

    sub_socket_adapter.disconnect("tcp", address, subscribe_port);

    return receivedElection;
}

void straightLineDistributeThread::forward() {
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t pub_socket = zmq::socket_t(*context, zmq::socket_type::pub);

    zmqSocketAdapter pub_socket_adapter(pub_socket);

    // TODO: Solve null reference
    pub_socket_adapter.bind("tcp", "*", publish_port);

    zmq_sleep(2);

    _logger.log("send on port: " + std::to_string(publish_port));

    pub_socket_adapter.send(std::to_string(election_snapshot_to_send.getPollId()));
    _logger.log("send: " + std::to_string(election_snapshot_to_send.getPollId()));
    pub_socket_adapter.send(std::to_string(election_snapshot_to_send.getSequenceNumber() + 1));
    _logger.log("send: " + std::to_string(election_snapshot_to_send.getSequenceNumber() + 1));
    pub_socket_adapter.send(std::to_string(election_snapshot_to_send.getSetupDate()));
    _logger.log("send: " + std::string(election_snapshot_to_send.getElectionOptionsJson().dump()));
    pub_socket_adapter.send(election_snapshot_to_send.getElectionOptionsJson().dump());
    _logger.log("send: " + std::string(election_snapshot_to_send.getVotesAsJson().dump()));
    pub_socket_adapter.send(election_snapshot_to_send.participantVotesAsJson().dump());
    _logger.log("send: " + std::string(election_snapshot_to_send.participantVotesAsJson().dump()));

    pub_socket_adapter.close();

    _logger.log("finished broadcasting");
}

void straightLineDistributeThread::cleanUp() {
    //publish_socket.unbind("tcp",);
    //subscribe_socket.unbind();
    //publish_socket.close();
    //subscribe_socket.close();
}

void straightLineDistributeThread::setParams(void *arg, std::string address_up, std::string address_down,
                                             size_t node_position, size_t network_size,
                                             const election &election_snapshot) {
    this->arg = arg;
    this->address_down = address_down;
    this->address_up = address_up;
    this->network_size = network_size;
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

void straightLineDistributeThread::setPublishPort(size_t port_number) {
    this->publish_port = port_number;
}

void straightLineDistributeThread::setSubscribePort(size_t port_number) {
    this->subscribe_port = port_number;
}

void straightLineDistributeThread::updateElectionBox() {
    auto *context = (zmq::context_t *) arg;
    zmq::socket_t inproc_socket = zmq::socket_t(*context, zmq::socket_type::pub);
    zmqSocketAdapter inproc_socket_adapter = zmqSocketAdapter(inproc_socket);
    inproc_socket_adapter.bind("inproc", "update_elections");

    inproc_socket_adapter.send(std::to_string(election_snapshot_to_send.getPollId()));
    inproc_socket_adapter.send(std::to_string(election_snapshot_to_send.getSequenceNumber()));
    inproc_socket_adapter.send(std::to_string(election_snapshot_to_send.getSetupDate()));
    inproc_socket_adapter.send(election_snapshot_to_send.getElectionOptionsJson().dump());
    inproc_socket_adapter.send(election_snapshot_to_send.participantVotesAsJson().dump());

    _logger.log("send: " + std::to_string(election_snapshot_to_send.getPollId()) + " to inproc thread", "localhost","distribute");
    _logger.log("send: " + std::to_string(election_snapshot_to_send.getSequenceNumber()) + " to inproc thread","localhost", "distribute");
    _logger.log("send: " + std::to_string(election_snapshot_to_send.getSetupDate()) + " to inproc thread", "localhost","distribute");
    _logger.log("send: " + std::string(election_snapshot_to_send.getElectionOptionsJson().dump()) + " to inproc thread","localhost", "distribute");
    _logger.log("send: " + std::string(election_snapshot_to_send.getVotesAsJson().dump()) + " to inproc thread","localhost", "distribute");

    inproc_socket_adapter.unbind("inproc", "update_elections");
}

void straightLineDistributeThread::setAddressUp(std::string addressUp) {
    address_up = addressUp;
}

void straightLineDistributeThread::setAddressDown(std::string addressDown) {
    address_down = addressDown;
}

void straightLineDistributeThread::setPosition(size_t position) {
    node_position = position;
}

void straightLineDistributeThread::setNetworkSize(size_t networkSize) {
    network_size = networkSize;
}

void straightLineDistributeThread::setContext(void *p_void) {
    arg = p_void;
}

void straightLineDistributeThread::interruptReceiveRequest() {
    is_interrupted = true;
}

bool straightLineDistributeThread::isRunning() {
    return is_running;
}

void straightLineDistributeThread::setIsRunning(bool b) {
    is_running = b;
}

void straightLineDistributeThread::resetHops() {
    current_number_of_hops = 0;
}

straightLineDistributeThread::straightLineDistributeThread() {

}
