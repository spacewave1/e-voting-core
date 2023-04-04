//
// Created by wld on 02.04.23.
//

#include "didDistributionService.h"
#include "../evoting/model/electionBuilder.h"


void didDistributionService::getDistributionParams(const inMemoryStorage& storage, const did id, std::string &address_up,
                                                   std::string &address_down) {
    const didDocument &document = storage.getDocument(id);

    if (!document.controller.empty()) {
        address_up = storage.fetchResource(document.controller);
    }

    did id_down = storage.getDIDChainDown()[id];
    if (!id_down.empty()) {
        address_down = storage.fetchResource(id_down);
    }
}

unsigned long didDistributionService::calculatePosition(const inMemoryStorage &storage, const did id) {
    std::map<std::string, std::string> reversed_connection_table;
    auto did_chain_down = storage.getDIDChainDown();
    auto did_chain_up = storage.getDIDChainUp(id);

    did root_did;

    std::for_each(storage.getDIDChainDown().begin(), storage.getDIDChainDown().end(),[&did_chain_down, &root_did](std::pair<did, did> entry){
        if(!did_chain_down.contains(entry.second)){
            root_did = entry.second;
        }
    });

    size_t pos = getAndIncrement(id, root_did, did_chain_down, 0);

    std::stringstream sstream;
    sstream << "Calculated position for " << id << " is " << std::to_string(pos);
    _logger.log(sstream.str());
    return pos;
}

void didDistributionService::sendDirectionRequestNumberOfHops(abstractSocket &socket, size_t hops) {
    socket.send(std::to_string(hops));
}

std::string
didDistributionService::sendInitialDistributionRequestDirection(abstractSocket &socket_up, abstractSocket &socket_down,
                                                                std::string local_address, std::string address_up,
                                                                std::string address_down, size_t network_size,
                                                                size_t position) {
    if (address_down.empty() && !address_up.empty()) {
        socket_up.setupSocket(local_address, 5049);
        socket_up.connect("tcp", address_up, 5049);
        socket_up.send("up");
        return "up";
    } else if (!address_down.empty() && address_up.empty()) {
        socket_down.setupSocket(local_address, 5049);
        socket_down.connect("tcp", address_down, 5049);
        socket_down.send("down");
        return "down";
    } else if (!address_down.empty() && !address_up.empty()) {
        // Send to the closest extreme
        if (position < network_size / 2) {
            socket_up.setupSocket(local_address, 5049);
            socket_up.connect("tcp", address_up, 5049);
            socket_up.send("up");
            return "up";
        } else {
            socket_down.setupSocket(local_address, 5049);
            socket_down.connect("tcp", address_down, 5049);
            socket_down.send("down");
            return "down";
        }
        // Position check case
        // if initial request in 1 ... n/2 -> receive from down -> forward up -> receive from up -> forward down
        // else if initial request in n/2 ... n -> receive from up -> forward down -> receive from down -> forward up
    }
}

std::string
didDistributionService::sendForwardistributionRequestDirection(abstractSocket &socket_up, abstractSocket &socket_down, std::string direction) {
    if(direction.find("up") != -1) {
        socket_up.send("up");
        return "up";
    } else if(direction.find("down") != -1) {
        socket_down.send("down");
        return "down";
    }
    // Position check case
    // if initial request in 1 ... n/2 -> receive from down -> forward up -> receive from up -> forward down
    // else if initial request in n/2 ... n -> receive from up -> forward down -> receive from down -> forward up
    return "";
}

void didDistributionService::sendSuccessResponse(abstractSocket &socket) {
    socket.send("accept");
}

election didDistributionService::receiveElection(abstractSocket &socket) {
    _logger.log("receive from up", "localhost", "distribute");
    //_logger.log("subscribe to " + address + ":" + std::to_string(subscribe_port), "localhost", "distribute");

    const std::string &election_id_string = socket.recv().payload;
    int election_id = std::stoi(election_id_string);
    _logger.log("received election id: " + std::to_string(election_id), "localhost", "distribute");

    const std::string &sequence_id_string = socket.recv().payload;
    int sequence_id = std::stoi(sequence_id_string);
    _logger.log("received sequence id: " + std::to_string(sequence_id), "localhost", "distribute");

    std::string setup_date_string = socket.recv().payload;
    std::for_each(setup_date_string.begin(), setup_date_string.end(), [](char c) {
        std::cout << c << " ";
    });
    std::cout << std::endl;
    std::cout << setup_date_string << std::endl;
    time_t setup_date = (unsigned int) std::stoul(setup_date_string);
    _logger.log("received setup date: " + std::to_string(setup_date), "localhost", "distribute");

    std::string json = socket.recv().payload;
    nlohmann::json electionOptionsJson = nlohmann::json::parse(json);
    _logger.log("received " + electionOptionsJson.dump(), "localhost", "distribute");

    std::string jsonVotes = socket.recv().payload;
    nlohmann::json election_votes_json = nlohmann::json::parse(jsonVotes);
    _logger.log("received " + election_votes_json.dump(), "localhost", "distribute");

    std::string jsonGroups = socket.recv().payload;
    nlohmann::json election_json_groups = nlohmann::json::parse(jsonGroups);
    _logger.log("received " + election_json_groups.dump(), "localhost", "distribute");

    std::string jsonResult = socket.recv().payload;
    nlohmann::json election_json_result = nlohmann::json::parse(jsonResult);
    _logger.log("received " + election_json_result.dump(), "localhost", "distribute");


    election receivedElection = election::create(election_id)
            .withVoteOptionsFromJson(electionOptionsJson)
            .withParticipantsVotesFromJson(election_votes_json)
            .withSetupDate(setup_date)
            .withParticipantsFromParticipantVotesKeySet()
            .withSequenceNumber(sequence_id)
            .withPreparedForDistribution(true)
            .withElectionGroupsFromJson(election_json_groups)
            .withElectionResultFromJson(election_json_result);

    return receivedElection;
}

std::string didDistributionService::invertDirection(std::string input_direction) {
    if (input_direction == "up") {
        return "down";
    } else {
        return "up";
    }
}

void didDistributionService::updateElectionBox(election election_update, std::vector<election> election_box) {
    auto p_function = [election_update](const election &_election) {
        return _election.getId() == election_update.getId() && _election.getSetupDate() == election_update.getSetupDate();
    };
    auto result = std::find_if(election_box.begin(), election_box.end(), p_function);
    if (result != election_box.end()) {
        result->setJsonOptionsToOptions(election_update.getElectionOptionsJson());
        result->setSequenceNumber(election_update.getSequenceNumber());
        result->setSetupDate(election_update.getSetupDate());
        result->setJsonVotesToVotes(election_update.getVotesAsJson());
        result->setJsonElectionGroupToGroups(election_update.getEvaluationGroupsAsJson());
        result->setJsonResultToResult(election_update.getElectionResultAsJson());
    } else {
        election new_election = election::create(election_update.getId())
                .withVoteOptionsFromJson(election_update.getElectionOptionsJson())
                .withParticipantsVotesFromJson(election_update.participantVotesAsJson())
                .withSetupDate(election_update.getSetupDate())
                .withParticipantsFromParticipantVotesKeySet()
                .withSequenceNumber(election_update.getSequenceNumber())
                .withPreparedForDistribution(true)
                .withElectionGroupsFromJson(election_update.getEvaluationGroupsAsJson())
                .withElectionResultFromJson(election_update.getElectionResultAsJson());

        election_box.push_back(new_election);
    }
}

size_t didDistributionService::getAndIncrement(const did own_id, did current_id, std::map<did, did> did_chain_down,
                                               size_t position) {
    std::stringstream sstream;
    sstream << "current id: " << current_id << ",\n self_address: " << own_id;
    _logger.log(sstream.str());

    if (current_id == own_id || !did_chain_down.contains(current_id)) {
        return position;
    } else {
        position++;
        return getAndIncrement(own_id, did_chain_down[current_id], did_chain_down, position);
    }
}

void didDistributionService::sendElection(abstractSocket &socket, election &election_update, int port) {

    _logger.log("send on port: " + std::to_string(port));

    socket.send(std::to_string(election_update.getId()));
    _logger.log("send: " + std::to_string(election_update.getId()));
    socket.send(std::to_string(election_update.getSequenceNumber() + 1));
    _logger.log("send: " + std::to_string(election_update.getSequenceNumber() + 1));
    socket.send(std::to_string(election_update.getSetupDate()));
    _logger.log("send: " + election_update.getSetupDateAsString());
    socket.send(election_update.getElectionOptionsJson().dump());
    _logger.log("send: " + std::string(election_update.getElectionOptionsJson().dump()));
    socket.send(election_update.participantVotesAsJson().dump(4, ' ', true));
    _logger.log("send: " + std::string(election_update.participantVotesAsJson().dump(4, ' ', true)));
    socket.send(election_update.getEvaluationGroupsAsJson().dump());
    _logger.log("send: " + std::string(election_update.getEvaluationGroupsAsJson().dump()));
    socket.send(election_update.getElectionResultAsJson().dump());
    _logger.log("send: " + std::string(election_update.getElectionResultAsJson().dump()));

    _logger.log("finished broadcasting");
}

void didDistributionService::sendInitialPortsSetupRequest(abstractSocket &socket_up, abstractSocket &socket_down,
                                                          std::string local_address, size_t position,
                                                          std::string address_up, std::string address_down) {
    int subscribe_port = 5051;
    int publish_port = 5050;

    nlohmann::json sendJson;
    sendJson["originPosition"] = position;
    sendJson["origin_publish_port"] = publish_port;

    _logger.log("up not empty: " + std::to_string(!address_up.empty()), local_address);
    _logger.log("down not empty: " + std::to_string(!address_down.empty()), local_address);

    if (!address_up.empty()) {
        socket_up.setupSocket(local_address, 5049);
        socket_up.connect("tcp", address_up, 5049);
        socket_up.send(sendJson.dump());
    }

    if (!address_down.empty()) {
        socket_down.setupSocket(local_address, 5049);
        socket_down.connect("tcp", address_down, 5049);
        socket_down.send(sendJson.dump());
    }

    _logger.log("Has setup from active Distribution");

}

void didDistributionService::sendDirectionRequest(abstractSocket &socket, nlohmann::json data, size_t position,
                                                  std::string address_down, std::string address_up) {
    size_t originPosition = std::stoi(data["originPosition"].dump());
    int subscribe_port = std::stoi(data["origin_publish_port"].dump());

    nlohmann::json sendJson;
    sendJson["originPosition"] = position;
    int publish_port;

    if (originPosition < position) {
        _logger.log("Set subscribe socket for upwards", "localhost", "distribute");

        if (!address_down.empty()) {
            _logger.log("Forward port number to subscribers", "localhost", "distribute");
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;

            sendJson["origin_publish_port"] = publish_port;
            socket.send(sendJson.dump());
        } else {
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;
        }
    } else if (originPosition > position) {
        _logger.log("Set subscribe socket for downwards", "localhost", "distribute");

        if (!address_up.empty()) {
            _logger.log("Forward port number to subscribers", "localhost", "distribute");
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;

            sendJson["origin_publish_port"] = publish_port;
            socket.send(sendJson.dump());
        } else {
            publish_port = (subscribe_port - 5050 + 1) % 2 + 5050;
        }
    }
}
